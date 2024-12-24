#pragma once

#include <filesystem>
#include <fstream>
#include "thread_pool.h"

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
	thread_pool<std::filesystem::path> tpool;
public:
	void analyse(const std::filesystem::path& path);
	analyse_directory() : tpool(5, 2, 10) {

	}
};