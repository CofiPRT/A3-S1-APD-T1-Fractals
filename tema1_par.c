#include "skel.h"
#include "utils.h"

pthread_barrier_t barrier;

params par_julia;
params par_mandel;
int **result_julia;
int **result_mandel;

void run_julia(int thread_id) {

	// total number of points to compute
	int N = par_julia.area;

	int start_index = thread_id * N / P;
	int end_index = fmin((thread_id + 1) * N / P, N);

	// iterate through the matrix of points in a left-right, top-bottom fashion
	for (int index = start_index; index < end_index; index++) {

		int h, w;

		// h is index / width, and w is index % width
		__asm__ __volatile__ (
			"idivl %[divisor]"
			: "=a" (h), "=d" (w)
			: "d" (0), "a" (index), [divisor] "rm" (par_julia.width)
			: // no clobbed registers
		);

		int step = 0;
		complex z = { w * par_julia.resolution + par_julia.x_min,
						h * par_julia.resolution + par_julia.y_min };

		while (complex_norm(z) < 2 && step < par_julia.iterations) {
			complex z_aux = { z.a, z.b };

			z.a = z_aux.a * z_aux.a - z_aux.b * z_aux.b + par_julia.c_julia.a;
			z.b = 2 * z_aux.a * z_aux.b + par_julia.c_julia.b;

			step++;
		}

		result_julia[par_julia.height - h - 1][w] = step % 256;

	}

}

void run_mandelbrot(int thread_id) {

	// total number of points to compute
	int N = par_mandel.area;

	int start_index = thread_id * N / P;
	int end_index = fmin((thread_id + 1) * N / P, N);

	// iterate through the matrix of points in a left-right, top-bottom fashion
	for (int index = start_index; index < end_index; index++) {

		int h, w;

		// h = index / width, w = index % width
		__asm__ __volatile__ (
			"idivl %[divisor]"
			: "=a" (h), "=d" (w)
			: "d" (0), "a" (index), [divisor] "rm" (par_mandel.width)
			: // no clobbed registers
		);

		complex c = { w * par_mandel.resolution + par_mandel.x_min,
						h * par_mandel.resolution + par_mandel.y_min };
		complex z = { 0, 0 };
		int step = 0;

		while (complex_norm(z) < 2 && step < par_mandel.iterations) {
			complex z_aux = { z.a, z.b };

			z.a = z_aux.a * z_aux.a - z_aux.b * z_aux.b + c.a;
			z.b = 2 * z_aux.a * z_aux.b + c.b;

			step++;
		}

		result_mandel[par_mandel.height - h - 1][w] = step % 256;

	}

}

void *thread_func(void *arg) {
	int thread_id = *(int *)arg;

	// split the file parsing between the first 2 AVAILABLE threads
	if (thread_id == 0) {
		read_input_file(in_filename_julia, &par_julia);
		result_julia = allocate_memory(par_julia.width, par_julia.height);
	}

	// thread 0 will also do this if P < 2
	if (P < 2 || thread_id == 1) {
		read_input_file(in_filename_mandelbrot, &par_mandel);
		result_mandel = allocate_memory(par_mandel.width, par_mandel.height);
	}

	// wait for the inits to complete
	pthread_barrier_wait(&barrier);

	run_julia(thread_id);
	run_mandelbrot(thread_id);

	// wait for all the threads to finish calculating
	pthread_barrier_wait(&barrier);

	// split the file writing between the first 2 AVAILABLE threads
	if (thread_id == 0) {
		write_output_file(out_filename_julia, result_julia, 
							par_julia.width, par_julia.height);
		free_memory(result_julia, par_julia.height);
	}

	// thread 0 will also do this if P < 2
	if (P < 2 || thread_id == 1) {
		write_output_file(out_filename_mandelbrot, result_mandel, 
							par_mandel.width, par_mandel.height);
		free_memory(result_mandel, par_mandel.height);
	}

	// don't end the main thread
	if (thread_id != 0)
		pthread_exit(NULL);

	return NULL;

}

int main(int argc, char *argv[]) {

	// read command line arguments
	get_args(argc, argv);

	DIE(pthread_barrier_init(&barrier, NULL, P),
		"[ERROR] Could not init barrier for %d threads\n", P);

	pthread_t threads[P];
	int arguments[P];

	// start threads
	for (int i = 1; i < P; i++) {
		arguments[i] = i;
		DIE(pthread_create(&threads[i], NULL, thread_func, &arguments[i]),
			"[ERROR] Could not create thread %d\n", i);
	}

	// the main thread also works
	arguments[0] = 0;
	thread_func(&arguments[0]);

	void *status;

	// join threads
	for (int i = 1; i < P; i++) {
		DIE(pthread_join(threads[i], &status),
			"[ERROR] Could not join thread %d\n", i);
	}

	DIE(pthread_barrier_destroy(&barrier),
		"[ERROR] Could not destroy barrier\n");

	return 0;
}
