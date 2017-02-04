/*
 * hafas-bin6-format.h Hafas Binary 6 format
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

#ifndef _HAFAS_BIN6_FORMAT_H
#define _HAFAS_BIN6_FORMAT_H

/**
 * Hafas Binary format version 6
 *
 * The binary format uses several headers that mark contain the start
 * different tables (*_tbl variables). An byte offset into such a table
 * is marked by a variable ending in _off. If the elements in a table
 * are fixed size (array) indexes are used. These end in _idx.
 *
 * Based on AbstractHafasProvider in public-transport-enabler and
 *
 */

#pragma GCC diagnostic ignored "-Wattributes"

/**
 * HafasBin6Header:
 * hafas binary format header version 6
 *
*/
typedef struct _HafasBin6Header {
    guint16 version;                                                 /* 0x00 */
    gchar   start[14];    /* HafasBin6Loc */                         /* 0x02 */
    gchar   end[14];      /* HafasBin6Loc */                         /* 0x10 */
    guint16 num_trips;    /* # of trips in this resp. */             /* 0x1e */
    guint32 service_tbl;  /* Start of service table */               /* 0x20 */
    guint32 strings_tbl;  /* start of strings table */               /* 0x24 */
    gint16  days;         /* date base in days since 1980 */         /* 0x28 */
    gchar   unknown0[12];                                            /* 0x2a */
    guint32 stations_tbl; /* start of stations table */              /* 0x36 */
    guint32 comments_tbl; /* start of comments table */              /* 0x3a */
    gchar   unknown1[8];                                             /* 0x3e */
    guint32 ext;          /* Start of extenson header */             /* 0x46 */
}  __attribute__ ((packed)) HafasBin6Header;


typedef enum _HafasBin6LocTypes {
    HAFAS_BIN6_LOC_TYPE_STATION = 1,
    HAFAS_BIN6_LOC_TYPE_ADDRESS = 2,
    HAFAS_BIN6_LOC_TYPE_POI     = 3,
} HafasBin6LocTypes;

/**
 * HafasBin6Loc:
 *
 * Start and end location from #HafasBin6Header
 */
typedef struct _HafasBin6Loc {
    guint16 name_off;    /* offset into string table */              /* 0x00 */
    guint16 unknown0;                                                /* 0x02 */
    guint16 type;        /* as in HafasBinLocTypes */                /* 0x04 */
    gint32  lon;         /* longitude * 10^6 */                      /* 0x06 */
    gint32  lat;         /* latitude * 10^6 */                       /* 0x0a */
} __attribute__ ((packed)) HafasBin6Loc;

/**
 * HafasBin6Trip:
 *
 * One entry per trip, follows directly after #HafasBin6Header
 */
typedef struct _HafasBin6Trip {
    guint16 service_off; /* offset into service days table */        /* 0x00 */
    guint32 parts_off;   /* offset after header */                   /* 0x02 */
    guint16 part_cnt;    /* number of parts in this trip */          /* 0x06 */
    guint16 changes;     /* number of train changes in this trip */  /* 0x08 */
    guint16 unknown0;                                                /* 0x0a */
} __attribute__ ((packed)) HafasBin6Trip;

/**
 * HafasBin6Station:
 *
 * A station in the stations table as pointed to by the #HafasBin6Header
 */
typedef struct _HafasBin6Station {
    guint16 name_off; /* offset in string table */                   /* 0x00 */
    guint32 id;       /* id of this station */                       /* 0x02 */
    gint32  lon;      /* longitute * 10^6 */                         /* 0x06 */
    gint32  lat;      /* latitude * 10^6 */                          /* 0x0a */
} __attribute__ ((packed)) HafasBin6Station;

/**
 * HafasBin6Errors:
 *
 * Errors in err field of #HafasBin6ExtHeader
 */
typedef enum _HafasBin6Errors {
    HAFAS_BIN6_ERROR_NONE = 0,
    HAFAS_BIN6_ERROR_SESSION_EXPIRED = 1,
    /* TBD */
} HafasBin6Errors;

