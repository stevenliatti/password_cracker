#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int main(int argc, char** argv) {
	struct crypt_data* cdata = (struct crypt_data *)
	malloc(sizeof(struct crypt_data));
	cdata->initialized = 0;
	char* hash = crypt_r("Steven aime sucer", "43", cdata);
	return EXIT_SUCCESS;
}
