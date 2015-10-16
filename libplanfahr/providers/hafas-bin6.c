/*
 * lpf-provider-hafas-bin6.c: HAFAS Binary Format Version 6 provider
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

#include <config.h>

#include <string.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <libsoup/soup.h>

#include "hafas-bin6.h"
#include "lpf-loc.h"
#include "lpf-priv.h"
#include "lpf-provider.h"
#include "lpf-trip.h"
#include "lpf-stop.h"
#include "lpf-trip-part.h"


static void lpf_provider_hafas_bin6_interface_init (LpfProviderInterface *iface);

G_DEFINE_TYPE_WITH_CODE (LpfProviderHafasBin6, lpf_provider_hafas_bin6, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (LPF_TYPE_PROVIDER, lpf_provider_hafas_bin6_interface_init));

#define PROVIDER_NAME "hafas_bin6"

enum {
    PROP_0,
    PROP_NAME,
    LAST_PROP
};

/* transfers data between invocation and the passed in callback */
typedef struct _LpfProviderGotItUserData {
    LpfProvider *self;
    gpointer callback;
    gpointer user_data;
} LpfProviderGotItUserData;

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), LPF_TYPE_PROVIDER_HAFAS_BIN6, LpfProviderHafasBin6Private))


typedef struct _LpfProviderHafasBin6Private LpfProviderHafasBin6Private;

struct _LpfProviderHafasBin6Private {
    SoupSession *session;
    char *logdir;
    gboolean debug;
};


const gchar*
lpf_provider_hafas_bin6_locs_url(LpfProviderHafasBin6 *self)
{
    g_return_val_if_fail (LPF_IS_PROVIDER_HAFAS_BIN6 (self), NULL);

    if (LPF_PROVIDER_HAFAS_BIN6_GET_CLASS (self)->locs_url != NULL)
        return LPF_PROVIDER_HAFAS_BIN6_GET_CLASS (self)->locs_url(self);
    else
        g_warning ("Class '%s' does not override the mandatory "
                   ".locs_url() virtual function.",
                   G_OBJECT_TYPE_NAME (self));
    return NULL;
}


const gchar*
lpf_provider_hafas_bin6_trips_url(LpfProviderHafasBin6 *self)
{
    g_return_val_if_fail (LPF_IS_PROVIDER_HAFAS_BIN6 (self), NULL);

    if (LPF_PROVIDER_HAFAS_BIN6_GET_CLASS (self)->trips_url != NULL)
        return LPF_PROVIDER_HAFAS_BIN6_GET_CLASS (self)->trips_url(self);
    else
        g_warning ("Class '%s' does not override the mandatory "
                   ".trips_url() virtual function.",
                   G_OBJECT_TYPE_NAME (self));
    return NULL;
}


static GSList*
parse_locs_xml (const char *xml)
{
    gint i = 0;
    xmlDocPtr doc = NULL;
    xmlChar *xpath = (xmlChar*) "//MLc[@t=\"ST\"]";
    xmlXPathContextPtr context = NULL;
    xmlXPathObjectPtr result = NULL;
    xmlNodeSetPtr nodeset = NULL;
    gchar *name, *x, *y;
    gdouble lo, la;
    LpfLoc *loc;
    char *id;
    GSList *locs = NULL;

    g_return_val_if_fail (xml, NULL);

    LPF_DEBUG("%s", xml);
    doc = xmlParseMemory(xml, strlen(xml));
    if (doc == NULL ) {
        g_warning("Document not parsed successfully");
        return NULL;
    }

    context = xmlXPathNewContext(doc);
    if (context == NULL) {
        g_warning("Error in xmlXPathNewContext\n");
        goto out;
    }

    result = xmlXPathEvalExpression(xpath, context);
    if (result == NULL) {
        g_warning("Error in xmlXPathEvalExpression\n");
        goto out;
    }

    if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
        g_warning("No result\n");
        goto out;
    }

    nodeset = result->nodesetval;
    for (i=0; i < nodeset->nodeNr; i++) {
        name = (gchar*) xmlGetProp(nodeset->nodeTab[i], BAD_CAST "n");
        x = (gchar*) xmlGetProp(nodeset->nodeTab[i], BAD_CAST "x");
        y = (gchar*) xmlGetProp(nodeset->nodeTab[i], BAD_CAST "y");
        id = (gchar*) xmlGetProp(nodeset->nodeTab[i], BAD_CAST "i");

        lo = (gdouble) g_ascii_strtod(x, NULL) / 1000000.0;
        la = (gdouble) g_ascii_strtod(y, NULL) / 1000000.0;

        LPF_DEBUG ("%s (%lf, %lf) - %s", name, lo, la, id);

        loc = (LpfLoc*) g_object_new (LPF_TYPE_LOC, "name", name, "long", lo, "lat", la, NULL);
        lpf_loc_set_opaque (loc, id);
        locs = g_slist_append(locs, loc);
        g_free(x);
        g_free(y);
    }
 out:
     xmlXPathFreeContext(context);
     xmlXPathFreeObject(result);
     xmlFreeDoc(doc);
     return locs;
}


