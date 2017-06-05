#include "endian.h"
#include "k128.h"

#include <stdlib.h>
#include <stdio.h>

#define GET_BIT(n, i) (((n) & ( 1 << (i) )) >> (i))
#define TOGGLE_BIT(n, i) ((n) ^ (1 << (7 - (i))))

int metrics_hamming(uint64_t block1, uint64_t block2)
{
	int i, ham = 0;
	for (i = 0; i < 64; ++i)
		ham += abs(GET_BIT(block1, i) - GET_BIT(block2, i));
	return ham;
}

uint8_t* metrics_generate_modified_data(size_t data_length, uint8_t data[], size_t to_toggle)
{
	size_t i, n = data_length/8;
	int byte, bit;

	uint8_t* modified_data = (uint8_t*) malloc(data_length);

	for (i = 0; i < data_length; ++i)
		modified_data[i] = data[i];

	byte = to_toggle/8;
	bit = to_toggle % 8;
	modified_data[byte] = TOGGLE_BIT(modified_data[byte], bit);
	
	return modified_data;
}

uint8_t* copy_vector(size_t length, uint8_t v[])
{
	size_t i;
	uint8_t* copy = (uint8_t*) malloc(length);
	for (i = 0; i < length; ++i)
		copy[i] = v[i];
	return copy;
}

#define MAX(a, b) (((a) > (b))? (a): (b))
#define MIN(a, b) (((a) < (b))? (a): (b))

void print_table(size_t n, int ham_max[], int ham_min[], uint64_t sum_ham[])
{
	size_t i;
	fputs("\nMetrics:\n", stdout);
	fputs("Maximum Hamming: ", stdout);
	for (i = 0; i < n; ++i)
		fprintf(stdout, "%d ", ham_max[i]);
	
	fputs("\nMinimum Hamming: ", stdout);
	for (i = 0; i < n; ++i)
		fprintf(stdout, "%d ", ham_min[i]);
	
	fputs("\nMean of Values : ", stdout);
	for (i = 0; i < n; ++i)
		fprintf(stdout, "%lu ", (sum_ham[i]/(64*(i+1))));
	
}

int metrics_calculate_1(const char* password, size_t data_length, uint8_t data[])
{
	uint8_t* vector_input_c;
	uint64_t* sum_ham;
	int* max_ham;
	int* min_ham;
	
	size_t n = data_length / 8;
	size_t i, j, k, bit_to_toggle;

	if (data_length % 8 != 0)
		return 1;

	sum_ham        = (uint64_t*) calloc(n, sizeof(uint64_t));
	max_ham        = (int*) calloc(n, sizeof(int));
	min_ham        = (int*) calloc(n, sizeof(int));
	
	vector_input_c = k128_encrypt(password, data_length, copy_vector(data_length, data));

	for (i = 0; i < n; ++i)
		max_ham[i] = 1000;

	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < 64; ++j)
		{
			
			uint8_t* vector_modified_c;
			bit_to_toggle = 8*i + j;
			
			vector_modified_c = k128_encrypt(password, data_length, metrics_generate_modified_data(data_length, data, bit_to_toggle));
			for (k = i; k < n; ++k)
			{
				int ham = metrics_hamming(((uint64_t*) vector_modified_c)[k] , ((uint64_t*)vector_input_c)[k]);
				sum_ham[k] += ham;
				max_ham[k] = MAX(max_ham[k], ham);
				min_ham[k] = MIN(min_ham[k], ham);
			}
			free(vector_modified_c);
		}
		
	}

	print_table(n, max_ham, min_ham, sum_ham);

	free(vector_input_c);
	free(sum_ham);
	free(max_ham);
	free(min_ham);

	return 0;
}

#ifdef TEST_METRICS
int test_hamming_distance()
{
	uint64_t a1 = 2, a2 = 3;
	int ham = metrics_hamming(a1, a2);
	
	fputs("Testing hamming function... ", stdout);
	
	if (ham == 1)
	{
		fputs("OK\n", stdout);
		return 1;
	}
	fputs("FAILED\n", stdout);
	return 0;
}

int test_modify_data()
{
	uint8_t data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};
	uint8_t result[]={0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x01};

	uint8_t* modified = metrics_generate_modified_data(16, data, 127);
	int i;

	fputs("Testing vector bit toggling function... ", stdout);
	
	for (i = 0; i < 16; ++i)
	{
		if (modified[i] != result[i])
		{
			fputs("FAILED\n", stdout);
			free(modified);
			return 0;
		}
	}
	fputs("OK\n", stdout);
	free(modified);
	return 1;
}

int main()
{
	test_hamming_distance();
	test_modify_data();
	return 0;
}
#endif
