default:
	@echo "Please choose a platform: linux, macos-intel, macos-arm64, win32"
	@echo "Or use 'make clean' to clean up the build files"

linux:
	g++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag -std=c++17
	strip ncmdump

macos-intel:
	clang++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag -std=c++17
	strip ncmdump

macos-arm64:export CPATH=/opt/homebrew/include
macos-arm64:export LIBRARY_PATH=/opt/homebrew/lib
macos-arm64:
	clang++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag -std=c++17
	strip ncmdump

win32:
	g++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag -Ltaglib/lib -Itaglib/include -static -O -municode -std=c++17
	strip ncmdump.exe

clean:
	rm -f ncmdump ncmdump.exe