#include "storage/bufpage.h"

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

void page_add_item(Oid relNode, char *item, Size size)
{
    PageHeader phdr;
    Size alignedSize;
    OffsetNumber offsetNumber;
    int lower;
    int upper;
    ItemId itemId;
    int fd;

    phdr = (PageHeader)malloc(BLCKSZ);
    memset(phdr, 0, BLCKSZ);
    fd = relation_open(relNode);
    read(fd, phdr, BLCKSZ);
    close(fd);

    offsetNumber = /*PageGetMaxOffsetNumber(phdr)*/ +1;
    lower = phdr->pd_lower + sizeof(ItemIdData);
    alignedSize = MAXALIGN(size);
    upper = (int)phdr->pd_upper - (int)alignedSize;

    itemId = PageGetItemId(phdr, offsetNumber);
    ItemIdSetNormal(itemId, upper, size);

    memcpy((char *)phdr + upper, item, size);
    phdr->pd_lower = (LocationIndex)lower;
    phdr->pd_upper = (LocationIndex)upper;

    fd = relation_open(relNode);
    write(fd, phdr, BLCKSZ);
    close(fd);
}