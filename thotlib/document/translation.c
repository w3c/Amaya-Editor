/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*

   This module handles document translation.
   It travels the abstract trees of a document and produces an
   external representation according to a set of translation schemas.

 */

  /* Authors:
   *         R. Guetari (W3C/INRIA): Unicode related code
   */

#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "consttra.h"
#include "typemedia.h"
#include "typetra.h"
#include "language.h"
#include "fileaccess.h"
#include "libmsg.h"
#include "appaction.h"
#include "appstruct.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "platform_tv.h"
#include "thotcolor_tv.h"
#include "select_tv.h"
#include "edit_tv.h"

/* maximum length of an output buffer */
#define MAX_BUFFER_LEN 1000

/* maximum number of output buffers */
#define MAX_OUTPUT_FILES 10

/* information about an output file */
typedef struct _AnOutputFile
  {
     CHAR_T              OfFileName[MAX_PATH];	/* file name */
     FILE               *OfFileDesc;	/* file descriptor */
     int                 OfBufferLen;	/* current length of output buffer */
     int		 OfIndent;	/* current value of indentation */
     int		 OfPreviousIndent;/* previous value of indentation */
     int                 OfLineNumber;  /* number of lines already written */
     ThotBool		 OfStartOfLine;	/* start a new line */
     CHAR_T              OfBuffer[MAX_BUFFER_LEN];	/* output buffer */
     ThotBool		 OfCannotOpen;	/* open failure */
  }
AnOutputFile;

/* number of output files in use */
static int          NOutputFiles = 0;

/* the output files */
static AnOutputFile OutputFile[MAX_OUTPUT_FILES];

	/* entry 0: stdout    */
	/* entry 1: main output file */
	/* other entries: secondary output files */

/* directory of output files */
static CHAR_T         fileDirectory[MAX_PATH];

/* name of main output file */
static CHAR_T         fileName[MAX_PATH];

/* file extension */
static CHAR_T         fileExtension[MAX_PATH];

extern CHARSET        CharEncoding;

#include "tree_f.h"

#include "callback_f.h"
#include "translation_f.h"
#include "absboxes_f.h"
#include "memory_f.h"

#include "readprs_f.h"
#include "references_f.h"
#include "externalref_f.h"
#include "schemas_f.h"
#include "schtrad_f.h"
#include "fileaccess_f.h"
#include "structschema_f.h"
#include "content_f.h"
#include "applicationapi_f.h"

/*----------------------------------------------------------------------
   GetSecondaryFile  retourne le fichier secondaire de nom fName.
   If open is True, the file is opened if it not open yet.
   Otherwise, the function returns 0 when the file is not open.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          GetSecondaryFile (STRING fName, PtrDocument pDoc, ThotBool open)

#else  /* __STDC__ */
static int          GetSecondaryFile (fName, pDoc, open)
STRING              fName;
PtrDocument         pDoc;
ThotBool		    open;

#endif /* __STDC__ */
{
   int                 i;

   /* on cherche d'abord si ce nom de fichier est dans la table des */
   /* fichiers secondaires ouverts */
   /* on saute les deux premiers fichiers, qui sont stdout et le fichier de */
   /* sortie principal */
   for (i = 2; i < NOutputFiles && ustrcmp (fName, OutputFile[i].OfFileName) != 0; i++) ;
   if (i < NOutputFiles &&
       ustrcmp (fName, OutputFile[i].OfFileName) == 0)
      /* le fichier est dans la table, on retourne son rang */
      return i;
   else if (!open)
      return 0;
   else if (NOutputFiles >= MAX_OUTPUT_FILES)
      /* table saturee */
     {
	TtaDisplaySimpleMessage (INFO, LIB, TMSG_TOO_MANY_OUTPUT_FILES);
	return -1;
     }
   else
     {
	OutputFile[NOutputFiles].OfFileDesc = ufopen (fName, TEXT("w"));
	if (OutputFile[NOutputFiles].OfFileDesc == NULL)
	  {
	     if (!OutputFile[NOutputFiles].OfCannotOpen)
		{
	        TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB,
					TMSG_CREATE_FILE_IMP), fName);
	        OutputFile[NOutputFiles].OfCannotOpen = TRUE;
		}
	  }
	else
	   /* fichier ouvert */
	     OutputFile[NOutputFiles].OfCannotOpen = FALSE;
	ustrcpy (OutputFile[NOutputFiles].OfFileName, fName);
	OutputFile[NOutputFiles].OfBufferLen = 0;
	OutputFile[NOutputFiles].OfIndent = 0;
	OutputFile[NOutputFiles].OfPreviousIndent = 0;
	OutputFile[NOutputFiles].OfLineNumber = 0;
	OutputFile[NOutputFiles].OfStartOfLine = TRUE;
	NOutputFiles++;
	return (NOutputFiles - 1);
     }
}


/*----------------------------------------------------------------------
   PutChar   ecrit le caractere c sur le terminal ou dans le fichier de  
   sortie, selon fileNum. S'il s'agit du fichier de sortie,        
   le caractere est range' dans le buffer de sortie et ce buffer   
   est ecrit dans le fichier des que la longueur limite des lignes 
   est atteinte.                                                   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         PutChar (CHAR_T c, int fileNum, STRING outBuffer, PtrDocument pDoc, ThotBool lineBreak, CHARSET encoding)

#else  /* __STDC__ */
static void         PutChar (c, fileNum, outBuffer, pDoc, lineBreak, encoding)
CHAR_T                c;
int                 fileNum;
STRING              outBuffer;
PtrDocument         pDoc;
ThotBool            lineBreak;
CHARSET             encoding;
#endif /* __STDC__ */

{
   int                 i, j, indent;
   PtrTSchema          pTSch;
   FILE               *fileDesc;
   CHAR_T              tmp[2];
#  ifdef _I18N_
   int                 nb_bytes2write, index;
   unsigned char       mbc [MAX_BYTES];
#  endif /* !_I18N_ */


   if (outBuffer != NULL) {
      /* la sortie doit se faire dans le buffer outBuffer. On ajoute le */
      /* caractere a sortir en fin de ce buffer */
      tmp[0] = c;
      tmp[1] = WC_EOS;
      ustrcat (outBuffer, tmp);
   } else if (fileNum == 0)
          /* la sortie doit se faire dans stdout. On sort le caractere */
          uputchar (c);
   else if (fileNum > 0) {
        /* sortie dans un fichier */
        /* on cherche le schema de traduction du document pour acceder aux */
        /* parametres definissant la longueur de ligne et le caractere de */
        /* fin de ligne */
        pTSch = GetTranslationSchema (pDoc->DocSSchema);
        fileDesc = OutputFile[fileNum].OfFileDesc;
        if (pTSch != NULL && fileDesc != NULL) {
           if (pTSch->TsLineLength == 0) {
              /* pas de longueur max. des lignes de sortie, on ecrit */
              /* directement le caractere dans le fichier de sortie */
#             ifdef _I18N_ 
              nb_bytes2write = TtaWC2MB (c, mbc, CharEncoding);
              for (index = 0; index < nb_bytes2write; index++)
                  putc (mbc[index], fileDesc);
#             else  /* !_I18N_ */
              putc (c, fileDesc);
#             endif /* !_I18N_ */
		   } else if (c == pTSch->TsEOL[0]) {
                  /*  fin de ligne, on ecrit le contenu du buffer de sortie */
                  for (i = 0; i < OutputFile[fileNum].OfBufferLen; i++) {
#                     ifdef _I18N_
                      nb_bytes2write = TtaWC2MB (OutputFile[fileNum].OfBuffer[i], mbc, CharEncoding);
                      for (index = 0; index < nb_bytes2write; index++)
                          putc (mbc[index], fileDesc);
#                     else  /* !_I18N_ */
                      putc (OutputFile[fileNum].OfBuffer[i], fileDesc);
#                     endif /* !_I18N_ */
				  }
                  ufprintf (fileDesc, pTSch->TsEOL);
                  /* le buffer de sortie est vide maintenant */
                  OutputFile[fileNum].OfBufferLen = 0;
                  OutputFile[fileNum].OfLineNumber++;
                  OutputFile[fileNum].OfStartOfLine = TRUE;
		   } else {
                  /* ce n'est pas un caractere de fin de ligne */
			   if (OutputFile[fileNum].OfBufferLen >= MAX_BUFFER_LEN) {
                  /* le buffer de sortie est plein, on ecrit son contenu */
                  for (i = 0; i < OutputFile[fileNum].OfBufferLen; i++) {
#                     ifdef _I18N_
                      nb_bytes2write = TtaWC2MB (OutputFile[fileNum].OfBuffer[i], mbc, CharEncoding);
                      for (index = 0; index < nb_bytes2write; index++)
                          putc (mbc[index], fileDesc);
#                     else  /* !_I18N_ */
                      putc (OutputFile[fileNum].OfBuffer[i], fileDesc);
#                     endif /* !_I18N_ */
				  }
                  OutputFile[fileNum].OfBufferLen = 0;
			   }
               if (OutputFile[fileNum].OfStartOfLine) {
                  if (OutputFile[fileNum].OfIndent >= MAX_BUFFER_LEN)
                     indent = MAX_BUFFER_LEN - 1;
                  else {
                       indent = OutputFile[fileNum].OfIndent;
                       if (indent < 0)
                          indent = 0;
				  }
                  for (j = 0; j < indent; j++)
                      OutputFile[fileNum].OfBuffer[j] = WC_SPACE;
                  OutputFile[fileNum].OfBufferLen = indent;
                  OutputFile[fileNum].OfStartOfLine = FALSE;
			   }
               /* on met le caractere dans le buffer */
               OutputFile[fileNum].OfBuffer[OutputFile[fileNum].OfBufferLen] = c;
               OutputFile[fileNum].OfBufferLen++;
               if (lineBreak)
				   if (OutputFile[fileNum].OfBufferLen > pTSch->TsLineLength) {
                     /* le contenu du buffer depasse la longueur de ligne maximum */
                     /* on cherche le dernier blanc */
                     i = OutputFile[fileNum].OfBufferLen - 1;
                     while (OutputFile[fileNum].OfBuffer[i] != WC_SPACE && i > 0)
                           i--;
                     if (OutputFile[fileNum].OfBuffer[i] == WC_SPACE) {
                        /* on a trouve' le dernier blanc */
                        /* cherche s'il y a au moins un caractere non blanc
                           avant */
                        for (j = i; j > 0 && OutputFile[fileNum].OfBuffer[j] <= WC_SPACE; j--);
						if (OutputFile[fileNum].OfBuffer[j] != WC_SPACE) {
                           /* le blanc trouve' ne fait pas partie des
                           blancs d'indentation */
                           /* on ecrit tout ce qui precede ce blanc */
                           for (j = 0; j < i; j++) {
#                              ifdef _I18N_
                               nb_bytes2write = TtaWC2MB (OutputFile[fileNum].OfBuffer[j], mbc, CharEncoding);
                               for (index = 0; index < nb_bytes2write; index++)
                                   putc (mbc[index], fileDesc);
#                              else  /* !_I18N_ */
                               putc (OutputFile[fileNum].OfBuffer[j], fileDesc); 
#                              endif /* !_I18N_ */
						   }
                           /* on ecrit un saut de ligne */
                           ufprintf (fileDesc, pTSch->TsTranslEOL);
                           OutputFile[fileNum].OfLineNumber++;
                           /* on traite l'indentation */
                           if (OutputFile[fileNum].OfIndent > pTSch->TsLineLength - 10)
                              indent = pTSch->TsLineLength - 10;
                           else
                               indent = OutputFile[fileNum].OfIndent;

                           if (indent < 0)
                              indent = 0;

                           for (j = 0; j < indent; j++)
                               OutputFile[fileNum].OfBuffer[j] = WC_SPACE;
                           i -= indent;
                           i++;
                          if (i < 0)
                             i = 0;
                          OutputFile[fileNum].OfStartOfLine = FALSE;
			       
                          /* on decale ce qui suit le blanc */
                          OutputFile[fileNum].OfBufferLen -= i;
                          if (i > 0 )
                             for (j = indent; j < OutputFile[fileNum].OfBufferLen; j++)
                                 OutputFile[fileNum].OfBuffer[j] = OutputFile[fileNum].OfBuffer[i + j];
						}
					 }
				   }
		}
	  }
   }
}


/*----------------------------------------------------------------------
   PutColor        sort dans fichier le nom de la couleur qui se	
   trouve au rang n dans la table des couleurs.                   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         PutColor (int n, int fileNum, PtrDocument pDoc, ThotBool lineBreak)
#else  /* __STDC__ */
static void         PutColor (n, fileNum, pDoc, lineBreak)
int                 n;
int                 fileNum;
PtrDocument         pDoc;
ThotBool            lineBreak;

#endif /* __STDC__ */
{
   int                 i;
   CHAR_T*             ptr;

   if (n < NColors && n >= 0)
     {
	ptr = Color_Table[n];
	i = 0;
	while (ptr[i] != EOS)
	   PutChar (ptr[i++], fileNum, NULL, pDoc, lineBreak, CharEncoding);
     }
}

/*----------------------------------------------------------------------
   PutPattern  sort dans fichier le nom du motif qui se trouve au	
   rang n dans la table des motifs.				
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         PutPattern (int n, int fileNum, PtrDocument pDoc, ThotBool lineBreak)
#else  /* __STDC__ */
static void         PutPattern (n, fileNum, pDoc, lineBreak)
int                 n;
int                 fileNum;
PtrDocument         pDoc;
ThotBool            lineBreak;

#endif /* __STDC__ */
{
   int                 i;
   CHAR_T*             ptr;

   if (n < NbPatterns && n >= 0)
     {
	ptr = Patterns[n];
	i = 0;
	while (ptr[i] != EOS)
	   PutChar (ptr[i++], fileNum, NULL, pDoc, lineBreak, CharEncoding);
     }
}

/*----------------------------------------------------------------------
   PutInt convertit le nombre n sous la forme d'une chaine de         
   caracteres et sort cette chaine de caracteres dans fichier      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         PutInt (int n, int fileNum, STRING outBuffer, PtrDocument pDoc,
			    ThotBool lineBreak)

#else  /* __STDC__ */
static void         PutInt (n, fileNum, outBuffer, pDoc, lineBreak)
int                 n;
int                 fileNum;
STRING              outBuffer;
PtrDocument         pDoc;
ThotBool            lineBreak;

#endif /* __STDC__ */

