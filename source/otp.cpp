#include <cstdint>
#include <span>
#include <hmac_sha1.hpp>
#include <algorithm>
#include <chrono>
#include <bit>
#include <otp.hpp>
#include <optional>
#include <charconv>
#include <string_view>
#include <iostream>
#include <sstream>
#include <base32.hpp>

uint32_t hotp(
	const std::span<unsigned char> secret,
	uint64_t counter
) {
	std::array<unsigned char, sizeof counter> ctr_bytes;
	if constexpr(std::endian::native == std::endian::little) {
		counter = std::byteswap(counter);
	}
	std::copy_n(reinterpret_cast<unsigned char *>(&counter), ctr_bytes.size(), ctr_bytes.begin());
	auto hash = hmac_sha1(secret, ctr_bytes);
	uint8_t offset = hash[SHA1_SIZE - 1] & 0x0f;
	// s is supposed to be 31-bit so we & 0x7f to erase top bit of top byte
	hash[offset] &= 0x7f;

	uint32_t s = *reinterpret_cast<uint32_t*>(hash.data() + offset);
	if constexpr(std::endian::native == std::endian::little) {
		s = std::byteswap(s);
	}

	return s % HOTP_DIGITS;
}

uint32_t totp(
	const std::span<unsigned char> secret,
	uint64_t timestamp
) {
	return hotp(secret, (timestamp - TOTP_START_TIME) / TOTP_INTERVAL);
}

uint32_t totp(
	const std::span<unsigned char> secret
) {
	auto now = std::chrono::system_clock::now();
	uint64_t timestamp = 
		std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
	
	return totp(secret, timestamp);
}

uint64_t totpValidity() {
	auto now = std::chrono::system_clock::now();
	uint64_t timestamp =
		std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

	return TOTP_INTERVAL - ((timestamp - TOTP_START_TIME) % TOTP_INTERVAL);
}

std::string zeroPadOTP(uint32_t otp) {
	std::ostringstream s;
	s << std::setw(HOTP_DIGIT_COUNT) << std::setfill('0') << otp;
	return s.str();
}

constexpr std::string OTP_AUTH_URL_PREFIX = "otpauth://";
constexpr std::string OTP_AUTH_TOTP_PREFIX = "totp/";
constexpr std::string OTP_AUTH_HOTP_PREFIX = "hotp/";

std::optional<OTPAuth> parseOTPAuthURL(std::string_view url) {
	OTPAuth res;

	if (!url.starts_with(OTP_AUTH_URL_PREFIX)) {
		return std::nullopt;
	}
	url = url.substr(OTP_AUTH_URL_PREFIX.size());

	if (url.starts_with(OTP_AUTH_TOTP_PREFIX)) {
		res.kind = OTPAuth::TOTP;
		url = url.substr(OTP_AUTH_TOTP_PREFIX.size());
	}
	else if (url.starts_with(OTP_AUTH_HOTP_PREFIX)) {
		res.kind = OTPAuth::HOTP;
		url = url.substr(OTP_AUTH_HOTP_PREFIX.size());
	}
	else {
		return std::nullopt;
	}

	size_t questionMarkIdx = url.find_first_of('?');
	// we don't want ? being the last character since that implies no parameters
	// ie. no secret
	if (questionMarkIdx >= url.size() - 1) {
		return std::nullopt;
	}
	// todo: uridecode this
	res.label = url.substr(0, questionMarkIdx);
	url = url.substr(questionMarkIdx + 1);

	bool foundSecret = false;
	bool foundCounter = false;

	while (url.size() != 0) {
		size_t nextParamIdx = url.find_first_of('&');
		auto param = url.substr(0, nextParamIdx);
		// skip processing & if it's last character
		if (nextParamIdx < url.size() - 1) {
			url = url.substr(nextParamIdx + 1);
		}
		else {
			url = url.substr(url.size());
		}
		size_t equalIdx = param.find_first_of('=');
		if (equalIdx >= param.size() - 1) {
			std::cerr << "parseOTPAuthURL: skipping uninitalized parameter: " << param << std::endl;
		}
		auto key = param.substr(0, equalIdx);
		auto value = param.substr(equalIdx + 1);
		if (key == "secret") {
			res.secret = decode_base32(value);
			foundSecret = true;
		}
		else if (key == "counter") {
			auto [ptr, err] = std::from_chars(value.data(), value.data() + value.size(), res.counter);
			// err equal to default-constructed std::errc means no error
			// ptr is at first character not part of parsed number -- it should
			// be at the end of the view (ie. nothing left unparsed)
			if (err == std::errc() && ptr == value.data() + value.size()) {
				foundCounter = true;
			}
		}
		else {
			std::cerr << "parseOTPAuthURL: ignoring unsupported parameter: " << key << std::endl;
		}
	}

	if (!foundSecret) {
		return std::nullopt;
	}

	if (!foundCounter && res.kind == OTPAuth::HOTP) {
		return std::nullopt;
	}

	return res;
}

std::string_view OTPAuthKind(OTPAuth::Kind kind) {
	if (kind == OTPAuth::HOTP) return "HOTP";
	if (kind == OTPAuth::TOTP) return "TOTP";
	return "???";
}
