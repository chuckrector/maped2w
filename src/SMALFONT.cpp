
// smalfont.c

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// CHANGELOG:
// <aen, apr 21>
// + added some prototypes to SMALFONT.H
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <stdio.h>

#include "smalfont.h"
#include "vdriver.h"

#define xx 31

int fontx=0;
int fonty=0;

BITMAP* bmpUpperA = 0;
char sbA[]={3,
            00,xx,00,
            xx,00,xx,
            xx,xx,xx,
            xx,00,xx,
            xx,00,xx};

BITMAP* bmpLowerA = 0;
char ssA[]={3,
            00,00,00,
            00,xx,00,
            xx,00,xx,
            xx,xx,xx,
            xx,00,xx};

BITMAP* bmpUpperB = 0;
char sbB[]={3,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00};

BITMAP* bmpLowerB = 0;
char ssB[]={3,
            00,00,00,
            xx,xx,00,
            xx,xx,00,
            xx,00,xx,
            xx,xx,xx};

BITMAP* bmpUpperC = 0;
char sbC[]={3,
            00,xx,xx,
            xx,00,00,
            xx,00,00,
            xx,00,00,
            00,xx,xx};

BITMAP* bmpLowerC = 0;
char ssC[]={3,
            00,00,00,
            00,xx,xx,
            xx,00,00,
            xx,00,00,
            00,xx,xx};

BITMAP* bmpUpperD = 0;
char sbD[]={3,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,xx,00};

BITMAP* bmpLowerD = 0;
char ssD[]={3,
            00,00,00,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx,
            xx,xx,00};

BITMAP* bmpUpperE = 0;
char sbE[]={3,
            xx,xx,xx,
            xx,00,00,
            xx,xx,00,
            xx,00,00,
            xx,xx,xx};

BITMAP* bmpLowerE = 0;
char ssE[]={3,
            00,00,00,
            xx,xx,xx,
            xx,xx,00,
            xx,00,00,
            xx,xx,xx};

BITMAP* bmpUpperF = 0;
char sbF[]={3,
            xx,xx,xx,
            xx,00,00,
            xx,xx,00,
            xx,00,00,
            xx,00,00};

BITMAP* bmpLowerF = 0;
char ssF[]={3,
            00,00,00,
            xx,xx,xx,
            xx,00,00,
            xx,xx,00,
            xx,00,00};

BITMAP* bmpUpperG = 0;
char sbG[]={3,
            00,xx,xx,
            xx,00,00,
            xx,00,xx,
            xx,00,xx,
            00,xx,xx};

BITMAP* bmpLowerG = 0;
char ssG[]={3,
            00,00,00,
            00,xx,xx,
            xx,00,00,
            xx,00,xx,
            00,xx,xx};

BITMAP* bmpUpperH = 0;
char sbH[]={3,
            xx,00,xx,
            xx,00,xx,
            xx,xx,xx,
            xx,00,xx,
            xx,00,xx};

BITMAP* bmpLowerH = 0;
char ssH[]={3,
            00,00,00,
            xx,00,xx,
            xx,00,xx,
            xx,xx,xx,
            xx,00,xx};

BITMAP* bmpUpperI = 0;
char sbI[]={1,
            xx,
            xx,
            xx,
            xx,
            xx};

BITMAP* bmpLowerI = 0;
char ssI[]={1,
            00,
            xx,
            xx,
            xx,
            xx};

BITMAP* bmpUpperJ = 0;
char sbJ[]={3,
            00,00,xx,
            00,00,xx,
            00,00,xx,
            xx,00,xx,
            00,xx,00};

BITMAP* bmpLowerJ = 0;
char ssJ[]={3,
            00,00,00,
            00,00,xx,
            00,00,xx,
            xx,00,xx,
            00,xx,00};

BITMAP* bmpUpperK = 0;
char sbK[]={3,
            xx,00,xx,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx};

BITMAP* bmpLowerK = 0;
char ssK[]={3,
            00,00,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx};

BITMAP* bmpUpperL = 0;
char sbL[]={3,
            xx,00,00,
            xx,00,00,
            xx,00,00,
            xx,00,00,
            xx,xx,xx};

