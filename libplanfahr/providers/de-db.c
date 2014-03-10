/*
 * db.c: Deutsche Bahn provider for libplanfahr
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
n */

#include <config.h>

#include <gmodule.h>

#include "de-db.h"
#include "hafas-bin6.h"
#include "lpf-provider.h"

#define LOC_URL    "http://mobile.bahn.de/bin/mobil/query.exe/en"
#define TRIPS_URL  "http://reiseauskunft.bahn.de/bin/query.exe/eox"

#define PROVIDER_NAME "de_db"

enum {
    PROP_0,
    PROP_NAME,
    LAST_PROP
};

static void lpf_provider_de_db_interface_init (LpfProviderInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LpfProviderDeDb, lpf_provider_de_db, LPF_TYPE_PROVIDER_HAFAS_BIN6,
                         G_IMPLEMENT_INTERFACE (LPF_TYPE_PROVIDER, lpf_provider_de_db_interface_init));

int lpf_provider_major_version = LPF_PROVIDER_MAJOR_VERSION;
int lpf_provider_minor_version = LPF_PROVIDER_MINOR_VERSION;


static const char*
lpf_provider_de_db_get_name (LpfProvider *self)
{
    return PROVIDER_NAME;
}


static const char*
lpf_provider_de_db_locs_url(LpfProviderHafasBin6 *self)
{
    return LOC_URL;
}


static const char*
lpf_provider_de_db_trips_url(LpfProviderHafasBin6 *self)
{
    return TRIPS_URL;
}


G_MODULE_EXPORT LpfProvider *
lpf_provider_create (void)
{
    return LPF_PROVIDER (lpf_provider_de_db_new ());
}


static void
set_property (GObject *object, guint prop_id,
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
get_property (GObject *object, guint prop_id,
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
lpf_provider_de_db_class_init (LpfProviderDeDbClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    LpfProviderHafasBin6Class *hafas_class = LPF_PROVIDER_HAFAS_BIN6_CLASS (klass);

    object_class->get_property = get_property;
    object_class->set_property = set_property;

    /* LpfProvider */
    g_object_class_override_property (object_class,
                                      PROP_NAME,
                                      "name");

    /* LpfProviderHafasBin6 */
    hafas_class->locs_url = lpf_provider_de_db_locs_url;
    hafas_class->trips_url = lpf_provider_de_db_trips_url;
}

static void
lpf_provider_de_db_interface_init (LpfProviderInterface *iface)
{
    iface->get_name = lpf_provider_de_db_get_name;
}

static void
lpf_provider_de_db_init (LpfProviderDeDb *self)
{
}

LpfProviderDeDb *
lpf_provider_de_db_new (void)
{
    return g_object_new (LPF_TYPE_PROVIDER_DE_DB, NULL);
}
