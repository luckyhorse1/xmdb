#ifndef PG_ATTR_ACCESS_H
#define PG_ATTR_ACCESS_H
#include "c.h"
#include "access/tuple.h"
#include "catalog/pg_attr.h"

extern void init_pg_attribute();
extern void pgattr_page_add_item(Oid oid, const char *attname, Type type, int attrnum);
extern bool get_tupdesc_by_reloid(HeapTupleHeader tuple, TupleDesc &tupleDesc, Oid reloid);
extern Type get_type_by_attr(FormData_pg_attribute *attr);

#endif