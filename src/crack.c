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

char* init_seedchars() {
	char* seedchars = malloc(sizeof(char)*65);
	for (int i = 0; i < 10; i++) {
		seedchars[i] = i + '0';
	}
	for (int i = 0; i < 26; i++) {
		seedchars[i+10] = i + 'A';
		seedchars[i+36] = i + 'a';
	}
	seedchars[62] = '!';
	seedchars[63] = '*';
	seedchars[64] = '~';
	return seedchars;
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
		// initialisation des variables
		passwd_st* passwd = malloc(sizeof(passwd_st));
		passwd->hash = argv[1];
		passwd->salt = argv[2];
		passwd->seedchars = init_seedchars();
		passwd->threads_nb = atoi(argv[3]);
		passwd->cdata = (struct crypt_data *)malloc(sizeof(struct crypt_data));
		passwd->cdata->initialized = 0;

		// initialisation des threads
		pthread_t t[passwd->threads_nb];
		for (int i = 0; i < passwd->threads_nb; i++) {
			if (pthread_create(&t[i], NULL, thread, passwd) != 0) {
				fprintf(stderr, "pthread_create failed!\n");
				return EXIT_FAILURE;
			}
		}
		for (int i = 0; i < passwd->threads_nb; i++) {
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
