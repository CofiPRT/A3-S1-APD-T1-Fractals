#ifndef _UTILS_H
#define _UTILS_H

#include "skel.h"

// if condition -> display message AND exit program
#define DIE(assertion, fmt, ...)						\
	do {												\
		if (assertion) {								\
			fprintf(stderr, "(%s, %d): " fmt,			\
					__FILE__, __LINE__, ## __VA_ARGS__);\
			exit(EXIT_FAILURE);							\
		}												\
	} while(0)

static inline double complex_norm(complex z) {
	return sqrt(z.a * z.a + z.b * z.b);
}

#endif