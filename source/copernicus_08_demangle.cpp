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
#include <cxxabi.h>
std::string demangle(const std::string name) {
  auto status = -4;
  std::unique_ptr<char, void (*)(void *)> res{
      abi::__cxa_demangle(name.c_str(), nullptr, nullptr, &status), std::free};
  if ((0) == (status)) {
    return res.get();
  } else {
    return name;
  }
}