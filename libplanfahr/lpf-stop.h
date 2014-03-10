/*
 * lpf-stop.h: a public transport stop
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

#ifndef _LPF_STOP_H
#define _LPF_STOP_H

#if !defined (__LIBPLANFAHR_H_INSIDE__) && !defined (LIBPLANFAHR_COMPILATION)
# error "Only <libplanfahr.h> can be included directly."
#endif

#include <glib-object.h>

G_BEGIN_DECLS

#define LPF_TYPE_STOP lpf_stop_get_type()

#define LPF_STOP(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LPF_TYPE_STOP, LpfStop))

#define LPF_STOP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), LPF_TYPE_STOP, LpfStopClass))

#define LPF_IS_STOP(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LPF_TYPE_STOP))

#define LPF_IS_STOP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), LPF_TYPE_STOP))

#define LPF_STOP_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), LPF_TYPE_STOP, LpfStopClass))

typedef struct {
  GObject parent;
} LpfStop;

typedef struct {
  GObjectClass parent_class;
} LpfStopClass;

GType lpf_stop_get_type (void);

G_END_DECLS

#endif /* _LPF_STOP_H */
