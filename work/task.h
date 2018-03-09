#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#define CH 256

struct SYM
{
	unsigned char ch;
	float freq;
	char code[256]; // массив для нового кода
	struct SYM *left; // левый потомок в дереве
	struct SYM *right; // правый потомок в дереве
};
typedef struct SYM SYM;

struct HEAD
{
	char *name_orig; //имя исх. файла
	int tail; // размер хвоста
	int size_orig;  // размер исх. файла
	float freq[256]; //частота встреч. символов
};
typedef struct HEAD HEAD;

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
typedef union CODE CODE;

void sortCh(SYM syms[],int size);
SYM* buildTree(SYM **psym, int N);
void makeCodes(SYM *root);
unsigned char pack(unsigned char buf[]);
