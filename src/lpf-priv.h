/*
 * lpf-priv.h: internal functions and macros
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

#ifndef _LPF_PRIV_H
#define _LPF_PRIV_H

#if !defined (LIBPLANFAHR_COMPILATION)
# error "Only <libplanfahr.h> can be included directly."
#endif

#include <config.h>

#ifdef ENABLE_DEBUG

#include <glib.h>
#include <glib/gprintf.h>

#define LPF_DEBUG(fmt,...) \
    g_printf ("DEBUG: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#else
#define LPF_DEBUG(fmt,...) \
    do { } while (0)
#endif /* !ENABLE_DEBUG */

#endif
