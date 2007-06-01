/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Le programme GRM cree un fichier contenant la representation
 * codee d'une grammaire, a` partir d'un fichier contenant la   
 * description de cette grammaire sous la forme BNF.
 * La grammaire codee est destinee aux programmes STR, PRS, TRA ou APP
 * qui compilent, selon cette grammaire, un schema de structure,
 * de presentation, de traduction ou d'application.
 *
 * Author: V. Quint
 *
 */

#include "thot_sys.h"
#include "thot_gui.h"
#include "grmmsg.h"
#include "compilmsg.h"
#include "constgrm.h"
#include "constint.h"
#include "conststr.h"
#include "constprs.h"
#include "message.h"
#include "fileaccess.h"
#include "typegrm.h"
#include "typeint.h"

typedef char        fname[30];	/* nom de fichier */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "compil_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "analsynt_tv.h"


#define NBSTRING 200		/* nombre max de chaines dans une grammaire */
#define MAXNRULE 160		/* nombre max de regles de la grammaire */
#define MAXREF   50		/* nombre max de references a une regle */

#define MAX_STRING_GRM	350	/* nombre max de chaines dans une grammaire */
#define MAX_RULE_GRM	180	/* nombre max de regles de la grammaire */
#define MAX_RULE_REF	80	/* nombre max de references a une regle */

typedef struct _RefList
  {
     int                 NRuleRefs;	/* number of references in the list */
     int                 RuleRef[MAX_RULE_REF];		/* number of a referred list */
  }
RefList;

int                  LineNum;	/* compteur de lignes */
static RefList       identRef[MAX_IDENTIFIERS];	/* table des references des identificateurs */
static RefList       kwRef[MAX_STRING_GRM];	/* table des references des mots-cles */
static int           Nstrings;	/* longueur effective de la table */
static SrcIdentDesc  strng[MAX_STRING_GRM];	/* table des chaines */
static RefList       nameRef;	/* liste des references a 'NAME' */
static RefList       numberRef;	/* liste des references a 'NUMBER' */
static RefList       stringRef;	/* liste des references a 'STRING' */
static Name          fileName;	/* nom du fichier a compiler */

static int           shortKeywordCode;	/* code du dernier mot-cle court cree */
static int           KeywordCode;	/* code du dernier mot-cle long cree */
static int           NSyntRules;	/* longueur effective de la table */
static SyntacticRule SyntRule[MAX_RULE_GRM];	/* table des regles codees */
static int           curRule;	/* regle en cours de generation */
static int           curIndx;	/* position courante dans la regle courante */
static FILE         *listFile;	/* fichier des listes */

#include "compilmsg_f.h"
#include "parser_f.h"
#include "platform_f.h"
#include "registry_f.h"

#ifdef _WINGUI
#define DLLEXPORT __declspec (dllexport)
#define FATAL_EXIT_CODE 33
#define COMP_SUCCESS     0
#include "compilers_f.h"
#endif /* _WINGUI */

/*----------------------------------------------------------------------
   InitRefTables initialise les tables des references.		
  ----------------------------------------------------------------------*/
static void InitRefTables ()
{
   int                 j;

   for (j = 0; j < MAX_IDENTIFIERS; j++)
      identRef[j].NRuleRefs = 0;
   for (j = 0; j < MAX_STRING_GRM; j++)
      kwRef[j].NRuleRefs = 0;
   nameRef.NRuleRefs = 0;
   numberRef.NRuleRefs = 0;
   stringRef.NRuleRefs = 0;
}


/*----------------------------------------------------------------------
   AddRefToTable ajoute le numero de la regle courante dans la     
   liste des references passee en parametre.               
  ----------------------------------------------------------------------*/
static void AddRefToTable (RefList * ref, indLine wi)
{
   if (ref->NRuleRefs >= MAX_RULE_REF)
      CompilerMessage (wi, GRM, FATAL, NO_SPACE_LEFT_IN_REF_TABLE, inputLine,
		       LineNum);
   else
      ref->RuleRef[ref->NRuleRefs++] = curRule;
}


