/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * Traitements specifiques a la structure Draw             
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"
#include "message.h"
#include "libmsg.h"
#include "constmedia.h"
#include "typemedia.h"

typedef int         Ensemble[4];

/* procedures importees de l'Editeur */
#include "tree_f.h"
#include "memory_f.h"
#include "exceptions_f.h"
#include "structselect_f.h"
/* procedures definies dans le module */
#include "draw_f.h"

/*----------------------------------------------------------------------
 AddAttribute   ajoute a l'element pointe' par pEl l'attribut
 numerique auquel est associe' le numero d'exception NumExcept dans
 le schema de structure pointe par pSS. Initialise cet attribut avec
 la valeur Val.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddAttribute (PtrElement pEl, int NumExcept, PtrSSchema pSS, int Val)
#else  /* __STDC__ */
static void         AddAttribute (pEl, NumExcept, pSS, Val)
PtrElement          pEl;
int                 NumExcept;
PtrSSchema          pSS;
int                 Val;
#endif /* __STDC__ */
{
   PtrAttribute        pAttr;
   int                 attr;
   boolean             existe;
   PtrElement          pEl1;
   PtrAttribute        pAt1;

   pEl1 = pEl;
   attr = GetAttrWithException (NumExcept, pSS);

   /* verifie que l'element ne porte pas deja cet attribut */
   pAttr = pEl1->ElFirstAttr;
   existe = FALSE;
   while (pAttr != NULL && !existe)
      if (pAttr->AeAttrSSchema == pSS && pAttr->AeAttrNum == attr)
	 existe = TRUE;
      else
	 pAttr = pAttr->AeNext;

   if (!existe)
      /* l'element ne porte pas l'attribut */
     {
	GetAttribute (&pAttr);
	/* acquiert un bloc attribut */
	/* chaine ce bloc en tete de la liste des attributs de l'element */
	pAttr->AeNext = pEl1->ElFirstAttr;
	pEl1->ElFirstAttr = pAttr;
	/* remplit le bloc attribut */
	pAt1 = pAttr;
	pAt1->AeAttrSSchema = pSS;
	pAt1->AeAttrNum = attr;
	pAt1->AeDefAttr = FALSE;
	pAt1->AeAttrType = pSS->SsAttribute[pAt1->AeAttrNum - 1].AttrType;
	pAt1->AeAttrValue = Val;
     }
}


/*----------------------------------------------------------------------
 DrawEtendSelection   verifie si l'element pointe' par pEl est un
 objet dans un dessin et, dans ce cas, ajoute cet objet a la
 selection courante et retourne vrai.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DrawEtendSelection (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
boolean             DrawEtendSelection (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   boolean             ret;

   ret = FALSE;
   if (pEl->ElParent != NULL)
      if (TypeHasException (ExcExtendedSelection, pEl->ElParent->ElTypeNumber, pEl->ElParent->ElStructSchema))
	 /* l'element est dans un Dessin ou dans un Groupe a l'interieur */
	 /* d'un dessin */
	{
	   AddInSelection (pEl, TRUE);
	   ret = TRUE;
	}
   return ret;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      CheckAttributes (Ensemble Except, PtrElement * pEl, PtrAttribute * pAttr)
