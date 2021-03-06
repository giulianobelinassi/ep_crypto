#include "fileio.h"
#include "k128.h"
#include "metrics.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef enum {
	MODE_ENCRYPT,
	MODE_DECRYPT,
	MODE_1,
	MODE_2,
	MODE_INVALID
} MODE;

int check_password(const char* password)
{
	int digits = 0, alphas = 0;
	int n, i;
	
	
	if (password == NULL)
		return 0;

	n = strlen(password);

	if (n < 8)
		return 0;

	for (i = 0; i < n; ++i)
	{
		if (isdigit(password[i]))
			digits++;
		else if (isalpha(password[i]))
			alphas++;
	}
	if (digits >= 2 && alphas >= 2)
		return 1;
	return 0;
}

int check_args(const char* fpath_in, const char* fpath_out, MODE mode)
{
	switch (mode)
	{
		case MODE_ENCRYPT:
		case MODE_DECRYPT:
			if (!fpath_in || !fpath_out)
				return 0;
		break;

		case MODE_1:
		case MODE_2:
			if (!fpath_in)
				return 0;
		break;

		case MODE_INVALID:
			return 0;
		break;
	}
	return 1;
}
void print_usage_message()
{
	fputs(
	"\nERROR: Incorrect Parameters.\n"
	"Valid parameters are:\n"
	"-c            Encrypt a file.\n"
	"-d            Decrypt a file.\n"
	"-1            Display metrics with single toggle mode.\n"
	"-2            Display metrics with double toggle mode.\n"
	"-i <file>     Input file.\n"
	"-o <file>     Output file.\n"
	"-p <password> Password to be used. Must have at least 8 characters,\n"
	"              2 letters and 2 decimals.\n"
	"-a            Erase input file. Use with care.\n"
	"For more information, please consult the EP page.\n\n",
	stdout
	);
}

void assert_remove(int a)
{
	if (a != 0)
		fputs("\nWARNING: Unable to remove input file.\n", stdout);
}

int main(int argc, char* argv[])
{
	const char* password = NULL;
	const char* fpath_in = NULL;
	const char* fpath_out = NULL;
	MODE mode = MODE_INVALID;
	int delete_input = 0;

	FILE_CRYPTO file;

	int i;
	for (i = 0; i < argc; ++i)
	{
		if (!strcmp(argv[i], "-i"))
		{	if (i+1 < argc)
				fpath_in = argv[i+1];
		}
		else if (!strcmp(argv[i], "-o"))
		{
			if (i+1 < argc)
				fpath_out = argv[i+1];
		}
		else if (!strcmp(argv[i], "-p"))
		{
			if (i+1 < argc)
				password = argv[i+1];
		}
		else if (!strcmp(argv[i], "-d"))
			mode = MODE_DECRYPT;
		else if (!strcmp(argv[i], "-c"))
			mode = MODE_ENCRYPT;
		else if (!strcmp(argv[i], "-1"))
			mode = MODE_1;
		else if (!strcmp(argv[i], "-2"))
			mode = MODE_2;
		else if (!strcmp(argv[i], "-a"))
			delete_input = 1;
	}
	if (!check_password(password))
	{
		print_usage_message();
		return 1;
	}
	if (!check_args(fpath_in, fpath_out, mode))
	{
		print_usage_message();
		return 1;
	}


	if (mode == MODE_ENCRYPT)
	{
		
		file = file_read(fpath_in, FILE_ENCRYPT);
		if (file.data == NULL)
		{
			file_destroy(&file);
			fputs("\nError trying to read the file. Check if you have access to it\n", stdout);
			return 2;
		}
		if (delete_input)
			assert_remove(remove(fpath_in));
		k128_encrypt(password, file.length + file.added_bytes, file.data);
		file_write(&file, fpath_out, FILE_ENCRYPT);
	}
	else if (mode == MODE_DECRYPT)
	{
		file = file_read(fpath_in, FILE_DECRYPT);
		if (file.data == NULL)
		{
			file_destroy(&file);
			fputs("\nError trying to read the file. Check if you have access to it\n", stdout);
			return 2;
		}
		if (delete_input)
			assert_remove(remove(fpath_in));
		k128_decrypt(password, file.length + file.added_bytes, file.data);
		file_write(&file, fpath_out, FILE_DECRYPT);
	}
	else if (mode == MODE_1 || mode == MODE_2)
	{
		METRICS_MODE mode_m = (mode == MODE_1)? METRICS_SINGLE_TOGGLE: METRICS_DOUBLE_TOGGLE;
		file = file_read(fpath_in, FILE_ENCRYPT);
		if (file.data == NULL)
		{
			file_destroy(&file);
			fputs("\nError trying to read the file. Check if you have access to it\n", stdout);
			return 2;
		}
		if (delete_input)
			assert_remove(remove(fpath_in));
		metrics_calculate(password, file.length + file.added_bytes, file.data, mode_m);
	}

	file_destroy(&file);
	return 0;
}
