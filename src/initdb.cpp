
#include "storage/md.h"
#include "catalog/pg_class.h"
#include "storage/bufpage.h"
#include "access/htup.h"
#include "access/initdb.h"

void init_pg_class();
void get_pg_class_tuple(int num);
void init_pg_attribute();

void construct_pgclass_tuple(TupleDesc &tupleDesc, Datum *&values, Oid oid, const char *name, int nattr);
void construct_pgattr_tuple(TupleDesc &tupleDesc, Datum *&values, Oid oid, const char *name, Type type, int num);

void initdb()
{
    init_pg_class();
    get_pg_class_tuple(1);

    init_pg_attribute();
}

void init_pg_class()
{
    char path[PATH_LEN];
    get_relation_path(path, RelationRelationId);
    mdcreate(path);
    page_init(RelationRelationId, 0);

    pg_class_page_add_item(RelationRelationId, "pg_class", 5);
}

void pg_class_page_add_item(Oid oid, const char *relname, int nattr)
{
    TupleDesc tupleDesc;
    Datum *values;
    construct_pgclass_tuple(tupleDesc, values, oid, relname, nattr);
    HeapTuple tuple = heap_form_tuple(tupleDesc, values, NULL);
    page_add_item(RelationRelationId, (char *)(tuple->t_data), tuple->t_len);

    free(tupleDesc);
    free(values);
}

void construct_pgclass_tuple(TupleDesc &tupleDesc, Datum *&values, Oid oid, const char *name, int nattr)
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

    values[0] = oid;
    values[1] = (Datum)name;
    values[2] = oid;
    values[3] = 1;
    values[4] = nattr;
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

void init_pg_attribute()
{
    char path[PATH_LEN];
    get_relation_path(path, AttributeRelationId);
    mdcreate(path);
    page_init(AttributeRelationId, 0);
}

void pg_attr_page_add_item(Oid oid, const char *attname, Type type, int attrnum)
{
    TupleDesc tupleDesc;
    Datum *values;
    construct_pgattr_tuple(tupleDesc, values, oid, attname, type, attrnum);
    HeapTuple tuple = heap_form_tuple(tupleDesc, values, NULL);
    page_add_item(AttributeRelationId, (char *)(tuple->t_data), tuple->t_len);

    free(tupleDesc);
    free(values);
}

void construct_pgattr_tuple(TupleDesc &tupleDesc, Datum *&values, Oid oid, const char *name, Type type, int attrnum)
{
    int nattr = 5;
    tupleDesc = (TupleDesc)malloc(sizeof(TupleDescData) + nattr * sizeof(FormData_pg_attribute));
    values = (Datum *)malloc(nattr * sizeof(Datum));

    tupleDesc->natts = nattr;
    tupleDesc->attrs[0].attalign = 'i';
    tupleDesc->attrs[0].attlen = sizeof(Oid);

    tupleDesc->attrs[1].attalign = 'c';
    tupleDesc->attrs[1].attlen = -2;

    tupleDesc->attrs[2].attalign = 's';
    tupleDesc->attrs[2].attlen = sizeof(int16);

    tupleDesc->attrs[3].attalign = 'c';
    tupleDesc->attrs[3].attlen = sizeof(char);

    tupleDesc->attrs[4].attalign = 's';
    tupleDesc->attrs[4].attlen = sizeof(int16);

    values[0] = oid;
    values[1] = (Datum)name;
    values[2] = attrnum;
    switch (type)
    {
    case CHAR:
        values[3] = 'c';
        values[4] = 1;
        break;
    case SHORT:
        values[3] = 's';
        values[4] = 2;
        break;
    case INT:
        values[3] = 'i';
        values[4] = 4;
        break;
    case LONG:
        values[3] = 'd';
        values[4] = 8;
        break;
    case STRING:
        values[3] = 'c';
        values[4] = -2;
        break;
    default:
        fprintf(stderr, "unspported type");
        break;
    }
}