#ifndef TTC_BASE32_H
#define TTC_BASE32_H

#include <vector>
#include <string_view>

std::vector<unsigned char> decode_base32(std::string_view s);

#endif