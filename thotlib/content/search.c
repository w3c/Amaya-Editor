
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   cherche.c : gestion de la commande de recherche.
   Projet THOT
   Module traitant la commande de recherche
   V. Quint     Septembre 1984  
   Major Changes:
   V. Quint     Septembre 1984
 */

#include "thot_sys.h"
#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"
#include "appaction.h"
#include "app.h"
#include "message.h"
#include "typecorr.h"

#define EXPORT extern
#include "environ.var"
#include "edit.var"

#include "arbabs.f"
#include "attrherit.f"
#include "cherche.f"
#include "cherchemenu.f"
#include "commun.f"
#include "creation.f"
#include "crimabs.f"
#include "def.f"
#include "docvues.f"
#include "appexec.f"
#include "edit.f"
#include "except.f"
#include "imabs.f"
#include "memory.f"
#include "modif.f"
#include "modimabs.f"
#include "ouvre.f"
#include "refelem.f"
#include "schemas.f"
#include "sel.f"
#include "select.f"
#include "storage.f"
#include "structure.f"


/* ---------------------------------------------------------------------- */
/* |    RemplaceTexte remplace dans l'element texte pEl la chaine       | */
/* |            commencant au caractere icar et de longueur LgChaineCh  | */
/* |            par la chaine de remplacement.                          | */
/* |     Attention : a l'exception des recherches des references, les   | */
/* |    recherches se limitent a l'arbre (arbre principal ou arbre      | */
/* |    d'elements associes d'un meme type) auquel appartient la        | */
/* |    selection courante. On ne cherche pas dans toute la foret       | */
/* |    d'un document.                                                  | */
/* |    On pourrait combiner certains criteres de recherche,            | */
/* |    par exemple chercher telle chaine dans tel type d'element et/ou | */
/* |    avec tel attribut.                                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                RemplaceTexte (PtrDocument docsel, PtrElement pEl, int icar, int LgChaineCh, char pChaineRemplace[MAX_CHAR], int LgChaineRempl, boolean Affiche)

#else  /* __STDC__ */
void                RemplaceTexte (docsel, pEl, icar, LgChaineCh, pChaineRemplace, LgChaineRempl, Affiche)
PtrDocument         docsel;
PtrElement          pEl;
int                 icar;
int                 LgChaineCh;
char                pChaineRemplace[MAX_CHAR];
int                 LgChaineRempl;
boolean             Affiche;

#endif /* __STDC__ */