/*----------------------------------------------------------------------
   PutToken	Put a token in the current rule.                
  ----------------------------------------------------------------------*/
static void PutToken (SyntacticCode code, indLine wi)
{
   if (curIndx >= RULE_LENGTH)
      CompilerMessage (wi, GRM, FATAL, INVALID_RULE_SIZE, inputLine, LineNum);
   else
      SyntRule[curRule - 1][curIndx++] = code;
}


/*----------------------------------------------------------------------
   ProcessToken traite le mot commencant a` la position wi dans la 
   ligne courante, de longueur wl et code grammatical code,   
   apparaissant dans la regl r. Si c'est un identif, rank    
   contient son rang dans la table des identificateurs.    
  ----------------------------------------------------------------------*/
static void ProcessToken (indLine wi, indLine wl, SyntacticCode code, int r, int rank)
{
  int                 i;
  int                 j;
  ThotBool             known;

  if (code < 1099 && code > 1000)
    /* un caractere separateur */
    switch (code - 1000)
      {
      case 1:
	curIndx = 1;	/* =  */
	break;
      case 2:
	PutToken (2000, wi);	/* .  */
	curIndx = 0;
	break;
      case 3:
	PutToken (2003, wi);	/* /  */
	break;
      case 4:
	PutToken (2001, wi);	/* [  */
	break;
      case 5:
	PutToken (2002, wi);	/* ]  */
	break;
      case 6:
	PutToken (2004, wi);	/* <  */
	break;
      case 7:
	PutToken (2005, wi);	/* >  */
	break;
      }
  else if (code >= 1102 && code <= 1104)
    /* un type de base */
    switch (code)
      {
      case 1102:
	/* NOM */
	PutToken (3001, wi);
	AddRefToTable (&nameRef, wi);
	break;
      case 1103:
	/* STRING */
	PutToken (3003, wi);
	AddRefToTable (&stringRef, wi);
	break;
      case 1104:
	/* NOMBRE */
	PutToken (3002, wi);
	AddRefToTable (&numberRef, wi);
	break;
      }
  else if (code == 3001)
    /* un symbole de la grammaire */
    if (curIndx == 0)
      /* nouvelle regle */
      {
	curRule = rank;
	fprintf (listFile, "%5d", rank);
	if (curRule > NSyntRules)
	  {
	    if (curRule <= MAX_RULE_GRM)
	      NSyntRules = curRule;
	    else
	      CompilerMessage (wi, GRM, FATAL, NO_SPACE_LEFT_IN_RULES_TABLE,
			       inputLine, LineNum);
	  }
	Identifier[rank - 1].SrcIdentDefRule = rank;
      }
    else
      {
	/* la regle est en cours */
	if (rank == curRule)
	  CompilerMessage (wi, GRM, FATAL, NO_SPACE_LEFT_IN_RULES_TABLE,
			   inputLine, LineNum);
	else
	  PutToken (rank, wi);
	Identifier[rank - 1].SrcIdentRefRule = rank;
	AddRefToTable (&identRef[rank - 1], wi);
      }
  else if (code == 3003)
    /* une chaine entre quotes */
    {
      i = 0;
      known = False;
      if (wl - 1 > IDENTIFIER_LENGTH)
	CompilerMessage (wi, GRM, FATAL, INVALID_STRING_SIZE, inputLine,
			 LineNum);
      /* est-elle deja dans la table ? */
      else
	{
	  do
	    {
	      if (strng[i].SrcIdentLen == wl - 1)
		{
		  j = 0;
		  do
		    j++;
		  while (inputLine[wi + j - 2] == strng[i].SrcIdentifier[j - 1]
			 && j < wl - 1);
		  if (j == wl - 1)
		    if (inputLine[wi + j - 2] == strng[i].SrcIdentifier[j - 1])
		      known = True;
		}
	      i++;
	    }
	  while (!known && i < Nstrings);
	  if (known)
	    /* deja dans la table */
	    AddRefToTable (&kwRef[i - 1], wi);
	  /* elle n'est pas dans la table, on la met */
	  else if (Nstrings >= MAX_STRING_GRM)
	    CompilerMessage (wi, GRM, FATAL, NO_SPACE_LEFT_IN_STRING_TABLE,
			     inputLine, LineNum);
	  else
	    {
	      strng[Nstrings].SrcIdentLen = wl - 1;
	      for (j = 0; j < wl - 1; j++)
		strng[Nstrings].SrcIdentifier[j] = inputLine[wi + j - 1];
	      if (strng[Nstrings].SrcIdentLen == 1)
		/* mot-cle court */
		{
		  shortKeywordCode++;
		  strng[Nstrings].SrcIdentCode = shortKeywordCode;
		}
	      else
		/* mot-cle long */
		{
		  KeywordCode++;
		  strng[Nstrings].SrcIdentCode = KeywordCode;
		}
	      AddRefToTable (&kwRef[Nstrings], wi);
	      Nstrings++;
	      i = Nstrings;
	    }
	  if (!error)
	    PutToken (strng[i - 1].SrcIdentCode, wi);
	}
    }
}


