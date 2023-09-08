#pragma once

#include "DeviceEvent.hpp"

class StartedEvent : public DeviceEvent {
public:
	StartedEvent(std::shared_ptr<Device> device) : DeviceEvent(device) {}

	std::string toString() const override {
		return device->getName() + " has started";
	}
};