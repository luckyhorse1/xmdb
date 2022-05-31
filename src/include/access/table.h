#ifndef TABLE_H
#define TABLE_H

#include "c.h"

extern void create_table(Oid oid, const char *relname, char (*attnames)[NAMEDATALEN], Type *types, int nattr);
extern Oid get_oid_by_relname(const char *relname);
extern void table_insert(const char *relname, const char *name, int16 age, char sex, uint32 money);
extern void table_scan(const char *relname);
#endif