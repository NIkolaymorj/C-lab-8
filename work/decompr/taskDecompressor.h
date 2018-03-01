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
void depackfun(FILE *fp, FILE * wp);
void depack(unsigned char buf[], char s);
void createBuf(SYM_t *syms, FILE * fp, int countUnique);
struct SYM* buildTree(SYM_t *psym[], int N);
void makeCodes(SYM_t *root);
void fileRecovery(FILE*fp, SYM_t*root, FILE* wp, int tail);
SYM_t * seekCode(SYM_t *branch, char s, FILE *wp, SYM_t *root);