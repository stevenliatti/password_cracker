#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>

int main() {
	struct crypt_data *cdata = malloc(sizeof(struct crypt_data));
	cdata->initialized = 0;
	char *hash;
	char *str = NULL;
	FILE *f = fopen("hash.txt", "w");

	for (int i = 1; i < 10000; i++) {
		sprintf(str, "%d", i);
		hash = crypt_r(str, "43", cdata);
		fprintf(f, "%s\n", hash);
	}

	fclose(f);

	return EXIT_SUCCESS;
}
