#pragma once
#include "thread_pool.h"
#include <iostream>
#include <algorithm>

template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
void thread_pool<T...>::worker_thread() {
	while (true) {
		// if there are too few or too many tasks go to thread_manager()
		if (std::unique_lock<std::mutex> manager_lock(manager_mtx, std::try_to_lock);
			manager_lock.owns_lock() && ((tasks.size() < task_min.load() && workers.size() == num_threads_max.load())
			|| (tasks.size() > task_min.load() && workers.size() == num_threads_min.load()))) {
			thread_manager();
		}
		function_and_parameters<T...> task;
		{
			std::unique_lock<std::mutex> task_lock(queue_mtx);
			cv_queue.wait(task_lock, [this]() {
				return stop.load() || !tasks.empty() || stop_threads_count.load() > 0;
				});
			//if there are some threads to stop(because there are to few tasks), stop this one
			if (stop_threads_count.load() > 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				std::unique_lock<std::mutex> erase_lock(erase_mtx);
				std::vector<std::thread>::iterator find_thread_for_erase = std::find_if(workers.begin(), workers.end(),
					[this](const std::thread& thread) {
						return std::this_thread::get_id() == thread.get_id();
					});
				stop_threads_count.fetch_sub(1);
				thread_erase_it = find_thread_for_erase;
				erase_lock.unlock();
				cv_erase.notify_one();
				return;
			}
			else if (!tasks.empty()) {
				task = tasks.front();
				tasks.pop();
			}
			else if (stop.load() && tasks.empty()) {
				return;
			}
		}
		std::apply(task.get_function(), task.get_args());
	}
}

//thread manager adds threads if there are to many tasks or erases threads if there are to few tasks
template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
void thread_pool<T...>::thread_manager() {
	if (tasks.size() < task_min.load() && (workers.size() == num_threads_max)) {
		auto thread_diff = num_threads_max.load() - num_threads_min.load();
		stop_threads_count.store(thread_diff);
		for (auto i = 0; i < thread_diff; ++i) {
			if (stop.load() && tasks.empty()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				if (tasks.empty()) {
					return;
				}
			}
			cv_queue.notify_one();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			std::unique_lock<std::mutex> erase_lock(erase_mtx);
			cv_erase.wait(erase_lock, [this]() {
				return thread_erase_it != workers.end();
				});
			if (thread_erase_it->joinable()) {
				thread_erase_it->join();
			}
			workers.erase(thread_erase_it);
			thread_erase_it = workers.end();
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
	cv_queue.notify_one();
}

template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
thread_pool<T...>::thread_pool(const std::size_t& num_threads, const std::size_t& num_threads_min, const std::size_t& task_min)
	: stop(false), num_threads_max(num_threads),
	num_threads_min(num_threads_min), task_min(task_min) {
	for (std::size_t i = 0; i < num_threads; ++i) {
		workers.emplace_back([this]() { worker_thread(); });
	}
	thread_erase_it = workers.end();
}

template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
thread_pool<T...>::~thread_pool() {
	stop.store(true);
	while (tasks.size() != 0) {
		cv_queue.notify_all();
	}
	for (std::thread& worker : workers) {
		if (worker.joinable()) {
			worker.join();
		}
	}
}