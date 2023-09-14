#ifndef TTC_HMAC_SHA1_HPP
#define TTC_HMAC_SHA1_HPP

#include <array>
#include <span>

#define SHA1_SIZE		20
#define HMAC_BLOCK_SIZE	64
#define HMAC_IPAD		0x36
#define HMAC_OPAD		0x5C

std::array<unsigned char, SHA1_SIZE> sha1(
	const std::span<unsigned char> data
);

std::array<unsigned char, SHA1_SIZE> hmac_sha1(
	const std::span<unsigned char> secret,
	const std::span<unsigned char> text
);

#endif