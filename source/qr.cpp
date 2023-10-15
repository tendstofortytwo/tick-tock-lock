#include <quirc.h>
#include <new>
#include <utility>
#include <span>
#include <die.hpp>
#include <qr.hpp>

struct quirc* QRDecoder::init(int w, int h) {
	struct quirc* qr = quirc_new();
	if (!qr) {
		die("QRDecoder: quirc_new could not alloc");
	}
	int ret = quirc_resize(qr, w, h);
	if (ret != 0) {
		die("QRDecoder: quirc_resize could not alloc");
	}
	return qr;
}

QRDecoder::QRDecoder(int width, int height): w{width}, h{height} {
	qr = init(w, h);
}

QRDecoder::QRDecoder(const QRDecoder& other) {
	qr = init(other.w, other.h);
	w = other.w;
	h = other.h;
}

QRDecoder::QRDecoder(QRDecoder&& other) {
	qr = other.qr;
	other.qr = nullptr;
	w = other.w;
	h = other.h;
}

QRDecoder& QRDecoder::operator=(const QRDecoder& other) {
	if (this == &other) {
		return *this;
	}
	quirc_destroy(qr);
	qr = init(other.w, other.h);
	w = other.w;
	h = other.h;
	return *this;
}

QRDecoder& QRDecoder::operator=(QRDecoder&& other) {
	std::swap(other.qr, qr);
	std::swap(other.h, h);
	std::swap(other.w, w);
	return *this;
}

QRDecoder::~QRDecoder() {
	if (qr) {
		quirc_destroy(qr);
	}
}

std::optional<std::string> QRDecoder::scan(std::span<uint8_t> pic) {
	std::optional<std::string> res;

	uint8_t * buf = quirc_begin(qr, nullptr, nullptr);

	for (int j = 0; j < h; ++j) {
		for (int i = 0; i < w; ++i) {
			buf[j * w + i] = pic[j * w + i];
		}
	}

	quirc_end(qr);

	int codes = quirc_count(qr);
	
	for(int i = 0; i < codes; ++i) {
		struct quirc_code code;
		struct quirc_data data;
		quirc_decode_error_t err;

		quirc_extract(qr, i, &code);

		err = quirc_decode(&code, &data);

		// this error means we need to flip the QR code horizontally and try again
		if (err == QUIRC_ERROR_DATA_ECC) {
			quirc_flip(&code);
			err = quirc_decode(&code, &data);
		}

		if (err) {
			std::cerr << "QR decode failed: " << quirc_strerror(err) << std::endl;
		}
		else {
			res = std::string((char *) data.payload);
			return res;
		}
	}

	return res;
}
