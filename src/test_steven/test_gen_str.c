#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

// const char* alphabet = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!*~";
const char* alphabet = "abcd";
const int alphabet_size = 3;
const int passwd_size = 4;

/**
 * Cette fonction retourne la longueur d'un mot de passe
 * en fonction de sa position dans l'espace des possibilités
 * ou combinaisons de mots de passe (en fonction de l'alphabet).
 */
int pass_len(long position) {
	int i = 1;
	long min = 0;
	long max = (long) (pow(alphabet_size, i));
	
	while (!(min <= position && position < max)) {
		min = max;
		i++;
		max += (long) (pow(alphabet_size, i));
	}

	return i;
}

/**
 * Cette fonction retourne une position précédente ou se trouve le
 * même pattern de mot de passe (ex: le mot de passe "abb" est constitué
 * de "bb" sur les bits de poids faible).
 */
long sub_index(int pass_len_actual, int pass_len_previous, long position) {
	if (pass_len_actual == pass_len_previous) {
		return position;
	}
	else {
		long past_position = (long) pow(alphabet_size, pass_len_actual - 1);
		long new_position = position - past_position;
		while (pass_len(new_position) == pass_len_actual) {
			new_position = new_position - past_position;
		}
		return sub_index(pass_len_actual - 1, pass_len_previous, new_position);
	}
}

void gen_str(char* str, long position, int pass_len_start) {
	int pass_len_actual = pass_len(position);
	if (pass_len_actual == 1)
		str[pass_len_start - 1] = alphabet[position];
	else {
		long sub_position = sub_index(pass_len_actual, pass_len_actual - 1, position);
		int index = 0;
		long new_position = position;
		int pass_len_past = pass_len_actual - 1;

		if (pass_len_actual != pass_len_start)
			index = pass_len_start - pass_len_actual;

		for (int i = pass_len_past; i > 0; i--)
			new_position = new_position - (long) pow(alphabet_size, i);

		str[index] = alphabet[new_position / (long) pow(alphabet_size, pass_len_past)];
		gen_str(str, sub_position, pass_len_start);
	}
}

int main() {
	struct crypt_data* cdata;
	cdata = malloc(sizeof(struct crypt_data));
	cdata->initialized = 0;
	char* str = malloc(sizeof(char) * passwd_size + 1);
	for (int i = 0; i < 100; i++) {
		gen_str(str, i, pass_len(i));
		// printf("%s\n", crypt_r(str, "43", cdata));
		printf("%s\n", str);
	}
	free(str);
	free(cdata);
}
