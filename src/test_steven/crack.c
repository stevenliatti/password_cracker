#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

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
	bool* found;
	long* tab_pow;
} passwd_st;

passwd_st* init_passwd(char** argv) {
	int threads_nb = atoi(argv[3]);
	passwd_st* passwd = malloc(sizeof(passwd_st) * threads_nb);
	bool* found = malloc(sizeof(bool));
	*found = false;

	long* tab_pow = malloc(sizeof(long) * (passwd_size + 1));

	for (int i = 0; i <= passwd_size; i++) {
		tab_pow[i] = (long) pow(alphabet_size, i);
	}

	for (int i = 0; i < threads_nb; i++) {
		passwd[i].hash = argv[1];
		passwd[i].salt = argv[2];
		passwd[i].threads_nb = atoi(argv[3]);
		passwd[i].thread_id = i;
		passwd[i].cdata = malloc(sizeof(struct crypt_data));
		passwd[i].cdata->initialized = 0;
		passwd[i].found = found;
		passwd[i].tab_pow = tab_pow;
	}
	return passwd;
}

void free_passwd(passwd_st* passwd) {
	for (int i = 0; i < passwd->threads_nb; i++) {
		free(passwd[i].cdata);
	}
	free(passwd->tab_pow);
	free(passwd->found);
	free(passwd);
}

/**
 * Cette fonction retourne la longueur d'un mot de passe
 * en fonction de sa position dans l'espace des possibilités
 * ou combinaisons de mots de passe (en fonction de l'alphabet).
 */
int pass_len(long position, long tab_pow[]) {
	int i = 1;
	long min = 0;
	long max = tab_pow[i];
	
	while (!(min <= position && position < max)) {
		min = max;
		i++;
		max += tab_pow[i];
	}

	return i;
}

/**
 * Cette fonction retourne une position précédente ou se trouve le
 * même pattern de mot de passe (ex: le mot de passe "abb" est constitué
 * de "bb" sur les bits de poids faible).
 */
long sub_index(int pass_len_actual, int pass_len_previous, long position, long tab_pow[]) {
	if (pass_len_actual == pass_len_previous) {
		return position;
	}
	else {
		long past_position = tab_pow[pass_len_actual - 1];
		long new_position = position - past_position;
		while (pass_len(new_position, tab_pow) == pass_len_actual) {
			new_position = new_position - past_position;
		}
		return sub_index(pass_len_actual - 1, pass_len_previous, new_position, tab_pow);
	}
}

void gen_str(char* str, long position, int pass_len_start, long tab_pow[]) {
	int pass_len_actual = pass_len(position, tab_pow);
	if (pass_len_actual == 1)
		str[pass_len_start - 1] = alphabet[position];
	else {
		long sub_position = sub_index(pass_len_actual, pass_len_actual - 1, position, tab_pow);
		int index = 0;
		long new_position = position;
		int pass_len_past = pass_len_actual - 1;

		if (pass_len_actual != pass_len_start)
			index = pass_len_start - pass_len_actual;

		for (int i = pass_len_past; i > 0; i--)
			new_position = new_position - tab_pow[i];

		str[index] = alphabet[new_position / tab_pow[pass_len_past]];
		gen_str(str, sub_position, pass_len_start, tab_pow);
	}
}

void* thread(void* arg) {
	passwd_st* passwd = (passwd_st*) arg;
	char* str = malloc(sizeof(char) * (passwd_size + 1));
	long position = passwd->thread_id;

	while (!*(passwd->found)) {
		gen_str(str, position, pass_len(position, passwd->tab_pow), passwd->tab_pow);
		position += passwd->threads_nb;

		if (strcmp(crypt_r(str, passwd->salt, passwd->cdata), passwd->hash) == 0) {
			printf("%s\n", str);
			*(passwd->found) = true;
		}
	}
	free(str);

	return passwd->found;
}

int main(int argc, char** argv) {
	if (argc == 4) {
		struct timespec start, finish;
		clock_gettime(CLOCK_MONOTONIC, &start);
		int threads_nb = atoi(argv[3]);
		pthread_t t[threads_nb];
		bool* found;
		passwd_st* passwd = init_passwd(argv);

		for (int i = 0; i < threads_nb; i++) {
			CHECK_ERR(pthread_create(&t[i], NULL, thread, &passwd[i]), "pthread_create failed!");
		}
		for (int i = 0; i < threads_nb; i++) {
			CHECK_ERR(pthread_join(t[i], (void**)&found), "pthread_join failed!");
		}
		if (!*found) {
			printf("No match found\n");
		}

		free_passwd(passwd);

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
