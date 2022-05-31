#ifndef RELATION_H
#define RELATION_H

#include <fcntl.h>  // open
#include <unistd.h> // read write
#include "c.h"

extern void relation_create(Oid relNode);
extern int relation_open(Oid relNode);

#endif