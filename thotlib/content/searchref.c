
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   cherche.c : gestion de la commande de recherche.
   Projet THOT
   Module traitant la commande de recherche des references provenant de cherche.c
   I. Vatton    Avril 1996      
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"
#include "storage.h"
#include "appaction.h"
#include "app.h"
#include "message.h"
#include "typecorr.h"
#include "appdialogue.h"

#define EXPORT extern
#include "environ.var"
#include "edit.var"
#include "modif.var"
#include "appdialogue.var"

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
#include "refext.f"
#include "schemas.f"
#include "sel.f"
#include "select.f"
#include "storage.f"
#include "structure.f"

#define MaxEntreesMenuRef 10

/* ---------------------------------------------------------------------- */
/* |    ReferSuivante retourne la prochaine reference qui designe       | */
/* |            l'element pEl.                                          | */
/* |            - pDocEl est le document auquel appartient pEl.         | */
/* |            - TraiteNonCharge indique si on prend en compte les     | */
/* |              documents referencant non charge's (True) ou si au    | */
/* |              contraire on les ignore (False).                      | */
/* |            - pRefPrec contient la reference courante a l'appel.    | */
/* |              Si pRefPrec est NULL et *pDE est NULL, la fonction    | */
/* |              retourne la premiere reference a` l'element pEl,sinon | */
/* |              elle retourne la reference qui suit celle qui est     | */
/* |              pointee par pRefPrec.                                 | */
/* |            - pDocRef contient au retour un pointeur sur le         | */
/* |              contexte du document auquel appartient la reference   | */
/* |              retournee. Seulement si la valeur de retour n'est pas | */
/* |              NULL.                                                 | */
/* |            - pDE est le document externe precedemment traite' (si  | */
/* |              DocExtSuivant est True) ou celui qu'on veut traiter   | */
/* |              (si DocExtSuivant est False). pDE doit etre NULL si   | */
/* |              DocExtSuivant est True et qu'on n'a pas encore traite'| */
/* |              de references externes.                               | */
/* |              Au retour, pDE vaut NULL si la reference retournee    | */
/* |              appartient au meme document que l'element pEl; sinon  | */
/* |              pDE est le document externe auquel appartient la      | */
/* |              reference trouvee. pDE ne doit pas etre modifie'      | */
/* |              entre les appels successifs de la fonction.           | */
/* |            - DocExtSuivant indique si on passe au document         | */
/* |              referencant suivant celui decrit par pDE (True) ou si | */
/* |              on traite le document decrit par pDE (False).         | */
/* |            Retourne un pointeur sur la premiere reference trouvee. | */
/* |            Si la valeur de retour est NULL et                      | */
/* |               si pDE est NULL : on n'a rien trouve'.               | */
/* |               si pDE n'est pas NULL : il y a des references a`     | */
/* |                    l'element pEl dans le document designe' par pDE | */
/* |                    mais ce document n'est pas charge' (cela ne se  | */
/* |                    produit que si TraiteNonCharge est True).       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
PtrReference        ReferSuivante (PtrElement pEl, PtrDocument pDocEl, boolean TraiteNonCharge, PtrReference pRefPrec, PtrDocument * pDocRef, PtrExternalDoc * pDE, boolean DocExtSuivant)
#else  /* __STDC__ */
PtrReference        ReferSuivante (pEl, pDocEl, TraiteNonCharge, pRefPrec, pDocRef, pDE, DocExtSuivant)
PtrElement          pEl;
PtrDocument         pDocEl;
boolean             TraiteNonCharge;
PtrReference        pRefPrec;
PtrDocument        *pDocRef;
PtrExternalDoc      *pDE;
boolean             DocExtSuivant;

#endif /* __STDC__ */
{
   PtrReference        pRef;

   if (pRefPrec != NULL)
      pRef = pRefPrec->RdNext;
   else
     {
	pRef = NULL;
	if (*pDE == NULL)
	   /* premier appel de la fonction */
	   if (pEl != NULL)
	      if (pEl->ElReferredDescr != NULL)
		{
		   pRef = pEl->ElReferredDescr->ReFirstReference;
		   *pDocRef = pDocEl;
		}
     }
   if (pRef == NULL)
      /* c'etait la derniere reference dans ce document, cherche dans */
      /* un autre document la 1ere reference au meme element */
      pRef = ChRefDocExt (pEl, pDocEl, TraiteNonCharge, pDocRef, pDE,
			  DocExtSuivant);
   return pRef;
}


