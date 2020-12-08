// implementation

#include "utils.h"

#include "globals.h"

#include "copernicus_09_python.hpp"

extern State state;
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
namespace py = pybind11;
PYBIND11_EMBEDDED_MODULE(copernicus, m) {
  m.attr("_filename") = state._filename;
  m.attr("_start_time") = state._start_time;
  m.attr("_map_ele") = state._map_ele;
};
void run_embedded_python() {
  py::scoped_interpreter guard{};
  py::exec(R"(
import IPython
import copernicus
print('hello')
IPython.start_ipython()
)");
}