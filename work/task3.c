#define _CRT_SECURE_NO_WARNINGS
#include "task3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<ctype.h>
#define MAXSYMB 256
#define successfully 1
#define fall 0
typedef unsigned char UC;
#define WRITE "wb"
#define READ "rb"
void countTotalStructInTree(PSYM treeSym, int *totalStructInfile)
{

	if (treeSym->moreNode)   
	{
	
		countTotalStructInTree(treeSym->moreNode, totalStructInfile);
	}
	if (treeSym->lessNode)   
	{
		
		countTotalStructInTree(treeSym->lessNode, totalStructInfile);
	}
	*totalStructInfile += 1;
}
void countTotal2Tree(PSYM treeSym, ULL *totalCount, FILE*fpcheck)
{

	if (treeSym->moreNode)    countTotal2Tree(treeSym->moreNode, totalCount, fpcheck);
	if (treeSym->lessNode)    countTotal2Tree(treeSym->lessNode, totalCount, fpcheck);
	
	*totalCount+= treeSym->count;
	fprintf(fpcheck, "%u\t%u\t%0.6f\t%s\t%p\t%p\n", treeSym->ch, treeSym->count,treeSym->freq,treeSym->code,treeSym->lessNode,treeSym->moreNode);
 }

int check2Tree(PSYM treeSym, ULL trueTotal )//need for check number of total letters  in tree and in the source file
{
	FILE *fpcheck = fopen("checkTree.xls",WRITE);
	if (!fpcheck)
	{
		printf("ERROR of opened fpcheck\n");
		return fall;
	}
	fprintf(fpcheck, "%s\t%s\t%s\t%s\t%s\t%s\n","Input ch", "total", "frequency", "CODE", "lessNode", "moreNode");

	ULL countTree = 0;
		countTotal2Tree(treeSym, &countTree,fpcheck);
		fclose(fpcheck);
		if(countTree==trueTotal)
			return successfully;// it's ok and number of leters in Tree = number of letter in input's file  
	else 
		return fall;// it's bad becouse number of leters in Tree > or < number of letter in input's file 
	
}
PSYM makeTree(PSYM treeSym, UC inputSymbol, UC Node)
{
	long int rezult = 0;

	if (treeSym == NULL)
	{
		while (treeSym == NULL)
			treeSym = (TSYM*)calloc(1,sizeof(TSYM));
		
		treeSym->ch = inputSymbol;
		//strcpy(treeSym->chars, ch);// need if wil be string
		treeSym->count=1;
		treeSym->freq = 0.0;
		*treeSym->code ='\0';
		treeSym->moreNode = NULL;
		treeSym->lessNode = NULL;
		return treeSym;
	}
	else if (treeSym->ch== inputSymbol) // Will be work with only  different int
	//else if (strcmp(treeSym->chars, ch) == 0)// // Will be work with only different char
	{
		treeSym->count++;
		return treeSym;
	}
	else if ((rezult=(inputSymbol - Node))>0)
		treeSym->moreNode = makeTree(treeSym->moreNode, inputSymbol, treeSym->ch);
	else
		treeSym->lessNode = makeTree(treeSym->lessNode, inputSymbol, treeSym->ch);

	return treeSym;

}

