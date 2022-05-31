#ifndef PG_CLASS_H
#define PG_CLASS_H

#include "c.h"

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

#endif
