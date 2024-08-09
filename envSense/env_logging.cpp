#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "common/common.h"

std::string getCurrentTime() {
	auto               now = std::time(nullptr);
	std::tm           *pTm = std::localtime(&now);
	std::ostringstream oss;
	oss << std::put_time(pTm, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}

void appendDataToJson(const envPacked_s &data, const std::string &filename) {
	std::string jsonString = "{";
	jsonString += "\"timestamp\":\"" + getCurrentTime() + "\",";
	jsonString += "\"temp\":" + std::to_string(data.temperature) + ",";
	jsonString += "\"hum\":" + std::to_string(data.humidity) + ",";
	jsonString += "\"pres\":" + std::to_string(data.pressure) + "";
	jsonString += "}\n";

	// std::cout << jsonString;

	std::ofstream file;
	file.open(filename, std::ios::app);

	if (file.is_open()) {
		file << jsonString;
		file.close();
	} else {
		std::cerr << "Failed to open file: " << filename << std::endl;
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cerr << "Missing args!" << std::endl;
		return -1;
	}

	int         c;
	int         daemon = 0;
	std::string filename;
	while ((c = getopt(argc, argv, ":p:f:d")) != -1) {
		switch (c) {
			case 'p':
				httpPort = atoi(optarg);
				break;
			case 'f':
				filename = optarg;
				// std::cout << "Using " << filename << std::endl;
				break;
			case 'd':
				daemon = 1;
			default:
				break;
		}
	}
	if (filename.empty()) {
		std::cerr << "Please enter path to write logs to\n";
		return -1;
	}

	httplib::Client client(url, httpPort);
	while (1) {
		if (auto res = client.Get(envAllResource)) {
			if (res->status == 200) {
				std::vector<uint8_t> rx(res->body.begin(), res->body.end());
				auto                 envData = msgpack::unpack<envPacked_s>(rx);
				// Append data to CSV file
				appendDataToJson(envData, filename);
			} else {
				std::cerr << "Error! Status: " << res->status << std::endl;
				return -1;
			}
		} else {
			auto err = res.error();
			std::cerr << "HTTP error: " << httplib::to_string(err) << std::endl;
		}
		if (!daemon) {
			return 0;
		}
		sleep(60);
	}

	return 0;
}