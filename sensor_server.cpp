#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "common/common.h"
#include "driver/i2c_driver.h"
#include "device/bme280_wrapper.h"
#include "device/bme280_mock.h"

#define MOCK_SENSOR

static uint8_t i2cDev     = 5;
static int     serverMode = 0;
static int     getEnv     = 0;

int checkEnvSensePresent(std::shared_ptr<i2c> bus) {
	uint8_t data = 0;
	bus->readByte(0x76, 0xD0, &data);
	if (data == 0x60) {
		std::cout << "BME-280 is detected!" << std::endl;
		return 0;
	} else if (data == 0x58) {
		std::cout << "BMP-280 is detected!" << std::endl;
		return 1;
	} else {
		std::cout << "Unknown device, or no devices found" << std::endl;
		return -1;
	}
}

int main(int argc, char **argv) {
	int c;
	while ((c = getopt(argc, argv, ":ghsp:d:t")) != -1) {
		switch (c) {
			case 's':
				serverMode = 1;
				break;
			case 'p':
				httpPort = atoi(optarg);
				break;
			case 'd':
				i2cDev = atoi(optarg);
				std::cout << "Using i2c-" << (int)i2cDev << std::endl;
				break;
			case 'g':
				getEnv = 1;
				break;
			case 't': {
				// -t needs to be the last to be processed since this requires i2c bus to be spun up
				// which requires the i2c port to be selected first
				std::shared_ptr<i2c> sensorI2c(new i2c{(uint8_t)5});
				return checkEnvSensePresent(sensorI2c);
			}
			case 'h':
				std::cout << "Print help message here" << std::endl;
				break;
			default:
				std::cout << "Print help message here" << std::endl;
				return -1;
		}
	}

	// Initialise sensor
	std::shared_ptr<i2c> sensorI2c(new i2c{(uint8_t)i2cDev});

#ifndef MOCK_SENSOR
	std::shared_ptr<bme280> envSensor0(new bme280{sensorI2c, 0x76});
#else
	std::shared_ptr<bme280Mock> envSensor0(new bme280Mock{sensorI2c, 0x11});
#endif

	if (getEnv) {
		struct bme280_data envData;
#ifndef MOCK_SENSOR
		envData = envSensor0->getSensorData(bme280::all);
#else
		envData = envSensor0->getSensorData(bme280Mock::all);
#endif
		std::cout << "Temperature: " << envData.temperature << std::endl;
		std::cout << "Humidity: " << envData.humidity << std::endl;
		std::cout << "Pressure: " << envData.pressure << std::endl;
	}

	// Spin up HTTP server
	if (serverMode) {
		httplib::Server svr;
		svr.Get(envAllResource, [&](const httplib::Request &req, httplib::Response &res) {
			struct bme280_data envData;
			struct envPacked_s txData;
#ifndef MOCK_SENSOR
			envData = envSensor0->getSensorData(bme280::all);
#else
            envData     = envSensor0->getSensorData(bme280Mock::all);
#endif
			txData.pressure    = envData.pressure;
			txData.temperature = envData.temperature;
			txData.humidity    = envData.humidity;

			auto send = msgpack::pack(txData);

			res.set_content(reinterpret_cast<char *>(send.data()), sizeof(send), "application/msgpack");
		});

		std::cout << "Starting HTTP server at port " << httpPort << std::endl;
		// Listen on all interfaces
		svr.listen(url, httpPort);
	}

	return 0;
}