#ifndef COPERNICUS_09_PYTHON_H
#define COPERNICUS_09_PYTHON_H
// header;
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <string>
;
class Matrix {
public:
  Matrix(size_t rows, size_t cols);
  ~Matrix();
  float *data();
  size_t rows() const;
  size_t cols() const;

private:
  size_t m_rows, m_cols;
  float *m_data;
};
void run_embedded_python();
#endif