#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "httplib.h"
#include "sensor_serv.h"
#include "driver/i2c_driver.h"
#include "device/bme280_wrapper.h"

static int     httpPort   = 1337;
static uint8_t i2cDev     = 5;
static int     serverMode = 0;

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
	while ((c = getopt(argc, argv, ":hsp:d:t")) != -1) {
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
	try {
		std::shared_ptr<bme280> envSensor0(new bme280{sensorI2c, 0x76});
	} catch (int8_t err) {
		std::cout << "Unable to initialise BME280: " << (int)err << std::endl;
		return err;
	}

	// Spin up HTTP server
	if (serverMode) {
		std::cout << "Start HTTP server at port " << httpPort << std::endl;
	}

	return 0;
}