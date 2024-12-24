#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <functional>
#include <vector>
#include <tuple>
#include <type_traits>

template <typename... T>
requires(std::is_copy_assignable_v<T> && ...)
class function_and_parameters {
private:
	std::function<void(T...)> func;
	std::tuple<T...> parameters;
public:
	const std::function<void(T...)>& get_function() {
		return func;
	}
	std::tuple<T...>& get_args() {
		return parameters;
	}
	function_and_parameters() {}
	function_and_parameters(const std::function<void(T...)>& func, T... args) :
		func(func) {
		if constexpr (sizeof...(T) != 0)
			parameters = std::make_tuple(args...);
	}
	function_and_parameters& operator=(const function_and_parameters<T...>& other) {
		if (this != &other) {
			this->func = other.func;
			if constexpr (sizeof...(T) != 0)
				this->parameters = other.parameters;
		}
		return *this;
	}
};

template<typename... T>
requires (std::is_copy_assignable_v<T> && ...)
class thread_pool {
private:
	std::vector<std::thread> workers;
	//std::queue<function_and_parameters<T...>> tasks;
	std::queue<function_and_parameters<T...>> tasks;
	std::atomic<bool> stop;
	std::atomic<std::size_t> stop_threads_count;
	std::atomic<std::size_t> num_threads_max;
	std::atomic<std::size_t> num_threads_min;
	std::atomic<std::size_t> task_min;
	std::mutex queue_mtx;
	std::mutex manager_mtx;
	std::condition_variable cv;
	void worker_thread();
	void thread_manager();
public:
	void enqueue_task(const std::function<void(T...)>& task, T... args);
	thread_pool(const std::size_t& num_threads, const std::size_t& num_threads_min, const std::size_t& task_min);
	~thread_pool();
};

#include "thread_pool.cpp"