BITMAP* bmpLowerL = 0;
char ssL[]={3,
            00,00,00,
            xx,00,00,
            xx,00,00,
            xx,00,00,
            xx,xx,xx};

BITMAP* bmpUpperM = 0;
char sbM[]={5,
            xx,00,00,00,xx,
            xx,xx,00,xx,xx,
            xx,00,xx,00,xx,
            xx,00,00,00,xx,
            xx,00,00,00,xx};

BITMAP* bmpLowerM = 0;
char ssM[]={5,
            00,00,00,00,00,
            xx,00,00,00,xx,
            xx,xx,00,xx,xx,
            xx,00,xx,00,xx,
            xx,00,00,00,xx};

BITMAP* bmpUpperN = 0;
char sbN[]={4,
            xx,00,00,xx,
            xx,xx,00,xx,
            xx,00,xx,xx,
            xx,00,00,xx,
            xx,00,00,xx};

BITMAP* bmpLowerN = 0;
char ssN[]={4,
            00,00,00,00,
            xx,00,00,xx,
            xx,xx,00,xx,
            xx,00,xx,xx,
            xx,00,00,xx};

BITMAP* bmpUpperO = 0;
char sbO[]={3,
            00,xx,00,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            00,xx,00};

BITMAP* bmpLowerO = 0;
char ssO[]={3,
            00,00,00,
            00,xx,00,
            xx,00,xx,
            xx,00,xx,
            00,xx,00};

BITMAP* bmpUpperP = 0;
char sbP[]={3,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,00,
            xx,00,00};

BITMAP* bmpLowerP = 0;
char ssP[]={3,
            00,00,00,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,00};

BITMAP* bmpUpperQ = 0;
char sbQ[]={4,
            00,xx,xx,00,
            xx,00,00,xx,
            xx,00,00,xx,
            xx,00,xx,00,
            00,xx,00,xx};

BITMAP* bmpLowerQ = 0;
char ssQ[]={3,
            00,00,00,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            00,xx,xx};

BITMAP* bmpUpperR = 0;
char sbR[]={3,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx,
            xx,00,xx};

BITMAP* bmpLowerR = 0;
char ssR[]={3,
            00,00,00,
            xx,xx,00,
            xx,00,xx,
            xx,xx,00,
            xx,00,xx};

BITMAP* bmpUpperS = 0;
char sbS[]={3,
            00,xx,xx,
            xx,00,00,
            00,xx,00,
            00,00,xx,
            xx,xx,00};

BITMAP* bmpLowerS = 0;
char ssS[]={3,
            00,00,00,
            00,xx,xx,
            xx,00,00,
            00,xx,xx,
            xx,xx,00};

BITMAP* bmpUpperT = 0;
char sbT[]={3,
            xx,xx,xx,
            00,xx,00,
            00,xx,00,
            00,xx,00,
            00,xx,00};

BITMAP* bmpLowerT = 0;
char ssT[]={3,
            00,00,00,
            xx,xx,xx,
            00,xx,00,
            00,xx,00,
            00,xx,00};

BITMAP* bmpUpperU = 0;
char sbU[]={3,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,xx,xx};

BITMAP* bmpLowerU = 0;
char ssU[]={3,
            00,00,00,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,xx,xx};

BITMAP* bmpUpperV = 0;
char sbV[]={3,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            00,xx,00};

BITMAP* bmpLowerV = 0;
char ssV[]={3,
            00,00,00,
            xx,00,xx,
            xx,00,xx,
            xx,00,xx,
            00,xx,00};

BITMAP* bmpUpperW = 0;
char sbW[]={5,
            xx,00,00,00,xx,
            xx,00,00,00,xx,
            xx,00,xx,00,xx,
            xx,xx,00,xx,xx,
            xx,00,00,00,xx};

BITMAP* bmpLowerW = 0;
char ssW[]={5,
            00,00,00,00,00,
            xx,00,00,00,xx,
            xx,00,xx,00,xx,
            xx,xx,00,xx,xx,
            xx,00,00,00,xx};

