/*
 * lpf-manager.h: manage planfahr providers
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

#ifndef _LPF_MANAGER_H
#define _LPF_MANAGER_H

#if !defined (__LIBPLANFAHR_H_INSIDE__) && !defined (LIBPLANFAHR_COMPILATION)
# error "Only <libplanfahr.h> can be included directly."
#endif

#include <glib-object.h>
#include "lpf-provider.h"

#define LPF_PROVIDERS_ENV "LPF_PROVIDERS"

G_BEGIN_DECLS

/**
 * LPF_MANAGER_ERROR:
 *
 * Error domain for the provider manager
 */
#define LPF_MANAGER_ERROR g_spawn_error_quark ()

/**
 * LpfManagerError:
 * @LPF_MANAGER_ERROR_ACTIVATION_FAILED: activation of the provider failed
 *
 * Error codes returned by manager
 */
typedef enum {
    LPF_MANAGER_ERROR_ACTIVATION_FAILED,
} LpfManagerError;

#define LPF_TYPE_MANAGER lpf_manager_get_type()

G_DECLARE_FINAL_TYPE (LpfManager, lpf_manager, LPF, MANAGER, GObject)

LpfManager *lpf_manager_new (void);
GQuark lpf_manager_error_quark (void);
GStrv lpf_manager_get_available_providers(void);
LpfProvider* lpf_manager_activate_provider(LpfManager *self,
                                           const gchar *name,
                                           GError **error);
void lpf_manager_deactivate_provider(LpfManager *self, LpfProvider *provider);

G_END_DECLS

#endif /* _LPF_MANAGER_H */
