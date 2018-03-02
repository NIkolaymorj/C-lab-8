#include "Header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Функция распаковки
char * unPack(unsigned char buf[], unsigned char ch)
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
	return buf;
}

int scanCode(char *str, int sizeArrSyms, int * lenCode, int * indexSyms)
{
	int i;
	int flag = 0;
	int len = 0;
	for (i = 0; i < sizeArrSyms - 1 && str[i] != '\0'; i++)
	{
		if (str[i] == '1')
		{
			(*lenCode) = i + 1;	//Длинна кода зашифрованного символа
			(*indexSyms) = i;	//Индекс элемента структуры
			flag = 1;	//Элемент найден
			return 1;	
		}
	}
	if (str[i - 1] == '0' && flag == 0)
	{
		(*lenCode) = i;	//Длинна кода зашифрованного символа
		(*indexSyms) = i;	//Индекс элемента структуры
		flag = 1;
		return 1;

	}
	else
	{
		return 0;
	}

}

extern char SIGNATURE[];


int unPackFile(char * fname)
{
	char fin[256] = { 0 };
	strcpy(fin, fname);

	char signatura[10] = { 0 };	//Подпись формата
	int sizeArrSyms = 0;	//Количество уникальных символов в файле
	FILE *fpIn;
	fpIn = fopen(fin, "rb");
	if (fpIn == NULL) //Проверка наличия файла и его открытия
	{
		printf("Error open file in\n");
		return -1;
	}
	
	fread(&signatura, sizeof(unsigned char), strlen(SIGNATURE), fpIn);	//Подпись формата
	if(testFormat(&signatura, &(SIGNATURE), strlen(SIGNATURE)) != 0)	//Если подпись формата не верна декомпресия файла невозможна
	{
		printf("Decompression of this file is not possible!!!");
		return 1;
	}

	fread(&sizeArrSyms, sizeof(int), 1, fpIn);
	struct SYM *syms = (struct SYM*)calloc(sizeArrSyms, sizeof(struct SYM));	//Массив структур - таблица встречаемости
	for (int i = 0; i < sizeArrSyms; i++)
	{
		fread(&(syms[i].ch), sizeof(char), 1, fpIn);	//Читаем код символа 
		fread(&(syms[i].freq), sizeof(float), 1, fpIn);	//Читаем частоту встречаемости символа 
	}
	int extraBits;	//Количество бит остатка
	fread(&(extraBits), sizeof(int), 1, fpIn);	//Читаем частоту встречаемости символа 

	long int countByte;
	fread(&(countByte), sizeof(long int), 1, fpIn);	//Считываем количество символов в исходном файле в файл .pack
	struct SYM **psyms = (struct SYM **)calloc((sizeArrSyms + sizeArrSyms - 2), sizeof(struct SYM *));	//Выделяем память под массив указателей на структуры SYM
	for (int i = 0; i < sizeArrSyms; i++)
	{
		psyms[i] = &(syms[i]);
	}
	struct SYM * root = buildTree(psyms, sizeArrSyms);	//Строим дерево
	makeCodes(root);	//Проставляем префиксные коды
					

	unsigned long int poz = 0;
	unsigned char bufByte = 0;
	unsigned char * str_101 = (unsigned char *)calloc(sizeArrSyms * countByte, sizeof(char));	//Буферная строка для кода 101 в строковом представлении
	if (str_101 == NULL)
	{
		printf("Error RAM\n");
		return -1;
	}
	while (!feof(fpIn))
	{
		fread(&(bufByte), sizeof(char), 1, fpIn);	//Читаем код символа 
	
		unPack(&(str_101[poz]), bufByte);
		poz += 8;
	}
	//Затирание хвоста
	for (int i = 0; i < (8 - extraBits); i++)
	{
		str_101[poz - 1] = '\0';
		poz -= 1;
	}
	
	//Закрытие файла
	if (fclose(fpIn) == EOF)
	{
		printf("Error close file in\n");
	}
	//----------ОТКРЫВАЕМ РЕЗУЛЬТИРУЮЩИЙ ФАЙЛ---------------
	//--Стираем дополнительное расширение если оно сохранилось
	if (strcmp(SIGNATURE, &(fin[strlen(fin) - strlen(SIGNATURE)])) == 0)
	{
		for (int i = 0; i < strlen(SIGNATURE); i++)
		{
			fin[strlen(fin) - 1] = '\0';
		}
	}
	FILE *fpOut;
	fpOut = fopen(fin, "wb");
	//---------------------------------------------------------

	unsigned long int countCharOutFile = 0;	//Количество записанных символов

	for (int i = 0; countCharOutFile < countByte; countCharOutFile++)
	{
		int lenCode = 0;
		int ind = 0;
		scanCode(&(str_101[i]), sizeArrSyms, &lenCode, &(ind));
		
		
		fwrite(&(syms[ind].ch), sizeof(char), 1, fpOut);	//Записываем байты в выходной файл
		i += lenCode;
	}
	if (countCharOutFile == countByte)
	{
		printf("Decompression of the compressed file was successful!!!\n");
	}
	//Закрытие файла
	if (fclose(fpOut) == EOF)
	{
		printf("Error close file in\n");
	}
	free(str_101);	//Очистка выделенной динамической памяи под код формата 101
	return 0;
}

