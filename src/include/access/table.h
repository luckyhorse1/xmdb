#ifndef TABLE_H
#define TABLE_H

#include "c.h"

extern void create_table(Oid oid, const char* relname);
extern Oid get_oid_by_relname(const char* relname);

#endif