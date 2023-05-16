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

void bme280::setSensorMode(sensorMode_e mode) {
	int8_t rc = bme280_set_sensor_mode((uint8_t)mode, &bmeDev);
	if (rc != 0) {
		throw rc;
	}
}

bme280::sensorMode_e bme280::getSensorMode() {
	sensorMode_e mode;
	int8_t       rc = bme280_get_sensor_mode((uint8_t *)&mode, &bmeDev);
	if (rc != 0) {
		throw rc;
	} else {
		return mode;
	}
}

void bme280::softReset() {
	int8_t rc = bme280_soft_reset(&bmeDev);
	if (rc != 0) {
		throw rc;
	}
}

bme280_data bme280::getSensorData(sensorDataSel_e sensor) {
	bme280_data output;
	int8_t      rc = bme280_get_sensor_data((uint8_t)sensor, &output, &bmeDev);
	if (rc != 0) {
		throw rc;
	} else {
		return output;
	}
}

void bme280::setOverSampling(sensorDataSel_e sensor, oversampling_e oversamp) {
	int8_t rc = 255;
	switch (sensor) {
		case pressure:
			devSettings.osr_p = oversamp;
			rc                = bme280_set_sensor_settings(BME280_SEL_OSR_PRESS, &devSettings, &bmeDev);
			break;

		case temp:
			devSettings.osr_t = oversamp;
			rc                = bme280_set_sensor_settings(BME280_SEL_OSR_TEMP, &devSettings, &bmeDev);
			break;

		case humidity:
			devSettings.osr_h = oversamp;
			rc                = bme280_set_sensor_settings(BME280_SEL_OSR_HUM, &devSettings, &bmeDev);
			break;

		case all:
			devSettings.osr_p = oversamp;
			devSettings.osr_t = oversamp;
			devSettings.osr_h = oversamp;
			rc                = bme280_set_sensor_settings(
        BME280_SEL_OSR_PRESS | BME280_SEL_OSR_TEMP | BME280_SEL_OSR_HUM, &devSettings, &bmeDev);
			break;

		default:
			break;
	}
	if (rc != 0) {
		throw rc;
	}
}

bme280::oversampling_e bme280::getOverampling(sensorDataSel_e sensor) {
	int8_t rc = bme280_get_sensor_settings(&devSettings, &bmeDev);
	if (rc != 0) {
		throw rc;
	} else {
		switch (sensor) {
			case pressure:
				return (oversampling_e)devSettings.osr_p;
			case temp:
				return (oversampling_e)devSettings.osr_t;
			case humidity:
				return (oversampling_e)devSettings.osr_h;
			default:
				// The only way to get is if user app sets sensor to 'ALL'
				throw 1;
		}
	}
}

void bme280::setFilter(filterCoeff_e filter) {
	devSettings.filter = filter;
	int8_t rc          = bme280_set_sensor_settings(BME280_SEL_FILTER, &devSettings, &bmeDev);
	if (rc != 0) {
		throw rc;
	}
}

bme280::filterCoeff_e bme280::getFilter() {
	int8_t rc = bme280_get_sensor_settings(&devSettings, &bmeDev);
	if (rc != 0) {
		throw rc;
	} else {
		return (filterCoeff_e)devSettings.filter;
	}
}

void bme280::setStandbyTime(standbyTime_e time) {
	devSettings.standby_time = time;
	int8_t rc                = bme280_set_sensor_settings(BME280_SEL_STANDBY, &devSettings, &bmeDev);
	if (rc != 0) {
		throw rc;
	}
}

bme280::standbyTime_e bme280::getStandbyTime() {
	int8_t rc = bme280_get_sensor_settings(&devSettings, &bmeDev);
	if (rc != 0) {
		throw rc;
	} else {
		return (standbyTime_e)devSettings.standby_time;
	}
}

uint32_t bme280::calculateMinDelay() {
	uint32_t delay;
	int8_t   rc = bme280_cal_meas_delay(&delay, &devSettings);
	if (rc != 0) {
		throw rc;
	} else {
		return delay;
	}
}