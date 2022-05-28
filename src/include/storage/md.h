#ifndef MD_H
#define MD_H

#include <fcntl.h> // open
#include <unistd.h> // read write
#include "c.h"

extern void get_relation_path(char *path);
extern int mdcreate();
extern int relation_open();

#endif