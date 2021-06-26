#ifndef UTILS_BASE64_HPP
#define UTILS_BASE64_HPP

void base64_encode(unsigned char const* bytes_to_encode, 
                    unsigned int in_len, std::string& dst);
std::string base64_decode(std::string const& encoded_string);

#endif // UTILS_BASE64_HPP