static void
log_response_body(LpfProviderHafasBin6 *self, SoupMessage *msg, const char* type)
{
    LpfProviderHafasBin6Private *priv = GET_PRIVATE(self);
    gchar *querylog = NULL;
    gchar *filename = NULL;
    gchar *logtime = NULL;
    GDateTime *now;

    if (G_LIKELY(!priv->debug))
        return;

    now = g_date_time_new_now_local();

    logtime = g_date_time_format (now, "%F_%T");
    filename = g_strdup_printf ("%s-%s.body", type, logtime);

    querylog = g_build_path (G_DIR_SEPARATOR_S,
                             priv->logdir,
                             filename,
                             NULL);

    if (!g_file_set_contents (querylog, msg->response_body->data, msg->response_body->length, NULL))
        g_warning ("Failed to write out query contents to %s", querylog);

    g_free (querylog);
    g_free (filename);
    g_free (logtime);
    g_date_time_unref(now);
}


static void
got_locs (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
    GSList *locs = NULL;
    LpfProviderGotItUserData *locs_data = (LpfProviderGotItUserData*)user_data;
    LpfProviderGotLocsNotify callback;
    LpfProviderHafasBin6 *self;
    gpointer data;
    GError *err = NULL;

    g_return_if_fail(session);
    g_return_if_fail(msg);
    g_return_if_fail(user_data);

    callback = locs_data->callback;
    data = locs_data->user_data;
    self = LPF_PROVIDER_HAFAS_BIN6(locs_data->self);
    g_free (locs_data);

    LPF_DEBUG("Status: %d", msg->status_code);
    if (!SOUP_STATUS_IS_SUCCESSFUL(msg->status_code)) {
        LPF_DEBUG("HTTP request failed");
        g_set_error (&err,
                     LPF_PROVIDER_ERROR,
                     LPF_PROVIDER_ERROR_REQUEST_FAILED,
                     "HTTP request failed: %d", msg->status_code);
        goto out;
    }

    log_response_body (self, msg, "station");

    if ((locs = parse_locs_xml(msg->response_body->data)) == NULL) {
        g_set_error (&err,
                     LPF_PROVIDER_ERROR,
                     LPF_PROVIDER_ERROR_PARSE_FAILED,
                     "Failed to parse locations");
        goto out;
    }

out:
    (*callback)(locs, data, err);
}


