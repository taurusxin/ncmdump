#include "ncmcrypt.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <filesystem>
#include <cstring>

#if defined(_WIN32) && defined(_MSC_VER)
#include <Windows.h>
#endif

#include "color.h"

namespace fs = std::filesystem;

void displayHelp() {
    std::cout << "Usage: ncmdump [-d] [-h] file1 file2 ..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -d               Process files in a folder (requires folder path)" << std::endl;
    std::cout << "  -h, --help       Display this help message" << std::endl;
}

void processFile(const fs::path& filePath) {
    if (fs::exists(filePath) == false) {
        std::cerr << "Error: file '" << filePath.string() << "' does not exist." << std::endl;
        return;
    }

    try {
        NeteaseCrypt crypt(filePath.string());
        crypt.Dump();
        crypt.FixMetadata();

        std::cout << BOLDGREEN << "Done: '" << RESET << crypt.dumpFilepath().string() << "'" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cerr << BOLDRED << "Exception: " << RESET << RED << e.what() << RESET << " '" << filePath.string() << "'" << std::endl;
    } catch (...) {
        std::cerr << BOLDRED << "Unexpected exception while processing file: " << RESET << filePath.string() << std::endl;
    }
}

void processFilesInFolder(const fs::path& folderPath) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (fs::is_regular_file(entry.status())) {
            processFile(entry.path());
        }
    }
}

#if defined(_WIN32) && defined(_MSC_VER)
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char **argv)
#endif
{
#if defined(_WIN32) && defined(_MSC_VER)
    SetConsoleOutputCP(CP_UTF8);
#endif

    if (argc <= 1) {
        displayHelp();
        return 1;
    }

    std::vector<fs::path> files;
    bool processFolders = false;

    bool folderProvided = false;

#if defined(_WIN32) && defined(_MSC_VER)
#define COMPARE_STR(s1, s2) (wcscmp(s1, s2) == 0)
#define HELP_SHORT L"-h"
#define HELP_LONG L"--help"
#define FOLDER L"-d"
#else
#define COMPARE_STR(s1, s2) (strcmp(s1, s2) == 0)
#define HELP_SHORT "-h"
#define HELP_LONG "--help"
#define FOLDER "-d"
#endif

    for (int i = 1; i < argc; ++i) {
        if (COMPARE_STR(argv[i], HELP_SHORT) || COMPARE_STR(argv[i], HELP_LONG)) {
            displayHelp();
            return 0;
        } else if (COMPARE_STR(argv[i], FOLDER)) {
            processFolders = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                folderProvided = true;
                processFilesInFolder(argv[i + 1]);
                // Skip the folder name
                ++i;
            } else {
                std::cerr << "Error: -d option requires a folder path." << std::endl;
                return 1;
            }
        } else {
#if defined(_WIN32) && defined(_MSC_VER)
            int multiByteStrSize = WideCharToMultiByte(CP_UTF8, 0, argv[1], -1, NULL, 0, NULL, NULL);
            char *multiByteStr = new char[multiByteStrSize];
            WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, multiByteStr, multiByteStrSize, NULL, NULL);
            fs::path path(multiByteStr);
#else
            fs::path path(argv[i]);
#endif
            files.push_back(path);
        }
    }

    for (const auto& file : files) {
        if (processFolders && fs::is_directory(file)) {
            processFilesInFolder(file);
        } else {
            processFile(file);
        }
    }

    return 0;
}
