#ifndef COPERNICUS_05_DECODE_TYPE_AB_PACKET_H
#define COPERNICUS_05_DECODE_TYPE_AB_PACKET_H
#include "utils.h"
;
#include "globals.h"
;
#include "proto2.h"
;
#include <cassert>
;
inline int get_data_type_a_or_b(sequential_bit_t *s);
int init_decode_packet_type_a_or_b(int packet_idx, std::complex<float> *output);
#endif