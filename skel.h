#ifndef _SKEL_H
#define _SKEL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

extern char *in_filename_julia;
extern char *in_filename_mandelbrot;
extern char *out_filename_julia;
extern char *out_filename_mandelbrot;
extern int P; // thread count

// complex number struct
typedef struct _complex {
	double a;
	double b;
} complex;

// julia/mandelbrot params
typedef struct _params {
	int iterations, width, height, area;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia;
} params;

void get_args(int argc, char **argv);
void read_input_file(char *in_filename, params* par);
void write_output_file(char *out_filename, int **result, int width, int height);
int **allocate_memory(int width, int height);
void free_memory(int **result, int height);

#endif