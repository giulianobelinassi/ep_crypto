#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>

/* Convert a block of 8 bytes to a proper 64 bits value
 * with proper cast using the endianess of the machine.
 */
uint64_t endian_64(const uint8_t bytes[]);

/* Convert a block of 8 bytes to 64bits integer being careful about the
 * endianess of the machine.
 */

uint8_t* endian_64_to_bytes(uint8_t bytes[], uint64_t value);
#endif /*ENDIAN_H*/
