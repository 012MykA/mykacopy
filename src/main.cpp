#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

void get_dir_content(const std::filesystem::path& path, std::string& content);
std::string get_file_content(const std::filesystem::path& path);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    return -1;
  }

  std::string result;
  std::filesystem::path fs_path(argv[1]);
  get_dir_content(fs_path, result);

  std::cout << result << std::endl;
}

void get_dir_content(const std::filesystem::path& path, std::string& content) {
  for (auto& it : std::filesystem::directory_iterator(path)) {
    if (it.is_directory()) {
      get_dir_content(it, content);
    } else if (it.is_regular_file()) {
      content += std::format("{}:\n{}\n", it.path().string(),
                             get_file_content(it.path()));
    } else {
      content += std::format("File skipped: {}\n", it.path().string());
    }
  }
}

std::string get_file_content(const std::filesystem::path& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return {};
  }
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  return content;
}
