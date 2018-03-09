#include <stdio.h>
#define MAXSTRING 128
#define LENCODE 256

struct SYM
{
	unsigned char ch;
	float freq;
	char code[LENCODE];
	struct SYM* left;
	struct SYM* right;
};

union CODE {
	unsigned char ch;
	struct {
		unsigned short b1 : 1;
		unsigned short b2 : 1;
		unsigned short b3 : 1;
		unsigned short b4 : 1;
		unsigned short b5 : 1;
		unsigned short b6 : 1;
		unsigned short b7 : 1;
		unsigned short b8 : 1;
	} byte;
};

typedef struct SYM SYM_t;

//task1
void createBuf(SYM_t *syms);
int simbolCounter(FILE *fp, SYM_t *syms);
int UniqueSimbol(SYM_t *syms);
void bubleSort(SYM_t *syms);
void tofraction(SYM_t *syms, int count);
void printTable(SYM_t *syms);

//task2
struct SYM* buildTree( SYM_t *psym[], int N);
void makeCodes(SYM_t *root);
void codefile(FILE *fp, FILE * wp, SYM_t* syms, int count);
int seekTail(FILE *fp);
void packfun(FILE *fp, FILE * wp);
unsigned char pack(unsigned char buf[]);
void tableToFile(SYM_t *syms, int countUnique, FILE * wp);