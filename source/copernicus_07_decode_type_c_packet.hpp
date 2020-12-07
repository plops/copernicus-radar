#ifndef COPERNICUS_07_DECODE_TYPE_C_PACKET_H
#define COPERNICUS_07_DECODE_TYPE_C_PACKET_H
// header;
#include "utils.h"
#include <cassert>
#include <cmath>
;
inline int get_baq3_code(sequential_bit_t *s);
inline int get_baq4_code(sequential_bit_t *s);
inline int get_baq5_code(sequential_bit_t *s);
int init_decode_type_c_packet_baq3(int packet_idx, std::complex<float> *output);
int init_decode_type_c_packet_baq4(int packet_idx, std::complex<float> *output);
int init_decode_type_c_packet_baq5(int packet_idx, std::complex<float> *output);
#endif