/*
 * lpf-trip.h: a part of a trip via public transport
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

#ifndef _LPF_TRIP_PART_H
#define _LPF_TRIP_PART_H

#include <glib-object.h>

G_BEGIN_DECLS

#define LPF_TYPE_TRIP_PART lpf_trip_part_get_type()

#define LPF_TRIP_PART(obj)                                            \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), LPF_TYPE_TRIP_PART, LpfTripPart))

#define LPF_TRIP_PART_CLASS(klass)                                    \
    (G_TYPE_CHECK_CLASS_CAST ((klass), LPF_TYPE_TRIP_PART, LpfTripPartClass))

#define LPF_IS_TRIP_PART(obj)                                 \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LPF_TYPE_TRIP_PART))

#define LPF_IS_TRIP_PART_CLASS(klass)                         \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), LPF_TYPE_TRIP_PART))

#define LPF_TRIP_PART_GET_CLASS(obj)                                  \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), LPF_TYPE_TRIP_PART, LpfTripPartClass))

typedef struct {
    GObject parent;
} LpfTripPart;

typedef struct {
    GObjectClass parent_class;
} LpfTripPartClass;

GType lpf_trip_part_get_type (void);

G_END_DECLS

#endif /* _LPF_TRIP_PART_H */
