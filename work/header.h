#pragma once
#define SIZE 256 // maximalnoe kolichestvo elementov v massivah syms[] i psysms[], a tak zhe maximalnaya dlina prefiksnogo koda
#define NAME 32 // maximalnoe imya faila
#define METKA 3 // dlinna metki
#define FORMAT 8 // max dlinna rashireniya

struct SYM // predstavlenie simvola
{
	unsigned char ch; // ASCII- kod
	float freq; // chasota vstrechaemosti
	unsigned char code[SIZE]; // massiv dlya novogo koda
	struct SYM *left; // levyi potomok v dereve
	struct SYM *right; // pravyi potomok v dereve
};

union CODE 
{
	unsigned char kod; // permennaya dlya zapisi koda v szhatyi fail


	struct 
	{
		unsigned short b1 : 1;
		unsigned short b2 : 1;
		unsigned short b3 : 1;
		unsigned short b4 : 1;
		unsigned short b5 : 1;
		unsigned short b6 : 1;
		unsigned short b7 : 1;
		unsigned short b8 : 1;
	} byte;
};

int checkLabel(FILE *fp_in); // funkciya dlya poiska metki "HFM" v pervyh 3 bitah, kotoraya yavlyaetsya priznakom kompressii
/*vhod: podaetsya vhodnoi fail*/
/*vyhod: vozvrashaet 1 v sluchae nahozhdeniya metki, i 0 v inom sluchae*/

void renaming(char *argv[], int compression, unsigned char name[], int size_name, unsigned char format[], int size_format); // funkciya prednaznachennaya dlya zameny rashireniya faila v zavisimosti ot metki compression
/*vhod: podaetsya ukazatel na imya vhodnogo faila argv[], priznak szhatiya compression, massiv dlya hraneniya imeni name[] i rasshireniya format[], a tak zhe dlinny size_name i size_format etih massivov sootvetstvenno*/
/*vyhod: izmenyaet soderzhimoe massivov name[] i format[]*/

int inputProcessingCompress(struct SYM syms[], int size_syms, int *count_freq, FILE *fp_in); // funkciya zapolnyaet massiv syms[] simvolami iz vhodnogo neszhatogo faila i ih chastotnostyu
/*vhod: podaetsya massiv struktur syms[], razmer massiva syms[] ukazatel na obshee kolichestvo simvolov *count_freq i ukazatel na vhodnoi fail */
/*vyhod: zapolnyaet polya ch i freq massiva syms[] v sootvetstvii s dannymi schitannymi iz faila i vozvrashaet kolichestvo unikalnyh simvolov*/

void symsSort(struct SYM syms [], int size_syms); // funkciya dlya sortirovki massiva syms po ubyvaniyu chastotnosti
/*vhod: podaetsya massiv struktur syms[], i ego razmer size_syms*/
/*vyhod: sortiruet massiv po ubyvaniyu chastnosti syms[].freq */

struct SYM* buildTree(struct SYM *psysms[], int size_psysms); // funkciya dlya postroiki "dereva Haffmana"
/*vhod: podaetsya massiv ukazatelei na struktury *psysms[], i kolichestvo znachashih simvolov v nem size_psysms*/
/*vyhod: stroit derevo Haffmana svyazyvaya ukazateli ieft i right mezhdu soboi, i vozvrahaet ukazatel na ego nachalo */

void makeCodes(struct SYM *root); // funkciya prohodit po derevu Haffmana i poluchaet prefiksnyi kod
/*vhod: podaetsya ukazatel na koren dereva Haffmana*/
/*vyhod: zapolnyaet syms[].code prefiksnym kodom dlya kazhdogo simvola*/

void print101Compress(struct SYM syms[], int count_freq, FILE *fp_in, FILE *fp_101); // funkciya dlya konvertirovaniya simvolov v binarnyi kod
/*vhod: podaetsya massiv struktur syms[], obshee kolichestvo simvolov count_freq, ukazatel na vhodnoi fail i fail dlya vremennogo hraneniya koda*/
/*vyhod: zapisyvaet znacheniya syms[].code v vspomogatelnyi fail temp.101 kotorye sootvetstvuyut znacheniyam simvolov v vhodnom faile */

void headerWrite(struct SYM syms[], int size_alphabet, unsigned char format[], int *size_code, int *tail, FILE *fp_101, FILE *fp_out); // funkciya dlya podsheta simvolov v temp.101 i zapisi zagolovka v vyhodnoi szhatyi fail
/*vhod: podaetsya massiv struktur syms[], kolichestvo unikalnyh simvolov size_alphabet, massiv s rasshireniem ishodnogo faila format[], ukazatel na kolichestvo simolov v temp.101 *size_code i kolichestvo simvolov ne kratnyh 8 *tail, i ukazatel na fail dlya vremennogo hraneniya koda i vyhodnoi szhatyi fail* /
/*vyhod: formiruet zagolovok szhatogo vyhodnogo faila */

void symbolBin (int size_code, int tail, FILE *fp_101, FILE *fp_out); // dlya zapisi koda temp.101 v baity vyhodnogo faila
/*vhod: podaetsya kolichestvo simolov v temp.101 size_code, kolichestvo simvolov ne kratnyh 8 tail, ukazatel na fail dlya vremennogo hraneniya koda i vyhodnoi szhatyi fail */
/*vyhod: zapisyvaet '0' i '1' temp.101  v vyhodnoi szhatyi fail v vide baitov*/

void headerRead(struct SYM syms[], int size_format, int *size_alphabet, unsigned char format[], int *size_code, int *tail, FILE *fp_in);// funkciya dlya chteniya zagolovka iz szhatogo faila
/*vhod: podayutsya massiv struktur syms[], maksimalnaya dlinna rashireniya size_format, ukazatel na kolichestvo unikalnyh simvolov *size_alphabet, massiv dlya zapisi rasshireniya iz zagolovka format[], ukazatel na razmer koda v temp.101 *size_code i kolichestvo simvolov ne kratnyh 8 *tail i ukazatel na vhodnoi szhatyi fail*/
/*vyhod: schityvaet zagolovok szhatogo faila, zapolnyaet massiv struktur syms[] i zapisyvaet informaciyu iz zagolovka v peremennye*/

void binSymbol(int size_code, int tail, FILE *fp_in, FILE *fp_101); // dlya zapisi baitov vhodnogo faila v kod temp.101
/*vhod: podaetsya kolichestvo simolov v temp.101 size_code, kolichestvo simvolov ne kratnyh 8 tail, ukazatel na vhodnoi szhatyi fail i fail dlya vremennogo hraneniya koda*/
/*vyhod: zapisyvaet baity vhodnogo szhatogo faila v vide '0' i '1' v temp.101 */

void pathCodes(struct SYM *root, int size_code, FILE *fp_101, FILE *fp_out); // funkciya dlya obhoda binarnogo dereva i polucheniya simvola iz koda
/*vhod: podaetsya ukazatel na koren dereva Haffmana i ukazateli na fail dlya vremennogo hraneniya koda i vyhodnoi fail*/
/*vyhod: zapolnyaet vyhodnoi fail raskodirovannymi simvolami*/
