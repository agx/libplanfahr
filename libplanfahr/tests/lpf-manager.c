/*
 * db.c: Deutsche Bahn provider for libplanfahr
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

#include "../libplanfahr.h"

#define LPF_TEST_PROVIDER "test-test"

LpfManager *manager;
LpfProvider *provider;

static void
test_lpf_manager(void)
{
    manager = lpf_manager_new();
    g_assert_nonnull (manager);
}


static void
test_lpf_manager_activate_provider(void)
{
    GError *err;

    g_assert_nonnull (manager);
    provider = lpf_manager_activate_provider(manager, LPF_TEST_PROVIDER, &err);
    g_assert_nonnull (provider);
    g_assert_no_error (err);
    g_assert_cmpstr ("test_test", ==, lpf_provider_get_name(provider));
}


static void
test_lpf_manager_activate_nonexistent_provider(void)
{
    GError *err;
    LpfProvider *nonexistent;

    g_assert_nonnull (manager);
    nonexistent = lpf_manager_activate_provider(manager, "doesnot-exist", &err);
    g_assert_error (err, LPF_MANAGER_ERROR,  LPF_MANAGER_ERROR_ACTIVATION_FAILED);
    g_assert_null (nonexistent);
}


static void
test_lpf_manager_deactivate_provider(void)
{
    g_assert_nonnull (manager);
    lpf_manager_deactivate_provider(manager, provider);
}


static void
test_lpf_manager_get_available_providers(void)
{
    GStrv providers;
    gint i;

    g_assert_nonnull (manager);
    providers = lpf_manager_get_available_providers();
    g_assert_nonnull(providers);
    for (i = 0; i < g_strv_length (providers); i++) {
        if (!g_strcmp0 (LPF_TEST_PROVIDER, providers[i]))
            return;
    }
    g_assert_not_reached(); /* test provider not found */
}


int main(int argc, char **argv)
{
    gboolean ret;

    g_test_init (&argc, &argv, NULL);
    g_setenv(LPF_PROVIDERS_ENV, LPF_TEST_SRCDIR "/../providers/.libs/", TRUE);

    g_test_add_func ("/libplanfahr/lpf-manager/new", test_lpf_manager);
    g_test_add_func ("/libplanfahr/lpf-manager/get_available-providers", test_lpf_manager_get_available_providers);
    g_test_add_func ("/libplanfahr/lpf-manager/activate-provider", test_lpf_manager_activate_provider);
    g_test_add_func ("/libplanfahr/lpf-manager/activate-nonexistent-provider", test_lpf_manager_activate_nonexistent_provider);
    g_test_add_func ("/libplanfahr/lpf-manager/deactivate-provider", test_lpf_manager_deactivate_provider);

    ret = g_test_run ();
    return ret;
}
