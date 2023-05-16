#include <stdint.h>
#include <unistd.h>
#include "bme280_mock.h"

BME280_INTF_RET_TYPE bme280Mock::readBytes(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
	// TODO hmmm... We might need to mock the dev registers for reading?
	(void)reg_addr;
	(void)reg_data;
	(void)len;
	(void)intf_ptr;
	return 0;
}

BME280_INTF_RET_TYPE bme280Mock::writeBytes(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
	(void)reg_addr;
	(void)reg_data;
	(void)len;
	(void)intf_ptr;
	return 0;
}

void bme280Mock::delayUs(uint32_t period, void *intf_ptr) {
	(void)period;
	(void)intf_ptr;
}

uint8_t bme280Mock::getDevAddr() {
	struct devComms_s *dev = static_cast<struct devComms_s *>(bmeDev.intf_ptr);
	return dev->devAddr;
}

std::shared_ptr<i2c> bme280Mock::getI2cDev() {
	struct devComms_s *dev = static_cast<struct devComms_s *>(bmeDev.intf_ptr);
	return dev->i2cDev;
}

void bme280Mock::setSensorMode(sensorMode_e mode) { mockReg.mode = mode; }

bme280Mock::sensorMode_e bme280Mock::getSensorMode() { return mockReg.mode; }

void bme280Mock::softReset() { return; }

bme280_data bme280Mock::getSensorData(sensorDataSel_e sensor) {
	switch (sensor) {
		case pressure:
			mockOutput.pressure = (double)rand() / (RAND_MAX) * (30 - 10) + 20;
			break;
		case temp:
			mockOutput.temperature = (double)rand() / (RAND_MAX) * (40 - 20) + 30;
			break;
		case humidity:
			mockOutput.humidity = (double)rand() / (RAND_MAX) * (100 - 50) + 75;
			break;
		case all:
			mockOutput.pressure    = (double)rand() / (RAND_MAX) * (30 - 10) + 20;
			mockOutput.temperature = (double)rand() / (RAND_MAX) * (40 - 20) + 30;
			mockOutput.humidity    = (double)rand() / (RAND_MAX) * (100 - 50) + 75;
			break;
	}
	return mockOutput;
}

void bme280Mock::setOverSampling(sensorDataSel_e sensor, oversampling_e oversamp) {
	switch (sensor) {
		case pressure:
			mockReg.pressureOversampling = oversamp;
			break;

		case temp:
			mockReg.tempOversampling = oversamp;
			break;

		case humidity:
			mockReg.humidityOversampling = oversamp;
			break;

		case all:
			mockReg.pressureOversampling = oversamp;
			mockReg.tempOversampling     = oversamp;
			mockReg.humidityOversampling = oversamp;
			break;
	}
}

bme280Mock::oversampling_e bme280Mock::getOverampling(sensorDataSel_e sensor) {
	switch (sensor) {
		case pressure:
			return (oversampling_e)mockReg.pressureOversampling;
		case temp:
			return (oversampling_e)mockReg.tempOversampling;
		case humidity:
			return (oversampling_e)mockReg.humidityOversampling;
		default:
			// The only way to get is if user app sets sensor to 'ALL'
			throw 1;
	}
}

void bme280Mock::setFilter(filterCoeff_e filter) { mockReg.filter = filter; }

bme280Mock::filterCoeff_e bme280Mock::getFilter() { return (filterCoeff_e)mockReg.filter; }

void bme280Mock::setStandbyTime(standbyTime_e time) { mockReg.standbyTime = time; }

bme280Mock::standbyTime_e bme280Mock::getStandbyTime() { return (standbyTime_e)mockReg.standbyTime; }

uint32_t bme280Mock::calculateMinDelay() {
	uint32_t delay;
	int8_t   rc = bme280_cal_meas_delay(&delay, &devSettings);
	if (rc != 0) {
		throw rc;
	} else {
		return delay;
	}
}