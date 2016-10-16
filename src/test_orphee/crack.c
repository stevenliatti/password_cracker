#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define CHECK_ERR(expr, msg) if (expr) { fprintf(stderr, "%s\n", msg); return EXIT_FAILURE; }
#define SEEDCHARS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!*~"

const int seedchars_size = 65;
const int passwd_size = 8;

typedef struct passwd_st {
	char* hash;
	char* salt;
	char* seedchars;
	int threads_nb;
	int index;
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
	}
	return passwd;
}

void func(passwd_st* passwd, char* str, int index, int index_max) {
	for (int i = passwd->index; i < seedchars_size; i+=passwd->threads_nb) {
		str[index] = passwd->seedchars[i];
		if (index == index_max) {
			printf("%s\n", str);
			// if (!strcmp(str,passwd->hash)) {
			// 	return str;
			// }
		} else { // condition pour aller à la dernière lettre du mdp
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
		// on appelle la fonction autant de fois que la taille du mdp
		func(passwd,str,0,i);
	}
	// printf("Le mot de passe est : %s\n", str);
	free(str);
	return NULL;
}

int main(int argc, char** argv) {
	if (argc == 4) {
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
		return EXIT_SUCCESS;
	} else {
		fprintf(stderr,"Invalid argument count !\n");
		return EXIT_FAILURE;
	}
}