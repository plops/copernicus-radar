#ifndef COPERNICUS_04_DECODE_PACKET_H
#define COPERNICUS_04_DECODE_PACKET_H
// header;
#include "utils.h"
#include <cassert>
#include <cmath>
;
void init_sequential_bit_function(sequential_bit_t *seq_state, size_t byte_pos);
void consume_padding_bits(sequential_bit_t *s);
inline int get_bit_rate_code(sequential_bit_t *s);
inline int decode_huffman_brc0(sequential_bit_t *s);
inline int decode_huffman_brc1(sequential_bit_t *s);
inline int decode_huffman_brc2(sequential_bit_t *s);
inline int decode_huffman_brc3(sequential_bit_t *s);
inline int decode_huffman_brc4(sequential_bit_t *s);
int init_decode_packet(int packet_idx, std::complex<float> *output);
#endif