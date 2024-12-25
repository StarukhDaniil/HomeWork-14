#include <iostream>
#include <chrono>
#include "searching.h"
#include "thread_pool.h"
#include "Windows.h"

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);


	namespace fs = std::filesystem;
	fs::current_path("D:\\Hillel\\HW14");
	
	std::cout << "Enter path(relative or absolute): ";
	std::string pathInput;
	std::cin >> pathInput;
	
	std::cout << std::this_thread::get_id() << std::endl;
	fs::path path(pathInput);
	analyse_directory searching;
	searching.analyse(path);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	searching.print();

	return 0;
}