#ifndef HTUP_H
#define HTUP_H

#include "c.h"
#include "catalog/pg_attribute.h"

/*
 * information stored in t_infomask:
 */
#define HEAP_HASNULL 0x0001 /* has null attribute(s) */

typedef uint16 OffsetNumber;
typedef struct BlockIdData
{
    uint16 bi_hi;
    uint16 bi_lo;
} BlockIdData;

typedef struct ItemPointerData
{
    BlockIdData ip_blkid;
    OffsetNumber ip_posid;
} ItemPointerData;

struct HeapTupleHeaderData
{
    TransactionId t_xmin;
    TransactionId t_xmax;
    CommandId t_cid;

    ItemPointerData t_ctid;
    uint16 t_infomask2; /* number of attributes + various flags */
    uint16 t_infomask;  /* various flag bits, see below */
    uint8 t_hoff;       /* sizeof header incl. bitmap, padding */

    /* ^ - 23 bytes - ^ */

    bits8 t_bits[FLEXIBLE_ARRAY_MEMBER]; /* bitmap of NULLs */

    /* MORE DATA FOLLOWS AT END OF STRUCT */
};

typedef struct HeapTupleHeaderData HeapTupleHeaderData;
typedef HeapTupleHeaderData *HeapTupleHeader;

typedef struct HeapTupleData
{
    uint32 t_len;           /* length of *t_data */
    ItemPointerData t_self; /* SelfItemPointer */
    Oid t_tableOid;         /* table the tuple came from */
    HeapTupleHeader t_data; /* -> tuple header and data */
} HeapTupleData;

typedef HeapTupleData *HeapTuple;

#define HEAPTUPLESIZE MAXALIGN(sizeof(HeapTupleData))

typedef struct TupleDescData
{
    int natts; /* number of attributes in the tuple */
    FormData_pg_attribute attrs[FLEXIBLE_ARRAY_MEMBER];
} TupleDescData;

typedef struct TupleDescData *TupleDesc;

/* Accessor for the i'th attribute of tupdesc. */
#define TupleDescAttr(tupdesc, i) (&(tupdesc)->attrs[(i)])

extern HeapTuple heap_form_tuple(TupleDesc tupleDesc, Datum *values, bool *isnull);
extern void get_tuple();

#endif /* HTUP_H */