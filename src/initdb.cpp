#include "catalog/pg_class.h"
#include "access/pg_attr.h"

void initdb()
{
    init_pg_class();

    init_pg_attribute();
}