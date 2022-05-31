#include "c.h"
#include "storage/relation.h"
#include "access/tuple.h"
#include "access/pg_attr.h"
#include "storage/page.h"
#include "catalog/pg_class.h"

void create_table(Oid oid, const char *relname, char (*attnames)[NAMEDATALEN], Type *types, int nattr)
{
    relation_create(oid);

    int offsetnum = pgclass_page_add_item(oid, relname, nattr);
    print_pgclass_tuple(offsetnum);

    for (int i = 0; i < nattr; i++)
    {
        pgattr_page_add_item(oid, attnames[i], types[i], i);
    }
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
        if (nattr = get_nattr_by_reloid(tuple, reloid))
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
        if (get_tupdesc_by_reloid(tuple, tupleDesc, reloid))
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
        if (nattr = get_nattr_by_reloid(tuple, reloid))
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
        if (get_tupdesc_by_reloid(tuple, tupleDesc, reloid))
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