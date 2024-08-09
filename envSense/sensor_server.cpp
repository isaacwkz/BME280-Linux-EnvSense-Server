#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "common/common.h"
#include "driver/i2c_driver.h"
#include "device/bme280_wrapper.h"
#include "device/bme280_mock.h"

// #define MOCK_SENSOR

static uint8_t i2cDev     = 5;
static int     serverMode = 0;
static int     getEnv     = 0;

/* clang-format off */
void printHelpMessage(void) {
	std::cout   << "s       | Start HTTP server" << std::endl \
				<< "p [int] | Use HTTP port number (default: 1337)" << std::endl \
				<< "d [int] | Use I2C device number (default: 5)" << std::endl \
				<< "g       | Get sensor data without starting server" << std::endl \
				<< "t       | Check whether sensor is present on I2C bus" << std::endl \
				<< "p       | Print help message" << std::endl;
}
/* clang-format on */

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
	if (argc < 2) {
		std::cout << "Missing args!" << std::endl;
		printHelpMessage();
		return -1;
	}
	int c;
	while ((c = getopt(argc, argv, ":ghsp:d:t")) != -1) {
		switch (c) {
			case 's':
				serverMode = 1;
				break;
			case 'p':
				httpPort = atoi(optarg);
				std::cout << "Using port " << httpPort << std::endl;
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
				printHelpMessage();
				return 0;
			default:
				std::cout << "Invalid args!" << std::endl;
				printHelpMessage();
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
	envSensor0->setFilter(bme280::filter_2);
	envSensor0->setOverSampling(bme280::all, bme280::oversampling_1x);
	envSensor0->setStandbyTime(bme280::standbyTime_0_5ms);
	envSensor0->setSensorMode(bme280::normal);
	uint32_t mindelay = envSensor0->calculateMinDelay();
	std::cout << "Min delay time for current settings(ms): " << (int)mindelay << std::endl;
	// sleep(mindelay / 1000);
	sleep(1);

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
			(void)req;
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
			// const auto now     = std::chrono::system_clock::now();
			//  txData.timeTransmited = std::chrono::system_clock::to_time_t(now);

			// std::cout << "Seconds since epoch: " << (long int)txData.timeTransmited << std::endl;

			auto send = msgpack::pack(txData);

			res.set_content(reinterpret_cast<char *>(send.data()), sizeof(send), "application/msgpack");
		});

		std::cout << "Starting HTTP server at port " << httpPort << std::endl;
		// Listen on all interfaces
		svr.listen(url, httpPort);
	}

	return 0;
}