/*
 * lpf-loc.c: a part of a trip via public transport
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

#include "lpf-trip-part.h"
#include "lpf-stop.h"
#include "lpf-priv.h"

enum {
    LPF_TRIP_PART_PROP_0 = 0,
    LPF_TRIP_PART_PROP_START,
    LPF_TRIP_PART_PROP_END,
    LPF_TRIP_PART_PROP_LINE,
    LPF_TRIP_PART_PROP_STOPS,
};

/**
 * SECTION:lpf-trip-part
 * @short_description: A part of a trip
 *
 * A #LpfTripPart represents a part of a trip. It has a start and end
 * #LpfStop.
 */

G_DEFINE_TYPE (LpfTripPart, lpf_trip_part, G_TYPE_OBJECT)
#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_TRIP_PART, LpfTripPartPrivate))

typedef struct _LpfTripPartPrivate LpfTripPartPrivate;
struct _LpfTripPartPrivate {
    LpfStop *start;
    LpfStop *end;
    gchar *line;
    GSList *stops;
};

static void
lpf_trip_part_set_property (GObject *object,
                      guint property_id,
                      const GValue *value,
                      GParamSpec *pspec)
{
    LpfTripPart *self = LPF_TRIP_PART (object);
    LpfTripPartPrivate *priv = GET_PRIVATE(self);

    switch (property_id) {
    case LPF_TRIP_PART_PROP_START:
        if (priv->start)
            g_object_unref (priv->start);
        priv->start = g_value_get_object(value);
        break;

    case LPF_TRIP_PART_PROP_END:
        if (priv->end)
            g_object_unref (priv->end);
        priv->end = g_value_get_object(value);
        break;

    case LPF_TRIP_PART_PROP_LINE:
        g_free (priv->line);
        priv->line = g_value_dup_string(value);
        break;

    case LPF_TRIP_PART_PROP_STOPS:
        priv->stops = g_value_get_pointer(value);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
lpf_trip_part_get_property (GObject *object,
                      guint property_id,
                      GValue *value,
                      GParamSpec *pspec)
{
    LpfTripPart *self = LPF_TRIP_PART (object);
    LpfTripPartPrivate *priv = GET_PRIVATE(self);

    switch (property_id) {
    case LPF_TRIP_PART_PROP_START:
        g_value_set_object (value, priv->start);
        break;

    case LPF_TRIP_PART_PROP_END:
        g_value_set_object (value, priv->end);
        break;

    case LPF_TRIP_PART_PROP_LINE:
        g_value_set_string (value, priv->line);
        break;

    case LPF_TRIP_PART_PROP_STOPS:
        g_value_set_pointer (value, priv->stops);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lpf_trip_part_finalize (GObject *object)
{
    LpfTripPart *self = LPF_TRIP_PART (object);
    LpfTripPartPrivate *priv = GET_PRIVATE (self);
    GObjectClass *parent_class = G_OBJECT_CLASS (lpf_trip_part_parent_class);

    g_object_unref (priv->start);
    g_object_unref (priv->end);
    g_slist_free_full (priv->stops, g_object_unref);
    g_free (priv->line);

    parent_class->finalize (object);
}


static void
lpf_trip_part_class_init (LpfTripPartClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = lpf_trip_part_finalize;
    g_type_class_add_private (klass, sizeof (LpfTripPartPrivate));

    object_class->set_property = lpf_trip_part_set_property;
    object_class->get_property = lpf_trip_part_get_property;

    g_object_class_install_property (object_class,
                                     LPF_TRIP_PART_PROP_START,
                                     g_param_spec_object ("start",
                                                          "Start",
                                                          "The start location and date/time",
                                                          LPF_TYPE_STOP,
                                                          G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
                                     LPF_TRIP_PART_PROP_END,
                                     g_param_spec_object ("end",
                                                          "End",
                                                          "The end location and date/time",
                                                          LPF_TYPE_STOP,
                                                          G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
                                     LPF_TRIP_PART_PROP_LINE,
                                     g_param_spec_string ("line",
                                                          "Line",
                                                          "The Line that operates between start and end",
                                                          "",
                                                          G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
/**
 * LpfTripPart:stops: (type GSList(LpfStop))
 */
    g_object_class_install_property (object_class,
                                     LPF_TRIP_PART_PROP_STOPS,
                                     g_param_spec_pointer ("stops",
                                                           "Stops",
                                                           "Stops between start and end",
                                                           G_PARAM_CONSTRUCT | G_PARAM_READWRITE));
}

static void
lpf_trip_part_init (LpfTripPart *self)
{
}
