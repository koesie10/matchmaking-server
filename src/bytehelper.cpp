#include <cstdio>

#include "bytehelper.h"

void print_array_as_hex(uint8_t * buf, int bufSize) {
	int i;
	for (i = 0; i < bufSize; i++)
	{
		if (i > 0) printf(":");
		printf("%02X", buf[i]);
	}
	printf("\n");
}

void print_vector_as_hex(std::vector<uint8_t> * vector) {
	print_array_as_hex(vector->data(), vector->size());
}