#include "ncmcrypt.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void displayHelp() {
    std::cout << "Usage: ncmdump [-d] [-h] file1 file2 ..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -d                Process files in a folder (requires folder path)" << std::endl;
    std::cout << "  -h, --help       Display this help message" << std::endl;
}

void processFile(const std::string& filePath) {
    if (fs::exists(filePath) == false) {
        std::cerr << "Error: file '" << filePath << "' does not exist." << std::endl;
        return;
    }

    try {
        NeteaseCrypt crypt(filePath);
        crypt.Dump();
        crypt.FixMetadata();

        std::cout << "Done: " << crypt.dumpFilepath() << std::endl;
    } catch (const std::invalid_argument& e) {
        std::cout << "Exception: '" << filePath << "'" << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unexpected exception while processing file: " << filePath << std::endl;
    }
}

void processFilesInFolder(const std::string& folderPath) {
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (fs::is_regular_file(entry.status())) {
            processFile(entry.path().string());
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        displayHelp();
        return 1;
    }

    std::vector<std::string> files;
    bool processFolders = false;
    bool folderProvided = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            displayHelp();
            return 0;
        } else if (arg == "-d") {
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
            files.push_back(arg);
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
