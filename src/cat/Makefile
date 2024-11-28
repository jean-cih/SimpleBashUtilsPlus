FLAGS = -Wall -Werror -Wextra
VERSION = -std=c11
GCC = gcc $(FLAGS) $(VERSION)


all: clean s21_cat

s21_cat: s21_cat.o
	$(GCC) s21_cat.o -o s21_cat
	rm s21_cat.o

s21_cat.o: s21_cat.c
	$(GCC) -c s21_cat.c -o s21_cat.o

test: s21_cat
	bash tests/test.sh

test_leak: s21_cat
	bash tests/test_leak.sh

clang_check:
	clang-format -style=Google -n *.c

rebuild:
	make all

clean:
	rm -f s21_cat

