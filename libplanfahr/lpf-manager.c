/*
 * lpf-manager.h: manage planfahr providers
 *
 * Copyright (C) 2014 Guido Günther
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#include "lpf-provider.h"
#include "lpf-manager.h"
#include "lpf-priv.h"

#include <glob.h>
#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gmodule.h>

#define PROVIDER_LIBBASE "libplanfahr-provider-"


/**
 * SECTION:lpf-manager
 * @short_description: Provider Manager
 * @see_also: #LpfProvider
 *
 * A #LpfManager handles the different public transport information providers.
 */


G_DEFINE_TYPE (LpfManager, lpf_manager, G_TYPE_OBJECT)
#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_MANAGER, LpfManagerPrivate))

typedef struct _LpfManagerPrivate LpfManagerPrivate;
struct _LpfManagerPrivate {
    GSList *available; /* available providers */
    GSList *active;    /* active providers */
};


GQuark
lpf_manager_error_quark (void)
{
  return g_quark_from_static_string ("lpf-manager-error-quark");
}


static gchar *plugin_path(const char *name)
{
    gchar *dir = NULL;
    gchar *prefix = NULL;
    gchar *ret;

    dir = g_strdup(g_getenv(LPF_PROVIDERS_ENV));
    if (!dir)
        dir = g_strdup(LPF_PROVIDERS_DIR);
    prefix = g_strjoin("/", dir, PROVIDER_LIBBASE, NULL);
    ret = g_strjoin(NULL, prefix, name, ".", G_MODULE_SUFFIX, NULL);

    g_free(dir);
    g_free(prefix);
    return ret;
}

/**
 * lpf_manager_get_available_providers:
 *
 * Returns: (transfer full): List of available providers
 */
GStrv lpf_manager_get_available_providers(void)
{
    gint i, prefix_len;
    glob_t globbuf;
    gchar *pattern = NULL;
    GStrv providers = NULL;
    gchar **tmp;

    if (!g_module_supported ()) {
        g_warning ("GModules are not supported on your platform!");
        return NULL;
    }

    pattern = plugin_path("*");
    prefix_len = 1 + g_strrstr(pattern, "/") + strlen(PROVIDER_LIBBASE) - pattern ;
    LPF_DEBUG("Looking for providers in %s", pattern);
    if (glob(pattern, GLOB_NOSORT, NULL, &globbuf))
        goto out;

    if (!(providers = g_try_malloc (sizeof(gchar) * globbuf.gl_pathc + 1))) {
        goto out;
    }

    for (i = 0; i < globbuf.gl_pathc; i++) {
        tmp = g_strsplit(globbuf.gl_pathv[i] + prefix_len, ".", 2);
        if (!tmp) {
            providers[i] = NULL;
            goto out;
        }
        providers[i] = g_strdup(tmp[0]);
        g_strfreev(tmp);
    }
    providers[globbuf.gl_pathc] = NULL;

 out:
    globfree(&globbuf);
    g_free (pattern);
    return providers;
}

