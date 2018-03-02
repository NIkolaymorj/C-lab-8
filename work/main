#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CH 256
#define BYTE 8
#include "task.h"

int main(int argc, char *argv[])
{
	SYM syms[CH];
	SYM *psym[CH];
	HEAD header;
	int count = 0;
	FILE *inp, *outp;
	FILE *fp101;
	char ch_s;
	SYM *root;
	int tail = 0;
	argv[1] = "xyz.txt";
	char *fname;
	fname = argv[1];
	puts(fname);

	inp = fopen(fname, "rt");
	
	header.name_orig = fname; // Имя Файла
	
	if (inp == NULL)
	{
		puts("I can't open inp!");
		return 1;
	}

	for (int i = 0; i<CH; i++)    //инициализация структуры
	{
		syms[i].ch = i;
		syms[i].freq = 0;
		syms[i].left = NULL;
		syms[i].right = NULL;
	}

	while (!feof(inp))        //считаем кол-во одинаковых символов
	{
		fscanf(inp, "%c", &ch_s);
		syms[ch_s].freq++;
		count++;
	}
	header.size_orig = count; // размер файла
	
	sortCh(syms, CH); //сортировка

	int count_syms = 0;

	for (int i = 0; i<CH; i++) //считаем частоту символов
	{
		if (syms[i].freq > 0)
		{
			syms[i].freq = syms[i].freq / count;
			header.freq[i] = syms[i].freq;
			printf("ch: %c - %.2f\n", syms[i].ch, syms[i].freq);
			count_syms++;
		}
	}

	for (int i = 0; i<CH; i++) 
	{
		psym[i] = &syms[i];
	}
	
   	root = buildTree(psym, count_syms); //Дерево Хаффмана
		
	makeCodes(root); //получение кодов
		
	fseek(inp, sizeof(inp), SEEK_SET);

	fp101 = fopen("101.txt", "wt");
	int ch=0; 			 
	while ((ch = fgetc(inp)) != EOF) //кодиование
	{
		for (int i = 0; i<count_syms; i++)
			if (syms[i].ch == (unsigned char)ch)
			{
				fputs(syms[i].code, fp101); // выводим строку с кодом
				break;
			}
	}
	fclose(fp101);
	fclose(inp);
	fp101 = fopen("101.txt", "rt"); 
	
	fseek(fp101, 0, SEEK_END); 
	int size = ftell(fp101);
	if (size % BYTE)
		header.tail = BYTE - (size % BYTE); //Размер хвоста
	else
		header.tail = 0;
	printf("tail: %d\n", header.tail);
	
	fseek(fp101, sizeof(fp101), SEEK_SET);
	outp = fopen("fzip.myzip", "wt");
	
	fprintf(outp, "%s %d %d ", header.name_orig, header.size_orig, header.tail);

	for (int i = 0; i < CH; i++)
		fprintf(outp, "%.3f ", header.freq[i]);

	int count_byte = 0;
	unsigned char byte[BYTE];
	
	while ((ch = getc(fp101)) != EOF)
	{
		byte[count_byte] = ch;
		if (count_byte == BYTE - 1)
		{
			count_byte = 0;
			fputc(pack(byte), outp);
			continue;
		}
		count_byte++;
	}
	printf("count_byte %d\n", count_byte);
	for (int i = count_byte; i < BYTE; i++)
		byte[i] = '0';

	fputc(pack(byte), outp);
	
	fclose(outp);
	fclose(fp101);

	return 0;
}
