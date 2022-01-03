#include "skel.h"
#include "utils.h"

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int P;

// read command line arguments
void get_args(int argc, char **argv) {
	DIE(argc < 6, "[ERROR] Missing parameters. Usage:\n"
		"\t./tema1_par julia_in julia_out "
		"mandelbrot_in mandelbrot_out thread_count\n");

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	P = atoi(argv[5]);
}

// parse input file
void read_input_file(char *in_filename, params* par) {
	FILE *file = fopen(in_filename, "r");
	DIE(!file, "[ERROR] Could not open file '%s' for reading\n", in_filename);

	int is_julia, result;

	result = fscanf(file, "%d", &is_julia);
	DIE(result != 1,
		"[ERROR] Could not read 'is_julia' from file '%s'\n", in_filename);

	result = fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	DIE(result != 4,
		"[ERROR] Could not read minimum and maximum x's and y's "
		"from file '%s'\n", in_filename);

	result = fscanf(file, "%lf", &par->resolution);
	DIE(result != 1,
		"[ERROR] Could not read resolution from file '%s'\n", in_filename);

	result = fscanf(file, "%d", &par->iterations);
	DIE(result != 1,
		"[ERROR] Could not read the number of iterations "
		"from file '%s'\n", in_filename);

	par->width = (par->x_max - par->x_min) / par->resolution;
	par->height = (par->y_max - par->y_min) / par->resolution;
	par->area = par->width * par->height;

	if (is_julia) {
		result = fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
		DIE(result != 2,
			"[ERROR] Could not read Julia complex number "
			"from file '%s'\n", in_filename);
	}

	fclose(file);
}

// write result to output file
void write_output_file(char *out_filename, int **result,
						int width, int height) {
	FILE *file = fopen(out_filename, "w");
	DIE(!file, "[ERROR] Could not open file '%s' for writing\n", out_filename);

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

// alloc a matrix
int **allocate_memory(int width, int height) {
	int **result = malloc(height * sizeof(int*));
	DIE(!result, "[ERROR] Could not malloc.\n"
		"\tWith height: %d\n", height);

	for (int i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		DIE(!result[i], "[ERROR] Could not malloc. "
			"\tWith width: %d, at index: %d\n", width, i);
	}

	return result;
}

// free a matrix
void free_memory(int **result, int height) {
	for (int i = 0; i < height; i++)
		free(result[i]);

	free(result);
}