#pragma once

#include "Events/Event.hpp"

#include <mutex>
#include <queue>
#include <condition_variable>
#include <memory>

class EventQueue
{
public:
	// Положить сообщение в очередь
	void push(const std::shared_ptr<const Event>& event) {
		std::unique_lock<std::mutex> lock(mtx); //блокируем мьютекс, чтобы другой поток в этот момент не мог изменить очередь
		queue.push(event);
		cv.notify_one(); //будим потоки
	}

	// Удалить сообщение из очереди и вернуть его. По истечении duration, если очередь пуста, вернуть пустой указатель
	std::shared_ptr<const Event> pop(const std::chrono::seconds& duration) {
		std::unique_lock<std::mutex> lock(mtx); //блокируем мьютекс, чтобы другой поток в этот момент не мог изменить очередь
		// Ждем, пока не станет доступно сообщение или истечет время ожидания
		if (queue.empty()) {
			cv.wait_for(lock, duration, [this] { return !queue.empty(); });
		}
	
		if (!queue.empty()) {
			auto event = queue.front();
			queue.pop();
			cv.notify_one(); //будим потоки
			return event;
		}
		else {
			//в случае, если очередь всё же пуста по истечении времени
			cv.notify_one(); //будим потоки
			return nullptr;
		}
	}

private:
	std::queue<std::shared_ptr<const Event>> queue;
	std::mutex mtx;
	std::condition_variable cv;
};