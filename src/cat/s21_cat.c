#include "s21_cat.h"

int main(int argc, char *argv[]) {
  reading_files(argc, argv);
  return 0;
}

void reading_files(int argc, char *argv[]) {
  int indexes[LEN] = {0};
  int *filenames = indexes;
  opt flag = {0};
  parsing(argc, argv, &flag, filenames);
  for (flag.ind = 0; flag.ind < flag.number_of_files; flag.ind++) {
    flag.count = 1;
    FILE *f = fopen(argv[filenames[flag.ind]], "r");
    if (f != NULL) {
      processing(f, &flag);
      fclose(f);
    } else {
      fprintf(stderr, "cat: %s: No such file or directory\n",
              argv[filenames[flag.ind]]);
    }
  }
}

void parsing(int argc, char *argv[], opt *flag, int *filenames) {
  if (argc > 1) {
    int option = 0;
    int option_index = 0;
    opterr = 0;
    const struct option long_options[] = {
        {"number-nonblank", 0, 0, 'b'},
        {"number", 0, 0, 'n'},
        {"squeeze-blank", 0, 0, 's'},
        {0, 0, 0, 0},
    };
    while ((option = getopt_long(argc, argv, short_options, long_options,
                                 &option_index)) != -1) {
      switch (option) {
        case 'b':
          flag->b = 1;
          break;
        case 'e':
          flag->e = 1;
          flag->v = 1;
          break;
        case 'n':
          flag->n = 1;
          break;
        case 's':
          flag->s = 1;
          break;
        case 'v':
          flag->v = 1;
          break;
        case 't':
          flag->t = 1;
          flag->v = 1;
          break;
        case 'E':
          flag->e = 1;
          break;
        case 'T':
          flag->t = 1;
          break;
        case '?':
          fprintf(stderr,
                  "s21_cat: invalid option -- '%c'\nusage: cat [-benstuv] "
                  "[file ...]\n",
                  option);
          break;
      }
    }
    for (int i = optind; i < argc; i++) {
      filenames[flag->number_of_files] = i;
      flag->number_of_files += 1;
    }
  }
}

void processing(FILE *f, opt *flag) {
  char print_char = '\0';
  while ((print_char = getc(f)) != EOF) {
    squeeze(print_char, flag);
    non_blank(print_char, flag);
    number_all(print_char, flag);
    non_printable(print_char, flag);
    if (flag->t == 1 && print_char == 9) printf("^I");
    if (flag->e == 1 && print_char == 10 && flag->blank_line == 0) printf("$");
    if (flag->blank_line == 0 && flag->non_print == 0) {
      if (flag->t == 1 && print_char != 9) {
        printf("%c", print_char);
      } else if (flag->t != 1) {
        printf("%c", print_char);
      }
    }
  }
  if (flag->s == 1 && flag->blank_line > 0) {
    if (flag->n) printf("%6i\t", flag->count++);
    flag->e ? printf("$\n") : printf("\n");
  }
}

void squeeze(char print_char, opt *flag) {
  if (flag->s == 1) {
    if (print_char == 10 && flag->lines_to_squeeze == 0) {
      flag->lines_to_squeeze += 1;
    } else if (print_char == 10 && flag->lines_to_squeeze > 0) {
      flag->lines_to_squeeze += 1;
      flag->blank_line++;
    } else if (print_char != 10 && flag->lines_to_squeeze > 1) {
      if (flag->n) printf("%6i\t", flag->count++);
      flag->e ? printf("$\n") : printf("\n");
      flag->lines_to_squeeze = 0;
      flag->blank_line = 0;
    } else {
      flag->lines_to_squeeze = 0;
    }
  }
}

void non_blank(char print_char, opt *flag) {
  if (flag->b == 1) {
    flag->n = 0;
    if (flag->count == 1 && print_char != 10) printf("%6i\t", flag->count++);
    if (flag->count > 1 && print_char == 10 && flag->blank_line == 0)
      flag->new_line++;
    if (flag->count > 1 && print_char != 10 && flag->new_line != 0) {
      printf("%6i\t", flag->count++);
      flag->new_line = 0;
    }
  }
}

void number_all(char print_char, opt *flag) {
  if (flag->n == 1) {
    if (flag->count == 1 && flag->ind == 0) printf("%6i\t", flag->count++);
    if (flag->new_line == 1 && flag->lines_to_squeeze == 0)
      printf("%6i\t", flag->count++);
    if (flag->count > 1 && print_char == 10) flag->new_line = 1;
    if (flag->count > 1 && print_char != 10) flag->new_line = 0;
  }
}

void non_printable(char print_char, opt *flag) {
  if (flag->v == 1) {
    if (print_char < 32 && print_char != 9 && print_char != 10) {
      printf("^%c", print_char + 64);
      flag->non_print = 1;
    } else if (print_char == 127) {
      printf("^?");
      flag->non_print = 1;
    } else {
      flag->non_print = 0;
    }
  }
}