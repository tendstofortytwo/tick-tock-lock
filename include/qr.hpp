#ifndef TTC_QR_HPP
#define TTC_QR_HPP

#include <span>
#include <optional>
#include <string>
#include <cstdint>

class QRDecoder {
	struct quirc* qr;
	int w, h;
	static struct quirc* init(int w, int h);
	
public:
	QRDecoder(int w, int h);
	QRDecoder(const QRDecoder& other);
	QRDecoder(QRDecoder&& other);
	QRDecoder& operator=(const QRDecoder& other);
	QRDecoder& operator=(QRDecoder&& other);
	virtual ~QRDecoder();

    std::optional<std::string> scan(std::span<uint8_t> pic);
};

#endif