/* ---------------------------------------------------------------------- */
/* |    ChUneRef cherche une reference a` l'element selectionne'.       | */
/* |       A l'appel:                                                   | */
/* |            - ReferCour: pointeur sur la derniere reference trouvee | */
/* |              ou NULL si on cherche la premiere reference (dans ce  | */
/* |              cas, pDocExtCour doit aussi etre NULL).               | */
/* |            - ElemRefCour: l'element dont on cherche les references,| */
/* |              NULL si on cherche la premiere reference.             | */
/* |            - DocDeElemRefCour: le document auquel appartient       | */
/* |              l'element dont on cherche les references, NULL si on  | */
/* |              cherche la premiere reference.                        | */
/* |            - pDocExtCour: pointeur sur le descripteur de document  | */
/* |              externe contenant la reference ReferCour, NULL si on  | */
/* |              cherche la premiere reference.                        | */
/* |            - DocExtSuivant indique si on cherche une reference     | */
/* |              dans le document externe decrit par pDocExtCour       | */
/* |              (False) ou dans le document externe suivant.          | */
/* |       Au retour:                                                   | */
/* |            - ReferCour: pointeur sur la reference trouvee ou NULL  | */
/* |              si on n'a pas trouve' de reference.                   | */
/* |            - DocDeReferCour: document auquel appartient la         | */
/* |              reference trouvee.                                    | */
/* |            - ElemRefCour: l'element dont on cherche les references.| */
/* |            - DocDeElemRefCour: le document auquel appartient       | */
/* |              l'element dont on cherche les references.             | */
/* |            - pDocExtCour: document externe dans lequel on a trouve'| */
/* |              une reference. Peut etre non NULL meme si ReferCour   | */
/* |              est NULL, dans le cas ou ce document externe n'est    | */
/* |              pas charge':                                          | */
/* |            Si ReferCour est NULL et                                | */
/* |                si pDocExtCour est NULL : on n'a rien trouve'.      | */
/* |                si pDocExtCour n'est pas NULL : il y a des          | */
/* |                    references dans le document designe' par        | */
/* |                    pDocExtCour, mais ce document n'est pas charge' | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                ChUneRef (PtrReference * ReferCour, PtrDocument * DocDeReferCour, PtrElement * ElemRefCour, PtrDocument * DocDeElemRefCour, PtrExternalDoc * pDocExtCour, boolean DocExtSuivant)
#else  /* __STDC__ */
void                ChUneRef (ReferCour, DocDeReferCour, ElemRefCour, DocDeElemRefCour, pDocExtCour, DocExtSuivant)
PtrReference       *ReferCour;
PtrDocument        *DocDeReferCour;
PtrElement         *ElemRefCour;
PtrDocument        *DocDeElemRefCour;
PtrExternalDoc      *pDocExtCour;
boolean             DocExtSuivant;