static gint
lpf_provider_hafas_bin6_get_locs (LpfProvider *self, const char* match, LpfProviderGetLocsFlags flags, LpfProviderGotLocsNotify callback, gpointer user_data)
{
    LpfProviderHafasBin6Private *priv = GET_PRIVATE(self);
    SoupMessage *msg;
    char *xml;
    LpfProviderGotItUserData *locs_data = NULL;
    gint ret = -1;

    g_return_val_if_fail (priv->session, -1);
    g_return_val_if_fail (!flags, -1);

    locs_data = g_try_malloc(sizeof(LpfProviderGotItUserData));
    if (!locs_data)
        goto out;
    xml = g_strdup_printf ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                           "<ReqC ver=\"1.1\" prod=\"String\" lang=\"EN\">"
                           "<MLcReq><MLc n=\"%s\" t=\"ST\"/>"
                           "</MLcReq></ReqC>", match);

    msg = soup_message_new ("POST", lpf_provider_hafas_bin6_locs_url(LPF_PROVIDER_HAFAS_BIN6(self)));
    if (!msg)
        goto out;
    soup_message_set_request (msg, "text/xml", SOUP_MEMORY_TAKE, xml, strlen (xml));

    locs_data->user_data = user_data;
    locs_data->callback = callback;
    locs_data->self = self;

    soup_session_queue_message (priv->session, msg, got_locs, locs_data);
    ret = 0;
 out:
    if (ret < 0)
        g_free (locs_data);
    return ret;
}