/*----------------------------------------------------------------------
   InitGrammar initialise la table des mots-cles et la table des   
   regles.                                                 
  ----------------------------------------------------------------------*/
static void         InitGrammar ()
{
  Keywords[0].SrcKeywordLen = 1;
  Keywords[0].SrcKeyword[0] = '=';
  Keywords[0].SrcKeywordCode = 1001;
  Keywords[1].SrcKeywordLen = 1;
  Keywords[1].SrcKeyword[0] = '.';
  Keywords[1].SrcKeywordCode = 1002;
  Keywords[2].SrcKeywordLen = 1;
  Keywords[2].SrcKeyword[0] = '/';
  Keywords[2].SrcKeywordCode = 1003;
  Keywords[3].SrcKeywordLen = 1;
  Keywords[3].SrcKeyword[0] = '[';
  Keywords[3].SrcKeywordCode = 1004;
  Keywords[4].SrcKeywordLen = 1;
  Keywords[4].SrcKeyword[0] = ']';
  Keywords[4].SrcKeywordCode = 1005;
  Keywords[5].SrcKeywordLen = 1;
  Keywords[5].SrcKeyword[0] = '<';
  Keywords[5].SrcKeywordCode = 1006;
  Keywords[6].SrcKeywordLen = 1;
  Keywords[6].SrcKeyword[0] = '>';
  Keywords[6].SrcKeywordCode = 1007;
  LastShortKeyword = 7;
  strncpy (Keywords[7].SrcKeyword, "END", KEWWORD_LENGTH);
  Keywords[7].SrcKeywordLen = strlen (Keywords[7].SrcKeyword);
  Keywords[7].SrcKeywordCode = 1101;
  strncpy (Keywords[8].SrcKeyword, "NAME", KEWWORD_LENGTH);
  Keywords[8].SrcKeywordLen = strlen (Keywords[8].SrcKeyword);
  Keywords[8].SrcKeywordCode = 1102;
  strncpy (Keywords[9].SrcKeyword, "STRING", KEWWORD_LENGTH);
  Keywords[9].SrcKeywordLen = strlen (Keywords[9].SrcKeyword);
  Keywords[9].SrcKeywordCode = 1103;
  strncpy (Keywords[10].SrcKeyword, "NUMBER", KEWWORD_LENGTH);
  Keywords[10].SrcKeywordLen = strlen (Keywords[10].SrcKeyword);
  Keywords[10].SrcKeywordCode = 1104;
  NKeywords = 11;
  GramRule[0][1] = 2;
  GramRule[0][2] = 2004;
  GramRule[0][3] = 2;
  GramRule[0][4] = 2005;
  GramRule[0][5] = 1101;
  GramRule[0][6] = 2000;
  GramRule[1][1] = 3;
  GramRule[1][2] = 1001;
  GramRule[1][3] = 4;
  GramRule[1][4] = 1002;
  GramRule[1][5] = 2000;
  GramRule[2][1] = 3001;
  GramRule[2][2] = 2000;
  GramRule[3][1] = 5;
  GramRule[3][2] = 2003;
  GramRule[3][3] = 6;
  GramRule[3][4] = 2000;
  GramRule[4][1] = 1102;
  GramRule[4][2] = 2003;
  GramRule[4][3] = 1103;
  GramRule[4][4] = 2003;
  GramRule[4][5] = 1104;
  GramRule[4][6] = 2000;
  GramRule[5][1] = 7;
  GramRule[5][2] = 2004;
  GramRule[5][3] = 1003;
  GramRule[5][4] = 7;
  GramRule[5][5] = 2005;
  GramRule[5][6] = 2000;
  GramRule[6][1] = 8;
  GramRule[6][2] = 2004;
  GramRule[6][3] = 8;
  GramRule[6][4] = 2005;
  GramRule[6][5] = 2000;
  GramRule[7][1] = 9;
  GramRule[7][2] = 2003;
  GramRule[7][3] = 1004;
  GramRule[7][4] = 9;
  GramRule[7][5] = 2004;
  GramRule[7][6] = 9;
  GramRule[7][7] = 2005;
  GramRule[7][8] = 1005;
  GramRule[7][9] = 2003;
  GramRule[7][10] = 1006;
  GramRule[7][11] = 9;
  GramRule[7][12] = 2004;
  GramRule[7][13] = 9;
  GramRule[7][14] = 2005;
  GramRule[7][15] = 1007;
  GramRule[7][16] = 2000;
  GramRule[8][1] = 3;
  GramRule[8][2] = 2003;
  GramRule[8][3] = 10;
  GramRule[8][4] = 2000;
  GramRule[9][1] = 3003;
  GramRule[9][2] = 2000;
  NGramRules = 10;
}


