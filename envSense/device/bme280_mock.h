#ifndef BME280_MOCK_WRAPPER_H
#define BME280_MOCK_WRAPPER_H

#include <functional>
#include <memory>
#include <stdint.h>
#include "bme280.h"
#include "../driver/i2c_driver.h"

class bme280Mock {
 public:
	bme280Mock(std::shared_ptr<i2c> i2cDev, uint8_t addr) {
		devComms.devAddr = addr;
		devComms.i2cDev  = i2cDev;
		// Use BME280 dev's struct to store comms bus info
		bmeDev.intf_ptr = &devComms;
		bmeDev.read     = readBytes;
		bmeDev.write    = writeBytes;
		bmeDev.delay_us = delayUs;
		bmeDev.intf     = BME280_I2C_INTF;
		srand((unsigned int)time(NULL));
	}
	~bme280Mock() { ; }

	enum sensorMode_e {
		sleep  = 0,
		forced = 1,
		normal = 3,
	};

	enum sensorDataSel_e {
		pressure = 1,
		temp     = 2,
		humidity = 4,
		all      = 7,
	};

	enum standbyTime_e {
		standbyTime_0_5ms  = 0,
		standbyTime_62_5ms = 1,
		standbyTime_125ms  = 2,
		standbyTime_250ms  = 3,
		standbyTime_500ms  = 4,
		standbyTime_1000ms = 5,
		standbyTime_10ms   = 6,
		standbyTime_20ms   = 7,
	};

	enum oversampling_e {
		oversampling_0x  = 0,
		oversampling_1x  = 1,
		oversampling_2x  = 2,
		oversampling_4x  = 3,
		oversampling_8x  = 4,
		oversampling_16x = 5,
		oversampling_max = 16,
	};

	enum filterCoeff_e {
		filter_off = 0,
		filter_2   = 1,
		filter_4   = 2,
		filter_8   = 3,
		filter_16  = 4,
	};

	// All public interfaces will throw an exception if something fails
	void              setSensorMode(enum sensorMode_e mode);
	enum sensorMode_e getSensorMode();
	void              softReset();
	// We have to return a struct instead, because the data width of the Bosch driver
	// can change depending on the target platform
	struct bme280_data  getSensorData(enum sensorDataSel_e sensor);
	void                setOverSampling(enum sensorDataSel_e sensor, enum oversampling_e oversamp);
	enum oversampling_e getOverampling(enum sensorDataSel_e sensor);
	void                setFilter(enum filterCoeff_e filter);
	enum filterCoeff_e  getFilter();
	void                setStandbyTime(enum standbyTime_e time);
	enum standbyTime_e  getStandbyTime();
	uint32_t            calculateMinDelay();

	// For debug use
	uint8_t              getDevAddr();
	std::shared_ptr<i2c> getI2cDev();

 private:
	// Static member functions for serial comms
	static BME280_INTF_RET_TYPE readBytes(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
	static BME280_INTF_RET_TYPE writeBytes(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
	static void                 delayUs(uint32_t period, void *intf_ptr);

	struct devComms_s {
		uint8_t              devAddr;
		std::shared_ptr<i2c> i2cDev;
	};
	bme280_dev             bmeDev;
	struct devComms_s      devComms;
	struct bme280_settings devSettings;

	struct mockReg_s {
		enum sensorMode_e   mode;
		enum standbyTime_e  standbyTime;
		enum filterCoeff_e  filter;
		enum oversampling_e tempOversampling;
		enum oversampling_e humidityOversampling;
		enum oversampling_e pressureOversampling;
	} mockReg;
	bme280_data mockOutput;
};

#endif