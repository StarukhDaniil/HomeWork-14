#pragma once
#include "thread_pool.h"
#include <iostream>

template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
void thread_pool<T...>::worker_thread() {
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
		function_and_parameters<T...> task;
		{
			std::unique_lock<std::mutex> task_lock(queue_mtx);
			cv.wait(task_lock, [this]() { 
				return stop.load() || !tasks.empty();
				});
			if (stop.load() && tasks.empty()) {
				return;
			}
			task = tasks.front();
			tasks.pop();
		}
		std::apply(task.get_function(), task.get_args());
	}
}

template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
void thread_pool<T...>::thread_manager() {
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

template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
void thread_pool<T...>::enqueue_task(const std::function<void(T...)>& task, T... args) {
	{
		std::lock_guard<std::mutex> lock(queue_mtx);
		if constexpr (sizeof...(T) != 0)
			tasks.emplace(task, args...);
		else
			tasks.emplace(task);
	}
	cv.notify_one();
}

template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
thread_pool<T...>::thread_pool(const std::size_t& num_threads, const std::size_t& num_threads_min, const std::size_t& task_min)
	: stop(false), num_threads_max(num_threads),
	num_threads_min(num_threads_min), task_min(task_min) {
	for (std::size_t i = 0; i < num_threads; ++i) {
		workers.emplace_back([this]() { worker_thread(); });
	}
}

template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
thread_pool<T...>::~thread_pool() {
	stop.store(true);
	cv.notify_all();
	for (std::thread& worker : workers) {
		if (worker.joinable()) {
			worker.join();
		}
	}
}