/**
 * HafasBin6ExtHeader:
 *
 * Hafas Binary format version 6 extension header. If attrs_index0 !=
 * 0 and length >= 0x30 then attribute indexes for all trips follow
 * after this header. Location of this header is determined by ext in
 * the #HafasBin6Header.
 */
typedef struct _HafasBin6ExtHeader {
    guint32 length;                                                  /* 0x00 */
    guint32 unknown0;                                                /* 0x04 */
    guint16 seq;                                                     /* 0x08 */
    guint16 req_id_off;   /* request id offset into string table */  /* 0x0a */
    guint32 details_tbl;  /* offset to trip details header */        /* 0x0c */
    guint16 err;                                                     /* 0x10 */
    gchar   unknown1[14];                                            /* 0x12 */
    guint16 enc_off;      /* encoding offset into string table */    /* 0x20 */
    guint16 ld_off;       /* ld offset into string table */          /* 0x22 */
    guint16 attrs_off;    /* attributes offset */                    /* 0x24 */
    gchar   pad[6];                                                  /* 0x26 */
    guint32 attrs_index0; /* attribute indexes start here  */        /* 0x2c */
} HafasBin6ExtHeader;

/**
 * HafasBin6TripDetailsHeader:
 *
 * Header for all trip details following this header. The details_index_off
 * is relative to this header and stores just another offset to the final
 * location of the #HafasBin6TripDetail.
 */
typedef struct _HafasBin6TripDetailsHeader {
    guint16 version;                                                 /* 0x00 */
    guint16 unknown0;                                                /* 0x02 */
    guint16 details_index_off;                                       /* 0x04 */
    guint16 part_details_off;  /* size of part details struct */     /* 0x06 */
    guint16 part_detail_size;                                        /* 0x08 */
    guint16 stop_size;         /* size of stop struct */             /* 0x0a */
    guint16 stops_off;                                               /* 0x0c */
} HafasBin6TripDetailsHeader;

/**
 * HafasBin6ServiceDay:
 *
 * The service day table is used to calculate day offsets on top of
 * #HafasBin6Header days. See #hafas_binary_parse_service_day for details.
 */
typedef struct _HafasBin6ServiceDay {
    guint16 days_off;  /* offset into string table */                /* 0x00 */
    guint16 byte_base; /* days * 8 */                                /* 0x02 */
    guint16 byte_len;  /* # of bytes to follow */                    /* 0x04 */
    gchar   byte0;     /* first day offset byte */                   /* 0x06 */
} __attribute__ ((packed)) HafasBin6ServiceDay;

typedef enum _HafasBin6TripDetailRTStatus {
    HAFAS_BIN6_RTSTATUS_CANCELLED = 0x0002,
    HAFAS_BIN6_RTSTATUS_NORMAL    = 0xFFFF,
} HafasBin6TripDetailRTStatus;

/**
 * HafasBin6TripDetail:
 *
 * Status of this trip
 */
typedef struct _HafasBin6TripDetail {
    guint16 rt_status; /* real time status */                        /* 0x00 */
    guint16 delay;                                                   /* 0x02 */
}  HafasBin6TripDetail;

typedef enum _HafasBin6TripPartType {
    footway = 1,
    train   = 2,
} HafasBin6TripPartType;

/**
 * HafasBin6TripPartType:
 *
 * A part of the trip using one "vehicle".
 */
typedef struct _HafasBin6TripPart {
    guint16 dep;         /* planned departure time at start */       /* 0x00 */
    guint16 dep_off;     /* start, offset into stations table */     /* 0x02 */
    guint16 arr;         /* planned arrival time at end */           /* 0x04 */
    guint16 arr_off;     /* end, offset into stations table */       /* 0x06 */
    guint16 type;                                                    /* 0x08 */
    guint16 line_off;    /* offset into string table */              /* 0x0a */
    guint16 dep_pos_off; /* offset into string table */              /* 0x0c */
    guint16 arr_pos_off; /* offset into string table */              /* 0x0e */
    guint16 attr_index;                                              /* 0x10 */
    guint16 comments_off; /* offset into comments table */           /* 0x12 */
}  HafasBin6TripPart;