BITMAP* bmpUpperX = 0;
char sbX[]={3,
            xx,00,xx,
            xx,00,xx,
            00,xx,00,
            xx,00,xx,
            xx,00,xx};

BITMAP* bmpLowerX = 0;
char ssX[]={3,
            00,00,00,
            xx,00,xx,
            00,xx,00,
            xx,00,xx,
            xx,00,xx};

BITMAP* bmpUpperY = 0;
char sbY[]={3,
            xx,00,xx,
            xx,00,xx,
            00,xx,00,
            00,xx,00,
            00,xx,00};

BITMAP* bmpLowerY = 0;
char ssY[]={3,
            00,00,00,
            xx,00,xx,
            xx,00,xx,
            00,xx,00,
            00,xx,00};

BITMAP* bmpUpperZ = 0;
char sbZ[]={3,
            xx,xx,xx,
            00,00,xx,
            00,xx,00,
            xx,00,00,
            xx,xx,xx};

BITMAP* bmpLowerZ = 0;
char ssZ[]={2,
            00,00,
            xx,xx,
            00,xx,
            xx,00,
            xx,xx};

BITMAP* bmp1 = 0;
char s1[]={1,
           xx,
           xx,
           xx,
           xx,
           xx};

BITMAP* bmp2 = 0;
char s2[]={3,
           xx,xx,xx,
           00,00,xx,
           xx,xx,xx,
           xx,00,00,
           xx,xx,xx};

BITMAP* bmp3 = 0;
char s3[]={3,
           xx,xx,xx,
           00,00,xx,
           00,xx,xx,
           00,00,xx,
           xx,xx,xx};

BITMAP* bmp4 = 0;
char s4[]={3,
           xx,00,xx,
           xx,00,xx,
           xx,xx,xx,
           00,00,xx,
           00,00,xx};

BITMAP* bmp5 = 0;
char s5[]={3,
           xx,xx,xx,
           xx,00,00,
           xx,xx,xx,
           00,00,xx,
           xx,xx,xx};

BITMAP* bmp6 = 0;
char s6[]={3,
           xx,xx,xx,
           xx,00,00,
           xx,xx,xx,
           xx,00,xx,
           xx,xx,xx};

BITMAP* bmp7 = 0;
char s7[]={3,
           xx,xx,xx,
           00,00,xx,
           00,00,xx,
           00,00,xx,
           00,00,xx};

BITMAP* bmp8 = 0;
char s8[]={3,
           xx,xx,xx,
           xx,00,xx,
           xx,xx,xx,
           xx,00,xx,
           xx,xx,xx};

BITMAP* bmp9 = 0;
char s9[]={3,
           xx,xx,xx,
           xx,00,xx,
           xx,xx,xx,
           00,00,xx,
           xx,xx,xx};

BITMAP* bmp0 = 0;
char s0[]={3,
           xx,xx,xx,
           xx,00,xx,
           xx,00,xx,
           xx,00,xx,
           xx,xx,xx};

BITMAP* bmpQuote = 0;
char sQuote[]={3,
               xx,00,xx,
               xx,00,xx,
               00,00,00,
               00,00,00,
               00,00,00};

BITMAP* bmpYow = 0;
char sYow[]={1,
             xx,
             xx,
             xx,
             00,
             xx};

BITMAP* bmpQuotes = 0;
char sQuotes[]={1,
                xx,
                xx,
                00,
                00,
                00};


BITMAP* bmpComma = 0;
char sComma[]={2,
               00,00,
               00,00,
               00,00,
               00,xx,
               xx,00};


BITMAP* bmpPeriod = 0;
char sPeriod[]={1,
                00,
                00,
                00,
                00,
                xx};

BITMAP* bmpMinus = 0;
char sMinus[]={2,
               00,00,
               00,00,
               xx,xx,
               00,00,
               00,00};

BITMAP* bmpQuest = 0;
char sQuest[]={3,
               xx,xx,xx,
               00,00,xx,
               00,xx,xx,
               00,00,00,
               00,xx,00};

