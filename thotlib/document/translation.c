
/* -- Copyright (c) 1990 - 1996 Inria/CNRS  All rights reserved. -- */

/* ====================================================================== */
/* |                                                                    | */
/* |                           Projet THOT                              | */
/* |                                                                    | */
/* |            Ce programme lit un fichier au format Pivot             | */
/* |            et le traduit en suivant un schema de traduction.       | */
/* |            Les schemas de traduction sont ecrits en langage T      | */
/* |            et compiles par le compilateur tra, qui produit         | */
/* |            les tables de traduction utilisees ici.                 | */
/* |                                                                    | */
/* |                    V. Quint        Janvier 1988                    | */
/* |                                                                    | */
/* |            France Logiciel no de depot 88-39-001-00                | */
/* |                                                                    | */
/* ====================================================================== */


/* Module Exportateur */

#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "consttra.h"
#include "typemedia.h"
#include "typetra.h"
#include "language.h"
#include "storage.h"
#include "libmsg.h"
#include "appaction.h"
#include "app.h"

#undef EXPORT
#define EXPORT extern
#include "environ.var"
#include "thotcolor.var"
#include "select.var"
#include "edit.var"

#define LgMaxOutBuffer 1000	/* longueur maximum du buffer de sortie */

typedef struct _fichSec
  {
     char                SecFileName[MAX_PATH];		/* nom du fichier */
     FILE               *SecFileDesc;	/* descripteur du fichier */
     int                 SecLgOutBuffer;	/* longueur occupee dans le buffer
						   de sortie */
     char                SecOutBuffer[LgMaxOutBuffer];	/* buffer de sortie */
  }
fichSec;

static int          NbFichierSortie = 0;

#define MaxFichierSortie 10
/* entree 0 : stdout    */
/* entree 1 : fichier de sortie principal */
/* entrees suivantes : fichiers de sortie secondaires */
static fichSec      FichierSortie[MaxFichierSortie];
static char         tra_filedir[MAX_PATH];	/* nom du directory  de sortie */
static char         tra_filename[MAX_PATH];	/* nom du fichier de sortie principal */
static char         tra_extension[MAX_PATH];	/* extension du fichier a traduire */

/* procedures importees */
#include "tree_f.h"
#include "dofile_f.h"
#include "callback_f.h"
#include "translation_f.h"
#include "absboxes_f.h"
#include "memory_f.h"
#include "ouvre_f.h"
#include "readprs_f.h"
#include "references_f.h"
#include "externalref_f.h"
#include "schemas_f.h"
#include "schtrad_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "content_f.h"

