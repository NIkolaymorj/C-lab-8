#include <stdio.h>
#include <stdlib.h>
#include "header.h"


int checkLabel(FILE *fp_in)
{
	unsigned char metka[METKA + 1] = { 0 }; // massiv dlya proverki metki
	unsigned char proverka_metki[] = { "HFM" }; // massiv s oderzhashii metku
	int cmp = 0; 

	printf("Check the file compression\n");
	
	fread(&metka, sizeof(unsigned char), 3, fp_in);
	cmp = strcmp(proverka_metki, metka); // sravnivaem dva massiva

	if (cmp == 0)
	{
		printf("Label is <%s>, file is compressed\n", metka);
		return 1;
	}

	else if (cmp != 0)
	{
		printf("Label is <%s>, file is not compressed\n", metka);
		return 0;
	}

}

void renaming(char *argv[], int compression, unsigned char name[], int size_name, unsigned char format[], int size_format)
{
	int name_position = 0; // poziciya v massive name
	int format_position = 0; // poziciya v massive format


	strncpy(name, argv[1], size_name); // schityvaem imya faila vmeste s '.' i rashireniem
	for (name_position = 0; ((name[name_position] != '.') && (name[name_position] != '\0')); name_position++);


	if ((compression == 0) && (name[name_position] == '.')) // esli fail ne szhat i dostigli '.'
	{
		printf("Prepare the name for the compressed file\n");

		for (name_position++; ((format_position < size_format) && (name_position < size_name) && (name[name_position] != '\0')); )
		{
			format[format_position] = name[name_position]; // zapisyvaem rashirenie v massiv 
			format_position++;
			name_position++;
			name[name_position - 1] = '\0'; // stiraem prezhnee rashirenie faila posle ego schityvaniya
		}
		strcat(name, "hfm"); // dobavlyaem novoe rasshirenie

		printf("Write the old format <%s>\n", format);
		printf("The name for the compressed file will be <%s>\n", name);

		return;
	}

	else if ((compression == 1) && (name[name_position] == '.')) // esli fail szhat i dostigli '.'
	{
		printf("Prepare the name for the decompressed file\n");

		for (name_position++; (name_position < size_name); name_position++)
			name[name_position] = '\0'; // stiraem prezhnee rashirenie faila

		strcat(name, format); //dobavlyaem zapisannoe iz zagolovka rasshirenie

		printf("The name for the decompressed file will be <%s>\n", name);

		return;
	}

	else
		return;
}

int inputProcessingCompress(struct SYM syms[], int size_syms, int *count_freq, FILE *fp_in)
{
	unsigned char character = 0; // dlya vremennogo hraneniya simvolov
	int syms_position = 0; // poziciya v massive syms
	int size_alphabet = 0; // kolichestvo unikalnyh simvolov 
	int input_position = 0; // peremennaya associirovannaya s poziciei v vhodnom faile
	printf("Start input processing\n");

	rewind(fp_in); // peremotka na nachalo posle proverki metki

	while (!feof(fp_in)) // podschet obshego kolichestva simvolov
	{
		fread(&character, sizeof(unsigned char), 1, fp_in);
		(*count_freq)++;
	}
		rewind(fp_in); // peremotka na nachalo posle podscheta simvolov

		(*count_freq)--; // feof daet rezultat v +1 lishnii simvol

	while (input_position < (*count_freq)) // '<' s uchetom togo chto input_position inckrementiruetsya s 0
	{
		fread(&character, sizeof(unsigned char), 1, fp_in);
		syms_position = (int)character;
		syms[syms_position].ch = (int)character;
		if (syms[syms_position].freq == 0) //esli pervyi raz vstrechaem simvol
			size_alphabet++;

		syms[syms_position].freq++;
		input_position++;
	}

	for (syms_position = 0; syms_position < (size_syms); syms_position++)
		if (syms[syms_position].freq > 0)  // isklyuchenie simvolov ne vstrechayushihsya v faile
			syms[syms_position].freq = (syms[syms_position].freq / (*count_freq));

	printf("Finish input processing\n");
	return size_alphabet;
}

int compare(const void *a, const void *b) // sravnenie dlya qsort v funkcii symsSort
{
	if (((struct SYM*)a)->freq < ((struct SYM*)b)->freq) // bystraya sortirovka dlya polya struktur
		return 1;
	else
		return -1;
}

