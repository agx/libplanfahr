/*
 * lpf-provider-test-test.c: Test provider
 *
 * Copyright (C) 2014 Guido Günther
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
 *
 * Author: Guido Günther <agx@sigxcpu.org>
 */

#include <config.h>

#include <gmodule.h>

#include <stdlib.h>
#include <string.h>

#include "test-test.h"
#include "lpf-loc.h"
#include "lpf-priv.h"
#include "lpf-provider.h"
#include "lpf-trip.h"
#include "lpf-stop.h"
#include "lpf-trip-part.h"
#include "lpf-provider.h"

static void lpf_provider_test_test_interface_init (LpfProviderInterface *iface);


#define PROVIDER_NAME "test_test"

enum {
    PROP_0,
    PROP_NAME,
    LAST_PROP
};

G_DEFINE_TYPE_WITH_CODE (LpfProviderTestTest, lpf_provider_test_test, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (LPF_TYPE_PROVIDER, lpf_provider_test_test_interface_init));

int lpf_provider_major_version = LPF_PROVIDER_MAJOR_VERSION;
int lpf_provider_minor_version = LPF_PROVIDER_MINOR_VERSION;

/* transfers data between invocation and the passed in callback */
typedef struct _LpfProviderGotItUserData {
    LpfProvider *self;
    gpointer callback;
    gchar *name;
    LpfLoc *start, *end;
    gpointer user_data;
} LpfProviderGotItUserData;

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_PROVIDER_TEST_TEST, LpfProviderTestTestPrivate))

typedef struct _LpfProviderTestTestPrivate LpfProviderTestTestPrivate;

struct _LpfProviderTestTestPrivate {
    GSList *locs;
    GSList *trips;
    gboolean debug;
};


static const char*
lpf_provider_test_test_get_name (LpfProvider *self)
{
    return PROVIDER_NAME;
}


static gboolean
got_locs (gpointer user_data)
{
    LpfProviderTestTestPrivate *priv;
    GSList *loclist, *locs = NULL;
    LpfProviderGotItUserData *locs_data = (LpfProviderGotItUserData*)user_data;
    LpfProviderGotLocsNotify callback;
    LpfProviderTestTest *self;
    gpointer data;
    GError *err = NULL;
    gchar *name;
    LpfLoc *loc;

    g_return_val_if_fail(user_data, FALSE);

    callback = locs_data->callback;
    data = locs_data->user_data;
    self = LPF_PROVIDER_TEST_TEST(locs_data->self);
    name = locs_data->name;
    g_free (locs_data);

    priv = GET_PRIVATE(self);
    for (loclist = priv->locs; loclist; loclist = g_slist_next (loclist)) {
        loc = loclist->data;
        if (strstr (name, lpf_loc_get_name(loc)))
            locs = g_slist_append (locs, loc);
    }

    (*callback)(locs, data, err);
    return FALSE;
}


static gint
lpf_provider_test_test_get_locs (LpfProvider *self,
                                 const char* match,
                                 LpfProviderGetLocsFlags flags,
                                 LpfProviderGotLocsNotify callback,
                                 gpointer user_data)
{
    LpfProviderGotItUserData *locs_data = NULL;
    gint ret = -1;

    locs_data = g_malloc(sizeof(LpfProviderGotItUserData));
    if (!locs_data)
        goto out;

    locs_data->user_data = user_data;
    locs_data->callback = callback;
    locs_data->self = self;
    locs_data->name = g_strdup (match);

    g_timeout_add_seconds(0, got_locs, locs_data);
    ret = 0;
 out:
    if (ret < 0)
        g_free (locs_data);
    return ret;
}


static gboolean
got_trips (gpointer user_data)
{
    GSList *trips = NULL;
    LpfProviderGotItUserData *trips_data = (LpfProviderGotItUserData*)user_data;
    LpfProviderGotTripsNotify callback;
    LpfProviderTestTest *self;
    gpointer data;
    GError *err = NULL;
    LpfProviderTestTestPrivate *priv;
    LpfLoc *start, *end;

    g_return_val_if_fail(user_data, FALSE);

    callback = trips_data->callback;
    data = trips_data->user_data;
    self = LPF_PROVIDER_TEST_TEST(trips_data->self);
    priv = GET_PRIVATE(self);
    start = trips_data->start;
    end = trips_data->end;
    g_free (trips_data);

    if (!g_strcmp0 (lpf_loc_get_name(start), "testloc1") &&
        !g_strcmp0 (lpf_loc_get_name(end), "testloc2")) {
        trips = priv->trips;
    }

    (*callback)(trips, data, err);
    return FALSE;
}


