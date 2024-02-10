#pragma once

#include <filesystem>

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) \
|| defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define Q_DECL_EXPORT __declspec(dllexport)
#define Q_DECL_IMPORT __declspec(dllimport)
#else
#define Q_DECL_EXPORT __attribute__((visibility("default")))
#define Q_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(NCMDUMP_LIBRARY)
#  define NCMDUMP_EXPORT Q_DECL_EXPORT
#else
#  define NCMDUMP_EXPORT Q_DECL_IMPORT
#endif

class NeteaseCryptPrivateData;
class NCMDUMP_EXPORT NeteaseCrypt {

private:
    NeteaseCryptPrivateData *d;

public:
    const std::filesystem::path& filepath() const;
    const std::filesystem::path& dumpFilepath() const;

public:
	NeteaseCrypt(std::filesystem::path const&);
	~NeteaseCrypt();

public:
	void Dump();
	void FixMetadata();
};
