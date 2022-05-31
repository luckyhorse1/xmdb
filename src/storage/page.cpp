#include "storage/page.h"
#include "access/pg_class.h"

void page_init(Oid relNode, BlockNumber page)
{
    int fd = relation_open(relNode);
    PageHeader header = (PageHeader)malloc(BLCKSZ);
    memset(header, 0, BLCKSZ);
    header->pd_lower = SizeOfPageHeaderData;
    header->pd_upper = BLCKSZ;
    lseek(fd, page * BLCKSZ, SEEK_SET);
    write(fd, header, BLCKSZ);
    close(fd);
    free(header);
}

PageHeader page_read(Oid relNode, BlockNumber page)
{
    int fd = relation_open(relNode);
    PageHeader phdr = (PageHeader)malloc(BLCKSZ);
    memset(phdr, 0, BLCKSZ);
    lseek(fd, page * BLCKSZ, SEEK_SET);
    read(fd, phdr, BLCKSZ);
    close(fd);
    return phdr;
}

int page_add_item(Oid reloid, char *item, Size size)
{
    PageHeader phdr;
    Size alignedSize;
    OffsetNumber offsetNumber;
    int lower;
    int upper;
    ItemId itemId;
    int fd;
    int npage;

    if (reloid == RelationRelationId)
    {
        npage = 1;
    }
    else
    {
        npage = get_page_num(reloid);
    }

    phdr = (PageHeader)malloc(BLCKSZ);
    memset(phdr, 0, BLCKSZ);
    fd = relation_open(reloid);
    lseek(fd, (npage - 1) * BLCKSZ, SEEK_SET);
    read(fd, phdr, BLCKSZ);
    close(fd);

    offsetNumber = PageGetMaxOffsetNumber(phdr) + 1;
    lower = phdr->pd_lower + sizeof(ItemIdData);
    alignedSize = MAXALIGN(size);
    upper = (int)phdr->pd_upper - (int)alignedSize;

    if (lower > upper)
    {
        page_advance(reloid);
        return page_add_item(reloid, item, size);
    }

    itemId = PageGetItemId(phdr, offsetNumber);
    ItemIdSetNormal(itemId, upper, size);

    memcpy((char *)phdr + upper, item, size);
    phdr->pd_lower = (LocationIndex)lower;
    phdr->pd_upper = (LocationIndex)upper;

    fd = relation_open(reloid);
    lseek(fd, (npage - 1) * BLCKSZ, SEEK_SET);
    write(fd, phdr, BLCKSZ);
    close(fd);

    return offsetNumber;
}

void page_advance(Oid reloid)
{
    int npage = get_page_num(reloid);
    page_init(reloid, npage);
    pgclass_update_page(reloid, npage + 1);
}