/* ---------------------------------------------------------------------- */
/* | fichierSecondaire  retourne le fichier secondaire de nom fileName. | */
/* |    Si ce fichier n'est pas ouvert, il est cree' et ouvert.         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          fichierSecondaire (char *fileName, PtrDocument pDoc)

#else  /* __STDC__ */
static int          fichierSecondaire (fileName, pDoc)
char               *fileName;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 i;

   /* on cherche d'abord si ce nom de fichier est dans la table des */
   /* fichiers secondaires ouverts */
   /* on saute les deux premiers fichiers, qui sont stdout et le fichier de */
   /* sortie principal */
   for (i = 2; i < NbFichierSortie && strcmp (fileName, FichierSortie[i].SecFileName) != 0; i++) ;
   if (i < NbFichierSortie &&
       strcmp (fileName, FichierSortie[i].SecFileName) == 0)
      /* le fichier est dans la table, on retourne son rang */
      return i;
   else if (NbFichierSortie >= MaxFichierSortie)
      /* table saturee */
     {
	TtaDisplaySimpleMessage (INFO, LIB, LIB_TOO_MANY_OUTPUT_FILES);
	return 0;
     }
   else
     {
	FichierSortie[NbFichierSortie].SecFileDesc = fopen (fileName, "w");
	if (FichierSortie[NbFichierSortie].SecFileDesc == NULL)
	  {
	     TtaDisplayMessage (CONFIRM, TtaGetMessage(LIB, LIB_CANNOT_CREATE_FILE),
					    fileName);
	     return 0;
	  }
	else
	   /* fichier ouvert */
	  {
	     strcpy (FichierSortie[NbFichierSortie].SecFileName, fileName);
	     FichierSortie[NbFichierSortie].SecLgOutBuffer = 0;
	     NbFichierSortie++;
	     return (NbFichierSortie - 1);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* | Sort   ecrit le caractere c sur le terminal ou dans le fichier de  | */
/* |    sortie, selon fichier. S'il s'agit du fichier de sortie,        | */
/* |    le caractere est range' dans le buffer de sortie et ce buffer   | */
/* |    est ecrit dans le fichier des que la longueur limite des lignes | */
/* |    est atteinte.                                                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         Sort (char c, int fichier, char *outbuffer, PtrDocument pDoc,
			  boolean lineBreak)

#else  /* __STDC__ */
static void         Sort (c, fichier, outbuffer, pDoc, lineBreak)
char                c;
int                 fichier;
char               *outbuffer;
PtrDocument         pDoc;
boolean             lineBreak;

#endif /* __STDC__ */

{
   int                 i, j;
   PtrTSchema        pSchT;
   FILE               *filedesc;
   char                tmp[2];

   if (outbuffer != NULL)
      /* la sortie doit se faire dans le buffer outbuffer. On ajoute le */
      /* caractere a sortir en fin de ce buffer */
     {
	tmp[0] = c;
	tmp[1] = '\0';
	strcat (outbuffer, tmp);
     }
   else if (fichier == 0)
      /* la sortie doit se faire dans stdout. On sort le caractere */
      putchar (c);
   else
      /* sortie dans un fichier */
     {
	/* on cherche le schema de traduction du document pour acceder aux */
	/* parametres definissant la longueur de ligne et le caractere de */
	/* fin de ligne */
	pSchT = SchemaTrad (pDoc->DocSSchema);
	if (pSchT != NULL)
	  {
	     filedesc = FichierSortie[fichier].SecFileDesc;
	     if (pSchT->TsLineLength == 0)
		/* pas de longueur max. des lignes de sortie, on ecrit */
		/* directement le caractere dans le fichier de sortie */
		putc (c, filedesc);
	     else if (c == pSchT->TsEOL[0])
		/*  fin de ligne, on ecrit le contenu du buffer de sortie */
	       {
		  for (i = 1; i <= FichierSortie[fichier].SecLgOutBuffer; i++)
		     putc (FichierSortie[fichier].SecOutBuffer[i - 1], filedesc);
		  fprintf (filedesc, pSchT->TsEOL);
		  /* le buffer de sortie est vide maintenant */
		  FichierSortie[fichier].SecLgOutBuffer = 0;
	       }
	     else
		/* ce n'est pas un caractere de fin de ligne */
	       {
		  if (FichierSortie[fichier].SecLgOutBuffer >= LgMaxOutBuffer)
		     /* le buffer de sorite est plein, on ecrit son contenu */
		    {
		       for (i = 1; i <= FichierSortie[fichier].SecLgOutBuffer; i++)
			  putc (FichierSortie[fichier].SecOutBuffer[i - 1], filedesc);
		       FichierSortie[fichier].SecLgOutBuffer = 0;
		    }
		  /* on met le caractere dans le buffer */
		  FichierSortie[fichier].SecLgOutBuffer++;
		  FichierSortie[fichier].SecOutBuffer[FichierSortie[fichier].SecLgOutBuffer - 1] = c;
		  if (lineBreak)
		     if (FichierSortie[fichier].SecLgOutBuffer > pSchT->TsLineLength)
			/* le contenu du buffer depasse la longueur de ligne maximum */
			/* on cherche le dernier blanc */
		       {
			  i = FichierSortie[fichier].SecLgOutBuffer;
			  while (FichierSortie[fichier].SecOutBuffer[i - 1] != ' ' && i > 1)
			     i--;
			  if (FichierSortie[fichier].SecOutBuffer[i - 1] == ' ')
			     /* on ecrit tout ce qui precede ce blanc */
			    {
			       for (j = 1; j <= i - 1; j++)
				  putc (FichierSortie[fichier].SecOutBuffer[j - 1], filedesc);
			       fprintf (filedesc, pSchT->TsTranslEOL);
			       /* on decale ce qui suit le blanc */
			       FichierSortie[fichier].SecLgOutBuffer -= i;
			       for (j = 1; j <= FichierSortie[fichier].SecLgOutBuffer; j++)
				  FichierSortie[fichier].SecOutBuffer[j - 1] = FichierSortie[fichier].SecOutBuffer[i + j - 1];
			    }
		       }
	       }
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* | SortCouleur        sort dans fichier le nom de la couleur qui se   | */
/* |     trouve au rang n dans la table des couleurs.                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         SortCouleur (int n, int fichier, PtrDocument pDoc, boolean lineBreak)
#else  /* __STDC__ */
static void         SortCouleur (n, fichier, pDoc, lineBreak)
int                 n;
int                 fichier;
PtrDocument         pDoc;
boolean             lineBreak;

#endif /* __STDC__ */
{
   int                 i;
   char               *ptr;

   if (n < NbColors && n >= 0)
     {
	ptr = Color_Table[n];
	i = 0;
	while (ptr[i] != '\0')
	   Sort (ptr[i++], fichier, NULL, pDoc, lineBreak);
     }
}

/* ---------------------------------------------------------------------- */
/* | SortMotif  sort dans fichier le nom du motif qui se trouve au rang | */
/* |    n dans la table des motifs.                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         SortMotif (int n, int fichier, PtrDocument pDoc, boolean lineBreak)
#else  /* __STDC__ */
static void         SortMotif (n, fichier, pDoc, lineBreak)
int                 n;
int                 fichier;
PtrDocument         pDoc;
boolean             lineBreak;

#endif /* __STDC__ */
{
   int                 i;
   char               *ptr;

   if (n < NbPatterns && n >= 0)
     {
	ptr = Patterns[n];
	i = 0;
	while (ptr[i] != '\0')
	   Sort (ptr[i++], fichier, NULL, pDoc, lineBreak);
     }
}

/* ---------------------------------------------------------------------- */
/* | SortNb convertit le nombre n sous la forme d'une chaine de         | */
/* |    caracteres et sort cette chaine de caracteres dans fichier      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         SortNb (int n, int fichier, char *outbuffer, PtrDocument pDoc,
			    boolean lineBreak)

#else  /* __STDC__ */
static void         SortNb (n, fichier, outbuffer, pDoc, lineBreak)
int                 n;
int                 fichier;
char               *outbuffer;
PtrDocument         pDoc;
boolean             lineBreak;

#endif /* __STDC__ */

{
   char                buffer[20];
   int                 i;

   sprintf (buffer, "%d", n);
   i = 0;
   while (buffer[i] != '\0')
      Sort (buffer[i++], fichier, outbuffer, pDoc, lineBreak);
}

/* ---------------------------------------------------------------------- */
/* | TraduitFeuille   traite l'element feuille pointe' par pEl, en      | */
/* |    traduisant son contenu si tradCar est vrai. Produit le contenu  | */
/* |    dans le fichier de sortie fichier.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         TraduitFeuille (PtrElement pEl, boolean tradCar, boolean lineBreak,
				    int fichier, PtrDocument pDoc)

#else  /* __STDC__ */
static void         TraduitFeuille (pEl, tradCar, lineBreak, fichier, pDoc)
PtrElement          pEl;
boolean             tradCar;
boolean             lineBreak;
int                 fichier;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrTextBuffer      pBuffT, pBuffTs, pBuffTPrec;
   char                cs, c;
   int                 i, j, k, b, ft, lt;
   PtrTSchema        pSchT;
   PtrSSchema        pSS;
   boolean             __continue, egal, stop, tradexiste;
   PtrElement          pElAsc;
   int                 TradTxtDebut, TradTxtFin;
   PtrTSchema        pSc1;
   AlphabetTransl            *pTr1;
   PtrElement          pEl1;
   PtrTextBuffer      pBu1;
   StringTransl            *pTrad1;

   TradTxtDebut = 0;
   TradTxtFin = 0;
   if (!(pEl->ElLeafType == LtText || pEl->ElLeafType == LtSymbol ||
	 pEl->ElLeafType == LtGraphics || pEl->ElLeafType == LtPlyLine)
       || !tradCar)
      pSchT = SchemaTrad (pEl->ElSructSchema);
   else
      /* En examinant les elements englobants, on cherche un schema de */
      /* traduction qui contienne des regles pour ce type de feuille */
     {
	pSS = NULL;
	tradexiste = FALSE;
	pSchT = NULL;
	pElAsc = pEl;
	do
	  {
	     if (pSS != pElAsc->ElSructSchema)
		/* un schema de structure different du precedent */
	       {
		  pSS = pElAsc->ElSructSchema;
		  /* schema de traduction de cette structure */
		  pSchT = SchemaTrad (pSS);
		  if (pSchT != NULL)
		     switch (pEl->ElLeafType)
			   {
			      case LtText:
				 pSc1 = pSchT;
				 if (pSc1->TsNTranslAlphabets > 0)
				    /* il y a au moins un alphabet a traduire */
				    /* cherche les regles de traduction pour l'alphabet */
				    /* de la feuille */
				   {
				      i = 0;
				      do
					{
					   i++;
					   pTr1 = &pSc1->TsTranslAlphabet[i - 1];
					   if (pTr1->AlAlphabet == TtaGetAlphabet (pEl->ElLanguage))
					      if (pTr1->AlBegin > 0)
						{
						   tradexiste = TRUE;
						   TradTxtDebut = pTr1->AlBegin;
						   TradTxtFin = pTr1->AlEnd;
						}
					}
				      while (!(tradexiste || i >= pSc1->TsNTranslAlphabets));
				   }
				 break;
			      case LtSymbol:
				 tradexiste = pSchT->TsSymbolFirst != 0;
				 break;
			      case LtGraphics:
			      case LtPlyLine:
				 tradexiste = pSchT->TsGraphicsFirst != 0;
				 break;
			      default:
				 break;
			   }

	       }
	     pElAsc = pElAsc->ElParent;
	  }
	while (!(tradexiste || pElAsc == NULL));
     }
   pEl1 = pEl;
   switch (pEl1->ElLeafType)
	 {
	    case LtText /* traitement d'une feuille de texte */ :
	       if (pEl1->ElTextLength > 0)
		  /* la feuille n'est pas vide */
		 {
		    pBuffT = pEl1->ElText;	/* 1er buffer a traiter */
		    if (TradTxtDebut == 0 || !tradCar)
		       /* on ne traduit pas quand la table de traduction est vide */
		       /* parcourt les buffers de l'element */
		       while (pBuffT != NULL)
			 {
			    pBu1 = pBuffT;
			    i = 1;
			    while (pBu1->BuContent[i - 1] != '\0')
			      {
				 Sort (pBu1->BuContent[i - 1], fichier, NULL, pDoc, lineBreak);
				 i++;
			      }
			    pBuffT = pBu1->BuNext;
			 }
		    else if (pSchT != NULL)
		       /* effectue les traductions de caracteres selon la table */
		      {
			 pSc1 = pSchT;
			 /* indice dans la chaine source de la regle de traduction */
			 b = 0;
			 /* indice dans le buffer du caractere a traduire */
			 i = 1;
			 /* indice de la 1ere regle de traduction a appliquer */
			 ft = TradTxtDebut;
			 /* indice de la derniere regle de traduction a appliquer */
			 lt = TradTxtFin;
			 /* buffer source precedent */
			 pBuffTPrec = NULL;
			 /* 1er caractere a traduire */
			 c = pBuffT->BuContent[i - 1];
			 /* traduit la suite des buffers source */
			 do
			    /* Dans la table de traduction, les chaines sources sont */
			    /* rangees par ordre alphabetique. On cherche une chaine */
			    /* source qui commence par le caractere a traduire. */
			   {
			      while (c > pSc1->TsCharTransl[ft - 1].StSource[b] && ft < lt)
				 ft++;
			      pTrad1 = &pSc1->TsCharTransl[ft - 1];
			      if (c == pTrad1->StSource[b])
				 /* le caractere correspond au caractere courant de la */
				 /* chaine source de la regle ft */
				 if (pTrad1->StSource[b + 1] == '\0')
				    /* chaine complete */
				    /* cette regle de traduction s'applique */
				    /* on traduit */
				    /* cherche si les regles suivantes ne peuvent pas */
				    /* egalement s'appliquer: on recherche la plus */
				    /* longue chaine a traduire */
				   {
				      __continue = ft < TradTxtFin;
				      while (__continue)
					{
					   j = 0;
					   egal = TRUE;
					   /* compare la regle ft avec la suivante */
					   do
					      if (pSc1->TsCharTransl[ft - 1].StSource[j] ==
						  pSc1->TsCharTransl[ft].StSource[j])
						 j++;
					      else
						 egal = FALSE;
					   while (!(!egal || j > b));
					   if (!egal)
					      /* le debut de la regle suivante est different */
					      /* de la regle courante */
					      __continue = FALSE;
					   else
					      /* la fin de la regle suivante est-il identique */
					      /* a la suite du texte a traduire ? */
					     {
						k = i;
						cs = c;
						pBuffTs = pBuffT;
						/* cherche le caractere suivant du texte */
						stop = FALSE;
						do
						  {
						     if (cs != '\0')
						       {
							  k++;
							  cs = pBuffTs->BuContent[k - 1];
						       }
						     if (cs == '\0')
							/* passe au buffer suivant du meme texte */
							if (pBuffTs->BuNext != NULL)
							  {
							     pBuffTs = pBuffTs->BuNext;
							     k = 1;
							     cs = pBuffTs->BuContent[k - 1];
							  }
						     if (cs == '\0')
							__continue = FALSE;	/* fin du texte */
						     else
						       {
							  __continue = FALSE;
							  if (cs == pSc1->TsCharTransl[ft].
							    StSource[j])
							    {
							       stop = FALSE;
							       __continue = TRUE;
							       j++;
							    }
							  if (pSc1->TsCharTransl[ft].StSource[j] == '\0')
							    {
							       ft++;
							       b = j - 1;
							       i = k;
							       c = cs;
							       pBuffT = pBuffTs;
							       __continue = ft <
								  TradTxtFin;
							       stop = TRUE;
							    }
						       }
						  }
						while (!(stop || !__continue));
					     }
					}
				      /* on applique la regle de traduction ft */
				      j = 0;
				      while (pSc1->TsCharTransl[ft - 1].StTarget[j] != '\0')
					{
					   Sort (pSc1->TsCharTransl[ft - 1].StTarget[j],
					    fichier, NULL, pDoc, lineBreak);
					   j++;
					}
				      /* prepare la prochaine recherche dans la table */
				      b = 0;
				      ft = TradTxtDebut;
				      lt = TradTxtFin;
				   }
				 else
				    /* ce n'est pas le dernier caractere de la chaine */
				    /* csource de la table de traduction, on restreint la */
				    /* partie de la table de traduction dans laquelle on */
				    /* cherchera les caracteres suivants */
				   {
				      j = ft;
				      /* cherche parmi les regles suivantes la derniere */
				      /* qui contienne ce caractere a cette position dans */
				      /* la chaine source. On ne cherchera pas au-dela de */
				      /* cette regle. */
				      while (c == pSc1->TsCharTransl[j - 1].StSource[b] && j <
					     lt)
					 j++;
				      if (c != pSc1->TsCharTransl[j - 1].StSource[b])
					 lt = j - 1;
				      /* passe au caractere suivant de la chaine source */
				      /* de la table de traduction */
				      b++;
				   }
			      else
				 /* le caractere ne correspond pas */
			      if (b == 0)
				 /* le caractere ne se trouve au debut d'aucune */
				 /* chaine source de la table de traduction, on ne le */
				 /* traduit donc pas */
				{
				   ft = TradTxtDebut;
				   Sort (c, fichier, NULL, pDoc, lineBreak);
				}
			      else
				 /* on avait commence' a analyser une sequence de */
				 /* caracteres. Cette sequence ne se traduit pas, on */
				 /* sort le premier caractere de la sequence et on */
				 /* cherche une sequence traduisible a partir du */
				 /* caractere suivant. */
				{
				   if (i - b >= 1)
				      /* le premier caractere de la sequence est dans */
				      /* le buffer courant */
				      i -= b;
				   else
				      /* le premier caractere de la sequence est dans */
				      /* le buffer precedent */
				     {
					pBuffT = pBuffTPrec;
					i = pBuffT->BuLength + i - b;
				     }
				   Sort (pBuffT->BuContent[i - 1], fichier, NULL, pDoc,
					 lineBreak);
				   b = 0;
				   ft = TradTxtDebut;
				   lt = TradTxtFin;
				}
			      /* cherche le caractere suivant a traiter */
			      if (c != '\0')
				{
				   i++;
				   c = pBuffT->BuContent[i - 1];
				}
			      if (c == '\0')
				 /* passe au buffer suivant du meme element de texte */
				 if (pBuffT->BuNext != NULL)
				   {
				      pBuffTPrec = pBuffT;
				      pBuffT = pBuffT->BuNext;
				      i = 1;
				      c = pBuffT->BuContent[i - 1];
				   }
			   }
			 while (!(c == '\0'));
			 /* fin de la feuille de texte */
			 /* Si on a commence' a analyser une sequence de caracteres, */
			 /* on sort le debut de la sequence. */
			 for (i = 0; i <= b - 1; i++)
			    Sort (pSc1->TsCharTransl[ft - 1].StSource[i], fichier, NULL,
				  pDoc, lineBreak);
		      }
		 }
	       break;
	    case LtSymbol:
	    case LtGraphics:
	    case LtPlyLine:
	       if (pSchT != NULL)
		 {
		    pSc1 = pSchT;
		    if (!tradCar)
		       ft = 0;
		    else
		       /* cherche la premiere et la derniere regle de traduction */
		       /* a appliquer a l'element */
		    if (pEl1->ElLeafType == LtSymbol)
		      {
			 ft = pSc1->TsSymbolFirst;
			 lt = pSc1->TsSymbolLast;
		      }
		    else
		      {
			 ft = pSc1->TsGraphicsFirst;
			 lt = pSc1->TsGraphicsLast;
		      }
		    /* prend dans c le caractere qui represente la forme graphique */
		    if (pEl1->ElLeafType == LtPlyLine)
		       c = pEl1->ElPolyLineType;
		    else
		       c = pEl1->ElGraph;
		    if (ft == 0)
		       /* pas de traduction */
		       Sort (c, fichier, NULL, pDoc, lineBreak);
		    else
		       /* on traduit l'element */
		       /* cherche le symbole dans les chaines sources de la */
		       /* table de traduction */
		      {
			 while (pSc1->TsCharTransl[ft - 1].StSource[0] < c && ft < lt)
			    ft++;
			 if (pSc1->TsCharTransl[ft - 1].StSource[0] == c)
			    /* il y a une regle de traduction pour ce symbole */
			   {
			      b = 0;
			      pTrad1 = &pSc1->TsCharTransl[ft - 1];
			      while (pTrad1->StTarget[b] != '\0')
				{
				   Sort (pTrad1->StTarget[b], fichier, NULL, pDoc, lineBreak);
				   b++;
				}
			   }
			 else
			    /* ce symbole ne se traduit pas */
			    Sort (c, fichier, NULL, pDoc, lineBreak);
		      }
		    if (pEl1->ElLeafType == LtPlyLine)
		       if (pEl1->ElNPoints > 0)
			  /* la ligne a au moins un point de controle */
			  /* on ecrit les coordonnees des points de controle */
			 {
			    pBuffT = pEl1->ElPolyLineBuffer;	/* 1er buffer a traiter */
			    /* parcourt les buffers de l'element */
			    while (pBuffT != NULL)
			      {
				 for (i = 0; i < pBuffT->BuLength; i++)
				   {
				      Sort (' ', fichier, NULL, pDoc, lineBreak);
				      SortNb (pBuffT->BuPoints[i].XCoord, fichier, NULL,
					      pDoc, lineBreak);
				      Sort (',', fichier, NULL, pDoc, lineBreak);
				      SortNb (pBuffT->BuPoints[i].YCoord, fichier, NULL,
					      pDoc, lineBreak);
				   }
				 pBuffT = pBuffT->BuNext;
			      }
			 }
		 }
	       break;
	    case LtPicture:
	       /* Si le schema de traduction comporte un buffer */
	       /* pour les images, le nom du fichier contenant l'image */
	       /* est range' dans ce buffer */
	       if (pSchT != NULL)
		  if (pSchT->TsPictureBuffer > 0)
		    {
		       b = pSchT->TsPictureBuffer;
		       pSchT->TsBuffer[b - 1][0] = '\0';	/* raz du buffer */
		       if (pEl1->ElTextLength > 0)
			  /* la feuille n'est pas vide */
			 {
			    j = 0;
			    pBuffT = pEl1->ElText;	/* 1er buffer a traiter */
			    /* parcourt les buffers de l'element */
			    while (pBuffT != NULL)
			      {
				 pBu1 = pBuffT;
				 i = 0;
				 do
				   {
				      i++;
				      j++;
				      pSchT->TsBuffer[b - 1][j - 1] = pBu1->BuContent[i - 1];
				   }
				 while (!(pBu1->BuContent[i - 1] == '\0' || b >= MAX_TRANSL_BUFFER_LEN));
				 pBuffT = pBu1->BuNext;
			      }
			    if (j > 0)
			       pSchT->TsBuffer[b - 1][j - 1] = '\0';
			 }
		    }
	       break;
	    case LtPageColBreak:

	       break;		/* On ne fait rien */
	    case LtReference:

	       break;		/* On ne fait rien */
	    default:
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* | ValCaractPres      retourne le code caractere de la valeur de la   | */
/* |    regle de presentation specifique pointee par pPres.             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static char         ValCaractPres (PtrPRule pPres)

#else  /* __STDC__ */
static char         ValCaractPres (pPres)
PtrPRule        pPres;

#endif /* __STDC__ */

{
   char                val;

   val = ' ';
   switch (pPres->PrType)
	 {
	    case PtFont:
	    case PtStyle:
	    case PtUnderline:
	    case PtThickness:
	    case PtLineStyle:
	       val = pPres->PrChrValue;
	       break;
	    case PtJustify:
	    case PtHyphenate:
	       if (pPres->PrJustify)
		  val = 'Y';
	       else
		  val = 'N';
	       break;
	    case PtAdjust:
	       switch (pPres->PrAdjust)
		     {
			case AlignLeft:
			   val = 'L';
			   break;
			case AlignRight:
			   val = 'R';
			   break;
			case AlignCenter:
			   val = 'C';
			   break;
			case AlignLeftDots:
			   val = 'D';
			   break;
		     }
	       break;
	    default:
	       val = ' ';
	       break;
	 }
   return val;
}


/**** debut ajout ****/
/* ---------------------------------------------------------------------- */
/* | ElementVide        retourne TRUE si l'element pEl est vide ou n'a  | */
/* |    que des descendants vides.                                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      ElementVide (PtrElement pEl)

#else  /* __STDC__ */
static boolean      ElementVide (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pFils;
   boolean             elvide;

   elvide = TRUE;
   if (pEl->ElTerminal)
      /* l'element est une feuille. On traite selon le type de feuille */
      switch (pEl->ElLeafType)
	    {
	       case LtText:
	       case LtPicture:
		  elvide = (pEl->ElTextLength == 0);
		  break;
	       case LtGraphics:
	       case LtSymbol:
		  elvide = (pEl->ElGraph == '\0');
		  break;
	       case LtPageColBreak:
		  /* une marque de page est consideree comme vide */
		  elvide = TRUE;
		  break;
	       case LtReference:
		  if (pEl->ElReference != NULL)
		     if (pEl->ElReference->RdReferred != NULL)
			elvide = FALSE;
		  break;
	       case LtPairedElem:
		  /* un element de paire n'est jamais considere's comme vide */
		  elvide = FALSE;
		  break;
	       case LtPlyLine:
		  elvide = (pEl->ElNPoints == 0);
		  break;
	       default:
		  elvide = FALSE;
		  break;
	    }
   else
      /* ce n'est pas une feuille, on traite recursivement tous les fils */
     {
	pFils = pEl->ElFirstChild;
	while (pFils != NULL && elvide)
	   if (!ElementVide (pFils))
	      elvide = FALSE;
	   else
	      pFils = pFils->ElNext;
     }
   return elvide;
}
/**** fin ajout ****/

/* ---------------------------------------------------------------------- */
/* |    FirstSibling    returns TRUE if element pEl is the first child  | */
/* |            of its parent, ignoring PageBreak elements.             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      FirstSibling (PtrElement pEl)

#else  /* __STDC__ */
static boolean      FirstSibling (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   boolean             stop;
   PtrElement          pVois;

   /* on saute les marques de page precedentes */
   pVois = pEl->ElPrevious;
   stop = FALSE;
   do
      if (pVois == NULL)
	 stop = TRUE;
      else if (pVois->ElTypeNumber == PageBreak + 1)
	 pVois = pVois->ElPrevious;
      else
	 stop = TRUE;
   while (!(stop));
   /* l'element est premier s'il n'a pas de predecesseur */
   return (pVois == NULL);
}

/* ---------------------------------------------------------------------- */
/* | CondSatisfaite   evalue la condition du bloc de regles pointe' par | */
/* |    pBloc pour l'element pointe' par pEl et l'attribut pointe' par  | */
/* |    pA s'il est different de NULL. Retourne vrai si la condition    | */
/* |    satisfaite, faux sinon.                                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      CondSatisfaite (PtrTRuleBlock pBloc, PtrElement pEl,
				    PtrAttribute pA, PtrDocument pDoc)

#else  /* __STDC__ */
static boolean      CondSatisfaite (pBloc, pEl, pA, pDoc)
PtrTRuleBlock       pBloc;
PtrElement          pEl;
PtrAttribute         pA;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   boolean             ret, refpossible, typeOK, stop;
   PtrAttribute         pAttr;
   PtrSSchema        pSchS;
   int                 i, par;
   PtrElement          pEl1;
   PtrElement          pElem;
   PtrElement          pVois;
   PtrSSchema        pSc1;
   SRule              *pRe1;
   PtrAttribute         pAt1;
   int                 ncond;
   TranslCondition        *Cond;
   PtrPRule        pPres;
   PtrReference        pRef;
   DocumentIdentifier     IDocExt;
   PtrDocument         pDocExt;

   if (pBloc->TbNConditions == 0)
      /* sans condition */
      ret = TRUE;
   else
      /* il y a au moins une condition a evaluer */
     {
	ret = TRUE;
	ncond = 0;
	/* evalue les conditions du bloc jusqu'a en trouver une fausse */
	while (ret && ncond < pBloc->TbNConditions)
	  {
	     ncond++;
	     Cond = &pBloc->TbCondition[ncond - 1];
	     if (!Cond->TcTarget)
		pElem = pEl;
	     else
		/* la condition porte sur l'element pointe' par pEl ou pA.
		   on cherche cet element pointe' */
	       {
		  pElem = NULL;
		  if (pA != NULL &&
		  pA->AeAttrSSchema->SsAttribute[pA->AeAttrNum - 1].AttrType ==
		      AtReferenceAttr)
		     /* c'est un attribut reference */
		     pRef = pA->AeAttrReference;
		  else
		     /* l'element est-il une reference ? */
		  if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
		     pRef = pEl->ElReference;
		  else
		     /* c'est peut-etre une inclusion */
		     pRef = pEl->ElSource;
		  if (pRef != NULL)
		     pElem = ReferredElement (pRef, &IDocExt, &pDocExt);
	       }
	     if (pElem == NULL)
		ret = FALSE;
	     else
	       {
		  /* on cherche l'element concerne' par la condition */
		  if (Cond->TcAscendRelLevel > 0)
		     /* la condition concerne l'ascendant de rang TcAscendRelLevel */
		     for (i = 1; pElem != NULL && i <= Cond->TcAscendRelLevel; i++)
			pElem = pElem->ElParent;
		  else if (Cond->TcAscendType != 0)
		     /* la condition concerne l'element ascendant de type
		        TcAscendType */
		    {
		       typeOK = FALSE;
		       pElem = pElem->ElParent;
		       while (!typeOK && pElem != NULL)
			 {
			    if (Cond->TcAscendNature[0] == '\0')
			       /* le type de l'ascendant est defini dans le meme schema de
			          structure que l'element traite' */
			       typeOK = EquivalentSRules (Cond->TcAscendType, pEl->ElSructSchema,
						    pElem->ElTypeNumber, pElem->ElSructSchema, pElem->ElParent);
			    else
			       /* le type de l'ascendant est defini dans un autre schema */
			       if (strcmp (Cond->TcAscendNature,
					pElem->ElSructSchema->SsName) == 0)
			       typeOK = EquivalentSRules (Cond->TcAscendType,
					  pElem->ElSructSchema, pElem->ElTypeNumber,
					 pElem->ElSructSchema, pElem->ElParent);
			    pElem = pElem->ElParent;
			 }
		    }
		  if (pElem == NULL)
		     ret = FALSE;
		  else
		     /* traitement selon le type de condition */
		     switch (Cond->TcCondition)
			   {
			      case TcondFirst:
				 ret = FirstSibling (pElem);
				 break;

			      case TcondLast:
				 /* on saute les marques de page suivantes */
				 pVois = pElem->ElNext;
				 stop = FALSE;
				 do
				    if (pVois == NULL)
				       stop = TRUE;
				    else if (pVois->ElTypeNumber == PageBreak + 1)
				       pVois = pVois->ElNext;
				    else
				       stop = TRUE;
				 while (!(stop));
				 /* l'element est dernier s'il n'a pas de succcesseur */
				 ret = pVois == NULL;
				 break;

			      case TcondDefined:
				 /* la condition est satisfaite si l'element, qui est */
				 /* un parametre, a une valeur */
				 /* parcourt les parametres du document */
				 par = 0;
				 ret = FALSE;
				 do
				   {
				      par++;
				      if (pDoc->DocParameters[par - 1] != NULL)
					{
					   pEl1 = pDoc->DocParameters[par - 1];
					   ret = pEl1->ElTypeNumber == pElem->ElTypeNumber &&
					      pEl1->ElSructSchema->SsCode == pElem->ElSructSchema->SsCode;
					}
				   }
				 while (!(ret || par >= MAX_PARAM_DOC));
				 break;

			      case TcondReferred:
				 /* la condition est satisfaite si l'element (ou le */
				 /* premier de ses ascendants sur lequel peut porter une */
				 /* reference) est reference' au moins une fois. */
				 ret = FALSE;
				 pEl1 = pElem;
				 refpossible = FALSE;

				 do
				   {
				      if (pEl1->ElReferredDescr != NULL)
					 /* l'element est reference' */
					 ret = pEl1->ElReferredDescr->ReFirstReference != NULL;
				      if (Cond->TcAscendRelLevel > -1)
					 /* on s'interesse seulement a l'element pElem. On ne va pas
					    chercher plus loin. */
					 pEl1 = NULL;
				      else
					 /* on cherche le premier ascendant referencable si l'element
					    pElem lui-meme n'est pas reference' */
				      if (!ret)
					 /* l'element n'est pas reference' */
					{
					   /* l'element peut-il etre designe' par une reference? */
					   pSc1 = pEl1->ElSructSchema;
					   /* on cherche toutes les references dans le schema de */
					   /* structure de l'element */
					   refpossible = FALSE;
					   i = 1;
					   do
					     {
						i++;
						pRe1 = &pSc1->SsRule[i - 1];
						if (pRe1->SrConstruct == CsReference)
						   /* c'est une reference */
						   if (pRe1->SrReferredType != 0)
						      refpossible = EquivalentSRules (pRe1->SrReferredType, pSc1, pEl1->ElTypeNumber, pSc1, pEl1->ElParent);
					     }
					   while (!(refpossible || i >= pSc1->SsNRules));
					   if (!refpossible)
					     {
						/* l'element ne peut pas etre designe par un element */
						/* reference on cherche s'il peut etre designe' par un */
						/* attribut reference on cherche tous les attributs */
						/* reference dans le schema de structure de l'element */
						i = 1;
						do
						  {
						     i++;
						     if (pSc1->SsAttribute[i - 1].AttrType == AtReferenceAttr)
							/* c'est une reference */
							if (pSc1->SsAttribute[i - 1].AttrTypeRef != 0)
							   refpossible = (pSc1->SsAttribute[i - 1].AttrTypeRefNature[0] == '\0'	/* meme schema de structure */
									  && EquivalentSRules (pSc1->SsAttribute[i - 1].AttrTypeRef, pSc1, pEl1->ElTypeNumber, pSc1, pEl1->ElParent));
						  }
						while (!(refpossible || i >= pSc1->SsNAttributes));
					     }
					   if (!refpossible)
					      /* l'element ne peut pas etre designe' par une reference
					         type'e ; on examine l'element ascendant */
					      pEl1 = pEl1->ElParent;
					}
				   }
				 while (!refpossible && pEl1 != NULL && !ret);
				 break;

			      case TcondFirstRef:
			      case TcondLastRef:
				 /* la condition est satisfaite s'il s'agit de la premiere */
				 /* (ou de la derniere) reference a l'element reference' */
				 pRef = NULL;
				 if (pA != NULL &&
				     pA->AeAttrSSchema->SsAttribute[pA->AeAttrNum - 1].AttrType == AtReferenceAttr)
				    /* c'est un attribut reference */
				    pRef = pA->AeAttrReference;
				 else if (pElem->ElTerminal && pElem->ElLeafType == LtReference)
				    /* l'element est une reference */
				    pRef = pElem->ElReference;
				 if (pRef != NULL)
				    if (Cond->TcCondition == TcondFirstRef)
				       ret = pRef->RdPrevious == NULL;
				    else
				       ret = pRef->RdNext == NULL;
				 else
				    ret = FALSE;
				 break;

			      case TcondExternalRef:
				 /* la condition est satisfaite s'il s'agit d'un */
				 /* element ou d'un attribut reference externe */
				 pRef = NULL;
				 ret = FALSE;
				 if (pA != NULL &&
				     pA->AeAttrSSchema->SsAttribute[pA->AeAttrNum - 1].AttrType == AtReferenceAttr)
				    /* c'est un attribut reference */
				    pRef = pA->AeAttrReference;
				 else
				    /* l'element est-il une reference ? */
				 if (pElem->ElTerminal && pElem->ElLeafType == LtReference)
				    pRef = pElem->ElReference;
				 else
				    /* c'est peut-etre une inclusion */
				    pRef = pElem->ElSource;
				 if (pRef != NULL)
				    if (pRef->RdReferred != NULL)
				       if (pRef->RdReferred != NULL)
					  if (pRef->RdReferred->ReExternalRef)
					     ret = TRUE;
				 break;
			      case TcondFirstWithin:
			      case TcondWithin:
				 /* condition sur le nombre d'ancetres d'un type donne' */
				 pEl1 = pElem->ElParent;
				 if (pEl1 == NULL)
				    /* aucun ancetre, condition non satisfaite */
				    ret = FALSE;
				 else
				   {
				      if (Cond->TcCondition == TcondFirstWithin)
					 ret = FirstSibling (pElem);
				      if (ret)
					{
					   if (Cond->TcElemNature[0] == '\0')
					      /* le type de l'ascendant est defini dans le meme schema de
					         structure que l'element traite' */
					      pSchS = pEl->ElSructSchema;
					   else
					      /* le type de l'ascendant est defini dans un autre schema */
					      pSchS = NULL;
					   i = 0;
					   if (Cond->TcImmediatelyWithin)
					      /* Condition: If immediately within n element-type */
					      /* Les n premiers ancetres successifs doivent etre du type
					         TcElemType, sans comporter d'elements d'autres type */
					      /* on compte les ancetres successifs de ce type */
					      while (pEl1 != NULL)
						{
						   if (pSchS != NULL)
						      typeOK = EquivalentSRules (Cond->TcElemType, pSchS,
									   pEl1->ElTypeNumber, pEl1->ElSructSchema, pEl1->ElParent);
						   else if (strcmp (Cond->TcElemNature, pEl1->ElSructSchema->SsName) == 0)
						      typeOK = EquivalentSRules (Cond->TcElemType,
									   pEl1->ElSructSchema, pEl1->ElTypeNumber, pEl1->ElSructSchema, pEl1->ElParent);
						   else
						      typeOK = FALSE;
						   if (typeOK)
						     {
							i++;
							pEl1 = pEl1->ElParent;
						     }
						   else
						      pEl1 = NULL;
						}
					   else
					      /* Condition: If within n element-type */
					      /* on compte tous les ancetres de ce type */
					      while (pEl1 != NULL)
						{
						   if (pSchS != NULL)
						      typeOK = EquivalentSRules (Cond->TcElemType, pSchS,
									   pEl1->ElTypeNumber, pEl1->ElSructSchema, pEl1->ElParent);
						   else if (strcmp (Cond->TcElemNature, pEl1->ElSructSchema->SsName) == 0)
						      typeOK = EquivalentSRules (Cond->TcElemType,
									   pEl1->ElSructSchema, pEl1->ElTypeNumber, pEl1->ElSructSchema, pEl1->ElParent);
						   else
						      typeOK = FALSE;
						   if (typeOK)
						      i++;
						   pEl1 = pEl1->ElParent;		/* passe a l'element ascendant */
						}
					   if (Cond->TcAscendRel == CondEquals)
					      ret = i == Cond->TcAscendLevel;
					   else if (Cond->TcAscendRel == CondGreater)
					      ret = i > Cond->TcAscendLevel;
					   else if (Cond->TcAscendRel == CondLess)
					      ret = i < Cond->TcAscendLevel;
					}
				   }
				 break;

			      case TcondAttr:
				 /* cherche si l'element ou un de ses ascendants possede */
				 /* l'attribut cherche' avec la valeur cherchee */
				 ret = FALSE;
				 pSchS = pEl->ElSructSchema;
				 pEl1 = pElem;
				 while (pEl1 != NULL && !ret)
				   {
				      pAttr = pEl1->ElFirstAttr;		/* 1er attribut de l'element */
				      /* parcourt les attributs de l'element */
				      while (pAttr != NULL && !ret)
					{
					   pAt1 = pAttr;
					   if (pAt1->AeAttrSSchema == pSchS &&
					   pAt1->AeAttrNum == Cond->TcAttr)
					      /* c'est l'attribut cherche', on teste sa valeur */
					      switch (pSchS->SsAttribute[pAt1->AeAttrNum - 1].AttrType)
						    {
						       case AtNumAttr:
							  ret = pAt1->AeAttrValue <= Cond->TcUpperBound &&
							     pAt1->AeAttrValue >= Cond->TcLowerBound;
							  break;
						       case AtTextAttr:
							  if (Cond->TcTextValue[0] == '\0')
							     ret = TRUE;
							  else
							     ret = StringAndTextEqual (Cond->TcTextValue,
										       pAt1->AeAttrText);
							  break;
						       case AtReferenceAttr:
							  ret = TRUE;
							  break;
						       case AtEnumAttr:
							  ret = pAt1->AeAttrValue == Cond->TcAttrValue ||
							     Cond->TcAttrValue == 0;
							  break;
						       default:
							  break;
						    }
					   if (!ret)
					      pAttr = pAt1->AeNext;
					}
				      if (Cond->TcAscendRelLevel == -1)
					 /* on peut poursuivre la recherche parmi les ascendants */
					 pEl1 = pEl1->ElParent;	/* passe a l'element ascendant */
				      else
					 /* la recherche de l'attribut ne porte que sur l'element
					    lui-meme. On arrete la. */
					 pEl1 = NULL;
				   }
				 break;
			      case TcondPRule:
				 /* cherche si l'element possede la presentation cherchee */
				 /* avec la valeur cherchee */
				 ret = FALSE;
				 pPres = pElem->ElFirstPRule;
				 /* parcourt les presentations specifiques de l'element */
				 while (pPres != NULL && !ret)
				   {
				      if (pPres->PrType == Cond->TcAttr)
					 /* c'est la presentation cherchee, on teste sa valeur */
					 if (pPres->PrType == PtSize || pPres->PrType == PtIndent ||
					     pPres->PrType == PtLineSpacing ||
					   pPres->PrType == PtLineWeight)
					    /* c'est une presentation a valeur numerique */
					    ret = pPres->PrMinValue <= Cond->TcUpperBound &&
					       pPres->PrMinValue >= Cond->TcLowerBound;
					 else if (pPres->PrType == PtFillPattern ||
					    pPres->PrType == PtBackground ||
					     pPres->PrType == PtForeground)
					    ret = pPres->PrIntValue <= Cond->TcUpperBound &&
					       pPres->PrIntValue >= Cond->TcLowerBound;
					 else
					    ret = Cond->TcPresValue == ValCaractPres (pPres) ||
					       Cond->TcPresValue == '\0';
				      if (!ret)
					 pPres = pPres->PrNextPRule;
				   }
				 break;
			      case TcondPresentation:
				 /* la condition est satisfaite si l'element */
				 /* porte des regles de presentation specifique */
				 ret = pElem->ElFirstPRule != NULL;
				 break;
			      case TcondComment:
				 /* la condition est satisfaite si l'element */
				 /* porte des commentaires */
				 ret = pElem->ElComment != NULL;
				 break;
				 case TcondAlphabet
			      /* la condition porte sur l'alphabet */ :
				 if (pElem->ElTypeNumber == CharString + 1)
				    ret = (TtaGetAlphabet (pElem->ElLanguage) == Cond->TcAlphabet);
				 break;
			      case TcondAttributes:
				 /* la condition est satisfaite si l'element */
				 /* porte des attributs */
				 ret = pElem->ElFirstAttr != NULL;
				 break;
			      case TcondFirstAttr:
				 /* la condition est satisfaite si le bloc */
				 /* attribut pA est le 1er de l'element */
				 if (pA != NULL)
				    ret = pA == pEl->ElFirstAttr;
				 break;
			      case TcondLastAttr:
				 /* la condition est satisfaite si le bloc */
				 /* attribut pA est le dernier de l'element */
				 if (pA != NULL)
				    ret = pA->AeNext == NULL;
				 break;
			      case TcondComputedPage:
				 /* la condition est satisfaite si l'element
				    est un saut de page calcule */
				 if (pElem->ElTypeNumber == PageBreak + 1)
				    ret = pElem->ElPageType == PgComputed;
				 break;
			      case TcondStartPage:
				 /* la condition est satisfaite si l'element
				    est un saut de page de debut */
				 if (pElem->ElTypeNumber == PageBreak + 1)
				    ret = pElem->ElPageType == PgBegin;
				 break;
			      case TcondUserPage:
				 /* la condition est satisfaite si l'element
				    est un saut de page utilisateur */
				 if (pElem->ElTypeNumber == PageBreak + 1)
				    ret = pElem->ElPageType == PgUser;
				 break;
				 /* cas page rappel supprime */
			      case TcondEmpty:
				 /* la condition est satisfaite si l'element est vide */
				 ret = ElementVide (pElem);
				 break;
			      default:
				 break;
			   }
		  if (Cond->TcNegativeCond)
		     ret = !ret;
	       }
	  }
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* | ComptValTra     retourne la valeur du compteur de numero NCompt    | */
/* |    (defini dans le schema de traduction  pointe' par pSchT qui     | */
/* |    s'applique au schema de structure pointe' par pSS) pour         | */
/* |    l'element pointe' par pElNum.                                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          ComptValTra (int NCompt, PtrTSchema pSchT, PtrSSchema pSS, PtrElement pElNum)

#else  /* __STDC__ */
static int          ComptValTra (NCompt, pSchT, pSS, pElNum)
int                 NCompt;
PtrTSchema        pSchT;
PtrSSchema        pSS;
PtrElement          pElNum;

#endif /* __STDC__ */

{
   int                 val;
   PtrSSchema        PSchStr;
   PtrElement          pEl;
   TCounter         *pTr1;
   PtrElement          pEl1;
   boolean             initattr, stop;
   PtrAttribute         pAttr;
   int                 valinitattr;
   int                 level;

#define MaxAncetre 50
   PtrElement          PcWithin[MaxAncetre];

   valinitattr = 0;
   pTr1 = &pSchT->TsCounter[NCompt - 1];
   if (pTr1->TnOperation == TCntrNoOp)
      val = pTr1->TnParam1;
   else
     {
	/* a priori, la valeur initiale du compteur ne depend pas d'un */
	/* attribut */
	initattr = FALSE;
	if (pTr1->TnAttrInit > 0)
	   /* la valeur initiale du compteur est definie par un attribut */
	  {
	     /* remonte a  la racine de l'arbre auquel appartient l'element */
	     /* a numeroter */
	     pEl = pElNum;
	     while (pEl->ElParent != NULL)
		pEl = pEl->ElParent;
	     /* cet attribut est-il present sur la racine de l'arbre auquel */
	     /* appartient l'element a numeroter ? */
	     pAttr = pEl->ElFirstAttr;
	     stop = FALSE;	/* parcourt les attributs de la racine */
	     do
		if (pAttr == NULL)
		   stop = TRUE;	/* dernier attribut */
		else if (pAttr->AeAttrNum == pTr1->TnAttrInit &&
			 pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
		   stop = TRUE;	/* c'est l'attribut cherche' */
		else
		   pAttr = pAttr->AeNext;	/* au suivant */
	     while (!(stop));
	     if (pAttr != NULL)
		/* la racine porte bien l'attribut qui initialise le compteur */
	       {
		  initattr = TRUE;
		  valinitattr = pAttr->AeAttrValue;	/* on prend la valeur de l'attribut */
	       }
	  }
	if (pTr1->TnOperation == TCntrRLevel)
	  {
	     pEl = pElNum;
	     val = 0;
	     /* parcourt les elements englobants de l'element sur lequel porte */
	     /* le calcul du compteur */
	     while (pEl != NULL)
	       {
		  if (pEl->ElTypeNumber == pTr1->TnElemType1 &&
		      pEl->ElSructSchema == pElNum->ElSructSchema)
		     /* l'element rencontre' a la meme type que l'element traite' */
		     val++;	/* incremente le compteur */
		  pEl = pEl->ElParent;
	       }
	  }
	else
	  {
	     if (pTr1->TnElemType1 == PageBreak + 1)
		/* c'est un compteur de pages */
	       {
		  PSchStr = NULL;
		  pEl = pElNum;
	       }
	     else
	       {
		  /* schema de structure du compteur */
		  PSchStr = pElNum->ElSructSchema;
		  if (pTr1->TnOperation != TCntrRank || pTr1->TnAcestorLevel == 0)
		     /* Cherche le premier element de type TnElemType1 */
		     /* englobant l'element a numeroter */
		     pEl = GetTypedAncestor (pElNum, pTr1->TnElemType1, PSchStr);
		  else
		    {
		       /* Cherche le nieme element de type TnElemType1 qui englobe */
		       /* l'element a numeroter */
		       if (pTr1->TnAcestorLevel < 0)
			  /* on compte les ascendants en remontant de l'element */
			  /* concerne' vers la racine */
			  level = -pTr1->TnAcestorLevel;
		       else
			  /* on compte les ascendants en descendant de la racine vers */
			  /* l'element concerne'. Pour cela on commence par enregistrer */
			  /* le chemin de l'element concerne' vers la racine */
			  level = MaxAncetre;
		       pEl = pElNum;
		       while (level > 0 && pEl != NULL)
			 {
			    if (pEl->ElTypeNumber == pTr1->TnElemType1 &&
				pEl->ElSructSchema->SsCode == pElNum->ElSructSchema->SsCode)
			       /* cet element englobant a le type qui incremente le compteur */
			      {
				 level--;
				 PcWithin[level] = pEl;
			      }
			    if (level > 0)
			       pEl = pEl->ElParent;
			 }
		       if (pTr1->TnAcestorLevel > 0)
			  /* il faut redescendre a partir de la racine */
			  pEl = PcWithin[level + pTr1->TnAcestorLevel - 1];
		    }
	       }
	     if (pEl == NULL)
		val = 0;	/* pas trouve' */
	     else if (pTr1->TnOperation == TCntrRank)
		if (pTr1->TnElemType1 == PageBreak + 1)
		   /* c'est un compteur de pages */
		  {
		     if (pEl->ElTypeNumber == PageBreak + 1 && pEl->ElViewPSchema == 1)
			val = 1;
		     else if (initattr)
			val = valinitattr;
		     else
			val = 0;
		     /* compte les marques de page qui precedent l'element */
		     do
		       {
			  pEl = BackSearchTypedElem (pEl, PageBreak + 1, NULL);
			  if (pEl != NULL)
			     /* cas page rappel supprime */
			     /* on ne compte que les marques de page de la vue 1 */
			     if (pEl->ElViewPSchema == 1)
				val++;
		       }
		     while (!(pEl == NULL));
		  }
		else
		   /* numero = rang de l'element dans la liste */
		   /* Cherche le rang de l'element trouve' parmi ses freres */
		  {
		     if (initattr)
			val = valinitattr;
		     else
			val = 1;
		     while (pEl->ElPrevious != NULL)
		       {
			  pEl = pEl->ElPrevious;
			  if (EquivalentType (pEl, pTr1->TnElemType1, PSchStr))
			     /* on ne compte pas les eventuelles marques de page */
			     val++;
		       }
		  }
	     else
		/* l'element trouve est celui qui reinitialise le compteur */
	       {
		  if (initattr)
		     val = valinitattr - pTr1->TnParam2;
		  else
		     val = pTr1->TnParam1 - pTr1->TnParam2;
		  /* a partir de l'element trouve', cherche en avant tous les */
		  /* elements ayant le type qui incremente le compteur, */
		  /* jusqu'a rencontrer celui pour lequel on calcule la valeur du */
		  /* compteur. */
		  do
		    {
		       pEl = FwdSearchElem2Types (pEl, pTr1->TnElemType2, pElNum->ElTypeNumber, PSchStr,
					 pElNum->ElSructSchema);
		       if (pEl != NULL)
			 {
			    pEl1 = pEl;
			    if (EquivalentType (pEl, pTr1->TnElemType2, PSchStr))
			       /* on ignore les pages qui ne */
			       /* concernent pas la vue 1 */
			       if (pEl1->ElTypeNumber != PageBreak + 1 ||
				   (pEl1->ElTypeNumber == PageBreak + 1 &&
				    pEl1->ElViewPSchema == 1))
				  val += pTr1->TnParam2;
			 }
		    }
		  while (!(pEl == NULL || pEl == pElNum));
	       }
	  }
     }
   return val;
}


/* ---------------------------------------------------------------------- */
/* | ChDesc   cherche dans le sous-arbre de racine pEl (racine exclue)  | */
/* |    exclue), un element de type TypeCh defini dans le schema de     | */
/* |    structure pointe' par pSS, ou si pSS est nul dans le schema de  | */
/* |    nom NomSS.                                                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ChDesc (PtrElement * pEl, int TypeCh, PtrSSchema pSS, Name NomSS)

#else  /* __STDC__ */
static void         ChDesc (pEl, TypeCh, pSS, NomSS)
PtrElement         *pEl;
int                 TypeCh;
PtrSSchema        pSS;
Name                 NomSS;

#endif /* __STDC__ */

{
   PtrElement          pElFils;
   boolean             SchOk;
   PtrElement          pEl1;
   PtrSSchema        pSS1;


   if ((*pEl)->ElTerminal)
      *pEl = NULL;		/* element terminal, echec */
   else if ((*pEl)->ElFirstChild == NULL)
      *pEl = NULL;		/* pas de fils, echec */
   else
      /* il y a au moins un descendant direct */
     {
	pElFils = (*pEl)->ElFirstChild;
	/* cherche dans le sous-arbre de chacun des fils */
	do
	  {
	     /* le fils a-t-il le type cherche' ? */
	     pEl1 = pElFils;
	     if (pSS == NULL)
	       {
		  SchOk = strcmp (NomSS, pEl1->ElSructSchema->SsName) == 0;
		  pSS1 = pEl1->ElSructSchema;
	       }
	     else
	       {
		  SchOk = pEl1->ElSructSchema->SsCode == pSS->SsCode;
		  pSS1 = pSS;
	       }
	     if (SchOk && EquivalentSRules (TypeCh, pSS1, pEl1->ElTypeNumber, pSS1, *pEl))
		*pEl = pElFils;	/* Trouve' ! */
	     else
	       {
		  *pEl = pElFils;
		  ChDesc (pEl, TypeCh, pSS, NomSS);
		  if (*pEl == NULL)
		     pElFils = pElFils->ElNext;
	       }
	  }
	while (!(*pEl != NULL || pElFils == NULL));
     }
}


/* ---------------------------------------------------------------------- */
/* | SortContenu   sort le contenu des feuilles de l'element pEl dans   | */
/* |    dans le fichier.                                                | */
/* |    La traduction du contenu des feuilles a lieu seulement si       | */
/* |    tradCar est vrai.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         SortContenu (PtrElement pEl, boolean tradCar, boolean lineBreak,
				 int fichier, PtrDocument pDoc)

#else  /* __STDC__ */
static void         SortContenu (pEl, tradCar, lineBreak, fichier, pDoc)
PtrElement          pEl;
boolean             tradCar;
boolean             lineBreak;
int                 fichier;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrElement          pFils;

   if (!pEl->ElTerminal)
      /* l'element n'est pas une feuille, on sort le contenu de chacun de */
      /* ses fils */
     {
	pFils = pEl->ElFirstChild;
	while (pFils != NULL)
	  {
	     SortContenu (pFils, tradCar, lineBreak, fichier, pDoc);
	     pFils = pFils->ElNext;
	  }
     }
   else
      /* l'element est une feuille, on sort son contenu */
      TraduitFeuille (pEl, tradCar, lineBreak, fichier, pDoc);
}

#ifdef __STDC__
static void         AppliqueTra (PtrTRule pRegle, PtrTSchema pSchT,
			PtrSSchema pSchS, PtrElement pEl, boolean tradCar,
				 boolean lineBreak, boolean * ElRemove,
				 PtrPRule pRPres, PtrAttribute pAttribut,
				 PtrDocument pDoc);

#else  /* __STDC__ */
static void         AppliqueTra (	/* pRegle, pSchT, pSchS, pEl, tradCar, lineBreak,
					   ElRemove, pRPres, pAttribut, pDoc */ );

#endif /* __STDC__ */


/* ---------------------------------------------------------------------- */
/* | ApplAttrPosElem    applique a l'element pointe' par pEl les regles | */
/* |    de traduction associees a l'attribut pointe' par pAttr.         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ApplAttrPosElem (TOrder Posit, PtrElement pEl,
				     PtrAttribute pAttr, boolean * ElRemove,
				     boolean * tradCar, boolean * lineBreak,
				     PtrDocument pDoc)

#else  /* __STDC__ */
static void         ApplAttrPosElem (Posit, pEl, pAttr, ElRemove, tradCar, lineBreak,
				     pDoc)
TOrder          Posit;
PtrElement          pEl;
PtrAttribute         pAttr;
boolean            *ElRemove;
boolean            *tradCar;
boolean            *lineBreak;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrTRuleBlock       pBloc;
   PtrTRule        pRegle;

   PtrTSchema        pSchTAttr;
   int                 i;
   AttributeTransl      *pRT1;
   TranslNumAttrCase        *pTC1;
   PtrTRuleBlock       pBl1;
   PtrTRule        pRe1;
   NotifyAttribute     notifyAttr;

   /* prepare et envoie le message AttrExport.Pre s'il est demande' */
   notifyAttr.event = TteAttrExport;
   notifyAttr.document = (Document) IdentDocument (pDoc);
   notifyAttr.element = (Element) pEl;
   notifyAttr.attribute = (Attribute) pAttr;
   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
   notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
   if (SendAttributeMessage (&notifyAttr, TRUE))
      /* l'application ne laisse pas l'editeur ecrire l'attribut */
      return;
   /* cherche le premier bloc de regles correspondant a l'attribut */
   pSchTAttr = SchemaTrad (pAttr->AeAttrSSchema);
   pBloc = NULL;
   if (pSchTAttr != NULL)
     {
	pRT1 = &pSchTAttr->TsAttrTRule[pAttr->AeAttrNum - 1];
	switch (pAttr->AeAttrType)
	      {
		 case AtNumAttr:
		    i = 1;
		    while (pBloc == NULL && i <= pRT1->AtrNCases)
		      {
			 pTC1 = &pRT1->AtrCase[i - 1];
			 if (pAttr->AeAttrValue <= pTC1->TaUpperBound &&
			     pAttr->AeAttrValue >= pTC1->TaLowerBound)
			    pBloc = pTC1->TaTRuleBlock;
			 i++;
		      }
		    break;
		 case AtTextAttr:
		    if (pRT1->AtrTextValue[0] == '\0')
		       pBloc = pRT1->AtrTxtTRuleBlock;
		    else if (StringAndTextEqual (pRT1->AtrTextValue, pAttr->AeAttrText))
		       pBloc = pRT1->AtrTxtTRuleBlock;
		    break;
		 case AtReferenceAttr:
		    pBloc = pRT1->AtrRefTRuleBlock;
		    break;
		 case AtEnumAttr:
		    pBloc = pRT1->AtrEnuTRuleBlock[pAttr->AeAttrValue];
		    if (pBloc == NULL)
		       /* pas de regles de traduction pour cette valeur, on */
		       /* prend les regles qui s'appliquent a toute valeur */
		       pBloc = pRT1->AtrEnuTRuleBlock[0];
		    break;
		 default:
		    break;
	      }
     }
   /* parcourt les blocs de regles de la valeur de l'attribut */
   while (pBloc != NULL)
     {
	pBl1 = pBloc;
	if (CondSatisfaite (pBloc, pEl, pAttr, pDoc))
	   /* la condition du bloc est verifiee */
	  {
	     pRegle = pBl1->TbFirstTRule;	/* premiere regle du bloc */
	     /* parcourt les regles du bloc */
	     while (pRegle != NULL)
	       {
		  pRe1 = pRegle;
		  if (pRe1->TrOrder == Posit)
		     /* c'est une regle a appliquer a cette position */
		     if (pRe1->TrType == TRemove)
			*ElRemove = TRUE;
		     else if (pRe1->TrType == TNoTranslation)
			*tradCar = FALSE;
		     else if (pRe1->TrType == TNoLineBreak)
			*lineBreak = FALSE;
		     else
			/* on applique la regle */
			AppliqueTra (pRegle, pSchTAttr, pAttr->AeAttrSSchema, pEl, *tradCar, *lineBreak, ElRemove, NULL, pAttr, pDoc);
		  /* passe a la regle suivante */
		  pRegle = pRe1->TrNextTRule;
	       }
	  }
	/* passe au bloc suivant */
	pBloc = pBl1->TbNextBlock;
     }
   /* prepare et envoie le message AttrExport.Post s'il est demande' */
   notifyAttr.event = TteAttrExport;
   notifyAttr.document = (Document) IdentDocument (pDoc);
   notifyAttr.element = (Element) pEl;
   notifyAttr.attribute = (Attribute) pAttr;
   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
   notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
   SendAttributeMessage (&notifyAttr, FALSE);
}


/* ---------------------------------------------------------------------- */
/* | ApplAttrPos                                                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ApplAttrPos (TOrder Posit, PtrElement pEl, boolean * ElRemove,
		   boolean * tradCar, boolean * lineBreak, PtrDocument pDoc)

#else  /* __STDC__ */
static void         ApplAttrPos (Posit, pEl, ElRemove, tradCar, lineBreak, pDoc)
TOrder          Posit;
PtrElement          pEl;
boolean            *ElRemove;
boolean            *tradCar;
boolean            *lineBreak;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;
   PtrTSchema        pSchT;
   PtrElement          pAsc;
   int                 att;
   int                 nbAttr;

#define LgTableAttr 50
   PtrAttribute         TableAttr[LgTableAttr];

   pAttr = pEl->ElFirstAttr;	/* 1er attribut de l'element */
   /* Si on applique les regles "After", on commence par le dernier attribut */
   /* et on traitera les attributs dans l'ordre inverse */
   if (Posit == TAfter && pAttr != NULL)
     {
	nbAttr = 0;
	while (pAttr->AeNext != NULL && nbAttr < LgTableAttr)
	  {
	     TableAttr[nbAttr] = pAttr;
	     nbAttr++;
	     pAttr = pAttr->AeNext;
	  }
     }

   /* parcourt les attributs de l'element */
   while (pAttr != NULL)
     {
	pSchT = SchemaTrad (pAttr->AeAttrSSchema);
	if (pSchT != NULL)
	   if (pSchT->TsAttrTRule[pAttr->AeAttrNum - 1].AtrElemType == 0)
	      /* les regles de traduction de l'attribut s'appliquent a */
	      /* n'importe quel type d'element, on les applique */
	      ApplAttrPosElem (Posit, pEl, pAttr, ElRemove, tradCar, lineBreak,
			       pDoc);
	if (Posit == TAfter)
	   /* passe a l'attribut precedent de l'element */
	  {
	     if (nbAttr > 0)
	       {
		  nbAttr--;
		  pAttr = TableAttr[nbAttr];
	       }
	     else
		pAttr = NULL;
	  }
	else
	   /* passe a l'attribut suivant de l'element */
	   pAttr = pAttr->AeNext;
     }
   /* produit la traduction des attributs des elements ascendants */
   /* qui s'appliquent aux elements du type de notre element */
   pSchT = SchemaTrad (pEl->ElSructSchema);
   if (pSchT != NULL)
      if (pSchT->TsInheritAttr[pEl->ElTypeNumber - 1])
	 /* il y a effectivement heritage d'attribut pour ce type d'element */
	{
	   /* cherche tous les attributs dont ce type d'element peut */
	   /* heriter. */
	   /* balaye la table des attributs */
	   for (att = 1; att <= pEl->ElSructSchema->SsNAttributes; att++)
	      if (pSchT->TsAttrTRule[att - 1].AtrElemType == pEl->ElTypeNumber)
		 /* cet attribut s'applique a ce type d'element */
		{
		   /* Y a-t-il un element ascendant qui porte cet attribut? */
		   pAsc = pEl;	/* on commence par l'element lui-meme */
		   while (pAsc != NULL)		/* parcourt les ascendants */
		     {
			/* parcourt les attributs de chaque ascendant */
			pAttr = pAsc->ElFirstAttr;
			while (pAttr != NULL)
			   if (pAttr->AeAttrSSchema == pEl->ElSructSchema &&
			       pAttr->AeAttrNum == att)
			      /* on a trouve' */
			     {
				/* applique les regles de traduction de */
				/* l'attribut a l'element */
				ApplAttrPosElem (Posit, pEl, pAttr, ElRemove, tradCar,
						 lineBreak, pDoc);
				/* inutile de poursuivre la recherche */
				pAttr = NULL;
				pAsc = NULL;
			     }
			   else
			      /* passe a l'attribut suivant du meme ascendant */
			      pAttr = pAttr->AeNext;
			/* passe a l'ascendant du dessus si on n'a pas trouve' */
			if (pAsc != NULL)
			   pAsc = pAsc->ElParent;
		     }
		}
	}
}

/* ---------------------------------------------------------------------- */
/* | ApplPresPos applique a l'element pointe' par pEl les regles de     | */
/* |    traduction associees aux presentations portees par l'element.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ApplPresPos (TOrder Posit, PtrElement pEl, boolean * ElRemove,
				 boolean * tradCar, boolean * lineBreak,
				 PtrAttribute pAttribut, PtrDocument pDoc)

#else  /* __STDC__ */
static void         ApplPresPos (Posit, pEl, ElRemove, tradCar, lineBreak, pAttribut,
				 pDoc)
TOrder          Posit;
PtrElement          pEl;
boolean            *ElRemove;
boolean            *tradCar;
boolean            *lineBreak;
PtrAttribute         pAttribut;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule        pPres;
   PtrTSchema        pSchT;
   PRuleTransl       *pRT1;
   PtrTRuleBlock       pBloc;
   PtrTRule        pRegle;
   TranslNumAttrCase        *pTC1;
   int                 i;
   char                val;
   int                 nbRPres;

#define LgTableRPres 50
   PtrPRule        TableRPres[LgTableRPres];

   pSchT = SchemaTrad (pEl->ElSructSchema);
   if (pSchT == NULL)
      return;
   /* 1ere regle de presentation specifique de l'element */
   pPres = pEl->ElFirstPRule;
   /* Si on applique les regles "After", on commence par la derniere regle */
   /* et on traitera les regles dans l'ordre inverse */
   if (Posit == TAfter && pPres != NULL)
     {
	nbRPres = 0;
	while (pPres->PrNextPRule != NULL && nbRPres < LgTableRPres)
	  {
	     TableRPres[nbRPres] = pPres;
	     nbRPres++;
	     pPres = pPres->PrNextPRule;
	  }
     }

   /* parcourt les regles de presentation specifique de l'element */
   while (pPres != NULL)
     {
	pRT1 = &pSchT->TsPresTRule[pPres->PrType];
	if (pRT1->RtExist)
	   /* il y a des regles de traduction pour cette presentation */
	  {
	     /* cherche le premier bloc de regles correspondant a ce */
	     /* type de regle de presentation */
	     pBloc = NULL;
	     if (pSchT != NULL)
		if (pPres->PrType == PtSize || pPres->PrType == PtIndent ||
		    pPres->PrType == PtLineSpacing ||
		    pPres->PrType == PtLineWeight)
		  {
		     i = 1;
		     while (pBloc == NULL && i <= pRT1->RtNCase)
		       {
			  pTC1 = &pRT1->RtCase[i - 1];
			  if (pPres->PrMinValue <= pTC1->TaUpperBound &&
			      pPres->PrMinValue >= pTC1->TaLowerBound)
			     pBloc = pTC1->TaTRuleBlock;
			  i++;
		       }
		  }
		else if (pPres->PrType == PtFillPattern ||
			 pPres->PrType == PtBackground ||
			 pPres->PrType == PtForeground)
		  {
		     i = 1;
		     while (pBloc == NULL && i <= pRT1->RtNCase)
		       {
			  pTC1 = &pRT1->RtCase[i - 1];
			  if (pPres->PrIntValue <= pTC1->TaUpperBound &&
			      pPres->PrIntValue >= pTC1->TaLowerBound)
			     pBloc = pTC1->TaTRuleBlock;
			  i++;
		       }
		  }
		else
		  {
		     /* cherche si cette valeur de la presentation a un */
		     /* bloc de regles */
		     /* Calcule d'abord la valeur caractere de la presentation */
		     val = ValCaractPres (pPres);
		     pBloc = NULL;
		     i = 1;
		     while (pBloc == NULL && pRT1->RtPRuleValue[i] != '\0' &&
			    i <= MAX_TRANSL_PRES_VAL + 1)
		       {
			  if (pRT1->RtPRuleValue[i] == val)
			     pBloc = pRT1->RtPRuleValueBlock[i];
			  i++;
		       }
		     if (pBloc == NULL)
			/* pas de regles de traduction pour cette valeur, on */
			/* prend les regles qui s'appliquent a toute valeur */
			pBloc = pRT1->RtPRuleValueBlock[0];
		  }
	     /* parcourt les blocs de regles de la valeur de la presentation */
	     while (pBloc != NULL)
	       {
		  if (CondSatisfaite (pBloc, pEl, NULL, pDoc))
		     /* la condition du bloc est verifiee */
		    {
		       pRegle = pBloc->TbFirstTRule;	/* premiere regle du bloc */
		       /* parcourt les regles du bloc */
		       while (pRegle != NULL)
			 {
			    if (pRegle->TrOrder == Posit)
			       /* c'est une regle a appliquer a cette position */
			       if (pRegle->TrType == TRemove)
				  *ElRemove = TRUE;
			       else if (pRegle->TrType == TNoTranslation)
				  *tradCar = FALSE;
			       else if (pRegle->TrType == TNoLineBreak)
				  *lineBreak = FALSE;
			       else
				  /* on applique la regle */
				  AppliqueTra (pRegle, pSchT, pEl->ElSructSchema, pEl, *tradCar, *lineBreak, ElRemove, pPres, pAttribut, pDoc);
			    /* passe a la regle suivante */
			    pRegle = pRegle->TrNextTRule;
			 }
		    }
		  /* passe au bloc suivant */
		  pBloc = pBloc->TbNextBlock;
	       }
	  }

	if (Posit == TAfter)
	   /* passe a la regle de presentation precedente de l'element */
	  {
	     if (nbRPres > 0)
	       {
		  nbRPres--;
		  pPres = TableRPres[nbRPres];
	       }
	     else
		pPres = NULL;
	  }
	else
	   /* passe a la regle de presentation suivante de l'element */
	   pPres = pPres->PrNextPRule;
     }
}

/* ---------------------------------------------------------------------- */
/* | produitVariable                                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         produitVariable (PtrElement pEl, PtrAttribute pAttribut,
			 PtrTSchema pSchT, PtrSSchema pSchS, int NumVar,
		boolean Ref, char *outbuffer, int fichier, PtrDocument pDoc,
				     boolean lineBreak)

#else  /* __STDC__ */

static void         produitVariable (pEl, pAttribut, pSchT, pSchS, NumVar, Ref, outbuffer, fichier, pDoc, lineBreak)
PtrElement          pEl;
PtrAttribute         pAttribut;
PtrTSchema        pSchT;
PtrSSchema        pSchS;
int                 NumVar;
boolean             Ref;
char               *outbuffer;
int                 fichier;
PtrDocument         pDoc;
boolean             lineBreak;

#endif /* __STDC__ */
{
   TranslVariable            *pVa1;
   int                 el, i, j, k;
   TranslVarItem        *pVarT1;
   PtrElement          pElRef;
   PtrElement          pAsc;
   PtrReference        pRef;
   TtAttribute           *pAttr1;
   PtrAttribute         pAttr;
   DocumentIdentifier     IDocExt;
   PtrDocument         pDocExt;
   PtrTextBuffer      pBuf;
   boolean             trouve;
   char                Nombre[20];

   pAttr = NULL;
   if (outbuffer != NULL)
      /* on vide le buffer avant de commencer a le remplir */
      outbuffer[0] = '\0';
   pVa1 = &pSchT->TsVariable[NumVar - 1];
   /* parcourt les elements qui constituent la variable */
   for (el = 1; el <= pVa1->TrvNItems; el++)
     {
	pVarT1 = &pVa1->TrvItem[el - 1];
	/* traite selon le type de l'element de variable */
	switch (pVarT1->TvType)
	      {
		 case VtText:
		    /* une constante de texte */
		    i = pSchT->TsConstBegin[pVarT1->TvItem - 1];
		    while (pSchT->TsConstant[i - 1] != '\0')
		      {
			 Sort (pSchT->TsConstant[i - 1], fichier, outbuffer,
			       pDoc, lineBreak);
			 i++;
		      }
		    break;
		 case VtCounter:
		    /* valeur d'un compteur */
		    /* si la regle porte sur une reference, on */
		    /* prend la valeur du compteur pour l'element */
		    /* designe'par la reference si TrReferredObj est vrai */
		    pRef = NULL;
		    if (Ref)
		       if (pAttribut != NULL &&
			   pAttribut->AeAttrSSchema->SsAttribute[pAttribut->AeAttrNum - 1].AttrType == AtReferenceAttr)
			  /* c'est un attribut reference */
			  pRef = pAttribut->AeAttrReference;
		       else
			  /* l'element est-il une reference ? */
		       if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
			  pRef = pEl->ElReference;
		       else
			  /* c'est peut-etre une inclusion */
			  pRef = pEl->ElSource;
		    if (pRef != NULL)
		      {
			 pElRef = ReferredElement (pRef, &IDocExt, &pDocExt);
			 if (pElRef == NULL)
			    /* la reference ne designe rien */
			    i = 0;
			 else
			    /* valeur du compteur pour l'element */
			    /* designe' par la reference */
			    i = ComptValTra (pVarT1->TvItem, pSchT, pSchS, pElRef);
		      }
		    else
		       /* ce n'est pas une reference */
		       /* valeur du compteur pour l'element meme */
		       i = ComptValTra (pVarT1->TvItem, pSchT, pSchS, pEl);
		    /* produit quelques 0 si c'est demande' */
		    if (pVarT1->TvCounterStyle == CntArabic && pVarT1->TvLength > 0)
		      {
			 j = 1;
			 for (k = 1; k <= pVarT1->TvLength - 1; k++)
			   {
			      j = j * 10;
			      if (j > i)
				 Sort ('0', fichier, outbuffer, pDoc, lineBreak);
			   }
		      }
		    /* convertit la valeur du compteur dans le style demande' */
		    ConvertitNombre (i, pVarT1->TvCounterStyle, Nombre, &j);
		    /* sort la valeur du compteur */
		    for (k = 0; k < j; k++)
		       Sort (Nombre[k], fichier, outbuffer, pDoc, lineBreak);
		    break;
		 case VtBuffer:
		    /* le contenu d'un buffer */
		    i = 1;
		    while (pSchT->TsBuffer[pVarT1->TvItem - 1][i - 1] != '\0')
		      {
			 Sort (pSchT->TsBuffer[pVarT1->TvItem - 1][i - 1],
			       fichier, outbuffer, pDoc, lineBreak);
			 i++;
		      }
		    break;
		 case VtAttrVal:	/* la valeur d'un attribut */
		    /* cherche si l'element traduit ou l'un de ses */
		    /* ascendants possede cet attribut */
		    trouve = FALSE;
		    pAsc = pEl;
		    while (!trouve && pAsc != NULL)
		      {
			 pAttr = pAsc->ElFirstAttr;	/* premier attribut */
			 while (!trouve && pAttr != NULL)
			    if (pAttr->AeAttrNum == pVarT1->TvItem
				&& pAttr->AeAttrSSchema == pSchS)
			       trouve = TRUE;
			    else
			       pAttr = pAttr->AeNext;
			 if (!trouve)
			    pAsc = pAsc->ElParent;
		      }
		    if (trouve)
		       /* l'element possede l'attribut */
		      {
			 switch (pAttr->AeAttrType)
			       {
				  case AtNumAttr:
				     SortNb (pAttr->AeAttrValue, fichier, outbuffer, pDoc,
					     lineBreak);
				     break;
				  case AtTextAttr:
				     pBuf = pAttr->AeAttrText;
				     while (pBuf != NULL)
				       {
					  i = 1;
					  while (i <= pBuf->BuLength)
					    {
					       Sort (pBuf->BuContent[i - 1], fichier, outbuffer,
						     pDoc, lineBreak);
					       i++;
					    }
					  pBuf = pBuf->BuNext;
				       }
				     break;
				  case AtReferenceAttr:
				     Sort ('R', fichier, outbuffer, pDoc, lineBreak);
				     Sort ('E', fichier, outbuffer, pDoc, lineBreak);
				     Sort ('F', fichier, outbuffer, pDoc, lineBreak);
				     break;
				  case AtEnumAttr:
				     i = 0;
				     pAttr1 = &pAttr->AeAttrSSchema->SsAttribute[pVarT1->TvItem - 1];
				     while (pAttr1->AttrEnumValue[pAttr->AeAttrValue - 1][i] != '\0')
				       {
					  i++;
					  Sort (pAttr1->AttrEnumValue[pAttr->AeAttrValue - 1]
					  [i - 1], fichier, outbuffer, pDoc,
						lineBreak);
				       }
				     break;
			       }
			 /* end case AttrType */
		      }
		    break;

		 case VtFileDir:	/* le nom du directory de sortie */
		    i = 0;
		    while (tra_filedir[i] != '\0')
		       Sort (tra_filedir[i++], fichier, outbuffer, pDoc, lineBreak);
		    break;

		 case VtFileName:	/* le nom du fichier de sortie */
		    i = 0;
		    while (tra_filename[i] != '\0')
		       Sort (tra_filename[i++], fichier, outbuffer, pDoc, lineBreak);
		    break;

		 case VtExtension:	/* le nom de l'extension de fichier */
		    i = 0;
		    while (tra_extension[i] != '\0')
		       Sort (tra_extension[i++], fichier, outbuffer, pDoc, lineBreak);
		    break;
		 case VtDocumentName:	/* le nom du document */
		    i = 0;
		    while (pDoc->DocDName[i] != '\0')
		       Sort (pDoc->DocDName[i++], fichier, outbuffer, pDoc, lineBreak);
		    break;
		 case VtDocumentDir:	/* le repertoire du document */
		    i = 0;
		    while (pDoc->DocDirectory[i] != '\0')
		       Sort (pDoc->DocDirectory[i++], fichier, outbuffer, pDoc, lineBreak);
		    break;

		 default:
		    break;
	      }
     }
}

#ifdef __STDC__
static void         Traduit (PtrElement pEl, PtrDocument pDoc, boolean tradCar,
			     boolean lineBreak, boolean MemeSiDejaTraduit);

#else  /* __STDC__ */
static void         Traduit ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* | AppliqueTra   applique la regle de traduction pRegle du schema de  | */
/* |    traduction pSchT (qui correspond au schema de structure pointe' | */
/* |    par pSchS) a l'element pointe par pEl, en demandant la          | */
/* |    traduction des caracteres contenus si tradCar est vrai.         | */
/* |    S'il s'agit de la traduction d'une presentation, pRPres pointe  | */
/* |    sur la regle de presentation specifique traduite.               | */
/* |    S'il s'agit de la traduction des regles d'un attribut,          | */
/* |    pAttribut pointe sur l'attribut que l'on traduit.               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         AppliqueTra (PtrTRule pRegle, PtrTSchema pSchT,
			PtrSSchema pSchS, PtrElement pEl, boolean tradCar,
				 boolean lineBreak, boolean * ElRemove,
				 PtrPRule pRPres, PtrAttribute pAttribut,
				 PtrDocument pDoc)
#else  /* __STDC__ */
static void         AppliqueTra (pRegle, pSchT, pSchS, pEl, tradCar, lineBreak,
				 ElRemove, pRPres, pAttribut, pDoc)
PtrTRule        pRegle;
PtrTSchema        pSchT;
PtrSSchema        pSchS;
PtrElement          pEl;
boolean             tradCar;
boolean             lineBreak;
boolean            *ElRemove;
PtrPRule        pRPres;
PtrAttribute         pAttribut;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 i, j;
   boolean             trouve, refpossible;
   char                c;
   PtrElement          pElGet, pElRef;
   PtrDocument         pDocGet;
   Name                 fname;
   PtrSSchema        pSS;
   Name                 n;
   DocumentIdentifier     IDocExt;
   PtrDocument         pDocExt;
   PtrAttribute         pAttr;
   PtrTextBuffer      pBuf;
   PtrTRule        pRe1;
   PtrTSchema        pSc1;
   PtrAttribute         pAt1;
   TtAttribute           *pAttr1;
   PtrElement          pEl1;
   BinFile             tra_inclus;
   PtrReference        pRef;
   int                 fichier;
   char                secondaryFileName[MAX_PATH];
   char               *nomASortir;
   char                full_name[MAX_PATH];	/* nom d'un fichier a inclure */
   PathBuffer          NomDirectory;
   FILE               *newfile;
   char                currentFileName[MAX_PATH];	/* nom du fichier principal */

   n[0] = '\0';
   pRe1 = pRegle;
   pSc1 = pSchT;
   /* on applique la regle selon son type */
   switch (pRe1->TrType)
	 {
	    case TCreate:
	    case TWrite:
	       /* regle d'ecriture dans un fichier de sortie ou au terminal */
	       if (pRe1->TrType == TCreate)
		  if (pRe1->TrFileNameVar == 0)
		     /* sortie sur le fichier principal courant */
		     fichier = 1;
		  else
		     /* sortie sur un fichier secondaire */
		    {
		       /* construit le nom du fichier secondaire */
		       produitVariable (pEl, pAttribut, pSchT, pSchS, pRe1->TrFileNameVar,
			      FALSE, secondaryFileName, 0, pDoc, lineBreak);
		       fichier = fichierSecondaire (secondaryFileName, pDoc);
		    }
	       else		/* TWrite */
		  fichier = 0;	/* on ecrit sur stdout */
	       /* traitement selon le type d'objet a ecrire */
	       switch (pRe1->TrObject)
		     {
			case ToConst:
			   /* ecriture d'une constante */
			   i = pSc1->TsConstBegin[pRe1->TrObjectNum - 1];
			   while (pSc1->TsConstant[i - 1] != '\0')
			     {
				Sort (pSc1->TsConstant[i - 1], fichier, NULL, pDoc, lineBreak);
				i++;
			     }
			   break;
			case ToBuffer:
			   /* ecriture du contenu d'un buffer */
			   i = 1;
			   while (pSc1->TsBuffer[pRe1->TrObjectNum - 1][i - 1] != '\0')
			     {
				Sort (pSc1->TsBuffer[pRe1->TrObjectNum - 1][i - 1], fichier, NULL,
				      pDoc, lineBreak);
				i++;
			     }
			   break;
			case ToVariable:	/* creation d'une variable */
			   produitVariable (pEl, pAttribut, pSchT, pSchS, pRe1->TrObjectNum, pRe1->TrReferredObj, NULL, fichier, pDoc, lineBreak);
			   break;

			case ToAttr:
			   /* cherche si l'element ou un de ses ascendants possede */
			   /* l'attribut a sortir */
			   trouve = FALSE;
			   pSS = pEl->ElSructSchema;
			   while (pEl != NULL && !trouve)
			     {
				pAttr = pEl->ElFirstAttr;	/* 1er attribut de l'element */
				/* parcourt les attributs de l'element */
				while (pAttr != NULL && !trouve)
				  {
				     pAt1 = pAttr;
				     if (pAt1->AeAttrSSchema == pSS &&
					 pAt1->AeAttrNum == pRe1->TrObjectNum)
					trouve = TRUE;
				     else
					pAttr = pAt1->AeNext;
				  }
				if (!trouve)
				   pEl = pEl->ElParent;	/* passe a l'element ascendant */
			     }
			   /* si on a trouve' l'attribut, on sort sa valeur */
			   if (trouve)
			     {
				pAt1 = pAttr;
				switch (pAt1->AeAttrType)
				      {
					 case AtNumAttr:
					    /* ecrit la valeur numerique de l'attribut */
					    SortNb (pAt1->AeAttrValue, fichier, NULL, pDoc, lineBreak);
					    break;
					 case AtTextAttr:
					    /* ecrit la valeur de l'attribut */
					    pBuf = pAt1->AeAttrText;
					    while (pBuf != NULL)
					      {
						 i = 1;
						 while (i <= pBuf->BuLength)
						   {
						      Sort (pBuf->BuContent[i - 1], fichier, NULL, pDoc, lineBreak);
						      i++;
						   }
						 pBuf = pBuf->BuNext;
					      }
					    break;
					 case AtReferenceAttr:
					    /* cas non traite' */
					    break;
					 case AtEnumAttr:
					    /* ecrit le nom de la valeur de l'attribut */
					    pAttr1 = &pAt1->AeAttrSSchema->SsAttribute[pAt1->AeAttrNum - 1];
					    i = 1;
					    while (pAttr1->AttrEnumValue[pAt1->AeAttrValue - 1][i - 1] != '\0')
					      {
						 Sort (pAttr1->AttrEnumValue[pAt1->AeAttrValue - 1][i - 1],
						       fichier, NULL, pDoc, lineBreak);
						 i++;
					      }
					    break;
					 default:
					    break;
				      }

			     }
			   break;
			case ToContent:
			   /* produit le contenu des feuilles de l'element */
			   SortContenu (pEl, tradCar, lineBreak, fichier, pDoc);
			   break;
			case ToPRuleValue:
			   /* produit la valeur numerique de la presentation a laquelle */
			   /* se rapporte la regle */
			   if (pRPres != NULL)
			      if (pRPres->PrPresMode == PresImmediate)
				 switch (pRPres->PrType)
				       {
					  case PtFont:
					  case PtStyle:
					  case PtUnderline:
					  case PtThickness:
					  case PtLineStyle:
					     Sort (pRPres->PrChrValue, fichier, NULL, pDoc, lineBreak);
					     break;
					  case PtIndent:
					  case PtSize:
					  case PtLineSpacing:
					  case PtLineWeight:
					     SortNb (pRPres->PrMinValue, fichier, NULL, pDoc, lineBreak);
					     break;
					  case PtFillPattern:
					     SortMotif (pRPres->PrIntValue, fichier, pDoc, lineBreak);
					     break;
					  case PtBackground:
					  case PtForeground:
					     SortCouleur (pRPres->PrIntValue, fichier, pDoc, lineBreak);
					     break;
					  case PtJustify:
					  case PtHyphenate:
					     if (pRPres->PrJustify)
						Sort ('Y', fichier, NULL, pDoc, lineBreak);
					     else
						Sort ('N', fichier, NULL, pDoc, lineBreak);
					     break;
					  case PtAdjust:
					     switch (pRPres->PrAdjust)
						   {
						      case AlignLeft:
							 Sort ('L', fichier, NULL, pDoc, lineBreak);
							 break;
						      case AlignRight:
							 Sort ('R', fichier, NULL, pDoc, lineBreak);
							 break;
						      case AlignCenter:
							 Sort ('C', fichier, NULL, pDoc, lineBreak);
							 break;
						      case AlignLeftDots:
							 Sort ('D', fichier, NULL, pDoc, lineBreak);
							 break;
						   }
					     break;
					  default:
					     break;
				       }
			   break;
			case ToComment:
			   /* produit le contenu du commentaire de l'element */
			   pBuf = pEl->ElComment;
			   while (pBuf != NULL)
			     {
				i = 1;
				while (i <= pBuf->BuLength)
				  {
				     Sort (pBuf->BuContent[i - 1], fichier, NULL, pDoc, lineBreak);
				     i++;
				  }
				pBuf = pBuf->BuNext;
			     }
			   break;
			case ToAllAttr:
			   /* produit la traduction de tous les attributs de l'element */
			   ApplAttrPos (pRe1->TrOrder, pEl, ElRemove, &tradCar, &lineBreak,
					pDoc);
			   pEl->ElTransAttr = TRUE;	/* les regles des attributs ont ete
							 * appliquees */
			   break;
			case ToAllPRules:
			   /* produit la traduction de toutes les regles de presentation */
			   /* specifique portees par l'element */
			   ApplPresPos (pRe1->TrOrder, pEl, ElRemove, &tradCar, &lineBreak,
					pAttribut, pDoc);
			   /* marque dans l'element que sa presentation a ete traduite */
			   pEl->ElTransPres = TRUE;
			   break;

			case ToPairId:
			   /* traduit l'identificateur d'une paire */
			   if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsPairedElement)
			      /* l'element est bien une paire */
			      SortNb (pEl->ElPairIdent, fichier, NULL, pDoc, lineBreak);
			   break;

			case ToFileDir:
			   i = 0;
			   while (tra_filedir[i] != '\0')
			      Sort (tra_filedir[i++], fichier, NULL, pDoc, lineBreak);
			   break;

			case ToFileName:
			   i = 0;
			   while (tra_filename[i] != '\0')
			      Sort (tra_filename[i++], fichier, NULL, pDoc, lineBreak);
			   break;

			case ToExtension:
			   i = 0;
			   while (tra_extension[i] != '\0')
			      Sort (tra_extension[i++], fichier, NULL, pDoc, lineBreak);
			   break;

			case ToDocumentName:
			   i = 0;
			   while (pDoc->DocDName[i] != '\0')
			      Sort (pDoc->DocDName[i++], fichier, NULL, pDoc, lineBreak);
			   break;

			case ToDocumentDir:
			   i = 0;
			   while (pDoc->DocDirectory[i] != '\0')
			      Sort (pDoc->DocDirectory[i++], fichier, NULL, pDoc, lineBreak);
			   break;

			case ToReferredDocumentName:
			case ToReferredDocumentDir:
			   pRef = NULL;
			   if (pAttribut != NULL &&
			       pAttribut->AeAttrSSchema->SsAttribute[pAttribut->AeAttrNum - 1].AttrType == AtReferenceAttr)
			      /* c'est un attribut reference qu'on traduit */
			      pRef = pAttribut->AeAttrReference;
			   else
			      /* l'element est-il une reference ? */
			   if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
			      pRef = pEl->ElReference;
			   else
			      /* c'est peut-etre une inclusion */
			      pRef = pEl->ElSource;
			   if (pRef != NULL)
			     {
				pElRef = ReferredElement (pRef, &IDocExt, &pDocExt);
				nomASortir = NULL;
				if (pRe1->TrObject == ToReferredDocumentName)
				  {
				     if (pElRef != NULL && IDocExt[0] == '\0')
					/* reference interne. On sort le nom du document lui-meme */
					nomASortir = pDoc->DocDName;
				     else if (IDocExt[0] != '\0')
					/* on sort le nom du document reference' */
					nomASortir = IDocExt;
				  }
				else if (pRe1->TrObject == ToReferredDocumentDir)
				   if (pElRef != NULL && IDocExt[0] == '\0')
				      /* reference interne. On sort le directory du document lui-meme */
				      nomASortir = pDoc->DocDirectory;
				   else if (IDocExt[0] != '\0')
				     {
					/* on sort le directory du document reference' */
					if (pDocExt != NULL)
					   /* le document reference' est charge' */
					   nomASortir = pDocExt->DocDirectory;
					else
					   /* le document reference' n'est pas charge' */
					  {
					     strncpy (NomDirectory, DirectoryDoc, MAX_PATH);
					     BuildFileName (IDocExt, "PIV", NomDirectory, full_name, &i);
					     if (full_name[0] != '\0')
						/* on a trouve' le fichier */
						nomASortir = NomDirectory;
					  }
				     }
				if (nomASortir != NULL)
				   while (*nomASortir != '\0')
				     {
					Sort (*nomASortir, fichier, NULL, pDoc, lineBreak);
					nomASortir++;
				     }
			     }
			   break;

			case ToReferredElem:
			   /* traduit l'elment reference' de type pRe1->TrObjectNum */
			   pRef = NULL;
			   if (pAttribut != NULL &&
			       pAttribut->AeAttrSSchema->SsAttribute[pAttribut->AeAttrNum - 1].AttrType == AtReferenceAttr)
			      /* c'est un attribut reference qu'on traduit */
			      pRef = pAttribut->AeAttrReference;
			   else
			      /* l'element est-il une reference ? */
			   if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
			      pRef = pEl->ElReference;
			   else
			      /* c'est peut-etre une inclusion */
			      pRef = pEl->ElSource;
			   if (pRef != NULL)
			     {
				pElRef = ReferredElement (pRef, &IDocExt, &pDocExt);
				if (pElRef != NULL)
				   /* la reference designe l'element pElRef */
				   /* On le prend s'il a le type voulu */
				  {
				     if (pRe1->TrObjectNature[0] == '\0')
					pSS = pEl->ElSructSchema;
				     else
					pSS = NULL;
				     if (!((pSS != NULL &&
					    EquivalentSRules (pRe1->TrObjectNum, pSS, pElRef->ElTypeNumber, pElRef->ElSructSchema, pElRef->ElParent)
					   )
					   || (pSS == NULL &&
					       strcmp (pRe1->TrObjectNature, pElRef->ElSructSchema->SsName) == 0
					       && EquivalentSRules (pRe1->TrObjectNum, pElRef->ElSructSchema,
							      pElRef->ElTypeNumber, pElRef->ElSructSchema, pElRef->ElParent)
					   )
					 )
					)
					/* Il n'a pas le type voulu, on cherche dans */
					/* le sous arbre de l'element designe' */
					ChDesc (&pElRef, pRe1->TrObjectNum, pSS, pRe1->TrObjectNature);
				  }
				if (pElRef != NULL)
				   /* traduit l'element reference', meme s'il a deja ete traduit */
				   if (IDocExt[0] == '\0')
				      /* reference interne */
				      Traduit (pElRef, pDoc, tradCar, lineBreak, TRUE);
				   else if (pDocExt != NULL)
				      /* reference externe a un document charge' */
				      Traduit (pElRef, pDocExt, tradCar, lineBreak, TRUE);
			     }
			   break;

			case ToRefId:
			case ToReferredRefId:
			   pElGet = NULL;
			   if (pRe1->TrObject == ToReferredRefId)
			      /* il faut traduire le label de l'element reference' */
			     {
				pRef = NULL;
				/* si on traduit un attribut reference, on ne s'occupe que de */
				/* l'attribut */
				if (pAttribut != NULL
				    && pAttribut->AeAttrSSchema->SsAttribute[pAttribut->AeAttrNum - 1].AttrType == AtReferenceAttr)
				   /* c'est un attribut reference */
				   pRef = pAttribut->AeAttrReference;
				/* sinon on s'occupe de l'element */
				else
				  {
				     /* l'element est-il une reference ? */
				     if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsReference)
					pRef = pEl->ElReference;
				     /* ou est-il defini comme identique a une reference */
				     else if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsIdentity)
				       {
					  i = pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrIdentRule;
					  if (pEl->ElSructSchema->SsRule[i - 1].SrConstruct == CsBasicElement
					      && pEl->ElSructSchema->SsRule[i - 1].SrBasicType == CsReference)
					     pRef = pEl->ElReference;
				       }
				  }
				if (pRef == NULL)
				   /* c'est peut-etre une inclusion */
				   pRef = pEl->ElSource;
				if (pRef != NULL)
				  {
				     pElGet = ReferredElement (pRef, &IDocExt, &pDocExt);
				     if (pElGet == NULL && IDocExt[0] != '\0')
					/* reference a un document externe non charge' */
					if (pRef != NULL)
					   if (pRef->RdReferred != NULL)
					      if (pRef->RdReferred->ReExternalRef)
						{

						   i = 0;
						   while (pRef->RdReferred->ReReferredLabel[i] != '\0')
						      Sort (pRef->RdReferred->ReReferredLabel[i++], fichier, NULL,
							    pDoc, lineBreak);
						}
				  }
			     }

			   if (pRe1->TrObject == ToRefId)
			     {
				/* on cherche si l'element (ou le premier de ses ascendants sur */
				/* lequel porte une reference) est reference' et on recupere la */
				/* reference. */
				pElGet = pEl;
				do
				  {
				     pSS = pElGet->ElSructSchema;
				     refpossible = FALSE;
				     /* l'element est-il reference'? */
				     if (pElGet->ElReferredDescr != NULL)
					refpossible = pElGet->ElReferredDescr->ReFirstReference != NULL;
				     if (!refpossible)
				       {
					  /* l'element peut-il etre designe' par un element reference? */
					  /* on cherche tous les elements references dans le schema de */
					  /* structure de l'element */
					  i = 1;
					  do
					    {
					       i++;
					       if (pSS->SsRule[i - 1].SrConstruct == CsReference)
						  /* c'est une reference */
						  if (pSS->SsRule[i - 1].SrReferredType != 0)
						     refpossible = EquivalentSRules (pSS->SsRule[i - 1].SrReferredType, pSS, pElGet->ElTypeNumber, pSS, pElGet->ElParent);
					    }
					  while (!(refpossible || i >= pSS->SsNRules));
				       }
				     if (!refpossible)
				       {
					  /* l'element ne peut pas etre designe par un elem. reference */
					  /* on cherche s'il peut etre designe' par un attr. reference */
					  /* on cherche tous les attributs reference dans le schema de */
					  /* structure de l'element */
					  i = 1;
					  do
					    {
					       i++;
					       if (pSS->SsAttribute[i - 1].AttrType == AtReferenceAttr)
						  /* c'est une reference */
						  if (pSS->SsAttribute[i - 1].AttrTypeRef != 0)
						     refpossible = (pSS->SsAttribute[i - 1].AttrTypeRefNature[0] == '\0'	/* meme schema de structure */
								    && EquivalentSRules (pSS->SsAttribute[i - 1].AttrTypeRef, pSS, pElGet->ElTypeNumber, pSS, pElGet->ElParent));
					    }
					  while (!(refpossible || i >= pSS->SsNAttributes));
				       }
				     if (!refpossible)
					/* l'element ne peut pas etre designe'; on examine */
					/* l'element ascendant */
					pElGet = pElGet->ElParent;
				  }
				while (!(refpossible || pElGet == NULL));
			     }

			   if (pElGet != NULL)
/** if (pElGet->ElReferredDescr != NULL) **/
			     {
				i = 0;
				while (pElGet->ElLabel[i] != '\0')
				   Sort (pElGet->ElLabel[i++], fichier, NULL, pDoc, lineBreak);
			     }
			   break;

			default:
			   break;
		     }
	       break;

	    case TChangeMainFile:
	       produitVariable (pEl, pAttribut, pSchT, pSchS, pRe1->TrNewFileVar, FALSE, currentFileName, 0, pDoc, lineBreak);
	       if (currentFileName[0] != '\0')
		 {
		    newfile = fopen (currentFileName, "w");
		    if (newfile == NULL)
		       TtaDisplayMessage (CONFIRM, TtaGetMessage(LIB, LIB_CANNOT_CREATE_FILE), currentFileName);
		    else
		       /* on a reussi a ouvrir le nouveau fichier */
		      {
			 /* on vide le buffer en cours dans l'ancien fichier */
			 for (i = 0; i < FichierSortie[1].SecLgOutBuffer; i++)
			    putc (FichierSortie[1].SecOutBuffer[i], FichierSortie[1].SecFileDesc);
			 /* on ferme l'ancien fichier */
			 fclose (FichierSortie[1].SecFileDesc);
			 /* on bascule sur le nouveau fichier */
			 FichierSortie[1].SecLgOutBuffer = 0;
			 FichierSortie[1].SecFileDesc = newfile;
		      }
		 }
	       break;

	    case TSetCounter:
	       pSchT->TsCounter[pRe1->TrCounterNum - 1].TnParam1 = pRe1->TrCounterParam;
	       break;

	    case TAddCounter:
	       pSchT->TsCounter[pRe1->TrCounterNum - 1].TnParam1 += pRe1->TrCounterParam;
	       break;

	    case TGet:
	    case TCopy:
	       /* on traduit l'element indique' dans la regle Get */
	       /* cherche d'abord l'element a prendre */
	       pElGet = pEl;
	       pDocGet = pDoc;
	       switch (pRe1->TrRelPosition)
		     {
			case RpSibling:
			   /* Cherche un frere ayant le type voulu */
			   /* cherche d'abord le frere aine' */
			   while (pElGet->ElPrevious != NULL)
			      pElGet = pElGet->ElPrevious;
			   /* cherche ensuite parmi les freres successifs */
			   trouve = FALSE;
			   do
			     {
				pEl1 = pElGet;
				if ((pEl1->ElSructSchema == pEl->ElSructSchema ||
				     (strcmp (pRe1->TrElemNature, pEl1->ElSructSchema->SsName) == 0))
				    && EquivalentSRules (pRe1->TrElemType, pEl1->ElSructSchema,
						   pEl1->ElTypeNumber, pEl1->ElSructSchema, pElGet->ElParent))
				   trouve = TRUE;
				else
				   pElGet = pEl1->ElNext;
			     }
			   while (!(trouve || pElGet == NULL));
			   break;
			case RpDescend:
			   /* Cherche dans le sous-arbre un element ayant le type voulu. */
			   if (pRe1->TrElemNature[0] == '\0')
			      pSS = pEl->ElSructSchema;
			   else
			      pSS = NULL;
			   ChDesc (&pElGet, pRe1->TrElemType, pSS, pRe1->TrElemNature);
			   break;
			case RpReferred:
			   /* Cherche dans le sous-arbre de l'element designe', un element
			      ayant le type voulu. */
			   /* cherche d'abord l'element designe' */
			   pRef = NULL;
			   if (pAttribut != NULL &&
			       pAttribut->AeAttrSSchema->SsAttribute[pAttribut->AeAttrNum - 1].AttrType == AtReferenceAttr)
			      /* c'est un attribut reference qu'on traduit */
			      pRef = pAttribut->AeAttrReference;
			   else
			      /* l'element est-il une reference ? */
			   if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
			      pRef = pEl->ElReference;
			   else
			      /* c'est peut-etre une inclusion */
			      pRef = pEl->ElSource;
			   if (pRef == NULL)
			      pElGet = NULL;
			   else
			      pElGet = ReferredElement (pEl->ElReference, &IDocExt, &pDocExt);
			   if (pElGet != NULL)
			      /* il y a bien un element designe'. On le prend s'il */
			      /* a le type voulu */
			     {
				pEl1 = pElGet;
				if (pRe1->TrElemNature[0] == '\0')
				   pSS = pEl->ElSructSchema;
				else
				   pSS = NULL;
				if (!((pSS != NULL &&
				       EquivalentSRules (pRe1->TrElemType, pSS, pEl1->ElTypeNumber, pEl1->ElSructSchema, pEl1->ElParent)
				      )
				      || (pSS == NULL &&
					  strcmp (pRe1->TrElemNature, pEl1->ElSructSchema->SsName) == 0
					  && EquivalentSRules (pRe1->TrElemType, pEl1->ElSructSchema,
							 pEl1->ElTypeNumber, pEl1->ElSructSchema, pEl1->ElParent)
				      )
				    )
				   )
				   /* Il n'a pas le type voulu, on cherche dans */
				   /* le sous arbre de l'element designe' */
				   ChDesc (&pElGet, pRe1->TrElemType, pSS, pRe1->TrElemNature);
				if (IDocExt[0] != '\0' && pDocExt != NULL)
				   /* reference externe a un document charge' */
				   pDocGet = pDocExt;
			     }
			   break;
			case RpAssoc:
			   /* on prend les elements associes du type indique' */
			   i = 0;
			   pElGet = NULL;
			   do
			     {
				i++;
				if (pDoc->DocAssocRoot[i - 1] != NULL)
				   if (pDoc->DocAssocRoot[i - 1]->ElTypeNumber == pRe1->TrElemType)
				      pElGet = pDoc->DocAssocRoot[i - 1];
				   else if (pDoc->DocAssocRoot[i - 1]->ElFirstChild != NULL)
				      if (pDoc->DocAssocRoot[i - 1]->ElFirstChild->ElTypeNumber == pRe1->TrElemType)
					 pElGet = pDoc->DocAssocRoot[i - 1];
			     }
			   while (!(pElGet != NULL || i >= MAX_ASSOC_DOC));
			   break;
			default:
			   break;
		     }
	       if (pElGet != NULL)
		  /* traduit l'element a prendre, sauf s'il a deja ete traduit et */
		  /* qu'il s'agit d'une regle Get */
		  Traduit (pElGet, pDocGet, tradCar, lineBreak,
			   pRe1->TrType == TCopy);
	       break;
	    case TUse:
	       /* On ne fait rien. Cette regle est utilisee uniquement */
	       /* lors du chargement des schemas de traduction, au debut */
	       /* du chargement du document a traduire. */
	       break;
	    case TRemove:
	    case TNoTranslation:
	    case TNoLineBreak:
	       /* On ne fait rien */
	       break;
	    case TRead:
	       /* lecture au terminal */
/***** supprime' ****
      met dans fname le texte saisi au terminal
      for (j = 1; j <= i + 1; j++)
	pSc1->TsBuffer[pRe1->TrBuffer - 1][j - 1] = fname[j - 1];
************************************/
	       break;
	    case TInclude:
	       /* inclusion d'un fichier */
	       /* compose le nom du fichier a ouvrir avec le nom du directory */
	       /* des schemas... */
	       if (pRe1->TrBufOrConst == ToConst)
		 {
		    j = 1;
		    i = pSc1->TsConstBegin[pRe1->TrInclFile - 1];
		    while (pSc1->TsConstant[i - 1] != '\0' && j < MAX_TXT_LEN)
		      {
			 fname[j - 1] = pSc1->TsConstant[i - 1];
			 j++;
			 i++;
		      }
		    fname[j - 1] = '\0';
		 }
	       if (pRe1->TrBufOrConst == ToBuffer)
		 {
		    /* le nom du fichier est dans un buffer */
		    i = 1;
		    while (pSc1->TsBuffer[pRe1->TrInclFile - 1][i - 1] != '\0' &&
			   i < MAX_TXT_LEN)
		      {
			 fname[i - 1] = pSc1->TsBuffer[pRe1->TrInclFile - 1][i - 1];
			 i++;
		      }
		    fname[i - 1] = '\0';
		 }
	       if (fname[0] == '\0')
		  /* pas de nom de fichier */
		  full_name[0] = '\0';
	       else
		 {
		    /* construit le nom de fichier */
		    strncpy (NomDirectory, DirectorySchemas, MAX_PATH);
		    BuildFileName (fname, "", NomDirectory, full_name, &i);
		 }
	       /* ouvre le fichier a inclure */
	       tra_inclus = BIOreadOpen (full_name);
	       if (tra_inclus == 0)
		  TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_CANNOT_INCLUDE_FILE), fname);
	       else
		  /* le fichier a inclure est ouvert */
		 {
		    while (BIOreadByte (tra_inclus, &c))
		       /* on ecrit dans le fichier principal courant */
		       Sort (c, 1, NULL, pDoc, lineBreak);
		    BIOreadClose (tra_inclus);
		 }
	       break;
	    default:
	       break;
	 }
   /* end case TrType */
}

