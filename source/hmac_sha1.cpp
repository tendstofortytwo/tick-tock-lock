#include <hmac_sha1.hpp>
#include <array>
#include <span>
#include <vector>
#include <algorithm>
#include <mbedtls/sha1.h>

std::array<unsigned char, SHA1_SIZE> sha1(
	const std::span<unsigned char> data
) {
	std::array<unsigned char, SHA1_SIZE> res;
	mbedtls_sha1(data.data(), data.size(), res.data());
	return res;
}

std::array<unsigned char, SHA1_SIZE> hmac_sha1(
	const std::span<unsigned char> secret,
	const std::span<unsigned char> text
) {
	std::array<unsigned char, HMAC_BLOCK_SIZE> key;
	std::fill_n(key.begin(), HMAC_BLOCK_SIZE, 0x00);
	if(secret.size() <= HMAC_BLOCK_SIZE) {
		std::copy_n(secret.begin(), secret.size(), key.begin());
	}
	else {
		mbedtls_sha1(secret.data(), secret.size(), key.data());
	}

	std::vector<unsigned char> inner_hash_text(HMAC_BLOCK_SIZE + text.size());
	for(size_t i = 0; i < HMAC_BLOCK_SIZE; ++i) {
		inner_hash_text[i] = key[i] ^ HMAC_IPAD;
	}
	std::copy_n(text.data(), text.size(), &inner_hash_text[HMAC_BLOCK_SIZE]);
	auto inner_hash = sha1(inner_hash_text);

	std::array<unsigned char, HMAC_BLOCK_SIZE + SHA1_SIZE> outer_hash_text;
	for(size_t i = 0; i < HMAC_BLOCK_SIZE; ++i) {
		outer_hash_text[i] = key[i] ^ HMAC_OPAD;
	}
	std::copy_n(inner_hash.begin(), SHA1_SIZE, &outer_hash_text[HMAC_BLOCK_SIZE]);
	auto outer_hash = sha1(outer_hash_text);
	return outer_hash;
}
