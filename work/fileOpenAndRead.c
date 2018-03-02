#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "Header.h"

extern char SIGNATURE[];


unsigned char * fileOpenAndRead(char *fname, long int *countByte)
{

	FILE *fp;
	fp = fopen(fname, "rb");
	if (fp == NULL) //Проверка наличия файла и его открытия
	{
		printf("Error open file in\n");
		return -1;
	}
	//------------Проверка на формат файла---------------------------//
	unsigned char * buf = (char*)calloc(strlen(SIGNATURE), sizeof(char));
	fread(buf, sizeof(unsigned char), strlen(SIGNATURE), fp);
	if (testFormat(buf, &(SIGNATURE), strlen(SIGNATURE)) == 0)
	{	//Если добавочный формат (сигнатура) прописана в файле, то его сжатие не возможно и приведёт к потере данных
		printf("This file is already compressed, re-compression is not possible and will result in loss of data!!!");
		return -1;
	}
	free(buf);	//освобождаем память из под буферной переменной
	fseek(fp, 0, SEEK_END);	//перемещаем позицию указателя в потоке на конец файла
	long int sizeFile = ftell(fp);	// получаем размер в байтах
	long int RAM = sizeFile + (int)(sizeFile / 100 * 10);	//Увеличиваем количество выделяемой памяти на случай
	fseek(fp, 0, SEEK_SET);	//перемещаем позицию указателя в потоке на начало файла
	unsigned char *fileByte = (unsigned char*)malloc(RAM * sizeof(unsigned char));
	if (fileByte == NULL)
	{
		fputs("Error RAM\n", stderr);
		return -1;
	}

	long int countByteRead = 0;	//Количество реально считанных байт

	countByteRead = fread(fileByte, sizeof(unsigned char), RAM, fp);	// считываем файл в буфер

	if (sizeFile == countByteRead)
	{
		printf("Read file end\n");
	}
	
	if (countByteRead != sizeFile)
	{
		fputs("Error read file\n", stderr);
		exit(3);
	}
	if (fclose(fp) == EOF)
	{
		printf("Error close file in\n");
		return -1;
	}
	(*countByte) = countByteRead;	//Записываем количество прочитанных и записанных байт
	return fileByte;
}