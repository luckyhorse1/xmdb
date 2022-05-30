
#include "storage/md.h"
#include "catalog/pg_class.h"
#include "storage/bufpage.h"
#include "access/htup.h"

void init_pg_class();
void pg_class_add_item(Oid oid, const char* relname);
void construct_relinfo(TupleDesc &tupleDesc, Datum *&values, bool *&isnull, Oid oid, const char *name);
void get_pg_class_tuple(int num);

void initdb()
{
    init_pg_class();
    get_pg_class_tuple(1);
}

void init_pg_class()
{
    char path[PATH_LEN];
    get_relation_path(path, RelationRelationId);
    mdcreate(path);
    page_init(RelationRelationId, 0);

    pg_class_add_item(RelationRelationId, "pg_class");
}

void pg_class_add_item(Oid oid, const char* relname)
{
    TupleDesc tupleDesc;
    Datum *values;
    bool *isnull;
    construct_relinfo(tupleDesc, values, isnull, oid, relname);
    HeapTuple tuple = heap_form_tuple(tupleDesc, values, isnull);
    page_add_item(RelationRelationId, (char *)(tuple->t_data), tuple->t_len);

    free(tupleDesc);
    free(values);
    free(isnull);
}

void construct_relinfo(TupleDesc &tupleDesc, Datum *&values, bool *&isnull, Oid oid, const char *name)
{
    tupleDesc = (TupleDesc)malloc(sizeof(TupleDescData) + 4 * sizeof(FormData_pg_attribute));
    values = (Datum *)malloc(4 * sizeof(Datum));
    isnull = (bool *)malloc(4 * sizeof(bool));

    tupleDesc->natts = 4;
    tupleDesc->attrs[0].attalign = 'i';
    tupleDesc->attrs[0].attlen = sizeof(Oid);

    tupleDesc->attrs[1].attalign = 'c';
    tupleDesc->attrs[1].attlen = -2;

    tupleDesc->attrs[2].attalign = 'i';
    tupleDesc->attrs[2].attlen = sizeof(Oid);

    tupleDesc->attrs[3].attalign = 'i';
    tupleDesc->attrs[3].attlen = sizeof(int32);

    values[0] = oid;
    values[1] = (Datum)name;
    values[2] = oid;
    values[3] = 1;

    isnull[0] = false;
    isnull[1] = false;
    isnull[2] = false;
    isnull[3] = false;
}

void get_pg_class_tuple(int num)
{
    int offset = 0;
    int fd = relation_open(RelationRelationId);
    PageHeader phdr = (PageHeader)malloc(BLCKSZ);

    memset(phdr, 0, BLCKSZ);
    read(fd, phdr, BLCKSZ);

    printf("lower: %d upper: %d\n", phdr->pd_lower, phdr->pd_upper);

    ItemId itemId = PageGetItemId(phdr, num);
    HeapTupleHeader tuple = (HeapTupleHeader)((char *)phdr + itemId->lp_off);

    // first value no need to align because hoff has been max aligned. see heap_form_tuple
    char *start = (char *)tuple + tuple->t_hoff;
    printf("oid: %u\n", *(Oid *)start);

    offset += 4;
    offset = att_align_nominal(offset, 'c');
    printf("relname: %s\n", start + offset);

    offset += strlen(start + offset) + 1;
    offset = att_align_nominal(offset, 'i');
    printf("relfilenode: %u\n", *(Oid *)(start + offset));

    offset += 4;
    offset = att_align_nominal(offset, 'i');
    printf("relpages: %d\n", *(int32 *)(start + offset));

    close(fd);
    free(phdr);
}