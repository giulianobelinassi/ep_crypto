#include "fileio.h"

#include <stdio.h>
#include <stdlib.h>


size_t file_get_size(FILE* file)
{
	size_t size;
	rewind(file);
	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	rewind(file);
	return size;
}

FILE_CRYPTO file_read(const char* path, FILE_MODE mode)
{
	FILE* in;
	int c;
	size_t fsize;
	uint8_t* ptr;

	FILE_CRYPTO file = {.data = NULL, .length = 0, .added_bytes = 0};
	
	in = fopen(path, "rb");
	if (!in)
		return file;
	
	fsize = file_get_size(in);

	file.data = (uint8_t*) malloc(fsize + 8);
	if (!file.data)
	{	fclose(in);
		return file;
	}
	if (mode == FILE_DECRYPT)
	{
		c = fgetc(in);
		if (c == EOF)
		{
			free(file.data);
			file.data = NULL;
			fclose(in);
			return file;
		}
		file.added_bytes = c;
		file.length = fsize - c - 1;
	}
	else /*mode == FILE_ENCRYPT*/
	{
		file.length = fsize;
		file.added_bytes = (8 - (fsize % 8)) % 8;
	}
	
	ptr = file.data;

	while ((c = fgetc(in)) != EOF)
		*ptr++ = (uint8_t) c;
	
	if (mode == FILE_ENCRYPT)
	{
		int i;
		for (i = 0; i < file.added_bytes; ++i)
			*ptr++ = 0xff;
	}
	fclose(in);
	return file;
}

/*Write file to disk*/
int file_write(const FILE_CRYPTO* file, const char* path, FILE_MODE mode)
{
	FILE* out;
	size_t n, i;

	out = fopen(path, "wb");
	if (!out)
		return 1;
	
	if (mode == FILE_ENCRYPT)
	{
		n = file->length + file->added_bytes;
		if (fputc(file->added_bytes, out) == EOF)
			return 2;
	}
	else /*FILE_DECRYPT*/
		n = file->length;

	for (i = 0; i < n; ++i)
	{
		if (fputc(file->data[i], out) == EOF)
		{
			fclose(out);
			return 2;
		}
	}
	fclose(out);
	return 0;
}

void file_destroy(FILE_CRYPTO* file)
{
	if (file->data != NULL)
		free(file->data);
	file->data = NULL;
	file->length = 0;
	file->added_bytes = 0;
}

#ifdef TEST_FILEIO
int test_file_copy()
{
	char fpath_in[] = "fileio.c";
	char fpath_out[] = "fileio.test";
	
	FILE_CRYPTO f;

	FILE* f1, *f2;
	int c1, c2;

	fputs("Testing file copy... ", stdout);

	f = file_read(fpath_in, FILE_ENCRYPT);
	file_write(&f, fpath_out, FILE_ENCRYPT);
	file_destroy(&f);
	
	f = file_read(fpath_out, FILE_DECRYPT);
	file_write(&f, fpath_out, FILE_DECRYPT);
	file_destroy(&f);

	f1 = fopen(fpath_in, "r");
	f2 = fopen(fpath_out, "r");

	while (1)
	{
		c1 = fgetc(f1);
		c2 = fgetc(f2);

		if (c1 == EOF && c2 == EOF)
		{
			fputs("OK\n", stdout);
			return 1;
		}

		if (c1 != c2)
		{
			fputs("FAILED\n", stdout);
			return 0;
		}
	}
	return 1;
}

int test_file_length()
{
	char fpath_in[] = "fileio.c";
	char fpath_out[] = "fileio.test";
	
	FILE_CRYPTO f1, f2;

	fputs("Testing file lengths ... ", stdout);

	f1 = file_read(fpath_in, FILE_ENCRYPT);
	file_write(&f1, fpath_out, FILE_ENCRYPT);
	
	f2 = file_read(fpath_out, FILE_DECRYPT);
	file_write(&f2, fpath_out, FILE_DECRYPT);


	if (f1.length == f2.length && f1.added_bytes == f2.added_bytes)
	{
		file_destroy(&f1);
		file_destroy(&f2);
		fputs("OK\n", stdout);
		return 1;
	}
	file_destroy(&f1);
	file_destroy(&f2);

	fputs("FAILED\n", stdout);
	return 0;

}

int test_buffer_concistency()
{
	char fpath_in[] = "fileio.c";
	char fpath_out[] = "fileio.test";
	int i;

	FILE_CRYPTO f1, f2;

	fputs("Testing buffers concinstency ... ", stdout);

	f1 = file_read(fpath_in, FILE_ENCRYPT);
	file_write(&f1, fpath_out, FILE_ENCRYPT);
	
	f2 = file_read(fpath_out, FILE_DECRYPT);
	file_write(&f2, fpath_out, FILE_DECRYPT);

	if (!(f1.length == f2.length && f1.added_bytes == f2.added_bytes))
	{
		file_destroy(&f1);
		file_destroy(&f2);
		fputs("FAILED\n", stdout);
		return 0;
	}

	for (i = 0; i < f1.length + f1.added_bytes; ++i)
	{
		if (f1.data[i] != f2.data[i])
		{
			fputs("FAILED\n", stdout);
			file_destroy(&f1);
			file_destroy(&f2);
			return 0;
		}
	}

	fputs("OK\n", stdout);
	file_destroy(&f1);
	file_destroy(&f2);
	return 0;
}

int main()
{
	test_file_copy();
	test_file_length();
	test_buffer_concistency();
	return 0;
}
#endif
