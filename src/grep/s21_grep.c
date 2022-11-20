#include "s21_grep.h"

int main(int argc, char *argv[]) {
  reading_files(argc, argv);
  return 0;
}
void reading_files(int argc, char *argv[]) {
  int indexes[LEN] = {0};
  int *filenames = indexes;
  char search[2048] = {0};
  opt flag = {0};
  parsing(argc, argv, &flag, filenames, search);
  if (flag.error != 1) {
    for (flag.file_ind = 0; flag.file_ind < flag.number_of_files;
         flag.file_ind++) {
      char *file = argv[filenames[flag.file_ind]];
      FILE *f = fopen(file, "r");
      if (f != NULL) {
        processing(f, &flag, search, file);
        fclose(f);
      } else if (flag.s != 1) {
        fprintf(stderr, "grep: %s: No such file or directory\n",
                argv[filenames[flag.file_ind]]);
      }
    }
  }
}

void parsing(int argc, char *argv[], opt *flag, int *filenames, char *search) {
  if (argc > 1) {
    int option = 0;
    int option_index = 0;
    opterr = 0;
    char pattern_file[2048] = {0};
    const struct option long_options[] = {
        {0, 0, 0, 0},
    };
    while ((option = getopt_long(argc, argv, short_options, long_options,
                                 &option_index)) != -1) {
      switch (option) {
        case 'e':
          flag->e++;
          if (flag->e > 1) strcat(search, "|");
          strcat(search, optarg);
          break;
        case 'i':
          flag->i = 1;
          break;
        case 'v':
          flag->v = 1;
          break;
        case 'c':
          flag->c = 1;
          break;
        case 'l':
          flag->l = 1;
          break;
        case 'n':
          flag->n = 1;
          break;
        case 'h':
          flag->h = 1;
          break;
        case 's':
          flag->s = 1;
          break;
        case 'f':
          flag->f = 1;
          strcat(pattern_file, optarg);
          break;
        case 'o':
          flag->o = 1;
          break;
        case '?':
          flag->error = 1;
          fprintf(stderr,
                  "grep: invalid option -- '%c'\nUsage: grep [OPTION]... "
                  "PATTERNS [FILE]...\n",
                  option);
          break;
      }
    }
    if (flag->v == 1) {
      flag->o = 0;
    }
    if (flag->error == 0) {
      if (flag->e == 0 && flag->f == 0) {
        int length = strlen(argv[optind]);
        for (int i = 0; i < length; i++) {
          search[i] = argv[optind][i];
        }
        flag->begin = optind + 1;
      } else {
        flag->begin = optind;
        if (flag->f == 1) {
          if (flag->e != 0) {
            strcat(search, "|");
          }
          patterns_file(search, pattern_file);
        }
      }
      for (int i = flag->begin; i < argc; i++) {
        filenames[flag->number_of_files] = i;
        flag->number_of_files += 1;
      }
    }
  }
}

void processing(FILE *f, opt *flag, char *search, char *file) {
  regex_t regex = {0};
  char buffer[2048] = {0};
  int result = 0;
  int error = 0;
  if (flag->i == 1 && flag->e != 1 && flag->f != 1) {
    error = regcomp(&regex, search, REG_ICASE);
  }
  if (flag->e != 0 || flag->f != 0) {
    if (flag->i == 1) {
      error = regcomp(&regex, search, REG_EXTENDED | REG_ICASE);
    } else {
      error = regcomp(&regex, search, REG_EXTENDED);
    }
  }
  if (flag->i == 0 && flag->e == 0 && flag->f == 0) {
    error = regcomp(&regex, search, 0);
  }
  if (error == 0) {
    while (fgets(buffer, 2048, f)) {
      flag->all_lines_counter++;
      char *new = strchr(buffer, '\n');
      if (new == NULL) {
        strcat(buffer, "\n");
      }
      buffer[strcspn(buffer, "\n")] = 0;
      if (flag->o == 0) {
        result = regexec(&regex, buffer, 0, NULL, 0);
        without_o(flag, result, file, buffer);
      } else {
        with_o(regex, buffer, flag, file);
      }
    }
    c_l_flags(flag, file);
    flag->matching_file = 0;
    flag->matching_lines_counter = 0;
    flag->all_lines_counter = 0;
    regfree(&regex);
  } else if (flag->f == 1) {
    while (fgets(buffer, 2048, f)) {
      printf("%s", buffer);
    }
  }
}