BITMAP* bmpColon = 0;
char sColon[]={1,
               00,
               xx,
               00,
               xx,
               00};

BITMAP* bmpSch = 0;
char sch[]={3,
            00,xx,00,
            xx,xx,xx,
            xx,xx,xx,
            xx,xx,xx,
            00,xx,00};

BITMAP* bmpUsc = 0;
char usc[]={2,
            00,00,
            00,00,
            00,00,
            00,00,
            xx,xx};

BITMAP* bmpStar = 0;
char star[]={4,
             00,00,00,xx,
             xx,00,xx,00,
             00,xx,00,00,
             00,00,00,00,
             00,00,00,00};

BITMAP* bmpSs = 0;
char ss[]={2,
           xx,xx,
           xx,xx,
           xx,xx,
           xx,xx,
           xx,xx};

BITMAP* bmpSra = 0;
char sra[]={3,
            xx,00,00,
            xx,xx,00,
            xx,xx,xx,
            xx,xx,00,
            xx,00,00};

BITMAP* bmpLParen = 0;
char slParen[]={2,
                00,xx,
                xx,00,
                xx,00,
                xx,00,
                00,xx};

BITMAP* bmpRParen = 0;
char srParen[]={2,
                xx,00,
                00,xx,
                00,xx,
                00,xx,
                xx,00};

BITMAP* bmpSemic = 0;
char ssemic[]={2,
               xx,xx,
               00,00,
               xx,xx,
               00,xx,
               xx,00};

BITMAP* bmpSlash = 0;
char sSlash[]={3,
               00,00,xx,
               00,00,xx,
               00,xx,00,
               xx,00,00,
               xx,00,00};

BITMAP* bmpBlank = 0;
char sBlank[]={2,
               00,00,
               00,00,
               00,00,
               00,00,
               00,00};

BITMAP* create_char_bitmap(char* data) {
	int w = *data++;
	BITMAP* result = create_bitmap(w, 5);
	for (int y = 0; y < 5; y++) {
		for (int x = 0; x < w; x++) {
			int value=*data++;
			if (value) {
				putpixel(result,x,y,makecol(255,255,255));
			} else {
				putpixel(result,x,y,0);
			}
		}
	}
	return result;
}