{
   CHAR_T                buffer[20];
   int                 i;

   usprintf (buffer, TEXT("%d"), n);
   i = 0;
   while (buffer[i] != EOS)
      PutChar (buffer[i++], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
}

/*----------------------------------------------------------------------
   GetTransSchForContent
   En examinant les elements ascendants de pEl, on cherche un schema de
   traduction qui contienne des regles de traduction de contenu pour les
   feuilles de type leafType.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrTSchema   GetTransSchForContent (PtrElement pEl, LeafType leafType, AlphabetTransl **pTransAlph)

#else  /* __STDC__ */
static PtrTSchema   GetTransSchForContent (pEl, leafType, pTransAlph)
PtrElement      pEl;
LeafType        leafType;
AlphabetTransl** pTransAlph;

#endif /* __STDC__ */

{
   PtrTSchema   pTSch;
   PtrSSchema   pSS;
   PtrElement   pAncestor;
   int          i;
   CHAR_T         alphabet;
   ThotBool     transExist;
   
   pSS = NULL;
   transExist = FALSE;
   pTSch = NULL;
   pAncestor = pEl;
   *pTransAlph = NULL;
   if (pEl->ElTerminal && pEl->ElLeafType == LtText)
      alphabet = TtaGetAlphabet (pEl->ElLanguage);
   else
      alphabet = TEXT('L');
   do
     {
     if (pSS != pAncestor->ElStructSchema)
	/* un schema de structure different du precedent rencontre */
        {
	pSS = pAncestor->ElStructSchema;
	/* schema de traduction de cette structure */
	pTSch = GetTranslationSchema (pSS);
	if (pTSch != NULL)
	   switch (leafType)
              {
	      case LtText:
		 if (pTSch->TsNTranslAlphabets > 0)
		    /* il y a au moins un alphabet a traduire */
		    /* cherche les regles de traduction pour l'alphabet */
		    /* de la feuille */
		    {
		    i = 0;
		    do
		       {
		       *pTransAlph = &pTSch->TsTranslAlphabet[i++];
		       if ((*pTransAlph)->AlAlphabet == alphabet &&
                           (*pTransAlph)->AlBegin > 0)
		           transExist = TRUE;
		       else
			   *pTransAlph = NULL;
		       }
		    while (!transExist && i < pTSch->TsNTranslAlphabets);
		    }
		 break;
	      case LtSymbol:
		 transExist = pTSch->TsSymbolFirst != 0;
		 break;
	      case LtGraphics:
	      case LtPolyLine:
		 transExist = pTSch->TsGraphicsFirst != 0;
		 break;
	      default:
		 break;
	      }
        }
     pAncestor = pAncestor->ElParent;
     }
   while (!transExist && pAncestor != NULL);
   return pTSch;
}

/*----------------------------------------------------------------------
   TranslateText
   effectue les traductions de caracteres selon la table
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         TranslateText (PtrTextBuffer pBufT, PtrTSchema pTSch,
                                AlphabetTransl *pTransAlph, ThotBool lineBreak,
                                int fileNum, PtrDocument pDoc)

#else  /* __STDC__ */
static void         TranslateText (pBufT, pTSch, *pTransAlph, lineBreak, fileNum, pDoc)
PtrTextBuffer   pBufT;
PtrTSchema      pTSch;
AlphabetTransl  *pTransAlph;
ThotBool        lineBreak;
int             fileNum;
PtrDocument     pDoc;

#endif /* __STDC__ */

{
   PtrTextBuffer        pNextBufT, pPrevBufT;
   int                  i, j, k, b, ft, lt;
   CHAR_T               c, cs;
   ThotBool             continu, equal, stop;
   int                  textTransBegin, textTransEnd;
   StringTransl         *pTrans;   

   if (!pBufT)
     return;
   textTransBegin = pTransAlph->AlBegin;
   textTransEnd = pTransAlph->AlEnd;
   b = 0;       /* indice dans la chaine source de la regle de traduction */
   i = 1;       /* indice dans le buffer du caractere a traduire */
   ft = textTransBegin;/* indice de la 1ere regle de traduction a appliquer */
   lt = textTransEnd; /*indice de la derniere regle de traduction a appliquer*/
   pPrevBufT = NULL;  /* buffer source precedent */
   c = pBufT->BuContent[0];     /* 1er caractere a traduire */
   
   /* traduit la suite des buffers source */
   do
      /* Dans la table de traduction, les chaines sources sont */
      /* rangees par ordre alphabetique. On cherche une chaine */
      /* source qui commence par le caractere a traduire. */
      {
      while (c > pTSch->TsCharTransl[ft - 1].StSource[b] && ft < lt)
	 ft++;
      pTrans = &pTSch->TsCharTransl[ft - 1];
      if (c == pTrans->StSource[b])
	 /* le caractere correspond au caractere courant de la */
	 /* chaine source de la regle ft */
	 if (pTrans->StSource[b + 1] == EOS)
	    /* chaine complete */
	    /* cette regle de traduction s'applique, on traduit */
	    /* cherche si les regles suivantes ne peuvent pas egalement */
            /* s'appliquer: on recherche la plus longue chaine a traduire */
	   {
	      continu = ft < textTransEnd;
	      while (continu)
		{
		   j = 0;
		   equal = TRUE;
		   /* compare la regle ft avec la suivante */
		   do
		      if (pTSch->TsCharTransl[ft - 1].StSource[j] ==
			  pTSch->TsCharTransl[ft].StSource[j])
			 j++;
		      else
			 equal = FALSE;
		   while (equal && j <= b);
		   if (!equal)
		      /* le debut de la regle suivante est different */
		      /* de la regle courante */
		      continu = FALSE;
		   else
		      /* la fin de la regle suivante est-il identique */
		      /* a la suite du texte a traduire ? */
		     {
			k = i;
			cs = c;
			pNextBufT = pBufT;
			/* cherche le caractere suivant du texte */
			stop = FALSE;
			do
			  {
			     if (cs != EOS)
				cs = pNextBufT->BuContent[k++];
			     if (cs == EOS)
				/* passe au buffer suivant du meme texte */
				if (pNextBufT->BuNext != NULL)
				  {
				     pNextBufT = pNextBufT->BuNext;
				     k = 1;
				     cs = pNextBufT->BuContent[0];
				  }
			     if (cs == EOS)
				continu = FALSE;	/* fin du texte */
			     else
			        {
				continu = FALSE;
				if (cs == pTSch->TsCharTransl[ft].StSource[j])
				   {
				   stop = FALSE;
				   continu = TRUE;
				   j++;
				   }
				if (pTSch->TsCharTransl[ft].StSource[j] == EOS)
				   {
				   ft++;
				   b = j - 1;
				   i = k;
				   c = cs;
				   pBufT = pNextBufT;
				   continu = ft < textTransEnd;
				   stop = TRUE;
				   }
			        }
			  }
			while (!stop && continu);
		     }
		}
	      /* on applique la regle de traduction ft */
	      j = 0;
	      while (pTSch->TsCharTransl[ft - 1].StTarget[j] != EOS)
		{
		   PutChar (pTSch->TsCharTransl[ft - 1].StTarget[j], fileNum, NULL, pDoc, lineBreak, CharEncoding);
		   j++;
		}
	      /* prepare la prochaine recherche dans la table */
	      b = 0;
	      ft = textTransBegin;
	      lt = textTransEnd;
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
	      while (c == pTSch->TsCharTransl[j - 1].StSource[b] && j < lt)
		 j++;
	      if (c != pTSch->TsCharTransl[j - 1].StSource[b])
		 lt = j - 1;
	      /* passe au caractere suivant de la chaine source */
	      /* de la table de traduction */
	      b++;
	   }
      else
	 /* le caractere ne correspond pas */
      if (b == 0)
	 /* le caractere ne se trouve au debut d'aucune chaine source de la */
         /* table de traduction, on ne le traduit donc pas */
	{
	   ft = textTransBegin;
	   if (c != WC_EOS)
	      PutChar (c, fileNum, NULL, pDoc, lineBreak, CharEncoding);
	}
      else
	 /* on avait commence' a analyser une sequence de caracteres. */
         /* Cette sequence ne se traduit pas, on sort le premier caractere */
         /* de la sequence et on cherche une sequence traduisible a partir */
         /* du caractere suivant. */
	{
	   if (i - b >= 1)
	      /* le premier caractere de la sequence est dans */
	      /* le buffer courant */
	      i -= b;
	   else
	      /* le premier caractere de la sequence est dans */
	      /* le buffer precedent */
	     {
		pBufT = pPrevBufT;
		i = pBufT->BuLength + i - b;
	     }
	   if (c != WC_EOS)
	     PutChar (pBufT->BuContent[i - 1], fileNum, NULL, pDoc, lineBreak, CharEncoding);
	   b = 0;
	   ft = textTransBegin;
	   lt = textTransEnd;
	}
      /* cherche le caractere suivant a traiter */
      if (c != EOS)
	 c = pBufT->BuContent[i++];
      if (c == EOS)
	 /* passe au buffer suivant du meme element de texte */
	 if (pBufT->BuNext != NULL)
	   {
	      pPrevBufT = pBufT;
	      pBufT = pBufT->BuNext;
	      i = 1;
	      c = pBufT->BuContent[0];
	   }
      }
   while (c != EOS);
   /* fin de la feuille de texte */
   /* Si on a commence' a analyser une sequence de caracteres, */
   /* on sort le debut de la sequence. */
   for (i = 0; i <= b - 1; i++)
      PutChar (pTSch->TsCharTransl[ft - 1].StSource[i], fileNum, NULL, pDoc, lineBreak, CharEncoding);
}

/*----------------------------------------------------------------------
   TranslateLeaf   traite l'element feuille pointe' par pEl, en	
   traduisant son contenu si transChar est vrai. Produit le	
   contenu dans le fichier de sortie fileNum.			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         TranslateLeaf (PtrElement pEl, ThotBool transChar, ThotBool lineBreak,
				   int fileNum, PtrDocument pDoc)

#else  /* __STDC__ */
static void         TranslateLeaf (pEl, transChar, lineBreak, fileNum, pDoc)
PtrElement          pEl;
ThotBool            transChar;
ThotBool            lineBreak;
int                 fileNum;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrTSchema          pTSch;
   PtrTextBuffer       pBufT;
   CHAR_T              c;
   int                 i, j, b, ft, lt;
   AlphabetTransl     *pTransAlph;
   StringTransl       *pTrans;

   pTransAlph = NULL;
   lt = 0;
   if (!(pEl->ElLeafType == LtText || pEl->ElLeafType == LtSymbol ||
	 pEl->ElLeafType == LtGraphics || pEl->ElLeafType == LtPolyLine)
       || !transChar)
      pTSch = GetTranslationSchema (pEl->ElStructSchema);
   else
      /* En examinant les elements englobants, on cherche un schema de */
      /* traduction qui contienne des regles pour ce type de feuille */
      pTSch = GetTransSchForContent (pEl, pEl->ElLeafType, &pTransAlph);
   switch (pEl->ElLeafType)
	 {
	    case LtText /* traitement d'une feuille de texte */ :
	       if (pEl->ElTextLength > 0)
		  /* la feuille n'est pas vide */
		 {
		    pBufT = pEl->ElText;	/* 1er buffer a traiter */
		    if (!pTransAlph || !transChar)
		       /* on ne traduit pas quand la table de traduction est vide */
		       /* parcourt les buffers de l'element */
		       while (pBufT != NULL)
			 {
			    i = 0;
			    while (pBufT->BuContent[i] != EOS)
			       PutChar (pBufT->BuContent[i++], fileNum, NULL, pDoc, lineBreak, CharEncoding);
			    pBufT = pBufT->BuNext;
			 }
		    else if (pTSch != NULL)
		       /* effectue les traductions de caracteres selon la table */
                       TranslateText (pBufT, pTSch, pTransAlph, lineBreak,
                                      fileNum, pDoc);
		 }
	       break;

	    case LtSymbol:
	    case LtGraphics:
	    case LtPolyLine:
	       if (pTSch != NULL)
		 {
		    if (!transChar)
		       ft = 0;
		    else
		       /* cherche la premiere et la derniere regle de traduction */
		       /* a appliquer a l'element */
		    if (pEl->ElLeafType == LtSymbol)
		      {
			 ft = pTSch->TsSymbolFirst;
			 lt = pTSch->TsSymbolLast;
		      }
		    else
		      {
			 ft = pTSch->TsGraphicsFirst;
			 lt = pTSch->TsGraphicsLast;
		      }
		    /* prend dans c le caractere qui represente la forme graphique */
		    if (pEl->ElLeafType == LtPolyLine)
		       c = pEl->ElPolyLineType;
		    else
		       c = pEl->ElGraph;
		    if (ft == 0)
		       /* pas de traduction */
		       {
		       if (c != WC_EOS)
		          PutChar (c, fileNum, NULL, pDoc, lineBreak, CharEncoding);
		       }
		    else
		       /* on traduit l'element */
		       /* cherche le symbole dans les chaines sources de la */
		       /* table de traduction */
		      {
			 while (pTSch->TsCharTransl[ft - 1].StSource[0] < c && ft < lt)
			    ft++;
			 if (pTSch->TsCharTransl[ft - 1].StSource[0] == c)
			    /* il y a une regle de traduction pour ce symbole */
			   {
			      b = 0;
			      pTrans = &pTSch->TsCharTransl[ft - 1];
			      while (pTrans->StTarget[b] != EOS)
				{
				   PutChar (pTrans->StTarget[b], fileNum, NULL, pDoc, lineBreak, CharEncoding);
				   b++;
				}
			   }
			 else
			    /* ce symbole ne se traduit pas */
		            if (c != WC_EOS)
			       PutChar (c, fileNum, NULL, pDoc, lineBreak, CharEncoding);
		      }
		    if (pEl->ElLeafType == LtPolyLine)
		       if (pEl->ElNPoints > 0)
			  /* la ligne a au moins un point de controle */
			  /* on ecrit les coordonnees des points de controle */
			 {
			    pBufT = pEl->ElPolyLineBuffer;	/* 1er buffer a traiter */
			    /* parcourt les buffers de l'element */
			    while (pBufT != NULL)
			      {
				 for (i = 0; i < pBufT->BuLength; i++)
				   {
				      PutChar (TEXT(' '), fileNum, NULL, pDoc, lineBreak, CharEncoding);
				      PutInt (pBufT->BuPoints[i].XCoord, fileNum, NULL, pDoc, lineBreak);
				      PutChar (TEXT(','), fileNum, NULL, pDoc, lineBreak, CharEncoding);
				      PutInt (pBufT->BuPoints[i].YCoord, fileNum, NULL, pDoc, lineBreak);
				   }
				 pBufT = pBufT->BuNext;
			      }
			 }
		 }
	       break;

	    case LtPicture:
	       /* Si le schema de traduction comporte un buffer */
	       /* pour les images, le nom du fichier contenant l'image */
	       /* est range' dans ce buffer */
	       if (pTSch != NULL)
		  if (pTSch->TsPictureBuffer > 0)
		    {
		       b = pTSch->TsPictureBuffer;
		       pTSch->TsBuffer[b - 1][0] = EOS;	/* raz du buffer */
		       if (pEl->ElTextLength > 0)
			  /* la feuille n'est pas vide */
			 {
			    j = 0;
			    pBufT = pEl->ElText;	/* 1er buffer a traiter */
			    /* parcourt les buffers de l'element */
			    while (pBufT != NULL)
			      {
				 i = 0;
				 do
				    pTSch->TsBuffer[b - 1][j++] = pBufT->BuContent[i++];
				 while (pBufT->BuContent[i - 1] != EOS &&
					b < MAX_TRANSL_BUFFER_LEN);
				 pBufT = pBufT->BuNext;
			      }
			    if (j > 0)
			       pTSch->TsBuffer[b - 1][j - 1] = EOS;
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

/*----------------------------------------------------------------------
   PresRuleValue      retourne le code caractere de la valeur de la   
   regle de presentation specifique pointee par pPRule.            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static CHAR_T         PresRuleValue (PtrPRule pPRule)

#else  /* __STDC__ */
static CHAR_T         PresRuleValue (pPRule)
PtrPRule            pPRule;

#endif /* __STDC__ */

{
   CHAR_T                val;

   val = WC_SPACE;
   switch (pPRule->PrType)
	 {
	    case PtFont:
	    case PtStyle:
	    case PtWeight:
	    case PtUnderline:
	    case PtThickness:
	    case PtLineStyle:
	       val = pPRule->PrChrValue;
	       break;
	    case PtJustify:
	    case PtHyphenate:
	       if (pPRule->PrJustify)
		  val = TEXT('Y');
	       else
		  val = TEXT('N');
	       break;
	    case PtAdjust:
	       switch (pPRule->PrAdjust)
		     {
			case AlignLeft:
			   val = TEXT('L');
			   break;
			case AlignRight:
			   val = TEXT('R');
			   break;
			case AlignCenter:
			   val = TEXT('C');
			   break;
			case AlignLeftDots:
			   val = TEXT('D');
			   break;
		     }
	       break;
	    default:
	       val = WC_SPACE;
	       break;
	 }
   return val;
}


/*----------------------------------------------------------------------
   EmptyElement       retourne TRUE si l'element pEl est vide ou n'a  
   que des descendants vides.                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool     EmptyElement (PtrElement pEl)

#else  /* __STDC__ */
static ThotBool     EmptyElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   ThotBool            empty;

   empty = TRUE;
   if (pEl->ElTerminal)
      /* l'element est une feuille. On traite selon le type de feuille */
      switch (pEl->ElLeafType)
	    {
	       case LtText:
	       case LtPicture:
		  empty = (pEl->ElTextLength == 0);
		  break;
	       case LtGraphics:
	       case LtSymbol:
		  empty = (pEl->ElGraph == EOS);
		  break;
	       case LtPageColBreak:
		  /* un saut de page est consideree comme vide */
		  empty = TRUE;
		  break;
	       case LtReference:
		  if (pEl->ElReference != NULL)
		     if (pEl->ElReference->RdReferred != NULL)
			empty = FALSE;
		  break;
	       case LtPairedElem:
		  /* un element de paire n'est jamais considere' comme vide */
		  empty = FALSE;
		  break;
	       case LtPolyLine:
		  empty = (pEl->ElNPoints == 0);
		  break;
	       default:
		  empty = FALSE;
		  break;
	    }
   else
      /* ce n'est pas une feuille, on traite recursivement tous les fils */
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL && empty)
	   if (!EmptyElement (pChild))
	      empty = FALSE;
	   else
	      pChild = pChild->ElNext;
     }
   return empty;
}


/*----------------------------------------------------------------------
   ConditionIsTrue   evalue la condition du bloc de regles pointe' par
   pBlock pour l'element pointe' par pEl et l'attribut pointe' par 
   pAttr s'il est different de NULL. Retourne vrai si la condition est
   satisfaite, faux sinon.                                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static ThotBool     ConditionIsTrue (PtrTRuleBlock pBlock, PtrElement pEl,
				     PtrAttribute pAttr, PtrDocument pDoc)

#else  /* __STDC__ */
static ThotBool     ConditionIsTrue (pBlock, pEl, pAttr, pDoc)
PtrTRuleBlock       pBlock;
PtrElement          pEl;
PtrAttribute        pAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute        pAttrEl;
   PtrSSchema          pSS, pRefSS;
   PtrElement          pEl1, pElem, pSibling;
   SRule              *pSRule;
   PtrPRule            pPRule;
   TranslCondition    *Cond;
   PtrReference        pRef;
   DocumentIdentifier  docIdent;
   PtrDocument         pExtDoc;
   int                 i, par, nCond;
   ThotBool            ret, possibleRef, typeOK, stop;

   if (pBlock->TbNConditions == 0)
      /* no condition */
      ret = TRUE;
   else
      /* il y a au moins une condition a evaluer */
     {
	ret = TRUE;
	nCond = 0;
	/* evalue les conditions du bloc jusqu'a en trouver une fausse */
	while (ret && nCond < pBlock->TbNConditions)
	  {
	     Cond = &pBlock->TbCondition[nCond++];
	     if (!Cond->TcTarget)
		pElem = pEl;
	     else
		/* la condition porte sur l'element pointe' par pEl ou pAttr.
		   on cherche cet element pointe' */
	       {
		  pElem = NULL;
		  if (pAttr != NULL &&
		      pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType ==
		      AtReferenceAttr)
		     /* c'est un attribut reference */
		     pRef = pAttr->AeAttrReference;
		  else
		     /* l'element est-il une reference ? */
		  if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
		     pRef = pEl->ElReference;
		  else
		     /* c'est peut-etre une inclusion */
		     pRef = pEl->ElSource;
		  if (pRef != NULL)
		     pElem = ReferredElement (pRef, &docIdent, &pExtDoc);
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
			    if (Cond->TcAscendNature[0] == EOS)
			       /* le type de l'ascendant est defini dans le meme schema de
			          structure que l'element traite' */
			       typeOK = EquivalentSRules (Cond->TcAscendType, pEl->ElStructSchema,
							  pElem->ElTypeNumber, pElem->ElStructSchema, pElem->ElParent);
			    else
			       /* le type de l'ascendant est defini dans un autre schema */
			       if (ustrcmp (Cond->TcAscendNature,
					pElem->ElStructSchema->SsName) == 0)
			       typeOK = EquivalentSRules (Cond->TcAscendType,
				 pElem->ElStructSchema, pElem->ElTypeNumber,
				    pElem->ElStructSchema, pElem->ElParent);
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
				 pSibling = pElem->ElPrevious;
				 /* on saute les marques de page precedentes */
				 BackSkipPageBreak (&pSibling);
				 ret = (pSibling == NULL);
				 break;

			      case TcondLast:
				 /* on saute les marques de page suivantes */
				 pSibling = pElem->ElNext;
				 stop = FALSE;
				 do
				    if (pSibling == NULL)
				       stop = TRUE;
				    else if (pSibling->ElTypeNumber == PageBreak + 1)
				       pSibling = pSibling->ElNext;
				    else
				       stop = TRUE;
				 while (!stop);
				 /* l'element est dernier s'il n'a pas de succcesseur */
				 ret = pSibling == NULL;
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
					      pEl1->ElStructSchema->SsCode == pElem->ElStructSchema->SsCode;
					}
				   }
				 while (!ret && par < MAX_PARAM_DOC);
				 break;

			      case TcondReferred:
				 /* la condition est satisfaite si l'element (ou le */
				 /* premier de ses ascendants sur lequel peut porter une */
				 /* reference) est reference' au moins une fois. */
				 ret = FALSE;
				 pEl1 = pElem;
				 possibleRef = FALSE;

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
					   pRefSS = pEl1->ElStructSchema;
					   /* on cherche toutes les references dans le schema de */
					   /* structure de l'element */
					   possibleRef = FALSE;
					   i = 1;
					   do
					     {
						pSRule = &pRefSS->SsRule[i++];
						if (pSRule->SrConstruct == CsReference)
						   /* c'est une reference */
						   if (pSRule->SrReferredType != 0)
						      possibleRef = EquivalentSRules (pSRule->SrReferredType, pRefSS, pEl1->ElTypeNumber, pRefSS, pEl1->ElParent);
					     }
					   while (!possibleRef && i < pRefSS->SsNRules);
					   if (!possibleRef)
					     {
						/* l'element ne peut pas etre designe par un element */
						/* reference on cherche s'il peut etre designe' par un */
						/* attribut reference on cherche tous les attributs */
						/* reference dans le schema de structure de l'element */
						i = 1;
						do
						  {
						     if (pRefSS->SsAttribute[i++].AttrType == AtReferenceAttr)
							/* c'est une reference */
							if (pRefSS->SsAttribute[i - 1].AttrTypeRef != 0)
							   possibleRef = (pRefSS->SsAttribute[i - 1].AttrTypeRefNature[0] == EOS	/* meme schema de structure */
									  && EquivalentSRules (pRefSS->SsAttribute[i - 1].AttrTypeRef, pRefSS, pEl1->ElTypeNumber, pRefSS, pEl1->ElParent));
						  }
						while (!possibleRef &&
						 i < pRefSS->SsNAttributes);
					     }
					   if (!possibleRef)
					      /* l'element ne peut pas etre designe' par une reference
					         type'e ; on examine l'element ascendant */
					      pEl1 = pEl1->ElParent;
					}
				   }
				 while (!possibleRef && pEl1 != NULL && !ret);
				 break;

			      case TcondFirstRef:
			      case TcondLastRef:
				 /* la condition est satisfaite s'il s'agit de la premiere */
				 /* (ou de la derniere) reference a l'element reference' */
				 pRef = NULL;
				 if (pAttr != NULL &&
				     pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
				    /* c'est un attribut reference */
				    pRef = pAttr->AeAttrReference;
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
				 if (pAttr != NULL &&
				     pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
				    /* c'est un attribut reference */
				    pRef = pAttr->AeAttrReference;
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
					{
					   pSibling = pElem->ElPrevious;
					   /* on saute les marques de page precedentes */
					   BackSkipPageBreak (&pSibling);
					   ret = (pSibling == NULL);
					}
				      if (ret)
					{
					   if (Cond->TcElemNature[0] == EOS)
					      /* le type de l'ascendant est defini dans le meme schema de
					         structure que l'element traite' */
					      pSS = pEl->ElStructSchema;
					   else
					      /* le type de l'ascendant est defini dans un autre schema */
					      pSS = NULL;
					   i = 0;
					   if (Cond->TcImmediatelyWithin)
					      /* Condition: If immediately within n element-type */
					      /* Les n premiers ancetres successifs doivent etre du type
					         TcElemType, sans comporter d'elements d'autres type */
					      /* on compte les ancetres successifs de ce type */
					      while (pEl1 != NULL)
						{
						   if (pSS != NULL)
						      typeOK = EquivalentType (pEl1, Cond->TcElemType, pSS);
						   else if (ustrcmp (Cond->TcElemNature, pEl1->ElStructSchema->SsName) == 0)
						      typeOK = EquivalentType (pEl1, Cond->TcElemType, pEl1->ElStructSchema);
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
						   if (pSS != NULL)
						      typeOK = EquivalentType (pEl1, Cond->TcElemType, pSS);
						   else if (ustrcmp (Cond->TcElemNature, pEl1->ElStructSchema->SsName) == 0)
						      typeOK = EquivalentType (pEl1, Cond->TcElemType, pEl1->ElStructSchema);
						   else
						      typeOK = FALSE;
						   if (typeOK)
						      i++;
						   pEl1 = pEl1->ElParent;	/* passe a l'element ascendant */
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
				 pSS = pEl->ElStructSchema;
				 pEl1 = pElem;
				 while (pEl1 != NULL && !ret)
				   {
				      pAttrEl = pEl1->ElFirstAttr;	/* 1er attribut de l'element */
				      /* parcourt les attributs de l'element */
				      while (pAttrEl != NULL && !ret)
					{
					   if (pAttrEl->AeAttrSSchema->SsCode == pSS->SsCode &&
					       pAttrEl->AeAttrNum == Cond->TcAttr)
					      /* c'est l'attribut cherche', on teste sa valeur */
					      switch (pSS->SsAttribute[pAttrEl->AeAttrNum - 1].AttrType)
						    {
						       case AtNumAttr:
							  ret = pAttrEl->AeAttrValue <= Cond->TcUpperBound &&
							     pAttrEl->AeAttrValue >= Cond->TcLowerBound;
							  break;
						       case AtTextAttr:
							  if (Cond->TcTextValue[0] == EOS)
							     ret = TRUE;
							  else
							     ret = StringAndTextEqual (Cond->TcTextValue,
										       pAttrEl->AeAttrText);
							  break;
						       case AtReferenceAttr:
							  ret = TRUE;
							  break;
						       case AtEnumAttr:
							  ret = pAttrEl->AeAttrValue == Cond->TcAttrValue ||
							     Cond->TcAttrValue == 0;
							  break;
						       default:
							  break;
						    }
					   if (!ret)
					      pAttrEl = pAttrEl->AeNext;
					}
				      if (Cond->TcAscendRelLevel == -1)
					 /* on peut poursuivre la recherche parmi les ascendants */
					 pEl1 = pEl1->ElParent;		/* passe a l'element ascendant */
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
				 pPRule = pElem->ElFirstPRule;
				 /* parcourt les presentations specifiques de l'element */
				 while (pPRule != NULL && !ret)
				   {
				      if (pPRule->PrType == (PRuleType)(Cond->TcAttr))
					 /* c'est la presentation cherchee, on teste sa valeur */
					 if (pPRule->PrType == PtSize || pPRule->PrType == PtIndent ||
					  pPRule->PrType == PtLineSpacing ||
					     pPRule->PrType == PtLineWeight)
					    /* c'est une presentation a valeur numerique */
					    ret = pPRule->PrMinValue <= Cond->TcUpperBound &&
					       pPRule->PrMinValue >= Cond->TcLowerBound;
					 else if (pPRule->PrType == PtFillPattern ||
					    pPRule->PrType == PtBackground ||
					     pPRule->PrType == PtForeground)
					    ret = pPRule->PrIntValue <= Cond->TcUpperBound &&
					       pPRule->PrIntValue >= Cond->TcLowerBound;
					 else
					    ret = Cond->TcPresValue == PresRuleValue (pPRule) ||
					       Cond->TcPresValue == EOS;
				      if (!ret)
					 pPRule = pPRule->PrNextPRule;
				   }
				 break;
			      case TcondElementType:
				 /* cherche si l'attribut porte sur un element
				    du type voulu */
				 ret = FALSE;
				 if (pAttr != NULL)
				   if (pElem->ElTypeNumber == Cond->TcAttr &&
				       pElem->ElStructSchema->SsCode == pAttr->AeAttrSSchema->SsCode)
				      ret = TRUE;
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
				 /* attribut pAttr est le 1er de l'element */
				 if (pAttr != NULL)
				    ret = pAttr == pEl->ElFirstAttr;
				 break;
			      case TcondLastAttr:
				 /* la condition est satisfaite si le bloc */
				 /* attribut pAttr est le dernier de l'element */
				 if (pAttr != NULL)
				    ret = pAttr->AeNext == NULL;
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
			      case TcondEmpty:
				 /* la condition est satisfaite si l'element est vide */
				 ret = EmptyElement (pElem);
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


/*----------------------------------------------------------------------
   CounterVal     retourne la valeur du compteur de numero countNum	
   (defini dans le schema de traduction  pointe' par pTSch qui     
   s'applique au schema de structure pointe' par pSS) pour         
   l'element pointe' par pElNum.                                   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static int          CounterVal (int countNum, PtrTSchema pTSch, PtrSSchema pSS, PtrElement pElNum)

#else  /* __STDC__ */
static int          CounterVal (countNum, pTSch, pSS, pElNum)
int                 countNum;
PtrTSchema          pTSch;
PtrSSchema          pSS;
PtrElement          pElNum;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrSSchema          pSSch;
   TCounter           *pCntr;
   PtrAttribute        pAttr;
   int                 val, valInit, level;
   ThotBool            initAttr, stop;

#define MAX_ANCESTOR 50
   PtrElement          pAncest[MAX_ANCESTOR];

   valInit = 0;
   pCntr = &pTSch->TsCounter[countNum - 1];
   if (pCntr->TnOperation == TCntrNoOp)
      val = pCntr->TnParam1;
   else
     {
	/* a priori, la valeur initiale du compteur ne depend pas d'un */
	/* attribut */
	initAttr = FALSE;
	if (pCntr->TnAttrInit > 0)
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
		else if (pAttr->AeAttrNum == pCntr->TnAttrInit &&
			 pAttr->AeAttrSSchema->SsCode == pSS->SsCode)
		   stop = TRUE;	/* c'est l'attribut cherche' */
		else
		   pAttr = pAttr->AeNext;	/* au suivant */
	     while (!stop);
	     if (pAttr != NULL)
		/* la racine porte bien l'attribut qui initialise le compteur */
	       {
		  initAttr = TRUE;
		  valInit = pAttr->AeAttrValue;		/* on prend la valeur de l'attribut */
	       }
	  }
	if (pCntr->TnOperation == TCntrRLevel)
	  {
	     pEl = pElNum;
	     val = 0;
	     /* parcourt les elements englobants de l'element sur lequel porte */
	     /* le calcul du compteur */
	     while (pEl != NULL)
	       {
		  if (pEl->ElTypeNumber == pCntr->TnElemType1 &&
		      pEl->ElStructSchema->SsCode == pElNum->ElStructSchema->SsCode)
		     /* l'element rencontre' a la meme type que l'element traite' */
		     val++;	/* incremente le compteur */
		  pEl = pEl->ElParent;
	       }
	  }
	else
	  {
	     if (pCntr->TnElemType1 == PageBreak + 1)
		/* c'est un compteur de pages */
	       {
		  pSSch = NULL;
		  pEl = pElNum;
	       }
	     else
	       {
		  /* schema de structure du compteur */
		  pSSch = pElNum->ElStructSchema;
		  if (pCntr->TnOperation != TCntrRank || pCntr->TnAcestorLevel == 0)
		     /* Cherche le premier element de type TnElemType1 */
		     /* englobant l'element a numeroter */
		     pEl = GetTypedAncestor (pElNum, pCntr->TnElemType1, pSSch);
		  else
		    {
		       /* Cherche le nieme element de type TnElemType1 qui englobe */
		       /* l'element a numeroter */
		       if (pCntr->TnAcestorLevel < 0)
			  /* on compte les ascendants en remontant de l'element */
			  /* concerne' vers la racine */
			  level = -pCntr->TnAcestorLevel;
		       else
			  /* on compte les ascendants en descendant de la racine vers */
			  /* l'element concerne'. Pour cela on commence par enregistrer */
			  /* le chemin de l'element concerne' vers la racine */
			  level = MAX_ANCESTOR;
		       pEl = pElNum;
		       while (level > 0 && pEl != NULL)
			 {
			    if (pEl->ElTypeNumber == pCntr->TnElemType1 &&
				pEl->ElStructSchema->SsCode == pElNum->ElStructSchema->SsCode)
			       /* cet element englobant a le type qui incremente le compteur */
			      {
				 level--;
				 pAncest[level] = pEl;
			      }
			    if (level > 0)
			       pEl = pEl->ElParent;
			 }
		       if (pCntr->TnAcestorLevel > 0)
			  /* il faut redescendre a partir de la racine */
			  pEl = pAncest[level + pCntr->TnAcestorLevel - 1];
		    }
	       }
	     if (pEl == NULL)
		val = 0;	/* pas trouve' */
	     else if (pCntr->TnOperation == TCntrRank)
		if (pCntr->TnElemType1 == PageBreak + 1)
		   /* c'est un compteur de pages */
		  {
		     if (pEl->ElTypeNumber == PageBreak + 1 && pEl->ElViewPSchema == 1)
			val = 1;
		     else if (initAttr)
			val = valInit;
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
		     while (pEl != NULL);
		  }
		else
		   /* numero = rang de l'element dans la liste */
		   /* Cherche le rang de l'element trouve' parmi ses freres */
		  {
		     if (initAttr)
			val = valInit;
		     else
			val = 1;
		     while (pEl->ElPrevious != NULL)
		       {
			  pEl = pEl->ElPrevious;
			  if (EquivalentType (pEl, pCntr->TnElemType1, pSSch))
			     /* on ne compte pas les eventuelles marques de page */
			     val++;
		       }
		  }
	     else
		/* l'element trouve est celui qui reinitialise le compteur */
	       {
		  if (initAttr)
		     val = valInit - pCntr->TnParam2;
		  else
		     val = pCntr->TnParam1 - pCntr->TnParam2;
		  /* a partir de l'element trouve', cherche en avant tous les */
		  /* elements ayant le type qui incremente le compteur, */
		  /* jusqu'a rencontrer celui pour lequel on calcule la valeur du */
		  /* compteur. */
		  do
		    {
		       pEl = FwdSearchElem2Types (pEl, pCntr->TnElemType2, pElNum->ElTypeNumber, pSSch,
						  pElNum->ElStructSchema);
		       if (pEl != NULL)
			  if (EquivalentType (pEl, pCntr->TnElemType2, pSSch))
			     /* on ignore les pages qui ne concernent */
			     /* pas la vue 1 */
			     if (pEl->ElTypeNumber != PageBreak + 1 ||
				 (pEl->ElTypeNumber == PageBreak + 1 &&
				  pEl->ElViewPSchema == 1))
				val += pCntr->TnParam2;
		    }
		  while (pEl != NULL && pEl != pElNum);
	       }
	  }
     }
   return val;
}


/*----------------------------------------------------------------------
   SearchDescent   cherche dans le sous-arbre de racine pEl (racine	
   exclue), un element de type typeNum defini dans le schema de    
   structure pointe' par pSS, ou si pSS est nul dans le schema de  
   nom schemaName.                                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         SearchDescent (PtrElement * pEl, int typeNum, PtrSSchema pSS, Name schemaName)

#else  /* __STDC__ */
static void         SearchDescent (pEl, typeNum, pSS, schemaName)
PtrElement         *pEl;
int                 typeNum;
PtrSSchema          pSS;
Name                schemaName;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   PtrSSchema          pSSchema;
   ThotBool            SSchemaOK;

   if ((*pEl)->ElTerminal)
      *pEl = NULL;		/* element terminal, echec */
   else if ((*pEl)->ElFirstChild == NULL)
      *pEl = NULL;		/* pas de fils, echec */
   else
      /* il y a au moins un descendant direct */
     {
	pChild = (*pEl)->ElFirstChild;
	/* cherche dans le sous-arbre de chacun des fils */
	do
	  {
	     /* le fils a-t-il le type cherche' ? */
	     if (pSS == NULL)
	       {
		  SSchemaOK = ustrcmp (schemaName, pChild->ElStructSchema->SsName) == 0;
		  pSSchema = pChild->ElStructSchema;
	       }
	     else
	       {
		  SSchemaOK = pChild->ElStructSchema->SsCode == pSS->SsCode;
		  pSSchema = pSS;
	       }
	     if (SSchemaOK && EquivalentSRules (typeNum, pSSchema, pChild->ElTypeNumber, pSSchema, *pEl))
		/* trouve' */
		*pEl = pChild;
	     else
	       {
		  *pEl = pChild;
		  SearchDescent (pEl, typeNum, pSS, schemaName);
		  if (*pEl == NULL)
		     pChild = pChild->ElNext;
	       }
	  }
	while ((*pEl) == NULL && pChild != NULL);
     }
}


/*----------------------------------------------------------------------
   PutContent   e'crit le contenu des feuilles de l'element pEl dans  
   dans le fichier.                                                
   La traduction du contenu des feuilles a lieu seulement si       
   transChar est vrai.                                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         PutContent (PtrElement pEl, ThotBool transChar, ThotBool lineBreak,
				int fileNum, PtrDocument pDoc)

#else  /* __STDC__ */
static void         PutContent (pEl, transChar, lineBreak, fileNum, pDoc)
PtrElement          pEl;
ThotBool            transChar;
ThotBool            lineBreak;
int                 fileNum;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrElement          pChild;

   if (!pEl->ElTerminal)
      /* l'element n'est pas une feuille, on ecrit le contenu de chacun de */
      /* ses fils */
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL)
	  {
	     PutContent (pChild, transChar, lineBreak, fileNum, pDoc);
	     pChild = pChild->ElNext;
	  }
     }
   else
      /* l'element est une feuille, on sort son contenu */
      TranslateLeaf (pEl, transChar, lineBreak, fileNum, pDoc);
}

