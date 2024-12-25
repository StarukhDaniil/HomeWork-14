#include "searching.h"
#include <filesystem>
#include <iostream>

void analyse_directory::analyse_for_thread_pool(const std::filesystem::path path) {
	namespace fs = std::filesystem;

	fs::directory_iterator search(path);
	std::shared_lock<std::shared_mutex> cout_lock;

	for (fs::directory_entry entry : search) {
		fs::path extension = entry.path().extension();
		if (entry.is_directory()) {
			tpool.enqueue_task([this](const std::filesystem::path path) { this->analyse_for_thread_pool(path); }, entry.path());
			continue;
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
		std::stringstream output_temp;
		output_temp << std::this_thread::get_id() << ": " << entry.path() << '\n';
		output << output_temp.str();
	}
}

void analyse_directory::analyse(const std::filesystem::path& path) {
	tpool.enqueue_task([this](const std::filesystem::path path) {this->analyse_for_thread_pool(path); }, path);
}

void analyse_directory::print() {
	tpool.enqueue_task([this](const std::filesystem::path path) {
		std::unique_lock<std::shared_mutex> cout_lock;
		std::cout << output.str();
		std::cout << "Number of:" << std::endl;
		std::cout << "Text files: " << txtCounter.load() << std::endl;
		std::cout << "Picture files: " << picCounter.load() << std::endl;
		std::cout << "Executable files: " << exeCounter.load() << std::endl;
		std::cout << "Other files(except directories): " << otherCounter.load() << std::endl;
		std::cout << std::endl;
		std::cout << "Size of(in bytes):" << std::endl;
		std::cout << "Text files: " << txtMem.load() << std::endl;
		std::cout << "Picture files: " << picMem.load() << std::endl;
		std::cout << "Executable files: " << exeMem.load() << std::endl;
		std::cout << "Other files: " << otherMem.load() << std::endl;
		std::cout << "Global size: " << txtMem.load() + picMem.load() + exeMem.load() + otherMem.load() << std::endl;
		}, std::filesystem::path());
}