void patterns_file(char *search, char *pattern_file) {
  int count = 0;
  char buf[2048] = {0};
  FILE *patterns = fopen(pattern_file, "r");
  if (patterns != NULL) {
    while (fgets(buf, 2048, patterns)) {
      char *ptr = strchr(buf, '\n');
      if (ptr != NULL) {
        *ptr = '\0';
      }
      if (count == 0) {
        strcat(search, buf);
        count++;
      } else {
        strcat(search, "|");
        strcat(search, buf);
      }
    }
    fclose(patterns);
  }
}

void c_l_flags(opt *flag, char *file) {
  if (flag->c == 1 && flag->l == 1) {
    if (flag->number_of_files > 1) {
      if (flag->matching_lines_counter > 0) {
        if (flag->h == 1) {
          printf("1\n%s\n", file);
        } else {
          printf("%s:1\n%s\n", file, file);
        }
      } else {
        if (flag->h == 0) {
          printf("%s:0\n", file);
        } else {
          printf("0\n");
        }
      }
    } else {
      if (flag->matching_lines_counter > 0) {
        printf("1\n%s\n", file);
      } else {
        printf("0\n");
      }
    }
  } else {
    if (flag->c == 1) {
      if (flag->number_of_files > 1 && flag->h == 0) {
        printf("%s:%i\n", file, flag->matching_lines_counter);
      } else {
        printf("%i\n", flag->matching_lines_counter);
      }
    }
    if (flag->matching_file == 1) printf("%s\n", file);
  }
}

void without_o(opt *flag, int result, char *file, char *buffer) {
  if (flag->c == 1 || flag->l == 1) {
    if (flag->c == 1) {
      if (flag->v == 1 && result == REG_NOMATCH) flag->matching_lines_counter++;
      if (flag->v != 1 && result == 0) flag->matching_lines_counter++;
    }
    if (flag->l == 1) {
      if (flag->v == 1 && result == REG_NOMATCH) flag->matching_file = 1;
      if (flag->v != 1 && result == 0) flag->matching_file = 1;
    }
  } else {
    if ((flag->v == 1 && result == REG_NOMATCH) ||
        (flag->v == 0 && result == 0)) {
      if (flag->h == 0 && flag->number_of_files > 1) {
        printf("%s:", file);
      }
      if (flag->n == 1) {
        printf("%i:", flag->all_lines_counter);
      }
      printf("%s\n", buffer);
    }
  }
}

void with_o(regex_t regex, char *buffer, opt *flag, char *file) {
  regmatch_t pmatch[1] = {0};
  char *p = buffer;
  while (regexec(&regex, p, 1, pmatch, 0) == 0) {
    if (flag->l == 1 || flag->c == 1) {
      if (flag->l == 1) {
        flag->matching_file = 1;
      }
      if (flag->c == 1) {
        if (flag->n_counter == 0) {
          flag->matching_lines_counter++;
          flag->n_counter = 1;
        }
      }
    } else {
      if (flag->h != 1 && flag->number_of_files > 1) {
        printf("%s:", file);
      }
      if (flag->n == 1 && flag->n_counter == 0) {
        printf("%i:", flag->all_lines_counter);
        flag->n_counter = 1;
      }
      printf("%.*s\n", (int)(pmatch[0].rm_eo - pmatch[0].rm_so),
             p + pmatch[0].rm_so);
    }
    p += pmatch[0].rm_eo;
  }
  flag->n_counter = 0;
}
