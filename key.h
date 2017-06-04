#ifndef KEY_H
#define KEY_H

#include <stdint.h>
#define NUM_ROUNDS 12

/* Generate key and subkeys from password.
 */
uint64_t* key_generate(const char* password);

#endif