static GSList*
hafas_bin6_parse_each_trip (const gchar *data, gsize num, guint base, const char *enc)
{
    gint i, j, k;
    const HafasBin6Trip *t;
    const HafasBin6TripPartDetail *pd;
    const HafasBin6TripPart *p;
    const HafasBin6TripStop *stop;
    guint16 day_off;

#ifdef ENABLE_DEBUG
    const HafasBin6TripDetail *d;
#endif
    guint h, m;
    LpfTrip *trip = NULL;
    LpfTripPart *part = NULL;
    LpfStop *start = NULL, *end = NULL, *astop = NULL;
    LpfTripStatusFlags status;
    GDateTime *dt;
    GSList *trips = NULL, *parts = NULL, *stops = NULL;
    const char *line;

    for (i = 0; i < num; i++) {
        status = LPF_TRIP_STATUS_FLAGS_NONE;

        /* The trips itself */
        t = HAFAS_BIN6_TRIP(data, i);
        day_off = lpf_provider_hafas_bin6_parse_service_day(data, i);

        LPF_DEBUG("Trip #%d, Changes:            %4d", i, t->changes);
#ifdef ENABLE_DEBUG
        /* trip details */
        d = HAFAS_BIN6_TRIP_DETAIL(data, i);
        LPF_DEBUG("Trip #%d, Status:             %4d", i, d->rt_status);
        LPF_DEBUG("Trip #%d, Delay:              %4d", i, d->delay);
#endif
        /* trip parts */
        for (j = 0; j < t->part_cnt; j++) {
            p = HAFAS_BIN6_TRIP_PART(data, i, j);
            start = g_object_new(LPF_TYPE_STOP, NULL);
            if (lpf_provider_hafas_bin6_parse_station(data, p->dep_off, LPF_LOC(start), enc) < 0) {
                g_warning("Failed to parse start station %d/%d", i, j);
                goto error;
            }
            end = g_object_new(LPF_TYPE_STOP, NULL);
            if (lpf_provider_hafas_bin6_parse_station(data, p->arr_off, LPF_LOC(end), enc) < 0) {
                g_warning("Failed to parse end station %d/%d", i, j);
                goto error;
            }
            line = HAFAS_BIN6_STR(data, p->line_off);

            h = p->dep / 100;
            m = p->dep % 100;
            dt = lpf_provider_hafas_bin6_date_time (base, day_off, h, m);
            g_object_set (start, "departure", dt, NULL);

            if (g_strcmp0 (HAFAS_BIN6_NO_PLATFORM, HAFAS_BIN6_STR(data, p->dep_pos_off))) {
                g_object_set (start,
                              "dep_plat", HAFAS_BIN6_STR(data, p->dep_pos_off),
                              NULL);
            }

            h = p->arr / 100;
            m = p->arr % 100;
            dt = lpf_provider_hafas_bin6_date_time (base, day_off, h, m);
            g_object_set (end, "arrival", dt, NULL);

            if (g_strcmp0 (HAFAS_BIN6_NO_PLATFORM, HAFAS_BIN6_STR(data, p->arr_pos_off))) {
                g_object_set (end,
                              "arr_plat", HAFAS_BIN6_STR(data, p->arr_pos_off),
                              NULL);
            }

            /* trip part details */
            pd = HAFAS_BIN6_TRIP_PART_DETAIL(data, i, j);

            LPF_DEBUG("Trip-Part #%d, Flags:            %4d", j, pd->flags);
            if (pd->flags & HAFAS_BIN6_PART_DETAIL_FLAGS_CANCELED_MASK) {
                status = LPF_TRIP_STATUS_FLAGS_CANCELED;
            }

            if (pd->arr_pred != HAFAS_BIN6_NO_REALTIME) {
                h = pd->arr_pred / 100;
                m = pd->arr_pred % 100;
                dt = lpf_provider_hafas_bin6_date_time (base, day_off, h, m);
                g_object_set (end, "rt_arrival", dt, NULL);
            }

            if (pd->dep_pred != HAFAS_BIN6_NO_REALTIME) {
                h = pd->dep_pred / 100;
                m = pd->dep_pred % 100;
                dt = lpf_provider_hafas_bin6_date_time (base, day_off, h, m);
                g_object_set (start, "rt_departure", dt, NULL);
            }

            LPF_DEBUG("Trip #%d, part #%d, Pred. Dep Plat: %s, Pred. Arr Plat: %s", i, j,
                      HAFAS_BIN6_STR(data, pd->dep_pos_pred_off),
                      HAFAS_BIN6_STR(data, pd->arr_pos_pred_off));

            for (k = 0; k < pd->stops_cnt; k++) {
                stop = HAFAS_BIN6_STOP(data, i, j, k);

                astop = g_object_new (LPF_TYPE_STOP,
                                      "arrival", lpf_provider_hafas_bin6_date_time (base,
                                                                                    day_off,
                                                                                    stop->arr / 100,
                                                                                    stop->arr % 100),
                                      "departure", lpf_provider_hafas_bin6_date_time (base,
                                                                                      day_off,
                                                                                      stop->dep / 100,
                                                                                      stop->dep % 100),
                                      NULL
                    );

                if (lpf_provider_hafas_bin6_parse_station(data, stop->stop_idx, LPF_LOC(astop), enc) < 0) {
                    g_warning("Failed to parse stop %d/%d", i, j);
                    goto error;
                }

#ifdef ENABLE_DEBUG
                /* FIXME: add these and predicted dep/arr/plat to LpfStop too */
                LPF_DEBUG("Trip #%d, part #%d,"
                          "Dep: %.6s Arr: %.6s", i, j,
                          HAFAS_BIN6_STR(data, stop->dep_pos_off),
                          HAFAS_BIN6_STR(data, stop->arr_pos_off));
#endif
                stops = g_slist_append (stops, astop);
                astop = NULL;
            }
            part = g_object_new (LPF_TYPE_TRIP_PART,
                                 "start", start,
                                 "end", end,
                                 "line", line,
                                 "stops", stops,
                                 NULL);
            start = end = NULL;
            line = NULL;
            stops = NULL;

            parts = g_slist_append (parts, part);
            part = NULL;
        }

        trip = g_object_new (LPF_TYPE_TRIP,
                             "parts", parts,
                             "status", status,
                             NULL);
        parts = NULL;
        trips = g_slist_append (trips, trip);
        trip = NULL;
    }

    return trips;

error:
    if (trips)
        g_slist_free_full (trips, g_object_unref);
    if (parts)
        g_slist_free_full (parts, g_object_unref);
    if (stops)
        g_slist_free_full (stops, g_object_unref);
    if (start)
        g_object_unref (start);
    if (trip)
        g_object_unref (trip);
    if (part)
        g_object_unref (part);
    if (astop)
        g_object_unref (astop);

    return NULL;
}


