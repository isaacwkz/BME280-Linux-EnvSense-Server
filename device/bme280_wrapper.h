#ifndef BME280_WRAPPER_H
#define BME280_WRAPPER_H

#include <functional>
#include <memory>
#include <stdint.h>
#include "bme280.h"
#include "../driver/i2c_driver.h"

class bme280 {
 public:
	bme280(std::shared_ptr<i2c> i2cDev, uint8_t addr) {
		devComms.devAddr = addr;
		devComms.i2cDev  = i2cDev;
		// Use BME280 dev's struct to store comms bus info
		bmeDev.intf_ptr = &devComms;
		bmeDev.read     = readBytes;
		bmeDev.write    = writeBytes;
		bmeDev.delay_us = delayUs;
		bmeDev.intf     = BME280_I2C_INTF;
		int8_t rc       = bme280_init(&bmeDev);
		if (rc != 0) {
			throw rc;
		}
	}
	~bme280() { ; }

	uint8_t              getDevAddr();
	std::shared_ptr<i2c> getI2cDev();

 private:
	// Static member functions for serial comms
	static BME280_INTF_RET_TYPE readBytes(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
	static BME280_INTF_RET_TYPE writeBytes(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
	static void                 delayUs(uint32_t period, void *intf_ptr);

	bme280_dev bmeDev;
	struct devComms_s {
		uint8_t              devAddr;
		std::shared_ptr<i2c> i2cDev;
	};
	struct devComms_s devComms;
};

#endif