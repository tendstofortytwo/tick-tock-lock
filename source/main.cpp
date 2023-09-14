#include <iostream>
#include <string>
#include <3ds.h>
#include <otp.hpp>
#include <base32.hpp>

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	std::cout << "Hello, world!" << std::endl;

	// test case from https://github.com/hectorm/otpauth/blob/66c70a31bd775248d9159620ca37110e471372e6/test/test.mjs#L178
	// used under MIT license https://github.com/hectorm/otpauth/blob/master/LICENSE.md
	auto secret = decode_base32("OR6O5BU2ZCD6PPEJ6OB2LKW5SXUZ7LJM6KS3ND7PX664ZOTWZOY6JJN24KX3N2FPVPT3BA7RXO6ISLJN26MOLF4O6GDK3AHTQ6S3XY4PW7UITDRA6OUZPCGVU7Z2HHE34KL2G");

	std::cout << "totp: got " << totp(secret, 1451606400)  << ", expected 329537" << std::endl;
	std::cout << "hotp: got " << hotp(secret, 10000000000) << ", expected 361593" << std::endl;

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
