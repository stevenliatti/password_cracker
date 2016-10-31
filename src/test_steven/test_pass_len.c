#define _GNU_SOURCE
#include <crypt.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

// const int alphabet_size = 2;

int pass_len(long position, int alphabet_size) {
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

int main() {
	for (int i = 0; i < 126; i++) {
		printf("%d - %d\n", i, pass_len(i, 2));
	}

	int nb = 2;
	assert(1 == pass_len(0, 2));
	assert(1 == pass_len(1, nb));
	assert(2 == pass_len(2, nb));
	assert(2 == pass_len(5, nb));
	assert(3 == pass_len(6, nb));
	assert(3 == pass_len(13, nb));
	assert(4 == pass_len(14, nb));
	assert(4 == pass_len(29, nb));
	assert(5 == pass_len(30, nb));
	assert(5 == pass_len(61, nb));
	assert(6 == pass_len(62, nb));
	assert(6 == pass_len(125, nb));
	assert(7 == pass_len(126, nb));
	assert(7 == pass_len(253, nb));
	assert(8 == pass_len(254, nb));
	assert(8 == pass_len(509, nb));
	assert(9 == pass_len(510, nb));

	nb = 3;
	assert(1 == pass_len(0, nb));
	assert(1 == pass_len(2, nb));
	assert(2 == pass_len(3, nb));
	assert(2 == pass_len(11, nb));
	assert(3 == pass_len(12, nb));
	assert(3 == pass_len(38, nb));
	assert(4 == pass_len(39, nb));
	assert(4 == pass_len(119, nb));
	assert(5 == pass_len(120, nb));
	assert(5 == pass_len(362, nb));
	assert(6 == pass_len(363, nb));
	assert(6 == pass_len(1091, nb));
	assert(7 == pass_len(1092, nb));
	assert(7 == pass_len(3278, nb));
	assert(8 == pass_len(3279, nb));
	assert(8 == pass_len(9839, nb));
	assert(9 == pass_len(9840, nb));

	nb = 4;
	assert(1 == pass_len(0, nb));
	assert(1 == pass_len(3, nb));
	assert(2 == pass_len(4, nb));
	assert(2 == pass_len(19, nb));
	assert(3 == pass_len(20, nb));
	assert(3 == pass_len(83, nb));
	assert(4 == pass_len(84, nb));
	assert(4 == pass_len(339, nb));
	assert(5 == pass_len(340, nb));
	assert(5 == pass_len(1263, nb));
	assert(6 == pass_len(1364, nb));
	assert(6 == pass_len(5459, nb));
	assert(7 == pass_len(5460, nb));
	assert(7 == pass_len(21843, nb));
	assert(8 == pass_len(21844, nb));
	assert(8 == pass_len(87379, nb));
	assert(9 == pass_len(87380, nb));
}
