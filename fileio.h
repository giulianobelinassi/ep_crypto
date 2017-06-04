#ifndef FILEIO_H
#include <stddef.h>
#include <stdint.h>

struct file_crypto {
	uint8_t* data;   /*File data*/
	size_t length;   /*Size of file*/
	int added_bytes; /*Added bytes to length become divisible by 8*/
};

typedef enum {
	FILE_ENCRYPT,
	FILE_DECRYPT
} FILE_MODE;

/*Read file.*/
typedef struct file_crypto FILE_CRYPTO;

FILE_CRYPTO file_read(const char* path, FILE_MODE mode);

/*Write file to disk*/
int file_write(const FILE_CRYPTO* file, const char* path, FILE_MODE mode);

void file_destroy(FILE_CRYPTO* file);

#endif /*FILEIO_H*/
