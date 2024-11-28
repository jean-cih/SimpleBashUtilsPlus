#include "s21_grep.h"

int main(int argc, char **argv) {
  if (find_help(argc, argv)) {
    flag_help();
  } else {
    CmdLineOptions options = {NULL, NULL, 0, NULL, 0};
    options.flags = (char **)calloc(128, sizeof(char *));
    options.filenames = (char **)calloc(128, sizeof(char *));
    int exist_e_f = 0;
    parse_cmdline(argc, argv, &options, &exist_e_f);

    if (options.filename_count != 0) {
      char **new_argv = (char **)calloc(128, sizeof(char *));
      for (int i = 0; i < argc; i++) {
        new_argv[i] = (char *)calloc(128, sizeof(char));
      }

      reinvent_argv(new_argv, argv, &options, &exist_e_f);

      Flags flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, 0};
      if (read_flags(argc, new_argv, &flags) == 0) {
        validation(argc, new_argv, &flags);
      }

      for (int i = 0; i < argc; i++) {
        free(new_argv[i]);
      }
      free(new_argv);
    }
    free(options.flags);
    free(options.filenames);
  }

  return 0;
}

void reinvent_argv(char **new_argv, char **argv, CmdLineOptions *options,
                   int *exist_e_f) {
  strcat(new_argv[0], argv[0]);
  for (int i = 0; i < options->flag_count; i++) {
    strcat(new_argv[i + 1], options->flags[i]);
  }
  if (*exist_e_f == 0) {
    strcat(new_argv[options->flag_count + 1], options->pattern);
  }
  for (int i = 0; i < options->filename_count; i++) {
    strcat(new_argv[i + options->flag_count + 2 - *exist_e_f],
           options->filenames[i]);
  }
}

int parse_cmdline(int argc, char **argv, CmdLineOptions *options,
                  int *exist_e_f) {
  int pattern = 0;
  char *pos_e = NULL;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      options->flag_count++;
      if ((pos_e = strpbrk(argv[i], "ef")) && pos_e[1] == '\0') {
        options->flags[options->flag_count - 1] = argv[i];
        options->flag_count++;
        i++;
        options->flags[options->flag_count - 1] = argv[i];
        *exist_e_f = 1;
      } else {
        options->flags[options->flag_count - 1] = argv[i];
      }
    } else {
      if (pattern == 0) {
        options->pattern = argv[i];
        pattern = 1;
      } else {
        options->filename_count++;
        options->filenames[options->filename_count - 1] = argv[i];
      }
    }
  }

  if (*exist_e_f == 1) {
    options->filename_count++;

    for (int i = options->filename_count - 1; i > 0; i--) {
      options->filenames[i] = options->filenames[i - 1];
    }
    options->filenames[0] = options->pattern;
    options->pattern = "";
  }

  return 0;
}

int read_flags(int argc, char **argv, Flags *flags) {
  int currentFlag = 0, flag = 0, error = 0;
  flags->patterns = (char *)calloc(1048, sizeof(char));
  while ((currentFlag = getopt(argc, argv, "e:ivclnhsf:orRE")) != -1) {
    switch (currentFlag) {
      case 'v':
        flags->flag_v = 1;
        break;
      case 'c':
        flags->flag_c = 1;
        break;
      case 'l':
        flags->flag_l = 1;
        break;
      case 'n':
        flags->flag_n = 1;
        break;
      case 'h':
        flags->flag_h = 1;
        break;
      case 's':
        flags->flag_s = 1;
        break;
      case 'f':
        flags->flag_f = 1;
        flags->regex_key |= REG_EXTENDED;
        error += pattern_cat_f(flags, optarg, &flag);
        break;
      case 'o':
        flags->flag_o = 1;
        break;
      case 'r':
        flags->flag_r = 1;
        break;
      case 'R':
        flags->flag_R = 1;
        break;
      case 'e':
        flags->flag_e = 1;
        flags->regex_key |= REG_EXTENDED;
        pattern_cat_e(flags, optarg, &flag);
        break;
      case 'E':
        flags->flag_E = 1;
        flags->regex_key |= REG_EXTENDED;
        break;
      case 'i':
        flags->regex_key |= REG_ICASE;
        break;
      default:
        flags->invalid_flag += 1;
        break;
    }
  }
  files_count(flags, argc);

  return error;
}

