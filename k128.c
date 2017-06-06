#include "key.h"
#include "endian.h"

#include <stdio.h>

/*Evil hack. Trick the preprocessor to include a table from those files.*/
const uint8_t lookup_exp[256] = { 
	#include "lookup_exp.dat"
};
const uint8_t lookup_log[256] = {
	#include "lookup_log.dat"
};

#define CEIL(a, b) (1 + (((a)-1)/(b)))

void k128_ht2(uint8_t* b1, uint8_t* b2, uint8_t a1, uint8_t a2)
{
	*b1 = 2*a1 + a2;
	*b2 = a1 + a2;
}

void k128_ht2_inv(uint8_t *a1, uint8_t *a2, uint8_t b1, uint8_t b2)
{
	*a1 = b1 - b2;
	*a2 = 2*b2 - b1;
}

void k128_e_first_step(uint8_t subkey1_bytes[], uint8_t data[])
{
	data[0] = data[0] ^ subkey1_bytes[0];
	data[1] = data[1] + subkey1_bytes[1];
	data[2] = data[2] + subkey1_bytes[2];
	data[3] = data[3] ^ subkey1_bytes[3];
	data[4] = data[4] ^ subkey1_bytes[4];
	data[5] = data[5] + subkey1_bytes[5];
	data[6] = data[6] + subkey1_bytes[6];
	data[7] = data[7] ^ subkey1_bytes[7];
}

void k128_d_first_step(uint8_t subkey1_bytes[], uint8_t data[])
{
	data[0] = data[0] ^ subkey1_bytes[0];
	data[1] = data[1] - subkey1_bytes[1];
	data[2] = data[2] - subkey1_bytes[2];
	data[3] = data[3] ^ subkey1_bytes[3];
	data[4] = data[4] ^ subkey1_bytes[4];
	data[5] = data[5] - subkey1_bytes[5];
	data[6] = data[6] - subkey1_bytes[6];
	data[7] = data[7] ^ subkey1_bytes[7];
}

void k128_e_second_step(uint8_t data[])
{
	data[0] = lookup_exp[data[0]];
	data[1] = lookup_log[data[1]];
	data[2] = lookup_log[data[2]];
	data[3] = lookup_exp[data[3]];
	data[4] = lookup_exp[data[4]];
	data[5] = lookup_log[data[5]];
	data[6] = lookup_log[data[6]];
	data[7] = lookup_exp[data[7]];
}

void k128_d_second_step(uint8_t data[])
{
	data[0] = lookup_log[data[0]];
	data[1] = lookup_exp[data[1]];
	data[2] = lookup_exp[data[2]];
	data[3] = lookup_log[data[3]];
	data[4] = lookup_log[data[4]];
	data[5] = lookup_exp[data[5]];
	data[6] = lookup_exp[data[6]];
	data[7] = lookup_log[data[7]];
}
void k128_e_third_step(uint8_t subkey2_bytes[], uint8_t data[])
{
	data[0] = data[0] + subkey2_bytes[0];
	data[1] = data[1] ^ subkey2_bytes[1];
	data[2] = data[2] ^ subkey2_bytes[2];
	data[3] = data[3] + subkey2_bytes[3];
	data[4] = data[4] + subkey2_bytes[4];
	data[5] = data[5] ^ subkey2_bytes[5];
	data[6] = data[6] ^ subkey2_bytes[6];
	data[7] = data[7] + subkey2_bytes[7];
}

void k128_d_third_step(uint8_t subkey2_bytes[], uint8_t data[])
{
	data[0] = data[0] - subkey2_bytes[0];
	data[1] = data[1] ^ subkey2_bytes[1];
	data[2] = data[2] ^ subkey2_bytes[2];
	data[3] = data[3] - subkey2_bytes[3];
	data[4] = data[4] - subkey2_bytes[4];
	data[5] = data[5] ^ subkey2_bytes[5];
	data[6] = data[6] ^ subkey2_bytes[6];
	data[7] = data[7] - subkey2_bytes[7];
}

