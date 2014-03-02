/*
 * lpf-provider-hafas-bin6.c: HAFAS Binary Format Version 6 provider
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
#include <libsoup/soup.h>


#include "hafas-bin6.h"
#include "lpf-loc.h"
#include "lpf-priv.h"
#include "lpf-provider.h"

static void lpf_provider_hafas_bin6_interface_init (LpfProviderInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LpfProviderHafasBin6, lpf_provider_hafas_bin6, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (LPF_TYPE_PROVIDER, lpf_provider_hafas_bin6_interface_init));

#define PROVIDER_NAME "hafas_bin6"

enum {
    PROP_0,
    PROP_NAME,
    LAST_PROP
};

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_PROVIDER_HAFAS_BIN6, LpfProviderHafasBin6Private))


typedef struct _LpfProviderHafasBin6Private LpfProviderHafasBin6Private;

struct _LpfProviderHafasBin6Private {
    gchar *name;
};

static void
lpf_provider_hafas_bin6_set_property (GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
    LpfProviderHafasBin6Private *priv = GET_PRIVATE (object);

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
lpf_provider_hafas_bin6_get_property (GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
    LpfProviderHafasBin6Private *priv = GET_PRIVATE (object);

    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string (value, priv->name);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

gint
lpf_provider_hafas_bin6_parse_station(const gchar *data, guint16 off, LpfLoc *loc, const char *enc)
{
    gchar *name;
    HafasBin6Station *station;
    gdouble lon, lat;
    gint ret = -1;

    station = HAFAS_BIN6_STATION(data, off);

    name = g_convert (HAFAS_BIN6_STR(data, station->name_off),
                      -1,
                      "utf-8",
                      enc,
                      NULL, NULL, NULL);
    if (name == NULL) {
        g_warning ("Failed to convert station name at %d", off);
        goto err;
    }
    lon = HAFAS_BIN6_LL_DOUBlE (station->lon);
    lat = HAFAS_BIN6_LL_DOUBlE (station->lat);

    LPF_DEBUG("name: %s", name);
    g_object_set (loc, "name", name, "long", lon, "lat", lat,
                  NULL);
    ret = 0;
err:
    return ret;
}


/**
 * lpf_provicer_hafas_bin6_parse_service_day:
 *
 * Parse a servide day entry and return the offset from the base date in days.
 */
guint
lpf_provider_hafas_bin6_parse_service_day (const char *data, int idx)
{
    gint i;
    gchar bits;
    guint off;
    const HafasBin6ServiceDay *s;

    s = HAFAS_BIN6_SERVICE_DAY(data, idx);
    off = s->byte_base * 8;

    /* look at all service bytes */
    for (i = 0; i < s->byte_len; i++) {
        bits = (&(s->byte0))[i];
        if (bits == 0) { /* zero means +8 days */
            off += 8;
            continue;
        }

        /* count leading zeros meaning +1 day */
        while ((bits & 0x80) == 0) {
            bits <<= 1;
            off++;
        }
        break;
    }
    return off;
}

/**
 * lpf_provider_hafas_bin6_date_time:
 * @base_days: day off set from 1980-01-01
 * @off_days: day offset from base_days
 * @hours: hour trip starts/ends
 * @minutes: minute the trip starts/ends
 *
 * Calculate date and time from hafas bin 6 input
 *
 * Returns: the travel date and time as #GDateTime
 */
GDateTime*
lpf_provider_hafas_bin6_date_time(guint base_days, guint off_days, guint hours, guint min)
{
    /* FIXME: should we always use Europe/Berlin as TZ? */
    GDateTime *dt, *base = g_date_time_new_local (1979, 12, 31, 0, 0, 0);

    dt = g_date_time_add (base,
                          (base_days + off_days) * G_TIME_SPAN_DAY +
                          hours * G_TIME_SPAN_HOUR +
                          min * G_TIME_SPAN_MINUTE);
    g_date_time_unref (base);
    return dt;
}

static void
lpf_provider_hafas_bin6_activate (LpfProvider *self, GObject *obj)
{
    g_warn_if_reached ();
}

static void
lpf_provider_hafas_bin6_deactivate (LpfProvider *self, GObject *obj)
{
    g_warn_if_reached ();
}


static void
lpf_provider_hafas_bin6_finalize (GObject *self)
{
    G_OBJECT_CLASS (lpf_provider_hafas_bin6_parent_class)->finalize (self);
}

static void
lpf_provider_hafas_bin6_class_init (LpfProviderHafasBin6Class *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (LpfProviderHafasBin6Private));

    object_class->get_property = lpf_provider_hafas_bin6_get_property;
    object_class->set_property = lpf_provider_hafas_bin6_set_property;
    object_class->finalize = lpf_provider_hafas_bin6_finalize;

    g_object_class_override_property (object_class,
                                      PROP_NAME,
                                      "name");
}

static void
lpf_provider_hafas_bin6_interface_init (LpfProviderInterface *iface)
{
    /* abstract base class */
    iface->activate = lpf_provider_hafas_bin6_activate;
    iface->deactivate = lpf_provider_hafas_bin6_deactivate;

    /* To be implemented */
    iface->get_locs = NULL; /* lpf_provider_hafas_bin6_get_locs; */
    iface->get_trips = NULL; /* lpf_provider_hafas_bin6_get_trips; */
}

static void
lpf_provider_hafas_bin6_init (LpfProviderHafasBin6 *self)
{
}
