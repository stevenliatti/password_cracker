#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main() {
	struct crypt_data *cdata = malloc(sizeof(struct crypt_data));
	cdata->initialized = 0;
	struct timespec random;
	clock_gettime(CLOCK_MONOTONIC, &random);
	srand(random.tv_nsec);

	int nb_threads = rand() % 500 + 1;

	char str[] = "43p0Ej8w7jMqs";

	char *hash;
	hash = crypt_r(str, "43", cdata);
	printf("%s 43 %d", hash, nb_threads);
	
	return EXIT_SUCCESS;
}
