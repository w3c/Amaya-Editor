/*
 *
 *  (c) COPYRIGHT INRIA 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constgrm.h"
#include "typegrm.h"
#include "constmedia.h"
#include "typemedia.h"
#include "compilmsg.h"
#include "message.h"
#include "registry.h"
#include "fileaccess.h"

typedef struct _ParserStackItem
  {
     int                 StRule;	/* numero de la regle */
     int                 StRuleInd;	/* pointeur dans la regle */
     ThotBool             Option;	/* option en cours */
     ThotBool             Alt;	        /* on peut chercher une alternative */
     ThotBool             Tested;	/* regle en cours de test avec le mot courant */
  }
ParserStackItem;

#define THOT_EXPORT extern
#include "compil_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "analsynt_tv.h"
#include "compilmsg_f.h"

extern int          LineNum;	/* Numero de la ligne courante dans le fichier
				   en cours de compilation */

static ThotBool      Comment;	/* on est dans un commentaire */

#define STACKSIZE 40		/* taille de la pile */
static int          level;	/* niveau courant dans la pile */
static ParserStackItem Stack[STACKSIZE];	/* pile d'analyse */

#include "fileaccess.h"
#include "parser_f.h"
#include "registry_f.h"


/*----------------------------------------------------------------------
   InitParser initialise les donnees de l'analyseur syntaxique.     
  ----------------------------------------------------------------------*/
void InitParser ()
{
   Comment = False;		/* pas de commentaire en cours */
   level = 0;			/* initialise la pile */
   Stack[0].StRule = 0;
   Stack[0].StRuleInd = 1;	/* au debut de la regle initiale */
}


/*----------------------------------------------------------------------
   CheckShortKeyword teste si le caractere qui est a` la position  
   index dans la ligne courante est un mot-cle court.	
   Retourne dans ret le code de ce mot-cle ou 0 si ce	
   n'est pas un mot-cle court.				
  ----------------------------------------------------------------------*/
static void CheckShortKeyword (indLine index, SyntacticCode * ret)
{
   int                 i;

   *ret = 0;
   i = 0;
   do
     {
	if (inputLine[index - 1] == Keywords[i].SrcKeyword[0])
	   *ret = Keywords[i].SrcKeywordCode;
	i++;
     }
   while (*ret == 0 && i < LastShortKeyword);
}


/*----------------------------------------------------------------------
   CheckLongKeyword teste si le mot de longueur len qui commence a` 
   la position index dans la ligne courante est un mot-cle 
   long. Rend dans ret le code de ce mot-cle' ou 0 si ce   
   n'est pas un mot cle long.                              
  ----------------------------------------------------------------------*/
static void CheckLongKeyword (indLine index, indLine len, SyntacticCode *ret)
{
   int                 i;

   *ret = 0;
   i = LastShortKeyword;
   do
     {
       if (Keywords[i].SrcKeywordLen == len)
	 if (strncasecmp ((char *)&inputLine[index - 1], (char *)Keywords[i].SrcKeyword, len) == 0)
	   *ret = Keywords[i].SrcKeywordCode;
       i++;
     }
   while (*ret == 0 && i < NKeywords);
}


/*----------------------------------------------------------------------
   CheckIdent teste si le mot de longueur len qui commence a` la   
   position index dans la ligne courante est dans la table 
   des identificateurs. Rend dans ret le code du type      
   grammatical de cet identificateur ou 0 s'il n'est       
   pas dans la table. Rend dans rank le rang de              
   l'identificateur dans la table Identifier.              
  ----------------------------------------------------------------------*/
static void CheckIdent (indLine index, indLine len, SyntacticCode *ret, int *rank)
{
   int                 i;

   *ret = 0;
   *rank = 0;
   i = 0;
   do
     {
	if (Identifier[i].SrcIdentLen == len)
	   if (strncmp ((char *)&inputLine[index - 1], (char *)Identifier[i].SrcIdentifier, len) == 0)
	     {
		*rank = i + 1;
		*ret = Identifier[i].SrcIdentCode;
	     }
	i++;
     }
   while (*ret == 0 && i < NIdentifiers);
}


