// implementation

#include "utils.h"

#include "globals.h"

#include "copernicus_09_python.hpp"

extern State state;
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <string>
// https://pybind11.readthedocs.io/en/stable/advanced/pycpp/numpy.html
;
Matrix::Matrix(size_t rows, size_t cols) : m_rows(rows), m_cols(cols) {
  m_data = new float[((rows) * (cols))];
}
float *Matrix::data() { return m_data; }
size_t Matrix::rows() const { return m_rows; }
size_t Matrix::cols() const { return m_cols; }
void run_embedded_python() {
  py::scoped_interpreter guard{};
  py::exec(R"(
import IPython
print('hello')
)");
}