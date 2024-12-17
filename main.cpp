#include <iostream>
#include "searching.h"

int main() {
	namespace fs = std::filesystem;
	fs::current_path("D:\\Hillel\\HW14");
	
	std::cout << "Enter path(relative or absolute): ";
	std::string pathInput;
	std::cin >> pathInput;
	
	fs::path path(pathInput);
	analyseDirectory(path);
	return 0;
}