/*----------------------------------------------------------------------
   NewIdent ajoute a` la table des identificateurs le mot de       
   longueur len qui commence a` la position index dans la  
   ligne courante et qui est de type syntaxique code.      
   Rend dans rank le rang de cet identificateur dans la    
   table Identifier.                                       
  ----------------------------------------------------------------------*/
static void NewIdent (indLine index, indLine len, SyntacticCode code, int *rank)
{
  indLine             i;

  *rank = 0;
  if (NIdentifiers >= MAX_IDENTIFIERS)
    CompilerMessage (index, COMPIL, FATAL, NO_SPACE_LEFT_IN_INDENT_TABLE,
		     inputLine, LineNum);
  else if (len > IDENTIFIER_LENGTH)
    CompilerMessage (index, COMPIL, FATAL, INVALID_WORD_SIZE, inputLine,
		     LineNum);
  else
    {
      Identifier[NIdentifiers].SrcIdentLen = len;
      Identifier[NIdentifiers].SrcIdentDefRule = 0;
      Identifier[NIdentifiers].SrcIdentRefRule = 0;
      Identifier[NIdentifiers].SrcIdentCode = code;
      for (i = 0; i < len; i++)
	Identifier[NIdentifiers].SrcIdentifier[i] = inputLine[index + i - 1];
      NIdentifiers++;
      *rank = NIdentifiers;
    }
}

/*----------------------------------------------------------------------
   AsciiToInt traduit le nombre qui est sous sa forme ASCII a` la  
   position index de la ligne courante et qui est de       
   longueur len.                                            
  ----------------------------------------------------------------------*/
int AsciiToInt (indLine index, indLine len)
{
  int                 num;

  sscanf ((char *)&inputLine[index - 1], "%d", &num);
  if (num > 65535)
    {
      CompilerMessage (index, COMPIL, FATAL, NUMBER_OVERFLOW, inputLine,
		       LineNum);
      num = 0;
    }
  return num;
}

/*----------------------------------------------------------------------
   OctalToChar remplace dans le buffer d'entree inputLine les      
   sequences \nn par le caractere dont le code octal est   
   nn. Remplace aussi \\ par \.                            
  ----------------------------------------------------------------------*/
void OctalToChar ()
{
  int                 i, shift, k, n;

  i = 0;
  while (inputLine[i] != '\0')
    {
      if (inputLine[i] == '\\')
	{
	  shift = 0;
	  if (inputLine[i + 1] == '\\')
	    shift = 1;
	  else if (inputLine[i + 1] >= '0' &&
		   inputLine[i + 1] <= '7')
	    {
	      /* \ suivi d'un chiffre octal */
	      k = i + 1;
	      n = 0;
	      while (inputLine[k] >= '0' &&
		     inputLine[k] <= '7' && k < i + 4)
		n = n * 8 + ((int) inputLine[k++]) - ((int) '0');
	      if (n < 1 || n > 255)
		CompilerMessage (i, COMPIL, FATAL, INVALID_CHAR, inputLine,
				 LineNum);
	      else
		{
		  inputLine[i] = (unsigned char) n;
		  shift = k - i - 1;
		}
	    }
	  if (shift > 0)
	    /* decale la fin de la ligne de shift caracteres vers la gauche */
	    {
	      k = i + 1;
	      do
		{
		  inputLine[k] = inputLine[k + shift];
		  k++;
		}
	      while (inputLine[k - 1] != '\0');
	       }
	}
      else if (inputLine[i] < ' ')
	inputLine[i] = ' ';
      i++;
    }
}


/*----------------------------------------------------------------------
   GetNextToken cherche le prochain mot a` partir de la position   
   start dans la ligne courante. Au retour:                
   - wi: position dans la ligne du debut du mot trouve, ou 
   si pas trouve, 0.                                       
   - wl: longueur du mot trouve, ou 0 si pas trouve. (len+1
   si wn=SynString).                                           
   - wn: SyntacticType du mot trouve, ou SynError si pas trouve.       
  ----------------------------------------------------------------------*/
