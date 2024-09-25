#include "libncmdump.h"
#include <filesystem>

namespace fs = std::filesystem;

extern "C" {
    API NeteaseCrypt* CreateNeteaseCrypt(const char* path) {
        fs::path fPath = fs::u8path(path);
        return new NeteaseCrypt(fPath.u8string());
    }

    API int Dump(NeteaseCrypt* neteaseCrypt, const char* outputPath) {
        try
        {
            neteaseCrypt->Dump(outputPath);
        }
        catch (const std::invalid_argument& e)
        {
            return 1;
        }
        return 0;
    }

    API void FixMetadata(NeteaseCrypt* neteaseCrypt) {
        neteaseCrypt->FixMetadata();
    }

    API void DestroyNeteaseCrypt(NeteaseCrypt* neteaseCrypt) {
        delete neteaseCrypt;
    }
}