#endif /* __STDC__ */
{
   PtrElement          premsel;
   PtrElement          dersel;
   PtrElement          pEl;
   PtrDocument         docsel;
   int                 premcar;
   int                 dercar;
   PtrElement          pAscendant;
   boolean             ok;
   boolean             trouve;

   ok = True;
   if (*ReferCour != NULL || *pDocExtCour != NULL)
      /* on a deja une reference courante */
      *ReferCour = ReferSuivante (*ElemRefCour, *DocDeElemRefCour, True,
		    *ReferCour, DocDeReferCour, pDocExtCour, DocExtSuivant);
   else
      /* pas de reference courante */
     {
	/* prend la selection courante */
	ok = SelEditeur (&docsel, &premsel, &dersel, &premcar, &dercar);
	if (!ok)
	   /* pas de selection, message 'Selectionnez' */
	   TtaDisplaySimpleMessage (LIB, INFO, LIB_SELECT_AN_ELEM);
	else
	   /* cherche le premier element reference' qui englobe la */
	   /* selection courante */
	  {
	     pEl = premsel;
	     trouve = False;
	     do
	       {
		  if (pEl->ElReferredDescr != NULL)
		     /* l'element a un descripteur d'element reference' */
		     if (pEl->ElReferredDescr->ReFirstReference != NULL ||
			 pEl->ElReferredDescr->ReExtDocRef != NULL)
			/* l'element est effectivement reference' */
			trouve = True;
		  if (!trouve)
		     /* l'element n'est pas reference', on passe au pere */
		     pEl = pEl->ElParent;
	       }
	     while (!trouve && pEl != NULL);
	     if (trouve)
		/* on a trouve' un ascendant reference' */
	       {
		  /* conserve un pointeur sur l'element dont on cherche les */
		  /* references ainsi que sur son document */
		  *ElemRefCour = pEl;
		  *DocDeElemRefCour = docsel;
		  /* cherche la premiere reference a cet element */
		  *ReferCour = ReferSuivante (*ElemRefCour, *DocDeElemRefCour, True,
		    *ReferCour, DocDeReferCour, pDocExtCour, DocExtSuivant);
	       }
	  }
     }
   if (*ReferCour != NULL)
      /* on a trouve' une reference */
     {
	ok = True;
	/* on ignore les references qui sont a l'interieur d'une inclusion */
	if ((*ReferCour)->RdElement != NULL)
	  {
	     pAscendant = (*ReferCour)->RdElement->ElParent;
	     while (pAscendant->ElParent != NULL && pAscendant->ElSource == NULL)
		pAscendant = pAscendant->ElParent;
	     if (pAscendant->ElSource != NULL)
		/* on est dans une inclusion */
		ok = False;
	  }
	/* on ignore les references dans les partie cachees */
	if (ok)
	   if (ElemHidden ((*ReferCour)->RdElement))
	      /* on est dans une partie cachee */
	      ok = False;
	/* on ignore les references qui sont dans le tampon de Copier-Couper */
	if (ok)
	   if (DansTampon ((*ReferCour)->RdElement))
	      ok = False;
	if (!ok)
	   /* cherche la reference suivante au meme element */
	   ChUneRef (ReferCour, DocDeReferCour, ElemRefCour, DocDeElemRefCour,
		     pDocExtCour, True);
	else
	   /* selectionne la reference trouvee */
	   SelectWithAPP (*DocDeReferCour, (*ReferCour)->RdElement, False, False);
     }
}


