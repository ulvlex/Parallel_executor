#pragma once

#include <string>

//Interface
class Device {
public:
	virtual std::string getName() = 0;
	virtual std::string getDataAsString() = 0;
	virtual bool read() = 0;
	virtual size_t returnDelay() = 0;
	virtual ~Device() {}
};