#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#define LEN 300

#ifdef __APPLE__
char *short_options = "+benstvET";
#else
char *short_options = "benstvET";
#endif

typedef struct {
  int b;
  int n;
  int e;
  int s;
  int t;
  int v;
  int number_of_files;
  int count;
  int ind;
  int lines_to_squeeze;
  int blank_line;
  int new_line;
  int non_print;
} opt;

void squeeze(char print_char, opt *flag);
void non_blank(char print_char, opt *flag);
void number_all(char print_char, opt *flag);
void non_printable(char print_char, opt *flag);

void processing(FILE *f, opt *flag);
void parsing(int argc, char *argv[], opt *flag, int *filenames);
void reading_files(int argc, char *argv[]);

#endif  // SRC_CAT_S21_CAT_H_