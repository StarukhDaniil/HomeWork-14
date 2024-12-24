#include "searching.h"
#include <filesystem>
#include <iostream>

void analyse_directory::analyse(const std::filesystem::path& path) {
	namespace fs = std::filesystem;

	fs::directory_iterator search(path);

	for (fs::directory_entry entry : search) {
		fs::path&& extension = entry.path().extension();
		if (entry.is_directory()) {
			tpool.enqueue_task([this](const std::filesystem::path& path) { this->analyse(path); }, entry.path());
		}
		else if (extension == ".txt") {
			txtCounter.fetch_add(1);
			txtMem.fetch_add(entry.file_size());
		}
		else if (extension == ".jpg" || extension == ".png" || extension == ".bmp") {
			picCounter.fetch_add(1);
			picMem.fetch_add(entry.file_size());
		}
		else if (extension == ".exe") {
			exeCounter.fetch_add(1);
			exeMem.fetch_add(entry.file_size());
		}
		else if (!fs::is_directory(entry.path())) {
			otherCounter.fetch_add(1);
			otherMem.fetch_add(entry.file_size());
		}
	}
}