void k128_e_fourth_step(uint8_t data[])
{
	uint8_t buffer[2][8];
	int i;

	for (i = 0; i < 8; ++i)
		buffer[0][i] = data[i];

	k128_ht2(&buffer[1][0], &buffer[1][4], buffer[0][0], buffer[0][1]);
	k128_ht2(&buffer[1][1], &buffer[1][5], buffer[0][2], buffer[0][3]);
	k128_ht2(&buffer[1][2], &buffer[1][6], buffer[0][4], buffer[0][5]);
	k128_ht2(&buffer[1][3], &buffer[1][7], buffer[0][6], buffer[0][7]);

	k128_ht2(&buffer[0][0], &buffer[0][4], buffer[1][0], buffer[1][1]);
	k128_ht2(&buffer[0][1], &buffer[0][5], buffer[1][2], buffer[1][3]);
	k128_ht2(&buffer[0][2], &buffer[0][6], buffer[1][4], buffer[1][5]);
	k128_ht2(&buffer[0][3], &buffer[0][7], buffer[1][6], buffer[1][7]);
	
	k128_ht2(&data[0], &data[1], buffer[0][0], buffer[0][1]);
	k128_ht2(&data[2], &data[3], buffer[0][2], buffer[0][3]);
	k128_ht2(&data[4], &data[5], buffer[0][4], buffer[0][5]);
	k128_ht2(&data[6], &data[7], buffer[0][6], buffer[0][7]);
}

void k128_d_fourth_step(uint8_t data[])
{
	uint8_t buffer[2][8];
	int i;

	for (i = 0; i < 8; ++i)
		buffer[0][i] = data[i];

	k128_ht2_inv(&buffer[1][0], &buffer[1][2], buffer[0][0], buffer[0][1]);
	k128_ht2_inv(&buffer[1][4], &buffer[1][6], buffer[0][2], buffer[0][3]);
	k128_ht2_inv(&buffer[1][1], &buffer[1][3], buffer[0][4], buffer[0][5]);
	k128_ht2_inv(&buffer[1][5], &buffer[1][7], buffer[0][6], buffer[0][7]);

	k128_ht2_inv(&buffer[0][0], &buffer[0][2], buffer[1][0], buffer[1][1]);
	k128_ht2_inv(&buffer[0][4], &buffer[0][6], buffer[1][2], buffer[1][3]);
	k128_ht2_inv(&buffer[0][1], &buffer[0][3], buffer[1][4], buffer[1][5]);
	k128_ht2_inv(&buffer[0][5], &buffer[0][7], buffer[1][6], buffer[1][7]);
	
	k128_ht2_inv(&data[0], &data[1], buffer[0][0], buffer[0][1]);
	k128_ht2_inv(&data[2], &data[3], buffer[0][2], buffer[0][3]);
	k128_ht2_inv(&data[4], &data[5], buffer[0][4], buffer[0][5]);
	k128_ht2_inv(&data[6], &data[7], buffer[0][6], buffer[0][7]);
}

void k128_e_one_round(uint8_t subkeys_bytes[2][8], uint8_t data[])
{
	k128_e_first_step(subkeys_bytes[0], data);
	k128_e_second_step(data);
	k128_e_third_step(subkeys_bytes[1], data);
	k128_e_fourth_step(data);
}

void k128_d_one_round(uint8_t subkeys_bytes[2][8], uint8_t data[])
{
	k128_d_fourth_step(data);
	k128_d_third_step(subkeys_bytes[1], data);
	k128_d_second_step(data);
	k128_d_first_step(subkeys_bytes[0], data);
}

