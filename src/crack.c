#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define CHECK_ERR(expr, msg) if (expr) { fprintf(stderr, "%s\n", msg); return EXIT_FAILURE; }
#define ALPHABET "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!*~"

typedef struct passwd_st {
	char* hash;
	char* salt;
	char* seedchars;
	int threads_nb;
	int index;
	int jumps_nb;
	struct crypt_data* cdata;
} passwd_st;

passwd_st* init_passwd(char** argv) {
	int threads_nb = atoi(argv[3]);
	passwd_st* passwd = malloc(sizeof(passwd_st) * threads_nb);
	for (int i = 0; i < threads_nb; i++) {
		passwd[i].hash = argv[1];
		passwd[i].salt = argv[2];
		passwd[i].seedchars = ALPHABET;
		passwd[i].threads_nb = atoi(argv[3]);
		passwd[i].index = i;
		passwd[i].jumps_nb = (66-i) / threads_nb;
		passwd[i].cdata = malloc(sizeof(struct crypt_data));
		passwd[i].cdata->initialized = 0;
	}
	return passwd;
}

// Zone Orphée
char* func(passwd_st* passwd, int* iter) {
	char* temp = malloc(sizeof(char)*(*iter));
	char* hash;
	int pos = passwd->index;
	size_t found = 0;
	for (int i = 0; i < *iter; i++) {
		for (int j = 0; j < passwd->jumps_nb; j++) {
			temp[i] = passwd->seedchars[pos];
			hash = crypt_r(temp, passwd->salt, passwd->cdata);
			printf("%s\n",temp);
			if (hash == passwd->hash) {
				found = 1;
				break;
			}
			pos += passwd->threads_nb;
		}
	}
	if (!found) {
		return("pas trouvé!");
	} else {
		return temp;
	}
}


// Zone Ludo



// Zone Steven
void compute_steven(passwd_st* passwd, int nb_char_fixed, int nb_of_call) {
	char* fixed = malloc(sizeof(char) * nb_char_fixed);
	

	for (int i = 0; i < passwd->size; i++) {
		
	}
	free(fixed);
	compute_steven();
}


void* thread(void* arg) {
	passwd_st* passwd = (passwd_st*)arg;
	// test de la fonction crypt_r
	char* hash = crypt_r("Steven aime sucer, mais pas autant qu'Orphée", temp->salt, temp->cdata);
	printf("%s\n",hash);
/*	char* hash = crypt_r("Steven aime sucer", temp->salt, temp->cdata);*/
	int iter = 1;
	char word[iter];
	char* hash;
	int pos = passwd->index;
	size_t found = 0;
	for (int j = 0; j < passwd->jumps_nb; j++) {
		word[0] = passwd->seedchars[pos];
		hash = crypt_r(word, passwd->salt, passwd->cdata);
		if (!strcmp(hash,passwd->hash)) {
			found = 1;
		}
		pos += passwd->threads_nb;
	}
	if (found) {
		printf("trouvé!\n");
	} else {
		printf("pas trouvé!\n");
	}
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
