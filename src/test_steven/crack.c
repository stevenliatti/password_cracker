#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define CHECK_ERR(expr, msg) if (expr) { fprintf(stderr, "%s\n", msg); return EXIT_FAILURE; }
// ce sera plus opti de le définir dans cet ordre, tester lettres minuscules et chiffres en premier
#define ALPHABET "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!*~"

typedef struct passwd_st {
	char* hash;
	char* salt;
	char* alphabet;
	int alphabet_size;
	int passwd_size;
	int threads_nb;
	int thread_id;
	struct crypt_data* cdata;
	struct timespec* time_start;
	struct timespec* time_finish;
} passwd_st;

passwd_st* init_passwd(char** argv, struct timespec* time_start, struct timespec* time_finish) {
	int threads_nb = atoi(argv[3]);
	passwd_st* passwd = malloc(sizeof(passwd_st) * threads_nb);
	passwd->time_start = time_start;
	passwd->time_finish = time_finish;
	for (int i = 0; i < threads_nb; i++) {
		passwd[i].hash = argv[1];
		passwd[i].salt = argv[2];
		passwd[i].alphabet = ALPHABET;
		passwd[i].alphabet_size = strlen(passwd[i].alphabet);
		passwd[i].passwd_size = 8;
		passwd[i].threads_nb = atoi(argv[3]);
		passwd[i].thread_id = i;
		passwd[i].cdata = malloc(sizeof(struct crypt_data));
		passwd[i].cdata->initialized = 0;
	}
	return passwd;
}

void func(passwd_st* passwd, char* str, int index, int index_max) {
	int cnt = 0;
	if (index == index_max) {
		cnt = passwd->thread_id;
	}
	while (cnt < passwd->alphabet_size) {
		if (index == index_max) {
			str[index] = passwd->alphabet[cnt];
			char* hash = crypt_r(str, passwd->salt, passwd->cdata);
			// printf("%s\n", str);
			cnt += passwd->threads_nb;
			if (strcmp(hash,passwd->hash) == 0) {
				clock_gettime(CLOCK_MONOTONIC, &passwd->time_finish);
				double elapsed = passwd->time_finish.tv_sec - start.tv_sec;
				elapsed += (passwd->time_finish.tv_nsec - start.tv_nsec) / 1000000000.0;
				printf("%f\n%s\n", elapsed, str);
			}
		}
		else { // condition pour aller à la dernière lettre du mdp
			str[index] = passwd->alphabet[cnt];
			cnt++;
			func(passwd, str, index + 1, index_max);
		}
	}
}

void* thread(void* arg) {
	passwd_st* passwd = (passwd_st*) arg;
	// initialisation du tableau contenant le mdp à tester
	char* str = malloc(passwd->passwd_size + 1);
	for (int i = 0; i < passwd->passwd_size; i++) {
		// on appelle la fonction autant de fois que la taille du mdp
		printf("%d\n", i);
		func(passwd, str, 0, i);
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
		passwd_st* passwd = init_passwd(argv, &finish);
		for (int i = 0; i < threads_nb; i++) {
			CHECK_ERR(pthread_create(&t[i], NULL, thread, &passwd[i]), "pthread_create failed!");
		}
		for (int i = 0; i < threads_nb; i++) {
			CHECK_ERR(pthread_join(t[i], NULL), "pthread_join failed!");
		}
		return EXIT_SUCCESS;
	}
	else {
		fprintf(stderr, "Invalid argument count !\n");
		return EXIT_FAILURE;
	}
}
