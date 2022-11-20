CC = gcc -std=c11
CFLAGS = -c -Wall -Wextra -Werror

s21_grep : s21_grep.o
	$(CC) s21_grep.o s21_grep.h -o s21_grep
	
s21_grep.o : s21_grep.c s21_grep.h
	$(CC) $(CFLAGS) s21_grep.c

clean:
	rm -rf *.o