/*
 * lpf-loc.c: a trip via public transport
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

#include <glob.h>
#include <string.h>
#include <math.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gmodule.h>

#include "lpf-enumtypes.h"
#include "lpf-trip.h"
#include "lpf-loc.h"
#include "lpf-priv.h"

enum {
    LPF_TRIP_PROP_0 = 0,
    LPF_TRIP_PROP_PARTS,
};

/**
 * SECTION:lpf-trip
 * @short_description: A trip via public transport
 *
 * A #LpfTrip represents a trip. It consists of several #LpfTripParts each with a start
 * and end location.
 */

G_DEFINE_TYPE (LpfTrip, lpf_trip, G_TYPE_OBJECT)
#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_TRIP, LpfTripPrivate))

typedef struct _LpfTripPrivate LpfTripPrivate;
struct _LpfTripPrivate {
    GSList *parts;
};

/**
 * lpf_trip_get_parts:
 * @self: A #LpfTrip
 *
 * Returns the individual parts of the trip.
 *
 * Returns: (transfer none) (element-type LpfTripPart): The parts of the trip.
 **/
GSList*
lpf_trip_get_parts(LpfTrip *self)
{
    GSList *parts;

    g_object_get(self, "parts", &parts, NULL);
    return parts;
}


static void
lpf_trip_set_property (GObject *object,
                      guint property_id,
                      const GValue *value,
                      GParamSpec *pspec)
{
    LpfTrip *self = LPF_TRIP (object);
    LpfTripPrivate *priv = GET_PRIVATE(self);

    switch (property_id) {
    case LPF_TRIP_PROP_PARTS:
        priv->parts = g_value_get_pointer(value);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
lpf_trip_get_property (GObject *object,
                      guint property_id,
                      GValue *value,
                      GParamSpec *pspec)
{
    LpfTrip *self = LPF_TRIP (object);
    LpfTripPrivate *priv = GET_PRIVATE(self);

    switch (property_id) {
    case LPF_TRIP_PROP_PARTS:
        g_value_set_pointer (value, priv->parts);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lpf_trip_dispose(GObject *object)
{
    GObjectClass *parent_class = G_OBJECT_CLASS (lpf_trip_parent_class);

    if (parent_class->dispose != NULL)
        parent_class->dispose (object);
}


static void
lpf_trip_finalize (GObject *object)
{
    LpfTrip *self = LPF_TRIP (object);
    LpfTripPrivate *priv = GET_PRIVATE (self);
    GObjectClass *parent_class = G_OBJECT_CLASS (lpf_trip_parent_class);

    g_slist_free_full (priv->parts, g_object_unref);

    parent_class->finalize (object);
}


static void
lpf_trip_class_init (LpfTripClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = lpf_trip_dispose;
    object_class->finalize = lpf_trip_finalize;
    g_type_class_add_private (klass, sizeof (LpfTripPrivate));

    object_class->set_property = lpf_trip_set_property;
    object_class->get_property = lpf_trip_get_property;


/**
 * LpfTrip:parts: (type GSList(LpfTripPart))
 *
 * The parts of a trip as #LpfTripPart.
 */
    g_object_class_install_property (object_class,
                                     LPF_TRIP_PROP_PARTS,
                                     g_param_spec_pointer ("parts",
                                                           "trip parts",
                                                           "The parts of the trip",
                                                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
}

static void
lpf_trip_init (LpfTrip *self)
{
}
