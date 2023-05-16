#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "common/common.h"

static int getEnv = 0;

int main(int argc, char **argv) {
	int c;
	while ((c = getopt(argc, argv, ":ghp:")) != -1) {
		switch (c) {
			case 'p':
				httpPort = atoi(optarg);
				break;
			case 'g':
				getEnv = 1;
				break;
			case 'h':
				std::cout << "Print help message here" << std::endl;
				break;
			default:
				std::cout << "Print help message here" << std::endl;
				return -1;
		}
	}

	if (getEnv) {
		std::cout << "Trying to get data from server!" << std::endl;

		httplib::Client client(url, httpPort);
		auto            res = client.Get(envAllResource);

		if (res->status < 300 && res->status >= 200) {
			std::vector<uint8_t> rx(res->body.begin(), res->body.end());
			auto                 envData = msgpack::unpack<envPacked_s>(rx); // Unpack it

			std::cout << "Status: " << res->status << std::endl;
			std::cout << "Temperature: " << envData.temperature << std::endl;
			std::cout << "Humidity: " << envData.humidity << std::endl;
			std::cout << "Pressure: " << envData.pressure << std::endl;
		} else {
			std::cout << "Error! Status: " << res->status << std::endl;
			return -1;
		}
	}

	return 0;
}