static int smal_tbl_initted = 0;
void RegenSmallFont() {
	bmpUpperA = create_char_bitmap(sbA); bmpLowerA = create_char_bitmap(ssA);
	bmpUpperB = create_char_bitmap(sbB); bmpLowerB = create_char_bitmap(ssB);
	bmpUpperC = create_char_bitmap(sbC); bmpLowerC = create_char_bitmap(ssC);
	bmpUpperD = create_char_bitmap(sbD); bmpLowerD = create_char_bitmap(ssD);
	bmpUpperE = create_char_bitmap(sbE); bmpLowerE = create_char_bitmap(ssE);
	bmpUpperF = create_char_bitmap(sbF); bmpLowerF = create_char_bitmap(ssF);
	bmpUpperG = create_char_bitmap(sbG); bmpLowerG = create_char_bitmap(ssG);
	bmpUpperH = create_char_bitmap(sbH); bmpLowerH = create_char_bitmap(ssH);
	bmpUpperI = create_char_bitmap(sbI); bmpLowerI = create_char_bitmap(ssI);
	bmpUpperJ = create_char_bitmap(sbJ); bmpLowerJ = create_char_bitmap(ssJ);
	bmpUpperK = create_char_bitmap(sbK); bmpLowerK = create_char_bitmap(ssK);
	bmpUpperL = create_char_bitmap(sbL); bmpLowerL = create_char_bitmap(ssL);
	bmpUpperM = create_char_bitmap(sbM); bmpLowerM = create_char_bitmap(ssM);
	bmpUpperN = create_char_bitmap(sbN); bmpLowerN = create_char_bitmap(ssN);
	bmpUpperO = create_char_bitmap(sbO); bmpLowerO = create_char_bitmap(ssO);
	bmpUpperP = create_char_bitmap(sbP); bmpLowerP = create_char_bitmap(ssP);
	bmpUpperQ = create_char_bitmap(sbQ); bmpLowerQ = create_char_bitmap(ssQ);
	bmpUpperR = create_char_bitmap(sbR); bmpLowerR = create_char_bitmap(ssR);
	bmpUpperS = create_char_bitmap(sbS); bmpLowerS = create_char_bitmap(ssS);
	bmpUpperT = create_char_bitmap(sbT); bmpLowerT = create_char_bitmap(ssT);
	bmpUpperU = create_char_bitmap(sbU); bmpLowerU = create_char_bitmap(ssU);
	bmpUpperV = create_char_bitmap(sbV); bmpLowerV = create_char_bitmap(ssV);
	bmpUpperW = create_char_bitmap(sbW); bmpLowerW = create_char_bitmap(ssW);
	bmpUpperX = create_char_bitmap(sbX); bmpLowerX = create_char_bitmap(ssX);
	bmpUpperY = create_char_bitmap(sbY); bmpLowerY = create_char_bitmap(ssY);
	bmpUpperZ = create_char_bitmap(sbZ); bmpLowerZ = create_char_bitmap(ssZ);
	
	bmpBlank = create_char_bitmap(sBlank);
	bmpYow = create_char_bitmap(sYow);
	bmpQuote = create_char_bitmap(sQuote);
	bmpSs = create_char_bitmap(ss);
	bmpQuotes = create_char_bitmap(sQuotes);
	bmpLParen = create_char_bitmap(slParen);
	bmpRParen = create_char_bitmap(srParen);
	bmpStar = create_char_bitmap(star);
	bmpComma = create_char_bitmap(sComma);
	bmpMinus = create_char_bitmap(sMinus);
	bmpPeriod = create_char_bitmap(sPeriod);
	bmpSlash = create_char_bitmap(sSlash);
	bmp0 = create_char_bitmap(s0);
	bmp1 = create_char_bitmap(s1);
	bmp2 = create_char_bitmap(s2);
	bmp3 = create_char_bitmap(s3);
	bmp4 = create_char_bitmap(s4);
	bmp5 = create_char_bitmap(s5);
	bmp6 = create_char_bitmap(s6);
	bmp7 = create_char_bitmap(s7);
	bmp8 = create_char_bitmap(s8);
	bmp9 = create_char_bitmap(s9);
	bmpColon = create_char_bitmap(sColon);
	bmpSemic = create_char_bitmap(ssemic);
	bmpSra = create_char_bitmap(sra);
	bmpQuest = create_char_bitmap(sQuest);
	bmpSch = create_char_bitmap(sch);
	bmpUsc = create_char_bitmap(usc);

	smal_tbl = new BITMAP*[256];
	int index;
	for (index=0; index<256; index++)
		smal_tbl[index]=0;
	index=0;
	smal_tbl[index++] = bmpBlank;
	smal_tbl[index++] = bmpYow;
	smal_tbl[index++] = bmpQuote;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpQuotes;
	smal_tbl[index++] = bmpLParen;
	smal_tbl[index++] = bmpRParen;
	smal_tbl[index++] = bmpStar;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpComma;
	smal_tbl[index++] = bmpMinus;
	smal_tbl[index++] = bmpPeriod;
	smal_tbl[index++] = bmpSlash;
	smal_tbl[index++] = bmp0;
	smal_tbl[index++] = bmp1;
	smal_tbl[index++] = bmp2;
	smal_tbl[index++] = bmp3;
	smal_tbl[index++] = bmp4;
	smal_tbl[index++] = bmp5;
	smal_tbl[index++] = bmp6;
	smal_tbl[index++] = bmp7;
	smal_tbl[index++] = bmp8;
	smal_tbl[index++] = bmp9;
	smal_tbl[index++] = bmpColon;
	smal_tbl[index++] = bmpSemic;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpSra;
	smal_tbl[index++] = bmpQuest;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpUpperA;
	smal_tbl[index++] = bmpUpperB;
	smal_tbl[index++] = bmpUpperC;
	smal_tbl[index++] = bmpUpperD;
	smal_tbl[index++] = bmpUpperE;
	smal_tbl[index++] = bmpUpperF;
	smal_tbl[index++] = bmpUpperG;
	smal_tbl[index++] = bmpUpperH;
	smal_tbl[index++] = bmpUpperI;
	smal_tbl[index++] = bmpUpperJ;
	smal_tbl[index++] = bmpUpperK;
	smal_tbl[index++] = bmpUpperL;
	smal_tbl[index++] = bmpUpperM;
	smal_tbl[index++] = bmpUpperN;
	smal_tbl[index++] = bmpUpperO;
	smal_tbl[index++] = bmpUpperP;
	smal_tbl[index++] = bmpUpperQ;
	smal_tbl[index++] = bmpUpperR;
	smal_tbl[index++] = bmpUpperS;
	smal_tbl[index++] = bmpUpperT;
	smal_tbl[index++] = bmpUpperU;
	smal_tbl[index++] = bmpUpperV;
	smal_tbl[index++] = bmpUpperW;
	smal_tbl[index++] = bmpUpperX;
	smal_tbl[index++] = bmpUpperY;
	smal_tbl[index++] = bmpUpperZ;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpSs;
	smal_tbl[index++] = bmpUsc;
	smal_tbl[index++] = bmpSch;
	smal_tbl[index++] = bmpLowerA;
	smal_tbl[index++] = bmpLowerB;
	smal_tbl[index++] = bmpLowerC;
	smal_tbl[index++] = bmpLowerD;
	smal_tbl[index++] = bmpLowerE;
	smal_tbl[index++] = bmpLowerF;
	smal_tbl[index++] = bmpLowerG;
	smal_tbl[index++] = bmpLowerH;
	smal_tbl[index++] = bmpLowerI;
	smal_tbl[index++] = bmpLowerJ;
	smal_tbl[index++] = bmpLowerK;
	smal_tbl[index++] = bmpLowerL;
	smal_tbl[index++] = bmpLowerM;
	smal_tbl[index++] = bmpLowerN;
	smal_tbl[index++] = bmpLowerO;
	smal_tbl[index++] = bmpLowerP;
	smal_tbl[index++] = bmpLowerQ;
	smal_tbl[index++] = bmpLowerR;
	smal_tbl[index++] = bmpLowerS;
	smal_tbl[index++] = bmpLowerT;
	smal_tbl[index++] = bmpLowerU;
	smal_tbl[index++] = bmpLowerV;
	smal_tbl[index++] = bmpLowerW;
	smal_tbl[index++] = bmpLowerX;
	smal_tbl[index++] = bmpLowerY;
	smal_tbl[index++] = bmpLowerZ;

	smal_tbl_initted = 1;
}