void symsSort(struct SYM syms[], int size_syms)
{
	printf("Begin sorting in descending order of frequency\n");

	qsort(syms, size_syms, sizeof(struct SYM), compare); // bibliotechnaya funkciya qsort

	printf("Finish sorting in descending order of frequency\n");

	return;
}

struct SYM *buildTree(struct SYM *psysms[], int size_psysms)
{
	// sozdaem vremennyi uzel
	struct SYM *temp = (struct SYM*)malloc(sizeof(struct SYM));
	// v pole chastoty zapisyvaetsya summa chastot
	// poslednego i predposlednego elementa massiva psym
	temp->freq = (psysms[size_psysms - 2]->freq) + (psysms[size_psysms - 1]->freq);
	// svyazyvaem sozdannyi uzel s dvumya poslednimi uzlami
	temp->left = psysms[size_psysms - 1];
	temp->right = psysms[size_psysms - 2];
	temp->code[0] = 0;
	if (size_psysms == 2) // my sformirovali kornevoi element s chastotoi 1.0
		return temp;

	// dobavlyaem temp v nuzhnuyu poziciyu psysms,
	// sohranyaya poryadok ubyvaniya chastoty

	else
	{
		int begin_position = 0, end_position = 0; // vremennye schetchiki

		for (begin_position = 0; begin_position < size_psysms; begin_position++) // sravnivaem chastotu kazhdogo elementa psym s chastotoi temp
			if ((temp->freq) > (psysms[begin_position]->freq))
			{                               // esli chastota temp bolshe
				for (end_position = size_psysms - 1; end_position > begin_position; end_position--) // "sdvigaem ot nachala" vse elementy (ukazateli na elementy)
					psysms[end_position] = psysms[end_position - 1];  // nacinaya "snizu"

				psysms[begin_position] = temp; // i zapisyavaem v element s menshei chastotoi temp
				break; // !!!
			}
	}

