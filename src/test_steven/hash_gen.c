#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	struct crypt_data *cdata = malloc(sizeof(struct crypt_data));
	cdata->initialized = 0;
	char *hash;

	for (int i = 1; i < argc; i++) {
		hash = crypt_r(argv[i], "43", cdata);
		printf("%s : %s\n", argv[i], hash);
	}
	
	return EXIT_SUCCESS;
}