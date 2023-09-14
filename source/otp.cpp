#include <cstdint>
#include <span>
#include <hmac_sha1.hpp>
#include <algorithm>
#include <chrono>
#include <bit>
#include <otp.hpp>

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
