
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   printtyp.c :
   Ce programme effectue le chargement d'un schema de typographie et
   du schema de structure associe' et liste le contenu de ce schema de
   typographie. 
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "message.h"
#include "libmsg.h"
#include "typmsg.h"
#include "typetyp.h"
#define EXPORT
#include "environ.var"

#include "cdialog.f"
#include "memory.f"

static PtrSSchema pSchemaStr;
static PtrSchTypo   pSchemaTyp;
static char         filename[1000];
static int          i, j, El, Attr, Val;
static int          TYP;	/* Identification des messages Typ */

TtAttribute           *pAt1;
RTypoAttribut      *pRT1;
TyCasAttrNum       *pCa1;


/* ---------------------------------------------------------------------- */
/* |    ReadEnv lit la variable THOTSCH dans DirectorySchemas.          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ReadEnv ()

#else  /* __STDC__ */
static void         ReadEnv ()
#endif				/* __STDC__ */

{
   char               *pT;

   /* lit la variable d'environnement THOTSCH */
   pT = (char *) TtaGetEnvString ("THOTSCH");
   if (pT == NULL)
      /* la variable d'environnement THOTSCH n'existe pas */
      DirectorySchemas[0] = '\0';
   else
      /* on copie la valeur de la variable THOTSCH */
      strncpy (DirectorySchemas, pT, MAX_TXT_LEN);
}


/* ---------------------------------------------------------------------- */
/* |    wrnb ecrit au terminal l'entier nb.                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         wrnb (int nb)

#else  /* __STDC__ */
static void         wrnb (nb)
int                 nb;

#endif /* __STDC__ */

{
   if (nb < 0)
     {
	printf ("-");
	nb = -nb;
     }
   if (nb < 10)
      printf ("%1d", nb);
   else if (nb < 100)
      printf ("%2d", nb);
   else if (nb < 1000)
      printf ("%3d", nb);
   else if (nb < 10000)
      printf ("%4d", nb);
   else if (nb < 100000)
      printf ("%5d", nb);
   else
      printf ("999999999");
}


/* ---------------------------------------------------------------------- */
/* |    wrnom ecrit au terminal le nom n.                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         wrnom (Name n)

#else  /* __STDC__ */
static void         wrnom (n)
Name                 n;

#endif /* __STDC__ */

{
   int                 i;

   i = 1;
   while (n[i - 1] != '\0')
     {
	if (n[i - 1] < ' ' || n[i - 1] > '~')
	   /* caractere non imprimable, ecrit son code */
	  {
	     printf ("\\%o", (unsigned char) n[i - 1]);
	  }
	else
	  {
	     /* caractere imprimable, ecrit le  caractere */
	     putchar (n[i - 1]);
	  }
	i++;
     }
}


/* ---------------------------------------------------------------------- */
/* |    wrnomregle ecrit au terminal le nom de la regle de numero r.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         wrnomregle (int r)

#else  /* __STDC__ */
static void         wrnomregle (r)
int         r;

#endif /* __STDC__ */

{
   if (r > 0)
      wrnom (pSchemaStr->SsRule[r - 1].SrName);
}


/* ---------------------------------------------------------------------- */
/* |    wrnomattr ecrit au terminal le nom de l'attribut de numero a.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         wrnomattr (int a)

#else  /* __STDC__ */
static void         wrnomattr (a)
int      a;

#endif /* __STDC__ */

{
   if (a != 0)
      wrnom (pSchemaStr->SsAttribute[abs (a) - 1].AttrName);
}

/* ---------------------------------------------------------------------- */
/* |    wrcondtypo                                                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         wrcondtypo (int n, PtrRegleTypo * pR)

#else  /* __STDC__ */
static void         wrcondtypo (n, pR)
int                 n;
PtrRegleTypo       *pR;

#endif /* __STDC__ */

