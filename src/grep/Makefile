FLAGS = -Wall -Werror -Wextra
VERSION = -std=c11
GCC = gcc $(FLAGS) $(VERSION)
COLOR = -DCOLOR_MODE


all: clean s21_grep

s21_grep: s21_grep.o
	$(GCC) s21_grep.o -o s21_grep
	rm s21_grep.o

s21_grep_color: s21_grep_color.o
	$(GCC) s21_grep_color.o -o s21_grep
	rm s21_grep_color.o

s21_grep.o: s21_grep.c
	$(GCC) -c s21_grep.c -o s21_grep.o

s21_grep_color.o: s21_grep.c
	$(GCC) $(COLOR) -c s21_grep.c -o s21_grep_color.o

test: s21_grep
	bash tests/test.sh

test_leak: s21_grep
	bash tests/test_leak.sh

clang_check:
	clang-format -style=Google -n *.c

rebuild:
	make clean
	make all

clean:
	rm -f s21_grep