void files_count(Flags *flags, int argc) {
  if (optind < argc && !flags->flag_e && !flags->flag_f) {
    optind++;
  }
  while (optind < argc) {
    flags->file_count++;
    optind++;
  }
}

void pattern_cat_e(Flags *flags, char *optarg, int *flag) {
  if (*flag != 0) {
    strcat(flags->patterns, "|");
  }
  strcat(flags->patterns, optarg);
  ++*flag;

  if (strstr(flags->patterns, "|.|") == 0 ||
      strstr(flags->patterns, "|.") == 0 ||
      strstr(flags->patterns, ".|") == 0) {
    flags->regex_key |= REG_NEWLINE;
  }
}

int pattern_cat_f(Flags *flags, char *optarg, int *flag) {
  int error = 0;
  FILE *file = fopen(optarg, "r");

  if (file) {
    char *line = NULL;
    size_t length = 0;
    size_t size_pattern = 1024;
    while (getline(&line, &length, file) > 0) {
      if (strlen(line) + strlen(flags->patterns) > size_pattern) {
        size_pattern *= 2;
        flags->patterns = (char *)realloc(flags->patterns, size_pattern);
      }
      if (*flag != 0) {
        strcat(flags->patterns, "|");
      }

      strncat(flags->patterns, line, strlen(line) - 1);
      ++*flag;
    }

    fclose(file);
    free(line);
  } else {
    error = 1;
    fprintf(stderr, "s21_grep: %s: No such file or directory", optarg);
  }

  return error;
}

void validation(int argc, char **argv, Flags *flags) {
  char **start = &argv[1];
  char **end = &argv[argc];

  int number_flags = 0;
  while (start != end && start[0][0] == '-') {
    number_flags++;
    ++start;
  }

  if (!flags->flag_e && !flags->flag_f) {
    strcat(flags->patterns, *start);
  }

  regex_t preg_storage;
  regex_t *preg = &preg_storage;

  options(start, end, argc, argv, preg, flags);

  if (flags->invalid_flag == 0) {
    regfree(preg);
  }
  free(flags->patterns);
}

void options(char **start, char **end, int argc, char **argv, regex_t *preg,
             Flags *flags) {
  if ((start == end && !flags->flag_s) || flags->invalid_flag != 0) {
    fprintf(stderr,
            "Usage: s21_grep [OPTION]... PATTERNS [FILE]...\nTry 's21_grep "
            "--help' for more information.\n");
  } else if (regcomp(preg, flags->patterns, flags->regex_key) &&
             !flags->flag_s) {
    fprintf(stderr, "s21_grep: invalid character range\n");
  } else if (flags->flag_r || flags->flag_R) {
    char dir_names[32][32];
    for (int i = argc - flags->file_count; i < argc; i++) {
      read_directory(argv[i], dir_names, *flags, preg);
    }
  } else {
    for (int i = argc - flags->file_count; i < argc; i++) {
      DIR *dir_check = opendir(argv[i]);
      if (dir_check && !flags->flag_s) {
        printf("s21_grep: %s: Is a directory\n", argv[i]);
      } else {
        check_dir(argv[i], flags, preg);
      }
    }
  }
}

void read_directory(char *start, char (*dir_names)[32], Flags flags,
                    regex_t *preg) {
  struct dirent *entry;

  DIR *dir = opendir(start);
  if (!dir && !flags.flag_s) {
    fprintf(stderr, "s21_grep: is not a directory\n");
  } else {
    for (int i = 0; (entry = readdir(dir)) != NULL; i++) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
        continue;
      }

      strcpy(dir_names[i], start);
      strcat(dir_names[i], entry->d_name);

      check_directories_file(entry, dir_names, flags, preg, i);
    }
    closedir(dir);
  }
}

void check_directories_file(struct dirent *entry, char (*dir_names)[32],
                            Flags flags, regex_t *preg, int i) {
  if (entry->d_type == DT_DIR) {
    strcat(dir_names[i], "/");
    read_directory(dir_names[i], dir_names, flags, preg);
  } else {
    if (is_binary(dir_names[i]) && !flags.flag_s) {
      printf("s21_grep: %s: binary file matches\n", dir_names[i]);
    } else {
      check_dir(dir_names[i], &flags, preg);
    }
  }
}

