#include "key.h"
#include "endian.h"

#include <string.h>
#include <stdio.h>

#define KEY_SIZE 16

/*See https://stackoverflow.com/questions/13289397/circular-shift-in-c*/
#define ROT_SHL64(a, x) (((a) << (x)) | ((a) >> (64 - (x)))) 

void key_password(uint8_t buffer[KEY_SIZE], const char* password)
{
	int i;
	int pw_len = strlen(password);

	for (i = 0; i < KEY_SIZE; ++i)
		buffer[i] = password[i % pw_len];	
}

uint64_t* key_generate(const char* password)
{
	uint8_t key[KEY_SIZE];
	uint64_t L[3*NUM_ROUNDS];
	static uint64_t subkeys[3*NUM_ROUNDS];
	uint64_t A = 0, B = 0;
	int j;
	
	key_password(key, password);
	
	L[0] = endian_64(key);
	L[1] = endian_64(key+8);

	for (j = 2; j <= 2*NUM_ROUNDS+1; ++j)
		L[j] = L[j-1] + 0x9e3779b97f4a7c15ULL;

	subkeys[0] = 0xb7e151628aed2a6bULL;
	for (j = 1; j <= 2*NUM_ROUNDS+1; ++j)
		subkeys[j] = subkeys[j-1] + 0x7f4a7c159e3779b9ULL;
	
	for (j = 0; j < 2*NUM_ROUNDS+1; ++j)
	{
		subkeys[j] = ROT_SHL64((subkeys[j] + A + B), 3);
		A = subkeys[j];
		L[j] = ROT_SHL64((L[j] + A + B), (A + B));
		B = L[j];
	}
	return subkeys;
}

#ifdef TEST_KEY
int test_password_generation()
{

	char password[] = "oimamae12";
	uint8_t buffer[KEY_SIZE];
	
	key_password(buffer, password);

	fprintf(stdout, "Testing key generation with password %s... ", password);
	
	if (!strncmp(buffer, "oimamae12oimamae", KEY_SIZE))
	{
		fputs("OK\n", stdout);
		return 0;
	}
	fputs("FAILED\n", stdout);
	return 1;
}

int test_key_generation()
{
	const char const password[] = "cripto0336cripto";
	const uint64_t const K[26] = {
		0xbf0a8b145769535d,
		0x7d8b47f426ecb08c,
		0xea62edd51764d135,
		0xafe63f36cef9deca,
		0x7c2b387220d2a61f,
		0x42faa99d95e0fa87,
		0x20705379481f5d52,
		0xe134633870c754f7,
		0xcd8c2617578dd07c,
		0x90f008193c5ee4b3,
		0x61ded2223afce4a7,
		0x8cbc478874eb92c0,
		0x0ef034f568aeff11,
		0x3349b6c15d907712,
		0x9d29fb715be8c723,
		0x7f3ac68dde660732,
		0xf05688789e09d165,
		0xc84cc1d2df334fea,
		0x1ef042ec0d528268,
		0x0f7745af7b7f9b10,
		0x68d5aca0ab3dbc36,
		0x144298245b8a848f,
		0x8e56cbd7069fb772,
		0x3a1da6205a67e920,
		0x9f5db405eddc11d2,
		0x26276f7efe580d7c
	};
	uint64_t* subkeys = key_generate(password);
	int i;

	fprintf(stdout, "Testing subkey generation with password %s... ", password);

	for (i = 0; i < 26; ++i)
	{
		if (subkeys[i] != K[i])
		{	fputs("FAILED\n", stdout);
			return 0;
		}
	}
		fputs("OK\n", stdout);
		return 0;
}

int main()
{
	test_password_generation();
	test_key_generation();
}
#endif
