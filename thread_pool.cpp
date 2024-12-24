#include "thread_pool.h"
#include <iostream>

void thread_pool::worker_thread() {
	while (true) {
		if (stop_threads_count.load() > 0) {
			stop_threads_count.fetch_sub(1);
			return;
		}
		else if (std::unique_lock<std::mutex> manager_lock(manager_mtx, std::try_to_lock);
			manager_lock.owns_lock() && ((tasks.size() < task_min.load() && workers.size() == num_threads_max.load())
			|| (tasks.size() > task_min.load() && workers.size() == num_threads_min.load()))) {
			thread_manager();
		}
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> task_lock(queue_mtx);
			cv.wait(task_lock, [this]() { 
				return stop.load() || !tasks.empty();
				});
			if (stop.load() && tasks.empty()) {
				return;
			}
			task = std::move(tasks.front());
			tasks.pop();
		}
		task();
	}
}

void thread_pool::thread_manager() {
	if (tasks.size() < task_min.load() && (workers.size() == num_threads_max)) {
		stop_threads_count.store(num_threads_min);
		for (int i = 0; i < num_threads_min; ++i) {
			cv.notify_one();
		}
	}
	else if ((tasks.size() >= task_min.load()) && (workers.size() == num_threads_min.load())) {
		for (std::size_t i = num_threads_max.load() - num_threads_min.load(); i > 0; --i) {
			workers.emplace_back([this]() {worker_thread(); });
		}
	}
}

void thread_pool::enqueue_task(std::function<void()> task) {
	{
		std::lock_guard<std::mutex> lock(queue_mtx);
		tasks.push(std::move(task));
	}
	cv.notify_one();
}

thread_pool::thread_pool(const std::size_t& num_threads, const std::size_t& num_threads_min, const std::size_t& task_min)
	: stop(false), num_threads_max(num_threads),
	num_threads_min(num_threads_min), task_min(task_min) {
	for (std::size_t i = 0; i < num_threads; ++i) {
		workers.emplace_back([this]() { worker_thread(); });
	}
}

thread_pool::~thread_pool() {
	stop.store(true);
	cv.notify_all();
	for (std::thread& worker : workers) {
		if (worker.joinable()) {
			worker.join();
		}
	}
}