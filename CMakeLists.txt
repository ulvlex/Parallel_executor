cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

project(Parallel_executor)

include_directories(${CMAKE_SOURCE_DIR}/src)
  
add_executable(Parallel_executor
	src/main.cpp
	src/Queue/EventQueue.hpp
	src/Events/Event.hpp
	src/Events/DeviceEvent.hpp
	src/Events/DataEvent.hpp
	src/Events/WorkDoneEvent.hpp
	src/Events/StartedEvent.hpp
	src/Devices/Device.hpp
	src/Devices/DeviceA.hpp
	src/Devices/DeviceB.hpp)