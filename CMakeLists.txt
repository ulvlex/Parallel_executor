cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
cmake_policy(SET CMP0016 NEW)

project(Parallel_executor)

set (BOOST_ROOT /boost_1_82_0) # Путь к библиотеке Boost
set (Boost_LIBRARY_DIR /boost_1_82_0/stage/lib)
set (Boost_USE_STATIC_LIBS ON)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)

# Подключаем необходимые модули
find_package(Boost REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/src)
  
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR}) # подключаем заголовочные файлы
    link_directories(${Boost_LIBRARY_DIRS})
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
    target_link_libraries(Parallel_executor LINK_PUBLIC ${Boost_LIBRARIES})
endif()