BITMAP** smal_tbl = 0;
/*=
{  bmpBlank,
   bmpYow,  bmpQuote,      bmpSs,      bmpSs,     bmpSs,     bmpSs, bmpQuotes, bmpLParen,  // 40
 bmpRParen,    bmpStar,      bmpSs,  bmpComma, bmpMinus,bmpPeriod,  bmpSlash,      bmp0,  // 48
     bmp1,      bmp2,      bmp3,      bmp4,     bmp5,     bmp6,      bmp7,      bmp8,  // 56
      bmp9,  bmpColon,  bmpSemic,      bmpSs,     bmpSs,    bmpSra,  bmpQuest,      bmpSs,  // 64
     bmpUpperA,     bmpUpperB,     bmpUpperC,     bmpUpperD,    bmpUpperE,    bmpUpperF,     bmpUpperG,     bmpUpperH,  // 72
     bmpUpperI,     bmpUpperJ,     bmpUpperK,     bmpUpperL,    bmpUpperM,    bmpUpperN,     bmpUpperO,     bmpUpperP,  // 80
     bmpUpperQ,     bmpUpperR,     bmpUpperS,     bmpUpperT,    bmpUpperU,    bmpUpperV,     bmpUpperW,     bmpUpperX,  // 88
     bmpUpperY,     bmpUpperZ,      bmpSs,      bmpSs,     bmpSs,     bmpSs,     bmpUsc,     bmpSch,  // 96
     bmpLowerA,     bmpLowerB,     bmpLowerC,     bmpLowerD,    bmpLowerE,    bmpLowerF,     bmpLowerG,     bmpLowerH,  // 102
     bmpLowerI,     bmpLowerJ,     bmpLowerK,     bmpLowerL,    bmpLowerM,    bmpLowerN,     bmpLowerO,     bmpLowerP,  // 110
     bmpLowerQ,     bmpLowerR,     bmpLowerS,     bmpLowerT,    bmpLowerU,    bmpLowerV,     bmpLowerW,     bmpLowerX,  // 118
     bmpLowerY,     bmpLowerZ };                                                    // 120
 */

