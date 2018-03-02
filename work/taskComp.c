#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "taskComp.h"

void prepareStructArray(SYM syms[])
{
	for (int i = 0; i < ASCII; i++)
	{
		syms[i].ch = i;
		syms[i].freq = 0;
		syms[i].left = NULL;
		syms[i].right = NULL;
		for (int j = 0; j < ASCII; j++)
			syms[i].code[j] = 0;
	}
}

int readFile(FILE *fp, SYM syms[])
{
	int count = 0;
	int ch;
	while ((ch = fgetc(fp)) != EOF)
	{
		syms[ch].freq++;
		count++;
	}
	return count;
}

void freqCount(SYM syms[], int count)
{
	for (int i = 0; i < ASCII; i++)
	{
		syms[i].freq = syms[i].freq / count;
	}
}

int compare(const void *a, const void *b)
{
	return (int)(1000000 * (((struct SYM*)b)->freq - ((struct SYM*)a)->freq));
}

void sort(SYM syms[], int size)
{
	qsort(syms, size, sizeof(struct SYM), compare);
}

int printTable(SYM syms[])
{
	int countAllSyms = 0;
	printf(" Sorted symbols:\n");
	printf(" symbol | frequency\n");
	printf(" ------------------\n");

	for (int i = 0; i < ASCII; i++)
	{
		if (syms[i].freq > 0)
		{
			if (syms[i].ch == '\n')
				printf("     \\n | %f  \n", syms[i].freq);
			else printf("     %c  | %f  \n", syms[i].ch, syms[i].freq);
			countAllSyms++;
		}
	}
	return countAllSyms;
}

void initPointer(SYM syms[])
{
	for (int i = 0; i < ASCII; i++)
	{
		psym[i] = &syms[i];
	}
}

SYM* buildTree(SYM *psym[], int N)
{
	SYM *temp = (SYM*)malloc(sizeof(SYM));	
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;
	temp->left = psym[N - 1];
	temp->right = psym[N - 2];
	temp->code[0] = 0;
	if (N == 2)						
		return temp;
	else {							
		for (int i = 0; i < N; i++)
		{
			if (temp->freq > psym[i]->freq)
			{
				for (int j = N - 1; j > i; j--)
				{
					psym[j] = psym[j - 1];
				}
				psym[i] = temp;
				break;
			}
		}
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

unsigned int codeFile(FILE *fp_in, FILE *fp_101, int countAllSyms)
{
	int ch = 0;
	unsigned int countCode = 0;
	while ((ch = fgetc(fp_in)) != -1)
	{
		for (int i = 0; i < countAllSyms; i++)
		{
			if (syms[i].ch == (unsigned char)ch)
			{
				countCode = countCode + strlen(syms[i].code);
				fputs(syms[i].code, fp_101);
				break;
			}
		}
	}
	return countCode;
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

void createSignature(FILE *fp_out, SYM syms[], int count, int tail)
{
	fwrite("IMCOMPRESSED!", sizeof(char), 14, fp_out);
	fwrite(&count, sizeof(int), 1, fp_out);
	fwrite(&tail, sizeof(int), 1, fp_out);
	for (int i = 0; i < count; i++)
	{
		fwrite(&syms[i].ch, sizeof(char), 1, fp_out);
		fwrite(&syms[i].freq, sizeof(float), 1, fp_out);
	}
}

void packBitFile(FILE *fp_101, FILE *fp_out, unsigned int bytes, int tail)
{
	int sym[8] = { 0 };
	int ch = 0;
	union CODE code;

	while (bytes)
	{
		for (int i = 0; i < 8; i++)
		{
			ch = fgetc(fp_101);
			sym[i] = ch;
		}
		code.ch = pack(sym);
		fputc(code.ch, fp_out);
		bytes--;
	}

	for (int i = 0; i < tail; i++)
	{
		ch = fgetc(fp_101);
		sym[i] = ch;
	}
	code.ch = pack(sym);
	fputc(code.ch, fp_out);
}

