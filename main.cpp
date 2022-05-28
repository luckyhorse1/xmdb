#include "storage/bufpage.h"
#include "access/htup.h"

int main()
{
    // create table
    mdcreate();
    page_init();

    // insert
    HeapTuple tuple = heap_form_tuple();
    page_add_item((char *)(tuple->t_data), tuple->t_len);

    // select
    get_tuple();
}