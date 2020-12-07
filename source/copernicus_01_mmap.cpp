// implementation

#include "utils.h"

#include "globals.h"

#include "copernicus_00_main.hpp"
#include "copernicus_01_mmap.hpp"
#include "copernicus_02_collect_packet_headers.hpp"
#include "copernicus_03_process_packet_headers.hpp"
#include "copernicus_04_decode_packet.hpp"
#include "copernicus_05_decode_type_ab_packet.hpp"
#include "copernicus_06_decode_sub_commutated_data.hpp"
#include "copernicus_07_decode_type_c_packet.hpp"
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

    (std::cout) << (std::setw(10))
                << (std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count())
                << (" ") << (std::this_thread::get_id()) << (" ") << (__FILE__)
                << (":") << (__LINE__) << (" ") << (__func__) << (" ")
                << ("fail munmap") << (" ") << (std::setw(8)) << (" rc='")
                << (rc) << ("::") << (demangle(typeid(rc).name())) << ("'")
                << (std::endl) << (std::flush);
  }
  assert((0) == (rc));
}
void init_mmap(const char *filename) {
  auto filesize = get_filesize(filename);
  auto fd = open(filename, O_RDONLY, 0);

  (std::cout)
      << (std::setw(10))
      << (std::chrono::high_resolution_clock::now().time_since_epoch().count())
      << (" ") << (std::this_thread::get_id()) << (" ") << (__FILE__) << (":")
      << (__LINE__) << (" ") << (__func__) << (" ") << ("size") << (" ")
      << (std::setw(8)) << (" filesize='") << (filesize) << ("::")
      << (demangle(typeid(filesize).name())) << ("'") << (std::setw(8))
      << (" filename='") << (filename) << ("::")
      << (demangle(typeid(filename).name())) << ("'") << (std::endl)
      << (std::flush);
  if ((-1) == (fd)) {

    (std::cout) << (std::setw(10))
                << (std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count())
                << (" ") << (std::this_thread::get_id()) << (" ") << (__FILE__)
                << (":") << (__LINE__) << (" ") << (__func__) << (" ")
                << ("fail open") << (" ") << (std::setw(8)) << (" fd='") << (fd)
                << ("::") << (demangle(typeid(fd).name())) << ("'")
                << (std::setw(8)) << (" filename='") << (filename) << ("::")
                << (demangle(typeid(filename).name())) << ("'") << (std::endl)
                << (std::flush);
  }
  assert((-1) != (fd));
  auto data = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
  if ((MAP_FAILED) == (data)) {

    (std::cout) << (std::setw(10))
                << (std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count())
                << (" ") << (std::this_thread::get_id()) << (" ") << (__FILE__)
                << (":") << (__LINE__) << (" ") << (__func__) << (" ")
                << ("fail mmap") << (" ") << (std::setw(8)) << (" data='")
                << (data) << ("::") << (demangle(typeid(data).name())) << ("'")
                << (std::endl) << (std::flush);
  }
  assert((MAP_FAILED) != (data));
  state._mmap_filesize = filesize;
  state._mmap_data = data;
}