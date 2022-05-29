#ifndef MD_H
#define MD_H

#include <fcntl.h>  // open
#include <unistd.h> // read write
#include "c.h"

extern void get_relation_path(char *path, Oid relNode = 16384);
extern void mdcreate(const char *path);
extern int relation_open(Oid relNode);

#endif