static GSList*
hafas_binary_parse_trips (const char *data, gsize length, GError **err)
{
    HafasBin6Header *header;
#ifdef ENABLE_DEBUG
    HafasBin6Loc *start, *end;
#endif
    HafasBin6ExtHeader *ext;
    HafasBin6TripDetailsHeader *details;
    GSList *trips = NULL;
    guint16 version;
    const gchar *encoding;

    g_return_val_if_fail (data, NULL);
    g_return_val_if_fail (length, NULL);

    version = *(guint16*)data;
    if (version != 6) {
        g_set_error (err,
                     LPF_PROVIDER_ERROR,
                     LPF_PROVIDER_ERROR_PARSE_FAILED,
                     "Incorrect Hafas Binary version %d", version);
        goto out;
    }

    g_return_val_if_fail(sizeof (HafasBin6Header) < length, NULL);
    header = (HafasBin6Header*) data;
#ifdef ENABLE_DEBUG
    start = (HafasBin6Loc*)&header->start;
    end = (HafasBin6Loc*)&header->end;
#endif
    LPF_DEBUG("%d Trips from '%s' to '%s'",
              header->num_trips,
              HAFAS_BIN6_STR(data, start->name_off),
              HAFAS_BIN6_STR(data, end->name_off));

    g_return_val_if_fail ((sizeof (HafasBin6Header) +
                           sizeof(HafasBin6Header) * header->num_trips) < length,
                          NULL);

    ext = HAFAS_BIN6_EXT_HEADER(data);
    g_return_val_if_fail (header->ext +
                          sizeof(HafasBin6ExtHeader) < length, NULL);

    /* We might not have attrs_index0 */
    g_return_val_if_fail(sizeof (HafasBin6ExtHeader) - 1 < ext->length, NULL);

    LPF_DEBUG("Ext length:       0x%.4x", ext->length);
    LPF_DEBUG("Errors:           0x%.4x", ext->err);
    LPF_DEBUG("Sequence:         0x%.4x", ext->seq);
    LPF_DEBUG("Detail table:     0x%.4x", ext->details_tbl);
    encoding = HAFAS_BIN6_STR(data,  ext->enc_off);
    LPF_DEBUG("Encoding: %s", encoding);
    LPF_DEBUG("Request Id: %s", HAFAS_BIN6_STR(data, ext->req_id_off));

    if (ext->err) {
        g_set_error (err,
                     LPF_PROVIDER_ERROR,
                     LPF_PROVIDER_ERROR_PARSE_FAILED,
                     "Hafas Blob has error code %d", ext->err);
        goto out;
    }

    if (ext->seq <= 0) {
        g_set_error (err,
                     LPF_PROVIDER_ERROR,
                     LPF_PROVIDER_ERROR_PARSE_FAILED,
                     "Illegal sequence number %d", ext->seq);
        goto out;
    }

    g_return_val_if_fail (ext->details_tbl +
                          sizeof (HafasBin6TripDetailsHeader) < length, NULL);
    details = HAFAS_BIN6_TRIP_DETAILS_HEADER(data);
    LPF_DEBUG("Trip detail version: %d", details->version);
    g_return_val_if_fail (details->version == 1, NULL);
    g_return_val_if_fail (details->stop_size == sizeof(HafasBin6TripStop), NULL);
    g_return_val_if_fail (details->part_detail_size == sizeof(HafasBin6TripPartDetail), NULL);

    trips = hafas_bin6_parse_each_trip (data, header->num_trips, header->days, encoding);
    g_return_val_if_fail (trips, NULL);
 out:
    return trips;
}

