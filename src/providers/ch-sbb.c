/*
 * ch-sbb.c: SBB provider for libplanfahr
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

#include <stdlib.h>
#include <string.h>

#include <gmodule.h>

#include "ch-sbb.h"
#include "hafas-bin6.h"
#include "lpf-provider.h"

#define LOC_URL    "http://fahrplan.sbb.ch/bin/query.exe/dn"
#define TRIPS_URL  "http://fahrplan.sbb.ch/bin/query.exe/dn"

#define PROVIDER_NAME "ch_sbb"

enum {
    PROP_0,
    PROP_NAME,
    LAST_PROP
};

static void lpf_provider_ch_sbb_interface_init (LpfProviderInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LpfProviderChSbb, lpf_provider_ch_sbb, LPF_TYPE_PROVIDER_HAFAS_BIN6,
                         G_IMPLEMENT_INTERFACE (LPF_TYPE_PROVIDER, lpf_provider_ch_sbb_interface_init));

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_PROVIDER_CH_SBB, LpfProviderChSbbPrivate))

int lpf_provider_major_version = LPF_PROVIDER_MAJOR_VERSION;
int lpf_provider_minor_version = LPF_PROVIDER_MINOR_VERSION;

typedef struct _LpfProviderChSbbPrivate LpfProviderChSbbPrivate;
struct _LpfProviderChSbbPrivate {
    gchar *name;
};


static const char*
lpf_provider_ch_sbb_get_name (LpfProvider *self)
{
    LpfProviderChSbbPrivate *priv = GET_PRIVATE (self);

    return priv->name;
}


static const char*
lpf_provider_ch_sbb_locs_url(LpfProviderHafasBin6 *self)
{
    return LOC_URL;
}


static const char*
lpf_provider_ch_sbb_trips_url(LpfProviderHafasBin6 *self)
{
    return TRIPS_URL;
}


G_MODULE_EXPORT LpfProvider *
lpf_provider_create (void)
{
    return LPF_PROVIDER (lpf_provider_ch_sbb_new ());
}


static void
set_property (GObject *object, guint prop_id,
              const GValue *value, GParamSpec *pspec)
{
    LpfProviderChSbbPrivate *priv = GET_PRIVATE (object);

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
    LpfProviderChSbbPrivate *priv = GET_PRIVATE (object);

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
lpf_provider_ch_sbb_finalize (GObject *self)
{
    LpfProviderChSbbPrivate *priv = GET_PRIVATE(self);

    g_free (priv->name);
    G_OBJECT_CLASS (lpf_provider_ch_sbb_parent_class)->finalize (self);
}


static void
lpf_provider_ch_sbb_class_init (LpfProviderChSbbClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    LpfProviderHafasBin6Class *hafas_class = LPF_PROVIDER_HAFAS_BIN6_CLASS (klass);

    g_type_class_add_private (klass, sizeof (LpfProviderChSbbPrivate));

    object_class->get_property = get_property;
    object_class->set_property = set_property;
    object_class->finalize = lpf_provider_ch_sbb_finalize;

    /* LpfProvider */
    g_object_class_override_property (object_class,
                                      PROP_NAME,
                                      "name");

    /* LpfProviderHafasBin6 */
    hafas_class->locs_url = lpf_provider_ch_sbb_locs_url;
    hafas_class->trips_url = lpf_provider_ch_sbb_trips_url;
}

static void
lpf_provider_ch_sbb_interface_init (LpfProviderInterface *iface)
{
    iface->get_name = lpf_provider_ch_sbb_get_name;
}

static void
lpf_provider_ch_sbb_init (LpfProviderChSbb *self)
{
}

LpfProviderChSbb *
lpf_provider_ch_sbb_new (void)
{
    return g_object_new (LPF_TYPE_PROVIDER_CH_SBB, LPF_PROVIDER_PROP_NAME,
                         PROVIDER_NAME, NULL);
}
