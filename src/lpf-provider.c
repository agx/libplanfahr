/*
 * Copyright (C) 2014 Guido Guenther <agx@sigxcpu.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include "lpf-provider.h"

G_DEFINE_TYPE (LpfProvider, lpf_provider, G_TYPE_OBJECT)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_PROVIDER, LpfProviderPrivate))

/**
 * SECTION:lpf-provider
 * @short_description: Public transport information provider
 * @see_also: #LpfProvider
 *
 * A #LpfProvider represents a provider of public transport
 * information such as timetables or location information
 */
/**
 * LpfProviderGotLocsNotify:
 * @locs: (element-type Lpf.Loc): List of found locations
 * @user_data: userdata
 * @err: (transfer full): #GError
 *
 * Callback invoked after the locations matching the query were
 * received. In case of an error @locs is #NULL.
 */
/**
 * LpfProviderGotTripsNotify:
 * @trips: (element-type Lpf.Loc): List of found trips
 * @user_data: userdata
 * @err: (transfer full): #GError
 *
 * Callback invoked after the trips matching the query were
 * received. In case of an error @trips is #NULL.
 */

enum {
    PROP_0,
    PROP_NAME,
    LAST_PROP
};

typedef struct _LpfProviderPrivate LpfProviderPrivate;
struct _LpfProviderPrivate {
    char *name;
};

GQuark
lpf_provider_error_quark (void)
{
  return g_quark_from_static_string ("lpf-provider-error-quark");
}

/**
 * lpf_provider_name:
 * @LpfProvider: a #LpfProvider
 *
 * Returns: transfer-none: the providers name
 */
const char*
lpf_provider_get_name (LpfProvider *self)
{
    g_return_val_if_fail (LPF_IS_PROVIDER (self), NULL);
    LpfProviderPrivate *priv = GET_PRIVATE (self);

    return priv->name;
}

static void
set_property (GObject *object, guint prop_id,
              const GValue *value, GParamSpec *pspec)
{
    LpfProviderPrivate *priv = GET_PRIVATE (object);

    switch (prop_id) {
    case PROP_NAME:
        /* construct only */
        priv->name = g_value_dup_string (value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}


static void
get_property (GObject *object, guint prop_id,
              GValue *value, GParamSpec *pspec)
{
    LpfProviderPrivate *priv = GET_PRIVATE (object);

    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string (value, priv->name);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}


static void
finalize (GObject *object)
{
    LpfProviderPrivate *priv = GET_PRIVATE (object);

    g_free (priv->name);
}

/* invoked by LprManager to activate the provider */
void
lpf_provider_activate (LpfProvider *self, GObject *obj)
{
    LpfProviderPrivate *priv = GET_PRIVATE (self);
    gchar *cache_dir = NULL;

    g_return_if_fail (LPF_IS_PROVIDER (self));

    cache_dir = g_strdup_printf("%s/%s/%s/", g_get_user_cache_dir (), PACKAGE_NAME, priv->name);
    if (cache_dir)
        g_mkdir_with_parents (cache_dir, 0700);

    LPF_PROVIDER_GET_CLASS (self)->activate (self, obj);

    g_free (cache_dir);
}

/* invoked by LpfManager to activate the provider */
void
lpf_provider_deactivate (LpfProvider *self, GObject *obj)
{
    g_return_if_fail (LPF_IS_PROVIDER (self));

    LPF_PROVIDER_GET_CLASS (self)->deactivate (self, obj);
}

/**
 * lpf_provider_get_locs:
 * @self: a #LpfProvider
 * @match: locations to match
 * @callback: (scope async): #LpfProviderGotLocsNotify to invoke
 *   once locations are available
 * @user_data: (allow-none): User data for the callback
 *
 * Initiate a lookup for locations that match @match. Once complete
 *  @callback is invoked with a #GSList of matched #LpfLocation s.  The
 *  caller is responsible for freeing the locations list via
 *  #lpf_provider_free_locs.
 *
 * Returns: 0 on success, -1 on error
 */
gint
lpf_provider_get_locs (LpfProvider *self, const char* match, LpfProviderGotLocsNotify callback, gpointer user_data)
{
    g_return_val_if_fail (LPF_IS_PROVIDER (self), -1);
    g_return_val_if_fail (match, -1);
    g_return_val_if_fail (callback, -1);

    return LPF_PROVIDER_GET_CLASS (self)->get_locs (self, match, callback, user_data);
}

/**
 * lpf_provider_free_locs:
 * @self: a #LpfProvider
 * @locs: (element-type LpfLoc): A linked list of location
 *
 * Free the location list
 */
void
lpf_provider_free_locs(LpfProvider *self, GSList *locs)
{
    g_slist_free_full (locs, g_object_unref);
}

/**
 * lpf_provider_get_trips:
 * @self: a #LpfProvider
 * @start: start of trip location
 * @end: end of trip location
 * @date: Date and time the trip starts as #GDateTime
 * @flags: flags
 * @callback: (scope async): #LpfProviderGotTripsNotify to invoke
 *   once trips are available
 * @user_data: (allow-none): User data for the callback
 *
 * Initiate a lookup for trips starting at the location @start, ending
 * at @end and starting (or depending on @flags) ending at @date.
 * Once completed @callback is invoked with a #GSList of matched
 * trips. The caller is responsible for freeing the locations list via
 * #lpf_provider_free_trips.
 *
 * Returns: 0 on success, -1 on error
 */

gint
lpf_provider_get_trips (LpfProvider *self, LpfLoc *start, LpfLoc *end, GDateTime *date, guint64 flags, LpfProviderGotLocsNotify callback, gpointer user_data)
{
    g_return_val_if_fail (LPF_IS_PROVIDER (self), -1);
    g_return_val_if_fail (start, -1);
    g_return_val_if_fail (end, -1);
    g_return_val_if_fail (date, -1);
    g_return_val_if_fail (callback, -1);

    return LPF_PROVIDER_GET_CLASS (self)->get_trips (self, start, end, date, flags, callback, user_data);
}

static void
lpf_provider_class_init (LpfProviderClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (LpfProviderPrivate));

    object_class->get_property = get_property;
    object_class->set_property = set_property;
    object_class->finalize = finalize;

    g_object_class_install_property
        (object_class, PROP_NAME,
         g_param_spec_string (LPF_PROVIDER_PROP_NAME,
                              "Name",
                              "Provider Name",
                              NULL,
                              G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}


static void
lpf_provider_init (LpfProvider *self G_GNUC_UNUSED)
{
}

/**
 * lpf_provider_create: (skip)
 *
 * Create the provider
 */
