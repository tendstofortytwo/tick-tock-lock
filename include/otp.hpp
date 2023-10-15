#ifndef TTC_OTP_HPP
#define TTC_OTP_HPP

#include <cstdint>
#include <span>
#include <optional>
#include <vector>

#define HOTP_DIGITS			1000000
#define HOTP_DIGIT_COUNT	6
#define TOTP_START_TIME		0
#define TOTP_INTERVAL		30

// contains the contents of an otpauth:// URL
struct OTPAuth {
	using Kind = enum {
		TOTP,
		HOTP
	};

	Kind kind;
	std::vector<unsigned char> secret;
	uint64_t counter;
	std::string label;
	// TODO: support other otpauth fields? these seem to be the only necessary ones
};

std::optional<OTPAuth> parseOTPAuthURL(
	std::string_view url
);

std::string_view OTPAuthKind(
	OTPAuth::Kind kind
);

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

uint64_t totpValidity();
std::string zeroPadOTP(uint32_t otp);

#endif