/* ---------------------------------------------------------------------- */
/* |    ChElemRefer cherche l'element qui est reference' par la         | */
/* |            reference selectionnee ou par un attribut reference du  | */
/* |            premier element selectionne'.                           | */
/* ---------------------------------------------------------------------- */
void                ChElemRefer ()
{
   PtrElement          premsel;
   PtrElement          dersel;
   PtrElement          pEl;
   PtrDocument         docsel;
   int                 premcar;
   int                 dercar;
   PtrAttribute         pAttr;
   PtrReference        pRef;
   DocumentIdentifier     IdentDoc;
   PtrDocument         pDoc;
   int                 nbEntrees;
   char                bufMenu[MAX_TXT_LEN];
   int                 lgBufMenu;
   PtrReference        TablePRef[MaxEntreesMenuRef];
   int                 entreeChoisie;

   /* y-a-t'il une selection au niveau editeur ou mediateur ? */
   if (!SelEditeur (&docsel, &premsel, &dersel, &premcar, &dercar))
      TtaDisplaySimpleMessage (LIB, INFO, LIB_SELECT_AN_ELEM);
   else
     {
	pEl = NULL;		/* pointeur sur l'element reference' */
	pRef = NULL;		/* pointeur sur le bloc reference */
	NulIdentDoc (&IdentDoc);
	nbEntrees = 0;
	lgBufMenu = 0;
	if (premsel->ElTerminal && premsel->ElLeafType == LtReference)
	   /* la selection est un element reference */
	   if (premsel->ElReference != NULL && nbEntrees < MaxEntreesMenuRef &&
	       lgBufMenu + MAX_NAME_LENGTH < MAX_TXT_LEN)
	     {
		TablePRef[nbEntrees] = premsel->ElReference;
		nbEntrees++;
		strcpy (bufMenu + lgBufMenu, premsel->ElSructSchema->SsRule[premsel->ElTypeNumber - 1].SrName);
		lgBufMenu += strlen (bufMenu) + 1;
	     }
	if (premsel->ElSource != NULL)
	   /* la selection est un element inclus */
	   if (premsel->ElSource != NULL && nbEntrees < MaxEntreesMenuRef &&
	       lgBufMenu + MAX_NAME_LENGTH < MAX_TXT_LEN)
	     {
		TablePRef[nbEntrees] = premsel->ElSource;
		nbEntrees++;
	    /****
	    strcpy(bufMenu+lgBufMenu, TtaGetMessage(LIB_APPLI, LIB_Inclusion));
	    lgBufMenu += strlen(TtaGetMessage(LIB_APPLI, LIB_Inclusion))+1;
	    *******/
	     }

	/* cherche si l'element a un attribut reference */
	pAttr = premsel->ElFirstAttr;
	while (pAttr != NULL)
	  {
	     if (pAttr->AeAttrType == AtReferenceAttr)
		/* c'est un attribut reference */
		if (ExceptAttr (ExcActiveRef, pAttr->AeAttrNum, pAttr->AeAttrSSchema))
		   /* cet attribut a l'exception ActiveRef, on le prend */
		   if (pAttr->AeAttrReference != NULL)
		     {
			TablePRef[nbEntrees] = pAttr->AeAttrReference;
			nbEntrees++;
			strcpy (bufMenu + lgBufMenu, pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrName);
			lgBufMenu += strlen (pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrName) + 1;
		     }
	     pAttr = pAttr->AeNext;
	  }
	if (nbEntrees == 0)
	   pRef = NULL;
	else if (nbEntrees == 1)
	   pRef = TablePRef[0];
	else
	  {
	     CreeEtActiveMenuReferences (bufMenu, nbEntrees, &entreeChoisie);
	     pRef = TablePRef[entreeChoisie];
	  }
	if (pRef != NULL)
	   /* c'est bien une reference qui est selectionnee */
	  {
	     /* l'element reference' est pointe' par pEl */
	     pEl = ElemRefer (pRef, &IdentDoc, &pDoc);
	     if (pEl == NULL)
		/* il n'y a pas d'element reference' */
		if (IdentDocNul (IdentDoc) || pDoc != NULL)
		   /* ce n'est pas une reference externe ou c'est une reference vide */
		   TtaDisplaySimpleMessage (LIB, INFO, LIB_EMPTY_REF);
		else
		   /* l'element reference' est dans un autre document qui n'est */
		   /* pas charge' */
		   /* on proposera ce nom comme nom par defaut lorsque */
		   /* l'utilisateur demandera a ouvrir un document */
		  {
		     CreeDocument (&pDoc);
		     if (pDoc != NULL)
		       {
			  CopyIdentDoc (&pDoc->DocIdent, IdentDoc);
			  /* annule la selection */
			  RazSelect ();
			  LoadDocument (&pDoc, NULL);
		       }
		     if (pDoc != NULL)
			/* le chargement du document a reussi */
		       {
			  pEl = ElemRefer (pRef, &IdentDoc, &pDoc);
			  /* s'il s'agit d'une inclusion de */
			  /* document, applique les regles Transmit */
			  ApplReglesTransmit (pRef->RdElement, docsel);

		       }
		  }
	     else
		/* l'element reference est-il dans le buffer de sauvegarde ? */
	     if (DansTampon (pEl))
	       {
		  pEl = NULL;
		  /* message 'CsReference vide' */
		  TtaDisplaySimpleMessage (LIB, INFO, LIB_EMPTY_REF);
	       }
	  }
	if (pEl != NULL)
	  {
	     if (!IdentDocNul (IdentDoc))
		/* l'element reference' est dans un autre document */
		docsel = pDocument (IdentDoc);
	     SelectWithAPP (docsel, pEl, True, True);
	     /* dans le cas ou c'est un element d'une paire de marques, on */
	     /* selectionne l'intervalle compris entre ces marques. */
	     SelectIntervallePaire ();
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    NoteElemRefColle        L'element pointe' par pEl vient d'etre  | */
/* |    colle' dans le document pDoc et l'original vient d'un document  | */
/* |    different. S'il possede des elements reference's par d'autres   | */
/* |    documents, on note dans le contexte du document pDoc que cet    | */
/* |    element a change' de document. Ce sera utile lorsqu'on sauvera  | */
/* |    le document pDoc pour mettre a jour les fichiers .REF des       | */
/* |    documents referencant les elements deplace's.                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         NoteElemRefColle (PtrElement pEl, PtrDocument pDoc, LabelString AncienLabel)
#else  /* __STDC__ */
static void         NoteElemRefColle (pEl, pDoc, AncienLabel)
PtrElement          pEl;
PtrDocument         pDoc;
LabelString         AncienLabel;

#endif /* __STDC__ */
{
   PtrChangedReferredEl      ElemRef;

   if (pEl->ElReferredDescr != NULL)
      if (pEl->ElReferredDescr->ReExtDocRef != NULL)
	 /* l'element est reference' par d'autres documents */
	{
	   /* acquiert un descripteur et le remplit */
	   GetElemRefChng (&ElemRef);
	   strncpy (ElemRef->CrOldLabel, AncienLabel, MAX_LABEL_LEN);
	   strncpy (ElemRef->CrNewLabel, pEl->ElLabel, MAX_LABEL_LEN);
	   CopyIdentDoc (&ElemRef->CrOldDocument, DocDeSauve->DocIdent);
	   CopyIdentDoc (&ElemRef->CrNewDocument, pDoc->DocIdent);
	   ElemRef->CrReferringDoc = NULL;
	   /* chaine ce descripteur */
	   ElemRef->CrNext = pDoc->DocChangedReferredEl;
	   pDoc->DocChangedReferredEl = ElemRef;
	   /* copie la liste des documents qui referencent l'element */
	   CopieDocExt (pEl, ElemRef);
	}
}


/* ---------------------------------------------------------------------- */
/* |    PurgeRef        On vient de coller le sous-arbre de racine pRac | */
/* |    dans le document pDoc. Verifie la coherence des elements        | */
/* |    reference's et des references presents dans ce sous-arbre.      | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                PurgeRef (PtrElement pRac, PtrDocument pDoc)
#else  /* __STDC__ */
void                PurgeRef (pRac, pDoc)
PtrElement          pRac;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pEl, pElRef, pSource;
   PtrReference        pRef, pRefSuiv;
   PtrAttribute         pAttr;
   DocumentIdentifier     IdentDocRef;
   PtrDocument         pDocRef, pDocRefSuiv;
   boolean             memedoc, attrref;
   PtrExternalDoc       pDocExt;
   PtrElement          pElemRef;
   int                 l;
   LabelString         AncienLabel;
   PtrReference        pRefTampon;

   /* on se souvient du label de l'original */
   strncpy (AncienLabel, pRac->ElLabel, MAX_LABEL_LEN);
   if (ChangerLabel || pDoc != DocDeSauve)
     {
	/* on affecte un nouveau label a l'element */
	l = NewLabel (pDoc);
	LabelIntToString (l, pRac->ElLabel);
     }
   if (pRac->ElReferredDescr != NULL)
      /* l'original de l'element colle' est reference' ou possede un label */
     {
	if (pRac->ElReferredDescr->ReExternalRef)
	   pSource = NULL;
	else
	   pSource = pRac->ElReferredDescr->ReReferredElem;	/* l'element original */

	pDocExt = NULL;
	pRac->ElReferredDescr = NULL;
	/* l'element colle' n'est pas reference', il prend le label de son     */
	/* element original (celui qui est dans le tampon Couper-Copier)       */
	/* si la commande precedente etait Couper et s'il ne change pas de     */
	/* document. Dans les autres cas (commande precedente Copier ou        */
	/* changement de document), il prend un nouveau label.                 */

	/* alloue a l'element un descripteur d'element reference' */
	pRac->ElReferredDescr = NewRef (pDoc);
	pRac->ElReferredDescr->ReExternalRef = False;
	pRac->ElReferredDescr->ReReferredElem = pRac;
	if (!ChangerLabel && pSource != NULL && pDoc == DocDeSauve)
	   /* l'element prend le meme label que l'element original */
	   strncpy (pRac->ElLabel, pSource->ElLabel, MAX_LABEL_LEN);
	if (!ChangerLabel && pSource != NULL)
	  {
	     /* l'element prend les memes descripteurs de documents */
	     /* externes referencants que l'element original */
	     pDocExt = pSource->ElReferredDescr->ReExtDocRef;
	     while (pDocExt != NULL)
	       {
		  /* on ne considere pas le document lui-meme comme externe... */
		  if (!MemeIdentDoc (pDocExt->EdDocIdent, pDoc->DocIdent))
		     AjDocRefExt (pRac, pDocExt->EdDocIdent, pDoc);
		  pDocExt = pDocExt->EdNext;
	       }
	  }

	/* on cherche toutes les references a l'element original et on les */
	/* fait pointer sur l'element colle'. */
	/* cherche d'abord la premiere reference */
	pRef = ReferSuivante (pSource, DocDeSauve, False, NULL, &pDocRef, &pDocExt, True);
	pDocRefSuiv = pDocRef;	/* a priori la reference suivante sera dans le */
	/* meme document */
	while (pRef != NULL)
	  {
	     /* cherche la reference suivante a l'original avant de modifier */
	     /* la reference courante */
	     pRefSuiv = ReferSuivante (pSource, DocDeSauve, False, pRef, &pDocRefSuiv, &pDocExt, True);
	     /* traite la reference courante */
	     /* si elle est dans le tampon, on n'y touche pas : sa copie dans */
	     /* le document ou on colle a deja ete traitee ou sera traitee dans */
	     /* cette boucle */
	     if (!DansTampon (pRef->RdElement))
		/* on fait pointer la reference sur l'element colle' */
	       {
		  if (DansNouveaux (pRef->RdElement))
		     pDocRef = pDoc;
		  if (pRef->RdAttribute != NULL)
		     pElemRef = NULL;
		  else
		     pElemRef = pRef->RdElement;
		  LieReference (pElemRef, pRef->RdAttribute, pRac, pDocRef, pDoc, False, False);
		  /* si c'est une reference par attribut, verifie la */
		  /* validite de l'attribut dans le cas des extensions de */
		  /* cellule des tableaux */
		  if (pRef->RdAttribute != NULL)
		    if (ThotLocalActions[T_TableauVerifExtension]!=NULL)
		      (*ThotLocalActions[T_TableauVerifExtension])
			(pRef->RdAttribute, pRef->RdElement, pRef->RdElement, True);
	       }
	     pRef = pRefSuiv;	/* passe a la reference suivante */
	     pDocRef = pDocRefSuiv;
	  }
	if (!ChangerLabel && pDoc != DocDeSauve)
	   /* l'element a change' de document, on le note dans le */
	   /* contexte de son nouveau document */
	   NoteElemRefColle (pRac, pDoc, AncienLabel);
     }
   /* Traite les attributs de type reference porte's par l'element */
   pAttr = pRac->ElFirstAttr;
   while (pAttr != NULL)
      /* examine tous les attributs de l'element */
     {
	if (pAttr->AeAttrType == AtReferenceAttr)
	   /* c'est un attribut de type reference */
	   /* cherche l'element reference', pElRef */
	  {
	     pElRef = NULL;
	     if (pAttr->AeAttrReference != NULL)
		if (pAttr->AeAttrReference->RdReferred != NULL)
		   if (!pAttr->AeAttrReference->RdReferred->ReExternalRef)
		      pElRef = pAttr->AeAttrReference->RdReferred->ReReferredElem;
	     /* si l'element reference' est aussi colle', */
	     /* on ne fait rien: ce cas est traite' plus haut */
	     if (pElRef != NULL)
		if (!DansTampon (pElRef))
		   if (!DansNouveaux (pElRef))
		     {
			/* verifie la validite de l'attribut dans le cas des */
			/* extensions de cellule des tableaux */
		        if (ThotLocalActions[T_TableauVerifExtension]!=NULL)
			  (*ThotLocalActions[T_TableauVerifExtension])
			   (pAttr, pRac, pRac, True);
			if (DocDeSauve != pDoc)
			   /* reference et objet reference' sont */
			   /* dans des documents differents, on */
			   /* supprime l'attribut, sauf dans le */
			   /* cas particulier des tableaux. */
			  {
			    attrref = True;
			    if (ThotLocalActions[T_TableauAttributRef]!=NULL)
			      (*ThotLocalActions[T_TableauAttributRef])
				(pAttr, &attrref);
			    if (!attrref)
			      AttrSupprime (pRac, pAttr);
			  }
		     }
	  }
	pAttr = pAttr->AeNext;
     }
   /* Lorsqu'on fait un copier/coller d'un element  LtReference, */
   /* il faut chainer les references entre element ancien et */
   /* element nouveau : la reference de la copie qui est dans */
   /* le tampon est dechainee entre les deux elements. */
   /* pRac est le nouvel element cree : son descripteur */
   /* a ete chaine devant le descripteur de la copie qui est */
   /* dans le tampon, qui est devant celui de l'element copie */
   if (pRac->ElTerminal && pRac->ElLeafType == LtReference)
     {
	/* c'est un element reference */
	pRef = pRac->ElReference;
	if (pRef != NULL)	/*  on cherche le suivant */
	  {
	     pRefTampon = pRef->RdNext;
	     if (pRefTampon != NULL
		 && DansTampon (pRefTampon->RdElement))
		/* il faut retirer pRefTampon du chainage */
	       {
		  pRef->RdNext = pRefTampon->RdNext;
		  if (pRefTampon->RdNext != NULL)	/* toujours vrai ? */
		     pRefTampon->RdNext->RdPrevious = pRef;
	       }
	  }
     }

   /* on n'a pas de traitement particulier a faire sur les references si */
   /* la copie est dans le meme document que l'original */
   if (DocDeSauve != pDoc)
     {
	/* l'element traite' est-il une reference ? */
	if (pRac->ElTerminal && pRac->ElLeafType == LtReference)
	   /* c'est un element reference */
	   pRef = pRac->ElReference;
	else			/* c'est peut-etre une inclusion */
	   pRef = pRac->ElSource;
	if (pRef != NULL)	/* c'est une reference, on cherche
				   l'element reference', pElRef */
	   if (pRef->RdReferred != NULL)
	     {
		pElRef = ElemRefer (pRef, &IdentDocRef, &pDocRef);
		if (pElRef == NULL && IdentDocNul (IdentDocRef))
		   /* la reference ne designe rien */
		   pRef->RdReferred = NULL;
		else
		  {
		     if (pElRef != NULL)
			if (!DansTampon (pElRef))
			   if (DansNouveaux (pElRef))
			      /* l'element reference' est aussi colle' */
			      pDocRef = pDoc;
		     if (pRef->RdInternalRef)
			/* l'original etait une reference interne a son document */
			/* d'origine, l'element reference' appartient donc au */
			/* document d'origine */
			pDocRef = DocDeSauve;
		     else
			/* l'original etait une reference externe a son document */
			/* d'origine */
		     if (MemeIdentDoc (pDoc->DocIdent, IdentDocRef))
			/* l'element reference' et la reference sont dans le */
			/* meme document */
			pDocRef = pDoc;
		     if (pDocRef != NULL)
			/* le document contenant l'element reference' est charge' */
		       {
			  /* si l'original n'est pas une reference interne, */
			  /* l'element reference' ne peut pas avoir ete copie' en */
			  /* meme temps */
			  if (!pRef->RdInternalRef)
			     /* lie la reference a l'element designe' par l'original */
			     LieReference (pRac, NULL, pElRef, pDoc, pDocRef, False, False);
			  else
			     /* la reference originale etait une reference interne */
			    {
			       memedoc = False;
			       if (pElRef != NULL)
				  if (!DansTampon (pElRef))
				     /* l'element reference' est aussi colle', on ne fait */
				     /* rien : ce cas est traite' plus haut */
				     memedoc = DansNouveaux (pElRef);
			       if (!memedoc)

				  /* etablit le lien entre la reference copie'e et */
				  /* l'element reference */
				  LieReference (pRac, NULL, pElRef, pDoc, pDocRef, False, False);
			    }
		       }
		  }
	     }
     }
   if (!pRac->ElTerminal && pRac->ElSource == NULL)
      /* ce n'est ni une inclusion ni un terminal, on traite tous les fils */
     {
	pEl = pRac->ElFirstChild;
	while (pEl != NULL)
	  {
	     PurgeRef (pEl, pDoc);
	     pEl = pEl->ElNext;
	  }
     }
}