/*----------------------------------------------------------------------
   WriteFiles ecrit les mots-cles et regles dans le fichier de     
   sortie de type .GRM, produit le fichier .LST et le      
   fichier .h                                              
  ----------------------------------------------------------------------*/
static void         WriteFiles ()
{
  FILE               *GRMfile;
  FILE               *Hfile;
  int                 mc, l, r, i, j, ic;
  int                 lineLength;
  int                 maxIdent;

  /* met le suffixe GRM a la fin du nom de fichier */
  lineLength = 0;
  while (fileName[lineLength] != '.')
    lineLength++;
  strcpy (&fileName[lineLength + 1], "GRM");
  /* cree le fichier .GRM */
  GRMfile = fopen (fileName, "w");
  if (GRMfile == NULL)
    {
      TtaDisplayMessage (FATAL, TtaGetMessage (GRM, CANT_CREATE_HEADER_FILE),
			 fileName);
      exit (1);
    }
  
  /* cree le fichier .h */
  strcpy (&fileName[lineLength + 1], "h");
  Hfile = fopen (fileName, "w");
  if (Hfile == NULL)
    {
      TtaDisplayMessage (FATAL, TtaGetMessage (GRM, CANT_CREATE_HEADER_FILE),
			 fileName);
      exit (1);
    }
  else
    {
      fileName[lineLength] = '\0';
      putc ('\f', listFile);
      fprintf (listFile, "\n\n*** SHORT KEY-WORDS ***\n\n");
      fprintf (listFile, "code  character  numbers of rules using the key-word\n\n");
      fprintf (Hfile, "/* Definitions for compiler of language %s */\n\n",
	       fileName);
      fprintf (Hfile, "/*  SHORT KEY-WORDS  */\n\n");
      
      for (mc = 0; mc < Nstrings; mc++)
	{
	  if (strng[mc].SrcIdentLen == 1)
	    {
	      fprintf (Hfile, "#define CHR_%d \t%4d\n",
		     (int) strng[mc].SrcIdentifier[0], strng[mc].SrcIdentCode);
	      fprintf (GRMfile, "%c %4d\n", strng[mc].SrcIdentifier[0],
		       strng[mc].SrcIdentCode);
	      fprintf (listFile, "%4d      ", strng[mc].SrcIdentCode);
	      if (strng[mc].SrcIdentifier[0] < ' ')
		fprintf (listFile, "\\%3d ", (int) strng[mc].SrcIdentifier[0]);
	      else
		fprintf (listFile, "%c    ", strng[mc].SrcIdentifier[0]);
	      lineLength = 15;
	      for (j = 0; j < kwRef[mc].NRuleRefs; j++)
		{
		  if (lineLength > 76)
		    {
		      fprintf (listFile, "\n");
		      fprintf (listFile, "               ");
		      lineLength = 15;
		    }
		  fprintf (listFile, " %3d", kwRef[mc].RuleRef[j]);
		  lineLength += 4;
		}
	      fprintf (listFile, "\n");
	    }
	}
      fprintf (GRMfile, "\n");
      fprintf (listFile, "\n\n\n*** LONG KEY-WORDS ***\n\n");
      fprintf (listFile, "code  key-word      numbers of rules using the key-word\n\n");
      fprintf (Hfile, "\n/*  LONG KEY-WORDS  */\n\n");
      
      /* cherche la longueur du plus long des mots-cles */
      maxIdent = 0;
      for (mc = 0; mc < Nstrings; mc++)
	if (strng[mc].SrcIdentLen > maxIdent)
	  maxIdent = strng[mc].SrcIdentLen;
      /* ecrit les mots-cles et les regles qui les utilisent */
      for (mc = 0; mc < Nstrings; mc++)
	{
	  if (strng[mc].SrcIdentLen > 1)
	    {
	      fprintf (Hfile, "#define KWD_%s \t%4d\n",
		       strng[mc].SrcIdentifier, strng[mc].SrcIdentCode);
	      fprintf (listFile, "%4d  ", strng[mc].SrcIdentCode);
	      for (l = 0; l < strng[mc].SrcIdentLen; l++)
		{
		  putc (strng[mc].SrcIdentifier[l], GRMfile);
		  if (strng[mc].SrcIdentifier[l] < ' ')
		    fprintf (listFile, "*");
		  else
		    putc (strng[mc].SrcIdentifier[l], listFile);
		}
	      for (j = strng[mc].SrcIdentLen + 1; j <= maxIdent; j++)
		fprintf (listFile, " ");
	      lineLength = maxIdent + 6;
	      fprintf (GRMfile, " %4d\n", strng[mc].SrcIdentCode);
	      for (j = 0; j < kwRef[mc].NRuleRefs; j++)
		{
		  if (lineLength > 76)
		    {
		      fprintf (listFile, "\n");
		      lineLength = maxIdent + 8;
		      for (l = 1; l <= lineLength; l++)
			fprintf (listFile, " ");
		    }
		  fprintf (listFile, " %3d", kwRef[mc].RuleRef[j]);
		  lineLength += 4;
		}
	      fprintf (listFile, "\n");
	    }
	}
      fprintf (listFile, "\n\n\n*** TABLE OF RULES AND REFERENCES ***\n\n");
      
      fprintf (listFile, "code is the code of the symbol and the code of the rule\n");
      fprintf (listFile, "where that symbol appears in the left part. For each symbol,\n");
      fprintf (listFile, "the rule numbers indicate the rules that use that symbol in\n");
      fprintf (listFile, "their right part.\n\n");
      fprintf (listFile, " code  symbol        rule numbers\n\n");
      fprintf (Hfile, "\n/*  RULES  */\n\n");

      /* cherche la longueur du plus long des symboles */
      maxIdent = 0;
      for (ic = 0; ic < NIdentifiers; ic++)
	if (Identifier[ic].SrcIdentLen > maxIdent)
	  maxIdent = Identifier[ic].SrcIdentLen;
      /* ecrit les symboles et leurs references */
      for (ic = 0; ic < NIdentifiers; ic++)
	{
	  fprintf (Hfile, "#define RULE_%s \t%4d\n",
		   Identifier[ic].SrcIdentifier, ic + 1);
	  fprintf (listFile, " %4d  ", ic + 1);
	  for (l = 0; l < Identifier[ic].SrcIdentLen; l++)
	    if (Identifier[ic].SrcIdentifier[l] < ' ')
	      fprintf (listFile, "*");
	    else
	      putc (Identifier[ic].SrcIdentifier[l], listFile);
	  for (j = Identifier[ic].SrcIdentLen; j < maxIdent; j++)
	    fprintf (listFile, " ");
	  lineLength = maxIdent + 7;
	  for (j = 0; j < identRef[ic].NRuleRefs; j++)
	    {
	      if (lineLength > 76)
		{
		  fprintf (listFile, "\n");
		  lineLength = maxIdent + 9;
		  for (l = 1; l <= lineLength; l++)
		    fprintf (listFile, " ");
		}
	      fprintf (listFile, " %3d", identRef[ic].RuleRef[j]);
	      lineLength += 4;
	    }
	  fprintf (listFile, "\n");
	}
      fflush (Hfile);
      fclose (Hfile);

      fprintf (listFile, "\n\n\n*** REFERENCES TO BASIC TYPES ***\n\n");
      fprintf (listFile, "Numbers of the rules using NAME\n");
      
      lineLength = 0;
      for (j = 0; j < nameRef.NRuleRefs; j++)
	{
	  if (lineLength > 76)
	    {
	      fprintf (listFile, "\n");
	      lineLength = 0;
	    }
	  fprintf (listFile, " %3d", nameRef.RuleRef[j]);
	  lineLength += 4;
	}
      fprintf (listFile, "\n\n");

      fprintf (listFile, "Numbers of the rules using NUMBER\n");
      
      lineLength = 0;
      for (j = 0; j < numberRef.NRuleRefs; j++)
	{
	  if (lineLength > 76)
	    {
	      fprintf (listFile, "\n");
	      lineLength = 0;
	    }
	  fprintf (listFile, " %3d", numberRef.RuleRef[j]);
	  lineLength += 4;
	}
      fprintf (listFile, "\n\n");
      
      fprintf (listFile, "Numbers of the rules using STRING\n");
      
      lineLength = 0;
      for (j = 0; j < stringRef.NRuleRefs; j++)
	{
	  if (lineLength > 76)
	    {
	      fprintf (listFile, "\n");
	      lineLength = 0;
	    }
	  fprintf (listFile, " %3d", stringRef.RuleRef[j]);
	  lineLength += 4;
	}
      fprintf (listFile, "\n");
      fprintf (GRMfile, "\n");
      lineLength = 0;		/* ligne courante vide */
      for (r = 0; r < NSyntRules; r++)
	/* ecrit la table des regles */
	{
	  fprintf (GRMfile, "%4d ", r + 1);
	  /* numero du symbole defini par la regle */
	  lineLength += 5;
	  /* ecrit le contenu de la regle */
	  i = 0;
	  do
	    {
	      i++;
	      fprintf (GRMfile, "%4d ", SyntRule[r][i]);
	      lineLength += 5;
	      if (lineLength > 79)
		{
		  fprintf (GRMfile, "\n");
		  lineLength = 0;
		}
	    }
	  while (SyntRule[r][i] != 2000);
	  /* 2000 = Fin de la regle */
	  if (lineLength > 0)
	    {
	      fprintf (GRMfile, "\n");
	      lineLength = 0;
	    }
	  }
    }
  fclose (GRMfile);
}