{
   TypoCondition      *pCo1;

   pCo1 = &(*pR)->RTyCondition[n - 1];
   if (pCo1->CondNegative)
      printf ("NOT ");
   switch (pCo1->CondTypeCond)
	 {
	    case TyPremier:
	       printf ("FIRST ");
	       break;
	    case TyDernier:
	       printf ("LAST ");
	       break;
	    case TyDansType:
	       printf ("IN ");
	       wrnomregle (pCo1->CondTypeElem);
	       break;
	    case TyAvantType:
	       printf ("NEXT ");
	       if (pCo1->CondOpTypeIn)
		  printf ("IN ");
	       else
		  printf ("= ");
	       wrnomregle (pCo1->CondTypeElem);
	       break;
	    case TyApresType:
	       printf ("PREVIOUS");
	       if (pCo1->CondOpTypeIn)
		  printf ("IN ");
	       else
		  printf ("= ");
	       wrnomregle (pCo1->CondTypeElem);
	       break;
	    case TyLangue:
	       printf ("LANGUAGE = ");
	       wrnom (pCo1->CondNom);
	       break;
	    case TyFonction:
	       printf ("IS ");
	       wrnom (pCo1->CondNom);
	       break;
	 }
}

/* ------------------------------------------------------------------------ */
/* |    wrsuitefunctions ecrit au terminal la suite de fonctions chainees | */
/* |       dont RP pointe sur la fonction de tete.                        | */
/* ------------------------------------------------------------------------ */

#ifdef __STDC__
static void         wrsuitefunctions (PtrTypoFunction PT)

#else  /* __STDC__ */
static void         wrsuitefunctions (PT)
PtrTypoFunction     PT;

#endif /* __STDC__ */

{
   PtrTypoFunction     pFc1;

   while (PT != NULL)
      /* ecrit une fonction de typographie */
     {
	pFc1 = PT;
	printf ("   ");

	switch (pFc1->TFTypeFunct)
	      {
		 case TyCapital:
		    printf ("Capitalization: ");
		    break;
		 case TyWord:
		    printf ("Word: ");
		    break;
		 case TySpace:
		    printf ("SpaceTable: ");
		    break;
		 case TyPunct:
		    printf ("Punctuation: ");
		    break;
		 case TyPair:
		    printf ("InsertPair: ");
		    break;
		 case TyDistance:
		    printf ("Distance: ");
		    break;
		 case TyExponent:
		    printf ("Exponent: ");
		    break;
		 case TyAbbrev:
		    printf ("Abbreviation: ");
		    break;
		 case TyAttribute:
		    printf ("Attribute: ");
		    break;
	      }
	wrnom (pFc1->TFIdent);
	printf ("\n");		/* passe a la regle suivante */
	PT = pFc1->TFSuiv;
     }
}


/* ---------------------------------------------------------------------- */
/* |   wrsuiteregles ecrit au terminal la suite de regles chainees dont | */
/* |            RP pointe sur la regle de tete.                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         wrsuiteregles (PtrRegleTypo RT)

#else  /* __STDC__ */
static void         wrsuiteregles (RT)
PtrRegleTypo        RT;

#endif /* __STDC__ */

{
   PtrRegleTypo        pRe1;

   while (RT != NULL)
      /* ecrit une regle de typographie */
     {
	pRe1 = RT;
	printf ("   ");
	if (pRe1->RTyNbCond > 0)
	  {
	     printf ("IF (");
	     wrcondtypo (1, &RT);
	     for (i = 2; i <= pRe1->RTyNbCond; i++)
	       {
		  printf (" AND ");
		  wrcondtypo (i, &RT);
	       }
	     printf (") ");
	  }
	switch (pRe1->RTyTypeFunct)
	      {
		 case TyCapital:
		    printf ("Capitalization: ");
		    break;
		 case TyWord:
		    printf ("Word: ");
		    break;
		 case TySpace:
		    printf ("SpaceTable: ");
		    break;
		 case TyPunct:
		    printf ("Punctuation: ");
		    break;
		 case TyPair:
		    printf ("InsertPair: ");
		    break;
		 case TyDistance:
		    printf ("Distance: ");
		    break;
		 case TyExponent:
		    printf ("Exponent: ");
		    break;
		 case TyAbbrev:
		    printf ("Abbreviation: ");
		    break;
		 case TyAttribute:
		    printf ("Attribute: ");
		    break;
	      }
	wrnom (pRe1->RTyIdentFunct);
	printf ("\n");		/* passe a la regle suivante */
	RT = pRe1->RTyRegleSuiv;
     }
}

