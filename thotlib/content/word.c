
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* Module mot.c */
/* Procedure de recherche de mots
   E. Picheral, CICB - 29 aout 1991
   I. Vatton mars 1993 - reecriture apres insertion des langues
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "language.h"

static PtrDocument  DocSelInitial;
static PtrElement   ElDebutSelInitial;
static PtrElement   ElFinSelInitial;
static int          CarDebutSelInitial;
static int          CarFinSelInitial;

#include "select.f"
#include "arbabs.f"
#include "hyphen.f"

/* ---------------------------------------------------------------------- */
/* | CarSuivant retourne le caractere qui suit le caractere courant     | */
/* |            buffer[rank] et met a jour adbuff et rank.              | */
/* |            S'il n'y a pas de caractere suivant retourne le         | */
/* |            caractere `\0`.                                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
unsigned char       CarSuivant (PtrTextBuffer * buffer, int *rank)

#else  /* __STDC__ */
unsigned char       CarSuivant (buffer, rank)
PtrTextBuffer     *buffer;
int                *rank;

#endif /* __STDC__ */

{

   if (*buffer == NULL)
      return (0);
   else
     {
	/* Indice suivant */
	(*rank)++;

	if (*rank >= (*buffer)->BuLength)
	  {
	     /* Il faut changer de buffer */
	     *rank = 0;
	     *buffer = (*buffer)->BuNext;
	     if (*buffer == NULL)
		return (0);
	     else
		return ((*buffer)->BuContent[*rank]);
	  }
	else
	   return ((*buffer)->BuContent[*rank]);
     }
}				/*CarSuivant */


/* ---------------------------------------------------------------------- */
/* | CarPrecedent retourne le caractere qui precede le caractere        | */
/* |            courant buffer[rank] et met a jour adbuff et rank.      | */
/* |            S'il n'y a pas de caractere precedent retourne le       | */
/* |            caractere `\0`.                                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
unsigned char       CarPrecedent (PtrTextBuffer * buffer, int *rank)

#else  /* __STDC__ */
unsigned char       CarPrecedent (buffer, rank)
PtrTextBuffer     *buffer;
int                *rank;

#endif /* __STDC__ */

{

   if (*buffer == NULL)
      return (0);
   else
     {
	/* Indice suivant */
	(*rank)--;
	if (*rank < 0)
	  {
	     /* Il faut changer de buffer */
	     *rank = 0;
	     *buffer = (*buffer)->BuPrevious;
	     if (*buffer == NULL)
		return (0);
	     else
	       {
		  *rank = (*buffer)->BuLength - 1;
		  return ((*buffer)->BuContent[*rank]);
	       }
	  }
	else
	   return ((*buffer)->BuContent[*rank]);
     }
}				/*CarPrecedent */

/* ---------------------------------------------------------------------- */
/* |    MotOk teste que la chaine designee constitue bien un MOT.       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             MotOk (PtrElement premsel, int premcar, PtrElement dersel, int dercar)

#else  /* __STDC__ */
boolean             MotOk (premsel, premcar, dersel, dercar)
PtrElement          premsel;
int                 premcar;
PtrElement          dersel;
int                 dercar;

#endif /* __STDC__ */

{
   /* pas de caractere suivant immediatement dans le meme element */
   /* sauf espace, ponctuation, parenthesage, guillemet */
   if (dercar > 0)
     {
	if (dersel->ElTextLength >= dercar)
	   if (!SeparateurMot (dersel->ElText->BuContent[dercar - 1]))
	      return (False);
	/* pas de caractere precedant immediatement */
	/* sauf signe de ponctuation, parenthesage, guillemet */
	if (premcar > 1)
	   if (!SeparateurMot (premsel->ElText->BuContent[premcar - 2]))
	      return (False);
     }
   return (True);
}				/*MotOk */


/* ---------------------------------------------------------------------- */
/* | InitSearchDomain rend vrai si l'initialisation s'est bien passee   | */
/* |    Le parame`tre domain vaut :                                     | */
/* |      0 si on recherche avant la selection                          | */
/* |      1 si on recherche dans la selection                           | */
/* |      2 si on recherche apres la selection                          | */
/* |      3 si on recherche dans tout le document.                      | */
/* |    Le parame`tre context pointe sur le contexte de domaine de      | */
/* |    recherche a` initialiser.                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             InitSearchDomain (int domain, PtrSearchContext context)

#else  /* __STDC__ */
boolean             InitSearchDomain (domain, context)
int                 domain;
PtrSearchContext           context;

