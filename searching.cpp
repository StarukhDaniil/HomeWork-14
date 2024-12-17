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

	for (fs::directory_entry entry : search) {
		fs::path&& extension = entry.path().extension();
		if (extension == ".txt") {
			++txtCounter;
		}
		else if (extension == ".jpg" || extension == ".png" || extension == ".bmp") {
			++picCoutner;
		}
		else if (extension == ".exe") {
			++exeCounter;
		}
		else if (!fs::is_directory(entry)) {
			++otherCounter;
		}
	}

	std::cout << "Text files: " << txtCounter << std::endl;
	std::cout << "Picture files: " << picCoutner << std::endl;
	std::cout << "Executable files: " << exeCounter << std::endl;
	std::cout << "Other files(except directories): " << otherCounter << std::endl;
}