/*----------------------------------------------------------------------
   CheckKeyword teste si la chaine de longueur wl commencant a` la 
   position wi de la ligne courante est un mot-cle bien    
   forme'.                                                 
  ----------------------------------------------------------------------*/
static void         CheckKeyword (indLine wi, indLine wl)
{
  indLine             j;

  if (wl == 2)
    {
      /* mot-cle court */
      if ((inputLine[wi - 1] >= '!' && inputLine[wi - 1] <= '/') ||
	  (inputLine[wi - 1] >= ':' && inputLine[wi - 1] <= '@') ||
	  (inputLine[wi - 1] >= '[' && inputLine[wi - 1] <= '`') ||
	  inputLine[wi - 1] == '|' ||
	  inputLine[wi - 1] == '~')
	{
	  if (inputLine[wi - 1] == '\'')
	    CompilerMessage (wi, GRM, FATAL, BAD_KEYWORD, inputLine, LineNum);
	}
      else
	CompilerMessage (wi, GRM, FATAL, BAD_KEYWORD, inputLine, LineNum);
    }
  else
    {
      /* mot-cle long */
      if ((inputLine[wi - 1] >= 'A' && inputLine[wi - 1] <= 'Z') ||
	  (inputLine[wi - 1] >= 'a' && inputLine[wi - 1] <= 'z') ||
	  (((int) inputLine[wi - 1]) >= 1 && inputLine[wi - 1] < ' '))
	/* le premier caractere est une lettre */
	{
	  j = wi;
	  do
	    {
	      if (!((inputLine[j] >= 'A' && inputLine[j] <= 'Z') ||
		    (inputLine[j] >= 'a' && inputLine[j] <= 'z') ||
		    (inputLine[j] >= '0' && inputLine[j] <= '9') ||
		    (((int) inputLine[wi - 1]) >= 1 &&
		     inputLine[wi - 1] < ' ')))
		CompilerMessage (wi, GRM, FATAL, BAD_KEYWORD, inputLine,
				 LineNum);
	      j++;
	    }
	  while (!error && j < wi + wl - 2);
	}
      else
	CompilerMessage (wi, GRM, FATAL, BAD_KEYWORD, inputLine, LineNum);
    }
}


