#ifndef TUPDESC_H
#define TUPDESC_H

#include "c.h"
#include "catalog/pg_attribute.h"

typedef struct TupleDescData
{
	int natts; /* number of attributes in the tuple */
	FormData_pg_attribute attrs[FLEXIBLE_ARRAY_MEMBER];
} TupleDescData;

typedef struct TupleDescData *TupleDesc;

/* Accessor for the i'th attribute of tupdesc. */
#define TupleDescAttr(tupdesc, i) (&(tupdesc)->attrs[(i)])

#endif