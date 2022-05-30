#include "c.h"
#include "storage/md.h"
#include "access/htup.h"
#include "access/initdb.h"
#include "storage/bufpage.h"

void create_table(Oid oid, char relname[NAMEDATALEN])
{
    char path[PATH_LEN];
    get_relation_path(path, oid);
    mdcreate(path);
    page_init(oid, 0);
    pg_class_add_item(oid, relname);
    get_pg_class_tuple(2);
}