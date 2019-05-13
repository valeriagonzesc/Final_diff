//
//  main.c
//  diff
//
//  Created by William McCarthy on 4/29/19.
//  Copyright © 2019 William McCarthy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARGC_ERROR 1
#define BUFLEN 256
#define MAXSTRINGS 1024

typedef struct para para;
struct para {
  char** base;
  int filesize;
  int start;
  int stop;
};

para* para_make(char* base[], int size, int start, int stop);
para* para_first(char* base[], int size);
para* para_next(para* p);
size_t para_filesize(para* p);
size_t para_size(para* p);
char** para_base(para* p);
char* para_info(para* p);
int   para_equal(para* p, para* q);
void para_print(para* p, void (*fp)(const char*));

para* para_make(char* base[], int filesize, int start, int stop) {
  para* p = (para*) malloc(sizeof(para));
  p->base = base;
  p->filesize = filesize;
  p->start = start;
  p->stop = stop;

  return p;
}

para* para_first(char* base[], int size) {
  para* p = para_make(base, size, 0, -1);
  return para_next(p);
}

void para_destroy(para* p) { free(p); }

para* para_next(para* p) {
  if (p->stop == p->filesize) { return NULL; }

  int i;
  para* pnew = para_make(p->base, p->filesize, p->stop + 1, p->stop + 1);
  for (i = pnew->start; i < p->filesize && strcmp(p->base[i], "\n") != 0; ++i) { }
  pnew->stop = i;

  return pnew;
}
size_t para_filesize(para* p) { return p == NULL ? 0 : p->filesize; }

size_t para_size(para* p) { return p == NULL || p->stop < p->start ? 0 : p->stop - p->start + 1; }

char** para_base(para* p) { return p->base; }

char* para_info(para* p) {
  static char buf[BUFLEN];   // static for a reason
  snprintf(buf, sizeof(buf), "base: %p, filesize: %d, start: %d, stop: %d\n",
                  p->base, p->filesize, p->start, p->stop);
  return buf;  // buf MUST be static
}

int para_equal(para* p, para* q) {
  if (p == NULL || q == NULL) { return 0; }
  if (para_size(p) != para_size(q)) { return 0; }
  int i = p->start, j = q->start, equal = 0;
  while ((equal = strcmp(p->base[i], q->base[i])) == 0) { ++i; ++j; }
  return equal;
}

void para_print(para* p, void (*fp)(const char*)) {
  if (p == NULL) { return; }
  for (int i = p->start; i <= p->stop && i != p->filesize; ++i) { fp(p->base[i]); }
}

char* yesorno(int condition) { return condition == 0 ? "no" : "YES"; }

FILE* openfile(const char* filename, const char* openflags) {
  FILE* f;
  if ((f = fopen(filename, openflags)) == NULL) {  printf("can't open '%s'\n", filename);  exit(1); }
  return f;
}

void printleft(const char* left) {
  char buf[BUFLEN];

  strcpy(buf, left);
  int j = 0, len = (int)strlen(buf) - 1;
  for (j = 0; j <= 48 - len ; ++j) { buf[len + j] = ' '; }
  buf[len + j++] = '<';
  buf[len + j++] = '\0';
  printf("%s\n", buf);
}

void printright(const char* right) {
  if (right == NULL) { return; }
  printf("%50s %s", ">", right);
}

void printboth(const char* left_right) {
  char buf[BUFLEN];
  size_t len = strlen(left_right);
  if (len > 0) { strncpy(buf, left_right, len); }
  buf[len - 1] = '\0';
  printf("%-50s %s", buf, left_right); }

  void print_diff(FILE *f1, FILE *f2, int ar[]){
    int l = 0, s = 0;
    char c1 = getc(f1);
    char c2 = getc(f2);
    while(c1 != EOF && c2 != EOF){
      if(c1 == '\n' && c2 =='\n'){
        l++;
      }
      if(l == ar[s]){
        while (c1 != EOF || c1 != '\n'){
          printf("%c", c1);
          c1 = getc(f1);
        }
        printf("\t");
        while (c2 != EOF || c2 != '\n'){
          printf("%c", c2);
          c2 = getc(f1);
        }
        break;
      }
      c1 = getc(f1);
      c2 = getc(f2);
    }
  }

void diff_files(FILE *f1, FILE *f2){
  char c1 = getc(f1);
  char c2 = getc(f2);
  int l = 0, s = 0;
  int stored_diff[MAXSTRINGS];

  while(c1 != EOF && c2 != EOF){
    if(c1 == '\n' && c2 =='\n'){
      l++;
    }
    if(c1 != c2){
      stored_diff[s] = l;
      s++;
    }
    c1 = getc(f1);
    c2 = getc(f2);
  }
  print_diff(f1, f2, stored_diff);
}

int main(int argc, const char * argv[]) {
  char buf[BUFLEN];
  char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];

  memset(buf, 0, sizeof(buf));
  memset(strings1, 0, sizeof(strings1));
  memset(strings2, 0, sizeof(strings2));

  if (argc != 3) { fprintf(stderr, "Usage: ./diff file1 file2\n");  exit(ARGC_ERROR); }

  FILE *fin1 = openfile(argv[1], "r");
  FILE *fin2 = openfile(argv[2], "r");

  int count1 = 0, count2 = 0;
  while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }
  while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }

  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  if (para_equal(p, q)) { para_print(p, printboth);
  } else {
    para_print(q, printright);

    while ((q = para_next(q)) != NULL) {
      int equal = para_equal(p, q);
      para_print(q, equal ? printboth : printright);
    }
  }

  diff_files(fin1, fin2);
//  printf("p is: %s", para_info(p));
//  printf("q is: %s", para_info(q));
//  para_print(p, printleft);
//  para_print(q, printright);
//  printf("p and q are equal: %s\n\n", yesorno(para_equal(p, q)));

  //printf("\nTODO: check line by line in a paragraph, using '|' for differences");
  //printf("\nTODO: this starter code does not yet handle printing all of fin1's paragraphs.");
  //printf("\nTODO: handle the rest of diff's options\n");
  //printf("\tAs Tolkien said it, '...and miles to go before I sleep.'\n\n");
  fclose(fin1);
  fclose(fin2);
  return 0;
}
