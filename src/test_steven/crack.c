#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#define CHECK_ERR(expr, msg) if (expr) { fprintf(stderr, "%s\n", msg); return EXIT_FAILURE; }
#define SEEDCHARS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!*~"

const int seedchars_size = 10;
const int passwd_size = 8;

struct timespec start, finish, finish2;

typedef struct passwd_st {
	char* hash;
	char* salt;
	char* seedchars;
	int threads_nb;
	int index;
	struct crypt_data* cdata;
} passwd_st;

passwd_st* init_passwd(char** argv) {
	int threads_nb = atoi(argv[3]);
	passwd_st* passwd = malloc(sizeof(passwd_st) * threads_nb);
	for (int i = 0; i < threads_nb; i++) {
		passwd[i].hash = argv[1];
		passwd[i].salt = argv[2];
		passwd[i].seedchars = SEEDCHARS;
		passwd[i].threads_nb = atoi(argv[3]);
		passwd[i].index = i;
		passwd[i].cdata = malloc(sizeof(struct crypt_data));
		passwd[i].cdata->initialized = 0;
	}
	return passwd;
}

void func(passwd_st* passwd, char* str, int index, int index_max) {
	int cnt = 0;
	if (index == index_max) {
		cnt = passwd->index;
	}
	while (cnt < seedchars_size) {
		if (index == index_max) {
			str[index] = passwd->seedchars[cnt];
			char* hash = crypt_r(str, passwd->salt, passwd->cdata);
			// printf("%s\n", str);
			cnt += passwd->threads_nb;
			if (!strcmp(hash,passwd->hash)) {
				clock_gettime(CLOCK_MONOTONIC, &finish);
				double elapsed = finish.tv_sec - start.tv_sec;
				elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
				FILE *f = fopen("found.txt", "w");
				fprintf(f, "Temps d'exécution pour trouver le mot de passe : %f\nMot de passe : %s\n", elapsed, str);
				fclose(f);
			}
		} else { // condition pour aller à la dernière lettre du mdp
			str[index] = passwd->seedchars[cnt];
			cnt++;
			func(passwd,str,index+1,index_max);
		}
	}
	// return "NULL";
}

void* thread(void* arg) {
	passwd_st* passwd = (passwd_st*)arg;
	// initialisation du tableau contenant le mdp à tester
	char* str = malloc(passwd_size + 1);
	for (int i = 0; i < passwd_size; i++) {
		// str[i] = passwd->seedchars[passwd->index];
		// on appelle la fonction autant de fois que la taille du mdp
		printf("%d\n", i);
		func(passwd,str,0,i);
	}
	// printf("Le mot de passe est : %s\n", str);
	free(str);
	return NULL;
}

int main(int argc, char** argv) {
	if (argc == 4) {
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

		clock_gettime(CLOCK_MONOTONIC, &finish2);
		double elapsed2 = finish2.tv_sec - start.tv_sec;
		elapsed2 += (finish2.tv_nsec - start.tv_nsec) / 1000000000.0;
		FILE *f = fopen("found.txt", "r+");
		fprintf(f, "Temps d'exécution total : %f\n", elapsed2);
		fclose(f);

		return EXIT_SUCCESS;
	} else {
		fprintf(stderr,"Invalid argument count !\n");
		return EXIT_FAILURE;
	}
}
