/*
 * lpf-trip.h: a trip via public transport
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

#ifndef _LPF_TRIP_H
#define _LPF_TRIP_H

#if !defined (__LIBPLANFAHR_H_INSIDE__) && !defined (LIBPLANFAHR_COMPILATION)
# error "Only <libplanfahr.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * LpfTripStatusFlags:
 * @LPF_TRIP_STATUS_FLAGS_NONE: No flags set.
 * @LPF_TRIP_STATUS_FLAGS_CANCELED: Trip got canceled
 *
 * Flags used for the status of a #LpfTrip.
 */
typedef enum {
    LPF_TRIP_STATUS_FLAGS_NONE = 0,
    LPF_TRIP_STATUS_FLAGS_CANCELED = (1<<0),
} LpfTripStatusFlags;

#define LPF_TYPE_TRIP lpf_trip_get_type()

#define LPF_TRIP(obj)                                            \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), LPF_TYPE_TRIP, LpfTrip))

#define LPF_TRIP_CLASS(klass)                                    \
    (G_TYPE_CHECK_CLASS_CAST ((klass), LPF_TYPE_TRIP, LpfTripClass))

#define LPF_IS_TRIP(obj)                                 \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LPF_TYPE_TRIP))

#define LPF_IS_TRIP_CLASS(klass)                         \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), LPF_TYPE_TRIP))

#define LPF_TRIP_GET_CLASS(obj)                                  \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), LPF_TYPE_TRIP, LpfTripClass))

typedef struct {
    GObject parent;
} LpfTrip;

typedef struct {
    GObjectClass parent_class;
} LpfTripClass;

GType lpf_trip_get_type (void);

GSList* lpf_trip_get_parts(LpfTrip *self);

G_END_DECLS

#endif /* _LPF_TRIP_H */
