#include "access/htup.h"
#include "storage/bufpage.h"

void get_tuple(Oid relNode)
{
    int fd = relation_open(relNode);
    PageHeader phdr = (PageHeader)malloc(BLCKSZ);

    memset(phdr, 0, BLCKSZ);
    read(fd, phdr, BLCKSZ);

    printf("lower: %d upper: %d\n", phdr->pd_lower, phdr->pd_upper);

    ItemId itemId = PageGetItemId(phdr, 1);
    HeapTupleHeader tuple = (HeapTupleHeader)((char *)phdr + itemId->lp_off);
    printf("name: %s\n", (char *)tuple + tuple->t_hoff);

    int offset = strlen((char *)tuple + tuple->t_hoff) + 1;
    offset = att_align_nominal(offset, 's');
    printf("age: %d\n", *(uint16 *)((char *)tuple + tuple->t_hoff + offset));

    offset += 2;
    printf("sex: %c\n", *((char *)tuple + tuple->t_hoff + offset));
    close(fd);
    free(phdr);
}

static void construct_test_data(TupleDesc &tupleDesc, Datum *&values, bool *&isnull)
{
    tupleDesc = (TupleDesc)malloc(sizeof(TupleDescData) + 4 * sizeof(FormData_pg_attribute));
    values = (Datum *)malloc(4 * sizeof(Datum));
    isnull = (bool *)malloc(4 * sizeof(bool));
    tupleDesc->natts = 4;

    tupleDesc->attrs[0].attalign = 'c';
    tupleDesc->attrs[0].attlen = -2;

    tupleDesc->attrs[1].attalign = 's';
    tupleDesc->attrs[1].attlen = sizeof(uint16);

    tupleDesc->attrs[2].attalign = 'c';
    tupleDesc->attrs[2].attlen = sizeof(uint8);

    tupleDesc->attrs[3].attalign = 'c';
    tupleDesc->attrs[3].attlen = -1;

    const char *name = (char *)malloc(NAMEDATALEN);
    name = "xiaoma";
    values[0] = (Datum)name;
    values[1] = (Datum)malloc(sizeof(uint16));
    values[1] = 27;
    values[2] = (Datum)malloc(sizeof(uint8));
    values[2] = '1';

    isnull[0] = true;
    isnull[1] = true;
    isnull[2] = true;
    isnull[3] = false;
}

static Size heap_compute_data_size(TupleDesc tupleDesc, Datum *values, bool *isnull)
{
    Size data_length = 0;
    int numberOfAttributes = tupleDesc->natts;
    for (int i = 0; i < numberOfAttributes; i++)
    {
        Datum val;
        Form_pg_attribute atti;
        if (isnull[i])
            continue;
        val = values[i];
        atti = TupleDescAttr(tupleDesc, i);
        data_length = att_align_nominal(data_length, atti->attalign);
        data_length = att_addlength_datum(data_length, atti->attlen, val);
    }
    return data_length;
}

static void fill_val(Form_pg_attribute att, bits8 **bit, int *bitmask, char **dataP,
                     uint16 *infomask, Datum datum, bool isnull)
{

    Size data_length;
    char *data = *dataP;
    if (bit != NULL)
    {
        if (*bitmask != HIGHBIT)
            *bitmask <<= 1;
        else
        {
            *bit += 1;
            **bit = 0x0;
            *bitmask = 1;
        }

        if (isnull)
        {
            *infomask |= HEAP_HASNULL;
            return;
        }

        **bit |= *bitmask;
    }

    if (att->attlen == -2)
    {
        data_length = strlen(DatumGetCString(datum)) + 1;
        memcpy(data, DatumGetPointer(datum), data_length);
    }
    else
    {
        data = (char *)att_align_nominal(data, att->attalign);
        store_att_byval(data, datum, att->attlen);
        data_length = att->attlen;
    }
    data += data_length;
    *dataP = data;
}

static void heap_fill_tuple(TupleDesc tupleDesc, Datum *values, bool *isnull, char *data,
                            Size data_size, uint16 *infomask, bits8 *bit)
{
    bits8 *bitP;
    int bitmask;
    int numberOfAttributes = tupleDesc->natts;
    char *start = data;

    if (bit != NULL)
    {
        bitP = &bit[-1];
        bitmask = HIGHBIT;
    }
    else
    {
        bitP = NULL;
        bitmask = 0;
    }

    *infomask &= ~HEAP_HASNULL;

    for (int i = 0; i < numberOfAttributes; i++)
    {
        Form_pg_attribute attr = TupleDescAttr(tupleDesc, i);

        fill_val(attr,
                 bitP ? &bitP : NULL,
                 &bitmask,
                 &data,
                 infomask,
                 values ? values[i] : PointerGetDatum(NULL),
                 isnull[i]);
    }

    assert((data - start) == data_size);
}

HeapTuple heap_form_tuple(TupleDesc tupleDesc, Datum *values, bool *isnull)
{
    HeapTuple tuple;
    HeapTupleHeader td;
    Size len, data_len;
    int hoff;
    bool hasnull = false;
    int numberOfAttributes = tupleDesc->natts;

    for (int i = 0; i < numberOfAttributes; i++)
    {
        if (isnull[i])
        {
            hasnull = true;
            break;
        }
    }

    len = offsetof(HeapTupleHeaderData, t_bits);
    if (hasnull)
        len += BITMAPLEN(numberOfAttributes);

    hoff = len = MAXALIGN(len);

    data_len = heap_compute_data_size(tupleDesc, values, isnull);

    len += data_len;

    tuple = (HeapTuple)malloc(HEAPTUPLESIZE + len);
    memset(tuple, 0, HEAPTUPLESIZE + len);
    tuple->t_data = td = (HeapTupleHeader)((char *)tuple + HEAPTUPLESIZE);
    tuple->t_len = len;
    td->t_hoff = hoff;

    heap_fill_tuple(tupleDesc, values, isnull, (char *)td + hoff, data_len,
                    &td->t_infomask, (hasnull ? td->t_bits : NULL));

    // test first value
    // char *data = (char *)td + hoff;
    // printf("oid: %u\n", *(Oid*)(data));
    return tuple;
}
