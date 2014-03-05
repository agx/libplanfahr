/*
 * Copyright (C) 2014 Guido Guenther <agx@sigxcpu.org>
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
 */

#ifndef _LPF_PROVIDER_CH_SBB_H
#define _LPF_PROVIDER_CH_SBB_H

#include "hafas-bin6.h"

G_BEGIN_DECLS
#define LPF_TYPE_PROVIDER_CH_SBB lpf_provider_ch_sbb_get_type()
#define LPF_PROVIDER_CH_SBB(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LPF_TYPE_PROVIDER_CH_SBB, LpfProviderChSbb))
#define LPF_PROVIDER_CH_SBB_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), LPF_TYPE_PROVIDER_CH_SBB, LpfProviderChSbbClass))
#define LPF_IS_PROVIDER_CH_SBB(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LPF_TYPE_PROVIDER_CH_SBB))
#define LPF_IS_PROVIDER_CH_SBB_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), LPF_TYPE_PROVIDER_CH_SBB))
#define LPF_PROVIDER_CH_SBB_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), LPF_TYPE_PROVIDER_CH_SBB, LpfProviderChSbbClass))

typedef struct {
    LpfProviderHafasBin6 parent;
} LpfProviderChSbb;

typedef struct {
    LpfProviderHafasBin6Class parent_class;
} LpfProviderChSbbClass;

GType lpf_provider_ch_sbb_get_type (void);

LpfProviderChSbb *lpf_provider_ch_sbb_new (void);

G_END_DECLS
#endif /* _LPF_PROVIDER_CH_SBB */
