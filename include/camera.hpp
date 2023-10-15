#ifndef TTC_CAMERA_HPP
#define TTC_CAMERA_HPP

#include <span>
#include <3ds.h>

class Camera {
	// A Handle is used by the 3DS as a synchronization mechanism. Handles are
	// uint32s, where 0 seems to mean "no handle set". If a handle is set,
	// svcWaitSynchronization and similar functions can be used to wait for
	// an event to be fired on the handles.
	// I'm not sure if I understand what that means, but it seems to be the best
	// way I can describe it. Oh well.
	// The first handle in this array is for the camera to signal that it got
	// interrupted while capturing a frame, and the second handle is to signal
	// that it finished capturing a frame.
	enum HandleKind {
		CAPTURE_INTERRUPTED,
		CAPTURE_COMPLETE,
		HANDLES_COUNT
	};
	Handle eventHandles[HANDLES_COUNT] = {0};
	void closeHandle(HandleKind kind);

	// Size of the buffer that holds a frame.
	uint32_t transferChunkSize = 0;
	uint8_t* buf = nullptr;

public:
	static constexpr size_t WIDTH = 400;
	static constexpr size_t HEIGHT = 240;
	static constexpr CAMU_Size SCREEN = SIZE_CTR_TOP_LCD;
	static constexpr uint32_t CAMERA = SELECT_OUT1;
	// times 2 since YUV422 pixels are 2 bytes long on average
	static constexpr size_t BUF_SIZE = WIDTH * HEIGHT * 2;

	Camera();
	Camera(const Camera& other) = delete;
	Camera(Camera&& other);
	Camera& operator=(const Camera& other) = delete;
	Camera& operator=(Camera&& other);
	virtual ~Camera();

	bool getFrame(std::span<uint8_t> out);
	void startNextCapture();
};

#endif