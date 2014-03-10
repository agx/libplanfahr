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

#ifndef _LPF_PROVIDER
#define _LPF_PROVIDER

#if !defined (__LIBPLANFAHR_H_INSIDE__) && !defined (LIBPLANFAHR_COMPILATION)
# error "Only <libplanfahr.h> can be included directly."
#endif

#include <glib-object.h>
#include <libplanfahr/lpf-loc.h>

G_BEGIN_DECLS

#define LPF_PROVIDER_MAJOR_VERSION 0
#define LPF_PROVIDER_MINOR_VERSION 0

/**
 * LPF_PROVIDER_ERROR:
 *
 * Error domain for  providers
 */
#define LPF_PROVIDER_ERROR g_spawn_error_quark ()

/**
 * LpfProviderError:
 * @LPF_PROVIDER_ERROR_REQUEST_FAILED: a request to fetch data from a remote failed
 * @LPF_PROVIDER_ERROR_PARSE_FAILED:   parsing the reply failed
 *
 * Error codes returned by providers
 */
typedef enum {
    LPF_PROVIDER_ERROR_REQUEST_FAILED,
    LPF_PROVIDER_ERROR_PARSE_FAILED,
} LpfProviderError;

#define LPF_TYPE_PROVIDER (lpf_provider_get_type())

#define LPF_PROVIDER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LPF_TYPE_PROVIDER, LpfProvider))

#define LPF_IS_PROVIDER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LPF_TYPE_PROVIDER))

#define LPF_PROVIDER_GET_INTERFACE(obj) \
  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), LPF_TYPE_PROVIDER, LpfProviderInterface))

#define LPF_PROVIDER_PROP_NAME "name"

typedef void (*LpfProviderGotLocsNotify) (GSList *locs, gpointer user_data, GError *err);
typedef void (*LpfProviderGotTripsNotify) (GSList *trips, gpointer user_data, GError *err);

typedef struct _LpfProvider LpfProvider;

typedef struct {
    GTypeInterface parent;

    void (*activate)   (LpfProvider *self, GObject *obj);
    void (*deactivate) (LpfProvider *self, GObject *obj);

    const gchar* (*get_name) (LpfProvider *self);

    gint (*get_locs)   (LpfProvider *self, const gchar *match, LpfProviderGotLocsNotify callback, gpointer user_data);
    gint (*get_trips)  (LpfProvider *self, LpfLoc *start, LpfLoc *end,  GDateTime *date, guint64 flags, LpfProviderGotLocsNotify callback, gpointer user_data);
} LpfProviderInterface;

GType lpf_provider_get_type (void);

typedef LpfProvider *(*LpfProviderCreateFunc) (void);
LpfProvider *lpf_provider_create (void);

const char* lpf_provider_get_name (LpfProvider *self);
void lpf_provider_activate (LpfProvider *self, GObject *obj);
void lpf_provider_deactivate (LpfProvider *self, GObject *obj);
GQuark lpf_provider_error_quark (void);
gint lpf_provider_get_locs (LpfProvider *self, const gchar* match, LpfProviderGotLocsNotify callback, gpointer user_data);
gint lpf_provider_get_trips  (LpfProvider *self, LpfLoc *start, LpfLoc *end,  GDateTime *date, guint64 flags, LpfProviderGotLocsNotify callback, gpointer user_data);
void lpf_provider_free_locs (LpfProvider *self, GSList *locs);

G_END_DECLS

#endif /* _LPF_PROVIDER */