int is_binary(char *filename) {
  int code = 0;

  FILE *fp = fopen(filename, "rb");
  if (fp) {
    char buffer[2048];
    int num_nonprintable = 0;
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
      for (size_t i = 0; i < bytes_read; ++i) {
        if (buffer[i] < 32 || buffer[i] > 126) {
          num_nonprintable++;
        }
      }
    }

    fclose(fp);

    if (num_nonprintable > 100) {
      code = 1;
    }
  }

  return code;
}

void check_dir(char *dir_names, Flags *flags, regex_t *preg) {
  FILE *file = fopen(dir_names, "r");
  if (!file) {
    if (!flags->flag_s) {
      fprintf(stderr, "s21_grep: %s: No such file or directory\n", dir_names);
    }
  } else {
    read_file(file, *flags, preg, dir_names, flags->file_count);

    fclose(file);
  }
}

void read_file(FILE *file, Flags flags, regex_t *preg, char *filename,
               int number_files) {
  regmatch_t match;

  char *line = NULL;
  size_t length = 0;
  int number = 1, count = 0;
  while (getline(&line, &length, file) != -1) {
    if (flags.flag_v) {
      count += not_exist_pattern(*preg, line, match, flags, filename,
                                 number_files, number);
    } else {
      count += exist_pattern(*preg, line, match, flags, filename, number_files,
                             number);
    }
    number++;
  }

#ifdef __APPLE__
  flag_c_mac(flags, count, filename, number_files);
#elif __linux__
  flag_c_linux(flags, count, filename, number_files);
#endif

  if (flags.flag_l) {
    flag_l(filename, count);
  }

  free(line);
}

int not_exist_pattern(regex_t preg, char *line, regmatch_t match, Flags flags,
                      char *filename, int number_files, int number) {
  int count = 0;
  if (regexec(&preg, line, 1, &match, 0)) {
    count = 1;
#ifdef __APPLE__
    if (!flags.flag_c && !flags.flag_h && !flags.flag_l) {
      print_filename(filename, number_files);
    }
    if (flags.flag_n && !flags.flag_c && !flags.flag_l) {
      flag_n(number);
    }
    if (!flags.flag_c && !flags.flag_l) {
      printf("%s", line);
      if (strchr(line, '\n') == NULL) {
        printf("\n");
      }
    }
#elif __linux__
    if (!flags.flag_c && !flags.flag_h && !flags.flag_l && !flags.flag_o) {
      print_filename(filename, number_files);
    }
    if (flags.flag_n && !flags.flag_c && !flags.flag_l && !flags.flag_o) {
      flag_n(number);
    }
    if (!flags.flag_c && !flags.flag_l && !flags.flag_o) {
      printf("%s", line);
      if (strchr(line, '\n') == NULL) {
        printf("\n");
      }
    }
#endif
  }
  return count;
}
int exist_pattern(regex_t preg, char *line, regmatch_t match, Flags flags,
                  char *filename, int number_files, int number) {
  int count = 0;
  if (!regexec(&preg, line, 1, &match, 0)) {
    count = 1;
    if (!flags.flag_c && !flags.flag_h && !flags.flag_l) {
      print_filename(filename, number_files);
    }
    if (flags.flag_n && !flags.flag_c && !flags.flag_l) {
      flag_n(number);
    }
    if (flags.flag_o && !flags.flag_c && !flags.flag_l) {
      flag_o(match, line, &preg, filename, number_files, flags, number);
    } else if (flags.flag_h && !flags.flag_c && !flags.flag_l) {
      print_line(line, &preg, match);
    } else if (!flags.flag_c && !flags.flag_l) {
      print_line(line, &preg, match);
    }

    if (!flags.flag_c && !flags.flag_l && !flags.flag_o) {
      if (strchr(line, '\n') == NULL) {
        printf("\n");
      }
    }
  }

  return count;
}

void flag_l(char *filename, int overlap) {
  if (overlap > 0) {
#ifdef COLOR_MODE
    printf("\033[35m%s\033[34m\033[0m\n", filename);
#else
    printf("%s\n", filename);
#endif
  }
}

void flag_n(int number) {
#ifdef COLOR_MODE
  printf("\033[32m%d\033[34m:\033[0m", number);
#else
  printf("%d:", number);
#endif
}

