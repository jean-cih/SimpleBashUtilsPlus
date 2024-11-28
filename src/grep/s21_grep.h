#ifndef S21_GREP_H
#define S21_GREP_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

typedef struct {
    int flag_e;
    int flag_v;
    int flag_c;
    int flag_l;
    int flag_n;
    int flag_h;
    int flag_s;
    int flag_f;
    int flag_o;
    int flag_r;
    int flag_R;
    int flag_E;

    int regex_key;

    char *patterns;
    int file_count;
    int invalid_flag;
} Flags;

typedef struct {
    char *pattern;
    char **flags;
    int flag_count;
    char **filenames;
    int filename_count;
} CmdLineOptions;


void validation(int argc, char **argv, Flags *flags);
int read_flags(int argc, char **argv, Flags *flags);
void read_directory(char *start, char (*dir_names)[32], Flags flags, regex_t *preg);
void check_dir(char *dir_names, Flags *flags, regex_t *preg);
void read_file(FILE *file, Flags flags, regex_t *preg, char *filename, int number_files);
int is_binary(char *filename);
void print_filename(char *filename, int number_files);
void print_line(char *line, regex_t *preg, regmatch_t match);
void flag_help();
void files_count(Flags *flags, int argc);
void options(char **start, char **end, int argc, char **argv, regex_t *preg, Flags *flags);
void check_directories_file(struct dirent *entry, char (*dir_names)[32], Flags flags, regex_t *preg, int i);
int exist_pattern(regex_t preg, char *line, regmatch_t match, Flags flags, char *filename, int number_files, int number);
int not_exist_pattern(regex_t preg, char *line, regmatch_t match, Flags flags, char *filename, int number_files, int number);
int parse_cmdline(int argc, char **argv, CmdLineOptions *options, int *exist_e_f);
void reinvent_argv(char **new_argv, char **argv, CmdLineOptions *options, int *exist_e_f);


int pattern_cat_f(Flags *flags, char *optarg, int *flag);
void pattern_cat_e(Flags *flags, char *optarg, int *flag);
void flag_o(regmatch_t match, char *line, regex_t *preg, char *filename, int number_files, Flags flags, int number);
void flag_n(int number);
void flag_c_mac(Flags flags, int count, char *filename, int number_files);
void flag_c_linux(Flags flags, int count, char *filename, int number_files);
void flag_l(char *filename, int overlap);
int find_help(int argc, char **argv);


#endif
