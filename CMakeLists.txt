cmake_minimum_required(VERSION 3.1...3.26)

project(
  envServer
  VERSION 1.0
  LANGUAGES CXX)

set_source_files_properties(device/bme280.c PROPERTIES LANGUAGE CXX )

set(SERVER_SOURCE_FILES 
    sensor_server.cpp
    driver/i2c_driver.cpp
    device/bme280.c
    device/bme280_wrapper.cpp
    device/bme280_mock.cpp)

set(CLIENT_SOURCE_FILES 
    sensor_client.cpp)

#add_library(envServ simple_lib.cpp simple_lib.hpp)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(envServer ${SERVER_SOURCE_FILES})
add_executable(envClient ${CLIENT_SOURCE_FILES})


target_compile_features(envServer PRIVATE cxx_std_17)
target_compile_features(envClient PRIVATE cxx_std_17)

#target_link_libraries(envServ PRIVATE envServ)