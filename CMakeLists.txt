cmake_minimum_required(VERSION 3.1...3.26)

project(
  envServer
  VERSION 1.0
  LANGUAGES CXX)

set_source_files_properties(device/bme280.c PROPERTIES LANGUAGE CXX )

set(SOURCE_FILES 
    sensor_serv.cpp
    driver/i2c_driver.cpp
    device/bme280.c
    device/bme280_wrapper.cpp)

#add_library(envServ simple_lib.cpp simple_lib.hpp)

add_executable(envServ ${SOURCE_FILES})

target_compile_features(envServ PRIVATE cxx_std_17)

#target_link_libraries(envServ PRIVATE envServ)