#define _CRT_SECURE_NO_WARNINGS
#include "decompressor.h"
#include <string.h>
#include <stdlib.h>



struct SYM* getHeader(FILE *fp, int *numSymbol, struct SYM* syms, int *rest,unsigned long long* sizeFile, char* extension)
{
	int i = 0, j = 0;
	unsigned char ch;
	char temp[30] = { 0 };
	syms = NULL;
	if (fgets(temp, 4, fp) == NULL)
	{
		printf("Can`t read file!");
		return 0;
	}
	if (strcmp(NAME_HEADER, temp))
	{
		printf("File don`t support\n");
		return 0;
	}
	while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == '-')
		{
			fseek(fp, -2, SEEK_CUR);
			break;
		}
		temp[i++] = ch;
	}
	i--;
	temp[i] = 0;
	*numSymbol = atoi(temp);
	while (syms==NULL)
		syms = (struct SYM*)calloc(*numSymbol, sizeof(struct SYM));
	for(i=0;i<*numSymbol;i++)
	{
		ch = fgetc(fp);
		syms[i].ch = ch;
		ch = fgetc(fp);
		fgets(temp, 7, fp);  //4-signs
		syms[i].freq = (float)atof(temp);  //??? double to float
	}
	temp[0] = fgetc(fp);
	temp[1] = 0;
	*rest = atoi(temp);
	i = 0;
	while ((ch = fgetc(fp))>='0'&& ch <= '9')
	{
		temp[i] = ch;
		i++;
	}
	temp[i] = 0;
	*sizeFile = atoll(temp);
	i = 0;
	while (ch < '0'|| ch > '9')
	{
		temp[i++] = ch;
		ch = fgetc(fp);
	}
	temp[i] = 0;
	strcpy(extension, temp);
	return syms;
}

void unpack(unsigned char ch,unsigned char* buf)
{
	union CODE code;
	code.ch = ch;
	buf[0] = code.byte.b1 + '0';
	buf[1] = code.byte.b2 + '0';
	buf[2] = code.byte.b3 + '0';
	buf[3] = code.byte.b4 + '0';
	buf[4] = code.byte.b5 + '0';
	buf[5] = code.byte.b6 + '0';
	buf[6] = code.byte.b7 + '0';
	buf[7] = code.byte.b8 + '0';
}

void make101File(FILE* in, FILE* out)
{
	int ch;
	unsigned char buf[9] = { 0 };
	while ((ch = fgetc(in))!=EOF)
	{
		unpack(ch, buf);
		fputs(buf, out);
	}
}

struct SYM** mkPsyms(struct SYM* syms, int N)
{
	struct SYM** psyms = NULL;
	int i = 0;
	while (psyms == NULL)
		psyms = (struct SYM**)calloc(sizeof(struct SYM*), N);
	for (i = 0; i < N; i++)
			psyms[i] = syms + i;
	return psyms;
}

struct SYM* buildTree(struct SYM *psym[], int N)
{
	// создаЄм временный узел
	struct SYM *temp = (struct SYM*)malloc(sizeof(struct SYM));
	// в поле частоты записываетс€ сумма частот
	// последнего и предпоследнего элементов массива psym
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;
	// св€зываем созданный узел с двум€ последними узлами
	temp->left = psym[N - 1];
	temp->right = psym[N - 2];
	temp->code[0] = 0;
	if (N == 2) // мы сформировали корневой элемент с частотой 1.0
		return temp;
	// добавл€ем temp в нужную позицию psym,
	// сохран€€ пор€док убывани€ частоты
	int i = 1;
	while ((N - i - 1) >= 0 && temp->freq >= psym[N - i - 1]->freq)
	{
		psym[N - i] = psym[N - i - 1];
		psym[N - i - 1] = temp;
		i++;
	}
	return buildTree(psym, N - 1);
}

unsigned char makeChars(struct SYM *root,FILE *fp)
{
	unsigned char ch;
	if (root->left == NULL && root->right == NULL)
		return root->ch;
	else
	{
		ch = fgetc(fp);
		if (ch == '1')
			makeChars(root->right, fp);
		else
			makeChars(root->left, fp);
	}
}

void cutFile(FILE* fp, int rest)
{
	int position = -rest;
	fseek(fp, position, SEEK_END);
	fputc(EOF, fp);
	rewind(fp);
}

void makeSourceFile(FILE* fp101,FILE*  fpSource,struct SYM* root,int rest)
{
	unsigned long long position = 0;
	fseek(fp101, -rest, SEEK_END);
	position = ftell(fp101);
	rewind(fp101);
	unsigned char ch = 0;
	while (ftell(fp101)!= position)
	{
		ch=makeChars(root, fp101);
		fputc(ch, fpSource);
	}
}

void makeNameFile(char* name, char* extension)
{
	strcat(name, extension);
}

void printPsyms(struct SYM** arr, int N)
{
	for (int i = 0; i < N; i++)
	{
		printf("%c - %f\t%s\n", arr[i]->ch, arr[i]->freq, arr[i]->code);
	}
	printf("\n");
}
void printSym(struct SYM* arr, int N)
{
	for (int i = 0; i < N; i++)
	{
		if (arr[i].freq != 0)
			printf("%c - %f\t%s\n", arr[i].ch, arr[i].freq, arr[i].code);
	}
	printf("\n");
}
