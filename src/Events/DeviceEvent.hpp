#pragma once

#include "Event.hpp"
#include "Devices/Device.hpp"

#include <memory>

class DeviceEvent : public Event {
public:
	DeviceEvent(std::shared_ptr<Device> device) : device(device) {}

protected:
	std::shared_ptr<Device> device; 
};