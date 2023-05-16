#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "i2c_driver.h"

int i2c::openDev(uint8_t devNumber) {
	i2cFilePath += std::to_string(devNumber);
	int fd = open(i2cFilePath.c_str(), O_RDWR);
	if (fd <= 0) {
		int error = errno;
		std::cout << "Unable to open I2C device " << strerror(error) << std::endl;
		return error;
	} else {
		i2cDevice = fd;
		return 0;
	}
}

void i2c::closeDev() { close(i2cDevice); }

int i2c::writeByte(uint8_t i2caddr, uint8_t regaddr, uint8_t value) {
	std::lock_guard<std::mutex> lock(i2c_mutex);

	if (ioctl(i2cDevice, I2C_SLAVE, i2caddr) < 0) {
		// std::cout << "Setting I2C address failed" << std::endl;
		return errno;
	}
	uint8_t buf[2] = {regaddr, value};
	if (write(i2cDevice, (char *)buf, sizeof(buf)) != sizeof(buf)) {
		// std::cout << "I2C write failed :(" << std::endl;
		return errno;
	}
	return 0;
}

int i2c::writeBytes(uint8_t i2caddr, uint8_t regaddr, uint32_t count, const uint8_t *value) {
	std::lock_guard<std::mutex> lock(i2c_mutex);

	if (ioctl(i2cDevice, I2C_SLAVE, i2caddr) < 0) {
		std::cout << "Setting I2C address failed" << std::endl;
		return errno;
	}
	int32_t  size = count + 1;
	uint8_t *buf  = (uint8_t *)malloc(size);
	buf[0]        = regaddr;
	std::memmove(&buf[1], value, count);
	if (write(i2cDevice, (char *)buf, size) != size) {
		std::cout << "I2C bulk write failed :(" << std::endl;
		return errno;
	}
	return 0;
}

int i2c::readByte(uint8_t i2caddr, uint8_t regaddr, uint8_t *buf) {
	std::lock_guard<std::mutex> lock(i2c_mutex);

	int32_t        res    = 0;
	uint8_t        buffer = 0;
	struct i2c_msg msg[2] = {
		{
			.addr = i2caddr,
			.len  = 1,
			.buf  = &regaddr,
		},
		{
			.addr  = i2caddr,
			.flags = I2C_M_RD,
			.len   = 1,
			.buf   = &buffer,
		}};
	struct i2c_rdwr_ioctl_data msgset = {
		.msgs  = msg,
		.nmsgs = 2,
	};
	res = ioctl(i2cDevice, I2C_RDWR, &msgset);
	if (res < 0) {
		int errorCode = errno;
		std::cout << "Single read failed: " << strerror(errorCode) << std::endl;
		return errorCode;
	}
	*buf = buffer;
	return 0;
}

int i2c::readBytes(uint8_t i2caddr, uint8_t regaddr, uint16_t count, uint8_t *buffer) {
	std::lock_guard<std::mutex> lock(i2c_mutex);

	int32_t        res    = 0;
	struct i2c_msg msg[2] = {
		{
			.addr = i2caddr,
			.len  = 1,
			.buf  = &regaddr,
		},
		{
			.addr  = i2caddr,
			.flags = I2C_M_RD,
			.len   = count,
			.buf   = buffer,
		}};
	struct i2c_rdwr_ioctl_data msgset = {
		.msgs  = msg,
		.nmsgs = 2,
	};
	res = ioctl(i2cDevice, I2C_RDWR, &msgset);
	if (res < 0) {
		int errorCode = errno;
		std::cout << "Bulk read failed: " << strerror(errorCode) << std::endl;
		return errorCode;
	}
	return 0;
}

std::string i2c::getI2cFilePath() { return i2cFilePath; }