/* ---------------------------------------------------------------------- */
/* |    main                                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 main (int argc, char **argv)

#else  /* __STDC__ */
int                 main (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */

{

   TYP = TtaGetMessageTable ("libdialogue", LIB_MSG_MAX);
   TYP = TtaGetMessageTable ("typdialogue", TYP_MSG_MAX);
   /* recupere d'abord le nom du schema a lister */
   filename[0] = '\0';
   if (argc != 2)
      goto Usage;
   argv++;
   strcpy (filename, *argv);
   ReadEnv ();
   /* indique que le chargement du schema de typographie doit commencer par */
   /* le chargement du schema de structure correspondant */
   GetSchStruct (&pSchemaStr);
   pSchemaStr->SsRootElem = 0;
   /* lit le schema de typographie et le schema de structure correspondant */
   pSchemaTyp = RdSchTypo (filename, pSchemaStr);
   if (pSchemaTyp == NULL)
      TtaDisplaySimpleMessage (TYP, FATAL, TYP_FILE_NOT_FOUND);
   else
      /* les schemas ont ete lus correctement */
     {
	/* ecrit au terminal le nom du schema de structure */
	printf ("\nTYPOGRAPHY ");
	printf ("%s", pSchemaTyp->STyNomStruct);
	printf (";\n");

	/* ecrit au terminal la liste des alineas */
	printf ("\n\nALINEA\n");
	for (El = 1; El <= pSchemaStr->SsNRules; El++)
	   if (pSchemaTyp->STyElemAlinea[El - 1])
	     {
		wrnomregle (El);
		printf (", ");
	     }

	/* ecrit au terminal la liste des fonctions */
	printf ("\n\nFUNCTION\n");
	wrsuitefunctions (pSchemaTyp->STyFunction);

	/* ecrit au terminal les regles de typographie */
	/* des elements structure's */
	printf ("\n\nELEMENT\n");
	for (El = 1; El <= pSchemaStr->SsNRules; El++)
	   if (pSchemaTyp->STyRegleElem[El - 1] != NULL)
	     {
		wrnomregle (El);
		printf (":\n");
		printf ("   BEGIN\n");
		wrsuiteregles (pSchemaTyp->STyRegleElem[El - 1]);
		printf ("   END;\n");
		printf ("\n");
	     }

	/* ecrit au terminal les regles de typographie des attributs */
	if (pSchemaStr->SsNAttributes > 0)
	  {
	     printf ("\nATTR\n");
	     for (Attr = 1; Attr <= pSchemaStr->SsNAttributes; Attr++)
	       {
		  pAt1 = &pSchemaStr->SsAttribute[Attr - 1];
		  pRT1 = pSchemaTyp->STyAttribSem[Attr - 1];
		  if (pRT1 != NULL)
		    {
		       /* cet attribut a une typographie */
		       switch (pAt1->AttrType)
			     {
				case AtNumAttr:
				   for (i = 1; i <= pRT1->RTyANbCas; i++)
				     {
					pCa1 = &pRT1->RTyACas[i - 1];
					wrnom (pAt1->AttrName);
					if (pRT1->RTyATypeElem > 0)
					  {
					     printf ("(");
					     wrnomregle (pRT1->RTyATypeElem);
					     printf (")");
					  }
					if (pCa1->TyANBorneInf == pCa1->TyANBorneSup)
					  {
					     printf ("=");
					     wrnb (pCa1->TyANBorneInf);
					  }
					else if (pCa1->TyANBorneInf != -MAX_INT_ATTR_VAL - 1
						 && pCa1->TyANBorneSup != MAX_INT_ATTR_VAL + 1)
					  {
					     printf (" IN [");
					     wrnb (pCa1->TyANBorneInf);
					     printf ("..");
					     wrnb (pCa1->TyANBorneSup);
					     printf ("] ");
					  }
					else if (pCa1->TyANBorneInf != -MAX_INT_ATTR_VAL - 1
						 || pCa1->TyANBorneSup != MAX_INT_ATTR_VAL + 1)
					   if (pCa1->TyANBorneInf != -MAX_INT_ATTR_VAL - 1)
					     {
						printf (">");
						wrnb (pCa1->TyANBorneInf - 1);
					     }
					   else if (pCa1->TyANBorneSup != MAX_INT_ATTR_VAL + 1)
					     {
						printf ("<");
						wrnb (pCa1->TyANBorneSup + 1);
					     }
					printf (":\n");
					if (pCa1->TyANBlocRegles == NULL)
					   printf ("   BEGIN END;\n");
					else
					  {
					     if (pCa1->TyANBlocRegles->RTyRegleSuiv != NULL)
						printf ("   BEGIN\n");
					     wrsuiteregles (pCa1->TyANBlocRegles);
					     if (pCa1->TyANBlocRegles->RTyRegleSuiv != NULL)
						printf ("   END;\n");
					  }
					printf ("\n");
				     }
				   break;

				case AtTextAttr:
				   if (pRT1->RTyATxt != NULL)
				     {
					wrnom (pAt1->AttrName);
					if (pRT1->RTyATypeElem > 0)
					  {
					     printf ("(");
					     wrnomregle (pRT1->RTyATypeElem);
					     printf (")");
					  }
					if (pRT1->RTyATxtVal[0] != '\0')
					  {
					     printf ("=\'");
					     wrnom (pRT1->RTyATxtVal);
					     printf ("\'");
					  }
					printf (":\n");
					if (pRT1->RTyATxt->RTyRegleSuiv != NULL)
					   printf ("   BEGIN\n");
					wrsuiteregles (pRT1->RTyATxt);
					if (pRT1->RTyATxt->RTyRegleSuiv != NULL)
					   printf ("   END;\n");
					printf ("\n");
				     }
				   break;
				case AtReferenceAttr:
				   if (pRT1->RTyARefPremRegle != NULL)
				     {
					wrnom (pAt1->AttrName);
					if (pRT1->RTyATypeElem > 0)
					  {
					     printf ("(");
					     wrnomregle (pRT1->RTyATypeElem);
					     printf (")");
					  }
					printf (":\n");
					if (pRT1->RTyARefPremRegle->RTyRegleSuiv != NULL)
					   printf ("   BEGIN\n");
					wrsuiteregles (pRT1->RTyARefPremRegle);
					if (pRT1->RTyARefPremRegle->RTyRegleSuiv != NULL)
					   printf ("   END;\n");
					printf ("\n");
				     }
				   break;

				case AtEnumAttr:
				   for (Val = 0; Val <= pAt1->AttrNEnumValues; Val++)
				      if (pRT1->RTyAValEnum[Val] != NULL)
					{
					   wrnom (pAt1->AttrName);
					   if (pRT1->RTyATypeElem > 0)
					     {
						printf ("(");
						wrnomregle (pRT1->RTyATypeElem);
						printf (")");
					     }
					   if (Val > 0)
					     {
						printf ("=");
						wrnom (pAt1->AttrEnumValue[Val - 1]);
					     }
					   printf (":\n");
					   if (pRT1->RTyAValEnum[Val]->RTyRegleSuiv
					       != NULL)
					      printf ("   BEGIN\n");
					   wrsuiteregles (pRT1->RTyAValEnum[Val]);
					   if (pRT1->RTyAValEnum[Val]->RTyRegleSuiv
					       != NULL)
					      printf ("   END;\n");
					   printf ("\n");
					}
				   break;
				default:;
				   break;
			     }	/* end of switch */
		    }		/* end of if (pRT1 != NULL) */
	       }		/* end of for Attr ... */
	  }			/* end of if (pSchemaStr->SsNAttributes > 0) */
	printf ("END\n");
     }				/* end of else */
   exit (0);

 Usage:
   fprintf (stderr, "usage : %s <input-file>\n", argv[0]);
   exit (1);
}
/* End Of Module printtyp */
