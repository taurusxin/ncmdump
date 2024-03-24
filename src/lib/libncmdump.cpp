#include "ncmcrypt.h"

#ifdef _WIN32
#define API __declspec(dllexport)
#else
#define API
#endif

extern "C" {
	API NeteaseCrypt* CreateNeteaseCrypt(const char* path) {
		return new NeteaseCrypt(std::string(path));
	}

	API int Dump(NeteaseCrypt* neteaseCrypt) {
		try
		{
			neteaseCrypt->Dump();
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