#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define CHECK_ERR(expr, msg) if (expr) { fprintf(stderr, "%s\n", msg); return EXIT_FAILURE; }

const char* alphabet = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!*~";
const int alphabet_size = 65;
const int passwd_size = 8;

typedef struct passwd_st {
	char* hash;
	char* salt;
	int threads_nb;
	int thread_id;
	struct crypt_data* cdata;
} passwd_st;

passwd_st* init_passwd(char** argv) {
	int threads_nb = atoi(argv[3]);
	passwd_st* passwd = malloc(sizeof(passwd_st) * threads_nb);
	for (int i = 0; i < threads_nb; i++) {
		passwd[i].hash = argv[1];
		passwd[i].salt = argv[2];
		passwd[i].threads_nb = atoi(argv[3]);
		passwd[i].thread_id = i;
		passwd[i].cdata = malloc(sizeof(struct crypt_data));
		passwd[i].cdata->initialized = 0;
	}
	return passwd;
}

char* gen_str(long position) {
	char* str;
	
}

void* thread(void* arg) {
	passwd_st* passwd = (passwd_st*) arg;
	char* str = malloc(passwd_size + 1);
	static bool found = false;
	long position = passwd->thread_id;

	do {
		str = gen_str(position);
		position += passwd->threads_nb;
	} while (strcmp(crypt_r(str, passwd->salt, passwd->cdata), passwd->hash) != 0);
	free(str);

	return &found;
}

int main(int argc, char** argv) {
	if (argc == 4) {
		struct timespec start, finish;
		clock_gettime(CLOCK_MONOTONIC, &start);
		int threads_nb = atoi(argv[3]);
		// initialisation des threads et des structures
		pthread_t t[threads_nb];
		passwd_st* passwd = init_passwd(argv);
		bool* found;
		for (int i = 0; i < threads_nb; i++) {
			CHECK_ERR(pthread_create(&t[i], NULL, thread, &passwd[i]), "pthread_create failed!");
		}
		for (int i = 0; i < threads_nb; i++) {
			CHECK_ERR(pthread_join(t[i], (void**)&found), "pthread_join failed!");
		}
		if (!*found) {
			printf("No match found\n");
		}
		clock_gettime(CLOCK_MONOTONIC, &finish);
		double elapsed = finish.tv_sec - start.tv_sec;
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		printf("%f\n", elapsed);
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "Invalid argument count !\n");
		return EXIT_FAILURE;
	}
}
