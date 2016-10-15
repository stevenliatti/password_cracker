#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define CHECK_ERR(expr, msg) if (expr) { fprintf(stderr, "%s\n", msg); return EXIT_FAILURE; }
#define SEEDCHARS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!*~"
#define SEEDCHARS_SIZE 65

typedef struct passwd_st {
	char* hash;
	char* salt;
	char* seedchars;
	int threads_nb;
	int index;
	int jumps_nb;
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
		passwd[i].jumps_nb = (SEEDCHARS_SIZE-(i-1)) / threads_nb;
	}
	return passwd;
}

char* count_base_65(passwd_st* passwd, char* char_tab, int* int_tab, int pos) {
	switch(pos) {
		case 1 :
			if (int_tab[pos] + passwd->threads_nb < SEEDCHARS_SIZE) {
				int_tab[pos] += passwd->threads_nb;
				char_tab[pos] = passwd->seedchars[int_tab[pos]];
				return(char_tab);
			} else {
				int_tab[pos] = passwd->index;
				char_tab[pos] = passwd->seedchars[int_tab[pos]];
				return(count_base_65(passwd,char_tab,int_tab,pos+1));
			}
			break;
		default :
			if (int_tab[pos]+1 < SEEDCHARS_SIZE) {
				int_tab[pos]++;
				char_tab[pos] = passwd->seedchars[int_tab[pos]];
				return(char_tab);
			} else {
				int_tab[pos] = passwd->index;
				char_tab[pos] = passwd->seedchars[int_tab[pos]];
				return(count_base_65(passwd,char_tab,int_tab,pos+1));
			}
			break;
	}
}

size_t tab_full(passwd_st* passwd, int* tab, int tab_size) {
	if (tab_size > 1) {
		for (int i = 0; i < tab_size; i++) {
			if (tab[i] != SEEDCHARS_SIZE-1) {
				return 0;
			}
		}
		if (tab[1] + passwd->threads_nb < SEEDCHARS_SIZE) {
				return 0;
		}
	}
	if (tab[0] == SEEDCHARS_SIZE-1) {
		return 1;
	}
}

char* func(passwd_st* passwd, char* char_tab, int* int_tab, int tab_size) {
	size_t found = 0;
	if (tab_size > 1) {
		char_tab = count_base_65(passwd,char_tab,int_tab,1);
	}
	// variation de la la dernière lettre
	for (int i = 0; i < SEEDCHARS_SIZE; i++) {
		int_tab[0] = i;
		char_tab[0] = passwd->seedchars[i];
		printf("%s\n",char_tab);
		if (!strcmp(char_tab,passwd->hash)) {
			found = 1;
			break;
		}
	}
	// cas où il faut passer aux mots avec une lettre de plus
	if (tab_full(passwd,int_tab,tab_size)) {
		// free(char_tab);
		// free(int_tab);
		char* temp1 = malloc(sizeof(char)*(tab_size+1));
		int* temp2 = malloc(sizeof(int)*(tab_size+1));
		for (int i = 0; i < tab_size+1; i++) {
			temp1[i] = passwd->seedchars[0];
			temp2[i] = 0;
		}
		temp2[1] = passwd->index-passwd->threads_nb;
		printf("%s\n",temp1);
		return func(passwd,temp1,temp2,tab_size+1);
	}
	if (found == 1) {
		return "Mot de passe trouvé !";
	} else {
		return func(passwd,char_tab,int_tab,tab_size);
	}
	return "Le mot de passe n'existe pas :(";
}

void* thread(void* arg) {
	passwd_st* passwd = (passwd_st*)arg;
	// initialisation du tableau contenant le code à tester
	char* char_tab = malloc(sizeof(char));
	int* int_tab = malloc(sizeof(int));
	printf("%s\n",func(passwd, char_tab, int_tab,1));
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
