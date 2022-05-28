#ifndef PG_ATTRIBUTE_D_H
#define PG_ATTRIBUTE_D_H

#include "c.h"

#define AttributeRelationId 1249

CATALOG(pg_attribute, 1249, AttributeRelationId)
{
	/*
	 * 'c' = CHAR alignment, ie no alignment needed.
	 * 's' = SHORT alignment (2 bytes on most machines).
	 * 'i' = INT alignment (4 bytes on most machines).
	 * 'd' = DOUBLE alignment (8 bytes on many machines, but by no means all).
	 */
	char attalign;

	int16 attlen;
}
FormData_pg_attribute;

typedef FormData_pg_attribute *Form_pg_attribute;

#define att_align_nominal(cur_offset, attalign) \
	(                                           \
		((attalign) == 'i') ? INTALIGN(cur_offset) : (((attalign) == 'c') ? (uintptr_t)(cur_offset) : (((attalign) == 'd') ? DOUBLEALIGN(cur_offset) : (SHORTALIGN(cur_offset)))))

#define att_addlength_datum(cur_offset, attlen, attptr) \
	(                                                   \
		((attlen) > 0) ? (                              \
							 (cur_offset) + (attlen))   \
					   : (                              \
							 (cur_offset) + (strlen((char *)(attptr)) + 1)))

#define store_att_byval(T, newdatum, attlen)                                \
	do                                                                      \
	{                                                                       \
		switch (attlen)                                                     \
		{                                                                   \
		case sizeof(char):                                                  \
			*(char *)(T) = DatumGetChar(newdatum);                          \
			break;                                                          \
		case sizeof(int16):                                                 \
			*(int16 *)(T) = DatumGetInt16(newdatum);                        \
			break;                                                          \
		case sizeof(int32):                                                 \
			*(int32 *)(T) = DatumGetInt32(newdatum);                        \
			break;                                                          \
		case sizeof(Datum):                                                 \
			*(Datum *)(T) = (newdatum);                                     \
			break;                                                          \
		default:                                                            \
			fprintf(stderr, "unsupported byval length: %d", (int)(attlen)); \
			break;                                                          \
		}                                                                   \
	} while (0)

#endif