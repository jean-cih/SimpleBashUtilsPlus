#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 12

typedef struct all_flags {
    char *key_flag;
    char *indeed_flag;
}Flags;

void flags_parser(char *flags, int argc, char **argv);
void read_file(char *name, char *flags);
void print_symb(char c, char *prev, char *prev_prev, char *flags, int *index);

void flag_v(char *flags, char *c);

#endif
