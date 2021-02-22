cmake_minimum_required( VERSION 3.4 )
project( mytest LANGUAGES CXX )
set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_CXX_FLAGS "-Og -ggdb3 -pipe -march=native -mtune=native")
set( CMAKE_CXX_STANDARD 14 )
find_package( Python COMPONENTS Interpreter Development REQUIRED )
find_package( pybind11 REQUIRED )
set( SRCS /home/martin/stage/copernicus-radar/source/copernicus_00_main.cpp
/home/martin/stage/copernicus-radar/source/copernicus_01_mmap.cpp
/home/martin/stage/copernicus-radar/source/copernicus_02_collect_packet_headers.cpp
/home/martin/stage/copernicus-radar/source/copernicus_03_process_packet_headers.cpp
/home/martin/stage/copernicus-radar/source/copernicus_04_decode_packet.cpp
/home/martin/stage/copernicus-radar/source/copernicus_05_decode_type_ab_packet.cpp
/home/martin/stage/copernicus-radar/source/copernicus_06_decode_sub_commutated_data.cpp
/home/martin/stage/copernicus-radar/source/copernicus_07_decode_type_c_packet.cpp
/home/martin/stage/copernicus-radar/source/copernicus_08_demangle.cpp
/home/martin/stage/copernicus-radar/source/copernicus_09_python.cpp )
add_executable( mytest ${SRCS} )
target_link_libraries( mytest PRIVATE pybind11::embed gmp )