void Tree2max(PSYM treeSym,BASE_TYPE *max)
{
	if (treeSym->moreNode)    Tree2max(treeSym->moreNode,max);
	if (treeSym->lessNode)    Tree2max(treeSym->lessNode,max);
	if(*max <treeSym->count)
		*max =treeSym->count;

}
void copyTree2Parr(PSYM treeSym, PSYM*syms)
{
	#define OK 0
	static int i = 0;
	if (treeSym->moreNode)    copyTree2Parr(treeSym->moreNode,syms);
	if (treeSym->lessNode)    copyTree2Parr(treeSym->lessNode,syms);
	if(treeSym->count>0)
	{
		syms[i] = treeSym;
		i++;//syms++;
	}
}
int printArrayForCheck(PSYM*psyms)
{
	int count = 0;
	FILE *fpcheck = fopen("checkArray.xls", WRITE);
	if (!fpcheck)
	{
		printf("ERROR of opened fpcheck");
		return fall;
	}
	fprintf(fpcheck, "%s\t%s\t%s\t%s\t%s\t%s\n", "Input ch", "total", "frequency", "CODE", "lessNode", "moreNode");

	for (int i = 0;psyms[i] != NULL;i++)
	{
		fprintf(fpcheck, "%u\t%u\t%f\t%s\t%p\t%p\n", psyms[i]->ch, psyms[i]->count, psyms[i]->freq, psyms[i]->code, psyms[i]->lessNode, psyms[i]->moreNode);
		++count;
	}
	printf("Number of possition =%i\n", count);
	return successfully;
}
int findErrorCopy2psyms(PSYM treeSym, PSYM*syms)// need for search a problem, which were find in check checksum the new array (its must have just copy from treesym)
{
#define OK_ForCheck 0
	static int i = 0;
	if (treeSym->moreNode)    findErrorCopy2psyms(treeSym->moreNode, syms);
	if (treeSym->lessNode)    findErrorCopy2psyms(treeSym->lessNode, syms);
	int j;
	int result;
	int countFind;// calculation of the  find number 
	int lastFindCh;//the last previously found value
	for (lastFindCh= - 1, result = -1, j = (MAXSYMB - 1), countFind = 0;j >= 0;j--)//initialisation with wrong datas
	{
		if ((result = (treeSym->ch - syms[j]->ch)) == OK_ForCheck)
		{
			if (countFind==0) lastFindCh = i;
			countFind +=1;
		}
		if (countFind > 1)//check double record some data for the one letter, each number ch is only one
		{
			printArrayForCheck(syms);
			return i;// return position in array where were find double of matter
		}
	}

	j = -1;i++;syms++;
}
int checkDataParray(PSYM *syms, ULL totalCount, PSYM treeSym)// check data in new array and  checksum 
{
	int result = -1;
	ULL checkCount = 0;
	int i;
	
	for (i = 0;syms[i]!=0;i++)
	{
		checkCount +=syms[i]->count;//*(syms->count);
	}
	if (totalCount == checkCount)
		return successfully;// it's ok and number of leters in Tree = number of letter in input's file  

	else
	{
		result = findErrorCopy2psyms(treeSym, syms);// function for search and demonstration of double of data 
		return fall;// it's bad becouse data of Psyms different from data input Tree 
	
	}
}
int sort(PSYM *psym,BASE_TYPE maxCount)//head will be maxCount and at bottom line the less elemetnt
{
	int j = 0;
	#define errorSort 0;
    #define ok 1;
	long int result = 0;
	PSYM pbuf=0x0;// place(buffer) from will be change
	int numberTo = 0;
	for (ULL currentMaxCount = maxCount;currentMaxCount != 0;currentMaxCount--)//
	{
		
		for (int numberFrom=numberTo;psym[numberFrom]!=NULL;numberFrom++)
		{
			if (psym[numberFrom]->count == currentMaxCount)
			{
				if (numberTo > 0)
				{
					result = (psym[numberTo-1]->count - psym[numberFrom]->count);
					if (result < 0)// check matter in a previously with the current. we must have previously count< find current count
						return errorSort;
				}
				pbuf = psym[numberTo];
				psym[numberTo] = psym[numberFrom];
				psym[numberFrom]= pbuf;
				pbuf = 0x0; numberTo++;
			}

		}
	}
	

	return ok;
}
void makeFrequencyForArray(PSYM *psyms, ULL totalCount,int maxlengthArray)
{
	int i;
	float doubleCount;
	float doubleTotalCount = totalCount;

	for (doubleCount = 0.0,i = 0;i < maxlengthArray;i++)// check summ for frequency
	{
		doubleCount = psyms[i]->count;
		psyms[i]->freq = doubleCount / doubleTotalCount;
	}
}
int checkSumForFrequencyArray(PSYM*psyms)
{
	#define lowlimitAccuracy 0.99
	#define hightlimitAccuracy 1.001
	int i;
	auto float checkSumForFrequency;
	

	checkSumForFrequency = 0.0;
	for (i = 0;psyms[i]!=NULL;i++)// check summ for frequency
		checkSumForFrequency += psyms[i]->freq;
	if (checkSumForFrequency > lowlimitAccuracy && checkSumForFrequency < hightlimitAccuracy)
		return successfully;
		else
			return fall;
}
void printArrayForScreen(PSYM*psyms)
{

	int i;
	int count = 0;
	printf("%s\t%s\t%s\t%s\t%s\n", "Input ch", "frequency", "CODE", "lessNode", "moreNode");

	for ( i = 0;psyms[i]!=NULL;i++)
	{
		printf("%u\t%f\t%s\t%p\t%p\n", psyms[i]->ch, psyms[i]->freq, psyms[i]->code, psyms[i]->lessNode, psyms[i]->moreNode);
		count = i;
	}
	printf("Number of possition =%i\n", count);
}