#else  /* __STDC__ */
static boolean      CheckAttributes (Except, pEl, pAttr)
Ensemble            Except;
PtrElement         *pEl;
PtrAttribute       *pAttr;
#endif /* __STDC__ */
{
   boolean             trouve;
   int                 e, e1;
   PtrAttribute        pA, pASuiv;
   int                 i;
   PtrAttribute        pAt1;
   TtAttribute        *pAttr1;
   boolean             ret;

   /* L'attribut ajoute' est-il associe' a une des exceptions de */
   /* l'ensemble Except ? */
   trouve = FALSE;
   pAt1 = *pAttr;
   pAttr1 = &pAt1->AeAttrSSchema->SsAttribute[pAt1->AeAttrNum - 1];
   if (pAttr1->AttrFirstExcept != 0)
      /* il y a des exceptions pour cet attribut */
     {
	e = pAttr1->AttrFirstExcept;
	do
	  {
	     for (i = 1; i <= 4; i++)
		if (pAt1->AeAttrSSchema->SsException[e - 1] == Except[i - 1])
		   trouve = TRUE;
	     e++;
	  }
	while (!(trouve || e > pAttr1->AttrLastExcept));
     }

   if (trouve)
      /* l'attribut ajoute' fait partie des attributs consideres */
      /* parcourt la liste des attributs portes par l'element */
     {
	pA = (*pEl)->ElFirstAttr;
	while (pA != NULL)
	  {
	     pASuiv = pA->AeNext;
	     /* on ne traite pas le nouvel attribut */
	     if (pA != *pAttr)
		/* l'attribut courant est-il exclusif du nouvel attribut ? */
		/* cherche les exceptions de l'attribut courant */
	       {
		  trouve = FALSE;
		  pAt1 = pA;
		  pAttr1 = &pAt1->AeAttrSSchema->SsAttribute[pAt1->AeAttrNum - 1];
		  if (pAttr1->AttrFirstExcept != 0)
		     /* il y a des exceptions pour cet attribut */
		    {
		       e1 = pAttr1->AttrFirstExcept;
		       do
			 {
			    for (i = 1; i <= 4; i++)
			       if (pAt1->AeAttrSSchema->SsException[e1 - 1] == Except[i - 1])
				  trouve = TRUE;
			    e1++;
			 }
		       while (!(trouve || e1 > pAttr1->AttrLastExcept));
		    }
		  if (trouve)
		     /* l'attribut courant est exclusif du nouvel attribut, on */
		     /* le supprime */
		     DeleteAttribute (*pEl, pA);
	       }
	     pA = pASuiv;
	     /* passe a l'attribut suivant de l'element */
	  }
	ret = TRUE;
     }
   else
      /* Le nouvel attribut ne fait pas partie des attributs consideres */
      ret = FALSE;
   return ret;
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
 DrawAddAttr   on a ajoute' l'attribut pointe' par pAttr a l'element
 pointe' par pEl. Dans le cas d'un objet Draw, verifie la coherence
 des attributs porte's par l'element et retourne pAttr=NULL si
 l'attribut etait incorrect.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawAddAttr (PtrAttribute * pAttr, PtrElement pEl)
#else  /* __STDC__ */
void                DrawAddAttr (pAttr, pEl)
PtrAttribute       *pAttr;
PtrElement          pEl;
#endif /* __STDC__ */
{
   boolean             ok;
   Ensemble            Ens;
   int                 ex;
   PtrAttribute        pA, pAS;
   PtrAttribute        pAt1;
   TtAttribute        *pAttr1;


   if (pEl->ElParent != NULL)
      if (TypeHasException (ExcIsDraw, pEl->ElParent->ElTypeNumber, pEl->ElParent->ElStructSchema))
	 /* l'element est dans un Dessin ou dans un Groupe a l'interieur */
	 /* d'un dessin */
	 /* Les quatre attributs de position verticale (PositionV, */
	 /* Alignement_haut, Centr\37_horiz et Alignement_bas) sont */
	 /* mutuellement exclusifs. Verifie si le nouvel attribut est l'un de */
	 /* ceux la et dans ce cas supprime les autres attributs de position */
	 /* verticale qu'il pourrait avoir. */
	{
	   Ens[0] = 351;
	   Ens[1] = 355;
	   Ens[2] = 356;
	   Ens[3] = 357;
	   ok = CheckAttributes (Ens, &pEl, pAttr);
	   if (!ok)
	      /* Le nouvel attribut n'est pas un attribut de position verticale. */
	      /* Verifie si c'est un attribut de position horizontale et dans ce */
	      /* cas supprime de l'element les autres attributs de position */
	      /* horizontale qu'il pourrait avoir. */
	     {
		Ens[0] = 352;
		Ens[1] = 358;
		Ens[2] = 359;
		Ens[3] = 360;
		ok = CheckAttributes (Ens, &pEl, pAttr);
		if (!ok)
		   /* Le nouvel attribut est-il un attribut de boite elastique ? */
		   if (AttrHasException (361, (*pAttr)->AeAttrNum, (*pAttr)->AeAttrSSchema) ||
		       AttrHasException (362, (*pAttr)->AeAttrNum, (*pAttr)->AeAttrSSchema))
		      /* c'est un attribut de boite elastique */
		      /* Verifie que la boite elastique ne fait pas reference a */
		      /* elle-meme */
		     {
			ok = TRUE;
			pAt1 = *pAttr;
			if (pAt1->AeAttrType == AtReferenceAttr)
			   if (pAt1->AeAttrReference != NULL)
			      if (pAt1->AeAttrReference->RdReferred != NULL)
				 if (!pAt1->AeAttrReference->RdReferred->ReExternalRef)
				    if (pAt1->AeAttrReference->RdReferred->ReReferredElem != NULL)
				       if (pAt1->AeAttrReference->RdReferred->ReReferredElem == pEl)
					 {
					    TtaDisplaySimpleMessage (INFO, LIB, TMSG_SELF_REF_IMP);
					    ok = FALSE;
					 }
			/* message 'CsReference a soi-meme' */
			if (!ok)
			   /* l'attribut est incorrect, on le supprime */
			  {
			     DeleteAttribute (pEl, *pAttr);
			     *pAttr = NULL;
			  }
			else
			   /* L'attribut est correct, on enleve tous les attributs de */
			   /* position ou de dimension de l'element */
			   /* parcourt la liste des attributs portes par l'element */
			  {
			     pA = pEl->ElFirstAttr;
			     while (pA != NULL)
			       {
				  pAS = pA->AeNext;
				  /* attribut a traiter ensuite */
				  /* on ne traite pas le nouvel attribut */
				  if (pA != *pAttr)
				     /* cherche les exceptions de l'attribut courant */
				    {
				       pAt1 = pA;
				       pAttr1 = &pAt1->AeAttrSSchema->SsAttribute[pAt1->AeAttrNum - 1];
				       if (pAttr1->AttrFirstExcept != 0)
					  /* il y a des exceptions pour cet attribut */
					 {
					    ex = pAttr1->AttrFirstExcept;
					    do
					       if (pAt1->AeAttrSSchema->SsException[ex - 1] >= 351
						   && pAt1->AeAttrSSchema->SsException[ex - 1] <= 360)
						  /* l'attribut a une des exception  identifiant un */
						  /* attribut a supprimer, on supprime l'attribut */
						 {
						    DeleteAttribute (pEl, pA);
						    pA = NULL;
						 }
					       else
						  ex++;
					    while (!(pA == NULL || ex > pAttr1->AttrLastExcept));
					 }
				    }
				  pA = pAS;
			       }
			  }
		     }
	     }
	}
}
#endif /* _WIN_PRINT */

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      CheckRemovedAttribute (int Def, Ensemble Except, PtrElement * pEl, PtrAttribute * pAttr)

#else  /* __STDC__ */
static boolean      CheckRemovedAttribute (Def, Except, pEl, pAttr)
int                 Def;
Ensemble            Except;
PtrElement         *pEl;
PtrAttribute       *pAttr;
#endif /* __STDC__ */
{
   boolean             trouve;
   int                 e, e1;
   PtrAttribute        pA;
   int                 i;
   PtrAttribute        pAt1;
   TtAttribute        *pAttr1;
   boolean             ret;

   /* L'attribut supprime' est-il associe' a une des exceptions de */
   /* l'ensemble Except ? */
   trouve = FALSE;
   pAt1 = *pAttr;
   if (pAt1->AeAttrNum < 1)
      return FALSE;
   pAttr1 = &pAt1->AeAttrSSchema->SsAttribute[pAt1->AeAttrNum - 1];
   if (pAttr1->AttrFirstExcept != 0)
      /* il y a des exceptions pour cet attribut */
     {
	e = pAttr1->AttrFirstExcept;
	do
	  {
	     for (i = 1; i <= 4; i++)
		if (pAt1->AeAttrSSchema->SsException[e - 1] == Except[i - 1])
		   trouve = TRUE;
	     e++;
	  }
	while (!(trouve || e > pAttr1->AttrLastExcept));
     }
   if (trouve)
      /* l'attribut supprime' fait partie des attributs consideres */
      /* parcourt la liste des attributs portes par l'element */
     {
	trouve = FALSE;
	pA = (*pEl)->ElFirstAttr;
	while (pA != NULL && !trouve)
	   /* l'attribut courant fait-il partie des attributs cherches ? */
	   /* cherche les exceptions de l'attribut courant */
	  {
	     pAt1 = pA;
	     pAttr1 = &pAt1->AeAttrSSchema->SsAttribute[pAt1->AeAttrNum - 1];
	     if (pAttr1->AttrFirstExcept != 0)
		/* il y a des exceptions pour cet attribut */
	       {
		  e1 = pAttr1->AttrFirstExcept;
		  do
		    {
		       for (i = 1; i <= 4; i++)
			  if (pAt1->AeAttrSSchema->SsException[e1 - 1] == Except[i - 1])
			     trouve = TRUE;
		       e1++;
		    }
		  while (!(trouve || e1 > pAttr1->AttrLastExcept));
	       }
	     if (!trouve)
		/* l'attribut courant n'est pas l'un des attributs cherches */
		pA = pA->AeNext;
	     /* passe a l'attribut suivant */
	  }
	if (!trouve)
	   /* aucun des attributs cherches n'est porte' par l'element. */
	   /* On met l'attribut par defaut avec la valeur nulle. */
	   AddAttribute (*pEl, Def, (*pAttr)->AeAttrSSchema, 0);
	ret = TRUE;
     }
   else
      /* Le nouvel attribut ne fait pas partie des attributs consideres */
      ret = FALSE;
   return ret;
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
 DrawSupprAttr        on a supprime' l'attribut pointe' par pAttr a
 l'element pointe' par pEl. Dans le cas d'un objet Draw, verifie la
 coherence des attributs porte's par l'element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawSupprAttr (PtrAttribute pAttr, PtrElement pEl)
#else  /* __STDC__ */
void                DrawSupprAttr (pAttr, pEl)
PtrAttribute        pAttr;
PtrElement          pEl;
#endif /* __STDC__ */
{
   boolean             ok;
   Ensemble            Ens;


   if (pEl != NULL)
      if (pEl->ElParent != NULL)
	 if (TypeHasException (ExcIsDraw, pEl->ElParent->ElTypeNumber, pEl->ElParent->ElStructSchema))
	    /* l'element est dans un Dessin ou dans un Groupe a l'interieur */
	    /* d'un dessin */
	    /* Un au moins des attributs suivants doit etre present : PositionV, */
	    /* Alignement_haut, Centr\37_horiz et Alignement_bas. Verifie si on a */
	    /* supprime' l'un de ces attributs et dans ce cas remet l'attribut */
	    /* PositionV. */
	   {
	      Ens[0] = 351;
	      Ens[1] = 355;
	      Ens[2] = 356;
	      Ens[3] = 357;
	      ok = CheckRemovedAttribute (351, Ens, &pEl, &pAttr);
	      if (!ok)
		 /* Ce n'est pas un attribut de position verticale qui a ete */
		 /* supprime'. Verifie si c'est un attribut de position horizontale, */
		 /* et dans ce cas remet l'attribut PositionH. */
		{
		   Ens[0] = 352;
		   Ens[1] = 358;
		   Ens[2] = 359;
		   Ens[3] = 360;
		   ok = CheckRemovedAttribute (352, Ens, &pEl, &pAttr);
		}
	   }
}
#endif /* WIN_PRINT */
