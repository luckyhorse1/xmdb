#include "storage/relation.h"
#include "storage/page.h"
#include "catalog/pg_class.h"
#include "access/tuple.h"

void init_pg_class();
int pgclass_page_add_item(Oid oid, const char *relname, int nattr);
void construct_pgclass_tuple(TupleDesc &tupleDesc, Datum *&values, Oid v_oid, const char *v_name, int v_nattr);
void print_pgclass_tuple(int offsetnum);

/*
 * 1 create pg_class file
 * 2 init page 0 by 0
 * 3 add relinfo to pg_class
 */
void init_pg_class()
{
    relation_create(RelationRelationId);
    int offsetnum = pgclass_page_add_item(RelationRelationId, "pg_class", 5);
    print_pgclass_tuple(offsetnum);
}

int pgclass_page_add_item(Oid oid, const char *relname, int nattr)
{
    TupleDesc tupleDesc;
    Datum *values;
    construct_pgclass_tuple(tupleDesc, values, oid, relname, nattr);
    HeapTuple tuple = heap_form_tuple(tupleDesc, values, NULL);
    int offsetnum = page_add_item(RelationRelationId, (char *)(tuple->t_data), tuple->t_len);

    free(tupleDesc);
    free(values);
    return offsetnum;
}

void construct_pgclass_tuple(TupleDesc &tupleDesc, Datum *&values, Oid v_oid, const char *v_name, int v_nattr)
{
    int num_pgclass_attr = 5;
    tupleDesc = (TupleDesc)malloc(sizeof(TupleDescData) + num_pgclass_attr * sizeof(FormData_pg_attribute));
    values = (Datum *)malloc(num_pgclass_attr * sizeof(Datum));

    tupleDesc->natts = num_pgclass_attr;
    tupleDesc->attrs[0].attalign = 'i';
    tupleDesc->attrs[0].attlen = sizeof(Oid);

    tupleDesc->attrs[1].attalign = 'c';
    tupleDesc->attrs[1].attlen = -2;

    tupleDesc->attrs[2].attalign = 'i';
    tupleDesc->attrs[2].attlen = sizeof(Oid);

    tupleDesc->attrs[3].attalign = 'i';
    tupleDesc->attrs[3].attlen = sizeof(int32);

    tupleDesc->attrs[4].attalign = 's';
    tupleDesc->attrs[4].attlen = sizeof(int16);

    values[0] = v_oid;
    values[1] = (Datum)v_name;
    values[2] = v_oid;
    values[3] = 1;
    values[4] = v_nattr;
}

void print_pgclass_tuple(int offsetnum)
{
    int offset = 0;
    int fd = relation_open(RelationRelationId);
    PageHeader phdr = (PageHeader)malloc(BLCKSZ);

    memset(phdr, 0, BLCKSZ);
    read(fd, phdr, BLCKSZ);

    ItemId itemId = PageGetItemId(phdr, offsetnum);
    HeapTupleHeader tuple = (HeapTupleHeader)((char *)phdr + itemId->lp_off);

    printf("pg_class tuple [");

    // first value no need to align because hoff has been max aligned. see heap_form_tuple
    char *start = (char *)tuple + tuple->t_hoff;
    printf("oid: %u, ", *(Oid *)start);

    offset += 4;
    offset = att_align_nominal(offset, 'c');
    printf("relname: %s, ", start + offset);

    offset += strlen(start + offset) + 1;
    offset = att_align_nominal(offset, 'i');
    printf("relfilenode: %u, ", *(Oid *)(start + offset));

    offset += 4;
    offset = att_align_nominal(offset, 'i');
    printf("relpages: %d", *(int32 *)(start + offset));

    printf("]\n");

    close(fd);
    free(phdr);
}

static inline Oid get_oid_by_pgclass_tuple(HeapTupleHeader tuple, const char *relname)
{
    Oid ret = InvalidOid;
    char *start = (char *)tuple + tuple->t_hoff;

    if (strcmp(start + 4, relname) == 0)
    {
        ret = *(Oid *)start;
    }
    return ret;
}

Oid get_oid_by_relname(const char *relname)
{
    Oid ret = InvalidOid;
    PageHeader phdr = page_read(RelationRelationId, 0);
    int num = PageGetMaxOffsetNumber(phdr);
    for (int i = 0; i < num; i++)
    {
        HeapTupleHeader tuple = (HeapTupleHeader)((char *)phdr + phdr->pd_linp[i].lp_off);
        if (ret = get_oid_by_pgclass_tuple(tuple, relname))
        {
            break;
        }
    }
    return ret;
}

int get_nattr_by_reloid(HeapTupleHeader tuple, Oid reloid)
{
    int offset = 0;
    char *start = (char *)tuple + tuple->t_hoff;
    if (*(Oid *)start != reloid)
    {
        return 0;
    }
    offset += 4;
    offset += strlen(start + offset) + 1;
    offset = att_align_nominal(offset, 'i');
    offset += 4;
    offset = att_align_nominal(offset, 'i');
    offset += 4;
    offset = att_align_nominal(offset, 's');
    return *(int16 *)(start + offset);
}