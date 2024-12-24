#include <iostream>
#include <chrono>
#include "searching.h"
#include "thread_pool.h"

//std::mutex io_mtx;
//
//void thread_function() {
//	{
//		std::lock_guard lock(io_mtx);
//		std::cout << std::this_thread::get_id() << ": started function" << std::endl;
//	}
//	std::this_thread::sleep_for(std::chrono::seconds(1));
//	{
//		std::lock_guard lock(io_mtx);
//		std::cout << std::this_thread::get_id() << ": finished function" << std::endl;
//	}
//}

int main() {
	/*thread_pool tpool(3, 1, 2);
	for (int i = 0; i < 4; ++i) {
		tpool.enqueue_task(thread_function);
	}*/



	namespace fs = std::filesystem;
	fs::current_path("D:\\Hillel\\HW14");
	
	std::cout << "Enter path(relative or absolute): ";
	std::string pathInput;
	std::cin >> pathInput;
	
	fs::path path(pathInput);
	analyseDirectory(path);
	return 0;
}