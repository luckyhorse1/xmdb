#include "storage/relation.h"
#include "storage/page.h"
#include "catalog/pg_attr.h"
#include "access/tuple.h"

void init_pg_attribute();
void pgattr_page_add_item(Oid oid, const char *attname, Type type, int attrnum);
void construct_pgattr_tuple(TupleDesc &tupleDesc, Datum *&values, Oid oid, const char *name, Type type, int attrnum);

void init_pg_attribute()
{
    relation_create(AttributeRelationId);
}

void pgattr_page_add_item(Oid oid, const char *attname, Type type, int attrnum)
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
    int num_pgattr_attr = 5;
    tupleDesc = (TupleDesc)malloc(sizeof(TupleDescData) + num_pgattr_attr * sizeof(FormData_pg_attribute));
    values = (Datum *)malloc(num_pgattr_attr * sizeof(Datum));

    tupleDesc->natts = num_pgattr_attr;
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

bool get_tupdesc_by_reloid(HeapTupleHeader tuple, TupleDesc &tupleDesc, Oid reloid)
{
    int offset = 0;
    char *start = (char *)tuple + tuple->t_hoff;
    if (*(Oid *)start != reloid)
    {
        return false;
    }
    offset += 4;

    char *name = start + offset;
    offset += strlen(name) + 1;

    offset = att_align_nominal(offset, 's');
    int attnum = *(int16 *)(start + offset);
    memcpy(tupleDesc->attrs[attnum].attname.data, name, strlen(name) + 1);
    // printf("attrname %s\n", tupleDesc->attrs[attnum].attname.data);
    offset += 2;

    offset = att_align_nominal(offset, 'c');
    tupleDesc->attrs[attnum].attalign = *(start + offset);
    // printf("attalign %c\n", tupleDesc->attrs[attnum].attalign);
    offset += 1;

    offset = att_align_nominal(offset, 's');
    tupleDesc->attrs[attnum].attlen = *(int16 *)(start + offset);
    // printf("attlen %d\n", tupleDesc->attrs[attnum].attlen);
}

Type get_type_by_attr(FormData_pg_attribute *attr)
{
    switch (attr->attalign)
    {
    case 'c':
        if (attr->attlen == -2)
        {
            return STRING;
        }
        else if (attr->attlen == 1)
        {
            return CHAR;
        }
        break;
    case 's':
        return SHORT;
    case 'i':
        return INT;
    case 'd':
        return LONG;
    }

    return INVALID_TYPE;
}