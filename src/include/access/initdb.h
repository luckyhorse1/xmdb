#ifndef INITDB_H
#define INITDB_H

extern void pg_class_page_add_item(Oid oid, const char *relname, int nattr);
extern void get_pg_class_tuple(int num);
extern void initdb();

extern void pg_attr_page_add_item(Oid oid, const char *attname, Type type, int attrnum);

#endif