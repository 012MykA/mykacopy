#include <iostream>
#include <format>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <Windows.h>

void getDirContent(const std::filesystem::path& dir_path, std::string& content);
std::string getFileContent(const std::filesystem::path& filePath);

void CopyToClipboard(const std::string& text);

int main(int argc, char const* argv[])
{
    try
    {
        std::filesystem::path source_dir;
        if (argc < 2)
        {
            std::cout << "enter path to directory you want copy: ";
            std::string path;
            std::cin >> path;
            source_dir = { path };
        }
        else
        {
            source_dir = { argv[1] };
        }

        if (!std::filesystem::exists(source_dir))
        {
            throw std::runtime_error("directory does not exists");
        }

        std::string content{};
        getDirContent(source_dir, content);

        std::cout << content << std::endl;
        CopyToClipboard(content);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }

    return 0;
}

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

void getDirContent(const std::filesystem::path& dir_path, std::string& content)
{
    std::filesystem::directory_iterator dir_iter(dir_path);
    for (const auto& entry : dir_iter)
    {
        if (entry.is_directory())
        {
            getDirContent(entry.path(), content);
        }
        else if (entry.is_regular_file())
        {
            std::string path = entry.path().string();
            std::string fileContent = getFileContent(entry.path());

            content += std::format("{}:\n{}\n", path, fileContent);
        }
        else
        {
            std::cout << "[LOG] Unreadable file: " << entry.path().string() << std::endl;
        }
    }
}

std::string getFileContent(const std::filesystem::path& filePath)
{
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open())
    {
        throw std::runtime_error("failed to open file: {}" + filePath.string());
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();

    fileStream.close();

    return buffer.str();
}