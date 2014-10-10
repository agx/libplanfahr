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


typedef struct {
    LpfManager *manager;
    LpfProvider *provider;
    GMainLoop *loop;
    gboolean got_loc_reached;
} TestFixture;


static void
fixture_setup(TestFixture *fixture, gconstpointer user_data)
{
    fixture->manager = lpf_manager_new();
    g_assert_nonnull (fixture->manager);
    fixture->provider = lpf_manager_activate_provider(fixture->manager, LPF_TEST_PROVIDER, NULL);
    g_assert_nonnull (fixture->provider);
}


static void
fixture_teardown(TestFixture *fixture, gconstpointer user_data)
{
    g_assert_nonnull (fixture->manager);
    g_assert_nonnull (fixture->provider);
    lpf_manager_deactivate_provider(fixture->manager, fixture->provider);
    fixture->manager = NULL;
    fixture->provider = NULL;
}


static void
test_got_loc (GSList *locs, gpointer user_data, GError *err)
{
    TestFixture *fixture = user_data;
    LpfLoc *loc;

    g_assert_false (fixture->got_loc_reached);
    fixture->got_loc_reached = TRUE;
    g_main_loop_quit (fixture->loop);
    g_assert_no_error (err);
    g_assert_cmpint (g_slist_length (locs), ==, 1);
    loc = g_slist_nth (locs, 0)->data;
    g_assert_cmpstr ("testloc1", ==, lpf_loc_get_name (loc));
}


static void
test_lpf_loc(TestFixture *fixture, gconstpointer user_data)
{
    fixture->loop = g_main_loop_new (NULL, FALSE);
    g_assert_nonnull (fixture->manager);
    g_assert_nonnull (fixture->provider);

    lpf_provider_get_locs(fixture->provider, "testloc1", 0, test_got_loc, fixture);
    g_main_loop_run (fixture->loop);
    /* Make sure the async functon was called */
    g_assert_true (fixture->got_loc_reached);
}


int main(int argc, char **argv)
{
    gboolean ret;

    g_test_init (&argc, &argv, NULL);
    g_setenv(LPF_PROVIDERS_ENV, LPF_TEST_SRCDIR "/../providers/.libs/", TRUE);

    g_test_add ("/libplanfahr/lpf-loc", TestFixture, NULL,
                fixture_setup, test_lpf_loc, fixture_teardown);

    ret = g_test_run ();
    return ret;
}