#endif /* __STDC__ */

{
   boolean             ok;

   if (context == NULL)
      return (False);
   else
     {
	context->STree = 0;
	context->SWholeDocument = False;
     }

   /* Recupere la selection courante */
   ok = SelEditeur (&DocSelInitial, &ElDebutSelInitial, &ElFinSelInitial,
		    &CarDebutSelInitial, &CarFinSelInitial);
   /* Memorise le domaine de recherche des mots */
   if (domain == 3)		/* Dans tout le document */
     {
	context->SStartElement = NULL;
	context->STree = 0;
	context->SWholeDocument = True;
	context->SEndElement = NULL;
	context->SStartChar = 0;
	context->SEndChar = 0;
	context->SStartToEnd = True;
     }
   else if (!ok)
      /* Pas de selection courante */
      context->SDocument = NULL;
   else if (context->SDocument != DocSelInitial)
      /* La selection ne correspond pas au document */
      context->SDocument = NULL;
   else
     {
	context->SWholeDocument = False;
	/* Attention les indices de caracteres sont des indices a */
	/* la C (0 ... n) et des indices a la Pascal (1 ... n+1)  */
	if (domain == 0)	/* ElemIsBefore la selection */
	  {
	     context->SStartElement = NULL;
	     context->SStartChar = 0;
	     context->SEndElement = ElDebutSelInitial;
	     context->SEndChar = CarDebutSelInitial;	/*caractere avant */
	     context->SStartToEnd = False;
	  }
	else if (domain == 1)	/* Dans la selection */
	  {
	     context->SStartElement = ElDebutSelInitial;
	     context->SStartChar = CarDebutSelInitial - 1;	/* caractere courant */
	     context->SEndElement = ElFinSelInitial;
	     context->SEndChar = CarFinSelInitial - 1;	/* caractere courant */
	     context->SStartToEnd = True;
	  }
	else if (domain == 2)	/* Apres la selection */
	  {
	     context->SStartElement = ElFinSelInitial;
	     context->SStartChar = CarFinSelInitial - 1;
	     if (context->SStartChar < 0)
		context->SStartChar = 0;
	     if (ElFinSelInitial != NULL)
		if (CarFinSelInitial == 0 || CarFinSelInitial > ElFinSelInitial->ElTextLength)
		   if (!ElFinSelInitial->ElTerminal)
		     {
			context->SStartElement = NextElement (ElFinSelInitial);
			context->SStartChar = 0;
		     }
	     context->SEndElement = NULL;
	     context->SEndChar = 0;
	     context->SStartToEnd = True;
	  }
     }

   if (context->SDocument == NULL)
     {
	/* aucun document selectionne */
	context->SStartElement = NULL;
	context->SEndElement = NULL;
	context->SStartChar = 0;
	context->SEndChar = 0;
	context->STree = 0;
	context->SWholeDocument = False;
	return (False);
     }
   return (True);
}				/*InitSearchDomain */


/* ---------------------------------------------------------------------- */
/* | UpdateDuringSearch met a jour la selection initiale                | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                UpdateDuringSearch (PtrElement ElCourant, int lg)

#else  /* __STDC__ */
void                UpdateDuringSearch (ElCourant, lg)
PtrElement          ElCourant;
int                 lg;

#endif /* __STDC__ */

{
   if (ElCourant == ElFinSelInitial)
      /* la selection initiale est dans l'element  */
      /* ou` on a fait le remplacement */
      if (CarFinSelInitial != 0)
	 /* la borne n'est pas a la fin de l'element, on decale la borne */
	 CarFinSelInitial += lg;
}				/*UpdateDuringSearch */


/* ---------------------------------------------------------------------- */
/* | RestoreAfterSearch re'tablit la se'lection initiale et nettoie     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                RestoreAfterSearch ()

#else  /* __STDC__ */
void                RestoreAfterSearch ()
#endif				/* __STDC__ */

{
   int                 longprec, carfin;

   AnnuleSelect ();
   carfin = CarFinSelInitial;
   if (carfin > 0)
      carfin--;
   if (CarDebutSelInitial > 0)
     {
	if (ElDebutSelInitial == ElFinSelInitial)
	   longprec = carfin;
	else
	   longprec = 0;
	SelectString (DocSelInitial, ElDebutSelInitial, CarDebutSelInitial,
		      longprec);
     }
   else
      SelectEl (DocSelInitial, ElDebutSelInitial, True, True);

   if (ElFinSelInitial != ElDebutSelInitial && ElFinSelInitial != NULL)
      SelEtend (ElFinSelInitial, carfin, True, False, False);
}				/*RestoreAfterSearch */


