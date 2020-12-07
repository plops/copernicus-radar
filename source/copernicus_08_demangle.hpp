#ifndef COPERNICUS_08_DEMANGLE_H
#define COPERNICUS_08_DEMANGLE_H
#include "utils.h"
;
#include "globals.h"
;
#include "proto2.h"
;
#include <cxxabi.h>
;
std::string demangle(const std::string name);
#endif