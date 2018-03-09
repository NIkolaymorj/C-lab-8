#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define _CRT_SECURE_NO_WARNINGS
#define ASCII 256
#define SIZE 8

typedef struct SYM SYM;
typedef struct SYM TSYM;

struct SYM
{
	unsigned char ch;   //ASCII
	float freq;		    //frequency 
	char code[256];		//for new codes
	struct SYM *left;	//left branch in tree
	struct SYM *right;  //right branch in tree
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

TSYM syms[ASCII];
TSYM *psyms[ASCII];
FILE *fp_in;
FILE *fp_101;
FILE *fp_out;

int count = 0;
int countNum = 0;
int N = 0;

void readFile(FILE fp_in, TSYM syms[]);
void matchFreq(TSYM syms[]);
void print(SYM syms[]);
TSYM* buildTree(psyms, N);
void makeCodes(TSYM *root);
void codingFile();
int countForCode();
void fileOut(int tail);
void readAndPack(int tail,int bytes);
unsigned char pack(unsigned char buf[]);

//вывод на экран таблицы встречаемости
void print(SYM syms[])
{
	printf("Sorted symbols: \n");
	printf("----------------\n");
	for (int i = 0; i < ASCII; i++)
	{
		if (syms[i].freq > 0) {
			if (syms[i].ch == '\n')
				printf("Sorted Symbol:\\n -   %f  \n", syms[i].freq);
			else
			printf("Sorted Symbol: %c -   %f  \n", syms[i].ch, syms[i].freq);
		}
	}
}

void matchFreq()
{
	for (int i = 0; i < ASCII; i++)
	{
		syms[i].freq = syms[i].freq / count;
	}
}

//считываем файл
void readFile(FILE *fp, SYM syms[])
{
	char sym;
	int symNum;

	while ((symNum = fgetc(fp)) != EOF) //while (!feof(fp)) - can be also used
	{
		syms[(char)symNum].freq++;
		count++;
	}
}

//для сортировки
int compare(const void *a, const void *b)
{
	//Если не использовать *1000 то разница в вычитании типа float крайне мала и при преобразовании в int система засчитает разницу как 0
	return (int)(1000 * (((struct SYM*)b)->freq - ((struct SYM*)a)->freq));
}

struct SYM* buildTree(struct SYM *psym[], int N)
{
	int i = 0, j = 0;
	// создаём временный узел
	struct SYM *temp = (struct SYM*)malloc(sizeof(struct SYM));
	// в поле частоты записывается сумма частот
	// последнего и предпоследнего элементов массива psym
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;
	// связываем созданный узел с двумя последними узлами
	temp->left = psym[N - 1];
	temp->right = psym[N - 2];
	temp->code[0] = 0;
	if (N == 2)			// мы сформировали корневой элемент с частотой 1.0
		return temp;
	
	else				//если меньше двух символов
	{
		for (i = 0; i < N; i++)
		{
			if (temp->freq > psym[i]->freq)
			{
				for (j = N - 1; j > i; j--)
				{
					psym[j] = psym[j - 1];
				}
				psym[i] = temp;
				break;
			}
		}
	}
	
	return buildTree(psym, N - 1);		// добавляем temp в нужную позицию psym, сохраняя порядок убывания частоты
}

void makeCodes(struct SYM *root)		//получения префиксного кода
{
	if (root->left)
	{
		strcpy(root->left->code, root->code);
		strcat(root->left->code, "0");
		makeCodes(root->left);
	}
	if (root->right)
	{
		strcpy(root->right->code, root->code);
		strcat(root->right->code, "1");
		makeCodes(root->right);
	}
}

void codingFile()
{
	int i = 0;
	int ch;			// код символа из файла
	FILE *fp_in;	// исходный файл
	FILE *fp_101;	// закодированный файл

	while ((ch = fgetc(fp_in)) != -1)
	{
		for(i = 0; i < count; i++)
			if (syms[i].ch == (unsigned char)ch) {
				fputs(syms[i].code, fp_101);			// выводим строку с кодом
				break;									// прерываем поиск
			}
	}
}

int countForCode()
{
	int num = 0;

	while ((num = fgetc(fp_101)) != -1)
	{
		countNum++;
	}
	return countNum;
}

void fileOutput(int tail)
{
	int i = 0;
	int countSym = count - 1;

	fwrite("ABC", sizeof(char), 4, fp_out);
	fwrite(&countSym, sizeof(int), 1, fp_out);
	fwrite(&tail, sizeof(int), 1, fp_out);

	for (i = 0; i<count; i++)
	{
		fwrite(&syms[i].ch, sizeof(char), 1, fp_out);
		fwrite(&syms[i].freq, sizeof(float), 1, fp_out);
	}
}

void readAndPack(int tail, int bytes)
{
	unsigned char buf[SIZE];
	int i = 0;
	int ch = 0;
	union CODE code;

	for (i = 0; i < bytes; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			ch = fgetc(fp_101);
			buf[j] = ch;
		}
		code.ch = pack(buf);
		fputc(code.ch, fp_out);
	}
	for (i = 0; i<tail; i++)
	{
		ch = fgetc(fp_101);
		buf[i] = ch;
	}
	code.ch = pack(buf);
	fputc(code.ch, fp_out);
}

unsigned char pack(unsigned char buf[])
{
	union CODE code;
	code.byte.b1 = buf[0] - '0';
	code.byte.b2 = buf[1] - '0';
	code.byte.b3 = buf[2] - '0';
	code.byte.b4 = buf[3] - '0';
	code.byte.b5 = buf[4] - '0';
	code.byte.b6 = buf[5] - '0';
	code.byte.b7 = buf[6] - '0';
	code.byte.b8 = buf[7] - '0';
	return code.ch;
}

int main()
{
	int tail = countNum % 8;
	int bytes = countNum / 8;

	FILE *fp_in = fopen("text.txt", "rb");
	FILE *fp_101 = fopen("fp_101.txt", "w + b");
	FILE *fp_out = fopen("fp_out.txt", "wb");

	if (fp_in == NULL)
	{
		puts("404! FIle not found!");
		return 1;
	}

	for (int i = 0; i < ASCII; i++)
	{
		syms[i].ch = i;
		syms[i].freq = 0;
	}

	readFile(fp_in, syms);
	matchFreq(syms);
	qsort(syms, ASCII, sizeof(struct SYM), compare);
	print(syms);
	TSYM *root = buildTree(psyms, N);
	makeCodes(root);
	codingFile();
	countNum = countForCode();
	fileOut(tail);
	readAndPack(tail, bytes);
	
	fclose(fp_in);
	fclose(fp_101);
	fclose(fp_out);

	return 0;
}
