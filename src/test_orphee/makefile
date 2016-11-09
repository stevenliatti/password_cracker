C = gcc
FLAGS = -g -Wall -Wextra -std=c99
SRCS=$(wildcard *.c)
OUT=$(SRCS:%.c=%)

all: $(OUT)

%: %.c
	$(CC) $(FLAGS) $< -lcrypt -lm -o $@ -lpthread

clean:
	rm -rf $(OUT) *~

rebuild: clean all

