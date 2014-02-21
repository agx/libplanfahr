/*
 * hafas-bin6.c: HAFAS Binary Format Version 6 provider
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

gint
hafas_bin6_parse_station(const gchar *data, guint16 off, LpfLoc *loc, const char *enc)
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
 * hafas_bin6_parse_service_day:
 *
 * Parse a servide day entry and return the offset from the base date in days.
 */
guint
hafas_bin6_parse_service_day (const char *data, int idx)
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
 * hafas_bin6_date_time:
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
hafas_bin6_date_time(guint base_days, guint off_days, guint hours, guint min)
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
