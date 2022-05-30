#include "storage/bufpage.h"
#include "access/htup.h"
#include "access/initdb.h"
#include "access/table.h"

int main()
{
    initdb();

    create_table(16384, "person");

    printf("oid is %u\n", get_oid_by_relname("person"));
}