/* ---------------------------------------------------------------------- */
/* | ApplReglePos       applique a l'element pointe' par pEl les regles | */
/* |    de traduction qui correspondent a son type et qui doivent       | */
/* |    s'appliquer a la position Posit.                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ApplReglePos (TOrder Posit, boolean * tradCar,
				  boolean * lineBreak, boolean * ElRemove,
			     PtrElement pEl, int TypeEl, PtrTSchema pSchT,
				  PtrSSchema pSchS, PtrDocument pDoc)

#else  /* __STDC__ */
static void         ApplReglePos (Posit, tradCar, lineBreak, ElRemove, pEl, TypeEl,
				  pSchT, pSchS, pDoc)
TOrder          Posit;
boolean            *tradCar;
boolean            *lineBreak;
boolean            *ElRemove;
PtrElement          pEl;
int                 TypeEl;
PtrTSchema        pSchT;
PtrSSchema        pSchS;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrTRuleBlock       pBloc;
   PtrTRule        pRegle;
   PtrTRuleBlock       pBl1;
   PtrTRule        pRe1;


   /* premier bloc de regles correspondant au type de l'element */
   pBloc = pSchT->TsElemTRule[TypeEl - 1];
   /* parcourt les blocs de regles du type de l'element */
   while (pBloc != NULL)
     {
	pBl1 = pBloc;
	if (CondSatisfaite (pBloc, pEl, NULL, pDoc))
	   /* la condition du bloc est verifiee */
	  {
	     pRegle = pBl1->TbFirstTRule;	/* premiere regle du bloc */
	     /* parcourt les regles du bloc */
	     while (pRegle != NULL)
	       {
		  pRe1 = pRegle;
		  if (pRe1->TrOrder == Posit)
		     /* c'est une regle a appliquer a cette position */
		     if (pRe1->TrType == TRemove)
			*ElRemove = TRUE;
		     else if (pRe1->TrType == TNoTranslation)
			*tradCar = FALSE;
		     else if (pRe1->TrType == TNoLineBreak)
			*lineBreak = FALSE;
		     else	/* on applique la regle */
			AppliqueTra (pRegle, pSchT, pSchS, pEl, *tradCar, *lineBreak,
				     ElRemove, NULL, NULL, pDoc);
		  /* passe a la regle suivante */
		  pRegle = pRe1->TrNextTRule;
	       }
	  }
	/* passe au bloc suivant */
	pBloc = pBl1->TbNextBlock;
     }
}


