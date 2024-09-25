#include "ncmcrypt.h"

#ifdef _WIN32
#define API __declspec(dllexport)
#else
#define API
#endif

extern "C" {
    API NeteaseCrypt* CreateNeteaseCrypt(const char* path);
    API int Dump(NeteaseCrypt* neteaseCrypt, const char* outputPath);
    API void FixMetadata(NeteaseCrypt* neteaseCrypt);
    API void DestroyNeteaseCrypt(NeteaseCrypt* neteaseCrypt);
}