/*----------------------------------------------------------------------
   CheckDefAndRef verifie que tous les symboles intermediaires     
   sont bien definis et reference's.                       
  ----------------------------------------------------------------------*/
static ThotBool      CheckDefAndRef ()
{
  int                 ic;
  ThotBool             ok;

  ok = True;
  for (ic = 0; ic < NIdentifiers; ic++)
    {
      if (Identifier[ic].SrcIdentDefRule == 0)
	{
	  TtaDisplayMessage (FATAL, TtaGetMessage (GRM, UNDEFINED_SYMBOL),
			     Identifier[ic].SrcIdentifier);
	  ok = False;
	}
      if (Identifier[ic].SrcIdentRefRule == 0)
	if (ic > 0)
	  {
	    TtaDisplayMessage (FATAL, TtaGetMessage (GRM, UNREFERENCED_SYMBOL),
			       Identifier[ic].SrcIdentifier);
	    ok = False;
	  }
    }
  return ok;
}


/*----------------------------------------------------------------------
   main program                                                    
  ----------------------------------------------------------------------*/
#ifdef _WINGUI
int GRMmain (HWND hwnd, int argc, char **argv, int *Y)
#else  /* !_WINGUI */
int main (int argc, char **argv)
#endif /* _WINGUI */
{
  FILE               *infile;
  ThotBool             fileOK;
  lineBuffer          sourceLine;	/* ligne source non traduite */
  indLine             i;	/* position courante dans la ligne en cours */
  indLine             wi;	/* position du debut du mot courant dans la
                                   ligne */
  indLine             wl;	/* longueur du mot courant */
  SyntacticType       wn;	/* SyntacticType du mot courant */
  SyntRuleNum         r;	/* numero de regle */
  SyntRuleNum         pr;	/* numero de la regle precedente */
  SyntacticCode       code;	/* code grammatical du mot trouve */
  int                 rank;	/* indice dans Identifier du mot trouve */
#ifdef _WINGUI 
  char                msg [800];
  int                 ndx;
#endif /* _WINGUI */

#ifdef _WINGUI
  hWnd = hwnd;
  compilersDC = GetDC (hwnd);
  _CY_ = *Y;
  strcpy (msg, "Executing grm ");
  for (ndx = 1; ndx < argc; ndx++)
    {
      strcat (msg, argv [ndx]);
      strcat (msg, " ");
    }
  TtaDisplayMessage (INFO, msg);
#endif /* _WINGUI */

  TtaInitializeAppRegistry (argv[0]);
  GRM = TtaGetMessageTable ("grmdialogue", GRM_MSG_MAX);
  COMPIL = TtaGetMessageTable ("compildialogue", COMP_MSG_MAX);

  if (argc != 2)
    TtaDisplaySimpleMessage (FATAL, GRM, UNKNOWN_FILE);
  else
    {
      strncpy (fileName, argv[1], MAX_NAME_LENGTH - 1);
      i = strlen (fileName);
      /* ajoute le suffixe .LAN */
      strcat (fileName, ".LAN");
      
      if (!TtaFileExist(fileName))
	TtaDisplaySimpleMessage (FATAL, GRM, UNKNOWN_FILE);
      else
	/* le fichier d'entree existe, on l'ouvre */
	{
	  infile = fopen (fileName, "r");
	  error = False;
	  InitGrammar ();
	  NIdentifiers = 0;	/* table des identificateurs vide */
	  Nstrings = 0;  	/* table des chaines vide */
	  NSyntRules = 0;	/* table des  regles vide */
	  curIndx = 0;
	  LineNum = 0;
	  shortKeywordCode = 1000;
	  KeywordCode = 1100;
	  InitRefTables ();	/* initialise la table des references */
	  InitParser ();	/* initialise l'analyseur syntaxique */
	  /* met le suffixe LST a la fin du nom de fichier */
	  strcpy (&fileName[i + 1], "LST");
	  /* cree le fichier .LST */
	  listFile = fopen (fileName, "w");
	  if (listFile == NULL)
	    {
	      TtaDisplayMessage (FATAL,
				 TtaGetMessage (GRM, CANT_CREATE_HEADER_FILE),
				 fileName);
	      exit (1);
	    }
	  fprintf (listFile, "GRAMMAR OF FILE ");
	  i = 0;
	  while (fileName[i] != '.')
	    putc (fileName[i++], listFile);
	  fprintf (listFile, ".LAN\n\n\n");
	  fprintf (listFile, " rule\n");
	  fprintf (listFile, "number\n");
	  
	  fprintf (listFile, "\n");
	  /* lit tout le fichier et fait l'analyse */
	  fileOK = True;
	  while (fileOK && !error)
	    /* lit une ligne */
	    {
	      i = 0;
	      do
		fileOK = TtaReadByte (infile, &inputLine[i++]);
	      while (i < LINE_LENGTH && inputLine[i - 1] != '\n' && fileOK);
	      /* marque la fin reelle de la ligne */
	      inputLine[i - 1] = '\0';
	      /* garde une copie de la ligne avant traduction */
	      strncpy ((char *)sourceLine, (char *)inputLine, LINE_LENGTH);
	      LineNum++;
	      /* traduit les caracteres de la ligne */
	      OctalToChar ();
	      /* analyse la ligne */
	      wi = 1;
	      wl = 0;
	      /* analyse tous les mots de la ligne courante */
	      do
		{
		  i = wi + wl;
		  GetNextToken (i, &wi, &wl, &wn);  /* mot suivant */
		  if (wi > 0)
		    {
		      /* on a trouve un mot */
		      if (wn == SynString)
			CheckKeyword (wi, wl);	/* mot-cle valide ? */
		      if (!error)
			/* on analyse le mot */
			AnalyzeToken (wi, wl, wn, &code, &r, &rank, &pr);
		      if (!error)
			/* on le traite */
			ProcessToken (wi, wl, code, r, rank);
		    }
		}
	      while (wi != 0 && !error);
	      /* il n'y a plus de mots dans la ligne */
	      /* ecrit la ligne source dans le fichier .LST */
	      putc ('\t', listFile);
	      wi = 0;
	      while (sourceLine[wi] != '\0')
		putc (sourceLine[wi++], listFile);
	      fprintf (listFile, "\n");
	    }
	  if (!error)
            ParserEnd ();	/* fin d'analyse */
	  if (!error)
            if (CheckDefAndRef ())
	      {
		TtaDisplaySimpleMessage (INFO, GRM, NEW_GRAMMAR_FILE);
		WriteFiles ();	/* ecrit les tables dans le fichier */
	      } 
	  fclose (infile);
	  fclose (listFile);
	  TtaSaveAppRegistry ();
	}
    }
#ifdef _WINGUI
  *Y = _CY_;
  ReleaseDC (hwnd, compilersDC);
  if (error)
    return FATAL_EXIT_CODE;
  return COMP_SUCCESS;
#else  /* !_WINGUI */
  return (0);
#endif /* _WINGUI */
}
