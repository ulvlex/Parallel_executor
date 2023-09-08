#pragma once

#include "Device.hpp"
#include <random>

class DeviceA : public Device {
public:

	std::string getName() override{
		std::string name = "DeviceA";
		return name;
	}

    std::string getDataAsString() override {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 500);

        int size = dis(gen);
        std::string data;
        for (int i = 0; i < size; ++i) {
            int asciiValue = 97 + dis(gen) % 26; // Случайные маленькие буквы
            data += static_cast<char>(asciiValue);
        }
        return data;
    }

	bool read() override{
		std::this_thread::sleep_for(std::chrono::seconds(delay));
		return true;
	}

    size_t returnDelay() override{ //возвращаем задержку
        return delay;
    }

private:
    const size_t delay = 1;
};