/* ---------------------------------------------------------------------- */
/* |    ArbreSuivant Pour le contexte de recherche context, cherche le  | */
/* |    prochain arbre a traiter, lorsqu'on est dans une recherche qui  | */
/* |    porte sur tout le document.                                     | */
/* |    Met a jour le contexte de recherche pour pouvoir relancer la    | */
/* |    recherche dans le nouvel arbre a traiter.                       | */
/* |    Retourne True si un arbre suivant a ete trouve' et, dans ce cas,| */
/* |    elCourant et carCourant representent la position ou il faut     | */
/* |    relancer la recherche.                                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             ArbreSuivant (PtrElement * elCourant, int *carCourant, PtrSearchContext context)

#else  /* __STDC__ */
boolean             ArbreSuivant (elCourant, carCourant, context)
PtrElement         *elCourant;
int                *carCourant;
PtrSearchContext           context;

#endif /* __STDC__ */

{
   int                 i;
   boolean             ret;

   *elCourant = NULL;
   *carCourant = 0;
   ret = False;
   if (context != NULL)
      if (context->SWholeDocument && context->STree >= 0)
	{
	   /****context->SStartElement = NULL;
	   context->SEndElement = NULL; deja fait **********/
	   i = context->STree;
	   context->STree = -1;
	   if (context->SStartToEnd)
	      /* recherche en avant */
	     {
		i++;
		while (i < MAX_ASSOC_DOC && context->STree < 0)
		   if (context->SDocument->DocAssocRoot[i - 1] != NULL)
		      context->STree = i;
		   else
		      i++;
	     }
	   else
	      /* recherche en arriere */
	     {
		i--;
		while (i > 0 && context->STree < 0)
		   if (context->SDocument->DocAssocRoot[i - 1] != NULL)
		      context->STree = i;
		   else
		      i--;
		if (i == 0)
		   if (context->SDocument->DocRootElement != NULL)
		      context->STree = 0;
	     }
	   if (context->STree == 0)
	      *elCourant = context->SDocument->DocRootElement;
	   else if (context->STree > 0)
	      *elCourant = context->SDocument->DocAssocRoot[i - 1];
	   if (*elCourant != NULL)
	      if (!context->SStartToEnd)
		{
		   *elCourant = LastLeaf (*elCourant);
		   *carCourant = (*elCourant)->ElVolume;
		}
	   ret = (context->STree >= 0);
	}
   return ret;

}

/* ---------------------------------------------------------------------- */
/* |  SearchNextWord recheche dans le domaine de recherche le mot       | */
/* |    qui suit la position courante.                                  | */
/* |    Retourne le mot selectionne et met a jour le pointeur           | */
/* |    a la fin du mot selectionne.                                    | */
/* |    Le parame`tre context pointe sur le contexte de domaine de      | */
/* |    recherche concerne' ou NULL si aucun domaine fixe'.             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             SearchNextWord (PtrElement * ElCourant, int *CarCourant, char mot[MAX_WORD_LEN], PtrSearchContext context)

#else  /* __STDC__ */
boolean             SearchNextWord (ElCourant, CarCourant, mot, context)
PtrElement         *ElCourant;
int                *CarCourant;
char                mot[MAX_WORD_LEN];
PtrSearchContext           context;

#endif /* __STDC__ */

