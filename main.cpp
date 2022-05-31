#include "storage/bufpage.h"
#include "access/htup.h"
#include "access/initdb.h"
#include "access/table.h"

int main()
{
    initdb();

    char attnames[4][NAMEDATALEN] = {"name", "age", "sex", "money"};
    Type types[4] = {STRING, SHORT, CHAR, INT};
    create_table(16384, "person", attnames, types, 4);

    for (int i = 0; i < 4; i++)
    {
        table_insert("person", "xiaoma", 27, 'm', 20000);
    }

    table_scan("person");
}