/* ---------------------------------------------------------------------- */
/* | Traduit   traduit le sous-arbre dont la racine est pointee par pEl | */
/* |    et applique les regles de traduction des feuilles si tradCar    | */
/* |    est vrai.                                                       | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void         Traduit (PtrElement pEl, PtrDocument pDoc, boolean tradCar,
			     boolean lineBreak, boolean MemeSiDejaTraduit)

#else  /* __STDC__ */
static void         Traduit (pEl, pDoc, tradCar, lineBreak, MemeSiDejaTraduit)
PtrElement          pEl;
PtrDocument         pDoc;
boolean             tradCar;
boolean             lineBreak;
boolean             MemeSiDejaTraduit;

#endif /* __STDC__ */

{
   PtrElement          pFils;
   PtrTSchema        pSchT, pSchT1;
   PtrSSchema        pSchS;
   int                 TypeEl;
   int                 i;
   boolean             trouve;
   boolean             ElRemove;
   PtrSSchema        pSc1;
   SRule              *pRe1;
   NotifyElement       notifyEl;

   if (!pEl->ElTransContent || MemeSiDejaTraduit)
     {
	/* cherche le schema de traduction qui s'applique a l'element */
	pSchT = SchemaTrad (pEl->ElSructSchema);
	if (pSchT == NULL)
	   return;
	ElRemove = FALSE;
	pSchS = pEl->ElSructSchema;
	TypeEl = pEl->ElTypeNumber;
	/* envoie le message ElemExport.Pre a l'application, si elle */
	/* le demande */
	notifyEl.event = TteElemExport;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) pEl;
	notifyEl.elementType.ElTypeNum = TypeEl;
	notifyEl.elementType.ElSSchema = (SSchema) pSchS;
	notifyEl.position = 0;
	if (ThotSendMessage ((NotifyEvent *) & notifyEl, TRUE))
	   /* l'application refuse que Thot sauve l'element */
	   return;
	pEl->ElTransAttr = FALSE;	/* les attributs n'ont pas ete traduits */
	pEl->ElTransPres = FALSE;	/* la presentation n'a pas ete traduite */
	/* s'il s'agit de l'element racine d'une nature, on prend les regles */
	/* de presentation (s'il y en a) de la regle nature dans la structure */
	/* englobante. */
	/* on ne traite pas les marques de page */
	if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
	   if (pEl->ElParent != NULL)
	      /* il y a un englobant */
	      if (pEl->ElParent->ElSructSchema != pEl->ElSructSchema)
		 /* cherche la regle introduisant la nature dans le schema de */
		 /* structure de l'englobant. */
		{
		   pSc1 = pEl->ElParent->ElSructSchema;
		   trouve = FALSE;
		   i = 0;
		   do
		     {
			i++;
			pRe1 = &pSc1->SsRule[i - 1];
			if (pRe1->SrConstruct == CsNatureSchema)
			   if (pRe1->SrSSchemaNat == pEl->ElSructSchema)
			      trouve = TRUE;
		     }
		   while (!(trouve || i >= pSc1->SsNRules));
		   if (trouve)
		     {
			pSchT1 = SchemaTrad (pEl->ElParent->ElSructSchema);
			if (pSchT1 != NULL)
			   if (pSchT1->TsElemTRule[i - 1] != NULL)
			      /* il y a des regles de traduction pour la nature, on */
			      /* les prend */
			     {
				pSchT = pSchT1;
				pSchS = pEl->ElParent->ElSructSchema;
				TypeEl = i;
			     }
		     }
		}
	if (pSchT != NULL)
	   /* on ne traduit pas les elements dont le schema de structure n'a */
	   /* pas de schema de traduction correspondant */
	  {
	     /* Cherche et applique les regles de traduction associees au type */
	     /* de l'element et qui doivent s'appliquer avant la traduction du */
	     /* contenu de l'element */
	     ApplReglePos (TBefore, &tradCar, &lineBreak, &ElRemove, pEl, TypeEl,
			   pSchT, pSchS, pDoc);
	     /* on ne traduit les attributs que si ce n'est pas deja fait par */
	     /* une regle Create Attributes associee au type */
	     if (!pEl->ElTransAttr)
		/* Parcourt les attributs de l'element et applique les regles des
		 * attributs qui doivent ^etre appliquees avant la traduction du
		 * contenu de l'element */
		ApplAttrPos (TBefore, pEl, &ElRemove, &tradCar, &lineBreak, pDoc);

	     /* on ne traduit la presentation que si ce n'est pas deja fait par */
	     /* une regle Create Presentation */
	     if (!pEl->ElTransPres)
		/* Parcourt les presentations de l'element et applique les regles
		 * de traduction correspondantes qui doivent ^etre appliquees
		 * avant la traduction du contenu de l'element */
		ApplPresPos (TBefore, pEl, &ElRemove, &tradCar, &lineBreak, NULL,
			     pDoc);
	     /* traduit le contenu de l'element, sauf si on a deja rencontre' */
	     /* une regle Remove pour cet element. */
	     if (!ElRemove)
		/* pas de regle Remove */
		if (pEl->ElTerminal)
		   /* c'est une feuille, applique les regles de traduction des */
		   /* feuilles et sort le contenu dans le fichier principal */
		   TraduitFeuille (pEl, tradCar, lineBreak, 1, pDoc);
		else
		   /* ce n'est pas une feuille, traduit successivement tous les */
		   /* fils de l'element */
		  {
		     pFils = pEl->ElFirstChild;
		     while (pFils != NULL)
		       {
			  Traduit (pFils, pDoc, tradCar, lineBreak, MemeSiDejaTraduit);
			  pFils = pFils->ElNext;
		       }
		  }
	     /* marque que les regles qui doivent etre appliquees apres */
	     /* la traduction du contenu et qui sont associees aux attributs */
	     /* et a la presentation n'ont pas encore ete appliquees */
	     pEl->ElTransAttr = FALSE;	/* les attributs n'ont pas ete traduits */
	     pEl->ElTransPres = FALSE;	/* la presentation n'a pas ete traduite */
	     /* on ne traduit la presentation que si ce n'est pas deja fait par */
	     /* une regle Create Presentation */
	     if (!pEl->ElTransPres)
		/* Parcourt les presentations de l'element et applique les regles
		 * de traduction correspondantes qui doivent ^etre appliquees
		 * apres la traduction du contenu */
		ApplPresPos (TAfter, pEl, &ElRemove, &tradCar, &lineBreak, NULL,
			     pDoc);
	     if (!pEl->ElTransAttr)
		/* Parcourt les attributs de l'element et applique les regles des
		 * attributs qui doivent etre appliquees apres la traduction du
		 * contenu */
		ApplAttrPos (TAfter, pEl, &ElRemove, &tradCar, &lineBreak, pDoc);
	     /* Cherche et applique les regles associees au type de l'element et
	      * qui doivent s'appliquer apres la traduction du contenu */
	     ApplReglePos (TAfter, &tradCar, &lineBreak, &ElRemove, pEl, TypeEl,
			   pSchT, pSchS, pDoc);
	     if (!MemeSiDejaTraduit)
		/* marque que l'element a ete traite' */
		pEl->ElTransContent = TRUE;
	     /* envoie le message ElemExport.Post a l'application, si elle */
	     /* le demande */
	     notifyEl.event = TteElemExport;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElSructSchema);
	     notifyEl.position = 0;
	     ThotSendMessage ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* | RazDejaTraduit     remet a zero tous les indicateurs "deja traduit"| */
/* |    de l'arbre de racine pEl.                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         RazDejaTraduit (PtrElement pEl)
#else  /* __STDC__ */
static void         RazDejaTraduit (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pFils;

   if (pEl != NULL)
     {
	pEl->ElTransContent = FALSE;
	pEl->ElTransAttr = FALSE;
	pEl->ElTransPres = FALSE;
	if (!pEl->ElTerminal)
	  {
	     pFils = pEl->ElFirstChild;
	     while (pFils != NULL)
	       {
		  RazDejaTraduit (pFils);
		  pFils = pFils->ElNext;
	       }
	  }
     }
}

/* ---------------------------------------------------------------------- */
/* |    InitOutputFiles initialise les fichiers de sortie.              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         InitOutputFiles (FILE * mainFile, PtrDocument pDoc)

#else  /* __STDC__ */
static void         InitOutputFiles (mainFile, pDoc)
FILE               *mainFile;
PtrDocument         pDoc;

#endif /* _STDC__ */
{
   /* Entree 0 : standard output */
   FichierSortie[0].SecFileName[0] = '\0';
   FichierSortie[0].SecFileDesc = NULL;
   FichierSortie[0].SecLgOutBuffer = 0;
   /* Entree 1 : fichier de sortie principal */
   FichierSortie[1].SecFileName[0] = '\0';
   FichierSortie[1].SecFileDesc = mainFile;
   FichierSortie[1].SecLgOutBuffer = 0;
   NbFichierSortie = 2;
}

/* ---------------------------------------------------------------------- */
/* | FlushOutputFiles vide les buffers dans les fichiers de sortie      | */
/* |    correspondants.                                                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         FlushOutputFiles (PtrDocument pDoc)

#else  /* __STDC__ */
static void         FlushOutputFiles (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 i, fich;

   for (fich = 1; fich < NbFichierSortie; fich++)
     {
	for (i = 0; i < FichierSortie[fich].SecLgOutBuffer; i++)
	   putc (FichierSortie[fich].SecOutBuffer[i], FichierSortie[fich].SecFileDesc);
	if (FichierSortie[fich].SecFileDesc != NULL)
	   fclose (FichierSortie[fich].SecFileDesc);
     }
}

/* ---------------------------------------------------------------------- */
/* | ExportDocument     exporte le document pointe' par pDoc, selon le  | */
/* |    schema de traduction de nom nomSchTrad et produit le resultat   | */
/* |    dans le fichier de nom fileName.                                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ExportDocument (PtrDocument pDoc, char *fileName, char *nomSchTrad)

#else  /* __STDC__ */
void                ExportDocument (pDoc, fileName, nomSchTrad)
PtrDocument         pDoc;
char               *fileName;
char               *nomSchTrad;

#endif /* __STDC__ */

{
   int                 i;
   FILE               *tra_sortie;	/* fichier de sortie principal */

   /* cree le fichier de sortie principal */
   tra_sortie = fopen (fileName, "w");
   if (tra_sortie == NULL)
      TtaDisplayMessage (CONFIRM, TtaGetMessage(LIB, LIB_CANNOT_CREATE_FILE),
				     tra_filename);
   else
      /* le fichier de sortie principal a ete cree' */
     {
	/* separe nom de directory et nom de fichier */
	strncpy (tra_filedir, fileName, MAX_PATH);
	tra_filedir[MAX_PATH - 1] = '\0';
	i = strlen (tra_filedir);
	while (i > 0 && tra_filedir[i] != DIR_SEP)
	   i--;
	if (tra_filedir[i] == DIR_SEP)
	  {
	     strcpy (tra_filename, &tra_filedir[i + 1]);
	     tra_filedir[i + 1] = '\0';
	  }
	else
	  {
	     strcpy (tra_filename, &tra_filedir[i]);
	     tra_filedir[i] = '\0';
	  }
	/* charge le schema de traduction du document */
	if (!LdSchTrad (nomSchTrad, pDoc->DocSSchema) != 0)
	   /* echec au chargement du schema de traduction */
	   fclose (tra_sortie);
	else
	  {
	     /* separe nom de fichier et extension */
	     tra_extension[0] = '\0';
	     i = strlen (tra_filename);
	     i--;
	     while (i > 0 && tra_filename[i] != '.')
		i--;
	     if (tra_filename[i] == '.')
	       {
		  strncpy (tra_extension, &tra_filename[i], MAX_PATH);
		  tra_filename[i] = '\0';
	       }
	     /* charge tous les documents reference's par le document a traduire */
	     /* LoadReferedDocuments(pDoc);  faut-il vraiment le faire???? */
	     InitOutputFiles (tra_sortie, pDoc);
	     /* remet a zero les indicateurs "deja traduit" de tous les elements */
	     /* du document */
	     RazDejaTraduit (pDoc->DocRootElement);
	     for (i = 0; i < MAX_ASSOC_DOC; i++)
		if (pDoc->DocAssocRoot[i] != NULL)
		   RazDejaTraduit (pDoc->DocAssocRoot[i]);
	     /* traduit l'arbre principal du document */
	     Traduit (pDoc->DocRootElement, pDoc, TRUE, TRUE, FALSE);
	     /* traduit les arbres associe's */
	     for (i = 0; i < MAX_ASSOC_DOC; i++)
		if (pDoc->DocAssocRoot[i] != NULL)
		   Traduit (pDoc->DocAssocRoot[i], pDoc, TRUE, TRUE, FALSE);
	     /* vide ce qui traine dans les buffers de sortie */
	     /* et ferme ces fichiers */
	     FlushOutputFiles (pDoc);
	  }
     }
   ClearTableSchTrad ();
   fflush (stdout);
   fflush (stderr);
}

/* End Of Module export */