void GetNextToken (indLine start, indLine *wi, indLine *wl, SyntacticType *wn)
{
  indLine             j, k;
  ThotBool            stop;

  *wi = 0;
  *wl = 0;
  *wn = SynError;
  stop = False;
  j = start - 1;
  do
    {
      /* saute les caracteres vides et commentaires */
      /* les commentaires sont delimites par des accolades exclusivement */
       if (Comment)
	 {
	   /* cherche la fin du commentaire ou de la ligne */
	   while (inputLine[j] != '}' && inputLine[j] != '\0')
	     j++;
	   if (inputLine[j] == '}')
	     {
	       Comment = False;
	       j++;
	     }
	 }
       if (inputLine[j] != ' ')
	 {
	   if (inputLine[j] == '\0')
	     stop = True;
	   else if (inputLine[j] == '{')
	     {
	       /* debut de commentaire */
	       Comment = True;
	       while (inputLine[j] != '}' && inputLine[j] != '\0')
		 j++;
	       if (inputLine[j] == '\0')
	       stop = True;
	       else
		 Comment = False;
	     }
	   else
	     {
	       stop = True;
	       *wi = j + 1;
	     }
	 }
      j++;
     } 
   while (!stop);
   
   /* analyse les caracteres significatifs */
   if (*wi > 0 && !error)
     {
       stop = False;
       j = *wi - 1;
       *wl = 1;
       /* SyntacticType du mot trouve, d'apres son premier caractere */
       if (inputLine[j] >= '0' && inputLine[j] <= '9') 
         *wn = SynInteger;
       else if ((inputLine[j] >= 'A' && inputLine[j] <= 'Z') ||
                inputLine[j] == NBSP || /*nobreakspace */
                (inputLine[j] >= 'a' && inputLine[j] <= 'z') ||
                ( inputLine[j] >= 192 /*&& inputLine[j] <= 255 : not needed inputLine[] is unsigned char */))
	 *wn = SynIdentifier;
       else if (inputLine[j] == '\'')
	 *wn = SynString;
       else
	 *wn = SynShortKeyword;
       j++;
       if (*wn == SynInteger || *wn == SynIdentifier)
         /* verifie que le mot est bien forme et cherche la fin */
         do
	   {
	     if (inputLine[j] == '\0' || 
		 inputLine[j] == ' '  || 
		 (inputLine[j] >= '!' && inputLine[j] <= '/') ||
		 (inputLine[j] >= ':' && inputLine[j] <= '@') ||
		 (inputLine[j] >= '[' && inputLine[j] <= '^') || 
		 (inputLine[j] >= '{' && inputLine[j] <= '~') || 
		 inputLine[j] == 127 || 
		 inputLine[j] == '`')
               /* on a trouve un separateur */
               stop = True;	/* verifie l'homogeneite */
            else
	      {
		switch (*wn)
		  {
		  case SynInteger:
		    if (inputLine[j] < '0' || inputLine[j] > '9')
		      {
			CompilerMessage (j + 1, COMPIL, FATAL, BAD_NUMBER,
					 inputLine, LineNum);
			*wn = SynError;
			stop = True;
		      }
		    break;
		  case SynIdentifier:
		    /* @@@@@@@@@ Of course, this kind of test should be
		       replaced by another one if you want to handle Unicode
		       indentifiers and text.
		       For instance the test
		       (inputLine[j] >= 'A' && inputLine[j] <= 'Z'
		       or
		       'a' && inputLine[j] <= 'z' or
		       (inputLine[j] >= '0' && inputLine[j] <= '9'
		       should be:
		       if (iswalnum (inputLine[j])) to see if inputLine[j] is
		       scriptical or numerical value @@@@@@@@@@ */

		    if (!((inputLine[j] >= 'A' && inputLine[j] <= 'Z') || 
			  (inputLine[j] >= 'a' && inputLine[j] <= 'z') || 
			  (inputLine[j] == NBSP) || /*nobreakspace */
			  (inputLine[j] >= '0' && inputLine[j] <= '9') || 
			  (inputLine[j] >= 192 /* && inputLine[j] <= 255 : not needed inputLine[] is unsigned char*/)  || /* lettre accentuee */
			  inputLine[j] == '_')) {
		      CompilerMessage (j + 1, COMPIL, FATAL, BAD_WORD,
				       inputLine, LineNum);
		      *wn = SynError;
		      stop = True;
		    }
		    break;
		  default:
		    break;
		  }
		(*wl)++;
	      }
	     j++;
	   }
	 while (!stop);
       else if (*wn == SynString)
	 {
	   /* chaine de caracteres */
	   /* saute le quote initial */
	   (*wi)++;
	   *wl = 0;
	   do 
	     if (inputLine[j] == '\0')
	       {
		 CompilerMessage (*wi, COMPIL, FATAL,
				  MULTIPLE_LINE_STRINGS_ERROR, inputLine,
				  LineNum);
		 *wn = SynError;
                 stop = True;
	       }
	     else
	       {
		 if (inputLine[j] == '\'')
		   if (inputLine[j + 1] == '\'')
		     {
		       /* represente un seul quote */
		       k = j;
		       do
			 {
			   /* ecrase le quote double */
			   k++;
			   inputLine[k] = inputLine[k + 1];
			 }
		       while (inputLine[k] != '\0');
		       j++;
		     }
		   else
		     stop = True; /* quote final */
		 else
		   j++;
		 (*wl)++;
	       } 
	   while (!stop);
	 }
     }
}


