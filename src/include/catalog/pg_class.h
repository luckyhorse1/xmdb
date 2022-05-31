#ifndef PG_CLASS_H
#define PG_CLASS_H

#include "c.h"
#include "access/tuple.h"

#define RelationRelationId 1259

CATALOG(pg_class, 1259, RelationRelationId)
{
    Oid oid;
    NameData relname;
    Oid relfilenode;
    int32 relpages;
    int16 relnatts;
}
FormData_pg_class;

extern void init_pg_class();
extern int pgclass_page_add_item(Oid oid, const char *relname, int nattr);
extern void print_pgclass_tuple(int offsetnum);
extern Oid get_oid_by_relname(const char *relname);
extern int get_nattr_by_reloid(HeapTupleHeader tuple, Oid reloid);

#endif
