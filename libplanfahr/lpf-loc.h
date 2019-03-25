/*
 * lpf-loc.h: a public transport location
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

#ifndef _LPF_LOC_H
#define _LPF_LOC_H

#if !defined (__LIBPLANFAHR_H_INSIDE__) && !defined (LIBPLANFAHR_COMPILATION)
# error "Only <libplanfahr.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define LPF_TYPE_LOC (lpf_loc_get_type())

G_DECLARE_FINAL_TYPE (LpfLoc, lpf_loc, LPF, LOC, GObject)

const gchar *lpf_loc_get_name (LpfLoc* self);
double       lpf_loc_get_lat  (LpfLoc *self);
double       lpf_loc_get_long (LpfLoc *self);

gpointer lpf_loc_get_opaque (LpfLoc *self);
void lpf_loc_set_opaque (LpfLoc *self, gpointer opaque);

G_END_DECLS

#endif /* _LPF_LOC_H */