	return buildTree(psysms, size_psysms - 1);
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

void print101Compress(struct SYM syms[], int count_freq, FILE *fp_in, FILE *fp_101)
{
	unsigned char character = 0; // dlya vremennogo hraneniya simvolov
	int syms_position = 0; // poziciya v massive syms
	int input_position = 0; // peremennaya associirovannaya s poziciei v vhodnom faile

	printf("Starting to write code in a file <%s>\n", "temp.101");

	rewind(fp_in); // perehodim na nachalo vhodnogo faila

	while (input_position < count_freq)
	{
		fread(&character, sizeof(unsigned char), 1, fp_in);
		for (syms_position = 0; syms_position < count_freq; syms_position++)
			if (syms[syms_position].ch == character)
			{
				fputs(syms[syms_position].code, fp_101); // vyvodim stroku s kodom
				break;
			}
		input_position++;
	}
	printf("Finished write the code in the file <%s>\n", "temp.101");
	return;
}

void headerWrite(struct SYM syms[],  int size_alphabet, unsigned char format[], int *size_code, int *tail, FILE *fp_101, FILE *fp_out)
{
	unsigned char character = 0; // dlya vremennogo hraneniya simvolov
	unsigned char ch_size_alphabet = 0; // dlya zapisi v zagolovok v char-tipe
	unsigned char ch_tail = 0; // dlya zapisi v zagolovok v char-tipe
	int syms_position = 0; // poziciya v massive syms
	int format_position = 0; // poziciya v massive format

	/*Schitaem kolichestvo simvolov v temp.101*/
	while (!feof(fp_101))
	{
		fread(&character, sizeof(unsigned char), 1, fp_101);
		(*size_code)++;
	}

	(*size_code)--; //feof daet rezultat v +1 lishnii simvol

	*tail = (*size_code) % 8; // ishem ostatok ne kratnyi 8

	printf("The total number of characters in the file <%s> = %d\n", "temp.101", *size_code);
	printf("The number of characters not multiples of 8 in the file <%s> = %d\n", "temp.101", *tail);

	printf("Started write the header in the output fail\n");

	/*Sozdaem zagolovok szhatogo faila*/
	fwrite("HFM", sizeof(unsigned char), 3, fp_out); // metka szhatiya <1>

	ch_size_alphabet = (unsigned char)(size_alphabet - 1); // (size_alphabet - 1) = 255 dlya zapisi v tablice v chare
	fwrite(&ch_size_alphabet, sizeof(unsigned char), 1, fp_out); // unikalnye simvoly <2>

	for (syms_position = 0; syms_position < size_alphabet; syms_position++) // tablica vstrechaemosti <3>
	{
		fwrite(&syms[syms_position].ch, sizeof(unsigned char), 1, fp_out);
		fwrite(&syms[syms_position].freq, sizeof(float), 1, fp_out);
	}

	ch_tail = (char)(*tail); // dlya zapisi v chare
	fwrite(&ch_tail, sizeof(unsigned char), 1, fp_out); // kolichestvo znachashih simvolov v hvoste <4>

	fwrite(&(*size_code), sizeof(int), 1, fp_out); // kolichestvo simvolov v fp_101 dlya proverki <5>

	/* zdes hranitsya razreshenie ishodnogo faila <6>*/
	for (format_position = 0; format[format_position] != '\0'; format_position++); // schitaem kolichestvo znachashih simvolov
	fwrite(format, sizeof(unsigned char), format_position, fp_out); // format pisat bez &

	fwrite("#", sizeof(unsigned char), 1, fp_out); // metka konca zagolovka

	printf("Finished write the header in the output fail\n");

	return;
}

void symbolBin(int size_code, int tail, FILE *fp_101, FILE *fp_out)
{
	int bin[8] = { 0 }; // massiv dlya zapisi v bait  
	int bin_position = 0; // poziciya v bin 
	int temp_position = 0; // peremennaya associirovannaya s poziciei v faile temp.101
	unsigned char character = 0; // dlya vremennogo hraneniya simvolov

	union CODE var; // peremennaya dlay preobrazovaniya koda v baity

	rewind(fp_101); //peremotka na nachalo faila .101

	printf("Starting to write code in the output file\n");

	for (temp_position = 0; (size_code > tail) && (temp_position < (size_code - tail)); temp_position++) // schityvaem poka ne dostignem hvosta  index 1 ili 0 ????
	{
		bin[bin_position] = fgetc(fp_101);

		if (bin_position >= 7)
		{
			var.byte.b1 = bin[0] - '0';
			var.byte.b2 = bin[1] - '0';
			var.byte.b3 = bin[2] - '0';
			var.byte.b4 = bin[3] - '0';
			var.byte.b5 = bin[4] - '0';
			var.byte.b6 = bin[5] - '0';
			var.byte.b7 = bin[6] - '0';
			var.byte.b8 = bin[7] - '0';

			fprintf(fp_out, "%c", var.kod); // pechataem zakodirovannyi simvol
			bin_position = 0;
			continue; // !!!!
		}
		if (temp_position < (size_code - tail)) // dop proverka  bez -1
			bin_position++;
	}

	/*Ostatok ne kratnyi 8*/

	for (bin_position = 0; (bin_position <= 7); bin_position++)
		bin[bin_position] = 0; // zanulyaem massiv

	for (bin_position = 0; bin_position < (tail); bin_position++) //  bez -1
		bin[bin_position] = fgetc(fp_101);

	var.byte.b1 = bin[0] - '0';
	var.byte.b2 = bin[1] - '0';
	var.byte.b3 = bin[2] - '0';
	var.byte.b4 = bin[3] - '0';
	var.byte.b5 = bin[4] - '0';
	var.byte.b6 = bin[5] - '0';
	var.byte.b7 = bin[6] - '0';
	var.byte.b8 = bin[7] - '0';

	fprintf(fp_out, "%c", var.kod);

	printf("Finished to write code in the output file\n");

	return;
}

void headerRead(struct SYM syms[], int size_format, int *size_alphabet, unsigned char format[], int *size_code, int *tail, FILE *fp_in)
{
	unsigned char character = 0; // dlya vremennogo hraneniya simvolov
	unsigned char ch_size_alphabet = 0; // dlya zapisi v zagolovok v char-tipe
	unsigned char ch_tail = 0; // dlya zapisi v zagolovok v char-tipe
	int syms_position = 0; // poziciya v massive syms
	int format_position = 0; // poziciya v massive format

	printf("Started read the header from the input fail\n");

	fread(&ch_size_alphabet, sizeof(unsigned char), 1, fp_in); // schityvaem kolichestvo unikalnyh simvolov
	*size_alphabet = ((int)ch_size_alphabet + 1);

	printf("Number of unique characters: %d\n", *size_alphabet);

	for (syms_position = 0; syms_position < (*size_alphabet); syms_position++) // schityvaem tablicu vstrechaemosti
	{
		fread(&syms[syms_position].ch, sizeof(unsigned char), 1, fp_in);
		fread(&syms[syms_position].freq, sizeof(float), 1, fp_in);
	}

	fread(&ch_tail, sizeof(unsigned char), 1, fp_in); // schityvaem kolichestvo znachashih simvolov v hvoste
	*tail = (int)ch_tail;


	fread(&(*size_code), sizeof(int), 1, fp_in); // obshee kolichestvo simvolov v fp_101 dlya <5>

	printf("The total number of characters in the file <%s> = %d\n", "temp.101", *size_code);
	printf("The number of characters not multiples of 8 in the file <%s> = %d\n", "temp.101", *tail);

	for (format_position = 0; (format_position < size_format); format_position++)
	{
			fread(&format[format_position], sizeof(unsigned char), 1, fp_in);
			if (format[format_position] == '#')
				break;
	}

	format[format_position] = '\0'; // ubiraem '#'

	printf("Finished read the header from the input fail\n");

	return;
}

void binSymbol(int size_code, int tail, FILE *fp_in, FILE *fp_101)
{
	int bin[8] = { 0 }; // massiv dlya zapisi v bait  
	int bin_position = 0; // poziciya v bin 
	int temp_position = 0; // peremennaya associirovannaya s poziciei v faile temp.101
	unsigned char character = 0; // dlya vremennogo hraneniya simvolov

	union CODE var; // peremennaya dlay zapisi koda

	printf("Starting to write byte in the temp file\n");

	for (temp_position = 0; temp_position < (size_code - tail); ) // schityvaem poka ne dostignem hvosta / bez -1
	{
		fread(&character, sizeof(unsigned char), 1, fp_in);
		var.kod = character;

		bin[0] = var.byte.b1;
		bin[1] = var.byte.b2;
		bin[2] = var.byte.b3;
		bin[3] = var.byte.b4;
		bin[4] = var.byte.b5;
		bin[5] = var.byte.b6;
		bin[6] = var.byte.b7;
		bin[7] = var.byte.b8;

		temp_position = temp_position + 8;

		for (bin_position = 0; (bin_position <= 7); bin_position++)
			fprintf(fp_101, "%d", bin[bin_position]);

	}
	for (bin_position = 0; (bin_position <= 7); bin_position++)
		bin[bin_position] = 0; // zanulyaem massiv

	var.kod = fgetc(fp_in);

	if (tail >= 1)
		bin[0] = var.byte.b1;

	if (tail >= 2)
		bin[1] = var.byte.b2;

	if (tail >= 3)
		bin[2] = var.byte.b3;

	if (tail >= 4)
		bin[3] = var.byte.b4;

	if (tail >= 5)
		bin[4] = var.byte.b5;

	if (tail >= 6)
		bin[5] = var.byte.b6;

	if (tail >= 7)
		bin[6] = var.byte.b7;

	if (tail >= 8) // puskai poka budet
		bin[7] = var.byte.b8;

	for (bin_position = 0; ((tail > 0) && (bin_position < tail)); bin_position++) // bez-1
		fprintf(fp_101, "%d", bin[bin_position]);

	printf("Finished to write byte in the temp file\n");

	return;
}

void pathCodes(struct SYM *root, int size_code, FILE *fp_101, FILE *fp_out)
{
	struct SYM *current; //ukazatel na tekushii uzel
	current = root;
	unsigned char path = '\0'; // dlaya hraneniya simvola iz temp.101
	int output_position = 0; //

	for (output_position = 0; output_position < size_code; output_position++) // bez -1
	{
		fread(&path, sizeof(unsigned char), 1, fp_101); // scityvaem simvol
		if ((path == '0') && (current->left)) // esli '0' to left
		{
			current = current->left; // smeshaem ukazatel glubzhe
		}

		if ((path == '1') && (current->right)) //esli '1' to right
		{
			current = current->right; //smeshaem ukazatel glubzhe
		}

		if (((current->left) == NULL) && ((current->right) == NULL)) // esli dostigli lista
		{
			//if (feof(fp_101))
			//	break;

			fwrite(&current->ch, sizeof(unsigned char), 1, fp_out); // vyvodim znachenie simvola hranyashegosya v nem v vyhodnoi fail
			current = root;
		}
	}
	return;
}