static gint
decompress (const gchar *in, gsize inlen,
            gchar **out, gsize *outlen,
            GError **err) {
    gint ret = -1;
    gsize read_, written;
    GConverter *decomp = NULL;
    GConverterResult conv;
    gsize outbuflen, buflen, outpos = 0, inpos = 0;
    gchar *outbuf = NULL;

    g_return_val_if_fail (inlen > 0, ret);
    g_return_val_if_fail (in, ret);
    g_return_val_if_fail (err, ret);

    decomp = (GConverter *)g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_GZIP);

    outbuflen = buflen = inlen * 2;
    outbuf = g_try_malloc (outbuflen);
    if (outbuf == NULL)
        goto out;

    do {
        conv = g_converter_convert (decomp,
                                    (in + inpos), inlen,
                                    (outbuf + outpos), buflen,
                                    G_CONVERTER_INPUT_AT_END,
                                    &read_, &written,
                                    err);

        switch (conv) {
        case G_CONVERTER_ERROR:
                goto out;
        case G_CONVERTER_FINISHED:
            outpos += written;
            ret = 0;
            goto out;
        case G_CONVERTER_CONVERTED:
            outpos += written;
            inpos += read_;
            inlen -= read_;
            if (outbuflen - written < buflen) {
                outbuflen += buflen;
                outbuf = g_try_realloc (outbuf, outbuflen);
                if (outbuf == NULL)
                    goto out;
            }
            break;
        case G_CONVERTER_FLUSHED:
        default:
            g_warning ("Unhandled condition %d", conv);
            goto out;
        }
    } while (TRUE);

out:
    if (ret == 0) {
        *out = outbuf;
        *outlen = outpos;
    } else
        g_free (outbuf);

    g_object_unref (decomp);
    return ret;
}

static void
got_trips (SoupSession *session, SoupMessage *msg, gpointer user_data)
{
    GSList *trips = NULL;
    LpfProviderGotItUserData *trips_data = (LpfProviderGotItUserData*)user_data;
    LpfProviderGotTripsNotify callback;
    LpfProviderHafasBin6 *self;
    gpointer data;
    gchar *decomp = NULL;
    gsize len;
    GError *err = NULL;

    g_return_if_fail(session);
    g_return_if_fail(msg);
    g_return_if_fail(user_data);

    g_object_ref (msg);

    callback = trips_data->callback;
    data = trips_data->user_data;
    self = (LpfProviderHafasBin6*)trips_data->self;
    g_free (trips_data);

    LPF_DEBUG("Status: %d", msg->status_code);
    if (!SOUP_STATUS_IS_SUCCESSFUL(msg->status_code)) {
        g_set_error (&err,
                     LPF_PROVIDER_ERROR,
                     LPF_PROVIDER_ERROR_REQUEST_FAILED,
                     "HTTP request failed: %d", msg->status_code);
        goto out;
    }

    log_response_body (self, msg, "trip");

    if (decompress(msg->response_body->data, msg->response_body->length, &decomp, &len, &err) < 0) {
        goto out;
    }

    LPF_DEBUG("Decompressed to %" G_GSIZE_FORMAT " bytes", len);
    if ((trips = hafas_binary_parse_trips(decomp, len, &err)) == NULL) {
        if (err == NULL) {
            g_set_error (&err,
                         LPF_PROVIDER_ERROR,
                         LPF_PROVIDER_ERROR_PARSE_FAILED,
                         "Failed to parse trips - unknown error");
        }
        goto out;
    }

out:
    g_object_unref (msg);
    g_free (decomp);

    (*callback)(trips, data, err);
}


