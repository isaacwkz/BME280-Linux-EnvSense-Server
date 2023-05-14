#include <stdint.h>
#include <unistd.h>
#include "bme280_wrapper.h"

BME280_INTF_RET_TYPE bme280::readBytes(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
	struct devComms_s *dev = static_cast<struct devComms_s *>(intf_ptr);
	return dev->i2cDev->readBytes(dev->devAddr, reg_addr, (uint16_t)len, reg_data);
}

BME280_INTF_RET_TYPE bme280::writeBytes(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
	struct devComms_s *dev = static_cast<struct devComms_s *>(intf_ptr);
	return dev->i2cDev->writeBytes(dev->devAddr, reg_addr, len, reg_data);
}

void bme280::delayUs(uint32_t period, void *intf_ptr) { usleep(period); }

uint8_t bme280::getDevAddr() {
	struct devComms_s *dev = static_cast<struct devComms_s *>(bmeDev.intf_ptr);
	return dev->devAddr;
}

std::shared_ptr<i2c> bme280::getI2cDev() {
	struct devComms_s *dev = static_cast<struct devComms_s *>(bmeDev.intf_ptr);
	return dev->i2cDev;
}