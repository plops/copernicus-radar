// implementation

#include "utils.h"

#include "globals.h"

#include "copernicus_00_main.hpp"
#include "copernicus_01_mmap.hpp"
#include "copernicus_08_demangle.hpp"

extern State state;
#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
size_t get_filesize(const char *filename) {
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}
void destroy_mmap() {
  auto rc = munmap(state._mmap_data, state._mmap_filesize);
  if (!((0) == (rc))) {
  }
  assert((0) == (rc));
}
void init_mmap(const char *filename) {
  auto filesize = get_filesize(filename);
  auto fd = open(filename, O_RDONLY, 0);

  if ((-1) == (fd)) {
  }
  assert((-1) != (fd));
  auto data = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
  if ((MAP_FAILED) == (data)) {
  }
  assert((MAP_FAILED) != (data));
  state._mmap_filesize = filesize;
  state._mmap_data = data;
}