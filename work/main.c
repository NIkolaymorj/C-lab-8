#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int main(int argc, char *argv[])
{


	FILE *fp_in; // ukazatel faila dlya chteniya
	FILE *fp_101; // ukazatel na zakodirovannyi fail
	FILE *fp_out; // ukazatel faila dlya zapisi

	struct SYM syms[SIZE] = { 0 }; // massiv iz struktur
	int syms_position = 0; // poziciya v massive syms

	struct SYM *psysms[SIZE] = { NULL }; // massiv ukazatelei na struktury
	int psysms_position = 0; // poziciya v massive psysms

	int count_freq = 0; // dlya hraneniya obshego kolichestva simvolov
	int size_alphabet = 0; // kolichestvo unikalnyh simvolov 
	int size_code = 0; // kolichestvo simvolov v promezhutochnom faile temp.101
	int tail = 0; // ostatok v faile temp.101 ne kratnyi 8

	unsigned char name[NAME + 1] = { 0 }; // massiv dlya hraneniya imeni faila
	unsigned char format[FORMAT + 1] = { 0 }; // dlya zapisi formata faila
	

	int compression = 0; //priznak kompressii (1 - szhat, 0 - ne szhat)

	if (argc < 2)
	{
		printf("Use the program so: <%s + faile_name>\n", argv[0]); // esli ne vveden fail v komandnoi stroke
		system("pause");
		exit(EXIT_FAILURE);
	}

	/*otkrytie faila dlya chteniya v dvoichnom rezhime*/
	printf("Opening the input file for reading <%s>\n", argv[1]);
	fp_in = fopen(argv[1], "rb"); // imya dlya argv pishetsya bez kavychek!!!

	/*proverka otkrytiya faila dlya chteniya*/
	if (fp_in == NULL)
	{
		printf("Can not open file <%s>\n", argv[1]);
		system("pause");
		exit(EXIT_FAILURE);
	}
	printf("File <%s> is open \n", argv[1]);


	/*proverka szhatyi ili net fail*/
	compression = checkLabel(fp_in); 

	////////////////////////////////////////////////////////////////////////////////nachalo kompressii/////////////////////////////////////////////////////////////////////////////////////////////

	if (compression == 0)
	{
		/*Schityvaem imya faila i menyaem rashirenie*/
		renaming(argv, compression, name, (NAME + 1), format, (FORMAT + 1));
		/*proverit (NAME + 1) i (FORMAT + 1) */

		/*obrabotka vvoda*/
		size_alphabet = inputProcessingCompress(syms, SIZE, &count_freq /*&ptr_count_freq*/, fp_in);
		printf("Number of unique characters: %d\n", size_alphabet);
		printf("Total number of characters: %d\n", count_freq);

		/* pri size_alphabet oshibka v buildTree, poetomu zashita vvoda*/
		if (size_alphabet < 2)
		{
			printf("File <%s> can not be compressed\n", argv[1]);
			system("pause");
			exit(EXIT_FAILURE);
		}


		/*sortiruem massiv structur po ubyvaniy*/
		symsSort(syms, SIZE);

		/*pechat na ekran dlya proverki unikalnyh vstretivshihsya simvolov  + svyazyvanie massivov syms[] i *psysms[] */
		printf("\nPrinting on the screen of unique symbols and their frequency:\n");
		for (syms_position = 0; syms_position < size_alphabet; syms_position++) // ne vychitat 1
			{
				psysms[syms_position] = &syms[syms_position];
				printf(" %c - %lf\n", syms[syms_position].ch, syms[syms_position].freq);
			}
		printf("End printing of unique characters\n");

		/* vyzov funkcii postroeniya dereva */
		printf("\nBegin to build the Huffman's tree\n");
		struct SYM *root = buildTree(psysms, size_alphabet); 
		printf("Finished building the Huffman's tree\n");

		/* vyzov funkcii polucheniya koda*/
		printf("Begin to generate the code for the characters\n");
		makeCodes(root);
		printf("Finished to generate code for characters\n");


		/*Vyvod koda dlya simvolov*/
		printf("\nPrinting on the screen of unique symbols and their codes:\n");
		for (syms_position = 0; syms_position < size_alphabet; syms_position++)
				printf(" %c coded as %s\n", syms[syms_position].ch, syms[syms_position].code);
		printf("End printing of codes\n");

		/*Otkryte vspomogatelnogo kodovogo faila dlya zapisi v dvoichnom rezhime*/
		printf("\nOpening the temp file for write <%s>\n", "temp.101");
		fp_101 = fopen("temp.101", "wb");

		/*proverka otkrytiya kodovogo faila dlya zapisi*/
		if (fp_101 == NULL)
		{
			printf("Can not open file <%s>\n", "temp.101");
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is open \n", "temp.101" );

		/*Zapis '0' i '1' v temp*/
		print101Compress(syms, count_freq, fp_in, fp_101);

		/*zakrytie i proverka zakrytiya faila dlya chteniya*/
		if (fclose(fp_in) != 0) // fclose pri uspeshom zakrytii vozrashaet 0
		{
			printf("Can not close file <%s>\n", argv[1]);
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is closed \n", argv[1]);

		/*zakrytie i proverka zakrytiya vspomogatelnogo kodovogo faila dlya zapisi*/
		if (fclose(fp_101) != 0) // fclose pri uspeshom zakrytii vozrashaet 0
		{
			printf("Can not close file <%s>\n", "temp.101");
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is closed \n", "temp.101");

		/*Otkryte vspomogatelnogo kodovogo faila dlya chteniya v dvoicnom rezhime*/
		printf("Opening the temp file <%s> for read \n", "temp.101");
		fp_101 = fopen("temp.101", "rb");

		/*proverka otkrytiya kodovogo faila dlya chteniya*/
		if (fp_101 == NULL)
		{
			printf("Can not open file <%s>\n", "temp.101");
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is open\n", "temp.101");

		/*otkrytie szhatogo faila dlya zapisi v dvoichnom rezhime*/
		printf("Opening the output file <%s>\n", name);
		fp_out = fopen(name, "wb");


		/*proverka otkrytiya szhatogo faila dlya zapisi v dvoichnom rezhime*/
		if (fp_out == NULL)
		{
			printf("Can not open file <%s>\n", name);
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is open\n", name);

		/*Zapis zagolovka v vyhodnoi fail*/
		headerWrite(syms, size_alphabet, format, &size_code, &tail, fp_101, fp_out);

		/*Zapis "tela" vyhodnogo faila*/
		symbolBin(size_code, tail, fp_101, fp_out);

		/*zakrytie i proverka zakrytiya kodovogo faila dlya chteniya*/
		if (fclose(fp_101) != 0) // fclose pri uspeshom zakrytii vozrashaet 0
		{
			printf("Can not close file <%s>\n", "temp.101");
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is closed \n", "temp.101");


		/*zakrytie i proverka zakrytiya szhatogo faila  dlya zapisi v dvoichnom rezhime*/
		if (fclose(fp_out) != 0) // fclose pri uspeshom zakrytii vozrashaet 0
		{
			printf("Can not close file <%s>\n", name);
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is closed \n", name);

		printf("\nDone. Fail <%s> was compressed. The name of the compressed file is <%s> \n",argv[1] , name);
	}

	////////////////////////////////////////////////////////////////////////////////////konec kompressii//////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////nachalo dekompressii//////////////////////////////////////////////////////////////////////////////////////////
	
	else if (compression == 1)
	{
		/*Schityvaem zagolovok iz vhodnogo faila*/
		headerRead(syms, FORMAT, &size_alphabet, format, &size_code, &tail, fp_in);

		/*Vyvod na ekran simvolov iz tablicy vstrechaemosti*/
		printf("\nPrinting on the screen of unique symbols and their frequency:\n");
		for (syms_position = 0; syms_position < size_alphabet; syms_position++)
			if (syms[syms_position].freq > 0)  // isklyuchenie simvolov ne vstrechayushihsya v faile (zdes ne nuzhno)
			{
				printf(" %c - %lf\n", syms[syms_position].ch, syms[syms_position].freq);
				psysms[syms_position] = &syms[syms_position]; // svyazyvaem massiv struktur i massiv ukazatelei
			}
		printf("End printing of unique characters\n\n");



		/*Schityvaem imya faila i menyaem rashirenie*/
		renaming(argv, compression, name, (NAME + 1), format, (FORMAT + 1));


		/*Otkryte vspomogatelnogo kodovogo faila dlya zapisi v dvoichnom rezhime*/
		printf("Opening the temp file for write <%s>\n", "temp.101"); 
		fp_101 = fopen("temp.101", "wb");

		/*proverka otkrytiya kodovogo faila dlya zapisi*/
		if (fp_101 == NULL)
		{
			printf("Can not open file <%s>\n", "temp.101");
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is open\n", "temp.101");

		/*Schityvaem "telo" vhodnogo faila*/
		binSymbol(size_code, tail, fp_in, fp_101);

			/*zakrytie i proverka zakrytiya kodovogo faila dlya zapisi*/
		if (fclose(fp_101) != 0) // fclose pri uspeshom zakrytii vozrashaet 0
		{
			printf("Can not close file <%s>\n", "temp.101");
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is closed \n", "temp.101");

		/*Otkryte vspomogatelnogo kodovogo faila dlya chteniya v tekstovom rezhime*/
		printf("Opening the temp file for read <%s>\n", "temp.101");
		fp_101 = fopen("temp.101", "rb");

		/*proverka otkrytiya kodovogo faila dlya chteniya*/
		if (fp_101 == NULL)
		{
			printf("Can not open file <%s>\n", "temp.101");
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is open \n", "temp.101");

		/*otkrytie decompressionnogo faila dlya zapisi v tekstovom rezhime*/
		printf("Opening the output file for write <%s>\n", name);
		fp_out = fopen(name, "wb");

		/*proverka otkrytiya decompressionnogo faila  dlya zapisi*/
		if (fp_out == NULL)
		{
			printf("Can not open file <%s>\n", name);
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is open \n", name);

		/* vyzov funkcii postroeniya dereva */
		printf("Begin to build the Huffman's tree\n");
		struct SYM *root = buildTree(psysms, size_alphabet);
		printf("Finished building the Huffman's tree\n");

		/* vyzov funkcii polucheniya simvolov iz koda */
		printf("Start to convert the code into characters\n");
		pathCodes(root, size_code, fp_101, fp_out);
		printf("Finished to convert the code into characters\n");

		/*zakrytie i proverka zakrytiya vspomogatelnogo kodovogo faila dlya chteniya*/
		if (fclose(fp_101) != 0) // fclose pri uspeshom zakrytii vozrashaet 0
		{
			printf("Can not close file <%s>\n", "temp.101");
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is closed \n", "temp.101");

		/*zakrytie i proverka zakrytiya decompressionnogo faila  dlya zapisi v tekstovom rezhime*/
		if (fclose(fp_out) != 0) // fclose pri uspeshom zakrytii vozrashaet 0
		{
			printf("Can not close file <%s>\n", name);
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("File <%s> is closed \n", name);

		printf("\nDone. Fail <%s> was decompressed. The name of the decompressed file is <%s> \n", argv[1], name);
		
		//////////////////////////////////////////////////////////////////////////////////konec dekompressii//////////////////////////////////////////////////////////////////////////////////////////

	}
	system("pause");
	return 0;
}