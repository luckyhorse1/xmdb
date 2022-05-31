#ifndef PG_CLASS_ACCESS_H
#define PG_CLASS_ACCESS_H

#include "c.h"
#include "access/tuple.h"
#include "catalog/pg_class.h"

extern void init_pg_class();
extern int pgclass_page_add_item(Oid oid, const char *relname, int nattr);
extern void print_pgclass_tuple(int offsetnum);
extern Oid get_oid_by_relname(const char *relname);
extern Datum get_classinfo_by_reloid(HeapTupleHeader tuple, Oid reloid, CLASS_KIND kind);
extern int get_page_num(Oid reloid);
extern void pgclass_update_page(Oid reloid, int new_npage);

#endif