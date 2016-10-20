#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define CHECK_ERR(expr, msg) if (expr) { fprintf(stderr, "%s\n", msg); return EXIT_FAILURE; }
// ce sera plus opti de le définir dans cet ordre, tester lettres minuscules et chiffres en premier
#define ALPHABET "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!*~"
#define PASSWORD_SIZE 8

typedef struct passwd_st {
	char* hash;
	char* salt;
	char* alphabet;
	int alphabet_size;
	int passwd_size;
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
		passwd[i].alphabet = ALPHABET;
		passwd[i].alphabet_size = strlen(passwd[i].alphabet);
		passwd[i].passwd_size = PASSWORD_SIZE;
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
		while (cnt < passwd->alphabet_size) {
			if (index == index_max) {
				str[index] = passwd->alphabet[cnt];
				char* hash = crypt_r(str, passwd->salt, passwd->cdata);
				cnt += passwd->threads_nb;
				if (strcmp(hash,passwd->hash) == 0) {
					printf("%s ", str);
					*found = true;
				}
			}
			else { // condition pour aller à la dernière lettre du mdp
				str[index] = passwd->alphabet[cnt];
				cnt++;
				func(passwd, str, index + 1, index_max, found);
			}
		}
	}
}

void* thread(void* arg) {
	passwd_st* passwd = (passwd_st*) arg;
	// initialisation du tableau contenant le mdp à tester
	char* str = malloc(passwd->passwd_size + 1);
	static bool found = false;
	for (int i = 0; i < passwd->passwd_size; i++) {
		// on appelle la fonction autant de fois que la taille du mdp
		if (!found)
			func(passwd, str, 0, i, &found);
		else
			break;
	}
	free(str);
	return NULL;
}

int main(int argc, char** argv) {
	if (argc == 4) {
		struct timespec start, finish;
		clock_gettime(CLOCK_MONOTONIC, &start);
		int threads_nb = atoi(argv[3]);
		// initialisation des threads et des structures
		pthread_t t[threads_nb];
		passwd_st* passwd = init_passwd(argv);
		for (int i = 0; i < threads_nb; i++) {
			CHECK_ERR(pthread_create(&t[i], NULL, thread, &passwd[i]), "pthread_create failed!");
		}
		for (int i = 0; i < threads_nb; i++) {
			CHECK_ERR(pthread_join(t[i], NULL), "pthread_join failed!");
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
