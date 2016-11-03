/**
* @file crack.c
* @brief A password cracker
*
* This file contains a brute force password
* cracker multithreaded. The program need a
* password hash with him salt and a number of
* threads for the execution. He test all the 
* combinations from the alphabet declared and
* the password's max size. 
*
* @author Steven Liatti
* @author Orphée Antoniadis
* @author Ludovic Gindre
* @bug No known bugs.
* @date November 3, 2016
* @version 1.0
*/

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
	long* array_pow;
} passwd_st;

/**
 * This function initialize our array of passwd_st.
 *
 * @param argv array of arguments, each argument is a string
 * @return passwd pointer on passwd_st
 */
passwd_st* init_passwd(char** argv) {
	int threads_nb = atoi(argv[3]);
	passwd_st* passwd = malloc(sizeof(passwd_st) * threads_nb);
	bool* found = malloc(sizeof(bool));
	*found = false;

	long* array_pow = malloc(sizeof(long) * (passwd_size + 1));

	for (int i = 0; i <= passwd_size; i++) {
		array_pow[i] = (long) pow(alphabet_size, i);
	}

	for (int i = 0; i < threads_nb; i++) {
		passwd[i].hash = argv[1];
		passwd[i].salt = argv[2];
		passwd[i].threads_nb = atoi(argv[3]);
		passwd[i].thread_id = i;
		passwd[i].cdata = malloc(sizeof(struct crypt_data));
		passwd[i].cdata->initialized = 0;
		passwd[i].found = found;
		passwd[i].array_pow = array_pow;
	}
	return passwd;
}

/**
 * This function free our array of passwd_st.
 *
 * @param passwd array of passwd_st
 * @return void
 */
void free_passwd(passwd_st* passwd) {
	for (int i = 0; i < passwd->threads_nb; i++) {
		free(passwd[i].cdata);
	}
	free(passwd->array_pow);
	free(passwd->found);
	free(passwd);
}

/**
 * This function returns the length of a password
 * at the position in the area of possibilities.
 *
 * @param position the position of the string in the space of possibilities
 * @param array_pow array of long, it contains all the powers of alphabet_size (from 1 to passwd_size)
 * @return the length of the password at the position in argument
 */
int pass_len(long position, long* array_pow) {
	int len = 1;
	long min = 0;
	long max = array_pow[len];
	
	while (!(min <= position && position < max)) {
		min = max;
		len++;
		max += array_pow[len];
	}

	return len;
}

/**
 * This function return a previous position that include the same password pattern
 * (example: the password "abb" has "bb" in the least significant digits). To navigate in
 * the area of possibilities, the function jump from power of the alphabet to another
 * power of the alphabet, it use the pass_len function.
 *
 * @param pass_len_actual the actual length of the password
 * @param pass_len_previous the previous length of the password, generaly pass_len_actual - 1
 * @param position the position of the string in the space of possibilities
 * @param array_pow array of long, it contains all the powers of alphabet_size (from 1 to passwd_size)
 * @return the previous positon in the area of possibilities which contains the pattern that actual position need
 */
long sub_index(int pass_len_actual, int pass_len_previous, long position, long* array_pow) {
	if (pass_len_actual == pass_len_previous) {
		return position;
	}
	else {
		long past_position = array_pow[pass_len_actual - 1];
		long new_position = position - past_position;
		while (pass_len(new_position, array_pow) == pass_len_actual) {
			new_position = new_position - past_position;
		}
		return sub_index(pass_len_actual - 1, pass_len_previous, new_position, array_pow);
	}
}

/**
 * This function generate a password from the position in the area of possibilities
 * and a password length and store it in the str argument. It generate the most 
 * significant digit at the beginning of the recursion. At the nexts recursions, it 
 * generate the nexts digits until the last, where the recursion terminate.
 *
 * @param str the array of char where is store the password to test
 * @param position the position of the string in the space of possibilities
 * @param pass_len_start the length of the password at the beginning of the search
 * @param array_pow array of long, it contains all the powers of alphabet_size (from 1 to passwd_size)
 * @return void
 */
void gen_passwd(char* str, long position, int pass_len_start, long* array_pow) {
	int pass_len_actual = pass_len(position, array_pow);
	if (pass_len_actual == 1)
		str[pass_len_start - 1] = alphabet[position];
	else {
		long sub_position = sub_index(pass_len_actual, pass_len_actual - 1, position, array_pow);
		int index = 0;
		long new_position = position;
		int pass_len_past = pass_len_actual - 1;

		if (pass_len_actual != pass_len_start)
			index = pass_len_start - pass_len_actual;

		for (int i = pass_len_past; i > 0; i--)
			new_position = new_position - array_pow[i];

		str[index] = alphabet[new_position / array_pow[pass_len_past]];
		gen_passwd(str, sub_position, pass_len_start, array_pow);
	}
}

/**
 * This function is called at the creation of a thread. It generate the
 * sequence of positions in function of the thread ID and the number of
 * threads. With those positions, each thread generate their strings to
 * hash and test if it equals to the hash in argument of the program.
 *
 * If a thread found the password, it print it in the standard output
 * and cancel the other threads with a boolean mecanism.
 *
 * @param arg a void pointer, in reality the array passwd_st needed
 * @return if the password is found or not
 */
void* thread(void* arg) {
	passwd_st* passwd = (passwd_st*) arg;
	char* str = malloc(sizeof(char) * (passwd_size + 1));
	long position = passwd->thread_id;

	while (!*(passwd->found)) {
		gen_passwd(str, position, pass_len(position, passwd->array_pow), passwd->array_pow);
		position += passwd->threads_nb;

		if (strcmp(crypt_r(str, passwd->salt, passwd->cdata), passwd->hash) == 0) {
			printf("%s\n", str);
			*(passwd->found) = true;
		}
	}
	free(str);

	return passwd->found;
}

/**
 * This is the main function. It initialize the variables, launch the threads,
 * print if the solution is not found, free the memory in use and calculate the 
 * execution time of the program.
 *
 * @param argc 
 * @param argv
 * @return the code's exit of program
 */
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
