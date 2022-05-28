#include "storage/md.h"

const char *data_path = "/home/xiaoma/code/data";

void get_relation_path(char path[PATH_LEN])
{
    Oid dbNode = 1;
    Oid relNode = 16384;
    sprintf(path, "%s/base/%u/%u", data_path, dbNode, relNode);
}

int mdcreate()
{
    char path[PATH_LEN];
    get_relation_path(path);
    int fd = open(path, O_RDWR | O_CREAT | O_EXCL, (S_IRUSR | S_IWUSR));
    printf("mdcreate fd %d\n", fd);
    return fd;
}

int relation_open()
{
    char path[PATH_LEN];
    get_relation_path(path);
    int fd = open(path, O_RDWR);
    return fd;
}