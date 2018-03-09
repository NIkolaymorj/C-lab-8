#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "taskComp.h"

int main()
{
	int count = 0;
	int countAllSyms = 0;							//number of all unique symbols
	unsigned int countCode = 0;						//number of bits in file-101
	unsigned int countBytes = 0;					//number of bytes
	int tailSize = 0;								//size of tail in last byte

	FILE *fp_in = fopen("in.txt", "rt");
	FILE *fp_101 = fopen("101.txt", "w+b");
	FILE *fp_out = fopen("out.txt", "w+b");
	if (fp_in == NULL)
	{
		puts("Error: File not found! Please create in.txt");
		return 1;
	}	

	prepareStructArray(syms);
	count = readFile(fp_in, syms);					//read input-file and count number of symbols
	freqCount(syms, count);							//count frequency of occurrence of each symbol 
	sort(syms, ASCII);								//sorting symbol table
	countAllSyms = printTable(syms);				//print table of occurrence

	initPointer(syms);
	SYM *root = buildTree(psym, countAllSyms);		

	makeCodes(root);								
	rewind(fp_in);
	countCode = codeFile(fp_in, fp_101, countAllSyms);		//fill file 101.txt
	printf("Code101 = %d\n", countCode);
	countBytes = countCode / 8;								
	tailSize = countCode % 8;
	printf("Bytes = %d\n", countBytes);
	printf("Tail = %d\n", tailSize);
	createSignature(fp_out, syms, countAllSyms, tailSize);	//create signature for out.txt
	rewind(fp_101);
	packBitFile(fp_101, fp_out, countBytes, tailSize);		//create packed file out.txt

	printf("Your file has been compressed. Pay attention to the size of out.txt!\n");
	
	fclose(fp_in);
	fclose(fp_101);
	fclose(fp_out);
	return 0;
}