/*
 * hafas-bin6.c: test hafas binary v6 parser
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

#include <glib.h>

#include "../hafas-bin6.h"


/* sanity check our parser's structures */
static void
test_sizes (void)
{
    g_assert_cmpint(sizeof(HafasBin6Header), ==, 0x4a);
    g_assert_cmpint(sizeof(HafasBin6Trip), ==, 0x0c);
    g_assert_cmpint(sizeof(HafasBin6Loc), ==, 0x0e);
    g_assert_cmpint(sizeof(HafasBin6ExtHeader), ==, 0x30);
    g_assert_cmpint(sizeof(HafasBin6TripDetailsHeader), ==, 0x0e);
    g_assert_cmpint(sizeof(HafasBin6Station), ==, 0x0e);
    g_assert_cmpint(sizeof(HafasBin6ServiceDay), ==, 0x07);
    g_assert_cmpint(sizeof(HafasBin6TripDetail), ==, 0x04);
    g_assert_cmpint(sizeof(HafasBin6TripPart), ==, 0x14);
    g_assert_cmpint(sizeof(HafasBin6TripPartDetail), ==, 0x10);
    g_assert_cmpint(sizeof(HafasBin6TripStop), ==, 0x1a);
}

/*
 * Make sure we can parse the binary data trip information This is
 * just to test the raw parser. The wrapping into Lpf objects and
 * conversions of e.g. time and date are tested separately.
 */
static void
test_parse_erpel_unkel(void)
{
    gchar *bin;
    gsize  length;
    HafasBin6Loc *start, *end;
    HafasBin6ExtHeader *ext;
    HafasBin6TripDetailsHeader *detail_header;
    HafasBin6TripPartDetail *part_detail;
    HafasBin6TripDetail *detail;
    HafasBin6Trip *trip;
    HafasBin6TripPart *part;
    HafasBin6ServiceDay *day;
    HafasBin6TripStop *stop;
    HafasBin6Station *station;
    gint i, j, k;
    guint expected_changes[3] = { 0, 0, 0 };
    guint expected_parts[3] = { 1, 2, 1 };
    const gchar *expected_part_line[3][2] = { {"RB 12560", "doesnot" },
                                              {"Fussweg", "Bus  565" },
                                              {"RB 12562", "matter" }};
    const gint expected_part_dep[3][2] = { { 956,    0},
                                           { 958, 1003 },
                                           {1056,    0 }};

    const gint expected_part_arr[3][2] = { { 959, 2},
                                           {1003, 1014},
                                           {1059,    0 }};
    guint expected_stops[3][2] = { {0, 0}, {0, 8}, {0, 0} };

    g_assert_true(g_file_get_contents(LPF_TEST_SRCDIR "/hafas-bin-6-station-query-1.bin", &bin, &length, NULL));

    g_assert_cmpint(HAFAS_BIN6_HEADER(bin)->num_trips, ==, 3);

    /* header information */
    start = HAFAS_BIN6_START(bin);
    end = HAFAS_BIN6_END(bin);
    g_assert_cmpint (start->lon, ==, 7241593);
    g_assert_cmpint (start->lat, ==, 50582067);
    g_assert_cmpint (start->type, ==, HAFAS_BIN6_LOC_TYPE_STATION);
    g_assert_cmpint (end->lon, ==, 7219803);
    g_assert_cmpint (end->lat, ==, 50602742);
    g_assert_cmpint (end->type, ==, HAFAS_BIN6_LOC_TYPE_STATION);

    /* ext header information */
    ext = HAFAS_BIN6_EXT_HEADER(bin);
    g_assert_cmpint (ext->err, ==, HAFAS_BIN6_ERROR_NONE);
    g_assert_cmpstr (HAFAS_BIN6_STR(bin, ext->enc_off), ==,  "iso-8859-1");
    g_assert_cmpstr (HAFAS_BIN6_STR(bin, ext->req_id_off), ==,  "50.02519042.1387923122");
    g_assert_cmpint (ext->seq, ==,  1);

    /* detail header information */
    detail_header = HAFAS_BIN6_TRIP_DETAILS_HEADER(bin);
    g_assert_cmpint (detail_header->version, ==, 1);
    g_assert_cmpint (detail_header->stop_size, ==, sizeof(HafasBin6TripStop));
    g_assert_cmpint (detail_header->part_detail_size, == ,sizeof(HafasBin6TripPartDetail));

    for (i = 0; i < HAFAS_BIN6_HEADER(bin)->num_trips; i++) {
        trip = HAFAS_BIN6_TRIP(bin, i);
        g_assert_cmpint (trip->changes, ==, expected_changes[i]);
        g_assert_cmpint (trip->part_cnt, ==, expected_parts[i]);

        day = HAFAS_BIN6_SERVICE_DAY(bin, i);
        g_assert_cmpint (day->byte_base, ==, 0);
        g_assert_cmpint (day->byte_len, ==, 2);
        g_assert_cmpint (day->byte0, ==, (gchar)(0x80));

        detail = HAFAS_BIN6_TRIP_DETAIL(bin, i);
        g_assert_cmpint (detail->rt_status, ==, HAFAS_BIN6_RTSTATUS_NORMAL);
        g_assert_cmpint (detail->delay, ==, 0);

        for (j = 0; j < trip->part_cnt; j++) {
            /* planned departures and arrivals */
            part = HAFAS_BIN6_TRIP_PART(bin, i, j);
            g_assert_cmpstr (HAFAS_BIN6_STR(bin, part->line_off), ==, expected_part_line[i][j]);
            g_assert_cmpint (part->arr, ==, expected_part_arr[i][j]);
            g_assert_cmpint (part->dep, ==, expected_part_dep[i][j]);
            /* predicted departures and arrivals */
            part_detail = HAFAS_BIN6_TRIP_PART_DETAIL(bin, i, j);
            g_assert_cmpint (part_detail->arr_pred, ==, 65535);
            g_assert_cmpint (part_detail->dep_pred, ==, 65535);
            /* stops */
            g_assert_cmpint (part_detail->stops_cnt, ==, expected_stops[i][j]);
            for (k = 0; k < part_detail->stops_cnt; k++) {
                stop = HAFAS_BIN6_STOP(bin, i, j, k);
                g_assert_cmpstr (HAFAS_BIN6_STR(bin, stop->dep_pos_pred_off), ==, "---");
            }
            /* check a single station more thoroughly */
            stop = HAFAS_BIN6_STOP(bin, 1, 1, 2);
            station = HAFAS_BIN6_STATION(bin, stop->stop_idx);
            g_assert_cmpstr (HAFAS_BIN6_STR(bin, station->name_off), ==, "Heister Kapelle, Unkel");
        }
    }
}


int main(int argc, char **argv)
{
    gboolean ret;
    g_test_init (&argc, &argv, NULL);

    g_test_add_func ("/providers/hafas-bin6/sizes", test_sizes);
    g_test_add_func ("/providers/hafas_bin6/parse_erpel_unkel", test_parse_erpel_unkel);

    ret = g_test_run ();
    return ret;
}