/**
 * HAFAS_BIN6_NO_PLATFORM: no platform for this stop
 */
#define HAFAS_BIN6_NO_PLATFORM "---"
/**
 * HAFAS_BIN6_NO_REALTIME: no realtime information for this stop
 */
#define HAFAS_BIN6_NO_REALTIME 0xFFFF

/**
 * HafasBin6TripPartDetail:
 *
 * More details of one part of the trip. In contrast to
 * #HafasBin6TripPart times and platforms are predicted instead of
 * planned values.
 */
typedef struct _HafasBin6TripPartDetail {
    guint16 dep_pred; /* predicted departure time */                 /* 0x00 */
    guint16 arr_pred; /* predicted arrival time */                   /* 0x02 */
    guint16 dep_pos_pred_off;  /* offset into string table */        /* 0x04 */
    guint16 arr_pos_pred_off;  /* offset into string table */        /* 0x06 */
    guint16 flags;      /* flags */                                  /* 0x08 */
    guint16 unknown0;                                                /* 0x0a */
    guint16 stop_index; /* index of first stop */                    /* 0x0c */
    guint16 stops_cnt;  /* number of stops */                        /* 0x0e */
} HafasBin6TripPartDetail;

#define HAFAS_BIN6_PART_DETAIL_FLAGS_STOP_CANCELED 0x20 /* stop was cancelled */
#define HAFAS_BIN6_PART_DETAIL_FLAGS_TRIP_CANCELED 0x30 /* trip was cancelled */

#define HAFAS_BIN6_PART_DETAIL_FLAGS_CANCELED_MASK 0x30 /* trip was cancelled */

/**
 * HafasBin6Stop:
 *
 * Platforms and times (planned and predicted) for stops on the trip
 */
typedef struct _HafasBin6TripStop {
    guint16 dep; /* planned departure time at this stop */           /* 0x00 */
    guint16 arr; /* planned arrival time at this stop */             /* 0x02 */
    guint16 dep_pos_off; /* platform, offset into string table */    /* 0x04 */
    guint16 arr_pos_off; /* platform, offset into string table */    /* 0x06 */
    guint32 unknown0;                                                /* 0x08 */
    guint16 dep_pred; /* predicted departure time at this stop */    /* 0x0c */
    guint16 arr_pred; /* predicted arrival time at this stop */      /* 0x0e */
    guint16 dep_pos_pred_off; /* offset into string table */         /* 0x10 */
    guint16 arr_pos_pred_off; /* offset into string table */         /* 0x12 */
    guint32 unknown1;                                                /* 0x14 */
    guint16 stop_idx; /* index into stations table */                /* 0x18 */

} __attribute__ ((packed)) HafasBin6TripStop;

typedef struct _HafasBin6Attr {
    guint16 key_off; /* offset into string table */                  /* 0x00 */
    guint16 val_off; /* offset into string table */                  /* 0x02 */
} HafasBin6Attr;

#pragma GCC diagnostic pop /* "-Wattributes" */

/*
 * Access to headers and tables
 */
/* the main header */
#define HAFAS_BIN6_HEADER(data) ((HafasBin6Header*)(data))
/* extension header */
#define HAFAS_BIN6_EXT_HEADER(data) ((HafasBin6ExtHeader*)((data) + (((HafasBin6Header*)(data))->ext)))
/* trip details header */
#define HAFAS_BIN6_TRIP_DETAILS_HEADER(data) ((HafasBin6TripDetailsHeader*)((data) + HAFAS_BIN6_EXT_HEADER(data)->details_tbl))
/* trip details index table */
#define _HAFAS_BIN6_TRIP_DETAILS_INDEX(data)      ((gchar*)(((gchar*)HAFAS_BIN6_TRIP_DETAILS_HEADER(data)) + \
                                                            (HAFAS_BIN6_TRIP_DETAILS_HEADER(data)->details_index_off)))
/* trip part details index table */
#define _HAFAS_BIN6_TRIP_PART_DETAILS_INDEX(data) ((gchar*)(((gchar*)HAFAS_BIN6_TRIP_DETAILS_HEADER(data)) + \
                                                           (HAFAS_BIN6_TRIP_DETAILS_HEADER(data)->part_details_off)))
