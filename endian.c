#include "endian.h"

#include <stdio.h>
#include <string.h>

uint64_t endian_64(const uint8_t bytes[])
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	/*Assume a big endian processor (PowerPC)*/
	return *((uint64_t*) bytes);
#else /*Assume a little endian processor (x86)*/
	union {
		uint8_t bytes[8];
		uint64_t value;
	} endianess;
	
	int i;
	for (i = 0; i < 8; ++i)
		endianess.bytes[7-i] = bytes[i];
	return endianess.value;
#endif
}

uint8_t* endian_64_to_bytes(uint8_t bytes[], uint64_t value)
{
	union {
		uint8_t bytes[8];
		uint64_t value;
	} endianess;
	int i;
	endianess.value = value;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	/*Assume a big endian processor (PowerPC)*/	
	for (i = 0; i < 8; ++i)
		bytes[i] = endianess.bytes[i];
#else /*Assume a little endian processor (x86)*/
	for (i = 0; i < 8; ++i)
		bytes[i] = endianess.bytes[7-i];
#endif
	return bytes;
}

#ifdef TEST_ENDIAN
int main()
{
	const uint64_t value = 0x1122334455667788;
	const uint8_t bytes[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
	uint8_t buffer[8];

	fprintf(stdout, "Testing endianess cast functions... ");
	endian_64_to_bytes(buffer, value);
	
	if (strncmp(bytes, buffer, 8) || endian_64(bytes) != value)
	{
		fputs("FAILED\n", stdout);
		return 1;
	}
	fputs("OK\n", stdout);
	return 0;
}
#endif
