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

typedef enum CLASS_KIND
{
    CLASS_OID,
    CLASS_NAME,
    CLASS_NODE,
    CLASS_PAGE,
    CLASS_ATTR
} CLASS_KIND;
#endif
