#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LEN 300

char *short_options = "e:viclnhsf:o";

typedef struct {
  int e;
  int v;
  int i;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int file_ind;
  int number_of_files;
  int matching_lines_counter;
  int matching_file;
  int all_lines_counter;
  int begin;
  int error;
  int n_counter;
} opt;

void without_o(opt *flag, int result, char *file, char *buffer);
void with_o(regex_t regex, char *buffer, opt *flag, char *file);
void patterns_file(char *search, char *pattern_file);
void c_l_flags(opt *flag, char *file);

void reading_files(int argc, char *argv[]);
void parsing(int argc, char *argv[], opt *flag, int *filenames, char *search);
void processing(FILE *f, opt *flag, char *search, char *file);

#endif  // SRC_GREP_S21_GREP_H_