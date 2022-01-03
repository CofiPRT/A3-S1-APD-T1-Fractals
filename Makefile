CC=gcc
CFLAGS=-lm -lpthread -Wall -Wextra -Werror
SOURCES=skel.c tema1_par.c
EXEC=tema1_par

IN_FILE_JULIA=tests/julia2.in
IN_FILE_MANDEL=tests/mandelbrot2.in

OUT_FILE_JULIA=out_julia
OUT_FILE_MANDEL=out_mandel

THREADS=4

build: 
	${CC} -o tema1 tema1.c -lm
	${CC} -o ${EXEC} ${SOURCES} ${CFLAGS}

run_diff: build
	@echo "Running seq.............................................................."
	./tema1 ${IN_FILE_JULIA} ${OUT_FILE_JULIA}_seq ${IN_FILE_MANDEL} ${OUT_FILE_MANDEL}_seq ${THREADS}
	@echo "Running par.............................................................."
	./${EXEC} ${IN_FILE_JULIA} ${OUT_FILE_JULIA}_par ${IN_FILE_MANDEL} ${OUT_FILE_MANDEL}_par ${THREADS}
	@echo "Comparing julia.............................................................."
	diff -q ${OUT_FILE_JULIA}_seq ${OUT_FILE_JULIA}_par
	@echo "Comparing mandelbrot.............................................................."
	diff -q ${OUT_FILE_MANDEL}_seq ${OUT_FILE_MANDEL}_par

clean:
	rm -rf tema1 ${EXEC}
