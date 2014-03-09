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

#include "../hafas-bin6.c"

/* Make sure we can parse the station xml list as returned by the current Deutsche Bahn Hafas */
static void
test_parse_locs (void)
{
    GSList *locs;
    LpfLoc *loc;
    char *name;
    double lon, lat;

    char *xml = g_strjoin(NULL,
"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>",
"<ResC xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"No path to XML scheme defined\" ver=\"1.1\" prod=\"String\" lang=\"EN\">",
"  <MLcRes flag=\"FINAL\">",
"    <MLc t=\"ST\" n=\"Erpel(Rhein)\" i=\"A=1@O=Erpel(Rhein)@X=7241593@Y=50582067@U=80@L=008001858@B=1@p=1386184594@\" x=\"7241593\" y=\"50582067\" />",
"    <MLc t=\"ST\" n=\"Erpel B42\" i=\"A=1@O=Erpel B42@X=7231174@Y=50583649@U=81@L=000441204@B=1@p=1387219918@\" x=\"7231174\" y=\"50583649\" />",
"    <MLc t=\"ST\" n=\"Bahnhofstr., Erpel\" i=\"A=1@O=Bahnhofstr., Erpel@X=7243175@Y=50580943@U=81@L=000448602@B=1@p=1387219918@\" x=\"7243175\" y=\"50580943\" />",
"    <MLc t=\"ST\" n=\"Rheinfähre, Erpel\" i=\"A=1@O=Rheinfähre, Erpel@X=7238401@Y=50581663@U=81@L=000441205@B=1@p=1387219918@\" x=\"7238401\" y=\"50581663\" />",
"    <MLc t=\"ST\" n=\"Orsberg Ort, Erpel\" i=\"A=1@O=Orsberg Ort, Erpel@X=7243399@Y=50593061@U=81@L=000454657@B=1@p=1387219918@\" x=\"7243399\" y=\"50593061\" />",
"    <MLc t=\"ST\" n=\"Neutor, Erpel\" i=\"A=1@O=Neutor, Erpel@X=7235282@Y=50584108@U=81@L=000454652@B=1@p=1387219918@\" x=\"7235282\" y=\"50584108\" />",
"    <MLc t=\"ST\" n=\"Erpeldange Am Schlass, Luxemburg\" i=\"A=1@O=Erpeldange Am Schlass, Luxemburg@X=6114741@Y=49852458@U=81@L=000864180@B=1@p=1387219918@\" x=\"6114741\" y=\"49852458\" />",
"  </MLcRes>",
"</ResC>", NULL);

    locs = parse_locs_xml(xml);
#if GLIB_CHECK_VERSION (2, 40, 0)
    g_assert_nonnull (locs);
#else
    g_assert (locs != NULL);
#endif
    g_assert_cmpint (g_slist_length (locs), ==, 7);

    loc = g_slist_nth (locs, 4)->data;
    g_object_get (loc, "name", &name, "long", &lon, "lat", &lat, NULL);

    g_assert_cmpstr (name, ==, "Orsberg Ort, Erpel");
    g_assert_cmpint (lon, ==, 7);
    g_assert_cmpint (lat, ==, 50);

    g_slist_free_full (locs, g_object_unref);
    g_free (xml);
    g_free (name);
}

/* Make sure we can parse the binary data trip information */
static void
test_parse_trips (void)
{
    GSList *trips;
    gchar *binary;
    gsize  length;
    int i;
    GSList *parts;
    LpfTrip *trip;
    LpfTripPart *part;
    LpfLoc *stop;
    gchar *name;
    GDateTime *dep, *arr;

#if GLIB_CHECK_VERSION (2, 38, 0)
    g_assert_true(g_file_get_contents(LPF_TEST_SRCDIR "/hafas-bin-6-station-query-1.bin", &binary, &length, NULL));
#else
    g_assert(g_file_get_contents(LPF_TEST_SRCDIR "/hafas-bin-6-station-query-1.bin", &binary, &length, NULL) == TRUE);
#endif

    trips = hafas_binary_parse_trips (binary, length);

    g_assert (g_slist_length (trips) == 3);

    for (i = 0; i < g_slist_length (trips); i++) {
        trip = LPF_TRIP(g_slist_nth_data (trips, i));
        g_object_get (G_OBJECT(trip), "parts", &parts, NULL);

        part = LPF_TRIP_PART(g_slist_nth_data (parts, 0));
        g_object_get (G_OBJECT(part), "start", &stop, NULL);
        g_object_get (G_OBJECT(stop),
                      "name", &name,
                      "arrival", &arr,
                      "departure", &dep,
                      NULL);
        /* All trips start in Erpel */
        g_assert (!g_strcmp0 (name, "Erpel(Rhein)"));
        g_free (name);

        g_assert (dep != NULL);
        g_assert (arr == NULL);
        g_date_time_unref (dep);

        /* All trips end in Unkel */
        part = LPF_TRIP_PART(g_slist_nth_data (parts,
                                               g_slist_length(parts)-1));
        g_object_get (G_OBJECT(part), "end", &stop, NULL);
        g_object_get (G_OBJECT(stop),
                      "name", &name,
                      "arrival", &arr,
                      "departure", &dep,
                      NULL);

        g_assert (dep == NULL);
        g_assert (arr != NULL);
        g_date_time_unref (arr);

        g_assert (g_strrstr (name, "Unkel") != NULL);
        g_free (name);
    }

    g_slist_free (trips);

}


int main(int argc, char **argv)
{
    gboolean ret;
#if ! GLIB_CHECK_VERSION(2, 36, 0)
    g_type_init ();
#endif
    g_test_init (&argc, &argv, NULL);

    g_test_add_func ("/providers/de-db/parse_stations", test_parse_locs);
    g_test_add_func ("/providers/de-db/parse_trips", test_parse_trips);

    ret = g_test_run ();
    return ret;
}