/*----------------------------------------------------------------------
   TokenMatch retourne vrai si dans la ligne courante le mot       
   commencant a` l'index wi de longueur wl et de SyntacticType wn 
   correspond a` l'element de code c qui apparait dans la  
   regle r de la grammaire. Rend dans rank le rang du mot  
   dans la table Identifier si c'est un identificateur.    
  ----------------------------------------------------------------------*/
static ThotBool TokenMatch (indLine wi, indLine wl, SyntacticType wn,
			    SyntacticCode c, SyntacticCode r, int *rank)
{
  SyntacticCode       code;
  ThotBool             match;

  match = False;
  if (c < 1000)
    /* identificateur attendu */
    /* a priori ca marche */
    match = True;
  else if (c >= 1000 && c < 1100)
    /* mot-cle court attendu */
    {
      if (wl == 1 && wn == SynShortKeyword)
	/* c'est un mot-cle court */
	{
	  /* est-il valide ? */
	  CheckShortKeyword (wi, &code);
	  if (code > 0)
	    /* mot-cle court valide */
	    {
	      if (code == c)
		/* c'est le mot-cle court attendu */
		match = True;
	    }
	  else
	    /* mot-cle court invalide */
	    CompilerMessage (wi, COMPIL, FATAL, BAD_SYMBOL, inputLine,LineNum);
	}
    }
  else if (c >= 1100 && c < 2000)
    /* mot-cle long attendu */
    {
      CheckLongKeyword (wi, wl, &code);
      /* est-ce un mot-cle long valide ? */
      if (code == c)
	/* c'est le mot-cle long attendu */
	match = True;
    }
  else if (c > 3000)
    /* type de base attendu */
    {
      if (c == 3001 && wn == SynIdentifier)
	/* on attend un nom et c'est un nom */
	{
	  CheckIdent (wi, wl, &code, rank);
	  /* est-il dans la table ? */
	  if (code > 0)
	    /* il y est */
	    {
	      if (code == r)
		match = True;
	      /* oui, et avec le type attendu */
	    }
	  else
	    /* non, il n'est pas encore connu */
	    {
	      CheckLongKeyword (wi, wl, &code);
	      /* est-ce un mot-cle long ? */
	      if (code == 0)
		/* ce n'est pas un mot-cle long, c'est */
		/* donc un identificateur */
		{
		  NewIdent (wi, wl, r, rank);
		  /* on l'ajoute dans la table */
		  match = True;	/* et c'est bon... */
		}
	    }
	}
      if (c == 3002 && wn == SynInteger)
	match = True;
      if (c == 3003 && wn == SynString)
	match = True;
    }
  return match;
}