{
   int                 ibuf1, ibuf2, lg, diff, dvol, vue, i;
   PtrTextBuffer      pBT1, pBT2, pBTn;
   PtrAbstractBox             pAb;
   PtrTextBuffer      pBu1;
   PtrDocument         pDo1;
   PtrElement          pAsc;
   PtrAbstractBox             pPa1;
   NotifyOnTarget      notifyEl;
   boolean             DontReplace;
   boolean             UnPave;

   if (pEl->ElTypeNumber != CharString + 1)
      return;
   /* envoie l'evenement ElemTextModify.Pre a qui le demande */
   DontReplace = FALSE;
   pAsc = pEl;
   while (pAsc != NULL)
     {
	notifyEl.event = TteElemTextModify;
	notifyEl.document = (Document) IdentDocument (docsel);
	notifyEl.element = (Element) pAsc;
	notifyEl.target = (Element) pEl;
	notifyEl.targetdocument = (Document) IdentDocument (docsel);
	DontReplace = DontReplace || ThotSendMessage ((NotifyEvent *) & notifyEl, TRUE);
	pAsc = pAsc->ElParent;
     }
   if (DontReplace)
      return;
   /* cherche le buffer du premier caractere a remplacer: pBT1 */
   pBT1 = pEl->ElText;
   lg = 0;
   /* longueur cumulee des buffers de texte precedant le */
   /* buffer contenant le 1er caractere */
   while (lg + pBT1->BuLength < icar)
     {
	lg += pBT1->BuLength;
	pBT1 = pBT1->BuNext;
     }
   ibuf1 = icar - lg;
   /* index dans le buffer de texte pointe par pBT1 */
   /* du 1er caractere a remplacer */
   /* cherche le buffer du dernier caractere a remplacer: pBT2 */
   pBT2 = pBT1;
   while (lg + pBT2->BuLength < icar + LgChaineCh - 1)
     {
	lg += pBT2->BuLength;
	pBT2 = pBT2->BuNext;
     }
   ibuf2 = icar + LgChaineCh - 1 - lg;
   /* index dans le buffer de texte */
   /* pointe' par pBT2 du dernier caractere a remplacer */
   if (LgChaineRempl > LgChaineCh)
      /* la chaine de remplacement est plus longue que la chaine */
      /* a remplacer */
     {
	diff = LgChaineRempl - LgChaineCh;
	if (pBT2->BuLength + diff > MAX_CHAR - 1)
	   /* il n'y a pas assez de place */
	   /* ajoute un buffer apres le dernier buffer de la chaine */
	   /* a remplacer */
	  {
	     pBTn = pBT2->BuNext;
	     GetBufTexte (&pBT2->BuNext);
	     pBT2->BuNext->BuPrevious = pBT2;
	     pBT2->BuNext->BuNext = pBTn;
	     if (pBTn != NULL)
		pBTn->BuPrevious = pBT2->BuNext;
	     pBTn = pBT2->BuNext;
	     /* recopie la fin du buffer dans le nouveau */
	     for (i = 0; i <= pBT2->BuLength + diff - MAX_CHAR; i++)
		pBTn->BuContent[i] = pBT2->BuContent[MAX_CHAR - 1 - diff + i];
	     pBTn->BuLength = pBT2->BuLength + diff - MAX_CHAR + 1;
	     pBTn->BuContent[pBTn->BuLength] = '\0';
	     pBT2->BuLength -= pBTn->BuLength;
	     pBT2->BuContent[pBT2->BuLength] = '\0';
	  }
	/* decale a droite les caracteres qui suivent la chaine */
	/* a remplacer dans le buffer pBT2 */
	pBu1 = pBT2;
	for (i = pBu1->BuLength; i >= ibuf2; i--)
	   pBu1->BuContent[i + diff] = pBu1->BuContent[i];
	pBu1->BuLength += diff;
	diff = 0;
     }
   else if (LgChaineRempl < LgChaineCh)
      /* la chaine de remplacement est plus courte que la chaine */
      /* a remplacer */
     {
	diff = LgChaineCh - LgChaineRempl;
	if (diff > ibuf2)
	   diff = ibuf2;
	/* decale a gauche les caracteres qui suivent la chaine */
	/* a remplacer */
	pBu1 = pBT2;
	for (i = ibuf2 + 1 - diff; i <= pBu1->BuLength + 1 - diff; i++)
	   pBu1->BuContent[i - 1] = pBu1->BuContent[i + diff - 1];
	pBu1->BuLength -= diff;
	diff = LgChaineCh - LgChaineRempl - diff;
     }
   else
      /* les deux chaines ont meme longueur */
      diff = 0;
   /* copie la chaine de remplacement */
   lg = 0;
   pBTn = pBT1;
   i = ibuf1;
   while (lg < LgChaineRempl)
     {
	lg++;
	pBTn->BuContent[i - 1] = pChaineRemplace[lg - 1];
	i++;
	if (i > pBTn->BuLength)
	  {
	     pBu1 = pBTn;
	     pBu1->BuContent[i - 1] = '\0';
	     pBu1->BuLength = i - 1;
	     pBTn = pBu1->BuNext;
	     i = 1;
	  }
     }
   if (diff > 0)
     {
	pBTn->BuContent[i - 1] = '\0';
	pBTn->BuLength = i - 1;
     }
   /* met a jour le volume de l'element */
   dvol = LgChaineRempl - LgChaineCh;
   pEl->ElTextLength = pEl->ElTextLength + dvol;
   pEl->ElVolume = pEl->ElTextLength;
   /* met a jour le volume de tous les elements ascendants */
   pAsc = pEl->ElParent;
   while (pAsc != NULL)
     {
	pAsc->ElVolume = pAsc->ElVolume + dvol;
	pAsc = pAsc->ElParent;
     }
   /* change le volume des paves de l'element modifie' */
   UnPave = FALSE;
   for (vue = 1; vue <= MAX_VIEW_DOC; vue++)
     {
	pAb = pEl->ElAbstractBox[vue - 1];
	if (pAb != NULL)
	  {
	     UnPave = TRUE;
	     pAb->AbChange = TRUE;
	     pDo1 = docsel;
	     if (!VueAssoc (pEl))
		pDo1->DocViewModifiedAb[vue - 1] = Englobant (pAb, pDo1->DocViewModifiedAb[vue - 1]);
	     else
	       {
		  pDo1->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
		     Englobant (pAb, pDo1->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
	       }
	     dvol = pAb->AbVolume - pEl->ElTextLength;
	     do
	       {
		  pPa1 = pAb;
		  /* met a jour le volume du premier pave */
		  pPa1->AbVolume -= dvol;
		  pAb = pPa1->AbEnclosing;
	       }
	     while (!(pAb == NULL));
	  }
     }
   /* reaffiche toutes les vues */
   RazSelect ();
   if (UnPave)
     {
	MajImAbs (docsel);
	ReaffDoc (docsel);
     }

   /* si l'element TEXTE modifie' appartient soit a un element copie' */
   /* dans des paves par une regle Copy, soit a un element inclus */
   /* dans d'autres, il faut reafficher ses copies */
   ReaffPaveCopie (pEl, docsel, TRUE);

   /* selectionne la chaine remplacee */
   if (Affiche)
     {
	if (LgChaineRempl < 2)
	   i = icar;
	else
	   i = icar + LgChaineRempl - 1;
	SelectStringWithAPP (docsel, pEl, icar, i);
     }
   /* le document a ete modifie' */
   docsel->DocModified = TRUE;
   docsel->DocNTypedChars += LgChaineRempl;
   /* envoie l'evenement ElemTextModify.Post a qui le demande */
   pAsc = pEl;
   while (pAsc != NULL)
     {
	notifyEl.event = TteElemTextModify;
	notifyEl.document = (Document) IdentDocument (docsel);
	notifyEl.element = (Element) pAsc;
	notifyEl.target = (Element) pEl;
	notifyEl.targetdocument = (Document) IdentDocument (docsel);
	ThotSendMessage ((NotifyEvent *) & notifyEl, FALSE);
	pAsc = pAsc->ElParent;
     }
}


/* ---------------------------------------------------------------------- */
/* |    caregal compare les caracteres c1 et c2, en confondant les      | */
/* |            majuscules et minuscules si MajEgalMin est vrai.        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      caregal (char c1, char c2, boolean MajEgalMin)

#else  /* __STDC__ */
static boolean      caregal (c1, c2, MajEgalMin)
char                c1;
char                c2;
boolean             MajEgalMin;

#endif /* __STDC__ */

{
   if (MajEgalMin)
     {
	if ((c1 >= 'A' && c1 <= 'Z') || (c1 >= '\300' && c1 <= '\336'))
	   c1 = (char) ((int) (c1) + 32);
	if ((c2 >= 'A' && c2 <= 'Z') || (c2 >= '\300' && c2 <= '\336'))
	   c2 = (char) ((int) (c2) + 32);
     }
   return (c1 == c2);
}


/* ---------------------------------------------------------------------- */
/* |    TextEqual rend vrai si la chaine cherchee est a` la position    | */
/* |            indiquee par (bloc,inx).                                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      TextEqual (PtrTextBuffer bloc, int inx, boolean MajEgalMin, char pChaineCherchee[MAX_CHAR], int LgChaineCh)

#else  /* __STDC__ */
static boolean      TextEqual (bloc, inx, MajEgalMin, pChaineCherchee, LgChaineCh)
PtrTextBuffer      bloc;
int                 inx;
boolean             MajEgalMin;
char                pChaineCherchee[MAX_CHAR];
int                 LgChaineCh;

#endif /* __STDC__ */
{
   boolean             ret;
   int                 inx2;
   boolean             egal;

   ret = FALSE;
   inx2 = 1;
   while (TRUE)
      /* assure que l'on va pouvoir prendre le caractere dans le bloc */
     {
	while (bloc != NULL)
	   if (bloc->BuLength <= inx)
	     {
		inx -= bloc->BuLength;
		bloc = bloc->BuNext;
	     }
	   else
	      goto Label_2;
	/* est-ce fini ? */
      Label_2:
	if (bloc == NULL || inx2 > LgChaineCh)
	  {
	     ret = inx2 > LgChaineCh;
	     goto Label_1;
	  }			/* fini */
	/* on compare les caracteres */
	if (!caregal (bloc->BuContent[inx], pChaineCherchee[inx2 - 1], MajEgalMin))
	   goto Label_1;	/* pas trouve */
	/* on avance */
	inx++;
	inx2++;
     }
 Label_1:
   egal = ret;
   return egal;
}


/* ---------------------------------------------------------------------- */
/* |    TextOk teste qu'on est bien positionne sur une chaine de texte  | */
/* |            egale a` la chaine cherchee.                            | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             TextOk (PtrElement premsel, int premcar, PtrElement dersel, int dercar, boolean MajEgalMin, char pChaineCherchee[MAX_CHAR], int LgChaineCh)
#else  /* __STDC__ */
boolean             TextOk (premsel, premcar, dersel, dercar, MajEgalMin, pChaineCherchee, LgChaineCh)
PtrElement          premsel;
int                 premcar;
PtrElement          dersel;
int                 dercar;
boolean             MajEgalMin;
char                pChaineCherchee[MAX_CHAR];
int                 LgChaineCh;

#endif /* __STDC__ */
{
   PtrTextBuffer      bloc;
   boolean             ok;

   ok = FALSE;
   if (premsel == dersel && premsel != NULL)
      if (premsel->ElTerminal && premsel->ElLeafType == LtText)
	{
	   bloc = premsel->ElText;
	   premcar--;
	   while (bloc != NULL && premcar > bloc->BuLength)
	     {
		premcar -= bloc->BuLength;
		bloc = bloc->BuNext;
	     }
	   ok = TextEqual (bloc, premcar, MajEgalMin, pChaineCherchee, LgChaineCh);
	}
   return ok;
}


/* ---------------------------------------------------------------------- */
/* |    ChAvChaine recherche une chaine en avant.                       | */
/* |    Attention: On ne trouve pas la chaine cherchee si elle est      | */
/* |    coupee par une marque de page ou par un changement de feuille   | */
/* |    texte du a des regles de presentation specifiques ou des        | */
/* |    attributs differents.                                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ChAvChaine (PtrTextBuffer pB, int ic, boolean * trouve, int *icar, boolean MajEgalMin, char pChaineCherchee[MAX_CHAR])

#else  /* __STDC__ */
static void         ChAvChaine (pB, ic, trouve, icar, MajEgalMin, pChaineCherchee)
PtrTextBuffer      pB;
int                 ic;
boolean            *trouve;
int                *icar;
boolean             MajEgalMin;
char                pChaineCherchee[MAX_CHAR];

#endif /* __STDC__ */

{
   int                 ir;
   int                 ix;
   int                 icx;
   PtrTextBuffer      pBx;
   boolean             stop;

   stop = FALSE;
   ix = 0;
   ir = 1;
   icx = 0;
   pBx = NULL;
   /* index dans ChaineCherchee du caractere a comparer */
   do
     {
	if (pB->BuContent[ic - 1] == '\0')
	  {
	     pB = pB->BuNext;
	     if (pB != NULL)
		/* saute les buffers vides */
	       {
		  ic = 0;
		  do
		     if (pB->BuLength > 0)
			ic = 1;
		  /* buffer non vide */
		     else
			/* buffer vide */
		       {
			  pB = pB->BuNext;
			  /* buffer suivant */
			  stop = pB == NULL;
			  /* fin si dernier buffer */
		       }
		  while (!(stop || ic == 1));
	       }
	     else
		stop = TRUE;
	  }
	if (!stop)
	   if (caregal (pB->BuContent[ic - 1], pChaineCherchee[ir - 1], MajEgalMin))
	     {
		if (ir == 1)
		  {
		     ix = ic;
		     pBx = pB;
		     icx = *icar;
		  }
		if (pChaineCherchee[ir] == '\0')
		  {
		     *trouve = TRUE;
		     stop = TRUE;
		  }
		else
		  {
		     ic++;
		     ir++;
		  }
	     }
	   else if (ix != 0)
	     {
		ic = ix + 1;
		pB = pBx;
		*icar = icx + 1;
		ix = 0;
		ir = 1;
	     }
	   else
	     {
		ic++;
		(*icar)++;
	     }
     }
   while (!(stop));
}


/* ---------------------------------------------------------------------- */
/* |    ChArChaine recherche une chaine en arriere.                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ChArChaine (PtrTextBuffer pB, int ic, boolean * trouve, int *icar, boolean MajEgalMin, char pChaineCherchee[MAX_CHAR], int LgChaineCh)

#else  /* __STDC__ */
static void         ChArChaine (pB, ic, trouve, icar, MajEgalMin, pChaineCherchee, LgChaineCh)
PtrTextBuffer      pB;
int                 ic;
boolean            *trouve;
int                *icar;
boolean             MajEgalMin;
char                pChaineCherchee[MAX_CHAR];
int                 LgChaineCh;

#endif /* __STDC__ */

{
   int                 ir;
   int                 ix;
   int                 icx;
   PtrTextBuffer      pBx;
   boolean             stop;

   stop = FALSE;
   ix = 0;
   ir = LgChaineCh;
   icx = 0;
   pBx = NULL;
   /* index dans ChaineCherchee du caractere a comparer */
   do
     {
	if (ic < 1)
	  {
	     pB = pB->BuPrevious;
	     if (pB != NULL)
		ic = pB->BuLength;
	     else
		stop = TRUE;
	  }
	if (!stop && ic > 0)
	   if (caregal (pB->BuContent[ic - 1], pChaineCherchee[ir - 1], MajEgalMin))
	     {
		if (ir == LgChaineCh)
		  {
		     ix = ic;
		     pBx = pB;
		     icx = *icar;
		  }
		if (ir == 1)
		  {
		     *trouve = TRUE;
		     stop = TRUE;
		  }
		else
		  {
		     ic--;
		     ir--;
		  }
	     }
	   else if (ix != 0)
	     {
		ic = ix - 1;
		pB = pBx;
		*icar = icx - 1;
		ix = 0;
		ir = LgChaineCh;
	     }
	   else
	     {
		ic--;
		(*icar)--;
	     }
     }
   while (!(stop));
}


/* ---------------------------------------------------------------------- */
/* |    ChTexte recherche dans le document pDoc la chaine de caracteres | */
/* |    decrite par les variables pChaineCherchee et LgChaineCh.        | */
/* |    A l'appel, les variables pElDebut, NumCarDebut, pElFin,         | */
/* |    NumCarFin indiquent la region ou on cherche.                    | */
/* |    Au retour, elles indiquent la chaine trouvee si la fonction     | */
/* |    retourne Vrai. (La fonction retourne faux en cas d'echec).      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             ChTexte (PtrDocument pDoc, PtrElement * pElDebut, int *NumCarDebut, PtrElement * pElFin, int *NumCarFin, boolean EnAvant, boolean MajEgalMin, char pChaineCherchee[MAX_CHAR], int LgChaineCh)

#else  /* __STDC__ */
boolean             ChTexte (pDoc, pElDebut, NumCarDebut, pElFin, NumCarFin, EnAvant, MajEgalMin, pChaineCherchee, LgChaineCh)
PtrDocument         pDoc;
PtrElement         *pElDebut;
int                *NumCarDebut;
PtrElement         *pElFin;
int                *NumCarFin;
boolean             EnAvant;
boolean             MajEgalMin;
char                pChaineCherchee[MAX_CHAR];
int                 LgChaineCh;

#endif /* __STDC__ */

{

   PtrElement          pEl;
   int                 car;
   int                 ibuf;
   int                 icar;
   boolean             trouve;
   PtrTextBuffer      pBT;
   boolean             result;
   PtrElement          pAscendant;

   result = FALSE;
   if (*pElDebut != NULL)
     {
	/* on n'a pas encore trouve' la chaine cherchee */
	trouve = FALSE;
	pEl = *pElDebut;
	/* on cherche d'abord la chaine */
	if (EnAvant)
	   /* Recherche en avant */
	  {
	     /* dercar designe le caractere qui suit le dernier caractere */
	     /* selectionne' */
	     if (pEl->ElTerminal && pEl->ElLeafType == LtText)
		if (*NumCarDebut >= 1 && *NumCarDebut <= pEl->ElTextLength)
		   /* commencer la recherche dans l'element courant */
		   /*  cherche le buffer du dernier caractere selectionne' */
		  {
		     pBT = pEl->ElText;
		     car = 0;
		     while (car + pBT->BuLength < *NumCarDebut)
		       {
			  car += pBT->BuLength;
			  pBT = pBT->BuNext;
		       }
		     ibuf = *NumCarDebut - car;
		     /* index dans le buffer de texte */
		     /* pointe' par pBT du 1er caractere a tester */
		     icar = *NumCarDebut;
		     ChAvChaine (pBT, ibuf, &trouve, &icar, MajEgalMin, pChaineCherchee);
		  }
	     while (!trouve && pEl != NULL)
	       {
		  pEl = FwdSearchTypedElem (pEl, CharString + 1, NULL);
		  if (pEl != NULL)
		     /* on a trouve un element de texte */
		    {
		       /* on verifie que cet element ne fait pas partie d'une */
		       /* inclusion */
		       pAscendant = pEl;
		       while (pAscendant->ElParent != NULL &&
			      pAscendant->ElSource == NULL)
			  pAscendant = pAscendant->ElParent;
		       if (pAscendant->ElSource == NULL)
			  /* on n'est pas dans une inclusion */
			  if (!ElementIsHidden (pEl))
			     /* l'element n'est pas dans une partie cachee */
			    {
			       icar = 1;
			       ChAvChaine (pEl->ElText, 1, &trouve, &icar, MajEgalMin,
					   pChaineCherchee);
			    }
		    }
	       }
	  }
	else
	   /* Recherche en arriere */
	  {
	     if (pEl->ElTerminal && pEl->ElLeafType == LtText)
		if (*NumCarDebut >= 2)
		   /* commencer la recherche dans l'element courant */
		   /* cherche le buffer du premier caractere selectionne' */
		  {
		     pBT = pEl->ElText;
		     car = 0;
		     while (car + pBT->BuLength < *NumCarDebut - 1)
		       {
			  car += pBT->BuLength;
			  pBT = pBT->BuNext;
		       }
		     ibuf = *NumCarDebut - car - 1;
		     /* index dans le buffer de texte */
		     /* pointe par pBT du 1er caractere a tester */
		     icar = *NumCarDebut - 1;
		     ChArChaine (pBT, ibuf, &trouve, &icar, MajEgalMin,
				 pChaineCherchee, LgChaineCh);
		  }
	     while (!trouve && pEl != NULL)
	       {
		  pEl = BackSearchTypedElem (pEl, CharString + 1, NULL);
		  if (pEl != NULL)
		     /* on a trouve' un element de texte */
		    {
		       /* on verifie que cet element ne fait pas partie d'une */
		       /* inclusion */
		       pAscendant = pEl;
		       while (pAscendant->ElParent != NULL &&
			      pAscendant->ElSource == NULL)
			  pAscendant = pAscendant->ElParent;
		       if (pAscendant->ElSource == NULL)
			  /* on n'est pas dans une inclusion */
			  if (!ElementIsHidden (pEl))
			     /* l'element n'est pas dans une partie cachee */
			    {
			       pBT = pEl->ElText;
			       while (pBT->BuNext != NULL)
				  pBT = pBT->BuNext;
			       icar = pEl->ElTextLength;
			       ChArChaine (pBT, pBT->BuLength, &trouve, &icar, MajEgalMin,
					   pChaineCherchee, LgChaineCh);
			    }
		    }
	       }
	     if (trouve)
		icar = icar - LgChaineCh + 1;
	  }
	if (trouve)
	   /* on a trouve' la chaine cherchee */
	   /* l'element trouve' est pointe' par pEl et icar est le rang */
	   /* dans cet element du 1er caractere de la chaine trouvee */
	   if (*pElFin != NULL)
	      /* il faut s'arreter avant l'extremite' du document */
	      if (pEl == *pElFin)
		 /* la chaine trouvee est dans l'element ou il faut s'arreter */
		{
		   if (EnAvant)
		     {
			if (icar + LgChaineCh - 1 > *NumCarFin)
			   /* la chaine trouvee se termine au-dela du caractere ou il */
			   /* faut s'arreter, on fait comme si on n'avait pas trouve' */
			   trouve = FALSE;
		     }
		   else
		     {
			if (*NumCarFin > 0)
			   if (icar < *NumCarFin)
			      trouve = FALSE;
		     }
		}
	      else if (EnAvant)
		{
		   if (ElemIsBefore (*pElFin, pEl))
		      /* l'element trouve' est apres l'element de fin, on fait */
		      /* comme si on n'avait pas trouve' */
		      trouve = FALSE;
		}
	      else if (ElemIsBefore (pEl, *pElFin))
		 trouve = FALSE;
	if (trouve)
	  {
	     *pElDebut = pEl;
	     *pElFin = pEl;
	     *NumCarDebut = icar;
	     /* *NumCarFin est le rang du caractere qui suit le dernier */
	     /* caractere de la chaine trouvee dans le buffer */
	     *NumCarFin = icar + LgChaineCh;
	     result = TRUE;
	  }
     }
   return result;
}


#define LgTable 10
/* ---------------------------------------------------------------------- */
/* |    MetSchemaDansTable      met le schema de structure pSchStr dans | */
/* |    la table TablePtrNature et retourne dans LgTableNat le nombre   | */
/* |    d'entrees de la table.                                          | */
/* |    Si SansDouble est vrai, on ne met chaque schema qu'une fois.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         MetSchemaDansTable (PtrSSchema pSchStr, PtrSSchema TablePtrNature[LgTable], int *LgTableNat, boolean SansDouble)

#else  /* __STDC__ */
static void         MetSchemaDansTable (pSchStr, TablePtrNature, LgTableNat, SansDouble)
PtrSSchema        pSchStr;
PtrSSchema        TablePtrNature[LgTable];
int                *LgTableNat;
boolean             SansDouble;

#endif /* __STDC__ */

{
   int                 n;
   boolean             present;

   present = FALSE;
   if (SansDouble)
     {
	/* verifie que ce schema n'est pas deja dans la table */
	n = 0;
	while (n < *LgTableNat && !present)
	  {
	     n++;
	     if (TablePtrNature[n - 1]->SsCode == pSchStr->SsCode)
		present = TRUE;
	  }
     }
   if (!present)
      /* le schema n'est pas dans la table */
     {
	/* met le schema dans la table si elle n'est pas pleine */
	if (*LgTableNat < LgTable)
	  {
	     (*LgTableNat)++;
	     TablePtrNature[*LgTableNat - 1] = pSchStr;
	  }
	/* cherche les natures utilisees par ce schema */
	ChNatures (pSchStr, TablePtrNature, LgTableNat, SansDouble);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChNatures cherche toutes les natures explicitement              | */
/* |            mentionnees dans le schema de structure pSchStr et      | */
/* |            effectivement utilises, ainsi que les schemas de nature | */
/* |            charge's dynamiquement et les extensions de ces schemas.| */
/* |            Met ces schemas dans la table TablePtrNature et         | */
/* |            retourne dans LgTableNat le nombre d'entrees de la      | */
/* |            table.                                                  | */
/* |            Si SansDouble est vrai, on ne met chaque schema         | */
/* |            qu'une fois.                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ChNatures (PtrSSchema pSchStr, PtrSSchema TablePtrNature[LgTable], int *LgTableNat, boolean SansDouble)

#else  /* __STDC__ */
void                ChNatures (pSchStr, TablePtrNature, LgTableNat, SansDouble)
PtrSSchema        pSchStr;
PtrSSchema        TablePtrNature[LgTable];
int                *LgTableNat;
boolean             SansDouble;

#endif /* __STDC__ */

{
   int                 j;
   PtrSSchema        pSc1;
   SRule              *pRe1;

   pSc1 = pSchStr;
   /* cherche les regles de nature dans la table des regles */
   for (j = 1; j <= pSc1->SsNRules; j++)
     {
	pRe1 = &pSc1->SsRule[j - 1];
	if (pRe1->SrConstruct == CsNatureSchema)
	   /* c'est une regle de nature */
	   if (pRe1->SrSSchemaNat != NULL)
	      if (pRe1->SrSSchemaNat->SsNObjects > 0)
		 /* la nature est effectivement utilisee */
		 MetSchemaDansTable (pRe1->SrSSchemaNat, TablePtrNature, LgTableNat, SansDouble);
     }
   /* met les extensions du schema dans la table */
   while (pSc1->SsNextExtens != NULL)
     {
	pSc1 = pSc1->SsNextExtens;
	MetSchemaDansTable (pSc1, TablePtrNature, LgTableNat, SansDouble);
     }
}

#ifdef IV
/* ---------------------------------------------------------------------- */
/* |    PaveSuiv retourne le pave suivant d'un pave Pav.                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrAbstractBox      PaveSuiv (PtrAbstractBox Pav)
#else  /* __STDC__ */
static PtrAbstractBox      PaveSuiv (Pav)
PtrAbstractBox             Pav;

#endif /* __STDC__ */
{
   if (Pav != NULL)
      if (Pav->AbFirstEnclosed != NULL)
	 while (Pav->AbFirstEnclosed != NULL)
	    Pav = Pav->AbFirstEnclosed;
      else
	{
	   while (Pav->AbNext == NULL && Pav->AbEnclosing != NULL)
	      Pav = Pav->AbEnclosing;
	   Pav = Pav->AbNext;
	}
   return Pav;
}

/* ---------------------------------------------------------------------- */
/* |    FirstVisible recherche le premier pave visible d'une vue.       | */
/* |    Si assoc est vrai, vue est en fait le numero d'element associe' | */
/* |            des elements qui s'affichent dans la vue traitee.       | */
/* |    Si assoc est faux, vue est le numero de vue (pour le document)  | */
/* |            de la vue traitee.                                      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrElement   FirstVisible (PtrDocument pdoc, int vue, boolean assoc)

#else  /* __STDC__ */
static PtrElement   FirstVisible (pdoc, vue, assoc)
PtrDocument         pdoc;
int                 vue;
boolean             assoc;

#endif /* __STDC__ */

{
   PtrAbstractBox             pav;
   int                 dy;
   PtrElement          first;

   /* cherche le pave racine de la vue */
   if (assoc)
      pav = pdoc->DocAssocRoot[vue - 1]->ElAbstractBox[0];
   else
      pav = pdoc->DocViewRootAb[vue - 1];
   if (pav == NULL)
      first = NULL;
   else
     {
	while (pav->AbFirstEnclosed != NULL)
	   pav = pav->AbFirstEnclosed;
	do
	  {
	     if (assoc)
		dy = PavPosFen (pav, pdoc->DocAssocFrame[vue - 1], 0);
	     else
		dy = PavPosFen (pav, pdoc->DocViewFrame[vue - 1], 0);
	     if (dy <= 0)
		pav = PaveSuiv (pav);
	  }
	while (!(dy > 0 || pav == NULL));
	if (pav != NULL)
	   first = pav->AbElement;
	else
	   first = NULL;
     }
   return first;
}
#endif

/* ---------------------------------------------------------------------- */
/* |    ElemSuiv retourne l'element suivant d'un element pEl.           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrElement          ElemSuiv (PtrElement pEl)
#else  /* __STDC__ */
PtrElement          ElemSuiv (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   if (pEl != NULL)
     {
	while (pEl->ElNext == NULL && pEl->ElParent != NULL)
	   pEl = pEl->ElParent;
	pEl = pEl->ElNext;
     }
   return pEl;
}


/* ---------------------------------------------------------------------- */
/* |    CherchePage recherche une page.                                 | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrElement          CherchePage (PtrElement from, int vue, int depl, boolean relatif)
#else  /* __STDC__ */
PtrElement          CherchePage (from, vue, depl, relatif)
PtrElement          from;
int                 vue;
int                 depl;
boolean             relatif;

#endif /* __STDC__ */
{
   PtrElement          pp;
   PtrElement          res;

   pp = from;
   res = from;
   if (depl > 0)
     {
	if (relatif)
	   /* c'est un deplacement relatif, si on est sur une page, on */
	   /* l'ignore et on commence la recherche a partir du suivant */
	   if (from->ElTypeNumber == PageBreak + 1)
	      from = ElemSuiv (from);
	pp = from;
	while (depl > 0 && pp != NULL)
	  {
	     pp = FwdSearchTypedElem (from, PageBreak + 1, NULL);
	     if (pp != NULL)
	       {
		  if (pp->ElViewPSchema == vue)
		     if (relatif)
		       {
			  res = pp;
			  depl--;
		       }
		     else
			/* on cherche la page de numero depl */
		     if (pp->ElPageNumber == depl)
			/* c'est la page cherchee */
		       {
			  res = pp;	/* resultat */
			  depl = 0;	/* fin de la recherche */
		       }
		  from = pp;
	       }
	  }
     }
   else
      while (depl < 0 && pp != NULL)
	{
	   pp = BackSearchTypedElem (from, PageBreak + 1, NULL);
	   if (pp != NULL)
	     {
		if (pp->ElViewPSchema == vue)
		  {
		     res = pp;
		     depl++;
		  }
		from = pp;
	     }
	}
   return res;
}


/* ---------------------------------------------------------------------- */
/* |    PageHautFenetre positionne le document pDoc dans sa frame       | */
/* |            de facon que le filet du saut de page PP soit en haut   | */
/* |            de la fenetre.                                          | */
/* |            VueDoc indique la vue concernee (vue du document).      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PageHautFenetre (PtrElement PP, int VueDoc, PtrDocument pDoc)
#else  /* __STDC__ */
void                PageHautFenetre (PP, VueDoc, pDoc)
PtrElement          PP;
int                 VueDoc;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrAbstractBox             gpav;
   PtrAbstractBox             pav;
   int                 frame;

   /* le code qui suit etait precedemment dans MoveToPage */
   if (PP != NULL)
     {
	VerifPave (PP, VueDoc, pDoc, TRUE, FALSE);
	if (PP->ElAbstractBox[VueDoc - 1] == NULL)
	   /* message 'Panique! * (VerifPave)' */
	   printf ("Panique VerifPave\n");
	else
	  {
	     pav = PP->ElAbstractBox[VueDoc - 1]->AbFirstEnclosed;
	     gpav = NULL;
	     while (pav != NULL)
		/* recherche du filet */
		if (pav->AbLeafType == LtGraphics
		    && pav->AbShape == 'h'
		    && pav->AbHorizPos.PosAbRef == NULL)
		  {
		     gpav = pav;
		     pav = NULL;
		  }
		else
		   pav = pav->AbNext;
	     if (gpav != NULL)
	       {
		  /* cherche la fenetre correspondant a la vue */
		  if (PP->ElAssocNum == 0)
		     frame = pDoc->DocViewFrame[VueDoc - 1];
		  else
		     frame = pDoc->DocAssocFrame[PP->ElAssocNum - 1];
		  MontrerBoite (frame, gpav->AbBox, 0, 0);
	       }
	  }
     }
}
/* End Of Module cherche */
