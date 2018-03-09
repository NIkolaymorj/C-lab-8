#include <stdio.h>
#define NAME_HEADER "GOL"

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

void unpack(unsigned char ch, unsigned char* buf);
void make101File(FILE* in, FILE* out);
struct SYM* getHeader(FILE *fp, int *numSymbol, struct SYM* syms, int *rest, unsigned long long* sizeFile, char* extension);
struct SYM** mkPsyms(struct SYM* syms, int N);
struct SYM* buildTree(struct SYM *psym[], int N);
unsigned char makeChars(struct SYM *root, FILE *fp);
void makeSourceFile(FILE* fp101, FILE*  fpSource, struct SYM* root,int rest);
void makeNameFile(char* name, char* extension);
void printPsyms(struct SYM** arr, int N);
void printSym(struct SYM* arr, int N);