/* stops index table */
#define _HAFAS_BIN6_STOPS_INDEX(data)             ((gchar*)(((gchar*)HAFAS_BIN6_TRIP_DETAILS_HEADER(data)) + \
                                                           (HAFAS_BIN6_TRIP_DETAILS_HEADER(data)->stops_off)))
/* trips table */
#define _HAFAS_BIN6_TRIPS_TABLE(data)             ((gchar*)((data) + sizeof(HafasBin6Header)))

/*
 * Access to the data structures making up the trips
 */
/* Start and end of trip */
#define HAFAS_BIN6_START(data) ((HafasBin6Loc*)(((HafasBin6Header*)data)->start))
#define HAFAS_BIN6_END(data) ((HafasBin6Loc*)(((HafasBin6Header*)data)->end))
/* trip details and trip part details use an indirection via a common index table */
#define _HAFAS_BIN6_TRIP_INDEX(data, idx) \
    (*((guint16*)(_HAFAS_BIN6_TRIP_DETAILS_INDEX(data) + 2 * (idx))))
/* Get a string at offset off */
#define HAFAS_BIN6_STR(data, off) \
    ((const gchar*)((gchar*)((data) + (((HafasBin6Header*)(data))->strings_tbl) + (off))))
/* Get the n-th station from the staitons table */
#define HAFAS_BIN6_STATION(data, idx) \
    ((HafasBin6Station*)((data) + (((HafasBin6Header*)(data))->stations_tbl) + ((idx) * sizeof(HafasBin6Station))))
/* Get the idx-th trip */
#define HAFAS_BIN6_TRIP(data, idx) \
    ((HafasBin6Trip*)((data) + sizeof(HafasBin6Header) + ((idx) * sizeof(HafasBin6Trip))))
/* Get the service table entry for the idx-th trip */
#define HAFAS_BIN6_SERVICE_DAY(data, idx) \
    ((HafasBin6ServiceDay*)((data) + (((HafasBin6Header*)(data))->service_tbl) + \
                            (HAFAS_BIN6_TRIP(data, idx)->service_off)))
/* Get the idy-th part of the idx-th trip */
#define HAFAS_BIN6_TRIP_PART(data, idx, idy) \
    ((HafasBin6TripPart*)(_HAFAS_BIN6_TRIPS_TABLE(data) + \
                          (HAFAS_BIN6_TRIP(data, idx)->parts_off) + \
                          idy * sizeof(HafasBin6TripPart)))
/* Get the trip details for the idx-th trip */
#define HAFAS_BIN6_TRIP_DETAIL(data, idx) \
    ((HafasBin6TripDetail*)(_HAFAS_BIN6_TRIP_DETAILS_INDEX(data) + \
                            _HAFAS_BIN6_TRIP_INDEX(data, idx)))
/* Get the trip part details for the idx-th trip and the idy-th part */
#define HAFAS_BIN6_TRIP_PART_DETAIL(data, idx, idy) \
    ((HafasBin6TripPartDetail*)(_HAFAS_BIN6_TRIP_PART_DETAILS_INDEX(data) + \
                                _HAFAS_BIN6_TRIP_INDEX(data, idx) + \
                                ((idy) * sizeof(HafasBin6TripPartDetail))))
/* Get the idz-th stop of the idy-th part in the idx-th trip */
#define HAFAS_BIN6_STOP(data, idx, idy, idz) \
    ((HafasBin6TripStop*)(_HAFAS_BIN6_STOPS_INDEX(data) + \
                          (HAFAS_BIN6_TRIP_PART_DETAIL(data, idx, idy)->stop_index) * sizeof(HafasBin6TripStop) + \
                          idz * sizeof(HafasBin6TripStop)))


/* Convert hafas longitude/latitude information to floating point */
#define HAFAS_BIN6_LL_DOUBlE(l) ((gdouble) (l) / 1000000.0)

#endif /* _HAFAS_BINARY_H */
