/*
 * hafas-bin6.h: hafas binary format version 6
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

#ifndef _HAFAS_BIN6_H
#define _HAFAS_BIN6_H

#include "hafas-bin6-format.h"
#include "lpf-loc.h"

G_BEGIN_DECLS
#define LPF_TYPE_PROVIDER_HAFAS_BIN6 lpf_provider_hafas_bin6_get_type()
#define LPF_PROVIDER_HAFAS_BIN6(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LPF_TYPE_PROVIDER_HAFAS_BIN6, LpfProviderHafasBin6))
#define LPF_PROVIDER_HAFAS_BIN6_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), LPF_TYPE_PROVIDER_HAFAS_BIN6, LpfProviderHafasBin6Class))
#define LPF_IS_PROVIDER_HAFAS_BIN6(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LPF_TYPE_PROVIDER_HAFAS_BIN6))
#define LPF_IS_PROVIDER_HAFAS_BIN6_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), LPF_TYPE_PROVIDER_HAFAS_BIN6))
#define LPF_PROVIDER_HAFAS_BIN6_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), LPF_TYPE_PROVIDER_HAFAS_BIN6, LpfProviderHafasBin6Class))

typedef struct {
    GObject parent;
} LpfProviderHafasBin6;

typedef struct {
    GObjectClass parent_class;
} LpfProviderHafasBin6Class;

GType lpf_provider_hafas_bin6_get_type (void);

gint lpf_provider_hafas_bin6_parse_station(const gchar *data, guint16 off, LpfLoc *station, const char *enc);
guint lpf_provider_hafas_bin6_parse_service_day (const char *data, int idx);
GDateTime* lpf_provider_hafas_bin6_date_time(guint base_days, guint off_days, guint hours, guint min);

G_END_DECLS
#endif /* _HAFAS_BIN6_H */
