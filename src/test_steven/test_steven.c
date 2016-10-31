#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

const int alphabet_size = 2;
const int passwd_size = 8;

long pass_len(long position) {
	int i = 1;
	bool check = false;
	long min = 0;
	long max = (long) (pow(alphabet_size, i));
	while (!check) {
		check = (min <= position) && (position < max);
		min += max;
		i++;
		max += (long) (pow(alphabet_size, i));
	}

	return i;
}

int main() {
	// printf("%d\n", (int) pow(2,4));
	for (long i = 0; i < 14; i++) {
		printf("%ld\n", pass_len(i));
	}
}