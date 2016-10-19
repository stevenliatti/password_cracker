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

void func(passwd_st* passwd, char* str, int index, int index_max, bool* found) {
	if (!*found) {
		int cnt = 0;
		if (index == index_max) {
			cnt = passwd->thread_id;
		}
		while (cnt < alphabet_size) {
			if (index == index_max) {
				str[index] = alphabet[cnt];
				char* hash = crypt_r(str, passwd->salt, passwd->cdata);
				cnt += passwd->threads_nb;
				if (strcmp(hash,passwd->hash) == 0) {
					printf("%s\n", str);
					*found = true;
				}
			}
			else { // condition pour aller à la dernière lettre du mdp
				str[index] = alphabet[cnt];
				cnt++;
				func(passwd, str, index + 1, index_max, found);
			}
		}
	}
}

void* thread(void* arg) {
	passwd_st* passwd = (passwd_st*) arg;
	// initialisation du tableau contenant le mdp à tester
	char* str = malloc(passwd_size + 1);
	static bool found = false;
	for (int i = 0; i < passwd_size; i++) {
		// on appelle la fonction autant de fois que la taille du mdp
		if (!found) {
			func(passwd, str, 0, i, &found);
		}
		else {
			break;
		}
	}
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