void flag_o(regmatch_t match, char *line, regex_t *preg, char *filename,
            int number_files, Flags flags, int number) {
#ifdef COLOR_MODE
  printf("\033[31m%.*s\n", (int)(match.rm_eo - match.rm_so),
         line + match.rm_so);
#else
  printf("%.*s\n", (int)(match.rm_eo - match.rm_so), line + match.rm_so);
#endif

  char *left = line + match.rm_eo;

  while (!regexec(preg, left, 1, &match, 0)) {
    if (!flags.flag_h) {
      print_filename(filename, number_files);
    }
    if (flags.flag_n) {
      flag_n(number);
    }
#ifdef COLOR_MODE
    printf("\033[31m%.*s\n", (int)(match.rm_eo - match.rm_so),
           left + match.rm_so);
#else
    printf("%.*s\n", (int)(match.rm_eo - match.rm_so), left + match.rm_so);
#endif

    left += match.rm_eo;
  }

#ifdef COLOR_MODE
  printf("\033[0m");
#endif
}

void flag_c_linux(Flags flags, int count, char *filename, int number_files) {
  if (flags.flag_c && !flags.flag_l) {
    if (!flags.flag_h && (count != 0 || !flags.flag_l)) {
      print_filename(filename, number_files);
    }

    if (flags.flag_l) {
      if (count > 0) {
        count = 1;
      } else {
        count = 0;
      }
    } else {
      if (!flags.flag_l) {
        printf("%d\n", count);
      }
    }
  }
}

void flag_c_mac(Flags flags, int count, char *filename, int number_files) {
  if (flags.flag_c) {
    if (!flags.flag_h) {
      print_filename(filename, number_files);
    }

    if (flags.flag_l) {
      if (count > 0) {
        count = 1;
      } else {
        count = 0;
      }
    }

    printf("%d\n", count);
  }
}

void print_filename(char *filename, int number_files) {
  if (number_files > 1) {
#ifdef COLOR_MODE
    printf("\033[35m%s\033[34m:\033[0m", filename);
#else
    printf("%s:", filename);
#endif
  }
}

void print_line(char *line, regex_t *preg, regmatch_t match) {
  printf("%.*s", (int)match.rm_so, line);

#ifdef COLOR_MODE
  printf("\033[31m%.*s\033[0m", (int)(match.rm_eo - match.rm_so),
         line + match.rm_so);
#else
  printf("%.*s", (int)(match.rm_eo - match.rm_so), line + match.rm_so);
#endif

  char *left = line + match.rm_eo;
  while (!regexec(preg, left, 1, &match, 0) && match.rm_eo - match.rm_so != 0) {
    printf("%.*s", (int)match.rm_so, left);
#ifdef COLOR_MODE
    printf("\033[31m%.*s\033[0m", (int)(match.rm_eo - match.rm_so),
           left + match.rm_so);
#else
    printf("%.*s", (int)(match.rm_eo - match.rm_so), left + match.rm_so);
#endif

    left += match.rm_eo;
  }
  printf("%s", left);
}

int find_help(int argc, char **argv) {
  int option = 0;
  for (int i = 1; i < argc; i++) {
    if (strstr(argv[i], "--help")) {
      option = 1;
    }
  }
  return option;
}

void flag_help() {
  printf(
      "Usage: s21_grep [OPTION]... PATTERNS [FILE]...\n"
      "Search for PATTERNS in each FILE.\n"
      "Example: s21_grep -i 'hello world' menu.h main.c\n"
      "PATTERNS can contain multiple patterns separated by newlines.\n"
      "\n"
      "Pattern selection and interpretation:\n"
      "    -E                      PATTERNS are extended regular expressions\n"
      "    -e                      use PATTERNS for matching\n"
      "    -f                      take PATTERNS from FILE\n"
      "    -i                      ignore case distinctions in patterns and "
      "data\n"
      "\n"
      "Miscellaneous:\n"
      "    -s                      suppress error messages\n"
      "    -v                      select non-matching lines\n"
      "        --help              display this help text and exit\n"
      "\n"
      "Output control:\n"
      "    -n                      print line number with output lines\n"
      "    -h                      suppress the file name prefix on output\n"
      "    -o                      show only nonempty parts of lines that "
      "match\n"
      "    -l                      print only names of FILEs with selected "
      "lines\n"
      "    -c                      print only a count of selected lines per "
      "FILE\n"
      "    -r                      like -R\n"
      "    -R                      likewise, but follow all symlinks\n");
}
