win32:
	g++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag -Ltaglib/lib -Itaglib/include -static -O
	strip ncmdump.exe

linux:
	g++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag
	strip ncmdump

macos:
	g++ main.cpp cJSON.cpp aes.cpp ncmcrypt.cpp -o ncmdump -ltag
	strip ncmdump
