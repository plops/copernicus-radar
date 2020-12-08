// implementation

#include "utils.h"

#include "globals.h"

#include "copernicus_09_python.hpp"

extern State state;
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <string>
namespace py = pybind11;
// https://pybind11.readthedocs.io/en/stable/advanced/pycpp/numpy.html
;
Matrix::Matrix(size_t rows, size_t cols) : m_rows(rows), m_cols(cols) {
  m_data = new float[((rows) * (cols))];
}
Matrix::~Matrix() { delete (m_data); }
float *Matrix::data() { return m_data; }
size_t Matrix::rows() const { return m_rows; }
size_t Matrix::cols() const { return m_cols; }
PYBIND11_EMBEDDED_MODULE(mk_matrix, m) {
  py::class_<Matrix>(m, "Matrix", py::buffer_protocol())
      .def(py::init<size_t, size_t>())
      .def("__repr__",
           [](const Matrix &m) {
             auto r = std::string("Matrix(");
             (r) += (std::to_string(m.rows()));
             (r) += (", ");
             (r) += (std::to_string(m.cols()));
             (r) += (")");
             return r;
           })
      .def_buffer([](Matrix &m) -> py::buffer_info {
        return py::buffer_info(m.data(), sizeof(float),
                               py::format_descriptor<float>::format(), 2,
                               {m.rows(), m.cols()},
                               {((sizeof(float)) * (m.cols())), sizeof(float)});
      });
};
void run_embedded_python() {
  py::scoped_interpreter guard{};
  py::exec(R"(
import IPython
import mk_matrix
print('hello')
IPython.start_ipython()
)");
}