/*----------------------------------------------------------------------
   AnalyzeToken procede a` l'analyse du mot commencant a la postion
   wi de la ligne courante (inputLine), de longueur wl et de
   SyntacticType wn. Rend dans c le code grammatical du mot, dans 
   r le numero de la derniere regle ou` il a ete trouve et 
   dans rank son rang dans Identifier, si c'est un         
   identificateur. Dans pr se trouve le numero de l'avant  
   derniere regle appliquee.                               
  ----------------------------------------------------------------------*/
void AnalyzeToken (indLine wi, indLine wl, SyntacticType wn, SyntacticCode *c,
		   SyntRuleNum *r, int *rank, SyntRuleNum *pr)
{
  ThotBool             stop;
  ThotBool             st1;
  ThotBool             ok;
  ThotBool             meta;
  int                  s;

  *pr = 0;
  if (level < 0)
    CompilerMessage (wi, COMPIL, FATAL, END_HAS_BEEN_DETECTED, inputLine,
		     LineNum);
  else
    {
      ok = False;
      stop = False;
      for (s = 0; s < level; s++)
	Stack[s].Tested = False;
      /* aucune regle de la pile n'a ete testee avec ce mot, sauf celle
	 du haut, en cours */
      do
	/* cherche une regle a laquelle le mot correspond */
	{
	  meta = True;
	  /* traite les meta symboles de la regle courante */
	  do
	    {
	      if (GramRule[Stack[level].StRule][Stack[level].StRuleInd] >= 2000
		  && GramRule[Stack[level].StRule][Stack[level].StRuleInd] <= 2005)
		switch (GramRule[Stack[level].StRule][Stack[level].StRuleInd])
		  {
		  case 2001:
		    /* debut d'option */
		    Stack[level].Option = True;
		    Stack[level].StRuleInd++;
		    break;
		  case 2002:
		    /* fin d'option */
		    Stack[level].Option = False;
		    Stack[level].StRuleInd++;
		    break;
		  case 2000:
		  case 2003:
		    /* fin de regle ou alternative */
		    level--;	/* depile une regle */
		    if (level < 0)
		      stop = True;
		    /* on est a la fin de la regle initiale */
		    else
		      {
			if (Stack[level].Alt)
			  /* essaie une alternative de la nvelle regle */
			  {
			    do
			      Stack[level].StRuleInd++;
			    while (GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2003
				   && GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2000);
			    if (GramRule[Stack[level].StRule][Stack[level].StRuleInd] == 2003)
			      Stack[level].StRuleInd++;
			  }
			else
			  Stack[level].StRuleInd++;
		      }
		    break;
		  case 2004:
		    /* debut de repetition */
		    Stack[level].StRuleInd++;
		    Stack[level].Option = True;
		    break;
		  case 2005:
		    /* fin de repetition */
		    if (Stack[level].Tested)
		      /* on a deja essaye ce mot, on saute */
		      {
			Stack[level].StRuleInd++;
			Stack[level].Option = False;
		      }
		    else
		      /* ce mot n'a pas ete essaye, on y va */
		      {
			do
			  Stack[level].StRuleInd--;
			while (GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2004);
			Stack[level].StRuleInd++;
			Stack[level].Option = True;
			Stack[level].Tested = True;
		      }
		    break;
		  }
	      
	      else
		meta = False;
	    }
	  while (meta && level >= 0);
	  if (level >= 0)
	    {
	      *c = GramRule[Stack[level].StRule][Stack[level].StRuleInd];
	      if (TokenMatch (wi, wl, wn, *c, Stack[level].StRule + 1, rank))
		/* ca correspond */
		if (*c < 1000)
		  /* symbole non terminal */
		  if (level >= STACKSIZE)
		    CompilerMessage (wi, COMPIL, FATAL, NO_SPACE_LEFT_IN_STACK,
				     inputLine, LineNum);
		  else
		    /* empile la regle definissant ce symbole */
		    {
		      level++;
		      Stack[level].StRule = *c - 1;
		      Stack[level].StRuleInd = 1;
		      Stack[level].Option = False;
		      Stack[level].Alt = True;
		      Stack[level].Tested = True;
		    }
		else
		  {
		    ok = True;
		    *r = Stack[level].StRule + 1;
		    if (level > 0)
		      *pr = Stack[level - 1].StRule + 1;
		    for (s = 0; s < level; s++)
		      {
			/* sur toute la pile */
			Stack[s].Alt = False;
			/* s'il y a une alternative, on ne peut */
			/* plus en changer */
			Stack[s].Option = False;
		      }
		    do
		      /* si on etait dans une partie */
		      /* optionnelle, on ne peut plus la sauter */
		      /* traite les meta symboles suivant le mot ok */
		      {
			Stack[level].StRuleInd++;
			if (GramRule[Stack[level].StRule][Stack[level].StRuleInd] >= 2000
			    && GramRule[Stack[level].StRule][Stack[level].StRuleInd] <= 2005)
			  switch (GramRule[Stack[level].StRule][Stack[level].StRuleInd])
			    {
			    case 2001:
			      stop = True;	/* debut d'option */
			      break;
			    case 2002:
			      Stack[level].Option = False;  /* fin d'option */
			      break;
			    case 2000:
			    case 2003:
			      /* fin de regle ou alternative */
			      level--;	/* regle epuisee */
			      if (level < 0)
				stop = True;
			      /* on est a la fin de la regle initiale */
			      break;
			    case 2004:
			      stop = True;	/* debut de repetition */
			      break;
			    case 2005:
			      /* fin de repetition */
			      do
				Stack[level].StRuleInd--;
			      while (GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2004);
			      Stack[level].StRuleInd++;
			      Stack[level].Option = True;
			      stop = True;
			      break;
			    }
			else
			  stop = True;
		      }
		    while (!stop && level >= 0);
		  }
	      else
		{
		  /* le mot ne correspond pas */
		  if (Stack[level].Option)  /* saute la partie optionnelle */
		    /* cherche la fin de la partie a option */
		    {
		      do
			Stack[level].StRuleInd++;
		      while (GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2002
			     && GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2005);
		      Stack[level].StRuleInd++;
		      Stack[level].Option = False;
		      if (GramRule[Stack[level].StRule][Stack[level].StRuleInd] == 2000
			  || GramRule[Stack[level].StRule][Stack[level].StRuleInd] == 2003)
			/* fin regle */
			{
			  level--;	/* depile une regle */
			  if (level < 0)
			    stop = True;	/* pile vide */
			  else
			    Stack[level].StRuleInd++;
			}
		    }
		  else
		    /* ce n'est pas un element optionnel */
		    {
		      st1 = False;
		      do
			/* cherche une alternative dans la regle */
			{
			  do
			    Stack[level].StRuleInd++;
			  while (GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2003
				 && GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2000);
			  if (GramRule[Stack[level].StRule][Stack[level].StRuleInd] == 2003)
			    /* une alternative dans la regle */
			    {
			      if (Stack[level].Alt)
				/* alternative autorisee */
				{
				  /* essaie l'alternative */
				  Stack[level].StRuleInd++;
				  st1 = True;
				}
			    }
			  else
			    /* pas d'alternative dans la regle */
			    /* depile les regles jusqu'a en trouver une ou */
			    /* on est dans une partie optionnelle ou un choix*/
			    {
			      while (level >= 0 && !st1)
				{
				  level--;
				  if (level >= 0)
				    {
				      if (Stack[level].Option)
					/* saute la partie optionnelle */
					/* cherche la fin de la partie a option */
					{
					  do
					    Stack[level].StRuleInd++;
					  while (GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2002
						 && GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2005);
					  Stack[level].StRuleInd++;
					  Stack[level].Option = False;
					  st1 = True;
					}
				      else if (Stack[level].Alt)
					/* cherche une alternative */
					{
					  do
					    Stack[level].StRuleInd++;
					  while (GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2003
						 && GramRule[Stack[level].StRule][Stack[level].StRuleInd] != 2000);
					  if (GramRule[Stack[level].StRule][Stack[level].StRuleInd] == 2003)
					    {
					      Stack[level].StRuleInd++;
					      st1 = True;
					    }
					}
				    }
				}
			      st1 = True;
			    }
			}
		      while (!st1);
		    }
		}
	    }
	}
      while (!stop && level >= 0);	/* mot ok ou fin de regle */
      if (!ok)
	CompilerMessage (wi, COMPIL, FATAL, SYNTAX_ERROR, inputLine, LineNum);
    }
}


