#include "storage/md.h"

const char *data_path = "/home/xiaoma/code/data";

void get_relation_path(char path[PATH_LEN], Oid relNode)
{
    Oid dbNode = 1;
    sprintf(path, "%s/base/%u/%u", data_path, dbNode, relNode);
}

void mdcreate(const char *path)
{
    int fd = open(path, O_RDWR | O_CREAT | O_EXCL, (S_IRUSR | S_IWUSR));
    close(fd);
    printf("mdcreate fd %d\n", fd);
}

int relation_open(Oid relNode)
{
    char path[PATH_LEN];
    get_relation_path(path, relNode);
    int fd = open(path, O_RDWR);
    return fd;
}