#ifndef METRICS_H
#define METRICS_H

#include <stddef.h>

typedef enum {
	METRICS_SINGLE_TOGGLE,
	METRICS_DOUBLE_TOGGLE
} METRICS_MODE;

/** Calculate all metrics specified in the the 7nth topic.
 */
int metrics_calculate(
		const char* password, 
		size_t data_length, 
		unsigned char data[],
		METRICS_MODE mode
		);
#endif
