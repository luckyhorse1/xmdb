#include "storage/bufpage.h"
#include "access/htup.h"
#include "access/initdb.h"
#include "access/table.h"

int main()
{
    initdb();

    create_table(16384, "person");
}