#pragma once

#include "DeviceEvent.hpp"

#include <string>
#include <memory>

class StartedEvent : public DeviceEvent {
public:
	StartedEvent(std::shared_ptr<Device> device) : DeviceEvent(device) {}

	std::string toString() const override {
		return device->getName() + " has started";
	}
};