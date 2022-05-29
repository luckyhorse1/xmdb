
#include "storage/md.h"
#include "catalog/pg_class.h"
#include "storage/bufpage.h"
#include "access/htup.h"

void init_pg_class();
void construct_pg_class_init_data(TupleDesc &tupleDesc, Datum *&values, bool *&isnull);
void get_pg_class_tuple();

void initdb()
{
    init_pg_class();
    get_pg_class_tuple();
}

void init_pg_class()
{
    char path[PATH_LEN];
    get_relation_path(path, RelationRelationId);
    mdcreate(path);
    page_init(RelationRelationId, 0);

    TupleDesc tupleDesc;
    Datum *values;
    bool *isnull;
    construct_pg_class_init_data(tupleDesc, values, isnull);
    HeapTuple tuple = heap_form_tuple(tupleDesc, values, isnull);
    page_add_item(RelationRelationId, (char *)(tuple->t_data), tuple->t_len);

    free(tupleDesc);
    free(values);
    free(isnull);
}

void construct_pg_class_init_data(TupleDesc &tupleDesc, Datum *&values, bool *&isnull)
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

    values[0] = (Datum)malloc(sizeof(Oid));
    values[0] = RelationRelationId;

    const char *relname = (char *)malloc(NAMEDATALEN);
    relname = "pg_class";
    values[1] = (Datum)relname;

    values[2] = (Datum)malloc(sizeof(Oid));
    values[2] = RelationRelationId;

    values[3] = (Datum)malloc(sizeof(int32));
    values[3] = 1;

    isnull[0] = false;
    isnull[1] = false;
    isnull[2] = false;
    isnull[3] = false;
}

void get_pg_class_tuple()
{
    int offset = 0;
    int fd = relation_open(RelationRelationId);
    PageHeader phdr = (PageHeader)malloc(BLCKSZ);

    memset(phdr, 0, BLCKSZ);
    read(fd, phdr, BLCKSZ);

    printf("lower: %d upper: %d\n", phdr->pd_lower, phdr->pd_upper);

    ItemId itemId = PageGetItemId(phdr, 1);
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