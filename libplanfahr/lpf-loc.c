/*
 * lpf-loc.c: a public transport location
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
#include "lpf-priv.h"

enum {
    LPF_LOC_PROP_0 = 0,
    LPF_LOC_PROP_NAME,
    LPF_LOC_PROP_LONG,
    LPF_LOC_PROP_LAT,
};

/**
 * SECTION:lpf-loc
 * @short_description: Location
 *
 * A #LpfLoc represents a location (e.g. a station)
 */

G_DEFINE_TYPE (LpfLoc, lpf_loc, G_TYPE_OBJECT)
#define GET_PRIVATE(o) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_LOC, LpfLocPrivate))

typedef struct _LpfLocPrivate LpfLocPrivate;
struct _LpfLocPrivate {
    gchar *name;
    gdouble long_;
    gdouble lat;
    gpointer opaque;
};

static void
lpf_loc_set_property (GObject *object,
                      guint property_id,
                      const GValue *value,
                      GParamSpec *pspec)
{
    LpfLoc *self = LPF_LOC (object);
    LpfLocPrivate *priv = GET_PRIVATE(self);

    switch (property_id) {
    case LPF_LOC_PROP_NAME:
        g_free (priv->name);
        priv->name = g_value_dup_string (value);
        break;

    case LPF_LOC_PROP_LONG:
        priv->long_ = g_value_get_double(value);
        break;

    case LPF_LOC_PROP_LAT:
        priv->lat = g_value_get_double(value);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
lpf_loc_get_property (GObject *object,
                      guint property_id,
                      GValue *value,
                      GParamSpec *pspec)
{
    LpfLoc *self = LPF_LOC (object);
    LpfLocPrivate *priv = GET_PRIVATE(self);

    switch (property_id) {
    case LPF_LOC_PROP_NAME:
        g_value_set_string (value, priv->name);
        break;

    case LPF_LOC_PROP_LONG:
        g_value_set_double (value, priv->long_);
        break;

    case LPF_LOC_PROP_LAT:
        g_value_set_double (value, priv->lat);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
lpf_loc_dispose(GObject *object)
{
    GObjectClass *parent_class = G_OBJECT_CLASS (lpf_loc_parent_class);

    if (parent_class->dispose != NULL)
        parent_class->dispose (object);
}


static void
lpf_loc_finalize (GObject *object)
{
    LpfLoc *self = LPF_LOC (object);
    LpfLocPrivate *priv = GET_PRIVATE (self);
    GObjectClass *parent_class = G_OBJECT_CLASS (lpf_loc_parent_class);

    g_free (priv->name);
    g_free (priv->opaque);

    parent_class->finalize (object);
}


static void
lpf_loc_class_init (LpfLocClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    object_class->dispose = lpf_loc_dispose;
    object_class->finalize = lpf_loc_finalize;
    g_type_class_add_private (klass, sizeof (LpfLocPrivate));

    object_class->set_property = lpf_loc_set_property;
    object_class->get_property = lpf_loc_get_property;

    pspec = g_param_spec_string ("name",
                                 "Name",
                                 "Get Location name",
                                 "", G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
    g_object_class_install_property (object_class, LPF_LOC_PROP_NAME, pspec);

    pspec = g_param_spec_double ("long",
                                 "Longitude",
                                 "Get Location Longitude",
                                 -180.0, +180.0, 0,
                                 G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
    g_object_class_install_property (object_class, LPF_LOC_PROP_LONG, pspec);

    pspec = g_param_spec_double ("lat",
                                 "latitude",
                                 "Get Location Latitude",
                                 -90.0, +90.0, 0,
                                 G_PARAM_CONSTRUCT | G_PARAM_READWRITE);
    g_object_class_install_property (object_class, LPF_LOC_PROP_LAT, pspec);
}

static void
lpf_loc_init (LpfLoc *self)
{
}

/**
 * lpf_loc_get_opaque: (skip)
 * @self: a #LpfLoc
 *
 * Get the opaque data stored by a provider.
 *
 * Returns: pointer to the opaue data
 */
gpointer
lpf_loc_get_opaque(LpfLoc *self)
{
    LpfLocPrivate *priv = GET_PRIVATE (self);

    return priv->opaque;
}

/**
 * lpf_loc_set_opaque: (skip)
 * @self: a #LpfLoc
 * @opaque: pointer to opaque data
 *
 * Set the opaque data stored by a provider. The data will
 * be freed using g_free.
 */
void
lpf_loc_set_opaque(LpfLoc *self, gpointer opaque)
{
    LpfLocPrivate *priv = GET_PRIVATE (self);

    priv->opaque = opaque;
}

/**
 * lpf_loc_get_name: (transfer none):
 * @self: a #LpfLoc
 *
 * Returns: the location's name
 */
const gchar*
lpf_loc_get_name(LpfLoc *self)
{
    LpfLocPrivate *priv = GET_PRIVATE (self);

    return priv->name;
}

/**
 * lpf_loc_get_long:
 * @self: a #LpfLoc
 *
 * Returns: the locations longitude
 */
double
lpf_loc_get_long(LpfLoc *self)
{
    LpfLocPrivate *priv = GET_PRIVATE (self);

    return priv->long_;
}

/**
 * lpf_loc_get_lat:
 * @self: a #LpfLoc
 *
 * Returns: the locations latitude
 */
double
lpf_loc_get_lat(LpfLoc *self)
{
    LpfLocPrivate *priv = GET_PRIVATE (self);

    return priv->lat;
}
