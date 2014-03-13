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

#ifndef _TEST_TEST_H
#define _TEST_TEST_H

#include "lpf-loc.h"

G_BEGIN_DECLS
#define LPF_TYPE_PROVIDER_TEST_TEST lpf_provider_test_test_get_type()
#define LPF_PROVIDER_TEST_TEST(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), LPF_TYPE_PROVIDER_TEST_TEST, LpfProviderTestTest))
#define LPF_PROVIDER_TEST_TEST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), LPF_TYPE_PROVIDER_TEST_TEST, LpfProviderTestTestClass))
#define LPF_IS_PROVIDER_TEST_TEST(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LPF_TYPE_PROVIDER_TEST_TEST))
#define LPF_IS_PROVIDER_TEST_TEST_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), LPF_TYPE_PROVIDER_TEST_TEST))
#define LPF_PROVIDER_TEST_TEST_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), LPF_TYPE_PROVIDER_TEST_TEST, LpfProviderTestTestClass))

typedef struct {
    GObject parent;
} LpfProviderTestTest;

typedef struct {
    GObjectClass parent_class;

    const char* (*locs_url)(LpfProviderTestTest *self);
    const char* (*trips_url)(LpfProviderTestTest *self);
} LpfProviderTestTestClass;

GType lpf_provider_test_test_get_type (void);

LpfProviderTestTest *lpf_provider_test_test_new (void);

G_END_DECLS
#endif /* _TEST_TEST_H */
