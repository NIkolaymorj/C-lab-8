#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ASCII 256

typedef struct SYM SYM;

struct SYM
{
	unsigned char ch;					
	float freq;							
	char code[256];						
	struct SYM *left;					
	struct SYM *right;					
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

SYM syms[ASCII];
SYM *psym[ASCII];

void prepareStructArray(SYM syms[]);
int readFile(FILE *fp, SYM syms[]);
void freqCount(SYM syms[], int count);
void sort(SYM syms[], int size);
int printTable(SYM syms[]);
void initPointer(SYM syms[]);
SYM* buildTree(SYM *psym[], int N);
void makeCodes(struct SYM *root);
unsigned int codeFile(FILE *fp_in, FILE *fp_101, int countAllSyms);
void createSignature(FILE *fp_out, SYM syms[], int count, int tail);
void packBitFile(FILE *fp_101, FILE *fp_out, unsigned int bytes, int tail);