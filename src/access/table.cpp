#include "c.h"
#include "storage/md.h"
#include "access/htup.h"
#include "access/initdb.h"
#include "storage/bufpage.h"
#include "catalog/pg_class.h"

void create_table(Oid oid, const char *relname, char (*attnames)[NAMEDATALEN], Type *types, int nattr)
{
    char path[PATH_LEN];
    get_relation_path(path, oid);
    mdcreate(path);
    page_init(oid, 0);

    pg_class_page_add_item(oid, relname, nattr);
    get_pg_class_tuple(2);

    for (int i = 0; i < nattr; i++)
    {
        pg_attr_page_add_item(oid, attnames[i], types[i], i);
    }
}

Oid get_oid_by_pgclass_tuple(HeapTupleHeader tuple, const char *relname)
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

int get_nattr_by_pgclass_tuple(HeapTupleHeader tuple, Oid reloid)
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

bool get_tupdesc_by_pgattr(HeapTupleHeader tuple, TupleDesc &tupleDesc, Oid reloid)
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

void table_insert(const char *relname, const char *name, int16 age, char sex, uint32 money)
{
    Oid reloid = get_oid_by_relname(relname);

    // get nattr
    int nattr = 0;
    PageHeader phdr = page_read(RelationRelationId, 0);
    int num = PageGetMaxOffsetNumber(phdr);
    for (int i = 0; i < num; i++)
    {
        HeapTupleHeader tuple = (HeapTupleHeader)((char *)phdr + phdr->pd_linp[i].lp_off);
        if (nattr = get_nattr_by_pgclass_tuple(tuple, reloid))
        {
            break;
        }
    }
    free(phdr);

    if (nattr <= 0)
    {
        fprintf(stderr, "invalid nattr\n");
        return;
    }

    // get values
    Datum *values = (Datum *)malloc(nattr * sizeof(Datum));
    values[0] = (Datum)name;
    values[1] = age;
    values[2] = sex;
    values[3] = money;

    // get attrs
    TupleDesc tupleDesc = (TupleDesc)malloc(sizeof(TupleDescData) + nattr * sizeof(FormData_pg_attribute));
    tupleDesc->natts = nattr;
    phdr = page_read(AttributeRelationId, 0);
    num = PageGetMaxOffsetNumber(phdr);
    int count = 0;
    for (int i = 0; i < num; i++)
    {
        if (count == nattr)
        {
            break;
        }
        HeapTupleHeader tuple = (HeapTupleHeader)((char *)phdr + phdr->pd_linp[i].lp_off);
        if (get_tupdesc_by_pgattr(tuple, tupleDesc, reloid))
        {
            count++;
        }
    }
    free(phdr);

    // insert
    HeapTuple tuple = heap_form_tuple(tupleDesc, values, NULL);
    page_add_item(reloid, (char *)(tuple->t_data), tuple->t_len);

    free(tupleDesc);
    free(values);
}

Type get_type(FormData_pg_attribute *attr)
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

void print_attr_val(Type type, char *start)
{
    switch (type)
    {
    case CHAR:
        printf("%c, ", *start);
        break;
    case SHORT:
        printf("%d, ", *(int16 *)start);
        break;
    case INT:
        printf("%d, ", *(int32 *)start);
        break;
    case LONG:
        printf("%ld, ", *(int64 *)start);
        break;
    case STRING:
        printf("%s, ", start);
    default:
        break;
    }
}

void print_tup(HeapTupleHeader tuple, TupleDesc tupleDesc)
{
    int offset = 0;
    char *start = (char *)tuple + tuple->t_hoff;

    for (int i = 0; i < tupleDesc->natts; i++)
    {
        FormData_pg_attribute *attr = &tupleDesc->attrs[i];
        offset = att_align_nominal(offset, attr->attalign);
        Type type = get_type(attr);
        printf("%s: ", attr->attname.data);
        print_attr_val(type, start + offset);
        offset += (type == STRING ? (strlen(start + offset) + 1) : attr->attlen);
    }
    printf("\n");
}

void table_scan(const char *relname)
{
    Oid reloid = get_oid_by_relname(relname);
    // printf("oid %u\n", reloid);

    // get nattr
    int nattr = 0;
    PageHeader phdr = page_read(RelationRelationId, 0);
    int num = PageGetMaxOffsetNumber(phdr);
    for (int i = 0; i < num; i++)
    {
        HeapTupleHeader tuple = (HeapTupleHeader)((char *)phdr + phdr->pd_linp[i].lp_off);
        if (nattr = get_nattr_by_pgclass_tuple(tuple, reloid))
        {
            break;
        }
    }
    free(phdr);

    // printf("nattr %d\n", nattr);

    if (nattr <= 0)
    {
        fprintf(stderr, "invalid nattr");
        return;
    }

    // get attrs
    TupleDesc tupleDesc = (TupleDesc)malloc(sizeof(TupleDescData) + nattr * sizeof(FormData_pg_attribute));
    tupleDesc->natts = nattr;
    phdr = page_read(AttributeRelationId, 0);
    num = PageGetMaxOffsetNumber(phdr);
    int count = 0;
    for (int i = 0; i < num; i++)
    {
        if (count == nattr)
        {
            break;
        }
        HeapTupleHeader tuple = (HeapTupleHeader)((char *)phdr + phdr->pd_linp[i].lp_off);
        if (get_tupdesc_by_pgattr(tuple, tupleDesc, reloid))
        {
            count++;
        }
    }
    free(phdr);

    // print
    phdr = page_read(reloid, 0);
    num = PageGetMaxOffsetNumber(phdr);
    for (int i = 0; i < num; i++)
    {
        HeapTupleHeader tuple = (HeapTupleHeader)((char *)phdr + phdr->pd_linp[i].lp_off);
        print_tup(tuple, tupleDesc);
    }
    free(phdr);

    free(tupleDesc);
}