#ifdef __STDC__
static void         ApplyTRule (PtrTRule pTRule, PtrTSchema pTSch,
			PtrSSchema pSSch, PtrElement pEl, ThotBool * transChar,
				ThotBool * lineBreak, ThotBool * removeEl,
				PtrPRule pRPres, PtrAttribute pAttr,
				PtrDocument pDoc, ThotBool recordLineNb);

#else  /* __STDC__ */
static void         ApplyTRule ( /* pTRule, pTSch, pSSch, pEl, transChar,
				lineBreak, removeEl, pRPres, pAttr, pDoc,
				recordLineNb */ );

#endif /* __STDC__ */


/*----------------------------------------------------------------------
   ApplyAttrRulesToElem    applique a l'element pEl les regles de	
   traduction associees a l'attribut pAttr.			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ApplyAttrRulesToElem (TOrder position, PtrElement pEl,
				     PtrAttribute pAttr, ThotBool * removeEl,
				   ThotBool * transChar, ThotBool * lineBreak,
					  PtrDocument pDoc, ThotBool recordLineNb)

#else  /* __STDC__ */
static void         ApplyAttrRulesToElem (position, pEl, pAttr, removeEl, transChar, lineBreak,
					  pDoc, recordLineNb)
TOrder              position;
PtrElement          pEl;
PtrAttribute        pAttr;
ThotBool           *removeEl;
ThotBool           *transChar;
ThotBool           *lineBreak;
PtrDocument         pDoc;
ThotBool            recordLineNb;

