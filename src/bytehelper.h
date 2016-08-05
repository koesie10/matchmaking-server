#pragma once

#include <stdint.h>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>

void print_array_as_hex(uint8_t * buf, int bufSize);

void print_vector_as_hex(std::vector<uint8_t> * vector);

template< typename T >
std::string int_to_hex(T i)
{
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i;
	return stream.str();
}
