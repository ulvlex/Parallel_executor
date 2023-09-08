#pragma once

#include<mutex>
#include <queue>
#include "Events/Event.hpp"

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
	std::shared_ptr<const Event> pop(const std::chrono::seconds& duration, bool& checkWaiting) {
		std::unique_lock<std::mutex> lock(mtx); //блокируем мьютекс, чтобы другой поток в этот момент не мог изменить очередь
		// Ждем, пока не станет доступно сообщение или истечет время ожидания
		if (queue.empty()) {
			if (cv.wait_for(lock, duration) == std::cv_status::timeout) {
				// Если время ожидания истекло, вернуть пустой указатель
				checkWaiting = false;
				cv.notify_one(); //будим потоки
				return nullptr;
			}
			else {
				cv.notify_one(); //будим потоки
				return nullptr;
			}
		}
		else {
			auto event = queue.front();
			queue.pop();
			cv.notify_one(); //будим потоки
			return event;
		}
	}

private:
	std::queue<std::shared_ptr<const Event>> queue;
	std::mutex mtx;
	std::condition_variable cv;
};