#endif /* __STDC__ */

{
   PtrTRuleBlock       pBlock;
   PtrTRule            pTRule;
   PtrTSchema          pTSchAttr;
   AttributeTransl    *pAttrTrans;
   TranslNumAttrCase  *pTCase;
   NotifyAttribute     notifyAttr;
   int                 i;

   /* prepare et envoie l'evenement AttrExport.Pre s'il est demande' */
   notifyAttr.event = TteAttrExport;
   notifyAttr.document = (Document) IdentDocument (pDoc);
   notifyAttr.element = (Element) pEl;
   notifyAttr.attribute = (Attribute) pAttr;
   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
   notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
   if (CallEventAttribute (&notifyAttr, TRUE))
      /* l'application ne laisse pas l'editeur ecrire l'attribut */
      return;
   /* cherche le premier bloc de regles correspondant a l'attribut */
   pTSchAttr = GetTranslationSchema (pAttr->AeAttrSSchema);
   pBlock = NULL;
   if (pTSchAttr != NULL)
     {
	pAttrTrans = &pTSchAttr->TsAttrTRule[pAttr->AeAttrNum - 1];
	switch (pAttr->AeAttrType)
	      {
		 case AtNumAttr:
		    i = 1;
		    while (pBlock == NULL && i <= pAttrTrans->AtrNCases)
		      {
			 pTCase = &pAttrTrans->AtrCase[i - 1];
			 if (pAttr->AeAttrValue <= pTCase->TaUpperBound &&
			     pAttr->AeAttrValue >= pTCase->TaLowerBound)
			    pBlock = pTCase->TaTRuleBlock;
			 i++;
		      }
		    break;
		 case AtTextAttr:
		    if (pAttrTrans->AtrTextValue[0] == EOS)
		       pBlock = pAttrTrans->AtrTxtTRuleBlock;
		    else if (StringAndTextEqual (pAttrTrans->AtrTextValue, pAttr->AeAttrText))
		       pBlock = pAttrTrans->AtrTxtTRuleBlock;
		    break;
		 case AtReferenceAttr:
		    pBlock = pAttrTrans->AtrRefTRuleBlock;
		    break;
		 case AtEnumAttr:
		    pBlock = pAttrTrans->AtrEnuTRuleBlock[pAttr->AeAttrValue];
		    if (pBlock == NULL)
		       /* pas de regles de traduction pour cette valeur, on */
		       /* prend les regles qui s'appliquent a toute valeur */
		       pBlock = pAttrTrans->AtrEnuTRuleBlock[0];
		    break;
		 default:
		    break;
	      }
     }
   /* parcourt les blocs de regles de la valeur de l'attribut */
   while (pBlock != NULL)
     {
	if (ConditionIsTrue (pBlock, pEl, pAttr, pDoc))
	   /* la condition du bloc est verifiee */
	  {
	     pTRule = pBlock->TbFirstTRule;	/* premiere regle du bloc */
	     /* parcourt les regles du bloc */
	     while (pTRule != NULL)
	       {
		  if (pTRule->TrOrder == position)
		     /* c'est une regle a appliquer a cette position */
		     if (pTRule->TrType == TRemove)
			*removeEl = TRUE;
		     else if (pTRule->TrType == TNoTranslation)
			*transChar = FALSE;
		     else if (pTRule->TrType == TNoLineBreak)
			*lineBreak = FALSE;
		     else
			/* on applique la regle */
			ApplyTRule (pTRule, pTSchAttr, pAttr->AeAttrSSchema,
				    pEl, transChar, lineBreak, removeEl, NULL,
				    pAttr, pDoc, recordLineNb);
		  /* passe a la regle suivante */
		  pTRule = pTRule->TrNextTRule;
	       }
	  }
	/* passe au bloc suivant */
	pBlock = pBlock->TbNextBlock;
     }
   /* prepare et envoie l'evenement AttrExport.Post s'il est demande' */
   notifyAttr.event = TteAttrExport;
   notifyAttr.document = (Document) IdentDocument (pDoc);
   notifyAttr.element = (Element) pEl;
   notifyAttr.attribute = (Attribute) pAttr;
   notifyAttr.attributeType.AttrTypeNum = pAttr->AeAttrNum;
   notifyAttr.attributeType.AttrSSchema = (SSchema) (pAttr->AeAttrSSchema);
   CallEventAttribute (&notifyAttr, FALSE);
}


/*----------------------------------------------------------------------
   ApplyAttrRules                                                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ApplyAttrRules (TOrder position, PtrElement pEl, ThotBool * removeEl,
		 ThotBool * transChar, ThotBool * lineBreak, PtrDocument pDoc,
		 ThotBool recordLineNb)

#else  /* __STDC__ */
static void         ApplyAttrRules (position, pEl, removeEl, transChar, lineBreak, pDoc, recordLineNb)
TOrder              position;
PtrElement          pEl;
ThotBool           *removeEl;
ThotBool           *transChar;
ThotBool           *lineBreak;
PtrDocument         pDoc;
ThotBool            recordLineNb;

#endif /* __STDC__ */