/*----------------------------------------------------------------------
   ParserEnd verifie, en fin de fichier source, que tout est correct
   du point de vue syntaxique.                             
  ----------------------------------------------------------------------*/
void ParserEnd ()
{
  if (level >= 0)
    /* la pile n'est pas vide */
    CompilerMessage (1, COMPIL, FATAL, ABNORMAL_END, inputLine, LineNum);
  else
    {
      /* la pile est vide */
      if (GramRule[Stack[0].StRule][Stack[0].StRuleInd] != 2000)
	/* la regle initiale n'est pas terminee */
	CompilerMessage (1, COMPIL, FATAL, ABNORMAL_END, inputLine, LineNum);
    }
}


/*----------------------------------------------------------------------
   InitSyntax initialise la table des mots-cles et la table des	
   regles a` partir d'un fichier grammaire de type GRM.		
   fileName est le nom du fichier grammaire, avec le suffixe .GRM.	
  ----------------------------------------------------------------------*/
void InitSyntax (const char *fileName)
{
  indLine             j, wind, wlen;
  SyntacticType       wnat;
  SrcKeywordDesc     *pkw1;
  BinFile             grmFile;
  char                pgrname[200];
  char                pnomcourt[200];
  int                 l;
  int                 ruleptr;
  int                 currule = 1;
  ThotBool            readingKeywordTable;
  ThotBool             fileOK;

  strcpy (pnomcourt, fileName);
  /* cherche dans le directory compil si le fichier grammaire existe */
  if (SearchFile (pnomcourt, 3, pgrname) == 0)
    CompilerMessage (0, COMPIL, FATAL, GRM_FILE_NOT_FOUND, inputLine, LineNum);
  else
    {
      /* ouvre le fichier grammaire */
      grmFile = TtaReadOpen (pgrname);
      NKeywords = 0;
      LastShortKeyword = 0;
      /* on commence par la lecture de la table des mots-cles */
      readingKeywordTable = True;
      NGramRules = 0;
      ruleptr = 0;
      fileOK = True;
      while (fileOK)
	{
	  /* lit une ligne */
	  j = 0;
	  do
	    {
	      fileOK = TtaReadByte (grmFile, &inputLine[j]);
	      j++;
	    }
#ifdef _WINGUI
	  while (j < LINE_LENGTH && inputLine [j-1] != 13 &&
		 inputLine[j - 1] != '\n' && fileOK);
#else
	     while (j < LINE_LENGTH && inputLine[j - 1] != '\n' && fileOK);
#endif
	     /* marque la fin reelle de la ligne */
	     inputLine[j - 1] = '\0';
	     /* traite la ligne */
	     j = 1;
	     do
	       {
		 /* accede au mot suivant de la ligne */
		 GetNextToken (j, &wind, &wlen, &wnat);
		 if (readingKeywordTable)
		   /* lecture de la table des mots-cles */
		   {
		     if (wnat == SynIdentifier && LastShortKeyword == 0)
		       LastShortKeyword = NKeywords;
		     /* on passe des courts aux longs */
		     if (wnat == SynIdentifier || wnat == SynShortKeyword)
		       {
			 if (NKeywords >= MAX_KEYWORDS)
			   /* table saturee */
			   CompilerMessage (wind, COMPIL, FATAL,
					    NO_SPACE_LEFT_IN_KEYWORD_TABLE,
					    inputLine, LineNum);
			 else
			   NKeywords++;
			 /* entree suivante de la table */
			 pkw1 = &Keywords[NKeywords - 1];
			 /* remplit cette nouvelle entree */
			 if (wlen > KEWWORD_LENGTH)
			   {
			     wlen = KEWWORD_LENGTH;
			     CompilerMessage (wind, COMPIL, FATAL,
					      INVALID_KEYWORD_SIZE, inputLine,
					      LineNum);
			   }
			 pkw1->SrcKeywordLen = wlen;
			 for (l = 0; l < wlen; l++)
			   {
			     pkw1->SrcKeyword[l] = inputLine[wind + l - 1];
			     if ((char) (((int) pkw1->SrcKeyword[l]) - 32) >= 'A'
				 && (char) (((int) pkw1->SrcKeyword[l]) - 32) <= 'Z')
			       pkw1->SrcKeyword[l] = (char) (((int) pkw1->SrcKeyword[l]) - 32);
			   }
			 /* traduit le mot-cle en majuscules */
			 j = wind + wlen;
			 GetNextToken (j, &wind, &wlen, &wnat);
			 /* lit le code */
			 /* grammatical du mot-cle */
			 if (wnat == SynInteger)
			   pkw1->SrcKeywordCode = AsciiToInt (wind, wlen);
			 else
			   /* fichier incorrect */
			   CompilerMessage (wind, COMPIL, FATAL,
					    INCORR_GRAMMAR_FILE_GRM,
					    inputLine, LineNum);
		       }
		     else if (wnat == SynInteger)
		       /* fin de la table mots-cles */
		       readingKeywordTable = False;
		   }
		 if (!readingKeywordTable)
		   /* lecture de la table des regles */
		   {
		     if (wnat == SynInteger)
		       if (ruleptr == 0)
			 /* nouvelle regle */
			 {
			   /* numero de regle */
			   currule = AsciiToInt (wind, wlen);	
			   if (currule > MAX_RULES)
			     {
			       /* table des regles saturee */
			       CompilerMessage (wind, COMPIL, FATAL,
						NO_SPACE_LEFT_IN_GRAMMAR_TABLE,
						inputLine, LineNum);
			       currule = MAX_RULES;
			     }
			   GramRule[currule - 1][0] = 0;
			   ruleptr = 1;
			   if (currule > NGramRules)
			     NGramRules = currule;
			 }
		       else
			 {
			   /* on est dans une regle */
			   GramRule[currule - 1][ruleptr] = AsciiToInt (wind,
									wlen);
			   if (GramRule[currule - 1][ruleptr] == 2000)
			     ruleptr = 0;	/* fin regle */
			   else if (ruleptr >= RULE_LENGTH)
				/* regle trop longue */
			     CompilerMessage (wind, COMPIL, FATAL,
					      GRAMMAR_RULE_SIZE_EXCEEDED,
					      inputLine, LineNum);
			   else
			     ruleptr++;
			 }
		     else
		       /* ce n'est pas un nombre */
		       if (wind > 0)
			 /* fichier incorrect */
			 CompilerMessage (wind, COMPIL, FATAL,
					  INCORR_GRAMMAR_FILE_GRM, inputLine,
					  LineNum);
		   }
		 j = wind + wlen;	/* fin du mot */
	       }
	     while (wind != 0);	/* plus de mot dans la ligne */
	}
      TtaReadClose (grmFile);
    }
}
