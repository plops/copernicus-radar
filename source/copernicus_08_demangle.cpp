
#include "utils.h"

#include "globals.h"

#include "proto2.h"

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