#include <iostream>
#include "searching.h"

int main() {
	namespace fs = std::filesystem;
	fs::current_path("D:\\Hillel\\HW14");
	fs::path path("test");
	analyseDirectory(path);
	return 0;
}