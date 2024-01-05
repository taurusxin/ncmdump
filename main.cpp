#include "ncmcrypt.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#endif

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

        std::cout << "Done: '" << crypt.dumpFilepath().string() << "'" << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Exception: '" << filePath << "'" << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unexpected exception while processing file: " << filePath << std::endl;
    }
}

void processFilesInFolder(const fs::path& folderPath) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (fs::is_regular_file(entry.status())) {
            processFile(entry.path());
        }
    }
}

#ifdef _WIN32
int wmain(int argc, wchar_t* argv[])
#else
int main(int argc, char **argv)
#endif
{
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif

    if (argc <= 1) {
        displayHelp();
        return 1;
    }

    std::vector<fs::path> files;
    bool processFolders = false;

    bool folderProvided = false;

    #ifdef _WIN32
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
            #ifdef _WIN32
            int multiByteStrSize = WideCharToMultiByte(CP_UTF8, 0, argv[1], -1, NULL, 0, NULL, NULL);
            char *multiByteStr = new char[multiByteStrSize];
            WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, multiByteStr, multiByteStrSize, NULL, NULL);
            fs::path path(multiByteStr);
            #else
            fs::path path(arg);
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
