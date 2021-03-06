// implementation

#include "utils.h"

#include "globals.h"

#include "copernicus_00_main.hpp"
#include "copernicus_01_mmap.hpp"
#include "copernicus_02_collect_packet_headers.hpp"
#include "copernicus_04_decode_packet.hpp"
#include "copernicus_05_decode_type_ab_packet.hpp"
#include "copernicus_06_decode_sub_commutated_data.hpp"
#include "copernicus_08_demangle.hpp"
#include "copernicus_09_python.hpp"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
State state = {};
int main(int argc, char **argv) {
  state._start_time =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  state._filename =
      "/home/martin/Downloads/"
      "s1a-s3-raw-s-hh-20210221t213548-20210221t213613-036693-044fed.dat";
  if ((2) == (argc)) {
    state._filename = argv[1];
  }
  init_mmap(state._filename);
  init_collect_packet_headers();
  auto packet_idx = 0;
  std::unordered_map<int, int> map_ele;
  std::unordered_map<int, int> map_cal;
  std::unordered_map<int, int> map_sig;
  auto cal_count = 0;
  init_sub_commutated_data_decoder();
  remove("./o_anxillary.csv");
  for (auto &e : state._header_data) {
    // count number of packets in packet_idx. map_sig contains histogram of
    // signal_types (without distinction between calibration and signal
    // packets). count number of calibration packets in cal_count. map_cal
    // contains a histogram of calibration type in calibration packets. map_ele
    // contains a count of quads for each elevation beam address.
    ;
    auto offset = state._header_offset[packet_idx];
    auto p = ((offset) + (static_cast<uint8_t *>(state._mmap_data)));
    auto cal_p = ((0x1) & ((p[59]) >> (7)));
    auto ele = ((0xF) & ((p[60]) >> (4)));
    auto cal_type = ((ele) & (7));
    auto number_of_quads =
        ((((0x1) * (p[66]))) + (((0x100) * (((0xFF) & (p[65]))))));
    auto baq_mode = ((0x1F) & ((p[37]) >> (0)));
    auto test_mode = ((0x7) & ((p[21]) >> (4)));
    auto space_packet_count =
        ((((0x1) * (p[32]))) + (((0x100) * (p[31]))) + (((0x10000) * (p[30]))) +
         (((0x1000000) * (((0xFF) & (p[29]))))));
    auto sub_index = ((0xFF) & ((p[26]) >> (0)));
    auto sub_data = ((((0x1) * (p[28]))) + (((0x100) * (((0xFF) & (p[27]))))));
    auto signal_type = ((0xF) & ((p[63]) >> (4)));
    feed_sub_commutated_data_decoder(sub_data, sub_index, space_packet_count);
    (map_sig[signal_type])++;
    if (cal_p) {
      (cal_count)++;
      (map_cal[((ele) & (7))])++;
      (state._map_cal[((ele) & (7))])++;
      // print something like this:
      // 2809021335 ../copernicus_00_main.cpp:70 main cal   cal_p=1 cal_type=4
      // number_of_quads=2561 baq_mode=0 test_mode=0
      ;
      std::setprecision(3);
      (std::cout) << (std::setw(10))
                  << (((std::chrono::high_resolution_clock::now()
                            .time_since_epoch()
                            .count()) -
                       (state._start_time)))
                  << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                  << (__func__) << (" ") << ("cal") << (" ") << (std::setw(8))
                  << (" cal_p=") << (cal_p) << (std::setw(8)) << (" cal_type=")
                  << (cal_type) << (std::setw(8)) << (" number_of_quads=")
                  << (number_of_quads) << (std::setw(8)) << (" baq_mode=")
                  << (baq_mode) << (std::setw(8)) << (" test_mode=")
                  << (test_mode) << (std::endl);
    } else {
      (map_ele[ele]) += (number_of_quads);
      (state._map_ele[ele]) += (number_of_quads);
    }
    (packet_idx)++;
  };
  for (auto &cal : map_cal) {
    // print number of packets with each calibration type, like: cal_type=7
    // number_of_cal=116
    ;
    auto number_of_cal = cal.second;
    auto cal_type = cal.first;
    std::setprecision(3);
    (std::cout) << (std::setw(10))
                << (((std::chrono::high_resolution_clock::now()
                          .time_since_epoch()
                          .count()) -
                     (state._start_time)))
                << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                << (__func__) << (" ") << ("map_cal") << (" ") << (std::setw(8))
                << (" cal_type=") << (cal_type) << (std::setw(8))
                << (" number_of_cal=") << (number_of_cal) << (std::endl);
  };
  for (auto &sig : map_sig) {
    // print histogram of signal packets, like: sig_type=0 number_of_sig=48125
    ;
    auto number_of_sig = sig.second;
    auto sig_type = sig.first;
    std::setprecision(3);
    (std::cout) << (std::setw(10))
                << (((std::chrono::high_resolution_clock::now()
                          .time_since_epoch()
                          .count()) -
                     (state._start_time)))
                << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                << (__func__) << (" ") << ("map_sig") << (" ") << (std::setw(8))
                << (" sig_type=") << (sig_type) << (std::setw(8))
                << (" number_of_sig=") << (number_of_sig) << (std::endl);
  };
  auto ma = (-1.0f);
  auto ma_ele = -1;
  for (auto &elevation : map_ele) {
    auto number_of_Mquads = ((elevation.second) / ((1.0e+6f)));
    auto elevation_beam_address = elevation.first;
    if ((ma) < (number_of_Mquads)) {
      ma = number_of_Mquads;
      ma_ele = elevation_beam_address;
    }
    // show the number of quads for each elevation beam address, like:
    // elevation_beam_address=2 number_of_Mquads=526.46
    ;
    std::setprecision(3);
    (std::cout) << (std::setw(10))
                << (((std::chrono::high_resolution_clock::now()
                          .time_since_epoch()
                          .count()) -
                     (state._start_time)))
                << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                << (__func__) << (" ") << ("map_ele") << (" ") << (std::setw(8))
                << (" elevation_beam_address=") << (elevation_beam_address)
                << (std::setw(8)) << (" number_of_Mquads=")
                << (number_of_Mquads) << (std::endl);
  };
  // show the elevation beam address with the largest number of acquired quads,
  // like: ma_ele=2     ma=526.46
  ;
  std::setprecision(3);
  (std::cout) << (std::setw(10))
              << (((std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count()) -
                   (state._start_time)))
              << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
              << (__func__) << (" ") << ("largest ele") << (" ")
              << (std::setw(8)) << (" ma_ele=") << (ma_ele) << (std::setw(8))
              << (" ma=") << (ma) << (std::endl);
  std::setprecision(3);
  (std::cout) << (std::setw(10))
              << (((std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count()) -
                   (state._start_time)))
              << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
              << (__func__) << (" ") << ("calibrations") << (" ")
              << (std::setw(8)) << (" cal_count=") << (cal_count)
              << (std::endl);
  auto mi_data_delay = 10000000;
  auto ma_data_delay = -1;
  auto ma_data_end = -1;
  auto ele_number_echoes = 0;
  {
    std::unordered_map<int, int> map_azi;
    auto packet_idx = 0;
    // go through all packets and report the largest and smallest start sample
    // for range data, the largest number of samples. also the number of
    // recorded echoes is reported (data length in azimuth direction). this
    // should be enough information to allocate a 2d array for all the raw data.
    // output looks like this:
    // data_delay  mi_data_delay=3548 ma_data_delay=3641 ma_data_end=25479
    // ele_number_echoes=48141
    ;
    // map_azi counts the number of quads for each azimuth beam address.
    ;
    for (auto &e : state._header_data) {
      auto offset = state._header_offset[packet_idx];
      auto p = ((offset) + (static_cast<uint8_t *>(state._mmap_data)));
      auto ele = ((0xF) & ((p[60]) >> (4)));
      auto azi = ((((0x1) * (p[61]))) + (((0x100) * (((0x3) & (p[60]))))));
      auto number_of_quads =
          ((((0x1) * (p[66]))) + (((0x100) * (((0xFF) & (p[65]))))));
      auto cal_p = ((0x1) & ((p[59]) >> (7)));
      auto data_delay = ((40) + (((((0x1) * (p[55]))) + (((0x100) * (p[54]))) +
                                  (((0x10000) * (((0xFF) & (p[53]))))))));
      if (!(cal_p)) {
        if ((ele) == (ma_ele)) {
          (ele_number_echoes)++;
          if ((data_delay) < (mi_data_delay)) {
            mi_data_delay = data_delay;
          }
          if ((ma_data_delay) < (data_delay)) {
            ma_data_delay = data_delay;
          }
          auto v = ((data_delay) + (((2) * (number_of_quads))));
          if ((ma_data_end) < (v)) {
            ma_data_end = v;
          }
          (map_azi[azi]) += (number_of_quads);
        }
      }
      (packet_idx)++;
    };
    std::setprecision(3);
    (std::cout) << (std::setw(10))
                << (((std::chrono::high_resolution_clock::now()
                          .time_since_epoch()
                          .count()) -
                     (state._start_time)))
                << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                << (__func__) << (" ") << ("data_delay") << (" ")
                << (std::setw(8)) << (" mi_data_delay=") << (mi_data_delay)
                << (std::setw(8)) << (" ma_data_delay=") << (ma_data_delay)
                << (std::setw(8)) << (" ma_data_end=") << (ma_data_end)
                << (std::setw(8)) << (" ele_number_echoes=")
                << (ele_number_echoes) << (std::endl);
    for (auto &azi : map_azi) {
      auto number_of_Mquads = ((azi.second) / ((1.0e+6f)));
      auto azi_beam_address = azi.first;
      std::setprecision(3);
      (std::cout) << (std::setw(10))
                  << (((std::chrono::high_resolution_clock::now()
                            .time_since_epoch()
                            .count()) -
                       (state._start_time)))
                  << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                  << (__func__) << (" ") << ("map_azi") << (" ")
                  << (std::setw(8)) << (" azi_beam_address=")
                  << (azi_beam_address) << (std::setw(8))
                  << (" number_of_Mquads=") << (number_of_Mquads)
                  << (std::endl);
    };
  }
  // here would be the place to allocate a 2d array for all the azimuth x range
  // samples.
  // i usually run into trouble here because i have not enough ram (or the
  // decoder crashes).
  ;
  ele_number_echoes = 512;
  std::setprecision(3);
  (std::cout) << (std::setw(10))
              << (((std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count()) -
                   (state._start_time)))
              << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
              << (__func__) << (" ") << ("start big allocation") << (" ")
              << (std::setw(8))
              << (" ((ma_data_end)+(((ma_data_delay)-(mi_data_delay))))=")
              << (((ma_data_end) + (((ma_data_delay) - (mi_data_delay)))))
              << (std::setw(8)) << (" ele_number_echoes=")
              << (ele_number_echoes) << (std::endl);
  auto n0 = ((ma_data_end) + (((ma_data_delay) - (mi_data_delay))));
  auto sar_image = new std::complex<float>[((n0) * (ele_number_echoes))];
  std::setprecision(3);
  (std::cout) << (std::setw(10))
              << (((std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count()) -
                   (state._start_time)))
              << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
              << (__func__) << (" ") << ("end big allocation") << (" ")
              << (std::setw(8)) << (" (((1.00e-6f))*(n0)*(ele_number_echoes))=")
              << ((((1.00e-6f)) * (n0) * (ele_number_echoes))) << (std::endl);
  remove("./o_all.csv");
  remove("./o_range.csv");
  remove("./o_cal_range.csv");
  auto cal_n0 = 6000;
  auto cal_iter = 0;
  auto cal_image = new std::complex<float>[((cal_n0) * (cal_count))];
  {
    auto array_packet_version_number = std::vector<float>();
    auto array_packet_type = std::vector<float>();
    auto array_secondary_header_flag = std::vector<float>();
    auto array_application_process_id_process_id = std::vector<float>();
    auto array_application_process_id_packet_category = std::vector<float>();
    auto array_sequence_flags = std::vector<float>();
    auto array_sequence_count = std::vector<float>();
    auto array_data_length = std::vector<float>();
    auto array_coarse_time = std::vector<float>();
    auto array_fine_time = std::vector<float>();
    auto array_sync_marker = std::vector<float>();
    auto array_data_take_id = std::vector<float>();
    auto array_ecc_number = std::vector<float>();
    auto array_ignore_0 = std::vector<float>();
    auto array_test_mode = std::vector<float>();
    auto array_rx_channel_id = std::vector<float>();
    auto array_instrument_configuration_id = std::vector<float>();
    auto array_sub_commutated_index = std::vector<float>();
    auto array_sub_commutated_data = std::vector<float>();
    auto array_space_packet_count = std::vector<float>();
    auto array_pri_count = std::vector<float>();
    auto array_error_flag = std::vector<float>();
    auto array_ignore_1 = std::vector<float>();
    auto array_baq_mode = std::vector<float>();
    auto array_baq_block_length = std::vector<float>();
    auto array_ignore_2 = std::vector<float>();
    auto array_range_decimation = std::vector<float>();
    auto array_rx_gain = std::vector<float>();
    auto array_tx_ramp_rate_polarity = std::vector<float>();
    auto array_tx_ramp_rate_magnitude = std::vector<float>();
    auto array_tx_pulse_start_frequency_polarity = std::vector<float>();
    auto array_tx_pulse_start_frequency_magnitude = std::vector<float>();
    auto array_tx_pulse_length = std::vector<float>();
    auto array_ignore_3 = std::vector<float>();
    auto array_rank = std::vector<float>();
    auto array_pulse_repetition_interval = std::vector<float>();
    auto array_sampling_window_start_time = std::vector<float>();
    auto array_sampling_window_length = std::vector<float>();
    auto array_sab_ssb_calibration_p = std::vector<float>();
    auto array_sab_ssb_polarisation = std::vector<float>();
    auto array_sab_ssb_temp_comp = std::vector<float>();
    auto array_sab_ssb_ignore_0 = std::vector<float>();
    auto array_sab_ssb_elevation_beam_address = std::vector<float>();
    auto array_sab_ssb_ignore_1 = std::vector<float>();
    auto array_sab_ssb_azimuth_beam_address = std::vector<float>();
    auto array_ses_ssb_cal_mode = std::vector<float>();
    auto array_ses_ssb_ignore_0 = std::vector<float>();
    auto array_ses_ssb_tx_pulse_number = std::vector<float>();
    auto array_ses_ssb_signal_type = std::vector<float>();
    auto array_ses_ssb_ignore_1 = std::vector<float>();
    auto array_ses_ssb_swap = std::vector<float>();
    auto array_ses_ssb_swath_number = std::vector<float>();
    auto array_number_of_quads = std::vector<float>();
    auto array_ignore_4 = std::vector<float>();
    auto packet_idx = 0;
    auto ele_count = 0;
    for (auto &e : state._header_data) {
      auto offset = state._header_offset[packet_idx];
      auto p = ((offset) + (static_cast<uint8_t *>(state._mmap_data)));
      auto azi = ((((0x1) * (p[61]))) + (((0x100) * (((0x3) & (p[60]))))));
      auto baq_n = ((0xFF) & ((p[38]) >> (0)));
      auto baqmod = ((0x1F) & ((p[37]) >> (0)));
      auto cal_mode = ((0x3) & ((p[62]) >> (6)));
      auto cal_p = ((0x1) & ((p[59]) >> (7)));
      auto ecc = ((0xFF) & ((p[20]) >> (0)));
      auto ele = ((0xF) & ((p[60]) >> (4)));
      auto cal_type = ((ele) & (7));
      auto err = ((0x1) & ((p[37]) >> (7)));
      auto number_of_quads =
          ((((0x1) * (p[66]))) + (((0x100) * (((0xFF) & (p[65]))))));
      auto pol = ((0x7) & ((p[59]) >> (4)));
      auto pri_count =
          ((((0x1) * (p[36]))) + (((0x100) * (p[35]))) +
           (((0x10000) * (p[34]))) + (((0x1000000) * (((0xFF) & (p[33]))))));
      auto rank = ((0x1F) & ((p[49]) >> (0)));
      auto rx = ((0xF) & ((p[21]) >> (0)));
      auto rgdec = ((0xFF) & ((p[40]) >> (0)));
      auto signal_type = ((0xF) & ((p[63]) >> (4)));
      auto space_packet_count =
          ((((0x1) * (p[32]))) + (((0x100) * (p[31]))) +
           (((0x10000) * (p[30]))) + (((0x1000000) * (((0xFF) & (p[29]))))));
      auto swath = ((0xFF) & ((p[64]) >> (0)));
      auto swl = ((((0x1) * (p[58]))) + (((0x100) * (p[57]))) +
                  (((0x10000) * (((0xFF) & (p[56]))))));
      auto swst = ((((0x1) * (p[55]))) + (((0x100) * (p[54]))) +
                   (((0x10000) * (((0xFF) & (p[53]))))));
      auto sync_marker =
          ((((0x1) * (p[15]))) + (((0x100) * (p[14]))) +
           (((0x10000) * (p[13]))) + (((0x1000000) * (((0xFF) & (p[12]))))));
      auto tstmod = ((0x7) & ((p[21]) >> (4)));
      auto data_delay = ((40) + (((((0x1) * (p[55]))) + (((0x100) * (p[54]))) +
                                  (((0x10000) * (((0xFF) & (p[53]))))))));
      auto txprr_p = ((0x1) & ((p[42]) >> (7)));
      auto txprr_m = ((((0x1) * (p[43]))) + (((0x100) * (((0x7F) & (p[42]))))));
      auto txpsf_p = ((0x1) & ((p[44]) >> (7)));
      auto txpsf_m = ((((0x1) * (p[45]))) + (((0x100) * (((0x7F) & (p[44]))))));
      auto txpl_ = ((((0x1) * (p[48]))) + (((0x100) * (p[47]))) +
                    (((0x10000) * (((0xFF) & (p[46]))))));
      auto fref = (37.53472f);
      auto txprr_ = ((pow(-1, txprr_p)) * (txprr_m));
      auto txprr = ((((((fref) * (fref))) / (2097152))) *
                    (pow((-1.0f), txprr_p)) * (txprr_m));
      auto txpsf =
          ((((txprr) / (((fref) * (4))))) +
           (((((fref) / (16384))) * (pow((-1.0f), txpsf_p)) * (txpsf_m))));
      auto txpl = ((static_cast<double>(txpl_)) / (fref));
      assert((sync_marker) == (0x352EF853));
      {
        auto packet_version_number = ((0x7) & ((p[0]) >> (5)));
        auto packet_type = ((0x1) & ((p[0]) >> (4)));
        auto secondary_header_flag = ((0x1) & ((p[0]) >> (3)));
        auto application_process_id_process_id =
            (((((0xF0) & (p[1]))) >> (4)) + (((0x10) * (((0x7) & (p[0]))))));
        auto application_process_id_packet_category = ((0xF) & ((p[1]) >> (0)));
        auto sequence_flags = ((0x3) & ((p[2]) >> (6)));
        auto sequence_count =
            ((((0x1) * (p[3]))) + (((0x100) * (((0x3F) & (p[2]))))));
        auto data_length =
            ((((0x1) * (p[5]))) + (((0x100) * (((0xFF) & (p[4]))))));
        auto coarse_time =
            ((((0x1) * (p[9]))) + (((0x100) * (p[8]))) +
             (((0x10000) * (p[7]))) + (((0x1000000) * (((0xFF) & (p[6]))))));
        auto fine_time =
            ((((0x1) * (p[11]))) + (((0x100) * (((0xFF) & (p[10]))))));
        auto sync_marker =
            ((((0x1) * (p[15]))) + (((0x100) * (p[14]))) +
             (((0x10000) * (p[13]))) + (((0x1000000) * (((0xFF) & (p[12]))))));
        auto data_take_id =
            ((((0x1) * (p[19]))) + (((0x100) * (p[18]))) +
             (((0x10000) * (p[17]))) + (((0x1000000) * (((0xFF) & (p[16]))))));
        auto ecc_number = ((0xFF) & ((p[20]) >> (0)));
        auto ignore_0 = ((0x1) & ((p[21]) >> (7)));
        auto test_mode = ((0x7) & ((p[21]) >> (4)));
        auto rx_channel_id = ((0xF) & ((p[21]) >> (0)));
        auto instrument_configuration_id =
            ((((0x1) * (p[25]))) + (((0x100) * (p[24]))) +
             (((0x10000) * (p[23]))) + (((0x1000000) * (((0xFF) & (p[22]))))));
        auto sub_commutated_index = ((0xFF) & ((p[26]) >> (0)));
        auto sub_commutated_data =
            ((((0x1) * (p[28]))) + (((0x100) * (((0xFF) & (p[27]))))));
        auto space_packet_count =
            ((((0x1) * (p[32]))) + (((0x100) * (p[31]))) +
             (((0x10000) * (p[30]))) + (((0x1000000) * (((0xFF) & (p[29]))))));
        auto pri_count =
            ((((0x1) * (p[36]))) + (((0x100) * (p[35]))) +
             (((0x10000) * (p[34]))) + (((0x1000000) * (((0xFF) & (p[33]))))));
        auto error_flag = ((0x1) & ((p[37]) >> (7)));
        auto ignore_1 = ((0x3) & ((p[37]) >> (5)));
        auto baq_mode = ((0x1F) & ((p[37]) >> (0)));
        auto baq_block_length = ((0xFF) & ((p[38]) >> (0)));
        auto ignore_2 = ((0xFF) & ((p[39]) >> (0)));
        auto range_decimation = ((0xFF) & ((p[40]) >> (0)));
        auto rx_gain = ((0xFF) & ((p[41]) >> (0)));
        auto tx_ramp_rate_polarity = ((0x1) & ((p[42]) >> (7)));
        auto tx_ramp_rate_magnitude =
            ((((0x1) * (p[43]))) + (((0x100) * (((0x7F) & (p[42]))))));
        auto tx_pulse_start_frequency_polarity = ((0x1) & ((p[44]) >> (7)));
        auto tx_pulse_start_frequency_magnitude =
            ((((0x1) * (p[45]))) + (((0x100) * (((0x7F) & (p[44]))))));
        auto tx_pulse_length = ((((0x1) * (p[48]))) + (((0x100) * (p[47]))) +
                                (((0x10000) * (((0xFF) & (p[46]))))));
        auto ignore_3 = ((0x7) & ((p[49]) >> (5)));
        auto rank = ((0x1F) & ((p[49]) >> (0)));
        auto pulse_repetition_interval =
            ((((0x1) * (p[52]))) + (((0x100) * (p[51]))) +
             (((0x10000) * (((0xFF) & (p[50]))))));
        auto sampling_window_start_time =
            ((((0x1) * (p[55]))) + (((0x100) * (p[54]))) +
             (((0x10000) * (((0xFF) & (p[53]))))));
        auto sampling_window_length =
            ((((0x1) * (p[58]))) + (((0x100) * (p[57]))) +
             (((0x10000) * (((0xFF) & (p[56]))))));
        auto sab_ssb_calibration_p = ((0x1) & ((p[59]) >> (7)));
        auto sab_ssb_polarisation = ((0x7) & ((p[59]) >> (4)));
        auto sab_ssb_temp_comp = ((0x3) & ((p[59]) >> (2)));
        auto sab_ssb_ignore_0 = ((0x3) & ((p[59]) >> (0)));
        auto sab_ssb_elevation_beam_address = ((0xF) & ((p[60]) >> (4)));
        auto sab_ssb_ignore_1 = ((0x3) & ((p[60]) >> (2)));
        auto sab_ssb_azimuth_beam_address =
            ((((0x1) * (p[61]))) + (((0x100) * (((0x3) & (p[60]))))));
        auto ses_ssb_cal_mode = ((0x3) & ((p[62]) >> (6)));
        auto ses_ssb_ignore_0 = ((0x1) & ((p[62]) >> (5)));
        auto ses_ssb_tx_pulse_number = ((0x1F) & ((p[62]) >> (0)));
        auto ses_ssb_signal_type = ((0xF) & ((p[63]) >> (4)));
        auto ses_ssb_ignore_1 = ((0x7) & ((p[63]) >> (1)));
        auto ses_ssb_swap = ((0x1) & ((p[63]) >> (0)));
        auto ses_ssb_swath_number = ((0xFF) & ((p[64]) >> (0)));
        auto number_of_quads =
            ((((0x1) * (p[66]))) + (((0x100) * (((0xFF) & (p[65]))))));
        auto ignore_4 = ((0xFF) & ((p[67]) >> (0)));
        array_packet_version_number.push_back(packet_version_number);
        array_packet_type.push_back(packet_type);
        array_secondary_header_flag.push_back(secondary_header_flag);
        array_application_process_id_process_id.push_back(
            application_process_id_process_id);
        array_application_process_id_packet_category.push_back(
            application_process_id_packet_category);
        array_sequence_flags.push_back(sequence_flags);
        array_sequence_count.push_back(sequence_count);
        array_data_length.push_back(data_length);
        array_coarse_time.push_back(coarse_time);
        array_fine_time.push_back(fine_time);
        array_sync_marker.push_back(sync_marker);
        array_data_take_id.push_back(data_take_id);
        array_ecc_number.push_back(ecc_number);
        array_ignore_0.push_back(ignore_0);
        array_test_mode.push_back(test_mode);
        array_rx_channel_id.push_back(rx_channel_id);
        array_instrument_configuration_id.push_back(
            instrument_configuration_id);
        array_sub_commutated_index.push_back(sub_commutated_index);
        array_sub_commutated_data.push_back(sub_commutated_data);
        array_space_packet_count.push_back(space_packet_count);
        array_pri_count.push_back(pri_count);
        array_error_flag.push_back(error_flag);
        array_ignore_1.push_back(ignore_1);
        array_baq_mode.push_back(baq_mode);
        array_baq_block_length.push_back(baq_block_length);
        array_ignore_2.push_back(ignore_2);
        array_range_decimation.push_back(range_decimation);
        array_rx_gain.push_back(rx_gain);
        array_tx_ramp_rate_polarity.push_back(tx_ramp_rate_polarity);
        array_tx_ramp_rate_magnitude.push_back(tx_ramp_rate_magnitude);
        array_tx_pulse_start_frequency_polarity.push_back(
            tx_pulse_start_frequency_polarity);
        array_tx_pulse_start_frequency_magnitude.push_back(
            tx_pulse_start_frequency_magnitude);
        array_tx_pulse_length.push_back(tx_pulse_length);
        array_ignore_3.push_back(ignore_3);
        array_rank.push_back(rank);
        array_pulse_repetition_interval.push_back(pulse_repetition_interval);
        array_sampling_window_start_time.push_back(sampling_window_start_time);
        array_sampling_window_length.push_back(sampling_window_length);
        array_sab_ssb_calibration_p.push_back(sab_ssb_calibration_p);
        array_sab_ssb_polarisation.push_back(sab_ssb_polarisation);
        array_sab_ssb_temp_comp.push_back(sab_ssb_temp_comp);
        array_sab_ssb_ignore_0.push_back(sab_ssb_ignore_0);
        array_sab_ssb_elevation_beam_address.push_back(
            sab_ssb_elevation_beam_address);
        array_sab_ssb_ignore_1.push_back(sab_ssb_ignore_1);
        array_sab_ssb_azimuth_beam_address.push_back(
            sab_ssb_azimuth_beam_address);
        array_ses_ssb_cal_mode.push_back(ses_ssb_cal_mode);
        array_ses_ssb_ignore_0.push_back(ses_ssb_ignore_0);
        array_ses_ssb_tx_pulse_number.push_back(ses_ssb_tx_pulse_number);
        array_ses_ssb_signal_type.push_back(ses_ssb_signal_type);
        array_ses_ssb_ignore_1.push_back(ses_ssb_ignore_1);
        array_ses_ssb_swap.push_back(ses_ssb_swap);
        array_ses_ssb_swath_number.push_back(ses_ssb_swath_number);
        array_number_of_quads.push_back(number_of_quads);
        array_ignore_4.push_back(ignore_4);
      }
      try {
        if (cal_p) {
          init_decode_packet_type_a_or_b(
              packet_idx, ((cal_image) + (((cal_n0) * (cal_iter)))));
          {
            std::ofstream outfile;
            outfile.open("./o_cal_range.csv",
                         ((std::ios_base::out) | (std::ios_base::app)));
            if ((0) == (outfile.tellp())) {
              (outfile)
                  << ("azi,baq_n,baqmod,cal_iter,ele_count,cal_mode,cal_p,cal_"
                      "type,data_delay,number_of_quads,offset,packet_idx,pol,"
                      "pri_count,rank,rgdec,rx,signal_type,space_packet_count,"
                      "swath,swl,swst,tstmod,txpl,txpl_,txprr,txprr_,txpsf")
                  << (std::endl);
            }
            (outfile) << (azi) << (",") << (baq_n) << (",") << (baqmod) << (",")
                      << (cal_iter) << (",") << (ele_count) << (",")
                      << (cal_mode) << (",") << (cal_p) << (",") << (cal_type)
                      << (",") << (data_delay) << (",") << (number_of_quads)
                      << (",") << (offset) << (",") << (packet_idx) << (",")
                      << (pol) << (",") << (pri_count) << (",") << (rank)
                      << (",") << (rgdec) << (",") << (rx) << (",")
                      << (signal_type) << (",") << (space_packet_count) << (",")
                      << (swath) << (",") << (swl) << (",") << (swst) << (",")
                      << (tstmod) << (",") << (txpl) << (",") << (txpl_)
                      << (",") << (txprr) << (",") << (txprr_) << (",")
                      << (txpsf) << (std::endl);
            outfile.close();
          }
          (cal_iter)++;
        } else {
          if ((ele) == (ma_ele)) {
            auto n = init_decode_packet(
                packet_idx,
                ((sar_image) + (((((data_delay) - (mi_data_delay))) +
                                 (((n0) * (ele_count)))))));
            if (!((n) == (((2) * (number_of_quads))))) {
              std::setprecision(3);
              (std::cout) << (std::setw(10))
                          << (((std::chrono::high_resolution_clock::now()
                                    .time_since_epoch()
                                    .count()) -
                               (state._start_time)))
                          << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                          << (__func__) << (" ")
                          << ("unexpected number of quads") << (" ")
                          << (std::setw(8)) << (" n=") << (n) << (std::setw(8))
                          << (" number_of_quads=") << (number_of_quads)
                          << (std::endl);
            }
            {
              std::ofstream outfile;
              outfile.open("./o_range.csv",
                           ((std::ios_base::out) | (std::ios_base::app)));
              if ((0) == (outfile.tellp())) {
                (outfile) << ("azi,baq_n,baqmod,cal_iter,cal_mode,cal_p,data_"
                              "delay,ele,ele_count,number_of_quads,offset,"
                              "packet_idx,pol,pri_count,rank,rx,rgdec,signal_"
                              "type,space_packet_count,swath,swl,swst,tstmod,"
                              "txpl,txpl_,txprr,txprr_,txpsf")
                          << (std::endl);
              }
              (outfile) << (azi) << (",") << (baq_n) << (",") << (baqmod)
                        << (",") << (cal_iter) << (",") << (cal_mode) << (",")
                        << (cal_p) << (",") << (data_delay) << (",") << (ele)
                        << (",") << (ele_count) << (",") << (number_of_quads)
                        << (",") << (offset) << (",") << (packet_idx) << (",")
                        << (pol) << (",") << (pri_count) << (",") << (rank)
                        << (",") << (rx) << (",") << (rgdec) << (",")
                        << (signal_type) << (",") << (space_packet_count)
                        << (",") << (swath) << (",") << (swl) << (",") << (swst)
                        << (",") << (tstmod) << (",") << (txpl) << (",")
                        << (txpl_) << (",") << (txprr) << (",") << (txprr_)
                        << (",") << (txpsf) << (std::endl);
              outfile.close();
            }
            (ele_count)++;
          }
        }
      } catch (std::out_of_range e) {
        std::setprecision(3);
        (std::cout) << (std::setw(10))
                    << (((std::chrono::high_resolution_clock::now()
                              .time_since_epoch()
                              .count()) -
                         (state._start_time)))
                    << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                    << (__func__) << (" ") << ("exception") << (" ")
                    << (std::setw(8)) << (" packet_idx=") << (packet_idx)
                    << (std::setw(8)) << (" static_cast<int>(cal_p)=")
                    << (static_cast<int>(cal_p)) << (std::endl);
        state._packet_header["packet_version_number"] =
            array_packet_version_number;
        state._packet_header["packet_type"] = array_packet_type;
        state._packet_header["secondary_header_flag"] =
            array_secondary_header_flag;
        state._packet_header["application_process_id_process_id"] =
            array_application_process_id_process_id;
        state._packet_header["application_process_id_packet_category"] =
            array_application_process_id_packet_category;
        state._packet_header["sequence_flags"] = array_sequence_flags;
        state._packet_header["sequence_count"] = array_sequence_count;
        state._packet_header["data_length"] = array_data_length;
        state._packet_header["coarse_time"] = array_coarse_time;
        state._packet_header["fine_time"] = array_fine_time;
        state._packet_header["sync_marker"] = array_sync_marker;
        state._packet_header["data_take_id"] = array_data_take_id;
        state._packet_header["ecc_number"] = array_ecc_number;
        state._packet_header["ignore_0"] = array_ignore_0;
        state._packet_header["test_mode"] = array_test_mode;
        state._packet_header["rx_channel_id"] = array_rx_channel_id;
        state._packet_header["instrument_configuration_id"] =
            array_instrument_configuration_id;
        state._packet_header["sub_commutated_index"] =
            array_sub_commutated_index;
        state._packet_header["sub_commutated_data"] = array_sub_commutated_data;
        state._packet_header["space_packet_count"] = array_space_packet_count;
        state._packet_header["pri_count"] = array_pri_count;
        state._packet_header["error_flag"] = array_error_flag;
        state._packet_header["ignore_1"] = array_ignore_1;
        state._packet_header["baq_mode"] = array_baq_mode;
        state._packet_header["baq_block_length"] = array_baq_block_length;
        state._packet_header["ignore_2"] = array_ignore_2;
        state._packet_header["range_decimation"] = array_range_decimation;
        state._packet_header["rx_gain"] = array_rx_gain;
        state._packet_header["tx_ramp_rate_polarity"] =
            array_tx_ramp_rate_polarity;
        state._packet_header["tx_ramp_rate_magnitude"] =
            array_tx_ramp_rate_magnitude;
        state._packet_header["tx_pulse_start_frequency_polarity"] =
            array_tx_pulse_start_frequency_polarity;
        state._packet_header["tx_pulse_start_frequency_magnitude"] =
            array_tx_pulse_start_frequency_magnitude;
        state._packet_header["tx_pulse_length"] = array_tx_pulse_length;
        state._packet_header["ignore_3"] = array_ignore_3;
        state._packet_header["rank"] = array_rank;
        state._packet_header["pulse_repetition_interval"] =
            array_pulse_repetition_interval;
        state._packet_header["sampling_window_start_time"] =
            array_sampling_window_start_time;
        state._packet_header["sampling_window_length"] =
            array_sampling_window_length;
        state._packet_header["sab_ssb_calibration_p"] =
            array_sab_ssb_calibration_p;
        state._packet_header["sab_ssb_polarisation"] =
            array_sab_ssb_polarisation;
        state._packet_header["sab_ssb_temp_comp"] = array_sab_ssb_temp_comp;
        state._packet_header["sab_ssb_ignore_0"] = array_sab_ssb_ignore_0;
        state._packet_header["sab_ssb_elevation_beam_address"] =
            array_sab_ssb_elevation_beam_address;
        state._packet_header["sab_ssb_ignore_1"] = array_sab_ssb_ignore_1;
        state._packet_header["sab_ssb_azimuth_beam_address"] =
            array_sab_ssb_azimuth_beam_address;
        state._packet_header["ses_ssb_cal_mode"] = array_ses_ssb_cal_mode;
        state._packet_header["ses_ssb_ignore_0"] = array_ses_ssb_ignore_0;
        state._packet_header["ses_ssb_tx_pulse_number"] =
            array_ses_ssb_tx_pulse_number;
        state._packet_header["ses_ssb_signal_type"] = array_ses_ssb_signal_type;
        state._packet_header["ses_ssb_ignore_1"] = array_ses_ssb_ignore_1;
        state._packet_header["ses_ssb_swap"] = array_ses_ssb_swap;
        state._packet_header["ses_ssb_swath_number"] =
            array_ses_ssb_swath_number;
        state._packet_header["number_of_quads"] = array_number_of_quads;
        state._packet_header["ignore_4"] = array_ignore_4;
      };
      (packet_idx)++;
    };
    auto fn = ((std::string("/dev/shm/o_range")) + (std::to_string(n0)) +
               (std::string("_echoes")) + (std::to_string(ele_number_echoes)) +
               (std::string(".cf")));
    auto file = std::ofstream(fn, std::ofstream::binary);
    auto nbytes = ((n0) * (ele_number_echoes) * (sizeof(std::complex<float>)));
    std::setprecision(3);
    (std::cout) << (std::setw(10))
                << (((std::chrono::high_resolution_clock::now()
                          .time_since_epoch()
                          .count()) -
                     (state._start_time)))
                << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                << (__func__) << (" ") << ("store echo") << (" ")
                << (std::setw(8)) << (" nbytes=") << (nbytes) << (std::endl);
    file.write(reinterpret_cast<const char *>(sar_image), nbytes);
    std::setprecision(3);
    (std::cout) << (std::setw(10))
                << (((std::chrono::high_resolution_clock::now()
                          .time_since_epoch()
                          .count()) -
                     (state._start_time)))
                << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
                << (__func__) << (" ") << ("store echo finished") << (" ")
                << (std::endl);
  }
  delete[](sar_image);
  auto fn = ((std::string("/dev/shm/o_cal_range")) + (std::to_string(cal_n0)) +
             (std::string("_echoes")) + (std::to_string(cal_count)) +
             (std::string(".cf")));
  auto file = std::ofstream(fn, std::ofstream::binary);
  auto nbytes = ((cal_n0) * (cal_count) * (sizeof(std::complex<float>)));
  std::setprecision(3);
  (std::cout) << (std::setw(10))
              << (((std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count()) -
                   (state._start_time)))
              << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
              << (__func__) << (" ") << ("store cal") << (" ") << (std::setw(8))
              << (" nbytes=") << (nbytes) << (std::endl);
  file.write(reinterpret_cast<const char *>(cal_image), nbytes);
  std::setprecision(3);
  (std::cout) << (std::setw(10))
              << (((std::chrono::high_resolution_clock::now()
                        .time_since_epoch()
                        .count()) -
                   (state._start_time)))
              << (" ") << (__FILE__) << (":") << (__LINE__) << (" ")
              << (__func__) << (" ") << ("store cal finished") << (" ")
              << (std::endl);
  delete[](cal_image);
  destroy_mmap();
}