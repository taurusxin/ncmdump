#include "ncmcrypt.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <filesystem>

#if defined(_WIN32)
#include "platform.h"
#endif

#include "color.h"

namespace fs = std::filesystem;

void displayHelp()
{
    std::cout << "Usage: ncmdump [-d] [-h] file1 file2 ..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -d               Process files in a folder (requires folder path)" << std::endl;
    std::cout << "  -h, --help       Display this help message" << std::endl;
}

void processFile(const fs::path &filePath)
{
    if (fs::exists(filePath) == false)
    {
        std::cerr << BOLDRED << "Error: " << RESET << "file '" << filePath.u8string() << "' does not exist." << std::endl;
        return;
    }

    try
    {
        NeteaseCrypt crypt(filePath.u8string());
        crypt.Dump();
        crypt.FixMetadata();

        std::cout << BOLDGREEN << "Done: " << RESET << "'" << crypt.dumpFilepath().u8string() << "'" << std::endl;
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << BOLDRED << "Exception: " << RESET << RED << e.what() << RESET << " '" << filePath.u8string() << "'" << std::endl;
    }
    catch (...)
    {
        std::cerr << BOLDRED << "Unexpected exception while processing file: " << RESET << filePath.u8string() << std::endl;
    }
}

void processFilesInFolder(const fs::path &folderPath)
{
    for (const auto &entry : fs::directory_iterator(folderPath))
    {
        if (fs::is_regular_file(entry.status()))
        {
            processFile(entry.path());
        }
    }
}

int main(int argc, char **argv)
{
#if defined(_WIN32)
    win32_utf8argv(&argc, &argv);
#endif
    if (argc <= 1)
    {
        displayHelp();
        return 1;
    }

    std::vector<fs::path> files;
    bool processFolders = false;

    bool folderProvided = false;

#define COMPARE_STR(s1, s2) (strcmp(s1, s2) == 0)
#define HELP_SHORT "-h"
#define HELP_LONG "--help"
#define PROCESS_FOLDER "-d"
    for (int i = 1; i < argc; ++i)
    {
        if (COMPARE_STR(argv[i], HELP_SHORT) || COMPARE_STR(argv[i], HELP_LONG))
        {
            displayHelp();
            return 0;
        }
        else if (COMPARE_STR(argv[i], PROCESS_FOLDER))
        {
            processFolders = true;
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                folderProvided = true;
                processFilesInFolder(argv[i + 1]);
                // Skip the folder name
                ++i;
            }
            else
            {
                std::cerr << "Error: -d option requires a folder path." << std::endl;
                return 1;
            }
        }
        else
        {
            fs::path path = fs::u8path(argv[i]);
            files.push_back(path);
        }
    }

    for (const auto &file : files)
    {
        if (processFolders && fs::is_directory(file))
        {
            processFilesInFolder(file);
        }
        else
        {
            processFile(file);
        }
    }

    return 0;
}