{
   PtrElement          pEl, Elfin;
   PtrElement          pAscendant;
   int                 icar, carfin;
   int                 lg;
   int                 index;
   PtrTextBuffer      adbuff;
   unsigned char       car;

   pEl = *ElCourant;
   icar = *CarCourant;
   mot[0] = '\0';

   if (pEl == NULL && context != NULL)
     {
	/* C'est le debut de la recherche */
	pEl = context->SStartElement;
	icar = context->SStartChar;
	if (pEl == NULL && context->SDocument != NULL)
	   pEl = context->SDocument->DocRootElement;
     }

   /* Verifie que l'element est de type texte */
   /* et que la recherche ne debute pas sur le dernier caractere */
   if (pEl != NULL)
      if (pEl->ElTypeNumber != CharString + 1 || icar + 1 >= pEl->ElTextLength)
	{
	   /* On n'a pas trouve de buffer */
	   adbuff = NULL;
	   icar = 0;
	}
      else
	 /* 1er Buffer de l'element */
	 adbuff = pEl->ElText;

   /* Determine l'element limite du contexte de recherche */
   if (context == NULL)
     {
	Elfin = NULL;
	carfin = 0;
     }
   else
     {
	Elfin = context->SEndElement;
	carfin = context->SEndChar;
     }

   while (adbuff == NULL && pEl != NULL)
     {
	/* Recherche le premier element texte non vide et modifiable */

	pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	icar = 0;
	if (pEl != NULL)
	  {
	     if (Elfin != NULL)
		if (ElemIsBefore (Elfin, pEl))
		   /* l'element trouve' est apres l'element de fin, */
		   /* on fait comme si on n'avait pas trouve' */
		   pEl = NULL;
	  }
	else if (context != NULL)
	   /* Si on recherche dans tout le document on change d'arbre */
	   if (context->SWholeDocument)
	     {
		/* cherche l'arbre a traiter apres celui ou` on n'a pas trouve' */
		if (ArbreSuivant (&pEl, &icar, context))
		   /* Il se peut que l'element rendu soit de type texte  */
		   if (pEl->ElTypeNumber != CharString + 1)
		      pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
	     }

	if (pEl != NULL)
	   /* on verifie que cet element ne fait pas partie d'une inclusion et */
	   /* n'est pas cache' a l'utilisateur */
	  {
	     pAscendant = pEl;
	     while (pAscendant->ElParent != NULL && pAscendant->ElSource == NULL)
		pAscendant = pAscendant->ElParent;
	     if (pAscendant->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' */
		   /* On saute les elements vides */
		   if (pEl->ElTextLength != 0)
		      adbuff = pEl->ElText;
	  }
     }

   if (pEl == NULL)
      return (False);
   else if (pEl == Elfin && icar >= carfin)
      /* On est arrive a la fin du domaine de recherche */
      return (False);
   else
     {

	/* Calcule l'index de depart de la recherche */
	lg = icar;
	while (lg >= adbuff->BuLength)
	  {
	     /* On passe au buffer suivant */
	     lg -= adbuff->BuLength;
	     adbuff = adbuff->BuNext;
	  }
	index = lg;
	car = adbuff->BuContent[index];

	/* On se place au debut du mot */
	while (car != 0 && SeparateurMot (car)
	       && (pEl != Elfin || icar < carfin))
	  {
	     car = CarSuivant (&adbuff, &index);
	     icar++;
	  }

	/* Recherche le premier separateur apres le mot */
	/* On verifie que l'on ne depasse pas la fin du domaine de recherche */
	lg = 0;
	while (lg < MAX_WORD_LEN && car != 0 && !SeparateurMot (car)
	       && (pEl != Elfin || icar < carfin))
	  {
	     mot[lg++] = car;
	     car = CarSuivant (&adbuff, &index);
	     icar++;
	  }

	/* positionne les valeurs de retour */
	mot[lg] = '\0';
	*ElCourant = pEl;
	*CarCourant = icar;
	/* Si on a trouve effectivement un mot */
	if (lg > 0)
	   return (True);
	else
	   /* On peut etre en fin de feuille qui se termine par un espace */
	   /* On continue la recherche */
	   return (SearchNextWord (ElCourant, CarCourant, mot, context));
     }
}				/*SearchNextWord */


/* ---------------------------------------------------------------------- */
/* |  SearchPreviousWord recheche dans le domaine de recherche le mot   | */
/* |            qui precede la position courante.                       | */
/* |            Retourne le mot selectionne et met a jour le pointeur   | */
/* |            au debut du mot selectionne.                            | */
/* |    Le parame`tre context pointe sur le contexte de domaine de      | */
/* |    recherche concerne' ou NULL si aucun domaine fixe'.             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             SearchPreviousWord (PtrElement * ElCourant, int *CarCourant, char mot[MAX_WORD_LEN], PtrSearchContext context)

#else  /* __STDC__ */
boolean             SearchPreviousWord (ElCourant, CarCourant, mot, context)
PtrElement         *ElCourant;
int                *CarCourant;
char                mot[MAX_WORD_LEN];
PtrSearchContext           context;

#endif /* __STDC__ */

{
   PtrElement          pEl, Elfin;
   PtrElement          pAscendant;
   int                 icar, carfin, j;
   int                 lg;
   int                 index;
   PtrTextBuffer      adbuff;
   unsigned char       car;
   char                renvmot[MAX_WORD_LEN];

   pEl = *ElCourant;
   icar = *CarCourant;
   mot[0] = '\0';

   if (pEl == NULL)
     {
	/* C'est le debut de la recherche */
	pEl = context->SEndElement;
	icar = context->SEndChar;
	if (icar > 2)
	   /* La fin de selection pointe toujours sur un caratere plus loin */
	   icar--;
     }
   icar--;

   /* Verifie que l'element est de type texte */
   /* et que la recherche ne debute pas sur le dernier caractere */
   if (pEl->ElTypeNumber != CharString + 1 || icar <= 0)
     {
	/* On n'a pas trouve de buffer */
	adbuff = NULL;
	icar = 0;
     }
   else
     {
	/* 1er Buffer de l'element */
	adbuff = pEl->ElText;
     }

   /* Determine l'element limite du contexte de recherche */
   if (context == NULL)
     {
	Elfin = NULL;
	carfin = 0;
     }
   else
     {
	Elfin = context->SStartElement;
	carfin = context->SStartChar;
     }

   while (adbuff == NULL && pEl != NULL)
     {
	/* Recherche le premier element texte non vide */
	pEl = BackSearchTypedElem (pEl, CharString + 1, NULL);
	if (pEl != NULL)
	  {
	     if (Elfin != NULL)
		if (ElemIsBefore (pEl, Elfin))
		   /* l'element trouve' est avant l'element de debut, */
		   /* on fait comme si on n'avait pas trouve' */
		   pEl = NULL;
	  }
	else if (context != NULL)
	   /* Si on recherche dans tout le document on change d'arbre */
	   if (context->SWholeDocument)
	     {
		/* cherche l'arbre a traiter avant celui ou` on n'a pas trouve' */
		if (ArbreSuivant (&pEl, &icar, context))
		   /* Il se peut que l'element rendu soit de type texte */
		   if (pEl->ElTypeNumber != CharString + 1)
		      pEl = BackSearchTypedElem (pEl, CharString + 1, NULL);
	     }

	/* On saute les elements vides */
	if (pEl != NULL)
	   /* on verifie que cet element ne fait pas partie d'une inclusion et */
	   /* n'est pas cache' a l'utilisateur */
	  {
	     pAscendant = pEl;
	     while (pAscendant->ElParent != NULL && pAscendant->ElSource == NULL)
		pAscendant = pAscendant->ElParent;
	     if (pAscendant->ElSource == NULL)
		/* on n'est pas dans une inclusion */
		if (!ElementIsHidden (pEl))
		   /* l'element n'est pas cache' */
		   if (pEl->ElTextLength != 0)
		     {
			icar = pEl->ElTextLength - 1;
			adbuff = pEl->ElText;
		     }
	  }
     }

   if (pEl == NULL)
      return (False);
   else if (pEl == Elfin && icar < carfin)
      /* On est arrive a la fin du domaine de recherche */
      return (False);
   else
     {
	/* Calcule l'index de depart de la recherche */
	lg = icar;
	while (lg >= adbuff->BuLength)
	  {
	     /* On passe au buffer suivant */
	     lg -= adbuff->BuLength;
	     adbuff = adbuff->BuNext;
	  }

	index = lg;
	car = adbuff->BuContent[index];
	/* On se place a la fin du mot */
	while (car != 0 && icar >= 0 && SeparateurMot (car)
	       && (pEl != Elfin || icar >= carfin))
	  {
	     car = CarPrecedent (&adbuff, &index);
	     icar--;
	  }

	/* On se place au debut du mot et recupere le mot a l'envers */
	lg = 0;
	while (lg < MAX_WORD_LEN && car != 0 && icar >= 0 && !SeparateurMot (car)
	       && (pEl != Elfin || icar >= carfin))
	  {
	     renvmot[lg++] = car;
	     car = CarPrecedent (&adbuff, &index);
	     icar--;
	  }

	/* Recopie le mot a l'endroit */
	icar++;
	j = 0;
	while (j < lg)
	  {
	     mot[j] = renvmot[lg - 1 - j];
	     j++;
	  }

	/* positionne les valeurs de retour */
	mot[lg] = '\0';
	*ElCourant = pEl;
	*CarCourant = icar;
	/* Si on a trouve effectivement un mot */
	if (lg > 0)
	   return (True);
	else
	   /* On peut etre en fin de feuille qui se termine par un espace */
	   /* On continue la recherche */
	   return (SearchPreviousWord (ElCourant, CarCourant, mot, context));
     }
}				/*SearchPreviousWord */
