#pragma once

#include "DeviceEvent.hpp"

class DataEvent : public DeviceEvent {
public:
	DataEvent(std::shared_ptr<Device> device) : DeviceEvent(device) {}

	std::string toString() const override{
		return "Data from " + device->getName() + ": " + device->getDataAsString();
	}
};