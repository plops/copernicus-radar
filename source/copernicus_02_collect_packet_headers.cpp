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
#include <array>
#include <cstring>
#include <iostream>
#include <vector>
void destroy_collect_packet_headers() {}
void init_collect_packet_headers() {

  (std::cout)
      << (std::setw(10))
      << (std::chrono::high_resolution_clock::now().time_since_epoch().count())
      << (" ") << (std::this_thread::get_id()) << (" ") << (__FILE__) << (":")
      << (__LINE__) << (" ") << (__func__) << (" ") << ("collect") << (" ")
      << (std::setw(8)) << (" state._mmap_data='") << (state._mmap_data)
      << ("::") << (demangle(typeid(state._mmap_data).name())) << ("'")
      << (std::endl) << (std::flush);
  size_t offset = 0;
  while ((offset) < (state._mmap_filesize)) {
    auto p = ((offset) + (static_cast<uint8_t *>(state._mmap_data)));
    auto data_length = ((((0x1) * (p[5]))) + (((0x100) * (((0xFF) & (p[4]))))));
    std::array<uint8_t, 62 + 6> data_chunk;
    memcpy(data_chunk.data(), p, ((62) + (6)));
    state._header_offset.push_back(offset);
    state._header_data.push_back(data_chunk);
    (offset) += (((6) + (1) + (data_length)));
  }
}