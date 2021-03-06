#ifndef K128_H
#define K128_H

#include <stddef.h>

/** Encrypt data using password.
  */
uint8_t* k128_encrypt(const char* password, size_t data_length, uint8_t data[]);

/** Decrypt data using password
  */
uint8_t* k128_decrypt(const char* password, size_t data_length, uint8_t data[]);

#endif /*K128_H*/
