#ifndef INITDB_H
#define INITDB_H

extern void pg_class_add_item(Oid oid, const char* relname);
extern void get_pg_class_tuple(int num);
extern void initdb();

#endif