#ifndef COPERNICUS_06_DECODE_SUB_COMMUTATED_DATA_H
#define COPERNICUS_06_DECODE_SUB_COMMUTATED_DATA_H
// header;
#include <cassert>
#include <cstring>
#include <fstream>
;
void init_sub_commutated_data_decoder();
bool feed_sub_commutated_data_decoder(uint16_t word, int idx,
                                      int space_packet_count);
#endif