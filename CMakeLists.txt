cmake_minimum_required(VERSION 3.1...3.26)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

project(envSense LANGUAGES C CXX)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-psabi")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_STANDARD 17)

set(MAIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/envSense)

set(COMMON_INC ${MAIN_DIR}/common)

add_library(i2c-driver "${MAIN_DIR}/driver/i2c_driver.cpp")
set(I2C_DRIVER_INC "${MAIN_DIR}/driver")

add_library(bme280-driver
    "${MAIN_DIR}/device/bme280.c"
    "${MAIN_DIR}/device/bme280_wrapper.cpp"
    "${MAIN_DIR}/device/bme280_mock.cpp"
)
set(BME280_DRIVER_INC "${MAIN_DIR}/device")

add_executable(envServer "${MAIN_DIR}/sensor_server.cpp")
target_include_directories(envServer PUBLIC ${COMMON_INC} ${I2C_DRIVER_INC} ${BME280_DRIVER_INC})
target_link_libraries(envServer PRIVATE i2c-driver bme280-driver)

add_executable(envClient "${MAIN_DIR}/sensor_client.cpp")
target_include_directories(envClient PUBLIC ${COMMON_INC})
target_link_libraries(envClient PRIVATE pthread)
