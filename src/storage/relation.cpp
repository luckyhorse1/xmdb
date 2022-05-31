#include "storage/relation.h"
#include "storage/page.h"

const char *data_path = "/home/xiaoma/code/data";

void relation_create(Oid relNode)
{
    char path[PATH_LEN];
    sprintf(path, "%s/base/%u/%u", data_path, 1, relNode);
    int fd = open(path, O_RDWR | O_CREAT | O_EXCL, (S_IRUSR | S_IWUSR));
    close(fd);
    page_init(relNode, 0);
}

int relation_open(Oid relNode)
{
    char path[PATH_LEN];
    sprintf(path, "%s/base/%u/%u", data_path, 1, relNode);
    int fd = open(path, O_RDWR);
    return fd;
}