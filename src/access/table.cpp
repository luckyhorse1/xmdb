#include "c.h"
#include "storage/md.h"
#include "access/htup.h"
#include "access/initdb.h"
#include "storage/bufpage.h"
#include "catalog/pg_class.h"

void create_table(Oid oid, const char *relname)
{
    char path[PATH_LEN];
    get_relation_path(path, oid);
    mdcreate(path);
    page_init(oid, 0);
    pg_class_add_item(oid, relname);
    get_pg_class_tuple(2);
}

Oid get_oid_by_pgclass_tuple(HeapTupleHeader tuple, const char* relname)
{
    Oid ret = InvalidOid;
    char *start = (char *)tuple + tuple->t_hoff;

    if (strcmp(start + 4, relname) == 0) {
        ret = *(Oid*)start;
    }
    return ret;
}

Oid get_oid_by_relname(const char* relname)
{
    Oid ret = InvalidOid;
    PageHeader phdr = page_read(RelationRelationId, 0);
    int num = PageGetMaxOffsetNumber(phdr);
    for (int i = 1; i < num; i++) {
        HeapTupleHeader tuple = (HeapTupleHeader)((char*)phdr + phdr->pd_linp[i].lp_off);
        if (ret = get_oid_by_pgclass_tuple(tuple, relname)) {
            break;
        }
    }
    return ret;
}