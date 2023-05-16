#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <string>
#include <mutex>
#include <stdint.h>

class i2c {
 public:
	i2c(uint8_t devNumber) { openDev(devNumber); }
	~i2c() { closeDev(); }
	int         writeByte(uint8_t i2caddr, uint8_t regaddr, uint8_t value);
	int         writeBytes(uint8_t i2caddr, uint8_t regaddr, uint32_t count, const uint8_t *value);
	int         readByte(uint8_t i2caddr, uint8_t regaddr, uint8_t *buf);
	int         readBytes(uint8_t i2caddr, uint8_t regaddr, uint16_t count, uint8_t *buffer);
	std::string getI2cFilePath();

 private:
	int         openDev(uint8_t devNumber);
	void        closeDev();
	std::string i2cFilePath = "/dev/i2c-";
	int         i2cDevice;

	std::mutex i2c_mutex;
};

#endif