{
   PtrElement          pAsc;
   PtrAttribute        pAttr;
   PtrTSchema          pTSch;
   int                 att, nAttr = 0;

#define MAX_ATTR_TABLE 50
   PtrAttribute        AttrTable[MAX_ATTR_TABLE];

   pAttr = pEl->ElFirstAttr;	/* 1er attribut de l'element */
   /* Si on applique les regles "After", on commence par le dernier attribut */
   /* et on traitera les attributs dans l'ordre inverse */
   if (position == TAfter && pAttr != NULL)
     {
	nAttr = 0;
	while (pAttr->AeNext != NULL && nAttr < MAX_ATTR_TABLE)
	  {
	     AttrTable[nAttr++] = pAttr;
	     pAttr = pAttr->AeNext;
	  }
     }

   /* parcourt les attributs de l'element */
   while (pAttr != NULL)
     {
	pTSch = GetTranslationSchema (pAttr->AeAttrSSchema);
	if (pTSch != NULL)
	   if (pTSch->TsAttrTRule[pAttr->AeAttrNum - 1].AtrElemType == 0)
	      /* les regles de traduction de l'attribut s'appliquent a */
	      /* n'importe quel type d'element, on les applique */
	      ApplyAttrRulesToElem (position, pEl, pAttr, removeEl, transChar, lineBreak,
				    pDoc, recordLineNb);
	if (position == TAfter)
	   /* passe a l'attribut precedent de l'element */
	  {
	     if (nAttr > 0)
	       {
		  nAttr--;
		  pAttr = AttrTable[nAttr];
	       }
	     else
		pAttr = NULL;
	  }
	else
	   /* passe a l'attribut suivant de l'element */
	   pAttr = pAttr->AeNext;
     }
   /* produit la traduction des attributs des elements ascendants qui */
   /* s'appliquent aux elements du type de notre element */
   pTSch = GetTranslationSchema (pEl->ElStructSchema);
   if (pTSch != NULL)
      if (pTSch->TsInheritAttr[pEl->ElTypeNumber - 1])
	 /* il y a effectivement heritage d'attribut pour ce type d'element */
	{
	   /* cherche tous les attributs dont ce type d'element peut */
	   /* heriter. */
	   /* balaye la table des attributs */
	   for (att = 1; att <= pEl->ElStructSchema->SsNAttributes; att++)
	      if (pTSch->TsAttrTRule[att - 1].AtrElemType == pEl->ElTypeNumber)
		 /* cet attribut s'applique a ce type d'element */
		{
		   /* Y a-t-il un element ascendant qui porte cet attribut? */
		   pAsc = pEl;	/* on commence par l'element lui-meme */
		   while (pAsc != NULL)		/* parcourt les ascendants */
		     {
			/* parcourt les attributs de chaque ascendant */
			pAttr = pAsc->ElFirstAttr;
			while (pAttr != NULL)
			   if (pAttr->AeAttrSSchema->SsCode ==
				pEl->ElStructSchema->SsCode &&
			       pAttr->AeAttrNum == att)
			      /* on a trouve' */
			     {
				/* applique les regles de traduction de */
				/* l'attribut a l'element */
				ApplyAttrRulesToElem (position, pEl, pAttr, removeEl, transChar,
						      lineBreak, pDoc, recordLineNb);
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

/*----------------------------------------------------------------------
   ApplyPresTRules applique a l'element pointe' par pEl les regles de	
   traduction associees aux presentations portees par l'element.   
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ApplyPresTRules (TOrder position, PtrElement pEl, ThotBool * removeEl,
				   ThotBool * transChar, ThotBool * lineBreak,
				     PtrAttribute pAttr, PtrDocument pDoc, ThotBool recordLineNb)

#else  /* __STDC__ */
static void         ApplyPresTRules (position, pEl, removeEl, transChar, lineBreak, pAttr,
				     pDoc, recordLineNb)
TOrder              position;
PtrElement          pEl;
ThotBool           *removeEl;
ThotBool           *transChar;
ThotBool           *lineBreak;
PtrAttribute        pAttr;
PtrDocument         pDoc;
ThotBool            recordLineNb;

#endif /* __STDC__ */

{
   PtrPRule            pPRule;
   PtrTSchema          pTSch;
   PtrTRule            pTRule;
   PRuleTransl        *pPRuleTr;
   PtrTRuleBlock       pBlock;
   TranslNumAttrCase  *pTCase;
   int                 i, nPRules = 0;
   CHAR_T                val;

#define MAX_PRULE_TABLE 50
   PtrPRule            PRuleTable[MAX_PRULE_TABLE];

   pTSch = GetTranslationSchema (pEl->ElStructSchema);
   if (pTSch == NULL)
      return;
   /* 1ere regle de presentation specifique de l'element */
   pPRule = pEl->ElFirstPRule;
   /* Si on applique les regles "After", on commence par la derniere regle */
   /* et on traitera les regles dans l'ordre inverse */
   if (position == TAfter && pPRule != NULL)
     {
	nPRules = 0;
	while (pPRule->PrNextPRule != NULL && nPRules < MAX_PRULE_TABLE)
	  {
	     PRuleTable[nPRules++] = pPRule;
	     pPRule = pPRule->PrNextPRule;
	  }
     }

   /* parcourt les regles de presentation specifique de l'element */
   while (pPRule != NULL)
     {
	pPRuleTr = &pTSch->TsPresTRule[pPRule->PrType];
	if (pPRuleTr->RtExist)
	   /* il y a des regles de traduction pour cette presentation */
	  {
	     /* cherche le premier bloc de regles correspondant a ce */
	     /* type de regle de presentation */
	     pBlock = NULL;
	     if (pTSch != NULL)
		if (pPRule->PrType == PtSize || pPRule->PrType == PtIndent ||
		    pPRule->PrType == PtLineSpacing ||
		    pPRule->PrType == PtLineWeight)
		  {
		     i = 0;
		     while (pBlock == NULL && i < pPRuleTr->RtNCase)
		       {
			  pTCase = &pPRuleTr->RtCase[i++];
			  if (pPRule->PrMinValue <= pTCase->TaUpperBound &&
			      pPRule->PrMinValue >= pTCase->TaLowerBound)
			     pBlock = pTCase->TaTRuleBlock;
		       }
		  }
		else if (pPRule->PrType == PtFillPattern ||
			 pPRule->PrType == PtBackground ||
			 pPRule->PrType == PtForeground)
		  {
		     i = 0;
		     while (pBlock == NULL && i < pPRuleTr->RtNCase)
		       {
			  pTCase = &pPRuleTr->RtCase[i++];
			  if (pPRule->PrIntValue <= pTCase->TaUpperBound &&
			      pPRule->PrIntValue >= pTCase->TaLowerBound)
			     pBlock = pTCase->TaTRuleBlock;
		       }
		  }
		else
		  {
		     /* cherche si cette valeur de la presentation a un */
		     /* bloc de regles */
		     /* Calcule d'abord la valeur caractere de la presentation */
		     val = PresRuleValue (pPRule);
		     pBlock = NULL;
		     i = 1;
		     while (pBlock == NULL && pPRuleTr->RtPRuleValue[i] != EOS &&
			    i <= MAX_TRANSL_PRES_VAL + 1)
		       {
			  if (pPRuleTr->RtPRuleValue[i] == val)
			     pBlock = pPRuleTr->RtPRuleValueBlock[i];
			  i++;
		       }
		     if (pBlock == NULL)
			/* pas de regles de traduction pour cette valeur, on */
			/* prend les regles qui s'appliquent a toute valeur */
			pBlock = pPRuleTr->RtPRuleValueBlock[0];
		  }
	     /* parcourt les blocs de regles de la valeur de la presentation */
	     while (pBlock != NULL)
	       {
		  if (ConditionIsTrue (pBlock, pEl, NULL, pDoc))
		     /* la condition du bloc est verifiee */
		    {
		       pTRule = pBlock->TbFirstTRule;	/* premiere regle du bloc */
		       /* parcourt les regles du bloc */
		       while (pTRule != NULL)
			 {
			    if (pTRule->TrOrder == position)
			       /* c'est une regle a appliquer a cette position */
			       if (pTRule->TrType == TRemove)
				  *removeEl = TRUE;
			       else if (pTRule->TrType == TNoTranslation)
				  *transChar = FALSE;
			       else if (pTRule->TrType == TNoLineBreak)
				  *lineBreak = FALSE;
			       else
				  /* on applique la regle */
				  ApplyTRule (pTRule, pTSch,
					      pEl->ElStructSchema, pEl,
					      transChar, lineBreak, removeEl,
					      pPRule, pAttr, pDoc, recordLineNb);
			    /* passe a la regle suivante */
			    pTRule = pTRule->TrNextTRule;
			 }
		    }
		  /* passe au bloc suivant */
		  pBlock = pBlock->TbNextBlock;
	       }
	  }

	if (position == TAfter)
	   /* passe a la regle de presentation precedente de l'element */
	  {
	     if (nPRules > 0)
	       {
		  nPRules--;
		  pPRule = PRuleTable[nPRules];
	       }
	     else
		pPRule = NULL;
	  }
	else
	   /* passe a la regle de presentation suivante de l'element */
	   pPRule = pPRule->PrNextPRule;
     }
}


/*----------------------------------------------------------------------
   PutVariable                               				
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         PutVariable (PtrElement pEl, PtrAttribute pAttr,
			       PtrTSchema pTSch, PtrSSchema pSS, int varNum,
		ThotBool ref, STRING outBuffer, int fileNum, PtrDocument pDoc,
				 ThotBool lineBreak)

#else  /* __STDC__ */

static void         PutVariable (pEl, pAttr, pTSch, pSS, varNum, ref, outBuffer, fileNum, pDoc, lineBreak)
PtrElement          pEl;
PtrAttribute        pAttr;
PtrTSchema          pTSch;
PtrSSchema          pSS;
int                 varNum;
ThotBool            ref;
STRING              outBuffer;
int                 fileNum;
PtrDocument         pDoc;
ThotBool            lineBreak;

#endif /* __STDC__ */
{
   TranslVariable     *varTrans;
   TranslVarItem      *varItem;
   PtrElement          pRefEl, pAncest;
   PtrReference        pRef;
   TtAttribute        *attrTrans;
   PtrAttribute        pA;
   DocumentIdentifier  docIdent;
   PtrDocument         pExtDoc;
   PtrTextBuffer       pBuf;
   int                 item, i, j, k;
   ThotBool            found;
   CHAR_T              number[20];

   pA = NULL;
   if (outBuffer != NULL)
      /* on vide le buffer avant de commencer a le remplir */
      outBuffer[0] = EOS;
   varTrans = &pTSch->TsVariable[varNum - 1];
   /* parcourt les items qui constituent la variable */
   for (item = 0; item < varTrans->TrvNItems; item++)
     {
	varItem = &varTrans->TrvItem[item];
	/* traite selon le type de l'element de variable */
	switch (varItem->TvType)
	      {
		 case VtText:
		    /* une constante de texte */
		    i = pTSch->TsConstBegin[varItem->TvItem - 1];
		    while (pTSch->TsConstant[i - 1] != EOS)
		      {
			 PutChar (pTSch->TsConstant[i - 1], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
			 i++;
		      }
		    break;
		 case VtCounter:
		    /* valeur d'un compteur */
		    /* si la regle porte sur une reference, on */
		    /* prend la valeur du compteur pour l'element */
		    /* designe'par la reference si TrReferredObj est vrai */
		    pRef = NULL;
		    if (ref)
		       if (pAttr != NULL &&
			   pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
			  /* c'est un attribut reference */
			  pRef = pAttr->AeAttrReference;
		       else
			  /* l'element est-il une reference ? */
		       if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
			  pRef = pEl->ElReference;
		       else
			  /* c'est peut-etre une inclusion */
			  pRef = pEl->ElSource;
		    if (pRef != NULL)
		      {
			 pRefEl = ReferredElement (pRef, &docIdent, &pExtDoc);
			 if (pRefEl == NULL)
			    /* la reference ne designe rien */
			    i = 0;
			 else
			    /* valeur du compteur pour l'element */
			    /* designe' par la reference */
			    i = CounterVal (varItem->TvItem, pTSch, pSS, pRefEl);
		      }
		    else
		       /* ce n'est pas une reference */
		       /* valeur du compteur pour l'element meme */
		       i = CounterVal (varItem->TvItem, pTSch, pSS, pEl);
		    /* produit quelques 0 si c'est demande' */
		    if (varItem->TvCounterStyle == CntArabic && varItem->TvLength > 0)
		      {
			 j = 1;
			 for (k = 0; k < varItem->TvLength - 1; k++)
			   {
			      j = j * 10;
			      if (j > i)
				 PutChar (TEXT('0'), fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
			   }
		      }
		    /* convertit la valeur du compteur dans le style demande' */
		    GetCounterValue (i, varItem->TvCounterStyle, number, &j);
		    /* sort la valeur du compteur */
		    for (k = 0; k < j; k++)
		       PutChar (number[k], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
		    break;
		 case VtBuffer:
		    /* le contenu d'un buffer */
		    i = 0;
		    while (pTSch->TsBuffer[varItem->TvItem - 1][i] != EOS)
		      {
			 PutChar (pTSch->TsBuffer[varItem->TvItem - 1][i], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
			 i++;
		      }
		    break;
		 case VtAttrVal:	/* la valeur d'un attribut */
		    /* cherche si l'element traduit ou l'un de ses */
		    /* ascendants possede cet attribut */
		    found = FALSE;
		    pAncest = pEl;
		    while (!found && pAncest != NULL)
		      {
			 pA = pAncest->ElFirstAttr;	/* premier attribut */
			 while (!found && pA != NULL)
			    if (pA->AeAttrNum == varItem->TvItem
				&& pA->AeAttrSSchema->SsCode == pSS->SsCode)
			       found = TRUE;
			    else
			       pA = pA->AeNext;
			 if (!found)
			    pAncest = pAncest->ElParent;
		      }
		    if (found)
		       /* l'element possede l'attribut */
		      {
			 switch (pA->AeAttrType)
			       {
				  case AtNumAttr:
				     PutInt (pA->AeAttrValue, fileNum, outBuffer, pDoc,
					     lineBreak);
				     break;
				  case AtTextAttr:
				     pBuf = pA->AeAttrText;
				     while (pBuf != NULL)
				       {
					  i = 0;
					  while (i < pBuf->BuLength)
					     PutChar (pBuf->BuContent[i++], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
					  pBuf = pBuf->BuNext;
				       }
				     break;
				  case AtReferenceAttr:
				     PutChar (TEXT('R'), fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
				     PutChar (TEXT('E'), fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
				     PutChar (TEXT('F'), fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
				     break;
				  case AtEnumAttr:
				     i = 0;
				     attrTrans = &pA->AeAttrSSchema->SsAttribute[varItem->TvItem - 1];
				     while (attrTrans->AttrEnumValue[pA->AeAttrValue - 1][i] != EOS)
					PutChar (attrTrans->AttrEnumValue[pA->AeAttrValue - 1][i++], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
				     break;
			       }
		      }
		    break;

		 case VtFileDir:	/* le nom du directory de sortie */
		    i = 0;
		    while (fileDirectory[i] != WC_EOS)
		       PutChar (fileDirectory[i++], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
		    break;

		 case VtFileName:	/* le nom du fichier de sortie */
		    i = 0;
		    while (fileName[i] != WC_EOS)
		       PutChar (fileName[i++], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
		    break;

		 case VtExtension:	/* le nom de l'extension de fichier */
		    i = 0;
		    while (fileExtension[i] != WC_EOS)
		       PutChar (fileExtension[i++], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
		    break;
		 case VtDocumentName:	/* le nom du document */
		    i = 0;
		    while (pDoc->DocDName[i] != WC_EOS)
		       PutChar (pDoc->DocDName[i++], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
		    break;
		 case VtDocumentDir:	/* le repertoire du document */
		    i = 0;
		    while (pDoc->DocDirectory[i] != WC_EOS)
		       PutChar (pDoc->DocDirectory[i++], fileNum, outBuffer, pDoc, lineBreak, CharEncoding);
		    break;

		 default:
		    break;
	      }
     }
}

#ifdef __STDC__
static void         TranslateTree (PtrElement pEl, PtrDocument pDoc,
				   ThotBool transChar, ThotBool lineBreak,
				   ThotBool enforce, ThotBool recordLineNb);

#else  /* __STDC__ */
static void         TranslateTree ( /* pEl, pDoc, transChar, lineBreak, enforce, recordLineNb */ );

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   ApplyTRule   applique la regle de traduction pTRule du schema de	
   traduction pTSch (qui correspond au schema de structure pointe' 
   par pSSch) a l'element pointe par pEl, en demandant la          
   traduction des caracteres contenus si transChar est vrai.       
   S'il s'agit de la traduction d'une presentation, pRPres pointe  
   sur la regle de presentation specifique traduite.               
   S'il s'agit de la traduction des regles d'un attribut,          
   pAttr pointe sur l'attribut que l'on traduit.			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ApplyTRule (PtrTRule pTRule, PtrTSchema pTSch,
			PtrSSchema pSSch, PtrElement pEl, ThotBool * transChar,
				ThotBool * lineBreak, ThotBool * removeEl,
				PtrPRule pRPres, PtrAttribute pAttr,
				PtrDocument pDoc, ThotBool recordLineNb)
#else  /* __STDC__ */
static void         ApplyTRule (pTRule, pTSch, pSSch, pEl, transChar, lineBreak,
				removeEl, pRPres, pAttr, pDoc, recordLineNb)
PtrTRule            pTRule;
PtrTSchema          pTSch;
PtrSSchema          pSSch;
PtrElement          pEl;
ThotBool           *transChar;
ThotBool           *lineBreak;
ThotBool           *removeEl;
PtrPRule            pRPres;
PtrAttribute        pAttr;
PtrDocument         pDoc;
ThotBool            recordLineNb;
#endif /* __STDC__ */

{
   PtrElement          pElGet, pRefEl;
   PtrDocument         pDocGet;
   PtrSSchema          pSS;
   Name                n;
   DocumentIdentifier  docIdent;
   PtrDocument         pExtDoc;
   PtrAttribute        pA = NULL;
   PtrTextBuffer       pBuf;
   TtAttribute        *attrTrans;
   BinFile             includedFile;
   PtrReference        pRef;
   PtrTSchema          pTransTextSch;
   AlphabetTransl      *pTransAlph;
   int                 fileNum;
   int                 i;
   CHAR_T                secondaryFileName[MAX_PATH];
   STRING              nameBuffer;
   CHAR_T                fname[MAX_PATH];
   CHAR_T                fullName[MAX_PATH];	/* nom d'un fichier a inclure */
   PathBuffer          directoryName;
   FILE               *newFile;
   CHAR_T                currentFileName[MAX_PATH];	/* nom du fichier principal */
   ThotBool            found, possibleRef;
   CHAR_T                c;
#  ifndef _WINDOWS 
   char		       cmd[MAX_PATH];
   char                        fileNameStr[MAX_PATH];
#  endif /* _WINDOWS */
#  ifdef _I18N_
   int                 nb_bytes2write, index;
   unsigned char       mbc[MAX_BYTES + 1];
#  endif /* _I18N_ */

   n[0] = EOS;
   /* on applique la regle selon son type */
   switch (pTRule->TrType)
	 {
	    case TCreate:
	    case TWrite:
	       /* regle d'ecriture dans un fichier de sortie ou au terminal */
	       if (pTRule->TrType == TCreate)
		  if (pTRule->TrFileNameVar == 0)
		     /* sortie sur le fichier principal courant */
		     fileNum = 1;
		  else
		     /* sortie sur un fichier secondaire */
		    {
		       /* construit le nom du fichier secondaire */
		       PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrFileNameVar,
			      FALSE, secondaryFileName, 0, pDoc, *lineBreak);
		       fileNum = GetSecondaryFile (secondaryFileName, pDoc, TRUE);
		    }
	       else		/* TWrite */
		  fileNum = 0;	/* on ecrit sur stdout */
	       /* traitement selon le type d'objet a ecrire */
	       switch (pTRule->TrObject)
		     {
			case ToConst:
			   /* ecriture d'une constante */
			   i = pTSch->TsConstBegin[pTRule->TrObjectNum - 1];
			   while (pTSch->TsConstant[i - 1] != WC_EOS)
			     {
				PutChar (pTSch->TsConstant[i - 1], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
				i++;
			     }
			   break;
			case ToBuffer:
			   /* ecriture du contenu d'un buffer */
			   i = 0;
			   while (pTSch->TsBuffer[pTRule->TrObjectNum - 1][i] != WC_EOS)
			      PutChar (pTSch->TsBuffer[pTRule->TrObjectNum - 1][i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
			   break;
			case ToVariable:	/* creation d'une variable */
			   PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrObjectNum, pTRule->TrReferredObj, NULL, fileNum, pDoc, *lineBreak);
			   break;

			case ToAttr:
			case ToTranslatedAttr:
			   /* cherche si l'element ou un de ses ascendants
			      possede l'attribut a sortir */
			   found = FALSE;
			   while (pEl != NULL && !found)
			     {
				pA = pEl->ElFirstAttr;	/* 1er attribut de l'element */
				/* parcourt les attributs de l'element */
				while (pA != NULL && !found)
				   if (pA->AeAttrSSchema->SsCode == pSSch->SsCode &&
				       pA->AeAttrNum == pTRule->TrObjectNum)
				      found = TRUE;
				   else
				      pA = pA->AeNext;
				if (!found)
				   pEl = pEl->ElParent;		/* passe a l'element ascendant */
			     }
			   /* si on a trouve' l'attribut, on sort sa valeur */
			   if (found)
			      switch (pA->AeAttrType)
				 {
				 case AtNumAttr:
				    /* ecrit la valeur numerique de l'attribut */
				    PutInt (pA->AeAttrValue, fileNum, NULL, pDoc, *lineBreak);
				    break;
				 case AtTextAttr:
				    /* ecrit la valeur de l'attribut */
                                    pTransAlph = NULL;
                                    pTransTextSch = NULL;
                                    if (pTRule->TrObject == ToTranslatedAttr)
                                        pTransTextSch = GetTransSchForContent(pEl,
                                                         LtText, &pTransAlph);
				    pBuf = pA->AeAttrText;
				    if (pBuf)
                                      if (!pTransTextSch || !pTransAlph)
                                        /* no translation */
				        while (pBuf != NULL)
				          {
				          i = 0;
				          while (i < pBuf->BuLength)
					     PutChar (pBuf->BuContent[i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
				          pBuf = pBuf->BuNext;
				          }
                                      else
                                        /* translate the attribute value */
                                        TranslateText (pBuf, pTransTextSch,
                                        pTransAlph, *lineBreak, fileNum, pDoc);
				    break;
				 case AtReferenceAttr:
				    /* cas non traite' */
				    break;
				 case AtEnumAttr:
				    /* ecrit le nom de la valeur de l'attribut */
				    attrTrans = &pA->AeAttrSSchema->SsAttribute[pA->AeAttrNum - 1];
				    i = 0;
				    while (attrTrans->AttrEnumValue[pA->AeAttrValue - 1][i] != WC_EOS)
				       PutChar (attrTrans->AttrEnumValue[pA->AeAttrValue - 1][i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
				    break;
				 default:
				    break;
				 }

			   break;
			case ToContent:
			   /* produit le contenu des feuilles de l'element */
			   PutContent (pEl, *transChar, *lineBreak, fileNum, pDoc);
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
					  case PtWeight:
					  case PtUnderline:
					  case PtThickness:
					  case PtLineStyle:
					     PutChar (pRPres->PrChrValue, fileNum, NULL, pDoc, *lineBreak, CharEncoding);
					     break;
					  case PtIndent:
					  case PtSize:
					  case PtLineSpacing:
					  case PtLineWeight:
					     PutInt (pRPres->PrMinValue, fileNum, NULL, pDoc, *lineBreak);
					     break;
					  case PtFillPattern:
					     PutPattern (pRPres->PrIntValue, fileNum, pDoc, *lineBreak);
					     break;
					  case PtBackground:
					  case PtForeground:
					     PutColor (pRPres->PrIntValue, fileNum, pDoc, *lineBreak);
					     break;
					  case PtJustify:
					  case PtHyphenate:
					     if (pRPres->PrJustify)
						PutChar (TEXT('Y'), fileNum, NULL, pDoc, *lineBreak, CharEncoding);
					     else
						PutChar (TEXT('N'), fileNum, NULL, pDoc, *lineBreak, CharEncoding);
					     break;
					  case PtAdjust:
					     switch (pRPres->PrAdjust)
						   {
						      case AlignLeft:
							 PutChar (TEXT('L'), fileNum, NULL, pDoc, *lineBreak, CharEncoding);
							 break;
						      case AlignRight:
							 PutChar (TEXT('R'), fileNum, NULL, pDoc, *lineBreak, CharEncoding);
							 break;
						      case AlignCenter:
							 PutChar (TEXT('C'), fileNum, NULL, pDoc, *lineBreak, CharEncoding);
							 break;
						      case AlignLeftDots:
							 PutChar (TEXT('D'), fileNum, NULL, pDoc, *lineBreak, CharEncoding);
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
				i = 0;
				while (i < pBuf->BuLength)
				   PutChar (pBuf->BuContent[i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
				pBuf = pBuf->BuNext;
			     }
			   break;
			case ToAllAttr:
			   /* produit la traduction de tous les attributs de l'element */
			   ApplyAttrRules (pTRule->TrOrder, pEl, removeEl, transChar, lineBreak,
					   pDoc, recordLineNb);
			   /* les regles des attributs ont ete appliquees */
			   pEl->ElTransAttr = TRUE;
			   break;
			case ToAllPRules:
			   /* produit la traduction de toutes les regles de presentation */
			   /* specifique portees par l'element */
			   ApplyPresTRules (pTRule->TrOrder, pEl, removeEl,
					    transChar, lineBreak,
					    pAttr, pDoc, recordLineNb);
			   /* marque dans l'element que sa presentation a ete traduite */
			   pEl->ElTransPres = TRUE;
			   break;

			case ToPairId:
			   /* traduit l'identificateur d'une paire */
			   if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsPairedElement)
			      /* l'element est bien une paire */
			      PutInt (pEl->ElPairIdent, fileNum, NULL, pDoc, *lineBreak);
			   break;

			case ToFileDir:
			   i = 0;
			   while (fileDirectory[i] != WC_EOS)
			      PutChar (fileDirectory[i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
			   break;

			case ToFileName:
			   i = 0;
			   while (fileName[i] != WC_EOS)
			      PutChar (fileName[i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
			   break;

			case ToExtension:
			   i = 0;
			   while (fileExtension[i] != WC_EOS)
			      PutChar (fileExtension[i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
			   break;

			case ToDocumentName:
			   i = 0;
			   while (pDoc->DocDName[i] != WC_EOS)
			      PutChar (pDoc->DocDName[i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
			   break;

			case ToDocumentDir:
			   i = 0;
			   while (pDoc->DocDirectory[i] != WC_EOS)
			      PutChar (pDoc->DocDirectory[i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
			   break;

			case ToReferredDocumentName:
			case ToReferredDocumentDir:
			   pRef = NULL;
			   if (pAttr != NULL &&
			       pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
			      /* c'est un attribut reference qu'on traduit */
			      pRef = pAttr->AeAttrReference;
			   else
			      /* l'element est-il une reference ? */
			   if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
			      pRef = pEl->ElReference;
			   else
			      /* c'est peut-etre une inclusion */
			      pRef = pEl->ElSource;
			   if (pRef != NULL)
			     {
				pRefEl = ReferredElement (pRef, &docIdent, &pExtDoc);
				nameBuffer = NULL;
				if (pTRule->TrObject == ToReferredDocumentName)
				  {
				     if (pRefEl != NULL && docIdent[0] == EOS)
					/* reference interne. On sort le nom du document lui-meme */
					nameBuffer = pDoc->DocDName;
				     else if (docIdent[0] != EOS)
					/* on sort le nom du document reference' */
					nameBuffer = docIdent;
				  }
				else if (pTRule->TrObject == ToReferredDocumentDir)
				   if (pRefEl != NULL && docIdent[0] == EOS)
				      /* reference interne. On sort le directory du document lui-meme */
				      nameBuffer = pDoc->DocDirectory;
				   else if (docIdent[0] != EOS)
				     {
					/* on sort le directory du document reference' */
					if (pExtDoc != NULL)
					   /* le document reference' est charge' */
					   nameBuffer = pExtDoc->DocDirectory;
					else
					   /* le document reference' n'est pas charge' */
					  {
					     ustrncpy (directoryName, DocumentPath, MAX_PATH);
					     MakeCompleteName (docIdent, TEXT("PIV"), directoryName, fullName, &i);
					     if (fullName[0] != EOS)
						/* on a trouve' le fichier */
						nameBuffer = directoryName;
					  }
				     }
				if (nameBuffer != NULL)
				   while (*nameBuffer != WC_EOS)
				     {
					PutChar (*nameBuffer, fileNum, NULL, pDoc, *lineBreak, CharEncoding);
					nameBuffer++;
				     }
			     }
			   break;

			case ToReferredElem:
			   /* traduit l'elment reference' de type pTRule->TrObjectNum */
			   pRef = NULL;
			   if (pAttr != NULL &&
			       pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
			      /* c'est un attribut reference qu'on traduit */
			      pRef = pAttr->AeAttrReference;
			   else
			      /* l'element est-il une reference ? */
			   if (pEl->ElTerminal && pEl->ElLeafType == LtReference)
			      pRef = pEl->ElReference;
			   else
			      /* c'est peut-etre une inclusion */
			      pRef = pEl->ElSource;
			   if (pRef != NULL)
			     {
				pRefEl = ReferredElement (pRef, &docIdent, &pExtDoc);
				if (pRefEl != NULL)
				   /* la reference designe l'element pRefEl */
				   /* On le prend s'il a le type voulu */
				  {
				     if (pTRule->TrObjectNature[0] == EOS)
					pSS = pEl->ElStructSchema;
				     else
					pSS = NULL;
				     if (!((pSS != NULL &&
					    EquivalentSRules (pTRule->TrObjectNum, pSS, pRefEl->ElTypeNumber, pRefEl->ElStructSchema, pRefEl->ElParent)
					   )
					   || (pSS == NULL &&
					       ustrcmp (pTRule->TrObjectNature, pRefEl->ElStructSchema->SsName) == 0
					       && EquivalentSRules (pTRule->TrObjectNum, pRefEl->ElStructSchema,
								    pRefEl->ElTypeNumber, pRefEl->ElStructSchema, pRefEl->ElParent)
					   )
					 )
					)
					/* Il n'a pas le type voulu, on cherche dans */
					/* le sous arbre de l'element designe' */
					SearchDescent (&pRefEl, pTRule->TrObjectNum, pSS, pTRule->TrObjectNature);
				  }
				if (pRefEl != NULL)
				   /* traduit l'element reference', meme s'il a deja ete traduit */
				   if (docIdent[0] == EOS)
				      /* reference interne */
				      TranslateTree (pRefEl, pDoc, *transChar, *lineBreak, TRUE, recordLineNb);
				   else if (pExtDoc != NULL)
				      /* reference externe a un document charge' */
				      TranslateTree (pRefEl, pExtDoc, *transChar, *lineBreak, TRUE, recordLineNb);
			     }
			   break;

			case ToRefId:
			case ToReferredRefId:
			   pElGet = NULL;
			   if (pTRule->TrObject == ToReferredRefId)
			      /* il faut traduire le label de l'element reference' */
			     {
				pRef = NULL;
				/* si on traduit un attribut reference, on ne s'occupe que de */
				/* l'attribut */
				if (pAttr != NULL
				    && pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
				   /* c'est un attribut reference */
				   pRef = pAttr->AeAttrReference;
				/* sinon on s'occupe de l'element */
				else
				  {
				     /* l'element est-il une reference ? */
				     if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsReference)
					pRef = pEl->ElReference;
				     /* ou est-il defini comme identique a une reference */
				     else if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsIdentity)
				       {
					  i = pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrIdentRule;
					  if (pEl->ElStructSchema->SsRule[i - 1].SrConstruct == CsBasicElement
					      && pEl->ElStructSchema->SsRule[i - 1].SrBasicType == CsReference)
					     pRef = pEl->ElReference;
				       }
				  }
				if (pRef == NULL)
				   /* c'est peut-etre une inclusion */
				   pRef = pEl->ElSource;
				if (pRef != NULL)
				  {
				     pElGet = ReferredElement (pRef, &docIdent, &pExtDoc);
				     if (pElGet == NULL && docIdent[0] != EOS)
					/* reference a un document externe non charge' */
					if (pRef != NULL)
					   if (pRef->RdReferred != NULL)
					      if (pRef->RdReferred->ReExternalRef)
						{

						   i = 0;
						   while (pRef->RdReferred->ReReferredLabel[i] != EOS)
						      PutChar (pRef->RdReferred->ReReferredLabel[i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
						}
				  }
			     }

			   if (pTRule->TrObject == ToRefId)
			     {
				/* on cherche si l'element (ou le premier de ses ascendants sur */
				/* lequel porte une reference) est reference' et on recupere la */
				/* reference. */
				pElGet = pEl;
				do
				  {
				     pSS = pElGet->ElStructSchema;
				     possibleRef = FALSE;
				     /* l'element est-il reference'? */
				     if (pElGet->ElReferredDescr != NULL)
					possibleRef = pElGet->ElReferredDescr->ReFirstReference != NULL;
				     if (!possibleRef)
				       {
					  /* l'element peut-il etre designe' par un element reference? */
					  /* on cherche tous les elements references dans le schema de */
					  /* structure de l'element */
					  i = 1;
					  do
					    {
					       if (pSS->SsRule[i].SrConstruct == CsReference)
						  /* c'est une reference */
						  if (pSS->SsRule[i].SrReferredType != 0)
						     possibleRef = EquivalentSRules (pSS->SsRule[i].SrReferredType, pSS, pElGet->ElTypeNumber, pSS, pElGet->ElParent);
					       i++;
					    }
					  while (!possibleRef &&
						 i < pSS->SsNRules);
				       }
				     if (!possibleRef)
				       {
					  /* l'element ne peut pas etre designe par un elem. reference */
					  /* on cherche s'il peut etre designe' par un attr. reference */
					  /* on cherche tous les attributs reference dans le schema de */
					  /* structure de l'element */
					  i = 1;
					  do
					    {
					       if (pSS->SsAttribute[i].AttrType == AtReferenceAttr)
						  /* c'est une reference */
						  if (pSS->SsAttribute[i].AttrTypeRef != 0)
						     possibleRef = (pSS->SsAttribute[i].AttrTypeRefNature[0] == EOS
								    && EquivalentSRules (pSS->SsAttribute[i].AttrTypeRef, pSS, pElGet->ElTypeNumber, pSS, pElGet->ElParent));
					       i++;
					    }
					  while (!possibleRef &&
						 i < pSS->SsNAttributes);
				       }
				     if (!possibleRef)
					/* l'element ne peut pas etre designe'; on examine */
					/* l'element ascendant */
					pElGet = pElGet->ElParent;
				  }
				while (!possibleRef && pElGet != NULL);
			     }

			   if (pElGet != NULL)
/** if (pElGet->ElReferredDescr != NULL) **/
			     {
				i = 0;
				while (pElGet->ElLabel[i] != WC_EOS)
				   PutChar (pElGet->ElLabel[i++], fileNum, NULL, pDoc, *lineBreak, CharEncoding);
			     }
			   break;

			default:
			   break;
		     }
	       break;

	    case TChangeMainFile:
	       PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrNewFileVar, FALSE, currentFileName, 0, pDoc, *lineBreak);
	       if (currentFileName[0] != WC_EOS)
		 {
		    newFile = ufopen (currentFileName, TEXT("w"));
		    if (newFile == NULL)
		       TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_CREATE_FILE_IMP), currentFileName);
		    else
		       /* on a reussi a ouvrir le nouveau fichier */
		      {
			 /* on vide le buffer en cours dans l'ancien fichier */
				for (i = 0; i < OutputFile[1].OfBufferLen; i++) {
#                   ifdef _I18N_
                    nb_bytes2write = TtaWC2MB (OutputFile[1].OfBuffer[i], mbc, CharEncoding);
                    for (index = 0; index < nb_bytes2write; index++)
                        putc (mbc[index], OutputFile[1].OfFileDesc);
#                   else  /* !_I18N_ */
                    putc (OutputFile[1].OfBuffer[i], OutputFile[1].OfFileDesc);
#                   endif /* !_I18N_ */
				}
			 /* on ferme l'ancien fichier */
			 fclose (OutputFile[1].OfFileDesc);
			 /* on bascule sur le nouveau fichier */
			 OutputFile[1].OfBufferLen = 0;
			 OutputFile[1].OfIndent = 0;
			 OutputFile[1].OfPreviousIndent = 0;
			 OutputFile[1].OfLineNumber = 0;
			 OutputFile[1].OfStartOfLine = TRUE;
			 OutputFile[1].OfFileDesc = newFile;
			 OutputFile[1].OfCannotOpen = FALSE;
		      }
		 }
	       break;

	    case TRemoveFile:
	       PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrNewFileVar,
			    FALSE, currentFileName, 0, pDoc, *lineBreak);
	       if (currentFileName[0] != WC_EOS)
		 {
#           ifdef _WINDOWS
            uunlink (currentFileName);
#           else  /* !_WINDOWS */
            wc2iso_strcpy (fileNameStr, currentFileName);
	    sprintf (cmd, "/bin/rm %s\n", fileNameStr);
	    system (cmd);
#           endif /* _WINDOWS */
		 }
	       break;

	    case TSetCounter:
	       pTSch->TsCounter[pTRule->TrCounterNum - 1].TnParam1 = pTRule->TrCounterParam;
	       break;

	    case TAddCounter:
	       pTSch->TsCounter[pTRule->TrCounterNum - 1].TnParam1 += pTRule->TrCounterParam;
	       break;

	    case TIndent:
	       if (pTRule->TrIndentFileNameVar == 0)
		  /* sortie sur le fichier principal courant */
		  fileNum = 1;
	       else
		  /* sortie sur un fichier secondaire */
		 {
		    /* construit le nom du fichier secondaire */
		    PutVariable (pEl, pAttr, pTSch, pSSch, pTRule->TrIndentFileNameVar,
			         FALSE, secondaryFileName, 0, pDoc, *lineBreak);
		    fileNum = GetSecondaryFile (secondaryFileName, pDoc, TRUE);
		 }
	       if (fileNum >= 0)
		  {
		  switch (pTRule->TrIndentType)
		     {
		     case ItRelative:
		         OutputFile[fileNum].OfIndent += pTRule->TrIndentVal;
			 break;
	             case ItAbsolute:
		         OutputFile[fileNum].OfIndent = pTRule->TrIndentVal;
			 break;
		     case ItSuspend:
		         OutputFile[fileNum].OfPreviousIndent =
						OutputFile[fileNum].OfIndent;
			 OutputFile[fileNum].OfIndent = 0;
			 break;
		     case ItResume:
			 OutputFile[fileNum].OfIndent = 
					OutputFile[fileNum].OfPreviousIndent;
			 break;
		     }
		  if (OutputFile[fileNum].OfIndent < 0)
		     OutputFile[fileNum].OfIndent = 0;
		  }
	       break;

	    case TGet:
	    case TCopy:
	       /* on traduit l'element indique' dans la regle Get */
	       /* cherche d'abord l'element a prendre */
	       pElGet = pEl;
	       pDocGet = pDoc;
	       switch (pTRule->TrRelPosition)
		     {
			case RpSibling:
			   /* Cherche un frere ayant le type voulu */
			   /* cherche d'abord le frere aine' */
			   while (pElGet->ElPrevious != NULL)
			      pElGet = pElGet->ElPrevious;
			   /* cherche ensuite parmi les freres successifs */
			   found = FALSE;
			   do
			      if ((pElGet->ElStructSchema->SsCode == pEl->ElStructSchema->SsCode ||
				   (ustrcmp (pTRule->TrElemNature, pElGet->ElStructSchema->SsName) == 0))
				  && EquivalentSRules (pTRule->TrElemType, pElGet->ElStructSchema,
						       pElGet->ElTypeNumber, pElGet->ElStructSchema, pElGet->ElParent))
				 found = TRUE;
			      else
				 pElGet = pElGet->ElNext;
			   while (!found && pElGet != NULL);
			   break;
			case RpDescend:
			   /* Cherche dans le sous-arbre un element ayant le type voulu. */
			   if (pTRule->TrElemNature[0] == EOS)
			      pSS = pEl->ElStructSchema;
			   else
			      pSS = NULL;
			   SearchDescent (&pElGet, pTRule->TrElemType, pSS, pTRule->TrElemNature);
			   break;
			case RpReferred:
			   /* Cherche dans le sous-arbre de l'element designe', un element
			      ayant le type voulu. */
			   /* cherche d'abord l'element designe' */
			   pRef = NULL;
			   if (pAttr != NULL &&
			       pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
			      /* c'est un attribut reference qu'on traduit */
			      pRef = pAttr->AeAttrReference;
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
			      pElGet = ReferredElement (pEl->ElReference, &docIdent, &pExtDoc);
			   if (pElGet != NULL)
			      /* il y a bien un element designe'. On le prend s'il */
			      /* a le type voulu */
			     {
				if (pTRule->TrElemNature[0] == EOS)
				   pSS = pEl->ElStructSchema;
				else
				   pSS = NULL;
				if (!((pSS != NULL &&
				       EquivalentSRules (pTRule->TrElemType, pSS, pElGet->ElTypeNumber, pElGet->ElStructSchema, pElGet->ElParent)
				      )
				      || (pSS == NULL &&
					  ustrcmp (pTRule->TrElemNature, pElGet->ElStructSchema->SsName) == 0
					  && EquivalentSRules (pTRule->TrElemType, pElGet->ElStructSchema,
							       pElGet->ElTypeNumber, pElGet->ElStructSchema, pElGet->ElParent)
				      )
				    )
				   )
				   /* Il n'a pas le type voulu, on cherche dans */
				   /* le sous arbre de l'element designe' */
				   SearchDescent (&pElGet, pTRule->TrElemType, pSS, pTRule->TrElemNature);
				if (docIdent[0] != EOS && pExtDoc != NULL)
				   /* reference externe a un document charge' */
				   pDocGet = pExtDoc;
			     }
			   break;
			case RpAssoc:
			   /* on prend les elements associes du type indique' */
			   i = 0;
			   pElGet = NULL;
			   do
			     {
				if (pDoc->DocAssocRoot[i] != NULL)
				   if (pDoc->DocAssocRoot[i]->ElTypeNumber == pTRule->TrElemType)
				      pElGet = pDoc->DocAssocRoot[i];
				   else if (pDoc->DocAssocRoot[i]->ElFirstChild != NULL)
				      if (pDoc->DocAssocRoot[i]->ElFirstChild->ElTypeNumber == pTRule->TrElemType)
					 pElGet = pDoc->DocAssocRoot[i];
				i++;
			     }
			   while (pElGet == NULL && i < MAX_ASSOC_DOC);
			   break;
			default:
			   break;
		     }
	       if (pElGet != NULL)
		  /* traduit l'element a prendre, sauf s'il a deja ete traduit et */
		  /* qu'il s'agit d'une regle Get */
		  TranslateTree (pElGet, pDocGet, *transChar, *lineBreak, (ThotBool)(pTRule->TrType == TCopy), recordLineNb);
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
	       break;
	    case TInclude:
	       /* inclusion d'un fichier */
	       if (pTRule->TrBufOrConst == ToConst)
		 {
		    i = pTSch->TsConstBegin[pTRule->TrInclFile - 1] - 1;
		    ustrncpy(fname, &pTSch->TsConstant[i], MAX_PATH - 1);
		 }
	       else if (pTRule->TrBufOrConst == ToBuffer)
		  /* le nom du fichier est dans un buffer */
		  ustrncpy (fname, pTSch->TsBuffer[pTRule->TrInclFile - 1],
			   MAX_PATH - 1);
	       if (fname[0] == EOS)
		  /* pas de nom de fichier */
		  fullName[0] = EOS;
	       else if (fname[0] == TEXT('/'))
		  /* nom de fichier absolu */
	          ustrcpy (fullName, fname);
	       else
		 {
	           /* compose le nom du fichier a ouvrir avec le nom du
		      directory des schemas... */
		    ustrncpy (directoryName, SchemaPath, MAX_PATH);
		    MakeCompleteName (fname, TEXT(""), directoryName, fullName, &i);
		 }
	       /* si le fichier a inclure est deja ouvert en ecriture, on
		  le flush.  */
	       i = GetSecondaryFile (fullName, pDoc, FALSE);
	       if (i >= 0)
		  fflush (OutputFile[i].OfFileDesc);
	       /* ouvre le fichier a inclure */
	       includedFile = TtaReadOpen (fullName);
	       if (includedFile == 0)
		  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_INCLUDE_FILE_IMP), fname);
	       else
		  /* le fichier a inclure est ouvert */
		 {
		    /* while (TtaReadByte (includedFile, &c)) */
		    while (TtaReadWideChar (includedFile, &c, CharEncoding))
		       /* on ecrit dans le fichier principal courant */
		       PutChar (c, 1, NULL, pDoc, *lineBreak, CharEncoding);
		    TtaReadClose (includedFile);
		 }
	       break;
	    default:
	       break;
	 }
}

/*----------------------------------------------------------------------
   ApplyElTypeRules   applique a l'element pointe' par pEl les regles 
   de traduction qui correspondent a son type et qui doivent       
   s'appliquer a la position position.                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ApplyElTypeRules (TOrder position, ThotBool * transChar,
				    ThotBool * lineBreak, ThotBool * removeEl,
			       PtrElement pEl, int TypeEl, PtrTSchema pTSch,
				      PtrSSchema pSS, PtrDocument pDoc, ThotBool recordLineNb)

#else  /* __STDC__ */
static void         ApplyElTypeRules (position, transChar, lineBreak, removeEl, pEl, TypeEl,
				      pTSch, pSS, pDoc, recordLineNb)
TOrder              position;
ThotBool           *transChar;
ThotBool           *lineBreak;
ThotBool           *removeEl;
PtrElement          pEl;
int                 TypeEl;
PtrTSchema          pTSch;
PtrSSchema          pSS;
PtrDocument         pDoc;
ThotBool            recordLineNb

#endif /* __STDC__ */

{
   PtrTRuleBlock       pBlock;
   PtrTRule            pTRule;


   /* premier bloc de regles correspondant au type de l'element */
   pBlock = pTSch->TsElemTRule[TypeEl - 1];
   /* parcourt les blocs de regles du type de l'element */
   while (pBlock != NULL)
     {
	if (ConditionIsTrue (pBlock, pEl, NULL, pDoc))
	   /* la condition du bloc est verifiee */
	  {
	     pTRule = pBlock->TbFirstTRule;	/* premiere regle du bloc */
	     /* parcourt les regles du bloc */
	     while (pTRule != NULL)
	       {
		  if (pTRule->TrOrder == position)
		     /* c'est une regle a appliquer a cette position */
		     if (pTRule->TrType == TRemove)
			*removeEl = TRUE;
		     else if (pTRule->TrType == TNoTranslation)
			*transChar = FALSE;
		     else if (pTRule->TrType == TNoLineBreak)
			*lineBreak = FALSE;
		     else
			/* on applique la regle */
			ApplyTRule (pTRule, pTSch, pSS, pEl, transChar,
				    lineBreak, removeEl, NULL, NULL, pDoc,
				    recordLineNb);
		  /* passe a la regle suivante */
		  pTRule = pTRule->TrNextTRule;
	       }
	  }
	/* passe au bloc suivant */
	pBlock = pBlock->TbNextBlock;
     }
}


/*----------------------------------------------------------------------
   TranslateTree   traduit le sous-arbre dont la racine est pEl	
   et applique les regles de traduction des feuilles si transChar  
   est vrai.                                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         TranslateTree (PtrElement pEl, PtrDocument pDoc,
				   ThotBool transChar, ThotBool lineBreak,
				   ThotBool enforce, ThotBool recordLineNb)

#else  /* __STDC__ */
static void         TranslateTree (pEl, pDoc, transChar, lineBreak, enforce, recordLineNb)
PtrElement          pEl;
PtrDocument         pDoc;
ThotBool            transChar;
ThotBool            lineBreak;
ThotBool            enforce;
ThotBool            recordLineNb;
#endif /* __STDC__ */

{
   PtrElement          pChild;
   PtrTSchema          pTSch, pTS;
   PtrSSchema          pSS, pParentSS;
   SRule              *pSRule;
   NotifyElement       notifyEl;
   int                 elemType, i;
   ThotBool            found;
   ThotBool            removeEl;

   if (!pEl->ElTransContent || enforce)
     {
	/* cherche le schema de traduction qui s'applique a l'element */
	pTSch = GetTranslationSchema (pEl->ElStructSchema);
	if (pTSch == NULL)
	   return;
	removeEl = FALSE;
	pSS = pEl->ElStructSchema;
	elemType = pEl->ElTypeNumber;
	/* envoie l'evenement ElemExport.Pre a l'application, si elle */
	/* le demande */
	notifyEl.event = TteElemExport;
	notifyEl.document = (Document) IdentDocument (pDoc);
	notifyEl.element = (Element) pEl;
	notifyEl.elementType.ElTypeNum = elemType;
	notifyEl.elementType.ElSSchema = (SSchema) pSS;
	notifyEl.position = 0;
	if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
	   /* l'application refuse que Thot sauve l'element */
	   return;
	/* les attributs n'ont pas ete traduits */
	pEl->ElTransAttr = FALSE;
	/* la presentation n'a pas ete traduite */
	pEl->ElTransPres = FALSE;
	/* s'il s'agit de l'element racine d'une nature, on prend les regles */
	/* de presentation (s'il y en a) de la regle nature dans la structure */
	/* englobante. */
	/* on ne traite pas les marques de page */
	if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
	   if (pEl->ElParent != NULL)
	      /* il y a un englobant */
	      if (pEl->ElParent->ElStructSchema != pEl->ElStructSchema)
		 /* cherche la regle introduisant la nature dans le schema de */
		 /* structure de l'englobant. */
		{
		   pParentSS = pEl->ElParent->ElStructSchema;
		   found = FALSE;
		   i = 0;
		   do
		     {
			pSRule = &pParentSS->SsRule[i++];
			if (pSRule->SrConstruct == CsNatureSchema)
			   if (pSRule->SrSSchemaNat == pEl->ElStructSchema)
			      found = TRUE;
		     }
		   while (!found && i < pParentSS->SsNRules);
		   if (found)
		     {
			pTS = GetTranslationSchema (pEl->ElParent->ElStructSchema);
			if (pTS != NULL)
			   if (pTS->TsElemTRule[i - 1] != NULL)
			      /* il y a des regles de traduction pour la nature, on */
			      /* les prend */
			     {
				pTSch = pTS;
				pSS = pEl->ElParent->ElStructSchema;
				elemType = i;
			     }
		     }
		}
	if (pTSch != NULL)
	   /* on ne traduit pas les elements dont le schema de structure n'a */
	   /* pas de schema de traduction correspondant */
	  {
	     /* if needed, record the current line number of the main
		output file in the element being translated */
	     if (recordLineNb)
	        pEl->ElLineNb = OutputFile[1].OfLineNumber + 1;
	     /* Cherche et applique les regles de traduction associees au type */
	     /* de l'element et qui doivent s'appliquer avant la traduction du */
	     /* contenu de l'element */
	     ApplyElTypeRules (TBefore, &transChar, &lineBreak, &removeEl,
			       pEl, elemType, pTSch, pSS, pDoc, recordLineNb);
	     /* on ne traduit les attributs que si ce n'est pas deja fait par */
	     /* une regle Create Attributes associee au type */
	     if (!pEl->ElTransAttr)
		/* Parcourt les attributs de l'element et applique les regles
		   des attributs qui doivent ^etre appliquees avant la
		   traduction du contenu de l'element */
		ApplyAttrRules (TBefore, pEl, &removeEl, &transChar, &lineBreak, pDoc, recordLineNb);

	     /* on ne traduit la presentation que si ce n'est pas deja fait par */
	     /* une regle Create Presentation */
	     if (!pEl->ElTransPres)
		/* Parcourt les presentations de l'element et applique les regles
		 * de traduction correspondantes qui doivent ^etre appliquees
		 * avant la traduction du contenu de l'element */
		ApplyPresTRules (TBefore, pEl, &removeEl, &transChar,
				 &lineBreak, NULL, pDoc, recordLineNb);
	     /* traduit le contenu de l'element, sauf si on a deja rencontre' */
	     /* une regle Remove pour cet element. */
	     if (!removeEl)
		/* pas de regle Remove */
		if (pEl->ElTerminal)
		   /* c'est une feuille, applique les regles de traduction des */
		   /* feuilles et sort le contenu dans le fichier principal */
		   TranslateLeaf (pEl, transChar, lineBreak, 1, pDoc);
		else
		   /* ce n'est pas une feuille, traduit successivement tous les */
		   /* fils de l'element */
		  {
		     pChild = pEl->ElFirstChild;
		     while (pChild != NULL)
		       {
			  TranslateTree (pChild, pDoc, transChar, lineBreak,
					 enforce, recordLineNb);
			  pChild = pChild->ElNext;
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
		ApplyPresTRules (TAfter, pEl, &removeEl, &transChar,
				 &lineBreak, NULL, pDoc, recordLineNb);
	     if (!pEl->ElTransAttr)
		/* Parcourt les attributs de l'element et applique les regles des
		 * attributs qui doivent etre appliquees apres la traduction du
		 * contenu */
		ApplyAttrRules (TAfter, pEl, &removeEl, &transChar, &lineBreak, pDoc, recordLineNb);
	     /* Cherche et applique les regles associees au type de l'element et
	      * qui doivent s'appliquer apres la traduction du contenu */
	     ApplyElTypeRules (TAfter, &transChar, &lineBreak, &removeEl,
			       pEl, elemType, pTSch, pSS, pDoc, recordLineNb);
	     if (!enforce)
		/* marque que l'element a ete traite' */
		pEl->ElTransContent = TRUE;
	     /* envoie l'evenement ElemExport.Post a l'application, si elle */
	     /* le demande */
	     notifyEl.event = TteElemExport;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	  }
     }
}


/*----------------------------------------------------------------------
   ResetTranslTags   remet a zero tous les indicateurs "deja traduit" 
   de l'arbre de racine pEl.                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         ResetTranslTags (PtrElement pEl)
#else  /* __STDC__ */
static void         ResetTranslTags (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pChild;

   if (pEl != NULL)
     {
	pEl->ElTransContent = FALSE;
	pEl->ElTransAttr = FALSE;
	pEl->ElTransPres = FALSE;
	if (!pEl->ElTerminal)
	  {
	     pChild = pEl->ElFirstChild;
	     while (pChild != NULL)
	       {
		  ResetTranslTags (pChild);
		  pChild = pChild->ElNext;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   InitOutputFiles initialise les fichiers de sortie.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         InitOutputFiles (FILE * mainFile, PtrDocument pDoc)

#else  /* __STDC__ */
static void         InitOutputFiles (mainFile, pDoc)
FILE               *mainFile;
PtrDocument         pDoc;

#endif /* _STDC__ */
{
   /* Entree 0 : standard output */
   OutputFile[0].OfFileName[0] = EOS;
   OutputFile[0].OfFileDesc = NULL;
   OutputFile[0].OfBufferLen = 0;
   OutputFile[0].OfIndent = 0;
   OutputFile[0].OfPreviousIndent = 0;
   OutputFile[0].OfLineNumber = 0;
   OutputFile[0].OfStartOfLine = TRUE;
   OutputFile[0].OfCannotOpen = FALSE;

   /* Entree 1 : fichier de sortie principal */
   OutputFile[1].OfFileName[0] = EOS;
   OutputFile[1].OfFileDesc = mainFile;
   OutputFile[1].OfBufferLen = 0;
   OutputFile[1].OfIndent = 0;
   OutputFile[1].OfPreviousIndent = 0;
   OutputFile[1].OfLineNumber = 0;
   OutputFile[1].OfStartOfLine = TRUE;
   OutputFile[1].OfCannotOpen = FALSE;
   NOutputFiles = 2;
}

/*----------------------------------------------------------------------
   FlushOutputFiles vide les buffers dans les fichiers de sortie      
   correspondants.                                                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         FlushOutputFiles (PtrDocument pDoc)

#else  /* __STDC__ */
static void         FlushOutputFiles (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 i, fich;
#  ifdef _I18N_
   int                 nb_bytes2write, index;
   unsigned char       mbc[MAX_BYTES];
#  endif /* !_I18N_ */

   for (fich = 1; fich < NOutputFiles; fich++)
     if (OutputFile[fich].OfFileDesc != NULL)
        {
		 for (i = 0; i < OutputFile[fich].OfBufferLen; i++) {
#            ifdef _I18N_
             nb_bytes2write = TtaWC2MB (OutputFile[fich].OfBuffer[i], mbc, CharEncoding);
             for (index = 0; index < nb_bytes2write; index++)
                 putc (mbc[index], OutputFile[fich].OfFileDesc);
#            else  /* !_I18N_ */
             putc (OutputFile[fich].OfBuffer[i], OutputFile[fich].OfFileDesc);
#            endif /* !_I18N_ */
		 }
	if (OutputFile[fich].OfFileDesc != NULL)
	   fclose (OutputFile[fich].OfFileDesc);
        }
}

/*----------------------------------------------------------------------
   ExportDocument     exporte le document pointe' par pDoc, selon le  
   schema de traduction de nom TSchemaName et produit le resultat  
   dans le fichier de nom fName.
   Retourne TRUE si export reussi.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool      ExportDocument (PtrDocument pDoc, STRING fName,
			      STRING TSchemaName, ThotBool recordLineNb)

#else  /* __STDC__ */
ThotBool      ExportDocument (pDoc, fName, TSchemaName, recordLineNb)
PtrDocument         pDoc;
STRING              fName;
STRING              TSchemaName;
ThotBool            recordLineNb;

#endif /* __STDC__ */

{
   FILE               *outputFile; /* fichier de sortie principal */
   int                 i;
   ThotBool            ok = TRUE;

   /* cree le fichier de sortie principal */
   outputFile = ufopen (fName, TEXT("w"));
   if (outputFile == NULL)
     ok = FALSE;
   else
      /* le fichier de sortie principal a ete cree' */
     {
	/* separe nom de directory et nom de fichier */
	ustrncpy (fileDirectory, fName, MAX_PATH);
	fileDirectory[MAX_PATH - 1] = EOS;
	i = ustrlen (fileDirectory);
	while (i > 0 && fileDirectory[i] != DIR_SEP)
	   i--;
	if (fileDirectory[i] == DIR_SEP)
	  {
	     ustrcpy (fileName, &fileDirectory[i + 1]);
	     fileDirectory[i + 1] = EOS;
	  }
	else
	  {
	     ustrcpy (fileName, &fileDirectory[i]);
	     fileDirectory[i] = EOS;
	  }
	/* charge le schema de traduction du document */
	if (!LoadTranslationSchema (TSchemaName, pDoc->DocSSchema) != 0)
	  {
	    /* echec au chargement du schema de traduction */
	    fclose (outputFile);
	    ok = FALSE;
	  }
	else
	  {
	     /* separe nom de fichier et extension */
	     fileExtension[0] = EOS;
	     i = ustrlen (fileName);
	     i--;
	     while (i > 0 && fileName[i] != TEXT('.'))
		i--;
	     if (fileName[i] == TEXT('.'))
	       {
		  ustrncpy (fileExtension, &fileName[i], MAX_PATH);
		  fileName[i] = EOS;
	       }
	     InitOutputFiles (outputFile, pDoc);
	     /* remet a zero les indicateurs "deja traduit" de tous les elements */
	     /* du document */
	     ResetTranslTags (pDoc->DocRootElement);
	     for (i = 0; i < MAX_ASSOC_DOC; i++)
		if (pDoc->DocAssocRoot[i] != NULL)
		   ResetTranslTags (pDoc->DocAssocRoot[i]);
	     /* traduit l'arbre principal du document */
	     TranslateTree (pDoc->DocRootElement, pDoc, TRUE, TRUE, FALSE,
			    recordLineNb);
	     /* traduit les arbres associe's */
	     for (i = 0; i < MAX_ASSOC_DOC; i++)
		if (pDoc->DocAssocRoot[i] != NULL)
		   TranslateTree (pDoc->DocAssocRoot[i], pDoc, TRUE, TRUE,
				  FALSE, recordLineNb);
	     /* vide ce qui traine dans les buffers de sortie */
	     /* et ferme ces fichiers */
	     FlushOutputFiles (pDoc);
	  }
     }
   ClearTranslationSchemasTable ();
   fflush (stdout);
   fflush (stderr);
   if (!ok)
     TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_CREATE_FILE_IMP), fName);
   return (ok);
}

/*----------------------------------------------------------------------
   ExportTree   exporte le sous arbre pointe par pEl du document 
   pointe' par pDoc, selon le schema de traduction de nom TSchemaName 
   et produit le resultat dans le fichier de nom fName ou dans le buffer.					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ExportTree (PtrElement pEl, PtrDocument pDoc, STRING fName, STRING TSchemaName)

#else  /* __STDC__ */
void                ExportTree (pEl, pDoc, fName, TSchemaName)
PtrElement	    pEl;
PtrDocument         pDoc;
STRING              fName;
STRING              TSchemaName;
#endif /* __STDC__ */

{
  int                 i;

  /* fichier de sortie principal */
  FILE               *outputFile;

 
  /* cree le fichier de sortie principal */
  outputFile = ufopen (fName, TEXT("w"));
  
  if (outputFile == NULL)
    TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_CREATE_FILE_IMP), fName);
  else
    /* le fichier de sortie principal a ete cree' */
    {
      /* separe nom de directory et nom de fichier */
      ustrncpy (fileDirectory, fName, MAX_PATH);
      fileDirectory[MAX_PATH - 1] = EOS;
      i = ustrlen (fileDirectory);
      while (i > 0 && fileDirectory[i] != DIR_SEP)
	i--;
      if (fileDirectory[i] == DIR_SEP)
	{
	  ustrcpy (fileName, &fileDirectory[i + 1]);
	  fileDirectory[i + 1] = EOS;
	}
      else
	{
	  ustrcpy (fileName, &fileDirectory[i]);
	  fileDirectory[i] = EOS;
	}
      /* charge le schema de traduction du document */
      if (!LoadTranslationSchema (TSchemaName, pDoc->DocSSchema) != 0 ||
	  !GetTranslationSchema (pEl->ElStructSchema) != 0)
	/* echec au chargement du schema de traduction */
	fclose (outputFile);
      else
	{
	  /* separe nom de fichier et extension */
	  fileExtension[0] = EOS;
	  i = ustrlen (fileName);
	  i--;
	  while (i > 0 && fileName[i] != TEXT('.'))
	    i--;
	  if (fileName[i] == TEXT('.'))
	    {
	      ustrncpy (fileExtension, &fileName[i], MAX_PATH);
	      fileName[i] = EOS;
	    }
	  InitOutputFiles (outputFile, pDoc);
	  /* remet a zero les indicateurs "deja traduit" de tous les elements */
	  /* de l'arbre a traduire */
	  ResetTranslTags (pEl);
	  /* traduit l'arbre */
	  TranslateTree (pEl, pDoc, TRUE, TRUE, FALSE, FALSE);
	  /* vide ce qui traine dans les buffers de sortie */
	  /* et ferme ces fichiers */
	  FlushOutputFiles (pDoc);
	}
    }
  ClearTranslationSchemasTable ();
  fflush (stdout);
  fflush (stderr);
}
