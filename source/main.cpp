#include <iostream>
#include <string>
#include <3ds.h>
#include <mbedtls/sha1.h>

#define SHA1_SIZE 20

void printHash(std::string& data) {
	unsigned char hash[SHA1_SIZE];

	std::cout << "data: " << data << std::endl;

	mbedtls_sha1((unsigned char *) data.c_str(), data.size(), hash);

	std::cout << "hash: " << std::hex;
	for(size_t i = 0; i < SHA1_SIZE; ++i) {
		std::cout << (int) hash[i];
	}
	std::cout << std::endl;
}

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	std::cout << "Hello, world!" << std::endl;

	std::string data = "test data";
	printHash(data);

	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
	}

	gfxExit();
	return 0;
}
