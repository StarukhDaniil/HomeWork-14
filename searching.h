#pragma once

#include <filesystem>
#include <fstream>
#include "thread_pool.h"
#include <sstream>
#include <shared_mutex>

class analyse_directory {
private:
	std::atomic<int> txtCounter;
	std::atomic<int> picCounter;
	std::atomic<int> exeCounter;
	std::atomic<int> otherCounter;
	std::atomic<uintmax_t> txtMem;
	std::atomic<uintmax_t> picMem;
	std::atomic<uintmax_t> exeMem;
	std::atomic<uintmax_t> otherMem;
	std::stringstream output;
	thread_pool<std::filesystem::path> tpool;
	std::shared_mutex cout_mtx;
	void analyse_for_thread_pool(const std::filesystem::path path);
public:
	void analyse(const std::filesystem::path& path);
	void print();
	analyse_directory() : tpool(5, 2, 10) {
		//std::cout << &output << std::endl;
	}
};