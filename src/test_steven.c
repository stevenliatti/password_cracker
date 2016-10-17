#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	struct crypt_data *cdata = malloc(sizeof(struct crypt_data));
	cdata->initialized = 0;
	char *hash = crypt_r("12345678", "43", cdata);


	printf("%s\n", hash);


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