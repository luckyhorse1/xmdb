#ifndef C_H
#define C_H

#include <stdio.h>
#include <stdint.h> // uintptr_t
#include <string.h>
#include <malloc.h>
#include <assert.h>

typedef unsigned char uint8;   /* == 8 bits */
typedef unsigned short uint16; /* == 16 bits */
typedef unsigned int uint32;   /* == 32 bits */

typedef signed char int8;	/* == 8 bits */
typedef signed short int16; /* == 16 bits */
typedef signed int int32;	/* == 32 bits */

typedef uint8 bits8;   /* >= 8 bits */
typedef uint16 bits16; /* >= 16 bits */
typedef uint32 bits32; /* >= 32 bits */

typedef char *Pointer;

#define PATH_LEN 128

#define BITMAPLEN(NATTS) (((int)(NATTS) + 7) / 8)

#define TYPEALIGN(ALIGNVAL, LEN) \
	(((uintptr_t)(LEN) + ((ALIGNVAL)-1)) & ~((uintptr_t)((ALIGNVAL)-1)))

#define ALIGNOF_SHORT 2
#define ALIGNOF_INT 4
#define ALIGNOF_LONG 8
#define ALIGNOF_DOUBLE 8
#define MAXIMUM_ALIGNOF 8

#define SHORTALIGN(LEN) TYPEALIGN(ALIGNOF_SHORT, (LEN))
#define INTALIGN(LEN) TYPEALIGN(ALIGNOF_INT, (LEN))
#define LONGALIGN(LEN) TYPEALIGN(ALIGNOF_LONG, (LEN))
#define DOUBLEALIGN(LEN) TYPEALIGN(ALIGNOF_DOUBLE, (LEN))
#define MAXALIGN(LEN) TYPEALIGN(MAXIMUM_ALIGNOF, (LEN))

#define HIGHBIT (0x80)

#define FLEXIBLE_ARRAY_MEMBER /**/

#define DatumGetChar(X) ((char)(X))
#define DatumGetInt16(X) ((int16)(X))
#define DatumGetInt32(X) ((int32)(X))
#define DatumGetPointer(X) ((Pointer)(X))
#define DatumGetCString(X) ((char *)DatumGetPointer(X))

#define PointerGetDatum(X) ((Datum)(X))

typedef uint32 Oid;
#define InvalidOid		((Oid) 0)

typedef uint32 TransactionId;
typedef uint32 CommandId;
typedef uintptr_t Datum;
typedef size_t Size;
typedef uint32 BlockNumber;

#define CppConcat(x, y) x##y

#define CATALOG(name, oid, oidmacro) typedef struct CppConcat(FormData_, name)

#define NAMEDATALEN 64
typedef struct nameData
{
	char data[NAMEDATALEN];
} NameData;

#endif