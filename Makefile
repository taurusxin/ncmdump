default:
	@echo "Please choose a platform: linux, macos, win32"

linux:
	g++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag
	strip ncmdump

macos-intel:
	clang++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag
	strip ncmdump

macos-arm64:export CPATH=/opt/homebrew/include
macos-arm64:export LIBRARY_PATH=/opt/homebrew/lib
macos-arm64:
	clang++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag
	strip ncmdump

win32:
	g++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag -Ltaglib/lib -Itaglib/include -static -O
	strip ncmdump.exe

clean:
	rm -f ncmdump ncmdump.exe