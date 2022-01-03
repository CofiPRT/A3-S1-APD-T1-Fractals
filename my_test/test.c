#include <stdio.h>

int calc(int width, int height) {

	int w = height / width;
	int h = height % width;

	int a = 0;

	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			a += i * j;
		}
	}

	return a;

}

int main() {

	printf("%d\n", calc(5, 2));

}