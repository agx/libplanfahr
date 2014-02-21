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

gint hafas_bin6_parse_station(const gchar *data, guint16 off, LpfLoc *station, const char *enc);
guint hafas_bin6_parse_service_day (const char *data, int idx);
GDateTime* hafas_bin6_date_time(guint base_days, guint off_days, guint hours, guint min);

#endif /* _HAFAS_BIN6_H */
