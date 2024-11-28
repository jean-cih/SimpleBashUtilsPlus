#include "s21_cat.h"

int main(int argc, char **argv) {
  char flags[SIZE] = {'\0'};
  flags_parser(flags, argc, argv);
  for (int i = 1; i < argc; i++) {
    read_file(argv[i], flags);
  }
  if (argc == 1) {
    read_file("-", flags);
  }

  return 0;
}

void flags_parser(char *flags, int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    Flags cmd_flags[SIZE] = {{"-A", "vET"},     {"-b", "b"},
                             {"-e", "vE"},      {"-E", "E"},
                             {"-n", "n"},       {"-s", "s"},
                             {"-t", "vT"},      {"-T", "T"},
                             {"-v", "v"},       {"--number-nonblank", "b"},
                             {"--number", "n"}, {"--squeeze-blank", "s"}};

    for (int j = 0; j < SIZE; j++) {
      if (strcmp(argv[i], cmd_flags[j].key_flag) == 0) {
        if (strstr(flags, cmd_flags[j].indeed_flag) == NULL) {
          strcat(flags, cmd_flags[j].indeed_flag);
        }
      }
    }
  }
}

void read_file(char *name, char *flags) {
  FILE *file;

  if (strcmp("-", name) == 0 || strcmp("--", name) == 0) {
    file = stdin;
  } else {
    file = fopen(name, "r");
  }

  if (file != NULL) {
    int index = 0;
    char prev = '\n', prev_prev = '\n';
    char c = fgetc(file);

    while (c != EOF) {
      print_symb(c, &prev, &prev_prev, flags, &index);
      c = fgetc(file);
    }
    if (file != stdin) {
      fclose(file);
    }
  }
}

void print_symb(char c, char *prev, char *prev_prev, char *flags, int *index) {
  if (!(strchr(flags, 's') && *prev_prev == '\n' && *prev == '\n' &&
        c == '\n')) {
    if (((strchr(flags, 'n') != NULL && strchr(flags, 'b') == NULL) ||
         (strchr(flags, 'b') != NULL && c != '\n')) &&
        *prev == '\n') {
      *index += 1;
      printf("%6d\t", *index);
    }

    if ((strchr(flags, 'E') != NULL) && c == '\n') {
#ifdef __APPLE__
      if (strchr(flags, 'b') != NULL && *prev == '\n') {
        printf("      	$");
      } else {
        printf("$");
      }
#elif __linux__
      printf("$");
#endif
    }

    if ((strchr(flags, 'T') != NULL) && c == '\t') {
      printf("^");
      c = '\t' + 64;
    }

    flag_v(flags, &c);

    fputc(c, stdout);
  }
  *prev_prev = *prev;
  *prev = c;
}

void flag_v(char *flags, char *c) {
  if (strchr(flags, 'v') != NULL) {
    if (*c < 32 && *c != 9 && *c != 10) {
      *c += 64;
      printf("^");
    }
    if (*c == 127) {
      *c = '?';
      printf("^");
    }
  }
}
