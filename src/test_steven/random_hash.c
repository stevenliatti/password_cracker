#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main() {
	const char* alphabet = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!*~";
	const int alphabet_size = strlen(alphabet);
	const int passwd_size_max = 5;
	struct crypt_data *cdata = malloc(sizeof(struct crypt_data));
	cdata->initialized = 0;
	struct timespec random;
	clock_gettime(CLOCK_MONOTONIC, &random);
	srand(random.tv_nsec);

	int size_pass = rand() % (passwd_size_max - 2) + 3;
	int nb_threads = rand() % 102 + 1;
	printf("%d\n", size_pass);

	char str[size_pass];
	for (int i = 0; i < size_pass; i++) {
		str[i] = alphabet[rand() % alphabet_size];
	}

	char *hash;
	hash = crypt_r(str, "43", cdata);
	printf("%s 43 %d", hash, nb_threads);
	
	return EXIT_SUCCESS;
}