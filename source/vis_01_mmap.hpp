#ifndef VIS_01_MMAP_H
#define VIS_01_MMAP_H
#include "utils.h"
;
#include "globals.h"
;
#include "proto2.h"
;
#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
;
size_t get_filesize(const char *filename);
void destroy_mmap();
void init_mmap(const char *filename);
#endif