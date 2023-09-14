#ifndef TTC_OTP_HPP
#define TTC_OTP_HPP

#include <cstdint>
#include <span>

#define HOTP_DIGITS		1000000
#define TOTP_START_TIME	0
#define TOTP_INTERVAL	30

uint32_t hotp(
	const std::span<unsigned char> secret,
	uint64_t counter
);

uint32_t totp(
	const std::span<unsigned char> secret,
	uint64_t timestamp
);

uint32_t totp(
	const std::span<unsigned char> secret
);

#endif