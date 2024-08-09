#ifndef COMMON_H
#define COMMON_H

#include "cpp-httplib/httplib.h"
#include "cppack/msgpack/include/msgpack/msgpack.hpp"

// This will be common for both server and client
// But we keep this as a variable so that we can change it during runtime
static int         httpPort       = 1337;
static std::string url            = "127.0.0.1";
static std::string envAllResource = "/envAll";

struct envPacked_s {
	// Data is transmited a double, but backend reading it
	// May be in 32 bits integer
	double pressure;
	double temperature;
	double humidity;
	// std::time_t timeTransmited;

	template<class T>
	void pack(T &pack) {
		// pack(pressure, temperature, humidity, timeTransmited);
		pack(pressure, temperature, humidity);
	}
};

#endif