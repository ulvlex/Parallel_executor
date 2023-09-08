#pragma once

#include "DeviceEvent.hpp"

class WorkDoneEvent : public DeviceEvent {
public:
	WorkDoneEvent(std::shared_ptr<Device> device) : DeviceEvent(device) {}

	std::string toString() const override {
		return "Work done for " + device->getName();
	}
};