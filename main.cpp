#include <iostream>
#include <format>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

std::string getFileContent(const std::filesystem::path& filePath);
void getDirContent(const std::filesystem::path& dir_path, std::string& content, bool onlyPaths);
void CopyToClipboard(const std::string& text);

int main(int argc, char const* argv[])
{
    try
    {
        std::vector<std::string> args(argv, argv + argc);
        bool onlyPaths = std::find(args.begin(), args.end(), "--only-paths") != args.end();

        std::filesystem::path source_dir;

        std::string pathStr;
        for (size_t i = 1; i < args.size(); ++i)
        {
            if (args[i] != "--only-paths")
            {
                pathStr = args[i];
                break;
            }
        }

        if (pathStr.empty())
        {
            std::cout << "Enter path: ";
            std::getline(std::cin >> std::ws, pathStr);
        }

        source_dir = pathStr;

        if (!std::filesystem::exists(source_dir))
            throw std::runtime_error("Directory does not exist: " + source_dir.string());

        std::string content{};
        getDirContent(source_dir, content, onlyPaths);

        std::cout << content << std::endl;
        CopyToClipboard(content);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return -1;
    }
    return 0;
}

void getDirContent(const std::filesystem::path& dir_path, std::string& content, bool onlyPaths)
{
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path))
    {
        if (entry.is_regular_file())
        {
            std::string path = entry.path().string();
            if (onlyPaths) {
                content += path + "\n";
            }
            else {
                content += std::format("{}:\n{}\n\n", path, getFileContent(entry.path()));
            }
        }
    }
}

std::string getFileContent(const std::filesystem::path& filePath)
{
    std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
    if (!fileStream.is_open()) return "[Error: Could not open file]";

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}

#ifdef _WIN32
void CopyToClipboard(const std::string& text)
{
    if (OpenClipboard(nullptr))
    {
        EmptyClipboard();
        HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (hg != nullptr)
        {
            memcpy(GlobalLock(hg), text.c_str(), text.size() + 1);
            GlobalUnlock(hg);
            SetClipboardData(CF_TEXT, hg);
        }
        CloseClipboard();
    }
}
#else
void CopyToClipboard(const std::string& text) {}
#endif