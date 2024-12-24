#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <functional>
#include <vector>

class thread_pool {
private:
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
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
	void enqueue_task(std::function<void()> task);
	thread_pool(const std::size_t& num_threads, const std::size_t& num_threads_min, const std::size_t& task_min);
	~thread_pool();
};