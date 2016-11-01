#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

const int alphabet_size = 2;

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

long sub_str(int pass_len_actual, int pass_len_choose, long position) {
	if (pass_len_actual == pass_len_choose) {
		return position;
	}
	else {
		long past_position = (long) pow(alphabet_size, pass_len_actual - 1);
		long new_position = position - past_position;
		while (pass_len(new_position) == pass_len_actual) {
			new_position = new_position - past_position;
		}
		return sub_str(pass_len_actual - 1, pass_len_choose, new_position);
	}
}

int main() {
	for (int i = 30; i >= 0; i--) {
		int pass_len_actual = pass_len(i);
		int pass_len_choose = pass_len_actual - 1;
		long position = i;
		printf("De %d on va à %ld\n", i, sub_str(pass_len_actual, pass_len_choose, position));
	}
}
