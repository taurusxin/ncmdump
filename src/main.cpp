#include "ncmcrypt.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <filesystem>

#if defined(_WIN32)
#include "platform.h"
#endif

#include "color.h"
#include "version.h"
#include "cxxopts.hpp"

namespace fs = std::filesystem;

void processFile(const fs::path &filePath, const fs::path &outputFolder)
{
    if (fs::exists(filePath) == false)
    {
        std::cerr << BOLDRED << "[Error] " << RESET << "file '" << filePath.u8string() << "' does not exist." << std::endl;
        return;
    }

    // skip if not ending with ".ncm"
    if (!filePath.has_extension() || filePath.extension().u8string() != ".ncm")
    {
        return;
    }

    try
    {
        NeteaseCrypt crypt(filePath.u8string());
        crypt.Dump(outputFolder.u8string());
        crypt.FixMetadata();

        std::cout << BOLDGREEN << "[Done] " << RESET << "'" << filePath.u8string() << "' -> '" << crypt.dumpFilepath().u8string() << "'" << std::endl;
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << BOLDRED << "[Exception] " << RESET << RED << e.what() << RESET << " '" << filePath.u8string() << "'" << std::endl;
    }
    catch (...)
    {
        std::cerr << BOLDRED << "[Error] Unexpected exception while processing file: " << RESET << filePath.u8string() << std::endl;
    }
}

int main(int argc, char **argv)
{
#if defined(_WIN32)
    win32_utf8argv(&argc, &argv); // Convert command line arguments to UTF-8 under Windows
#endif

    cxxopts::Options options("ncmdump");

    options.add_options()
    ("h,help", "Print usage")
    ("d,directory", "Process files in a folder (requires folder path)", cxxopts::value<std::string>())
    ("r,recursive", "Process files recursively (requires -d option)", cxxopts::value<bool>()->default_value("false"))
    ("o,output", "Output folder (default: original file folder)", cxxopts::value<std::string>())
    ("v,version", "Print version information", cxxopts::value<bool>()->default_value("false"))
    ("filenames", "Input files", cxxopts::value<std::vector<std::string>>());

    options.positional_help("<files>");

    options.parse_positional({"filenames"});

    options.allow_unrecognised_options();

    // Parse options the usual way
    cxxopts::ParseResult result;
    try {
        result = options.parse(argc, argv);
    } catch(cxxopts::exceptions::parsing const& e) {
        std::cout << options.help() << std::endl;
        return 1;
    }

    // print usage message if unrecognised options are present
    if (result.unmatched().size() > 0)
    {
        std::cout << options.help() << std::endl;
        return 1;
    }

    // display help message
    if (result.count("help"))
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    // display version information
    if (result.count("version"))
    {
        std::cout << "ncmdump version " << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << std::endl;
        return 0;
    }

    // no input files or folder provided
    if (result.count("directory") == 0 && result.count("filenames") == 0)
    {
        std::cout << options.help() << std::endl;
        return 1;
    }

    // only -r option without -d option
    if (result.count("recursive") && result.count("directory") == 0)
    {
        std::cerr << BOLDRED << "[Error] " << RESET << "-r option requires -d option." << std::endl;
        return 1;
    }

    // check output folder
    fs::path outputDir = fs::u8path("");
    bool outputDirSpecified = result.count("output") > 0;

    if (outputDirSpecified)
    {
        outputDir = fs::u8path(result["output"].as<std::string>());
        if (fs::exists(outputDir))
        {
            if (!fs::is_directory(outputDir))
            {
                std::cerr << BOLDRED << "[Error] " << RESET << "'" << outputDir.u8string() << "' is not a valid directory." << std::endl;
                return 1;
            }
        }
        fs::create_directories(outputDir);
    }

    // process files in a folder
    if (result.count("directory"))
    {
        fs::path sourceDir = fs::u8path(result["directory"].as<std::string>());
        if (!fs::is_directory(sourceDir))
        {
            std::cerr << BOLDRED << "[Error] " << RESET << "'" << sourceDir.u8string() << "' is not a valid directory." << std::endl;
            return 1;
        }

        bool recursive = result["recursive"].as<bool>();

        if (recursive)
        {
            // 递归遍历源目录
            for (const auto &entry : fs::recursive_directory_iterator(sourceDir))
            {
                // 没有指定输出目录，则使用源目录作为输出目录
                if (!outputDirSpecified)
                {
                    outputDir = sourceDir;
                }

                // 获得递归遍历的相对路径
                const auto &path = fs::u8path(entry.path().u8string());
                auto relativePath = fs::relative(path, sourceDir);
                fs::path destinationPath = outputDir / relativePath;

                if (fs::is_regular_file(path))
                {
                    // 确保输出文件的目录存在
                    fs::create_directories(destinationPath.parent_path());

                    // 处理文件
                    processFile(path, destinationPath.parent_path());
                }
            }
        }
        else
        {
            for (const auto &entry : fs::directory_iterator(sourceDir))
            {
                const auto &path = fs::u8path(entry.path().u8string());
                if (entry.is_regular_file())
                {
                    if (outputDirSpecified)
                    {
                        processFile(path, outputDir);
                    }
                    else
                    {
                        processFile(path, "");
                    }
                }
            }
        }
        return 0;
    }

    // process individual files
    if (result.count("filenames"))
    {
        for (const auto &filePath : result["filenames"].as<std::vector<std::string>>())
        {
            fs::path filePathU8 = fs::u8path(filePath);
            if (!fs::is_regular_file(filePathU8))
            {
                std::cerr << BOLDRED << "[Error] " << RESET << "'" << filePathU8.u8string() << "' is not a valid file." << std::endl;
                continue;
            }

            if (outputDirSpecified)
            {
                processFile(filePathU8, outputDir);
            }
            else
            {
                processFile(filePathU8, "");
            }
        }
    }
    return 0;
}