uint8_t* k128_encrypt(const char* password, size_t data_length, uint8_t data[]) 
{
	uint64_t* subkeys = key_generate(password);
	uint8_t subkey_bytes[2][8];
	const uint8_t cbc_init[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	const uint8_t* cbc_feed = cbc_init;
	int r;
	int i;
	
	for (i = 0; i < CEIL(data_length, 8); ++i)
	{
		
		for (r = 0; r < 8; ++r)
			data[8*i + r] = data[8*i + r] ^ cbc_feed[r];
		
		for (r = 1; r <= NUM_ROUNDS; ++r)
		{
			endian_64_to_bytes(subkey_bytes[0], subkeys[2*r - 1]);
			endian_64_to_bytes(subkey_bytes[1], subkeys[2*r]);

			k128_e_one_round(subkey_bytes, data + 8*i);
		}
		endian_64_to_bytes(subkey_bytes[0], subkeys[2*NUM_ROUNDS + 1]);
		k128_e_first_step(subkey_bytes[0], data + 8*i); /*Final Transformation*/
		cbc_feed = data + 8*i;
	}
	return data;
}

uint8_t* k128_decrypt(const char* password, size_t data_length, uint8_t data[])
{
	uint64_t* subkeys = key_generate(password);
	uint8_t subkey_bytes[2][8];
	int r;
	int i;
	
	for (i = CEIL(data_length, 8) - 1; i >= 0; --i)
	{
		endian_64_to_bytes(subkey_bytes[0], subkeys[2*NUM_ROUNDS + 1]);
		k128_d_first_step(subkey_bytes[0], data + 8*i); /*Reverse the final Transform.*/
		for (r = NUM_ROUNDS; r >= 1; --r)
		{
			endian_64_to_bytes(subkey_bytes[0], subkeys[2*r - 1]);
			endian_64_to_bytes(subkey_bytes[1], subkeys[2*r]);

			k128_d_one_round(subkey_bytes, data + 8*i);
		}
		if (i != 0)
		{	for (r = 0; r < 8; ++r)
				data[8*i + r] = data[8*i + r] ^ data[8*(i-1) + r];
		}
		else
		{	for (r = 0; r < 8; ++r)
				data[8*i + r] = data[8*i + r] ^ 0xff;
		}
	}
	return data;
}

#ifdef TEST_K128

int assert_bytes(uint8_t a[], uint8_t b[], int n)
{	int i;
	for (i = 0; i < n; ++i)
	{
		if (a[i] != b[i])
		{
			fputs("FAILED\n", stdout);
			return 0;
		}
	}
	fputs("OK\n", stdout);
	return 1;
}

int test_e_first_step(uint8_t subkey1_bytes[8])
{
	uint8_t B[]      = {0x43, 0x48, 0x3e, 0xb1, 0x8d, 0x5a, 0xdf, 0x93};
	uint8_t answer[] = {0x2d, 0x95, 0x87, 0x45, 0xba, 0x39, 0x99, 0x03};
	k128_e_first_step(subkey1_bytes, B);

	fputs("Testing first step of encryption process... ", stdout);

	return assert_bytes(B, answer, 8);
}

int test_e_second_step()
{
	uint8_t B[]      = {0x2d, 0x95, 0x87, 0x45, 0xba, 0x39, 0x99, 0x03};
	uint8_t answer[] = {0xf3, 0xfb, 0x0a, 0xb5, 0x0d, 0x6e, 0x4a, 0x93};
	k128_e_second_step(B);

	fputs("Testing second step of encryption process... ", stdout);

	return assert_bytes(B, answer, 8);
}

int test_e_third_step(uint8_t subkey2_bytes[8])
{
	uint8_t B[]      = {0xf3, 0xfb, 0x0a, 0xb5, 0x0d, 0x6e, 0x4a, 0x93};
	uint8_t answer[] = {0x44, 0xb0, 0x5f, 0x00, 0xac, 0x40, 0x8f, 0x97};

	k128_e_third_step(subkey2_bytes, B);

	fputs("Testing third step of encryption process... ", stdout);

	return assert_bytes(B, answer, 8);
}

int test_e_fourth_step()
{
	uint8_t B[]      = {0x44, 0xb0, 0x5f, 0x00, 0xac, 0x40, 0x8f, 0x97};
	uint8_t answer[] = {0x41, 0x13, 0x8c, 0x45, 0x39, 0x43, 0xb8, 0x65};

	k128_e_fourth_step(B);

	fputs("Testing fourth step of encryption process... ", stdout);

	return assert_bytes(B, answer, 8);
}

int test_e_one_round(uint8_t subkeys_bytes[2][8])
{
	uint8_t B[]      = {0x43, 0x48, 0x3e, 0xb1, 0x8d, 0x5a, 0xdf, 0x93};
	uint8_t answer[] = {0x41, 0x13, 0x8c, 0x45, 0x39, 0x43, 0xb8, 0x65};

	k128_e_one_round(subkeys_bytes, B);

	fputs("Testing one round of encryption process... ", stdout);

	return assert_bytes(B, answer, 8);
}

int test_d_first_step(uint8_t subkey1_bytes[8])
{
	uint8_t B[]      = {0x2d, 0x95, 0x87, 0x45, 0xba, 0x39, 0x99, 0x03}; 
	uint8_t answer[] = {0x43, 0x48, 0x3e, 0xb1, 0x8d, 0x5a, 0xdf, 0x93};

	k128_d_first_step(subkey1_bytes, B);

	fputs("Testing first step of decryption process... ", stdout);

	return assert_bytes(B, answer, 8);
}


int test_d_second_step()
{
	uint8_t B[]      = {0xf3, 0xfb, 0x0a, 0xb5, 0x0d, 0x6e, 0x4a, 0x93}; 
	uint8_t answer[] = {0x2d, 0x95, 0x87, 0x45, 0xba, 0x39, 0x99, 0x03};

	k128_d_second_step(B);

	fputs("Testing second step of decryption process... ", stdout);

	return assert_bytes(B, answer, 8);

}
int test_d_third_step(uint8_t subkey2_bytes[8])
{
	uint8_t B[]      = {0x44, 0xb0, 0x5f, 0x00, 0xac, 0x40, 0x8f, 0x97}; 
	uint8_t answer[] = {0xf3, 0xfb, 0x0a, 0xb5, 0x0d, 0x6e, 0x4a, 0x93};

	k128_d_third_step(subkey2_bytes, B);

	fputs("Testing third step of decryption process... ", stdout);

	return assert_bytes(B, answer, 8);
}
int test_d_fourth_step()
{
	uint8_t B[]      = {0x41, 0x13, 0x8c, 0x45, 0x39, 0x43, 0xb8, 0x65};
	uint8_t answer[] = {0x44, 0xb0, 0x5f, 0x00, 0xac, 0x40, 0x8f, 0x97};

	k128_d_fourth_step(B);

	fputs("Testing fourth step of decryption process... ", stdout);

	return assert_bytes(B, answer, 8);
}

int test_d_one_round(uint8_t subkeys_bytes[2][8])
{
	uint8_t B[]      = {0x41, 0x13, 0x8c, 0x45, 0x39, 0x43, 0xb8, 0x65};
	uint8_t answer[] = {0x43, 0x48, 0x3e, 0xb1, 0x8d, 0x5a, 0xdf, 0x93};

	k128_d_one_round(subkeys_bytes, B);

	fputs("Testing one round of decryption process... ", stdout);

	return assert_bytes(B, answer, 8);
}

int test_small_buffer()
{
	uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	char password[] = "cripto0336cripto";
	uint8_t buffer[16];
	int i;
	
	for (i = 0; i < 16; ++i)
		buffer[i] = data[i];

	fputs("Testing encript-decrypt with small buffer with size 16... ", stdout);

	k128_encrypt(password, 16, buffer);
	k128_decrypt(password, 16, buffer);

	return assert_bytes(data, buffer, 16);
}

int main()
{
	uint8_t subkeys_bytes[2][8] = {
		{0x6e, 0x4d, 0x49, 0xf4, 0x37, 0xdf, 0xba, 0x90},
		{0x51, 0x4b, 0x55, 0x4b, 0x9f, 0x2e, 0xc5, 0x04} 
	};
	
	test_e_first_step(subkeys_bytes[0]);
	test_e_second_step();
	test_e_third_step(subkeys_bytes[1]);
	test_e_fourth_step();

	test_e_one_round(subkeys_bytes);

	test_d_first_step(subkeys_bytes[0]);
	test_d_second_step();
	test_d_third_step(subkeys_bytes[1]);
	test_d_fourth_step();
	
	test_d_one_round(subkeys_bytes);

	test_small_buffer();
	return 0;	
}
#endif