static gint
lpf_provider_hafas_bin6_get_trips (LpfProvider *self,
                                   LpfLoc *start,
                                   LpfLoc *end,
                                   GDateTime *date,
                                   LpfProviderGetTripsFlags flags,
                                   LpfProviderGotTripsNotify callback,
                                   gpointer user_data)
{
    LpfProviderHafasBin6Private *priv = GET_PRIVATE(self);
    SoupMessage *msg;
    SoupURI *uri = NULL;
    LpfProviderGotItUserData *trips_data = NULL;
    gint ret = -1;
    gchar *datestr = NULL, *timestr = NULL;
    /* allowed vehicle types */
    const gchar *train_restriction = "11111111111111";
    /* whether time is arrival or departure time */
    const gchar *by_departure;
    char *start_id = NULL, *end_id = NULL;

    g_return_val_if_fail (start, -1);
    g_return_val_if_fail (end, -1);
    g_return_val_if_fail (callback, -1);
    g_return_val_if_fail (priv->session, -1);
    g_return_val_if_fail (date, -1);

    g_object_ref (start);
    g_object_ref (end);
    g_date_time_ref (date);

    trips_data = g_try_malloc(sizeof(LpfProviderGotItUserData));
    if (!trips_data)
        goto out;

    datestr = g_date_time_format (date, "%d.%m.%y");
    timestr = g_date_time_format (date, "%H:%M");

    start_id = lpf_loc_get_opaque(start);
    end_id = lpf_loc_get_opaque(end);
    if (start_id == NULL || end_id == NULL) {
        g_warning ("Details missing.");
        goto out;
    }
    by_departure = (flags & LPF_PROVIDER_GET_TRIPS_ARRIVAL) ? "0" : "1";

    uri = soup_uri_new (lpf_provider_hafas_bin6_trips_url(LPF_PROVIDER_HAFAS_BIN6(self)));
    soup_uri_set_query_from_fields (uri,
                                    "start", "Suchen",
                                    "REQ0JourneyStopsS0ID", start_id,
                                    "REQ0JourneyStopsZ0ID", end_id,
                                    "REQ0JourneyDate", datestr,
                                    "REQ0JourneyTime", timestr,
                                    "REQ0HafasSearchForw", by_departure,
                                    "REQ0JourneyProduct_prod_list_1", train_restriction,
                                    "h2g-direct", "11",
                                    "clientType", "ANDROID",
                                    NULL);

    LPF_DEBUG ("URI: %s", soup_uri_to_string (uri, FALSE));

    msg = soup_message_new_from_uri ("GET", uri);
    if (!msg)
        goto out;

    trips_data->user_data = user_data;
    trips_data->callback = callback;
    trips_data->self = self;

    soup_session_queue_message (priv->session, msg, got_trips, trips_data);
    ret = 0;
 out:
    soup_uri_free (uri);
    g_free (datestr);
    g_free (timestr);
    g_object_unref (start);
    g_object_unref (end);
    g_date_time_unref (date);
    if (ret < 0)
        g_free (trips_data);
    return ret;
}


