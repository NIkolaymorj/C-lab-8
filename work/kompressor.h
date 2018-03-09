#include <stdio.h>
#define SIZE 256
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

void getExtension(char* path, char* extens);
struct SYM* mkSyms(FILE *fp, int *N, unsigned long long *sizeFile);
struct SYM** mkPsyms(struct SYM* syms, int N);
void sort(struct SYM** arr, int N);
void printPsyms(struct SYM** arr, int N);
void printSym(struct SYM* arr, int N);
struct SYM* buildTree(struct SYM *psym[], int N);
void makeCodes(struct SYM *root);
int make101File(struct SYM* syms, FILE *fpin, FILE *fpout);
unsigned char package(unsigned char buf[]);
void makeOutputFile(FILE *fpin, FILE *fpout);
void makeHeader(FILE *fp,char* nameHeader, int numSymbol, struct SYM** psyms, int rest, 
				unsigned long long sizeFile, char* extension,char flag);
