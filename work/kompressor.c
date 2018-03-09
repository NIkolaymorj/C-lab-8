#define _CRT_SECURE_NO_WARNINGS
#include "kompressor.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


void getExtension(char* path, char* extens)
{
	int i=0;
	while (*path != '.')
		path++;
	path++;
	while (*path)
	{
		extens[i++] = *path;
		path++;
	}
	extens[i] = 0;
}

struct SYM* mkSyms(FILE *fp,int *N,unsigned long long *sizeFile)
{
	struct SYM* syms=NULL;
	int i = 0;
	int ch = 0;
	*sizeFile = 0;
	/*long len = 0;
	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);
	fseek(fp,0L,SEEK_SET);*/
	while (syms==NULL)
		syms = (struct SYM*)calloc(SIZE, sizeof(struct SYM)); //создаем массив структур
	for (i = 0; i < SIZE; i++)							  //обнуляем
	{
		syms[i].ch = i;
		syms[i].freq = 0;
		syms[i].code[0] = 0;
		syms[i].left = NULL;
		syms[i].right = NULL;
	}
	while ((ch = fgetc(fp)) != EOF)						//анализ файла, подсчет общего колва символов, заполнение массива структур количеством символов
	{
		(*sizeFile)++;
		syms[ch].freq++;
	}
	rewind(fp);
	for (i = 0; i < SIZE; i++)							//считаем частоту в долях + определяем количество заполненных структур
	{
		if (syms[i].freq != 0)
		{
			(*N)++;
			syms[i].freq = ((float)round((syms[i].freq / (*sizeFile))*10000))/10000;
		}
	}
	return syms;
}


struct SYM** mkPsyms(struct SYM* syms,int N)
{
	struct SYM** psyms=NULL;
	int i = 0, j = 0;
	while (psyms==NULL)
		psyms = (struct SYM**)calloc(sizeof(struct SYM*), N);	//создаем массив указателей на ненулевые структуры
	for (i = 0; i < SIZE; i++)
	{
		if (syms[i].freq != 0)
			psyms[j++] = syms + i;
	}
	sort(psyms,N);
	return psyms;
}

void printPsyms(struct SYM** arr,int N)
{
	for (int i = 0; i < N; i++)
	{
		printf("%c - %f\t%s\n", arr[i]->ch, arr[i]->freq,arr[i]->code);
	}
	printf("\n");
}
void printSym(struct SYM* arr, int N)
{
	for (int i = 0; i < N; i++)
	{
		if (arr[i].freq!=0)
			printf("%c - %f\t%s\n", arr[i].ch, arr[i].freq, arr[i].code);
	}
	printf("\n");
}

void sort(struct SYM** arr,int N)
{
	struct SYM* temp;
	int i = 0, j = 0;
	for (i = 0; i < N;i++)
	for (j = N-1; j > i; j--)
	{
		if (arr[i]->freq < arr[j]->freq)
		{
			temp = arr[j];
			arr[j] = arr[i];
			arr[i] = temp;
		}
	}
}

struct SYM* buildTree(struct SYM *psym[], int N)
{
	// создаём временный узел
	struct SYM *temp = (struct SYM*)malloc(sizeof(struct SYM));
	// в поле частоты записывается сумма частот
	// последнего и предпоследнего элементов массива psym
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;
	// связываем созданный узел с двумя последними узлами
	temp->left = psym[N - 1];
	temp->right = psym[N - 2];
	temp->code[0] = 0;
	if (N == 2) // мы сформировали корневой элемент с частотой 1.0
		return temp;
	// добавляем temp в нужную позицию psym,
	// сохраняя порядок убывания частоты
	int i = 1;
	while ((N - i - 1) >= 0 && temp->freq >= psym[N - i - 1]->freq)
	{
		psym[N - i] = psym[N - i - 1];
		psym[N - i - 1] = temp;
		i++;
	}

	return buildTree(psym, N - 1);
}

void makeCodes(struct SYM *root)
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

int make101File(struct SYM* syms,FILE *fpin,FILE *fpout)
{
	int ch;
	int  i=0;
	unsigned long long count;
	int rest=0;

	while ((ch = fgetc(fpin)) != EOF)
	{
		fprintf(fpout, "%s", syms[ch].code);
	}
	count = ftell(fpout);
	rest = 8 - count % 8;
	if (rest)
	{
		while (i<rest)
		{
			fputc('0', fpout);
			i++;
		}
	}
	return rest;
}

unsigned char package(unsigned char buf[])
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


void makeOutputFile(FILE *fpin, FILE *fpout)
{
	unsigned char ch = 0;
	unsigned char temp[9] = { 0 };
	while (fgets(temp, 9, fpin))
	{
		ch = package(temp);
		fputc(ch, fpout);
	}
}

void makeHeader(FILE *fp, char* nameHeader, int numSymbol, struct SYM** psyms, int rest,
	unsigned long long sizeFile, char* extension,char flag)
{
	int len = strlen(extension);
	if (flag)
	{
		fprintf(fp, "%s%d", nameHeader, numSymbol);
		for (int i = 0; i < numSymbol; i++)
		{
			fprintf(fp, "%c%.4f", psyms[i]->ch, psyms[i]->freq); //???float 4-9 signs
		}
	}
	else
	{
		fprintf(fp, "%d%lu", rest, sizeFile);
		while (*extension)
		{
			fputc(*extension, fp);
			extension++;
		}
		fprintf(fp, "%d", len);
	}
}






