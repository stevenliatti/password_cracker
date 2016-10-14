#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	for (int i = 0; i < 100; i += 4) {
		printf("%d  ", i);
	}
	printf("\n");

	for (int i = 1; i < 100; i += 4) {
		printf("%d  ", i);
	}
	printf("\n");

	for (int i = 2; i < 100; i += 4) {
		printf("%d  ", i);
	}
	printf("\n");

	for (int i = 3; i < 100; i += 4) {
		printf("%d  ", i);
	}
	printf("\n");
}