#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

const char* alphabet = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!*~";
const int alphabet_size = 65;
const int passwd_size = 8;

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

long sub_index(int pass_len_actual, int pass_len_choose, long position) {
	if (pass_len_actual == pass_len_choose) {
		return position;
	}
	else {
		long past_position = (long) pow(alphabet_size, pass_len_actual - 1);
		long new_position = position - past_position;
		while (pass_len(new_position) == pass_len_actual) {
			new_position = new_position - past_position;
		}
		return sub_index(pass_len_actual - 1, pass_len_choose, new_position);
	}
}

void gen_str(char* str, long position, int pass_len_start) {
	int pass_len_actual = pass_len(position);
	if (pass_len_actual == 1) {
		str[pass_len_start - 1] = alphabet[position];
	}
	else {
		long sub_position = sub_index(pass_len_actual, pass_len_actual - 1, position);
		gen_str(str, sub_position, pass_len_start);
	}
}

int main() {
	struct crypt_data* cdata;
	cdata = malloc(sizeof(struct crypt_data));
	cdata->initialized = 0;
	char* str = malloc(sizeof(char) * passwd_size + 1);
	for (int i = 0; i < alphabet_size; i++) {
		gen_str(str, i, 1);
		// printf("%s\n", crypt_r(str, "43", cdata));
		printf("%s\n", str);
	}
	free(str);
}