static gint
lpf_provider_test_test_get_trips (LpfProvider *self,
                                  LpfLoc *start,
                                  LpfLoc *end,
                                  GDateTime *date,
                                  LpfProviderGetTripsFlags flags,
                                  LpfProviderGotTripsNotify callback,
                                  gpointer user_data)
{
    LpfProviderGotItUserData *trips_data = NULL;
    LpfProviderTestTestPrivate *priv = GET_PRIVATE(self);
    gint ret = -1;

    g_return_val_if_fail (start, -1);
    g_return_val_if_fail (end, -1);
    g_return_val_if_fail (callback, -1);
    g_return_val_if_fail (date, -1);

    g_return_val_if_fail (!flags, 0);

    g_object_ref (start);
    g_object_ref (end);
    g_date_time_ref (date);

    trips_data = g_try_malloc(sizeof(LpfProviderGotItUserData));
    if (!trips_data)
        goto out;

    trips_data->user_data = user_data;
    trips_data->callback = callback;
    trips_data->self = self;
    trips_data->start = LPF_LOC(g_slist_nth(priv->locs, 0)->data);
    trips_data->end = LPF_LOC(g_slist_nth(priv->locs, 1)->data);

    g_timeout_add_seconds(0, got_trips, trips_data);
    ret = 0;
 out:
    g_object_unref (start);
    g_object_unref (end);
    g_date_time_unref (date);
    if (ret < 0)
        g_free (trips_data);
    return ret;
}


static void
lpf_provider_test_test_set_property (GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
    switch (prop_id) {
    case PROP_NAME:
	g_warn_if_reached ();
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}


static void
lpf_provider_test_test_get_property (GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string (value, PROVIDER_NAME);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}


static void
lpf_provider_test_test_activate (LpfProvider *self, GObject *obj)
{
    LpfProviderTestTestPrivate *priv = GET_PRIVATE(self);
    gchar *debugstr;

    debugstr = getenv ("LPF_DEBUG");
    if (debugstr && strstr (debugstr, "provider"))
        priv->debug = TRUE;
}


static void
lpf_provider_test_test_deactivate (LpfProvider *self, GObject *obj G_GNUC_UNUSED)
{
    LpfProviderTestTestPrivate *priv = GET_PRIVATE(self);

    if (priv->locs)
        g_slist_free_full (priv->locs, g_object_unref);

    if (priv->trips)
        g_slist_free_full (priv->trips, g_object_unref);
}


static void
lpf_provider_test_test_class_init (LpfProviderTestTestClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (LpfProviderTestTestPrivate));

    object_class->get_property = lpf_provider_test_test_get_property;
    object_class->set_property = lpf_provider_test_test_set_property;

    g_object_class_override_property (object_class,
                                      PROP_NAME,
                                      "name");
}


static void
lpf_provider_test_test_interface_init (LpfProviderInterface *iface)
{
    iface->get_name = lpf_provider_test_test_get_name;

    /* abstract base class */
    iface->activate = lpf_provider_test_test_activate;
    iface->deactivate = lpf_provider_test_test_deactivate;

    /* To be implemented by each provider */
    iface->get_locs = lpf_provider_test_test_get_locs;
    iface->get_trips = lpf_provider_test_test_get_trips;
}

static void
lpf_provider_test_test_init (LpfProviderTestTest *self)
{
    LpfLoc *loc;
    LpfTrip *trip;
    LpfTripPart *part;
    LpfProviderTestTestPrivate *priv = GET_PRIVATE(self);
    GSList *parts = NULL;

    /* For now we just add two boring locations */
    loc = LPF_LOC(g_object_new (LPF_TYPE_LOC, "name", "testloc1", "long", 3.14, "lat", 15.0, NULL));
    priv->locs = g_slist_append(priv->locs, loc);
    loc = LPF_LOC(g_object_new (LPF_TYPE_LOC, "name", "testloc2", "long", 2.71, "lat", 82.0, NULL));
    priv->locs = g_slist_append(priv->locs, loc);

    /* For now we have single trip with just enough data to identify it */
    part = g_object_new(LPF_TYPE_TRIP_PART, "line", "at the end of the longest", NULL);
    parts = g_slist_append (parts, part);
    trip = g_object_new(LPF_TYPE_TRIP, "parts", parts, NULL);
    priv->trips = g_slist_append (priv->trips, trip);
}

LpfProviderTestTest *
lpf_provider_test_test_new (void)
{
    return g_object_new (LPF_TYPE_PROVIDER_TEST_TEST, NULL);
}

G_MODULE_EXPORT LpfProvider *
lpf_provider_create (void)
{
    return LPF_PROVIDER (lpf_provider_test_test_new ());
}