unsigned char lastcol=31;

static int _x_anchor = 0;
static int _x = 0;
static int _y = 0;

static int _putCharXY(int x, int y, char ch) {
	if (!smal_tbl_initted) RegenSmallFont();
	BITMAP* img = 0;
	ch -= 32;
	if (ch > 90) ch = 2;
	img = smal_tbl[ch];
	if (img==0) return 0;
	TCopySprite(x, y, img->w, img->h, img);
	return img->w+1;
}

void goXY(int x, int y) {
	_x = _x_anchor = x;
	_y = y;
}

void putXY(int x, int y, char* text) {
	goXY(x, y);
	for (; *text; text++) {
		_x += _putCharXY(_x, _y, *text);
	}
}

void putLineXY(int x, int y, char* text) {
	putXY(x, y, text);
	_y += 6;
	_x = _x_anchor;
}

void put(char* text) {
	for (; *text; text++) {
		_x += _putCharXY(_x, _y, *text);
	}
}

void putLine(char* text) {
	put(text);
	_y += 6;
	_x = _x_anchor;
}

void GotoXY(int x1, int y1)
{
  fontx=x1;
  fonty=y1;
}

void print_char(char c)
{ BITMAP* img;
  if (!smal_tbl_initted) RegenSmallFont();

  c-=32;
  if (c>90) c=2;
  else if (c<0) c=2;
  img=smal_tbl[c];
  if (img==0) return;
  TCopySprite(fontx,fonty,img->w,img->h,img);
  fontx+=img->w+1;
}

int print_char_score(char c)
{ BITMAP* img;
  if (!smal_tbl_initted) RegenSmallFont();

  c-=32;
  if (c>90) c=2;
  else if (c<0) c=2;
  img=smal_tbl[c];
  TCopySprite(fontx,fonty,img->w,img->h,img);
  fontx+=img->w + 1;
  return img->w + 1;
}

void printstring(int x, int y, const char* text, ...) {
	va_list args;
	char buffer[1024];
	va_start(args,text);
	vsprintf(buffer,text,args);
	va_end(args);

	GotoXY(x,y);
	printstring(buffer);
}

void printstring(char *str)
{
  for (; *str; ++str)
    print_char(*str);
}

void printstring_score(char* str, int cursor)
{
  int w = 0;
  int n = 0;
  for (; *str; ++str, ++n) {
    w = print_char_score(*str);
    if (n == cursor)
    {
      fontx -= w; fonty++;
      int temp = print_char_score('_');
      fontx -= temp;
	  fonty--;
	  fontx += w;
    }
  }
  if (n == cursor) {
      //fontx -= w;
      fonty++;
      int temp = print_char_score('_');
      fontx -= temp;
	  fonty--;
	  fontx += w;
  }
}

int pixels(char *str)
{ int pix;
if (!smal_tbl_initted) RegenSmallFont();

for (pix=0; *str; ++str) {
	int index=*str-32;
	if (index<0) index=0;
	if (index>255) index=255;
	BITMAP* img=smal_tbl[index];
	if (img)
    pix += img->w+1;//+1;
}
  return pix;
}

void dec_to_asciiz(int num, char *buf)
{
  sprintf(buf,"%d",num);
}

void TextColor(unsigned char newc)
{ char *src;

  src=sbA;
  while ((int)src < (int) smal_tbl)
  {
    if (*src==lastcol) *src=newc;
    src++;
  }
}
