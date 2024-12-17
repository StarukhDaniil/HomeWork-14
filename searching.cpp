#include "searching.h"
#include <filesystem>
#include <iostream>

void analyseDirectory(const std::filesystem::path& path) {
	namespace fs = std::filesystem;

	fs::recursive_directory_iterator search(path);
	int txtCounter = 0;
	int picCoutner = 0;
	int exeCounter = 0;
	int otherCounter = 0;

	uintmax_t txtMem = 0;
	uintmax_t picMem = 0;
	uintmax_t exeMem = 0;
	uintmax_t otherMem = 0;

	for (fs::directory_entry entry : search) {
		fs::path&& extension = entry.path().extension();
		if (extension == ".txt") {
			++txtCounter;
			txtMem += entry.file_size();
		}
		else if (extension == ".jpg" || extension == ".png" || extension == ".bmp") {
			++picCoutner;
			picMem += entry.file_size();
		}
		else if (extension == ".exe") {
			++exeCounter;
			exeMem += entry.file_size();
		}
		else if (!fs::is_directory(entry.path())) {
			++otherCounter;
			otherMem += entry.file_size();
		}
	}
	std::cout << "Number of:" << std::endl;
	std::cout << "Text files: " << txtCounter << std::endl;
	std::cout << "Picture files: " << picCoutner << std::endl;
	std::cout << "Executable files: " << exeCounter << std::endl;
	std::cout << "Other files(except directories): " << otherCounter << std::endl;
	std::cout << std::endl;
	std::cout << "Size of(in bytes):" << std::endl;
	std::cout << "Text files: " << txtMem << std::endl;
	std::cout << "Picture files: " << picMem << std::endl;
	std::cout << "Executable files: " << exeMem << std::endl;
	std::cout << "Other files: " << otherMem << std::endl;
}