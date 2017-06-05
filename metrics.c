#include "endian.h"
#include "k128.h"

#include <stdlib.h>

#define GET_BIT(n, i) (((n) & ( 1 << (i) )) >> (i))
#define TOGGLE_BIT(n, i) ((*n) ^ (1 << (i)))

int metrics_hamming(uint64_t block1, uint64_t block2)
{
	int i, ham = 0;
	for (i = 0; i < 64; ++i)
		ham += abs(GET_BIT(block1, i) - GET_BIT(block2, i));
	return ham;
}

int metrics_accumuled_hamming(size_t data_length, uint8_t data1[], uint8_t data2[])
{
	size_t i, n = data_length/8;
	int ham = 0;

	if (data_length % 8 != 0)
		return -1;

	for (i = 0; i < n; ++i)
		ham += metrics_hamming(endian64(data1 + 8*i, data2 + 8*i));
	return ham;
}

uint8_t* metrics_generate_modified_data(size_t data_length, uint8_t data[], size_t to_toggle)
{
	size_t i, n = data_length/8;
	int byte, bit;

	uint8_t* modified_data = (uint8_t*) malloc(sizeof(data_length));

	for (i = 0; i < data_length; ++i)
		modified_data = data[i];

	for (i = 0; i < n; ++i)
	{
		byte = to_toggle/8;
		bit = to_toggle % 8;
		modified_data[8*i + byte] = TOGGLE_BIT(modified_data[8*i + byte], bit);
		store_bits[i] = to_toggle;
	}
	return modified_data;
}

uint8_t* metrics_copy_vector(size_t length, uint8_t v[])
{
	size_t i;
	uint8_t* copy = (uint8_t*) malloc(length);
	for (i = 0; i < length; ++i)
		copy[i] = v[i];
	return copy;
}

#define MAX(a, b) (((a) > (b))? (a): (b))
#define MIN(a, b) (((a) < (b))? (a): (b))



void metrics_calculate_1(const char* password, size_t data_length, uint8_t data[])
{
	uint8_t* vector_input;
	uint8_t* vector_input_c;
	uint8_t* vector_modified;
	uint8_t* vector_modified_c;
	int* ham_distance;
	int* sum_ham;
	int* max_ham;
	int* min_ham;
	
	size_t n = data_length / 8;
	size_t j, k, bit_to_toggle;

	if (data_length % 8 != 0)
		return -1;

	ham_distance   = (int*) malloc(sizeof(int)*n);
	sum_ham        = (int*) calloc(n, sizeof(int));
	max_ham        = (int*) calloc(n, sizeof(int));
	min_ham        = (int*) calloc(n, sizeof(int));
	
	vector_input   = metrics_copy_vector(data_length, data);
	vector_input_c = k128_encrypt(password, data_length, data);

	for (i = 0; i < n; ++i)
		max_ham[i] = 1000;

	for (i = 0; i < n; ++i)
	{
		for (j = 0; j < 64; ++j)
		{
			bit_to_toggle = 8*i + j;
			vector_modified   = metrics_generate_modified_data(data_length, vector_input, bit_to_toggle);
			vector_modified_c = k128_encrypt(password, data_length, metrics_copy_vector(data_length, vector_modified));
			for (k = i; k < n; ++k)
			{
				int ham = metrics_hamming(((uint64_t*) vector_modified_c)[k] , ((uint64_t*)vector_input_c)[k]);
				sum_ham[k] += ham;
				max_ham[k] = MAX(max_ham[k], ham);
				min_ham[k] = MIN(min_ham[k], ham);
			}
		}
		
	}

	free(vector_input);
	free(vector_modified);
	free(vector_modified_c);
	free(sum_ham);
	free(max_ham);
	free(min_ham);
}