static void
lpf_provider_hafas_bin6_set_property (GObject *object, guint prop_id,
                                      const GValue *value, GParamSpec *pspec)
{
    switch (prop_id) {
    case PROP_NAME:
	g_warn_if_reached ();
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}


static void
lpf_provider_hafas_bin6_get_property (GObject *object, guint prop_id,
                                      GValue *value, GParamSpec *pspec)
{
    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string (value, PROVIDER_NAME);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

gint
lpf_provider_hafas_bin6_parse_station(const gchar *data, guint16 off, LpfLoc *loc, const char *enc)
{
    gchar *name;
    HafasBin6Station *station;
    gdouble lon, lat;
    gint ret = -1;

    station = HAFAS_BIN6_STATION(data, off);

    name = g_convert (HAFAS_BIN6_STR(data, station->name_off),
                      -1,
                      "utf-8",
                      enc,
                      NULL, NULL, NULL);
    if (name == NULL) {
        g_warning ("Failed to convert station name at %d", off);
        goto err;
    }
    lon = HAFAS_BIN6_LL_DOUBlE (station->lon);
    lat = HAFAS_BIN6_LL_DOUBlE (station->lat);

    LPF_DEBUG("name: %s", name);
    g_object_set (loc, "name", name, "long", lon, "lat", lat,
                  NULL);
    ret = 0;
err:
    g_free (name);
    return ret;
}


/**
 * lpf_provicer_hafas_bin6_parse_service_day:
 *
 * Parse a servide day entry and return the offset from the base date in days.
 */
guint
lpf_provider_hafas_bin6_parse_service_day (const char *data, int idx)
{
    gint i;
    gchar bits;
    guint off;
    const HafasBin6ServiceDay *s;

    s = HAFAS_BIN6_SERVICE_DAY(data, idx);
    off = s->byte_base * 8;

    /* look at all service bytes */
    for (i = 0; i < s->byte_len; i++) {
        bits = (&(s->byte0))[i];
        if (bits == 0) { /* zero means +8 days */
            off += 8;
            continue;
        }

        /* count leading zeros meaning +1 day */
        while ((bits & 0x80) == 0) {
            bits <<= 1;
            off++;
        }
        break;
    }
    return off;
}

/**
 * lpf_provider_hafas_bin6_date_time:
 * @base_days: day off set from 1980-01-01
 * @off_days: day offset from base_days
 * @hours: hour trip starts/ends
 * @minutes: minute the trip starts/ends
 *
 * Calculate date and time from hafas bin 6 input
 *
 * Returns: the travel date and time as #GDateTime
 */
GDateTime*
lpf_provider_hafas_bin6_date_time(guint base_days, guint off_days, guint hours, guint min)
{
    GDateTime *dt, *base = g_date_time_new_utc (1979, 12, 31, 0, 0, 0);

    dt = g_date_time_add (base,
                          (base_days + off_days) * G_TIME_SPAN_DAY +
                          hours * G_TIME_SPAN_HOUR +
                          min * G_TIME_SPAN_MINUTE);
    g_date_time_unref (base);
    return dt;
}

static void
lpf_provider_hafas_bin6_activate (LpfProvider *self, GObject *obj)
{
    LpfProviderHafasBin6Private *priv = GET_PRIVATE(self);
    GFile *dir;
    gchar *debugstr;

#ifdef HAVE_SOUP_SESSION_NEW
    priv->session = soup_session_new();
#else
    priv->session = soup_session_async_new();
#endif
    priv->logdir = g_build_path(G_DIR_SEPARATOR_S,
                                g_get_user_cache_dir(),
                                PACKAGE,
                                lpf_provider_get_name (self),
                                NULL);

    debugstr = getenv ("LPF_DEBUG");
    if (debugstr && strstr (debugstr, "provider"))
        priv->debug = TRUE;

    g_object_set (G_OBJECT (priv->session),
                  SOUP_SESSION_PROXY_RESOLVER,
                  g_proxy_resolver_get_default(),
                  NULL);

    dir = g_file_new_for_path (priv->logdir);
    g_file_make_directory_with_parents (dir, NULL, NULL);
    g_object_unref (dir);

}


static void
lpf_provider_hafas_bin6_deactivate (LpfProvider *self, GObject *obj)
{
    LpfProviderHafasBin6Private *priv = GET_PRIVATE(self);

    if (priv->session)
        g_object_unref (priv->session);

    g_free (priv->logdir);
}


static void
lpf_provider_hafas_bin6_class_init (LpfProviderHafasBin6Class *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (LpfProviderHafasBin6Private));

    object_class->get_property = lpf_provider_hafas_bin6_get_property;
    object_class->set_property = lpf_provider_hafas_bin6_set_property;

    klass->locs_url = lpf_provider_hafas_bin6_locs_url;
    klass->trips_url = lpf_provider_hafas_bin6_trips_url;

    g_object_class_override_property (object_class,
                                      PROP_NAME,
                                      "name");
}

static void
lpf_provider_hafas_bin6_interface_init (LpfProviderInterface *iface)
{
    /* abstract base class */
    iface->activate = lpf_provider_hafas_bin6_activate;
    iface->deactivate = lpf_provider_hafas_bin6_deactivate;

    /* To be implemented by each provider */
    iface->get_locs = lpf_provider_hafas_bin6_get_locs;
    iface->get_trips = lpf_provider_hafas_bin6_get_trips;
}

static void
lpf_provider_hafas_bin6_init (LpfProviderHafasBin6 *self)
{
}
