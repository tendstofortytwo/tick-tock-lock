#include <vector>
#include <string>
#include <bit>
#include <bitset>
#include <cstdint>
#include <unordered_map>
#include <iostream>

std::unordered_map<char, unsigned char> decode_lookup {
	{ 'A', 0 },
	{ 'B', 1 },
	{ 'C', 2 },
	{ 'D', 3 },
	{ 'E', 4 },
	{ 'F', 5 },
	{ 'G', 6 },
	{ 'H', 7 },
	{ 'I', 8 },
	{ 'J', 9 },
	{ 'K', 10 },
	{ 'L', 11 },
	{ 'M', 12 },
	{ 'N', 13 },
	{ 'O', 14 },
	{ 'P', 15 },
	{ 'Q', 16 },
	{ 'R', 17 },
	{ 'S', 18 },
	{ 'T', 19 },
	{ 'U', 20 },
	{ 'V', 21 },
	{ 'W', 22 },
	{ 'X', 23 },
	{ 'Y', 24 },
	{ 'Z', 25 },
	{ '2', 26 },
	{ '3', 27 },
	{ '4', 28 },
	{ '5', 29 },
	{ '6', 30 },
	{ '7', 31 },
};

#define BITSET_SIZE		40
#define BASE32_BITS		5

void append_bytes(std::vector<unsigned char>& res, uint64_t bits, int count) {
	if constexpr (std::endian::native == std::endian::little) {
		bits = std::byteswap(bits);
	}

	unsigned char * bytes = reinterpret_cast<unsigned char *>(&bits);

	for(size_t i = sizeof(bits) - (BITSET_SIZE / 8); i < sizeof(bits) && count > 0; ++i, --count) {
		res.push_back(bytes[i]);
	}
}

std::vector<unsigned char> decode_base32(const std::string &s) {
	std::vector<unsigned char> res;
	uint64_t bits = 0;
	size_t blank_count = 0;
	size_t i = 0;

	for(i = 0; i < s.size(); ++i) {
		if(s[i] == '=') {
			break;
		}
		else if(decode_lookup.contains(s[i])) {
			bits = (bits * (1 << BASE32_BITS)) + decode_lookup[s[i]];
		}
		else {
			std::cerr << "base32_decode: skipping unknown character " << s[i] << " at index " << i << std::endl;
		}

		if(i % 8 == 7) {
			append_bytes(res, bits, (BITSET_SIZE / 8));
			bits = 0;
		}
	}

	for(; i % 8 != 0; ++i, ++blank_count) {
		bits = bits * (1 << BASE32_BITS);
	}

	switch(blank_count) {
	case 0:
		break;
	case 6:
		append_bytes(res, bits, 1);
		break;
	case 4:
		append_bytes(res, bits, 2);
		break;
	case 3:
		append_bytes(res, bits, 3);
		break;
	case 1:
		append_bytes(res, bits, 4);
		break;
	default:
		std::cerr << "base32_decode: inferred or got " <<  blank_count << " padding characters, which should not be possible\n";
	}

	return res;
}
