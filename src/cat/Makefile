CC = gcc -std=c11
CFLAGS = -c -Wall -Wextra -Werror

s21_cat : s21_cat.o
	$(CC) s21_cat.o s21_cat.h -o s21_cat

s21_cat.o : s21_cat.c s21_cat.h
	$(CC) $(CFLAGS) s21_cat.c

clean:
	rm -rf *.o
