#ifndef BUFPAGE_H
#define BUFPAGE_H

#include "c.h"
#include "storage/relation.h"

#define BLCKSZ 8196
typedef uint16 LocationIndex;
typedef uint16 OffsetNumber;

#define LP_UNUSED 0	  /* unused (should always have lp_len=0) */
#define LP_NORMAL 1	  /* used (should always have lp_len>0) */
#define LP_REDIRECT 2 /* HOT redirect (should have lp_len=0) */
#define LP_DEAD 3	  /* dead, may or may not have storage */

typedef struct ItemIdData
{
	unsigned lp_off : 15, /* offset to tuple (from start of page) */
		lp_flags : 2,	  /* state of line pointer, see below */
		lp_len : 15;	  /* byte length of tuple */
} ItemIdData;

typedef ItemIdData *ItemId;

typedef struct PageHeaderData
{
	LocationIndex pd_lower;					   /* offset to start of free space */
	LocationIndex pd_upper;					   /* offset to end of free space */
	ItemIdData pd_linp[FLEXIBLE_ARRAY_MEMBER]; /* line pointer array */
} PageHeaderData;

typedef PageHeaderData *PageHeader;

#define SizeOfPageHeaderData (offsetof(PageHeaderData, pd_linp))

#define PageGetMaxOffsetNumber(page) \
	(((PageHeader)(page))->pd_lower <= SizeOfPageHeaderData ? 0 : ((((PageHeader)(page))->pd_lower - SizeOfPageHeaderData) / sizeof(ItemIdData)))

#define PageGetItemId(page, offsetNumber) \
	((ItemId)(&((PageHeader)(page))->pd_linp[(offsetNumber)-1]))

#define ItemIdSetNormal(itemId, off, len) \
	(                                     \
		(itemId)->lp_flags = LP_NORMAL,   \
		(itemId)->lp_off = (off),         \
		(itemId)->lp_len = (len))

extern void page_init(Oid relNode, BlockNumber page);
extern PageHeader page_read(Oid relNode, BlockNumber page);
extern int page_add_item(Oid relNode, char *item, Size size);

#endif