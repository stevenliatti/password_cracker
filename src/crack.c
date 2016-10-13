#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct passwd_st {
	char* hash;
	char* salt;
	char* seedchars;
	int threads_nb;
	struct crypt_data* cdata;
} passwd_st;

passwd_st* init_passwd(char** argv) {
	int threads_nb = atoi(argv[3]);
	passwd_st* passwd = malloc(sizeof(passwd_st)*threads_nb);
	for (int i = 0; i < threads_nb; i++) {
		passwd[i].hash = argv[1];
		passwd[i].salt = argv[2];
		passwd[i].seedchars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							"abcdefghijklmnopqrstuvwxyz!*~";
		passwd[i].threads_nb = atoi(argv[3]);
		passwd[i].cdata = malloc(sizeof(struct crypt_data));
		passwd[i].cdata->initialized = 0;
	}
	return passwd;
}

void* thread(void* arg) {
	passwd_st* temp = (passwd_st*)arg;
	// test de la fonction crypt_r
	char* hash = crypt_r("Steven aime sucer", temp->salt, temp->cdata);
	printf("%s\n",hash);
	return NULL;
}

int main(int argc, char** argv) {
	if (argc == 4) {
		int threads_nb = atoi(argv[3]);
		// initialisation des threads et des structures
		pthread_t t[threads_nb];
		passwd_st* passwd = init_passwd(argv);
		for (int i = 0; i < threads_nb; i++) {
			if (pthread_create(&t[i], NULL, thread, &passwd[i]) != 0) {
				fprintf(stderr, "pthread_create failed!\n");
				return EXIT_FAILURE;
			}
		}
		for (int i = 0; i < threads_nb; i++) {
			if (pthread_join(t[i], NULL) != 0) {
				fprintf(stderr, "pthread_join failed!\n");
				return EXIT_FAILURE;
			}
		}
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr,"Invalid argument count !\n");
		return EXIT_FAILURE;
	}
}
