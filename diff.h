#include <stdlib.h>

typedef struct para para;
para* para_make(char* base[], int size, int start, int stop);
para* para_first(char* base[], int size);
para* para_next(para* p);
size_t para_filesize(para* p);
size_t para_size(para* p);
char** para_base(para* p);
char* para_info(para* p);
int   para_equal(para* p, para* q);
void para_print(para* p, void (*fp)(const char*));

void print_diff(FILE *f1, FILE *f2, int ar[]);
void diff_files(FILE *f1, FILE *f2);