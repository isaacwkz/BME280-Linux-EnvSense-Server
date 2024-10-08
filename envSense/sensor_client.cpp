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
		if (auto res = client.Get(envAllResource)) {
			if (res->status == 200) {
				std::cout << "Status: " << res->status << std::endl;
				std::vector<uint8_t> rx(res->body.begin(), res->body.end());
				auto                 envData = msgpack::unpack<envPacked_s>(rx); // Unpack it

				// auto timeSent = envData.timeTransmited;
				// std::cout << "Time: " << std::ctime(&timeSent) << std::endl;
				// std::cout << "Seconds since epoch: " << (long int)timeSent << std::endl;
				std::cout << "Temperature: " << envData.temperature << std::endl;
				std::cout << "Humidity: " << envData.humidity << std::endl;
				std::cout << "Pressure: " << envData.pressure << std::endl;

			} else {
				std::cout << "Error! Status: " << res->status << std::endl;
				return -1;
			}
		} else {
			auto err = res.error();
			std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
		}
	}

	return 0;
}