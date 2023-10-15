#include <3ds.h>
#include <cstdint>
#include <span>
#include <vector>
#include <limits>
#include <camera.hpp>

Camera::Camera() {
	camInit();
	// todo: no idea what a CONTEXT is, assuming it's like a state of settings
	// and I can switch from one state to the other instantly? example code
	// uses CONTEXT_A and never switches, so hopefully that works for me too
	CAMU_SetSize(CAMERA, SCREEN, CONTEXT_A);

	CAMU_SetOutputFormat(CAMERA, OUTPUT_YUV_422, CONTEXT_A);
	CAMU_SetFrameRate(CAMERA, FRAME_RATE_15);
	CAMU_SetNoiseFilter(CAMERA, true);
	CAMU_SetAutoExposure(CAMERA, true);
	CAMU_SetAutoWhiteBalance(CAMERA, true);
	CAMU_SetTrimming(PORT_CAM1, false);

	CAMU_GetMaxBytes(&transferChunkSize, WIDTH, HEIGHT);
	CAMU_SetTransferBytes(PORT_CAM1, transferChunkSize, WIDTH, HEIGHT);
	CAMU_Activate(CAMERA);
	CAMU_GetBufferErrorInterruptEvent(&eventHandles[CAPTURE_INTERRUPTED], PORT_CAM1);
	CAMU_ClearBuffer(PORT_CAM1);
	CAMU_StartCapture(PORT_CAM1);

	buf = new uint8_t[BUF_SIZE];
}

Camera::Camera(Camera&& other) {
	for(int i = 0; i < HANDLES_COUNT; ++i) {
		eventHandles[i] = other.eventHandles[i];
		other.eventHandles[i] = 0;
	}
	transferChunkSize = other.transferChunkSize;
	other.transferChunkSize = 0;
	buf = other.buf;
	other.buf = nullptr;
}

Camera& Camera::operator=(Camera&& other) {
	if(this == &other) {
		return *this;
	}
	for(int i = 0; i < HANDLES_COUNT; ++i) {
		eventHandles[i] = other.eventHandles[i];
		other.eventHandles[i] = 0;
	}
	transferChunkSize = other.transferChunkSize;
	other.transferChunkSize = 0;
	buf = other.buf;
	other.buf = nullptr;
	return *this;
}

Camera::~Camera() {
	CAMU_StopCapture(PORT_CAM1);
	CAMU_Activate(SELECT_NONE);
	if (buf) {
		delete[] buf;
	}
	for (int i = 0; i < HANDLES_COUNT; ++i) {
		if (eventHandles[i] != 0) {
			svcCloseHandle(eventHandles[i]);
		}
	}
	camExit();
}

void Camera::closeHandle(HandleKind kind) {
	svcCloseHandle(eventHandles[kind]);
	eventHandles[kind] = 0;
}

// YUV422 gives us output in the following byte format
// [U Y V] [Y] [U Y V] [Y] [U Y V] [Y] ...
// where each [] represents a pixel, and the half-pixels without UV
// information are supposed to share it with the pixel prior to them.
// since I only want a greyscale image, I'm going to completely ignore
// the UV values, and essentially I only have to read every other byte
// to get the Y (luminance values).
bool Camera::getFrame(std::span<uint8_t> out) {
	size_t i, j;
	int32_t handleIdx;
	svcWaitSynchronizationN(&handleIdx, eventHandles, HANDLES_COUNT, false, std::numeric_limits<int64_t>::max());
	switch(handleIdx) {
	case CAPTURE_INTERRUPTED:
		closeHandle(CAPTURE_COMPLETE);
		CAMU_StartCapture(PORT_CAM1);
		return false;
		break;
	case CAPTURE_COMPLETE:
		closeHandle(CAPTURE_COMPLETE);
		for(i = 0, j = 0; j < out.size(); i += 2, ++j) {
			out[j] = buf[i];
		}
		return true;
		break;
	};
	return false;
}

void Camera::startNextCapture() {
	CAMU_SetReceiving(&eventHandles[CAPTURE_COMPLETE], buf, PORT_CAM1, BUF_SIZE, transferChunkSize);
}