// implementation

#include "utils.h"

#include "globals.h"

#include "copernicus_02_collect_packet_headers.hpp"
#include "copernicus_06_decode_sub_commutated_data.hpp"
#include "copernicus_08_demangle.hpp"

extern State state;
#include <cxxabi.h>
#include <string>
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