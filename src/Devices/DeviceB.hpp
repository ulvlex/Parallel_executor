#pragma once

#include <random>

class DeviceB : public Device {
public:

	std::string getName() override{
		std::string name = "DeviceB";
		return name;
	}

	std::string getDataAsString() override{
		//заполняем строку тремя рандомными целыми числаими от 0 до 198 
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 198);

		std::string data = std::to_string(dis(gen)) + " " + std::to_string(dis(gen)) + " " + std::to_string(dis(gen));
		return data;
	}

	bool read() override{
		std::this_thread::sleep_for(std::chrono::seconds(delay));
		return true;
	}

	size_t returnDelay() override{ ////возвращаем задержку
		return delay;
	}

private:
	const size_t delay = 5;
};