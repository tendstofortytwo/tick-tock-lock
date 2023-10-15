#include <iostream>
#include <string>
#include <3ds.h>
#include <otp.hpp>
#include <camera.hpp>
#include <qr.hpp>
#include <vector>
#include <cstdint>
#include <optional>

template<int width, int height>
void writeToFramebuffer(uint8_t* fb, std::span<uint8_t> pic) {
	for(int j = 0; j < height; ++j) {
		for(int i = 0; i < width; ++i) {
			int fbIdx = (i*height + (height - j)) * 3;
			int dataIdx = j*width + i;
			// show grayscale image by repeating same value for r, g, b
			fb[fbIdx] = pic[dataIdx];
			fb[fbIdx + 1] = pic[dataIdx];
			fb[fbIdx + 2] = pic[dataIdx];
		}
	}
}

int main(int argc, char* argv[])
{
	gfxInitDefault();
	gfxSet3D(false);
	consoleInit(GFX_BOTTOM, NULL);

	std::cout << "Hello, world!" << std::endl;

	Camera camera;
	QRDecoder qrdecoder(Camera::WIDTH, Camera::HEIGHT);
	std::vector<uint8_t> camBuf(Camera::WIDTH * Camera::HEIGHT, '\0');

	std::optional<OTPAuth> details = std::nullopt;

	int frameCounter = 0;

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START) {
			break; // break in order to return to hbmenu
		}
		if (kDown & KEY_A) {
			if (details.has_value()) {
				if (details->kind == OTPAuth::HOTP) {
					std::cout << zeroPadOTP(hotp(details->secret, details->counter)) << std::endl;
					details->counter++;
				}
				if (details->kind == OTPAuth::TOTP) {
					std::cout << zeroPadOTP(totp(details->secret)) << std::endl;
					std::cout << "valid for " << totpValidity() << " seconds!" << std::endl;
				}
			}
		}
		if (kDown & KEY_B) {
			details = std::nullopt;
		}

		camera.startNextCapture();
		if (camera.getFrame(camBuf)) {
			uint8_t* framebuffer = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
			writeToFramebuffer<Camera::WIDTH, Camera::HEIGHT>(framebuffer, camBuf);
			frameCounter = (frameCounter + 1) % 15;
			if(frameCounter == 0 && !details.has_value()) {
				details = qrdecoder.scan(camBuf).and_then(parseOTPAuthURL);
				if (details.has_value()) {
					std::cout << "kind: " << OTPAuthKind(details->kind) << std::endl;
					std::cout << "label: " << details->label << std::endl;
					if (details->kind == OTPAuth::HOTP) {
						std::cout << "counter: " << details->counter << std::endl;
					}
					std::cout << "stored details!" << std::endl;
					std::cout << "press A to generate next OTP" << std::endl;
					std::cout << "press B to clear OTP details" << std::endl;
				}
			}
		}

		gspWaitForVBlank();
		gfxSwapBuffers();
		gfxFlushBuffers();
	}

	gfxExit();
	return 0;
}