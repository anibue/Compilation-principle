/* PL/0 console-mode test driver.
 * Mirrors the compiler core from src/Unit1.cpp without VCL/GUI.
 * Usage: pl0_test <input.PL0> <output.COD>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
    void __InitVCL(void) {}
    void __ExitVCL(void) {}
}
namespace System {
    void* MainInstance = 0;
    bool IsMultiThread = false;
}

const  AL    =  10;
const  NORW  =  19;
const  TXMAX = 100;
const  NMAX  =  14;
const  AMAX  =2047;
const  LEVMAX=   3;
const  CXMAX = 200;
const  SYMSETMAX = 64;

typedef enum  { NUL, IDENT, NUMBER, PLUS, MINUS, TIMES,
	            SLASH, ODDSYM, EQL, NEQ, LSS, LEQ, GTR, GEQ,
	            LPAREN, RPAREN, COMMA, SEMICOLON, PERIOD,
	            BECOMES, BEGINSYM, ENDSYM, IFSYM, THENSYM,
	            WHILESYM, WRITESYM, READSYM, DOSYM, CALLSYM,
	            CONSTSYM, VARSYM, PROCSYM, PROGSYM,
                ELSESYM, FORSYM, DOWNTOSYM, RETURNSYM, TOSYM,
                PLUSEQ, MINUSEQ, PLUSPLUS, MINUSMINUS,
                TIMESBECOMES, SLASHBECOMES
        } SYMBOL;
char *SYMOUT[] = {"NUL", "IDENT", "NUMBER", "PLUS", "MINUS", "TIMES",
	    "SLASH", "ODDSYM", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ",
	    "LPAREN", "RPAREN", "COMMA", "SEMICOLON", "PERIOD",
	    "BECOMES", "BEGINSYM", "ENDSYM", "IFSYM", "THENSYM",
	    "WHILESYM", "WRITESYM", "READSYM", "DOSYM", "CALLSYM",
	    "CONSTSYM", "VARSYM", "PROCSYM", "PROGSYM",
        "ELSESYM", "FORSYM", "DOWNTOSYM", "RETURNSYM", "TOSYM",
        "PLUSEQ", "MINUSEQ", "PLUSPLUS", "MINUSMINUS",
        "TIMESBECOMES", "SLASHBECOMES"
        };
typedef  int *SYMSET;
typedef  char ALFA[11];
typedef  enum { CONSTANT, VARIABLE, PROCEDUR } OBJECTS ;
typedef  enum { LIT, OPR, LOD, STO, CAL, INI, JMP, JPC } FCT;
typedef struct {
	 FCT F;
	 int L;
	 int A;
} INSTRUCTION;

char   CH;
SYMBOL SYM;
ALFA   ID;
int    NUM;
int    CC;
int    LL;
int    CX;
char   LINE[81];
INSTRUCTION  CODE[CXMAX];
ALFA    KWORD[NORW+1];
SYMBOL  WSYM[NORW+1];
SYMBOL  SSYM['^'+1];
ALFA    MNEMONIC[9];
SYMSET  DECLBEGSYS, STATBEGSYS, FACBEGSYS;

struct {
  ALFA NAME;
  OBJECTS KIND;
  union {
    int VAL;
    struct { int LEVEL,ADR,SIZE; } vp;
  };
} TABLE[TXMAX];

FILE *FIN,*FOUT;
int ERR;

void EXPRESSION(SYMSET FSYS, int LEV, int &TX);
void TERM(SYMSET FSYS, int LEV, int &TX);
void STATEMENT(SYMSET FSYS,int LEV,int &TX);

int SymIn(SYMBOL SYM, SYMSET S1) {
  return S1[SYM];
}
SYMSET SymSetUnion(SYMSET S1, SYMSET S2) {
  SYMSET S=(SYMSET)malloc(sizeof(int)*SYMSETMAX);
  for (int i=0; i<SYMSETMAX; i++)
	if (S1[i] || S2[i]) S[i]=1;
	else S[i]=0;
  return S;
}
SYMSET SymSetAdd(SYMBOL SY, SYMSET S) {
  SYMSET S1;
  S1=(SYMSET)malloc(sizeof(int)*SYMSETMAX);
  for (int i=0; i<SYMSETMAX; i++) S1[i]=S[i];
  S1[SY]=1;
  return S1;
}
SYMSET SymSetNew(SYMBOL a) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMSETMAX);
  for (i=0; i<SYMSETMAX; i++) S[i]=0;
  S[a]=1;
  return S;
}
SYMSET SymSetNew(SYMBOL a, SYMBOL b) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMSETMAX);
  for (i=0; i<SYMSETMAX; i++) S[i]=0;
  S[a]=1;  S[b]=1;
  return S;
}
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMSETMAX);
  for (i=0; i<SYMSETMAX; i++) S[i]=0;
  S[a]=1;  S[b]=1; S[c]=1;
  return S;
}
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c, SYMBOL d) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMSETMAX);
  for (i=0; i<SYMSETMAX; i++) S[i]=0;
  S[a]=1;  S[b]=1; S[c]=1; S[d]=1;
  return S;
}
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c, SYMBOL d,SYMBOL e) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMSETMAX);
  for (i=0; i<SYMSETMAX; i++) S[i]=0;
  S[a]=1;  S[b]=1; S[c]=1; S[d]=1; S[e]=1;
  return S;
}
SYMSET SymSetNew(SYMBOL a, SYMBOL b, SYMBOL c, SYMBOL d,SYMBOL e, SYMBOL f) {
  SYMSET S; int i,k;
  S=(SYMSET)malloc(sizeof(int)*SYMSETMAX);
  for (i=0; i<SYMSETMAX; i++) S[i]=0;
  S[a]=1;  S[b]=1; S[c]=1; S[d]=1; S[e]=1; S[f]=1;
  return S;
}
SYMSET SymSetNULL() {
  SYMSET S; int i,n,k;
  S=(SYMSET)malloc(sizeof(int)*SYMSETMAX);
  for (i=0; i<SYMSETMAX; i++) S[i]=0;
  return S;
}

void Error(int n) {
  char s[128];
  int sp = 0;
  s[sp++]='*'; s[sp++]='*'; s[sp++]='*';
  for (int i=0;i<CC-1;i++) s[sp++]=' ';
  s[sp++]='^'; s[sp]=0;
  fprintf(FOUT,"%s%d\n", s, n);
  ERR++;
}

void GetCh() {
  if (CC==LL) {
    if (feof(FIN)) {
      fprintf(FOUT,"PROGRAM INCOMPLETE\n");
      fclose(FOUT);
      exit(0);
	}
	LL=0; CC=0;
	CH=' ';
	while (!feof(FIN) && CH!=10)
      { CH=fgetc(FIN);  LINE[LL++]=CH; }
	LINE[LL-1]=' ';  LINE[LL]=0;
    fprintf(FOUT,"%3d %s\n",CX,LINE);
  }
  CH=LINE[CC++];
}

void GetSym() {
  int i,J,K;   ALFA  A;
  while (CH<=' ') GetCh();
  if (CH>='A' && CH<='Z') {
    K=0;
	do {
	  if (K<AL) A[K++]=CH;
	  GetCh();
	}while((CH>='A' && CH<='Z')||(CH>='0' && CH<='9'));
	A[K]='\0';
	strcpy(ID,A); i=1; J=NORW;
	do {
	  K=(i+J) / 2;
	  if (strcmp(ID,KWORD[K])<=0) J=K-1;
	  if (strcmp(ID,KWORD[K])>=0) i=K+1;
	}while(i<=J);
	if (i-1 > J) SYM=WSYM[K];
	else SYM=IDENT;
  }
  else
    if (CH>='0' && CH<='9') {
      K=0; NUM=0; SYM=NUMBER;
	  do {
	    NUM=10*NUM+(CH-'0');
		K++; GetCh();
      }while(CH>='0' && CH<='9');
	  if (K>NMAX) Error(30);
    }
    else
      if(CH=='+')   {
        GetCh();
        if(CH=='='){ SYM=PLUSEQ; GetCh(); }
        else
          if(CH=='+'){ SYM=PLUSPLUS; GetCh(); }
          else SYM = PLUS;
      }
    else
      if(CH=='-'){
        GetCh();
        if(CH=='='){ SYM=MINUSEQ; GetCh(); }
        else
          if(CH=='-'){ SYM=MINUSMINUS; GetCh(); }
          else SYM = MINUS;
      }
    else
      if(CH=='*')   {
        GetCh();
        if(CH=='='){ SYM=TIMESBECOMES; GetCh(); }
        else SYM = TIMES;
      }
    else
      if(CH=='/'){
        GetCh();
        if(CH=='='){ SYM=SLASHBECOMES; GetCh(); }
        else SYM = SLASH;
      }
    else
      if(CH=='!'){
        GetCh();
        if(CH=='='){ SYM = NEQ; GetCh(); }
        else SYM = NUL;
      }
    else
      if (CH==':') {
	    GetCh();
		if (CH=='=') { SYM=BECOMES; GetCh(); }
		else SYM=NUL;
      }
    else
      if (CH=='<') {
		GetCh();
		if (CH=='=') { SYM=LEQ; GetCh(); }
		else if(CH=='>'){ SYM=NEQ; GetCh(); }
		else SYM=LSS;
	  }
	  else
	    if (CH=='>') {
		  GetCh();
		  if (CH=='=') { SYM=GEQ; GetCh(); }
		  else SYM=GTR;
        }
        else { SYM=SSYM[CH]; GetCh(); }
}

void GEN(FCT X, int Y, int Z) {
  if (CX>CXMAX) {
    fprintf(FOUT,"PROGRAM TOO LONG\n");
    fclose(FOUT);
    exit(0);
  }
  CODE[CX].F=X; CODE[CX].L=Y; CODE[CX].A=Z;
  CX++;
}

void TEST(SYMSET S1, SYMSET S2, int N) {
  if (!SymIn(SYM,S1)) {
    Error(N);
	while (!SymIn(SYM,SymSetUnion(S1,S2))) GetSym();
  }
}

void ENTER(OBJECTS K, int LEV, int &TX, int &DX) {
  TX++;
  strcpy(TABLE[TX].NAME,ID); TABLE[TX].KIND=K;
  switch (K) {
	case CONSTANT:
	       if (NUM>AMAX) { Error(31); NUM=0; }
	       TABLE[TX].VAL=NUM;
	       break;
    case VARIABLE:
	       TABLE[TX].vp.LEVEL=LEV; TABLE[TX].vp.ADR=DX; DX++;
	       break;
	case PROCEDUR:
	       TABLE[TX].vp.LEVEL=LEV;
	       break;
  }
}

int POSITION(ALFA ID, int TX) {
  int i=TX;
  strcpy(TABLE[0].NAME,ID);
  while (strcmp(TABLE[i].NAME,ID)!=0) i--;
  return i;
}

void ConstDeclaration(int LEV,int &TX,int &DX) {
  if (SYM==IDENT) {
    GetSym();
    if (SYM==EQL||SYM==BECOMES) {
	  if (SYM==BECOMES) Error(1);
	  GetSym();
	  if (SYM==NUMBER) { ENTER(CONSTANT,LEV,TX,DX); GetSym(); }
	  else Error(2);
    }
    else Error(3);
  }
  else Error(4);
}

void VarDeclaration(int LEV,int &TX,int &DX) {
  if (SYM==IDENT) { ENTER(VARIABLE,LEV,TX,DX); GetSym(); }
  else Error(4);
}

void ListCode(int CX0) {
  for (int i=CX0; i<CX; i++) {
    fprintf(FOUT,"%3d%5s%4d%4d\n",i,MNEMONIC[CODE[i].F],CODE[i].L,CODE[i].A);
  }
}

void FACTOR(SYMSET FSYS, int LEV, int &TX) {
  int i;
  TEST(FACBEGSYS,FSYS,24);
  while (SymIn(SYM,FACBEGSYS)) {
	if (SYM==IDENT) {
	  i=POSITION(ID,TX);
	  if (i==0) Error(11);
	  else
		switch (TABLE[i].KIND) {
		  case CONSTANT: GEN(LIT,0,TABLE[i].VAL); break;
		  case VARIABLE: GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR); break;
		  case PROCEDUR: Error(21); break;
		}
	  GetSym();
	}
	else
	  if (SYM==NUMBER) {
		if (NUM>AMAX) { Error(31); NUM=0; }
		GEN(LIT,0,NUM); GetSym();
	  }
	  else
		if (SYM==LPAREN) {
		  GetSym(); EXPRESSION(SymSetAdd(RPAREN,FSYS),LEV,TX);
		  if (SYM==RPAREN) GetSym();
		  else Error(22);
		}
	  TEST(FSYS,FACBEGSYS,23);
  }
}

void TERM(SYMSET FSYS, int LEV, int &TX) {
  SYMBOL MULOP;
  FACTOR(SymSetUnion(FSYS,SymSetNew(TIMES,SLASH)), LEV,TX);
  while (SYM==TIMES || SYM==SLASH) {
	MULOP=SYM;  GetSym();
	FACTOR(SymSetUnion(FSYS,SymSetNew(TIMES,SLASH)),LEV,TX);
	if (MULOP==TIMES) GEN(OPR,0,4);
	else GEN(OPR,0,5);
  }
}

void EXPRESSION(SYMSET FSYS, int LEV, int &TX) {
  SYMBOL ADDOP;
  if (SYM==PLUS || SYM==MINUS) {
    ADDOP=SYM; GetSym();
    TERM(SymSetUnion(FSYS,SymSetNew(PLUS,MINUS)),LEV,TX);
    if (ADDOP==MINUS) GEN(OPR,0,1);
  }
  else TERM(SymSetUnion(FSYS,SymSetNew(PLUS,MINUS)),LEV,TX);
  while (SYM==PLUS || SYM==MINUS) {
    ADDOP=SYM; GetSym();
    TERM(SymSetUnion(FSYS,SymSetNew(PLUS,MINUS)),LEV,TX);
    if (ADDOP==PLUS) GEN(OPR,0,2);
    else GEN(OPR,0,3);
  }
}

void CONDITION(SYMSET FSYS,int LEV,int &TX) {
  SYMBOL RELOP;
  if (SYM==ODDSYM) { GetSym(); EXPRESSION(FSYS,LEV,TX); GEN(OPR,0,6); }
  else {
	EXPRESSION(SymSetUnion(SymSetNew(EQL,NEQ,LSS,LEQ,GTR,GEQ),FSYS),LEV,TX);
	if (!SymIn(SYM,SymSetNew(EQL,NEQ,LSS,LEQ,GTR,GEQ))) Error(20);
	else {
	  RELOP=SYM; GetSym(); EXPRESSION(FSYS,LEV,TX);
	  switch (RELOP) {
	    case EQL: GEN(OPR,0,8);  break;
	    case NEQ: GEN(OPR,0,9);  break;
	    case LSS: GEN(OPR,0,10); break;
	    case GEQ: GEN(OPR,0,11); break;
	    case GTR: GEN(OPR,0,12); break;
	    case LEQ: GEN(OPR,0,13); break;
	  }
	}
  }
}

void STATEMENT(SYMSET FSYS,int LEV,int &TX) {
  int i,CX1,CX2;
  switch (SYM) {
	case IDENT:
		i=POSITION(ID,TX);
		if (i==0) Error(11);
		else
		  if (TABLE[i].KIND!=VARIABLE) { Error(12); i=0; }
        GetSym();
        {
          SYMBOL assignOp=SYM;
          if (assignOp==PLUSPLUS || assignOp==MINUSMINUS) {
            GetSym();
            if (i!=0) {
              GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
              GEN(LIT,0,1);
              if (assignOp==PLUSPLUS) GEN(OPR,0,2);
              else GEN(OPR,0,3);
              GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            }
          }
          else {
            if (SYM==BECOMES || SYM==TIMESBECOMES || SYM==SLASHBECOMES
                || SYM==PLUSEQ || SYM==MINUSEQ) GetSym();
            else Error(13);
            if (i!=0 && (assignOp==TIMESBECOMES || assignOp==PLUSEQ || assignOp==MINUSEQ))
              GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            if (i!=0 && assignOp==SLASHBECOMES)
              GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            EXPRESSION(FSYS,LEV,TX);
            if (i!=0 && assignOp==PLUSEQ)  GEN(OPR,0,2);
            if (i!=0 && assignOp==MINUSEQ) GEN(OPR,0,3);
            if (i!=0 && assignOp==TIMESBECOMES) GEN(OPR,0,4);
            if (i!=0 && assignOp==SLASHBECOMES) GEN(OPR,0,5);
            if (i!=0) GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          }
        }
		break;
	case READSYM:
		GetSym();
		if (SYM!=LPAREN) Error(34);
		else
		  do {
			GetSym();
			if (SYM==IDENT) i=POSITION(ID,TX);
			else i=0;
			if (i==0) Error(33);
			else {
			  fprintf(FOUT,"? "); fflush(FOUT);
			  int v; scanf("%d",&v);
			  fprintf(FOUT,"? %d\n",v);
			  GEN(LIT,0,v);
			  GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
			}
			GetSym();
		  }while(SYM==COMMA);
		if (SYM!=RPAREN) {
		  Error(38);
		  while (!SymIn(SYM,FSYS)) GetSym();
		}
		else GetSym();
		break;
	case WRITESYM:
		GetSym();
		if (SYM==LPAREN) {
		  do {
			GetSym();
			EXPRESSION(SymSetUnion(SymSetNew(RPAREN,COMMA),FSYS),LEV,TX);
			GEN(OPR,0,14);
		  }while(SYM==COMMA);
		  if (SYM!=RPAREN) Error(33);
		  else GetSym();
		}
		GEN(OPR,0,15);
		break;
	case CALLSYM:
		GetSym();
		if (SYM!=IDENT) Error(14);
		else {
		  i=POSITION(ID,TX);
		  if (i==0) Error(11);
		  else
			if (TABLE[i].KIND==PROCEDUR)
			  GEN(CAL,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
			else Error(15);
		  GetSym();
		}
		break;
	case IFSYM:
		GetSym();
		CONDITION(SymSetUnion(SymSetNew(THENSYM,DOSYM),FSYS),LEV,TX);
		if (SYM==THENSYM) GetSym();
		else Error(16);
		CX1=CX;  GEN(JPC,0,0);
        STATEMENT(SymSetAdd(ELSESYM,FSYS),LEV,TX);
        if(SYM!=ELSESYM) CODE[CX1].A=CX;
         else{
             GetSym();
             CX2=CX;
             GEN(JMP,0,0);
             CODE[CX1].A=CX;
             STATEMENT(SymSetAdd(ELSESYM,FSYS),LEV,TX);
             CODE[CX2].A=CX;
         }
		break;
	case BEGINSYM:
		GetSym();
		STATEMENT(SymSetUnion(SymSetNew(SEMICOLON,ENDSYM),FSYS),LEV,TX);
		while (SymIn(SYM, SymSetAdd(SEMICOLON,STATBEGSYS))) {
		  if (SYM==SEMICOLON) GetSym();
		  else Error(10);
		  STATEMENT(SymSetUnion(SymSetNew(SEMICOLON,ENDSYM),FSYS),LEV,TX);
		}
		if (SYM==ENDSYM) GetSym();
		else Error(17);
		break;
	case WHILESYM:
		CX1=CX; GetSym(); CONDITION(SymSetAdd(DOSYM,FSYS),LEV,TX);
		CX2=CX; GEN(JPC,0,0);
		if (SYM==DOSYM) GetSym();
		else Error(18);
		STATEMENT(FSYS,LEV,TX);
		GEN(JMP,0,CX1);
		CODE[CX2].A=CX;
		break;
    case FORSYM: {
        int isDownTo=0;
        GetSym();
        if (SYM!=IDENT) Error(14);
        else {
          i=POSITION(ID,TX);
          if (i==0) Error(11);
          else if (TABLE[i].KIND!=VARIABLE) { Error(12); i=0; }
          GetSym();
          if (SYM!=BECOMES) Error(13);
          else GetSym();
          EXPRESSION(SymSetAdd(TOSYM,SymSetAdd(DOWNTOSYM,FSYS)),LEV,TX);
          if (i!=0) GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          CX1=CX;
          if (i!=0) GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          if (SYM==TOSYM) { isDownTo=0; GetSym(); }
          else if (SYM==DOWNTOSYM) { isDownTo=1; GetSym(); }
          else Error(39);
          EXPRESSION(SymSetAdd(DOSYM,FSYS),LEV,TX);
          if (i!=0) {
            if (isDownTo) GEN(OPR,0,11);
            else GEN(OPR,0,13);
          }
          CX2=CX; GEN(JPC,0,0);
          if (SYM==DOSYM) GetSym();
          else Error(18);
          STATEMENT(FSYS,LEV,TX);
          if (i!=0) {
            GEN(LOD,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
            GEN(LIT,0,1);
            if (isDownTo) GEN(OPR,0,3);
            else GEN(OPR,0,2);
            GEN(STO,LEV-TABLE[i].vp.LEVEL,TABLE[i].vp.ADR);
          }
          GEN(JMP,0,CX1);
          CODE[CX2].A=CX;
        }
        break;
    }
    case RETURNSYM:
        GetSym();
        GEN(OPR,0,0);
        break;
  }
  fprintf(FOUT, "[STATEMENT END sym=%s]\n", SYMOUT[SYM]);
  TEST(FSYS,SymSetNULL(),19);
}

void Block(int LEV, int TX, SYMSET FSYS) {
  int DX=3;
  int TX0=TX;
  int CX0=CX;
  TABLE[TX].vp.ADR=CX; GEN(JMP,0,0);
  if (LEV>LEVMAX) Error(32);
  do {
    if (SYM==CONSTSYM) {
      GetSym();
      do {
        ConstDeclaration(LEV,TX,DX);
        while (SYM==COMMA) {
          GetSym();  ConstDeclaration(LEV,TX,DX);
        }
        if (SYM==SEMICOLON) GetSym();
        else Error(5);
      }while(SYM==IDENT);
    }
    if (SYM==VARSYM) {
      GetSym();
      do {
        VarDeclaration(LEV,TX,DX);
        while (SYM==COMMA) { GetSym(); VarDeclaration(LEV,TX,DX); }
	    if (SYM==SEMICOLON) GetSym();
	    else Error(5);
      }while(SYM==IDENT);
    }
    while ( SYM==PROCSYM) {
      GetSym();
	  if (SYM==IDENT) { ENTER(PROCEDUR,LEV,TX,DX); GetSym(); }
	  else Error(4);
	  if (SYM==SEMICOLON) GetSym();
	  else Error(5);
	  Block(LEV+1,TX,SymSetAdd(SEMICOLON,FSYS));
	  if (SYM==SEMICOLON) {
	    GetSym();
	    TEST(SymSetUnion(SymSetNew(IDENT,PROCSYM),STATBEGSYS),FSYS,6);
	  }
	  else Error(5);
    }
    TEST(SymSetAdd(IDENT,STATBEGSYS), DECLBEGSYS,7);
  }while(SymIn(SYM,DECLBEGSYS));
  CODE[TABLE[TX0].vp.ADR].A=CX;
  TABLE[TX0].vp.ADR=CX;
  TABLE[TX0].vp.SIZE=DX;
  GEN(INI,0,DX);
  STATEMENT(SymSetUnion(SymSetNew(SEMICOLON,ENDSYM),FSYS),LEV,TX);
  GEN(OPR,0,0);
  TEST(FSYS,SymSetNULL(),8);
  ListCode(CX0);
}

int BASE(int L,int B,int S[]) {
  int B1=B;
  while (L>0) { B1=S[B1]; L=L-1; }
  return B1;
}

void Interpret() {
  const STACKSIZE = 500;
  int P,B,T;
  INSTRUCTION I;
  int S[STACKSIZE];
  fprintf(FOUT,"~~~ RUN PL0 ~~~\n");
  T=0; B=1; P=0;
  S[1]=0; S[2]=0; S[3]=0;
  do {
    I=CODE[P]; P=P+1;
    switch (I.F) {
      case LIT: T++; S[T]=I.A; break;
	  case OPR:
	    switch (I.A) {
	      case 0: T=B-1; P=S[T+3]; B=S[T+2]; break;
	      case 1: S[T]=-S[T];  break;
	      case 2: T--; S[T]=S[T]+S[T+1];   break;
	      case 3: T--; S[T]=S[T]-S[T+1];   break;
	      case 4: T--; S[T]=S[T]*S[T+1];   break;
          case 5: T--; S[T]=S[T] / S[T+1]; break;
	      case 6: S[T]=(S[T]%2!=0);        break;
	      case 8: T--; S[T]=S[T]==S[T+1];  break;
	      case 9: T--; S[T]=S[T]!=S[T+1];  break;
	      case 10: T--; S[T]=S[T]<S[T+1];   break;
	      case 11: T--; S[T]=S[T]>=S[T+1];  break;
	      case 12: T--; S[T]=S[T]>S[T+1];   break;
	      case 13: T--; S[T]=S[T]<=S[T+1];  break;
	      case 14: fprintf(FOUT,"%d\n",S[T]); T--;
                   break;
	      case 15: break;
          case 16: fprintf(FOUT,"? "); fflush(FOUT);
                   int v; scanf("%d",&v);
                   fprintf(FOUT,"? %d\n",v);
                   T++; S[T]=v; break;
	    }
	    break;
      case LOD: T++; S[T]=S[BASE(I.L,B,S)+I.A]; break;
      case STO: S[BASE(I.L,B,S)+I.A]=S[T]; T--; break;
	  case CAL: S[T+1]=BASE(I.L,B,S); S[T+2]=B; S[T+3]=P;
	      B=T+1; P=I.A; break;
	  case INI: T=T+I.A;  break;
	  case JMP: P=I.A; break;
      case JPC: if (S[T]==0) P=I.A;  T--;  break;
    }
  }while(P!=0);
  fprintf(FOUT,"~~~ END PL0 ~~~\n");
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    fprintf(stderr,"Usage: %s <input.PL0> <output.COD>\n",argv[0]);
    return 1;
  }
  FIN=fopen(argv[1],"r");
  if (!FIN) { fprintf(stderr,"Cannot open %s\n",argv[1]); return 1; }
  FOUT=fopen(argv[2],"w");
  if (!FOUT) { fclose(FIN); fprintf(stderr,"Cannot write %s\n",argv[2]); return 1; }

  for (CH=' '; CH<='^'; CH++) SSYM[CH]=NUL;
  strcpy(KWORD[ 1],"BEGIN");    strcpy(KWORD[ 2],"CALL");
  strcpy(KWORD[ 3],"CONST");    strcpy(KWORD[ 4],"DO");
  strcpy(KWORD[ 5],"DOWNTO");   strcpy(KWORD[ 6],"ELSE");
  strcpy(KWORD[ 7],"END");      strcpy(KWORD[ 8],"FOR");
  strcpy(KWORD[ 9],"IF");       strcpy(KWORD[10],"ODD");
  strcpy(KWORD[11],"PROCEDURE"); strcpy(KWORD[12],"PROGRAM");
  strcpy(KWORD[13],"READ");     strcpy(KWORD[14],"RETURN");
  strcpy(KWORD[15],"THEN");     strcpy(KWORD[16],"TO");
  strcpy(KWORD[17],"VAR");      strcpy(KWORD[18],"WHILE");
  strcpy(KWORD[19],"WRITE");
  WSYM[ 1]=BEGINSYM;   WSYM[ 2]=CALLSYM;
  WSYM[ 3]=CONSTSYM;   WSYM[ 4]=DOSYM;
  WSYM[ 5]=DOWNTOSYM;  WSYM[ 6]=ELSESYM;
  WSYM[ 7]=ENDSYM;     WSYM[ 8]=FORSYM;
  WSYM[ 9]=IFSYM;      WSYM[10]=ODDSYM;
  WSYM[11]=PROCSYM;    WSYM[12]=PROGSYM;
  WSYM[13]=READSYM;    WSYM[14]=RETURNSYM;
  WSYM[15]=THENSYM;    WSYM[16]=TOSYM;
  WSYM[17]=VARSYM;     WSYM[18]=WHILESYM;
  WSYM[19]=WRITESYM;

  SSYM['+']=PLUS;      SSYM['-']=MINUS;
  SSYM['*']=TIMES;     SSYM['/']=SLASH;
  SSYM['(']=LPAREN;    SSYM[')']=RPAREN;
  SSYM['=']=EQL;       SSYM[',']=COMMA;
  SSYM['.']=PERIOD;    SSYM[';']=SEMICOLON;

  strcpy(MNEMONIC[LIT],"LIT");   strcpy(MNEMONIC[OPR],"OPR");
  strcpy(MNEMONIC[LOD],"LOD");   strcpy(MNEMONIC[STO],"STO");
  strcpy(MNEMONIC[CAL],"CAL");   strcpy(MNEMONIC[INI],"INI");
  strcpy(MNEMONIC[JMP],"JMP");   strcpy(MNEMONIC[JPC],"JPC");

  DECLBEGSYS=(int*)malloc(sizeof(int)*SYMSETMAX);
  STATBEGSYS=(int*)malloc(sizeof(int)*SYMSETMAX);
  FACBEGSYS =(int*)malloc(sizeof(int)*SYMSETMAX);
  for(int j=0; j<SYMSETMAX; j++) {
	DECLBEGSYS[j]=0;  STATBEGSYS[j]=0;  FACBEGSYS[j] =0;
  }
  DECLBEGSYS[CONSTSYM]=1;
  DECLBEGSYS[VARSYM]=1;
  DECLBEGSYS[PROCSYM]=1;
  STATBEGSYS[BEGINSYM]=1;
  STATBEGSYS[CALLSYM]=1;
  STATBEGSYS[IFSYM]=1;
  STATBEGSYS[WHILESYM]=1;
  STATBEGSYS[WRITESYM]=1;
  STATBEGSYS[FORSYM]=1;
  STATBEGSYS[RETURNSYM]=1;
  FACBEGSYS[IDENT] =1;
  FACBEGSYS[NUMBER]=1;
  FACBEGSYS[LPAREN]=1;

  fprintf(FOUT,"=== COMPILE PL0 ===\n");
  ERR=0;
  CC=0; CX=0; LL=0; CH=' '; GetSym();
  if (SYM!=PROGSYM) Error(0);
  else {
    GetSym();
    if (SYM!=IDENT) Error(0);
    else {
	  GetSym();
	  if (SYM!=SEMICOLON) Error(5);
	  else GetSym();
    }
  }
  Block(0,0,SymSetAdd(PERIOD,SymSetUnion(DECLBEGSYS,STATBEGSYS)));
  if (SYM!=PERIOD) Error(9);
  if (ERR==0) Interpret();
  else {
    fprintf(FOUT,"ERROR IN PL/0 PROGRAM");
  }
  fprintf(FOUT,"\n"); fclose(FOUT);
  fclose(FIN);
  return ERR;
}