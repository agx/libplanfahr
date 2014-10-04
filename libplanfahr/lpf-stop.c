/*
 * lpf-loc.c: a stop during public transport
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

#include "lpf-loc.h"
#include "lpf-stop.h"
#include "lpf-priv.h"

enum {
    LPF_STOP_PROP_0 = 0,
    LPF_STOP_PROP_ARRIVAL,
    LPF_STOP_PROP_DEPARTURE,
    LPF_STOP_PROP_ARRIVAL_PLATFORM,
    LPF_STOP_PROP_DEPARTURE_PLATFORM,
    LPF_STOP_PROP_RT_ARRIVAL,
    LPF_STOP_PROP_RT_DEPARTURE,
    LPF_STOP_PROP_ARRIVAL_DELAY,
    LPF_STOP_PROP_DEPARTURE_DELAY,
};

/**
 * SECTION:lpf-stop
 * @short_description: A stop during public transport
 *
 * A #LpfStop adds arrival and departure times to a #LpfLoc.
 */

G_DEFINE_TYPE (LpfStop, lpf_stop, LPF_TYPE_LOC)
#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_STOP, LpfStopPrivate))

typedef struct _LpfStopPrivate LpfStopPrivate;
struct _LpfStopPrivate {
    gchar *name;
    /* Planned */
    GDateTime *arr, *dep;
    gchar *arr_plat, *dep_plat;
    /* Predicted */
    GDateTime *rt_arr, *rt_dep;
};

