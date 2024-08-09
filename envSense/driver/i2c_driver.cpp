#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include "i2c_driver.h"

// #define DEBUG_PRINT

#ifdef DEBUG_PRINT
#include <iostream>
#include <iomanip>
std::string preamble = "i2c_driver.cpp: ";
#endif

int i2c::openDev(uint8_t devNumber) {
	i2cFilePath += std::to_string(devNumber);
	int fd = open(i2cFilePath.c_str(), O_RDWR);
	if (fd <= 0) {
		int error = errno;
#ifdef DEBUG_PRINT
		std::cout << preamble << "Unable to open I2C device " << strerror(error) << std::endl;
#endif
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
#ifdef DEBUG_PRINT
		std::cout << preamble << "Setting I2C address failed" << std::endl;
#endif
		return errno;
	}
	uint8_t buf[2] = {regaddr, value};
	if (write(i2cDevice, (char *)buf, sizeof(buf)) != sizeof(buf)) {
#ifdef DEBUG_PRINT
		std::cout << preamble << "I2C write failed :(" << std::endl;
#endif
		return errno;
	}
	return 0;
}

int i2c::writeBytes(uint8_t i2caddr, uint8_t regaddr, uint32_t count, const uint8_t *value) {
	std::lock_guard<std::mutex> lock(i2c_mutex);

#ifdef DEBUG_PRINT
	std::cout << preamble << "Writing to   address 0x" << std::setfill('0') << std::setw(2) << std::right << std::hex
						<< (int)regaddr << ": ";
	for (uint32_t i = 0; i < count; i += 1) {
		std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)value[i] << "h ";
	}
	std::cout << std::endl;
#endif

	if (ioctl(i2cDevice, I2C_SLAVE, i2caddr) < 0) {
#ifdef DEBUG_PRINT
		std::cout << preamble << "Setting I2C address failed" << std::endl;
#endif
		return errno;
	}
	int32_t  size = count + 1;
	uint8_t *buf  = (uint8_t *)malloc(size);
	buf[0]        = regaddr;
	std::memmove(&buf[1], value, count);
	if (write(i2cDevice, (char *)buf, size) != size) {
#ifdef DEBUG_PRINT
		std::cout << preamble << "I2C bulk write failed :(" << std::endl;
#endif
		free(buf);
		return errno;
	}
	free(buf);
	return 0;
}

int i2c::readByte(uint8_t i2caddr, uint8_t regaddr, uint8_t *buf) {
	std::lock_guard<std::mutex> lock(i2c_mutex);

	int32_t        res    = 0;
	uint8_t        buffer = 0;
	struct i2c_msg msg[2] = {
		{
			.addr  = i2caddr,
			.flags = 0,
			.len   = 1,
			.buf   = &regaddr,
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
#ifdef DEBUG_PRINT
		std::cout << preamble << "Single read failed: " << strerror(errorCode) << std::endl;
#endif
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
			.addr  = i2caddr,
			.flags = 0,
			.len   = 1,
			.buf   = &regaddr,
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
#ifdef DEBUG_PRINT
		std::cout << preamble << "Bulk read failed: " << strerror(errorCode) << std::endl;
#endif
		return errorCode;
	}

#ifdef DEBUG_PRINT
	std::cout << preamble << "Reading from address 0x" << std::setfill('0') << std::setw(2) << std::right << std::hex
						<< (int)regaddr << ": ";
	for (uint16_t i = 0; i < count; i += 1) {
		std::cout << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)buffer[i] << "h ";
	}
	std::cout << std::endl;
#endif

	return 0;
}

std::string i2c::getI2cFilePath() { return i2cFilePath; }