static LpfProvider*
load_provider (const char *path, GError **error)
{
    LpfProvider *provider = NULL;
    GModule *module;
    LpfProviderCreateFunc create_func;
    int *major_provider_version, *minor_provider_version;

    module = g_module_open (path, G_MODULE_BIND_LAZY);
    if (!module) {
        g_set_error (error,
                     LPF_MANAGER_ERROR,
                     LPF_MANAGER_ERROR_ACTIVATION_FAILED,
                     "Could not load provider %s: %s", path, g_module_error ());
        return NULL;
    }

    if (!g_module_symbol (module, "lpf_provider_major_version",(gpointer *) &major_provider_version)) {
        g_set_error (error,
                     LPF_MANAGER_ERROR,
                     LPF_MANAGER_ERROR_ACTIVATION_FAILED,
                     "Could not load provider %s: Missing major version info", path); 
        goto out;
    }

    if (*major_provider_version != LPF_PROVIDER_MAJOR_VERSION) {
        g_set_error (error,
                     LPF_MANAGER_ERROR,
                     LPF_MANAGER_ERROR_ACTIVATION_FAILED,
                     "Could not load provider %s: Provider major version %d, %d is required",
                     path, *major_provider_version, LPF_PROVIDER_MAJOR_VERSION);
        goto out;
    }

    if (!g_module_symbol (module, "lpf_provider_minor_version", (gpointer *) &minor_provider_version)) {
        g_set_error (error,
                     LPF_MANAGER_ERROR,
                     LPF_MANAGER_ERROR_ACTIVATION_FAILED,
                     "Could not load provider %s: Missing minor version info", path);
        goto out;
    }

    if (*minor_provider_version != LPF_PROVIDER_MINOR_VERSION) {
        g_set_error (error,
                     LPF_MANAGER_ERROR,
                     LPF_MANAGER_ERROR_ACTIVATION_FAILED,
                     "Could not load provider %s: Provider minor version %d, %d is required",
                     path, *minor_provider_version, LPF_PROVIDER_MINOR_VERSION);
        goto out;
    }

    if (!g_module_symbol (module, "lpf_provider_create", (gpointer *) &create_func)) {
        g_set_error (error,
                     LPF_MANAGER_ERROR,
                     LPF_MANAGER_ERROR_ACTIVATION_FAILED,
                     "Could not load provider %s: %s", path, g_module_error ());
        goto out;
    }

    provider = (*create_func) ();
    if (provider) {
        g_object_weak_ref (G_OBJECT (provider), (GWeakNotify) g_module_close, module);
        g_message ("Loaded provider %s", lpf_provider_get_name (provider));
    } else {
        g_set_error (error,
                     LPF_MANAGER_ERROR,
                     LPF_MANAGER_ERROR_ACTIVATION_FAILED,
                     "Could not load provider %s: initialization failed", path);
    }
 out:
    if (!provider)
        g_module_close (module);

    return provider;
}

/**
 * lpf_manager_activate_provider:
 * @self: a #LpfManager
 * @name: the name of the provider to activate
 * @error: a #GError for errorreporting or %NULL
 *
 * Activate the given provider
 *
 * Returns: (transfer none): The activated #LpfProvider
 */
LpfProvider *lpf_manager_activate_provider(LpfManager *self,
                                           const gchar *name,
                                           GError **error)
{
    gchar *path;
    LpfProvider *provider;
    LpfManagerPrivate *priv = GET_PRIVATE (self);

    path = plugin_path(name);
    provider = load_provider (path, error);
    if (provider) {
        lpf_provider_activate(provider, G_OBJECT(self));
        priv->active = g_slist_prepend (priv->active, provider);
    }
    g_free(path);
    return provider;
}

LpfManager *lpf_manager_new (void)
{
    return LPF_MANAGER(g_object_new (LPF_TYPE_MANAGER, NULL));
}

static void
deactivate_provider(gpointer provider, gpointer user_data)
{
    LpfManager *manager = LPF_MANAGER (user_data);
    lpf_manager_deactivate_provider (manager, provider);
}

void
lpf_manager_deactivate_provider(LpfManager *self, LpfProvider *provider)
{
    LPF_DEBUG ("Deactivating provider %s", lpf_provider_get_name (provider));
    lpf_provider_deactivate (provider, G_OBJECT(self));
}

static void
lpf_manager_dispose(GObject *object)
{
    LpfManager *self = LPF_MANAGER(object);
    LpfManagerPrivate *priv = GET_PRIVATE (self);
    GObjectClass *parent_class = G_OBJECT_CLASS (lpf_manager_parent_class);

    /* We need to do this before dropping the ref on applet */
    g_slist_foreach (priv->active, deactivate_provider, self);
    g_slist_free (priv->active);

    if (parent_class->dispose != NULL)
        parent_class->dispose (object);
}

static void
lpf_manager_class_init (LpfManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = lpf_manager_dispose;
    g_type_class_add_private (klass, sizeof (LpfManagerPrivate));
}

static void
lpf_manager_init (LpfManager *self)
{
    LpfManagerPrivate *priv = GET_PRIVATE (self);
    priv->active = NULL;
    priv->available = NULL;
}