static void
lpf_stop_set_property (GObject *object,
                      guint property_id,
                      const GValue *value,
                      GParamSpec *pspec)
{
    LpfStop *self = LPF_STOP (object);
    LpfStopPrivate *priv = GET_PRIVATE(self);

    switch (property_id) {
    case LPF_STOP_PROP_ARRIVAL:
        if (priv->arr)
            g_date_time_unref (priv->arr);
        priv->arr = g_value_get_boxed (value);
        break;

    case LPF_STOP_PROP_DEPARTURE:
        if (priv->dep)
            g_date_time_unref (priv->dep);
        priv->dep = g_value_get_boxed (value);
        break;

    case LPF_STOP_PROP_ARRIVAL_PLATFORM:
        g_free (priv->arr_plat);
        priv->arr_plat = g_value_dup_string (value);
        break;

    case LPF_STOP_PROP_DEPARTURE_PLATFORM:
        g_free (priv->dep_plat);
        priv->dep_plat = g_value_dup_string (value);
        break;

    case LPF_STOP_PROP_RT_ARRIVAL:
        if (priv->rt_arr)
            g_date_time_unref (priv->rt_arr);
        priv->rt_arr = g_value_get_boxed (value);
        break;

    case LPF_STOP_PROP_RT_DEPARTURE:
        if (priv->rt_dep)
            g_date_time_unref (priv->rt_dep);
        priv->rt_dep = g_value_get_boxed (value);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

/* calc_delay: Calculate the time difference between planned and real time in
 * minutes */
static gint
calc_delay(GDateTime *planned, GDateTime *real)
{
    if (planned == NULL || real == NULL)
        return 0;

    return g_date_time_difference (real, planned) / (60 * 1000 * 1000);
}


static void
lpf_stop_get_property (GObject *object,
                      guint property_id,
                      GValue *value,
                      GParamSpec *pspec)
{
    LpfStop *self = LPF_STOP (object);
    LpfStopPrivate *priv = GET_PRIVATE(self);
    gint delay;

    switch (property_id) {
    case LPF_STOP_PROP_ARRIVAL:
        g_value_set_boxed (value, priv->arr);
        break;

    case LPF_STOP_PROP_DEPARTURE:
        g_value_set_boxed (value, priv->dep);
        break;

    case LPF_STOP_PROP_ARRIVAL_PLATFORM:
        g_value_set_string (value, priv->arr_plat);
        break;

    case LPF_STOP_PROP_DEPARTURE_PLATFORM:
        g_value_set_string (value, priv->dep_plat);
        break;

    case LPF_STOP_PROP_RT_ARRIVAL:
        g_value_set_boxed (value, priv->rt_arr);
        break;

    case LPF_STOP_PROP_RT_DEPARTURE:
        g_value_set_boxed (value, priv->rt_dep);
        break;

    case LPF_STOP_PROP_ARRIVAL_DELAY:
        delay = calc_delay (priv->arr, priv->rt_arr);
        g_value_set_int (value, delay);
        break;

    case LPF_STOP_PROP_DEPARTURE_DELAY:
        delay = calc_delay (priv->dep, priv->rt_dep);
        g_value_set_int (value, delay);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
lpf_stop_dispose(GObject *object)
{
    GObjectClass *parent_class = G_OBJECT_CLASS (lpf_stop_parent_class);

    if (parent_class->dispose != NULL)
        parent_class->dispose (object);
}


static void
lpf_stop_finalize (GObject *object)
{
    LpfStop *self = LPF_STOP (object);
    LpfStopPrivate *priv = GET_PRIVATE (self);
    GObjectClass *parent_class = G_OBJECT_CLASS (lpf_stop_parent_class);

    if (priv->dep)
        g_date_time_unref (priv->dep);
    if (priv->arr)
        g_date_time_unref (priv->arr);
    g_free (priv->name);
    g_free (priv->arr_plat);
    g_free (priv->dep_plat);
    if (priv->rt_dep)
        g_date_time_unref (priv->rt_dep);
    if (priv->rt_arr)
        g_date_time_unref (priv->rt_arr);
    if (parent_class->finalize != NULL)
        parent_class->finalize (object);

    parent_class->finalize (object);
}


static void
lpf_stop_class_init (LpfStopClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->dispose = lpf_stop_dispose;
    object_class->finalize = lpf_stop_finalize;
    g_type_class_add_private (klass, sizeof (LpfStopPrivate));

    object_class->set_property = lpf_stop_set_property;
    object_class->get_property = lpf_stop_get_property;

/**
 * LpfStop:departure:
 *
 * Train departs at this stop at this date and time
 */
    g_object_class_install_property (object_class,
                                     LPF_STOP_PROP_DEPARTURE,
                                     g_param_spec_boxed ("departure",
                                                         "Departure",
                                                         "Departure date and time",
                                                         G_TYPE_DATE_TIME,
                                                         G_PARAM_READWRITE));
/**
 * LpfStop:arrival:
 *
 * Train arrives at this stop at this date and time
 */
    g_object_class_install_property (object_class,
                                     LPF_STOP_PROP_ARRIVAL,
                                     g_param_spec_boxed ("arrival",
                                                         "Arrival",
                                                         "Arrival date and time",
                                                         G_TYPE_DATE_TIME,
                                                         G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
                                     LPF_STOP_PROP_ARRIVAL_PLATFORM,
                                     g_param_spec_string ("arr_plat",
                                                          "Arrival Platform",
                                                          "Arrival platform",
                                                          "",
                                                          G_PARAM_READWRITE));
    g_object_class_install_property (object_class,
                                     LPF_STOP_PROP_DEPARTURE_PLATFORM,
                                     g_param_spec_string ("dep_plat",
                                                          "Departure platform",
                                                          "Departure platform",
                                                          "",
                                                          G_PARAM_READWRITE));

/**
 * LpfStop:rt_departure:
 *
 * Real time departure time
 */
    g_object_class_install_property (object_class,
                                     LPF_STOP_PROP_RT_DEPARTURE,
                                     g_param_spec_boxed ("rt_departure",
                                                         "Real Time Departure",
                                                         "Real Time Departure date and time",
                                                         G_TYPE_DATE_TIME,
                                                         G_PARAM_READWRITE));
/**
 * LpfStop:rt_arrival:
 *
 * Real time arrival time
 */
    g_object_class_install_property (object_class,
                                     LPF_STOP_PROP_RT_ARRIVAL,
                                     g_param_spec_boxed ("rt_arrival",
                                                         "Real Time Arrival",
                                                         "Real time arrival date and time",
                                                         G_TYPE_DATE_TIME,
                                                         G_PARAM_READWRITE));

    g_object_class_install_property (object_class,
                                     LPF_STOP_PROP_ARRIVAL_DELAY,
                                     g_param_spec_int ("arrival_delay",
                                                       "Arrival Delay",
                                                       "Arrival delay",
                                                       INT_MIN,
                                                       INT_MAX,
                                                       0,
                                                       G_PARAM_READABLE));

    g_object_class_install_property (object_class,
                                     LPF_STOP_PROP_DEPARTURE_DELAY,
                                     g_param_spec_int ("departure_delay",
                                                       "Departure Delay",
                                                       "Departure delay",
                                                       INT_MIN,
                                                       INT_MAX,
                                                       0,
                                                       G_PARAM_READABLE));
}

static void
lpf_stop_init (LpfStop *self)
{
}
