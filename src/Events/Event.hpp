#pragma once

#include <string>

//Interface
class Event {
public:
	virtual std::string toString() const = 0;
	virtual ~Event() {};
};