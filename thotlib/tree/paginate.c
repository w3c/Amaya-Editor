
/* -- Copyright (c) 1990 - 1994 Inria/CNRS All rights reserved. -- */

/*
  page.c :  gestion de la pagination d'un arbre abstrait. Insere les diverses
            marques de saut de page dans l'AA. Les effets de bord sont nombreux.
  Ce module insere les marques de saut de page dans la
  structure abstraite des documents.
  V. Quint	Avril 1987
  France Logiciel no de depot 88-39-001-00
  Major changes:
  
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"

#undef EXPORT
#define EXPORT extern
#include "appdialogue.var"

#undef EXPORT
#define EXPORT
#include "page.var"


#include "application.f"
#include "attribut.f"
#include "img.f"
#include "pos.f"
#include "imabs.f"
#include "arbabs.f"
#include "creation.f"
#include "crimabs.f"
#include "crpages.f"
#include "debug.f"
#include "appexec.f"
#include "except.f"
#include "font.f"
#include "modif.f"
#include "imabs.f"
#include "liste.f"
#include "modimabs.f"
#include "modpres.f"
#include "pres.f"
#include "page.f"
#include "traitepage.f"
#include "select.f"
#include "textelem.f"
#include "varpres.f"
#include "print.f"


#define MaxPageLib 20
#define HMinPage 60	/* Hauteur minimum du corps de la page */

/**FILE *list;*/
#ifdef __COLPAGE__

static int      HauteurRefBasPage;	/* Hauteur minimum des bas de page */
 
static int      HauteurRefHautPage;     /* Hauteur minimum des hauts de page */
#endif /* __COLPAGE__ */
static int      CompteurPages;
/* Hauteurffective est declaree dans page.var (car modifiee par traitepage.c) */

#ifdef __STDC__
extern void AfficherVue (int) ;
#else  /* __STDC__ */
extern void AfficherVue () ;
#endif /* __STDC__ */

#ifdef __COLPAGE__
 /* ----------------------------------------------------------------- */
 /* ChangeRHPage change la regle de dimension verticale de tous       */
 /*             les paves corps de page sous PavRacine.               */
 /*             On force la position du bas de page et du filet de    */
 /*             telle sorte qu'elle soit plus bas que la hauteur max  */
 /*             de la hauteur de coupure                              */
 /* ------------------------------------------------------------------*/
#ifdef __STDC__
static void ChangeRHPage(PtrAbstractBox PavRacine, PtrDocument pDoc, int VueNb)

#else /* __STDC__ */
static void ChangeRHPage(PavRacine, pDoc, VueNb)
	PtrAbstractBox PavRacine;
	PtrDocument pDoc;
	int VueNb;
#endif /* __STDC__ */

{

 PtrAbstractBox         pP;
 AbDimension    *pDim;
 AbPosition	*pPosV;
 PtrAbstractBox         pPFils, pCol;	

 pP = PavRacine->AbFirstEnclosed;
 /* on saute les paves morts (si appele depuis TueAvantPage) */
 while (pP != NULL && pP->AbDead)
   pP = pP->AbNext;
 while (pP != NULL)
   {
     if (pP->AbElement->ElTypeNumber == PageBreak + 1 /* tj vrai ? */
         && !pP->AbPresentationBox)
       {
         /* c'est un corps de page */
         /* on change sa regle de hauteur */
         pDim = &pP->AbHeight;
         pDim->DimIsPosition = False;
         pDim->DimValue = 0;
         pDim->DimAbRef = NULL;
         pDim->DimUnit = UnRelative;
         pDim->DimSameDimension = True;
         pDim->DimUserSpecified = False;
         pDim->DimMinimum = False;
         pP->AbHeightChange = True;
         /* si le fils est une colonne, on fait pareil */
         pPFils = pP->AbFirstEnclosed;
         while (pPFils != NULL 
                && pPFils->AbElement->ElTypeNumber == PageBreak + 1)
           /* on parcourt la suite des groupes de colonnes en */
           /* sautant les eventuels paves de presentation */
           /* TODO parcours en profondeur quand regles incluses ! */
           {
             while (pPFils->AbPresentationBox)
               pPFils = pPFils->AbNext;
             if (pPFils->AbElement->ElTypeNumber == PageBreak + 1)
               {
                 pDim = &pPFils->AbHeight;
                 pDim->DimIsPosition = False;
                 pDim->DimValue = 0;
                 pDim->DimAbRef = NULL;
                 pDim->DimUnit = UnRelative;
                 pDim->DimSameDimension = True;
                 pDim->DimUserSpecified = False;
                 pDim->DimMinimum = False;
                 pPFils->AbHeightChange = True;
                 /* si le fils est une colonne, on fait pareil */
                 pCol = pPFils->AbFirstEnclosed;
                 while (pCol != NULL 
                   && pCol->AbElement->ElTypeNumber == PageBreak + 1)
                   /* on parcourt la suite des colonnes en */
                   /* sautant les eventuels paves de presentation */
                   /* TODO parcours en profondeur quand regles incluses ! */
                   {
             	    while (pCol->AbPresentationBox)
               	      pCol = pCol->AbNext;
             	    if (pCol->AbElement->ElTypeNumber == PageBreak + 1)
               	      {
                 	pDim = &pCol->AbHeight;
                 	pDim->DimIsPosition = False;
                 	pDim->DimValue = 0;
                 	pDim->DimAbRef = NULL;
                 	pDim->DimUnit = UnRelative;
                 	pDim->DimSameDimension = True;
			pDim->DimUserSpecified = False;
                 	pDim->DimMinimum = False;
                 	pCol->AbHeightChange = True;
               	      }
             	    pCol = pCol->AbNext;
           	  }		
               }
             pPFils = pPFils->AbNext;
           }		
       }
     pP = pP->AbNext;
     /* change la position des paves suivants pour qu'ils n'interferent */
     /* pas lors du placement de la coupure de page par le mediateur */
     while (pP!= NULL && pP->AbPresentationBox &&
   	      pP->AbElement == pP->AbPrevious->AbElement)
       {
     	pPosV = &pP->AbVertPos;
	    	pPosV->PosAbRef = NULL;
		pPosV->PosDistance = HauteurTotalePage + 10;
		pPosV->PosUnit == UnPoint;
		pPosV->PosUserSpecified = False;
		pP->AbVertPosChange = True;
		pP = pP->AbNext;
       }
   }
 }

#endif /* __COLPAGE__ */

#ifndef PAGINEETIMPRIME

/* ---------------------------------------------------------------------- */
/* | AnnuleSelectionPage  Annule et deplace si besoin la selection	| */
/* | 			  courante du document.				| */
/* |			  Retourne les valeurs de cette selection 	| */
/* | 			  dans SelPrem, SelDer, SelPremCar et SelDerCar	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean AnnuleSelectionPage(PtrDocument pDoc, int VueSch, PtrElement *SelPrem, PtrElement *SelDer, int *SelPremCar, int *SelDerCar)

#else /* __STDC__ */
static boolean AnnuleSelectionPage(pDoc, VueSch, SelPrem, SelDer, SelPremCar, SelDerCar)
	PtrDocument pDoc;
	int VueSch;
	PtrElement *SelPrem;
	PtrElement *SelDer;
  	int  *SelPremCar;
	int *SelDerCar;

#endif /* __STDC__ */

{
  PtrDocument SelDoc;
  PtrElement pEl1, pEl2, Prem, Der;
  boolean	sel;

  /* demande quelle est la selection courante */
  sel = SelEditeur(&SelDoc, &Prem, &Der, SelPremCar, SelDerCar);
  if (sel && SelDoc != pDoc)
    sel = False;
  /* annule la selection si elle est dans le document a paginer */
  if (sel)
    {
      AnnuleSelect();
      /* on verifie si la selection commence ou se termine sur une marque */
      /* de page qui va disparaitre et dans ce cas on change la selection */
      pEl1 = Prem; /* debut de la selection */
      if (pEl1->ElTypeNumber == PageBreak + 1)
	if (pEl1->ElViewPSchema == VueSch)
#ifdef __COLPAGE__
	  if (pEl1->ElPageType == PgComputed
	      || pEl1->ElPageType == ColComputed
	      || pEl1->ElPageType == ColBegin
	      || pEl1->ElPageType == ColGroup)
#else /* __COLPAGE__ */
	  if (pEl1->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
	    /* c'est une marque de page qui va disparaitre */ 
	    if (pEl1->ElNext != NULL)
	      /* on selectionne l'element suivant la marque de page */
	      {
		if (Der == Prem)
		  Der = pEl1->ElNext;
		Prem = pEl1->ElNext;
	      } 
	    else	/* pas d'element suivant la marque de page */
	      if (pEl1->ElPrevious != NULL)
		/* on selectionne l'element precedent la marque de page */
		{
		  if (Der == Prem)
		    Der = pEl1->ElPrevious;
		  Prem = pEl1->ElPrevious;
		} 
	      else	/* pas de suivant ni de precedent */
		/* on selectionne l'element englobant la marque de page */
		{
		  Prem = pEl1->ElParent;
		  Der = pEl1->ElParent;
		}
      pEl1 = Der; 
      /* dernier element de la selection */ 
      if (pEl1->ElTypeNumber == PageBreak + 1)
	if (pEl1->ElViewPSchema == VueSch)
#ifdef __COLPAGE__
	  if (pEl1->ElPageType == PgComputed
	      || pEl1->ElPageType == ColComputed    
	      || pEl1->ElPageType == ColBegin
	      || pEl1->ElPageType == ColGroup)
#else /* __COLPAGE__ */
	  if (pEl1->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
	    /* le dernier element de la selection est une marque de */
	    /* page qui va disparaitre */
	    if (pEl1->ElPrevious != NULL)
	      Der = pEl1->ElPrevious;
      /* on selectionne le precedent */
	    else if (pEl1->ElNext != NULL)
	      Der = pEl1->ElNext;
      /* on selectionne le suivant */
	    else	/* on selectionne l'englobant */
	      {
		Prem = pEl1->ElParent;
		Der = pEl1->ElParent;
	      }
      /* le debut de la selection est-il dans une feuille de texte qui
	 n'est separee de la precedente que par une marque de page ? Dans
	 ce cas il y aura fusion des deux feuilles et la deuxieme
	 n'existera plus. */
      pEl1 = Prem; /* debut de la selection */ 
      if (pEl1->ElTerminal)
	if (pEl1->ElLeafType == LtText)
	  /* la selection debute dans une feuille de texte */
	  if (pEl1->ElPrevious != NULL)
	    if (pEl1->ElPrevious->ElTypeNumber == PageBreak + 1)
	      if (pEl1->ElPrevious->ElViewPSchema == VueSch)
#ifdef __COLPAGE__
		if (pEl1->ElPrevious->ElPageType == PgComputed
	      || pEl1->ElPrevious->ElPageType == ColComputed  
	      || pEl1->ElPrevious->ElPageType == ColBegin
	      || pEl1->ElPrevious->ElPageType == ColGroup)
#else /* __COLPAGE__ */
		if (pEl1->ElPrevious->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
		  /* la feuille de texte est precedee d'une marque de
		     page qui va disparaitre, on examine l'element
		     precedent la marque de page */
		  {
		    pEl2 = pEl1->ElPrevious->ElPrevious;
		    if (pEl2 != NULL)
		      if (pEl2->ElTerminal &&
			  pEl2->ElLeafType == LtText)
			/* c'est une feuille de texte */
			if (pEl2->ElLanguage == pEl1->ElLanguage)
			  /* meme langue */
			  if (MemesAttributs(Prem, pEl2))
			    /* memes attributs */
			    if (MemesRegleSpecif(Prem, pEl2))
			      /* meme present. */
			      /* les elements vont fusionner, on selectionne le 1er */
			      {
				if (Der == Prem)
				  {
				    Der = pEl2;
				    *SelDerCar = 0;
				  }
				Prem = pEl2;
				*SelDerCar = 0;
			      }
		  }
      /* la fin de la selection est-il dans une feuille de texte qui
	 n'est separee de la precedente que par une marque de page ? Dans
	 ce cas il y aura fusion des deux feuilles et la deuxieme
	 n'existera plus. */
      pEl1 = Der;
      /* fin de la selection */
      if (pEl1->ElTerminal)
	if (pEl1->ElLeafType == LtText)
	  /* la selection se termine dans une feuille de texte */
	  if (pEl1->ElPrevious != NULL)
	    if (pEl1->ElPrevious->ElTypeNumber == PageBreak + 1)
	      if (pEl1->ElPrevious->ElViewPSchema == VueSch)
#ifdef __COLPAGE__
		if (pEl1->ElPrevious->ElPageType == PgComputed
	            || pEl1->ElPrevious->ElPageType == ColComputed
	            || pEl1->ElPrevious->ElPageType == ColBegin
	            || pEl1->ElPrevious->ElPageType == ColGroup)
#else /* __COLPAGE__ */
		if (pEl1->ElPrevious->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
		  /* la feuille de texte est precedee d'une marque de
		     page qui va disparaitre, on examine l'element
		     precedent la marque de page */
		  {
		    pEl2 = pEl1->ElPrevious->ElPrevious;
		    if (pEl2 != NULL)
		      if (pEl2->ElTerminal &&
			  pEl2->ElLeafType == LtText)
			/* c'est une feuille de texte */
			if (pEl2->ElLanguage == pEl1->ElLanguage)
			  /* meme langue */
			  if (MemesAttributs(Der, pEl2))
			    /* memes attributs */
			    if (MemesRegleSpecif(Der,pEl2))
			      /* meme present. */
			      /* les elements vont fusionner, on selectionne le 1er */
			      {
				Der = pEl2;
				*SelDerCar = 0;
			      }
	
	  }
      *SelPrem = Prem;
      *SelDer = Der;
    } /* fin if (sel) */
  return sel;
} /* AnnuleSelectionPage */
#endif  /* PAGINEETIMPRIME */


/* ---------------------------------------------------------------------- */
/* |	SupprMarquePage	supprime la marque de page pointee par pPage et	| */
/* |		essaie de fusionner l'element precedent avec l'element	| */
/* |		suivant.						| */
/* |		Retourne dans pLib un pointeur sur l'element a libere	| */
/* |		resultant de la fusion, si elle a pu se faire.		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void SupprMarquePage(PtrElement pPage, PtrDocument pDoc, PtrElement *pLib)

#else /* __STDC__ */
static void SupprMarquePage(pPage, pDoc, pLib)
	PtrElement pPage;
	PtrDocument pDoc;
	PtrElement *pLib;
#endif /* __STDC__ */

{
  PtrElement      pPrec;
  NotifyElement	  notifyEl;
  int		  nbFreres;
  
  *pLib = NULL;
  /* envoie l'evenement ElemDelete.Pre */
  notifyEl.event = TteElemDelete;
  notifyEl.document = (Document)IdentDocument(pDoc);
  notifyEl.element = (Element)(pPage);
  notifyEl.elementType.ElTypeNum = pPage->ElTypeNumber;
  notifyEl.elementType.ElSSchema = (SSchema)(pPage->ElSructSchema);
  notifyEl.position = 1;
  if (!ThotSendMessage((NotifyEvent *)&notifyEl, True))
    {
    /* traitement de la suppression des pages dans les structures avec */
    /* coupures speciales */
     if (ThotLocalActions[T_Exc_Page_Break_Supprime]!= NULL)
       (*ThotLocalActions[T_Exc_Page_Break_Supprime])(pPage, pDoc);
    pPrec = pPage->ElPrevious;
    /* prepare l'evenement ElemDelete.Post */
    notifyEl.event = TteElemDelete;
    notifyEl.document = (Document)IdentDocument(pDoc);
    notifyEl.element = (Element)(pPage->ElParent);
    notifyEl.elementType.ElTypeNum = pPage->ElTypeNumber;
    notifyEl.elementType.ElSSchema = (SSchema)(pPage->ElSructSchema);
    nbFreres = 0;
    DeleteElement(&pPage);
    *pLib = NULL;
    if (pPrec != NULL)
      {
      /* il y avait un element avant la marque de page, on essaie de le */
      /* fusionner avec l'element qui le suit maintenant. */
      if (!MemeTexte(pPrec, pDoc, pLib))
        *pLib = NULL;
      while (pPrec != NULL)
	{
	nbFreres++;
	pPrec = pPrec->ElPrevious;
	}
      }
    notifyEl.position = nbFreres;
    ThotSendMessage((NotifyEvent *)&notifyEl, False);
    }
}
/* ---------------------------------------------------------------------- */
/* |	DtrMarquesDePages	detruit toutes les marques de page de la| */
/* |		vue VueSch, sauf les marques placees par l'utilisateur	| */
/* |		et celles de debut des elements portant une regle Page.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void DtrMarquesDePages(PtrDocument pDoc, PtrElement pElRacine, int VueSch)

#else /* __STDC__ */
static void DtrMarquesDePages(pDoc, pElRacine, VueSch)
	PtrDocument pDoc;
	PtrElement pElRacine;
	int VueSch;
#endif /* __STDC__ */

{
  PtrElement pEl, pElPage, pElLib ;


   pEl = pElRacine;
  pElPage = NULL;	
  /* pas encore de marque de page a supprimer */
  while (pEl != NULL)
    /* cherche la prochaine marque de page */
    {
      pEl = FwdSearchTypedElem(pEl, PageBreak + 1, NULL);
      if (pEl != NULL)
	if (pEl->ElViewPSchema == VueSch)
	  /* on a trouve' une marque de page concernant la vue */
#ifdef __COLPAGE__
	  if (pEl->ElPageType == PgComputed
	      || pEl->ElPageType == ColComputed
       || pEl->ElPageType == ColBegin
	      || pEl->ElPageType == ColGroup) 
#else /* __COLPAGE__ */
	  if (pEl->ElPageType == PgComputed)
#endif /* __COLPAGE__ */
	    /* c'est une marque de page calculee */ 
	    {
	      if (pElPage != NULL)
		/* il y a deja une marque de page a supprimer, on la supprime */
		{
		  SupprMarquePage(pElPage, pDoc, &pElLib);
		  if (pElLib != NULL)
		    DeleteElement(&pElLib);
		}
	      /* on supprimera cette marque de page au tour suivant */
	      pElPage = pEl;
	    } 
	  else	
	    /* c'est une marque de page a conserver */
	    /* on ne creera pas tout de suite ses boites de haut de page */
	    /* contenant des elements associes. */
	    pEl->ElAssocHeader = False;
    }
  if (pElPage != NULL)
    /* il reste une marque de page a supprimer, on la supprime */
    {
      SupprMarquePage(pElPage, pDoc, &pElLib);
      if (pElLib != NULL)
	DeleteElement(&pElLib);
    }

} /*DtrMarquesDePages */ 

#ifndef PAGINEETIMPRIME

/* ---------------------------------------------------------------------- */
/* | 	AffMsgPage 	Affiche un message avec le nom de la vue 	| */
/* | 			et le numero de page de l'element pEl		| */
/* | 			prempage indique si c'est la premiere page de 	| */
/* |			la vue						| */
/* |			procedure utilisee dans la pagination sous	| */
/* |			l'editeur (version vide pour l'appel depuis	| */
/* |			la commande d'impression)			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static	void AffMsgPage (PtrDocument pDoc, PtrElement pElRacine,PtrElement pEl, int VueSch, boolean Assoc, boolean *prempage)
#else /* __STDC__ */
static	void AffMsgPage (pDoc, pElRacine, pEl, VueSch, Assoc, prempage)
	PtrDocument pDoc;
  	PtrElement    pElRacine;
  	PtrElement    pEl;
	int VueSch;
	boolean Assoc;
	boolean *prempage;
#endif /* __STDC__ */
{
  char          *name;

  /* affiche un message avec le numero de page */
  /* affiche d'abord le nom de la vue */
  if (Assoc)
    name = pElRacine->ElSructSchema->SsRule[pElRacine->ElTypeNumber -1].SrName;
  else
    name = pDoc->DocSSchema->SsPSchema->PsView[VueSch -1];

  if (*prempage)
    {
      /* Affiche un message normal pour la 1ere fois */
      TtaDisplayMessage(INFO, TtaGetMessage(LIB, LIB_PAGE), name, (char *) pEl->ElPageNumber);
      *prempage = False;
    }
  else
    /* Sinon ecrase le message precedent */
    TtaDisplayMessage(OVERHEAD, TtaGetMessage(LIB, LIB_PAGE), name, (char *) pEl->ElPageNumber);
}
#endif /* PAGINEETIMPRIME */

#ifndef PAGINEETIMPRIME

/* ---------------------------------------------------------------------- */
/* | 	Aff_Select_Pages   Apres la pagination sous l'editeur, il faut	| */
/* | 			recreer l'image et retablir la selection.	| */
/* |			Procedure utilisee dans la pagination sous	| */
/* |			l'editeur (version vide pour l'appel depuis	| */
/* |			la commande d'impression)			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static	void Aff_Select_Pages (PtrDocument pDoc, PtrElement PremPage, int Vue, boolean Assoc, boolean sel, PtrElement SelPrem, PtrElement SelDer, int  SelPremCar, int SelDerCar)
#else /* __STDC__ */
static	void Aff_Select_Pages (pDoc, PremPage, Vue, Assoc, sel, SelPrem, SelDer, SelPremCar, SelDerCar)
	PtrDocument pDoc;
  	PtrElement    PremPage;
	int Vue;
	boolean Assoc;
	boolean sel;
	PtrElement SelPrem;
	PtrElement SelDer;
  	int  SelPremCar;
	int SelDerCar;

#endif /* __STDC__ */

{
  PtrElement pElRacine;
  PtrAbstractBox PavRacine /* , pP */;
  int v, VueSch, frame, h;
  boolean /*tropcourt, */ complet;

  /* reconstruit l'image de la vue et l'affiche */
  /* si on n'est pas en batch */
#ifdef __COLPAGE__ 
  HauteurCoupPage = 0;
  HauteurTotalePage = 0;
  HauteurRefBasPage = 0;
#else /* __COLPAGE__ */
  HauteurPage = 0;
  HauteurBasPage = 0;
#endif /* __COLPAGE__ */

  /* cree l'image abstraite des vues concernees */
      if (Assoc)
        {
#ifdef __COLPAGE__ 
	  pDoc->DocAssocFreeVolume[Vue - 1] = THOT_MAXINT;
 	  pDoc->DocAssocNPages[Vue - 1] = 0;
#else /* __COLPAGE__ */
          pDoc->DocAssocFreeVolume[Vue - 1] = pDoc->DocAssocVolume[Vue - 1];
#endif /* __COLPAGE__ */
          pElRacine = pDoc->DocAssocRoot[Vue - 1];
          PavRacine = pElRacine->ElAbstractBox[0];
          frame = pDoc->DocAssocFrame[Vue - 1];
	  CreePaves(pElRacine, pDoc, 1, True, True, &complet);
          h = 0;
          (void) ModifVue(frame, &h, PavRacine);
          if (!sel)
	    AfficherVue(frame);
        }
      else
        {
          pElRacine = pDoc->DocRootElement;
          VueSch = VueAAppliquer(pDoc->DocRootElement, NULL, pDoc, Vue);
      	  for (v = 1; v <= MAX_VIEW_DOC; v++)
	    if (pDoc->DocView[v - 1].DvPSchemaView == VueSch)
	      {
#ifdef __COLPAGE__ 
   	pDoc->DocViewFreeVolume[v - 1] = THOT_MAXINT;
         pDoc->DocViewNPages[v - 1] = 0;
#else /* __COLPAGE__ */
          	pDoc->DocViewFreeVolume[v - 1] = pDoc->DocViewVolume[v - 1];
#endif /* __COLPAGE__ */
          	PavRacine = pDoc->DocViewRootAb[v - 1];
          	frame = pDoc->DocViewFrame[v - 1];
	  	CreePaves(pElRacine, pDoc, v, True, True, &complet);
                h = 0;
                (void) ModifVue(frame, &h, PavRacine);
                if (!sel)
		  AfficherVue(frame);
	      }
        }    

  /* retablit la selection si elle ete supprimee avant le formatage */
  if (sel)
    FusEtSel(pDoc, SelPrem, SelDer, SelPremCar, SelDerCar);

  /* met a jour les numeros qui changent dans les autres vues a cause */
  /* de la creation des nouvelles marques de page */
  if (PremPage != NULL)
    MajNumeros(NextElement(PremPage), PremPage, pDoc, True);

} /* Aff_Select_Pages */
#endif /* PAGINEETIMPRIME */


/* ---------------------------------------------------------------------- */
/* |	Coupe coupe l'element de texte pointe par pEl apres le		| */
/* |		caractere de rang CarCoupe et met a jour les paves	| */
/* |		correspondant.						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static	void Coupe(PtrElement pEl, int CarCoupe, PtrDocument pDoc, int VueNb)

#else /* __STDC__ */
static	void Coupe(pEl, CarCoupe, pDoc, VueNb)
	PtrElement pEl;
	int CarCoupe;
	PtrDocument pDoc;
	int VueNb;
#endif /* __STDC__ */

{
  PtrAbstractBox         pP;
  int             dvol;
  PtrAbstractBox         pPa1;

  SplitTextElement(pEl, CarCoupe + 1, pDoc, True);
  /* reduit le volume du pave de l'element precedant le point de */
  /* coupure et de ses paves englobants, si ces paves existent dans la */
  /* vue traitee. */
  pP = pEl->ElAbstractBox[VueNb-1];
  if (pP != NULL)
    {
      /* le pave a change' : il est plus petit */
      pP->AbChange = True;
      if (!VueAssoc(pEl))
	pDoc->DocViewModifiedAb[VueNb-1] = Englobant(pP, pDoc->DocViewModifiedAb[VueNb-1]);
      else
	  pDoc->DocAssocModifiedAb[pEl->ElAssocNum-1] =  
	    Englobant(pP, pDoc->DocAssocModifiedAb[pEl->ElAssocNum-1]);
      dvol = pP->AbVolume-pEl->ElTextLength;
      /* repercute la difference de volume sur les paves englobants */
      do
	{
	  pPa1 = pP;
	  pPa1->AbVolume -= dvol;
	  pP = pPa1->AbEnclosing;
	}
      while (!(pP == NULL));
    }
  /* prepare la creation des paves de la 2eme partie */
  if (!VueAssoc(pEl))
      if (pDoc->DocView[VueNb-1].DvPSchemaView > 0)
        pDoc->DocViewFreeVolume[VueNb-1] = THOT_MAXINT;
  else
      if (pDoc->DocAssocFrame[pEl->ElAssocNum-1] != 0)
	pDoc->DocAssocFreeVolume[pEl->ElAssocNum-1] = THOT_MAXINT;
  /* cree les paves de la deuxieme partie */
  CrPaveNouv(pEl->ElNext, pDoc, VueNb);
  ApplReglesRetard(pEl->ElNext, pDoc);
}

/* ---------------------------------------------------------------------- */
/* |	Secable	retourne 'Vrai' si le pave pointe' par			| */
/* |		pAb est secable et 'Faux' sinon.			| */
/* |		Au retour, si la boite est secable, pR1 pointe sur la	| */
/* |		regle NoBreak1 a appliquer a l'element			| */
/* |		(pR1 est NULL s'il n'y a pas de regle NoBreak1 a		| */
/* |		appliquer)						| */
/* |		pAt1 pointe sur le bloc de l'attribut auquel correspond	| */
/* |		la regle pR1, si c'est une regle d'attribut (pAt1=NULL	| */
/* |		sinon),							| */
/* |		pR2 pointe sur la regle NoBreak2 a appliquer a l'element| */
/* |		(pR2 est NULL s'il n'y a pas de regle NoBreak2 a		| */
/* |		appliquer),						| */
/* |		pAt2 pointe sur le bloc de l'attribut auquel correspond	| */
/* |		la regle pR2, si c'est une regle d'attribut		| */
/* |		(pAt2=NULL sinon). 					| */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static boolean Secable(PtrAbstractBox pAb, PtrPRule *pR1, PtrAttribute *pAt1, PtrPRule *pR2, PtrAttribute *pAt2, int VueSch)

#else /* __STDC__ */
static boolean Secable(pAb, pR1, pAt1, pR2, pAt2, VueSch)
	PtrAbstractBox pAb;
	PtrPRule *pR1;
	PtrAttribute *pAt1;
	PtrPRule *pR2;
	PtrAttribute *pAt2;
	int VueSch;
#endif /* __STDC__ */

{
  PtrPSchema      pSchP;
  int             Entree;
  boolean         ret;
  PtrSSchema    pSchS;
  
  *pR1 = NULL;
  *pR2 = NULL;

  if (!pAb->AbAcceptPageBreak)
    /* le pave est insecable */
    ret = False;
  else
    /* le pave est secable */
    if (pAb->AbPresentationBox)
       /* c'est un pave de presentation */
       ret = True;
    else
      /* c'est le pave principal d'un element, on cherche les regles */
      /* NoBreak1 et NoBreak2 */
      {
      /* cherche le schema de presentation a appliquer a l'element */
      ChSchemaPres(pAb->AbElement, &pSchP, &Entree, &pSchS);
      ret = True;
      /* cherche la regle NoBreak1 qui s'applique au pave */
      *pR1 = ReglePEl(pAb->AbElement, &pSchP, &pSchS, 0, NULL, VueSch, PtBreak1,
		      False, True, pAt1);
      /* cherche la regle NoBreak2 qui s'applique au pave */
      *pR2 = ReglePEl(pAb->AbElement, &pSchP, &pSchS, 0, NULL, VueSch, PtBreak2,
		      False, True, pAt2);
      }
  return ret;
}


/* ---------------------------------------------------------------------- */
/* |	SautDePage Page	indique si l'element pEl debute par un saut de	| */
/* |		page de la vue VueSch					| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrElement SautDePage(PtrElement pEl, int VueSch)
#else /* __STDC__ */
static PtrElement SautDePage(pEl, VueSch)
	PtrElement pEl;
	int VueSch;
#endif /* __STDC__ */

{
   PtrElement pE;
   boolean    trouve;

	if (pEl->ElTerminal)
	   return NULL;
	else
	   if (pEl->ElFirstChild == NULL)
	      return NULL;
	   else
	      if (pEl->ElFirstChild->ElTypeNumber == PageBreak+1)
		{
		  /* on cherche dans les premiers fils une marque de la vue */
		    trouve = False;
		    pE = pEl->ElFirstChild;
		    while (!trouve && pE != NULL && 
			   pE->ElTypeNumber == PageBreak+1)
		      if (pE->ElViewPSchema == VueSch)
		        trouve = True;
		      else
	       		pE = pE->ElNext;
		    if (trouve)
		      return (pE);
		    else
		      return (NULL);
		  }
	      else
		 return (SautDePage(pEl->ElFirstChild, VueSch));
}


/* ---------------------------------------------------------------------- */
/* |	InsereMarque insere une Marque de Page avant l'element auquel	| */
/* |		correspond le pave pointe' par pP. Si la Marque doit	| */
/* |		etre placee dans un element mis en lignes, en premiere	| */
/* |		position, elle est placee avant cet element. 		| */
/* |		Retourne un pointeur sur l'element Marque de Page	| */
/* |		insere'.						| */
 /*      On detruit la partie de l'i.a. qui suit cette marque    | */
 /*      et on reconstruit l'i.a. (donc le pave pAb change ! )  | */
/* ---------------------------------------------------------------------- */
#ifdef __COLPAGE__
#ifdef __STDC__
static PtrElement InsereMarque(PtrAbstractBox pAb, int frame, int VueNb, PtrAbstractBox *PaveCoupeOrig, boolean *PaveTropHaut, int VueSch, PtrDocument pDoc, PtrElement ElRacine)

#else /* __STDC__ */
static PtrElement InsereMarque(pAb, frame, VueNb, PaveCoupeOrig, PaveTropHaut, VueSch, pDoc, ElRacine)
	PtrAbstractBox pAb;
	int frame;
	int VueNb;
	PtrAbstractBox *PaveCoupeOrig;
	boolean *PaveTropHaut;
	int VueSch;
	PtrDocument pDoc;
	PtrElement ElRacine;
#endif /* __STDC__ */

#else  /*__COLPAGE__ */
#ifdef __STDC__
static PtrElement InsereMarque(PtrAbstractBox pAb, int frame, int VueNb, PtrAbstractBox *PaveCoupeOrig, boolean *PaveTropHaut, int VueSch, PtrDocument pDoc, PtrElement ElRacine)

#else /* __STDC__ */
static PtrElement InsereMarque(pAb, frame, VueNb, PaveCoupeOrig, PaveTropHaut, VueSch, pDoc, ElRacine)
	PtrAbstractBox pAb;
	int frame;
	int VueNb;
	PtrAbstractBox *PaveCoupeOrig;
	boolean *PaveTropHaut;
	int VueSch;
	PtrDocument pDoc;
	PtrElement ElRacine;
#endif /* __STDC__ */

#endif /* __COLPAGE__ */

{
  PtrElement      pElPage, pEl;
  boolean         /* bool,*/  stop, enhaut, ElemIsBefore, coupe;
  PtrAbstractBox         pP1, pP;
  int             cpt, h;
  PtrPSchema      pSchP;
  PtrAbstractBox         pPa1;
  AbPosition    *pPavP1;
  PtrElement      pEl1;
  int		  nbFreres;
  PtrElement	  pF;
  NotifyElement	  notifyEl;
  PtrPRule    pRegle;
#ifdef __COLPAGE__
 PtrElement pElRacine, pElCol, pElColG;
 PtrAbstractBox   pRacine, pP2, PavR;
 boolean   complet, trouve;
 int       b, NbCol;
#else /* __COLPAGE__ */ 
  PtrAbstractBox          PavModifie, PavHautPage, SauvePavPage;
#endif /* __COLPAGE__ */ 

#ifdef __COLPAGE__

  pElPage = NULL;
  pP = pAb;
 /* si pAb est un pave de marque page, inutile d'inserer */
 /* une marque en plus : le decoupage courant est satisfaisant */
 if (!(pP->AbElement->ElTypeNumber == PageBreak+1
	    && (pP->AbElement->ElPageType == PgBegin
		|| pP->AbElement->ElPageType == PgComputed
		|| pP->AbElement->ElPageType == PgUser)))
   {
#else /* __COLPAGE__ */ 
  RefAssocHautPage = NULL;
  pElPage = NULL;
  pP = pAb;
#endif /* __COLPAGE__ */
  /* teste si le pave est le premier d'un pave mis en lignes */
  if (pP->AbEnclosing != NULL)
    if (pP->AbPrevious == NULL)
      {
	pPa1 = pP->AbEnclosing;
	if (pPa1->AbLeafType == LtCompound && pPa1->AbInLine)
	  /* on inserera la marque de page avant le pave englobant */
	  pP = pP->AbEnclosing;
      }
  /* teste si le pave est en haut de son englobant */
  do
    {
      stop = True;
      enhaut = False;
      if (pP->AbEnclosing != NULL)
	{
	  pPavP1 = &pP->AbVertPos;
	  if (pPavP1->PosAbRef == NULL)
	  {
		  /* postion verticale par defaut = en haut de l'englobant */
		  enhaut = True;
		  /* si l'englobant est mis en ligne et que le pave n'est pas*/
		  /* le premier des paves mis en ligne, il n'est pas en haut */
		  /* de son englobant */
		  if (pP->AbEnclosing->AbInLine)
			  if (pP->AbPrevious != NULL)
				  enhaut = False;
	  }
	  else if (pPavP1->PosAbRef == pP->AbEnclosing)
	    if (pPavP1->PosDistance == 0)
	      if (pPavP1->PosEdge == Top)
		if (pPavP1->PosRefEdge == Top)
		  enhaut = True;
	}
      if (enhaut)
	/* le pave est en haut de son englobant, on place la */
	/* marque de page avant l'englobant */
	{
	  pP = pP->AbEnclosing;
	  stop = False;
	}
    }
  while (!(stop));

  pEl = pP->AbElement;
  ElemIsBefore = True;	/* on inserera la nouvelle marque de page avant pEl */
  /*on regarde s'il n'y a pas deja une marque de page juste avant pEl*/
  pEl1 = pEl;
  if (pEl->ElPrevious != NULL)
    {

      /* On ignore les elements repetes en haut de page */
      /* (tetieres de tableaux par exemple)             */
     stop = False;
     do
        if (pEl1->ElPrevious == NULL)
           stop = True;
        else
           if (pEl1->ElPrevious->ElIsCopy &&
               TypeHasException(ExcPageBreakRepetition, pEl1->ElPrevious->ElTypeNumber, pEl1->ElPrevious->ElSructSchema))
              pEl1 = pEl1->ElPrevious;
           else
              stop = True;
     while (!stop);
    }
  pEl1 = PreviousLeaf(pEl1);
  if (pEl1 != NULL)
      if (pEl1->ElTerminal && pEl1->ElLeafType == LtPageColBreak)
	if (pEl1->ElViewPSchema == VueSch)
	  /* il y a deja devant l'element pEl une marque de page
	     pour cette vue */
	  /* on mettra la nouvelle marque apres l'element pEl */
	  {
	    ElemIsBefore = False;
	    /* il y a un pave plus haut que la page avant la */
	    /* nouvelle marque de page que l'on va inserer */
	    *PaveTropHaut = True;
	  }

   /* si le pave pP est un pave de presentation de l'element place' */
   /* apres ou comme dernier fils, on insere l'element marque de page */
   /* apres cet element. Rq: la page sera trop longue ! */
   if (pP->AbPresentationBox)
   {
     pRegle = RegleFonction(pEl, &pSchP);
     while (pRegle != NULL && ElemIsBefore == True)
       {
         if (pP->AbTypeNum == pRegle->PrPresBox[0]
             && pP->AbPSchema == pSchP)
           /* c'est la regle correspondant a ce pave */
           if (pRegle->PrPresFunction == FnCreateAfter
               || pRegle->PrPresFunction == FnCreateLast)
             /* on insere la marque apres l'element */
             {
               *PaveCoupeOrig = NULL;
               ElemIsBefore = False;
             }
         pRegle = pRegle->PrNextPRule;
         if (pRegle != NULL && pRegle->PrType > PtFunction)
           pRegle = NULL;
       }
   }

  pEl1 = pEl;
  if (!ElemIsBefore)
    if (*PaveCoupeOrig != NULL)
      /* Il y a un pave insecable plus haut qu'une page, on inserera */
      /* la marque de page apres l'element de ce pave */
      pEl1 = (*PaveCoupeOrig)->AbElement;
/*653*/ if (pEl1->ElParent == NULL)
/*653*/   /* si pEl1 est la racine il faut descendre d'un niveau */
/*653*/   if (ElemIsBefore)
/*653*/     pEl1 = pEl1->ElFirstChild;
/*653*/   else
/*653*/     {
/*653*/       pEl1 = pEl1->ElFirstChild;
/*653*/       while (pEl1->ElNext != NULL)
/*653*/         pEl1 = pEl1->ElNext;
/*653*/	    }
  if (ElemIsBefore && pEl1->ElTypeNumber == PageBreak+1)
     return pEl1;
  if (ElemIsBefore)
    {
    pElPage = SautDePage(pEl1, VueSch);
    if (pElPage != NULL)	
       /* on veut inserer un saut de page devant un element qui a la regle */
       /* Page. On n'insere pas de nouveau saut de page et on retourne le */
       /* saut de page de l'element suivant */
       return pElPage;
    }
  /* envoie l'evenement ElemNew.Pre */
  notifyEl.event = TteElemNew;
  notifyEl.document = (Document)IdentDocument(pDoc);
  notifyEl.element = (Element)(pEl->ElParent);
  notifyEl.elementType.ElTypeNum = PageBreak+1;
  notifyEl.elementType.ElSSchema = (SSchema)(ElRacine->ElSructSchema);
  nbFreres = 0;
  pF = pEl1;
  while (pF->ElPrevious != NULL)
    {
    nbFreres++;
    pF = pF->ElPrevious;
    }
  if (!ElemIsBefore)
    nbFreres++;
  notifyEl.position = nbFreres;
  ThotSendMessage((NotifyEvent *)&notifyEl, True);
  /* cree l'element Marque de Page */
  pElPage = NewSubtree(PageBreak+1, ElRacine->ElSructSchema,
		       pDoc, pEl->ElAssocNum, True, True, True, True);
  /* insere l'element dans l'arbre abstrait */
  if (pEl1->ElParent != NULL)
    if (ElemIsBefore)
      InsertElementBefore(pEl1, pElPage);
    else
      InsertElementAfter(pEl1, pElPage);
  pEl1 = pElPage;
#ifdef __COLPAGE__
 /* tout le reste du code de la procedure est different */
 pElCol = NULL; /* a priori, pas d'element colonne simple cree en plus */
 pElColG = NULL; /* a priori, pas d'element colonne groupees cree en plus */
  /* remplit l'element cree' */
 /* test si l'element suivant porte une regle page ou colonne */
 /* TODO */
 /* si ce n'est pas le cas, on regarde si la marque inseree */
 /* doit etre une marque de page ou de colonne : pour cela, on */
 /* regarde si il y a un (ou plusieurs) pave de colonne dans */
 /* la page ; si oui, on recherche la regle colonne pour */
 /* savoir si il reste des colonnes a creer et on la cree. */
 /* Sinon, on  cree une page. */
 pP = pAb;
 trouve = False;
 while (pP->AbElement->ElTypeNumber != PageBreak+1)
   pP = pP->AbEnclosing;
 /* les paves de page ou colonnes englobent leur contenu */
 if (pP->AbElement->ElPageType == ColBegin
	    || pP->AbElement->ElPageType == ColComputed
	    || pP->AbElement->ElPageType == ColUser)
    {
      /* on recherche la regle colonne pour comparer */
      /* le numero de cette colonne avec le nombre de */
      /* colonnes specifie dans la regle */
      b = TypeBCol (pP->AbElement, VueNb, &pSchP, &NbCol);
	     if (b == 0)
		 /* erreur */
	       AffPaveDebug (pAb);
	     else
	       if (pP->AbElement->ElPageNumber < NbCol)
		   /* la nouvelle marque doit etre une colonne */
	          {
		   pEl1->ElPageType = ColComputed;
	           pEl1->ElPageNumber = pP->AbElement->ElPageNumber + 1;
	         }
	       else
	         {
		   /* il faut creer une nouvelle marque de page, */
		   pEl1->ElPageType = PgComputed;
	            /* creer une nouvelle marque de colonnes ColGroup */
	           pElColG = NewSubtree(PageBreak+1, ElRacine->ElSructSchema,
	                       pDoc, pEl->ElAssocNum, True, True, True, True);
		   /* insere l'element dans l'arbre abstrait */
	           InsertElementAfter(pEl1, pElColG);
		   pElColG->ElPageType = ColGroup;
		   pElColG->ElPageNumber = 0;
		   pElColG->ElViewPSchema = VueSch;   
		   /* et creer une nouvelle marque de colonne gauche */
	           pElCol = NewSubtree(PageBreak+1, ElRacine->ElSructSchema,
	                       pDoc, pEl->ElAssocNum, True, True, True, True);
		   /* insere l'element dans l'arbre abstrait */
	           InsertElementAfter(pElColG, pElCol);
		   pElCol->ElPageType = ColComputed;
		   pElCol->ElPageNumber = 1;
		   pElCol->ElViewPSchema = VueSch;   
	         }
	      }
	  else
	    pEl1->ElPageType = PgComputed;
	  pEl1->ElViewPSchema = VueSch;
	 if (pEl1->ElPageType == PgComputed
	    || pEl1->ElPageType == PgBegin
	    || pEl1->ElPageType == PgUser)
	   {
	    /* ajout au 6 dec 93 : compteur de page. Est-ce necessaire ? */
            /* cherche le compteur de page a appliquer */
            cpt = CptPage(pElPage, VueSch, &pSchP);
            if (cpt == 0) /* page non numerotee */
              /* on entretient un compteur de pages pour pouvoir afficher un */
              /* message indiquant la progression du formatage */
              pEl1->ElPageNumber = CompteurPages;
            else  /* calcule le numero de page */
              pEl1->ElPageNumber = ComptVal(cpt, pElPage->ElSructSchema, pSchP,
                               pElPage, VueSch);
   }	    
  /* envoie l'evenement ElemNew.Post */
#ifndef PAGINEETIMPRIME
  NotifySubTree(TteElemNew, pDoc, pElPage, 0);
#endif PAGINEETIMPRIME
 NbBoiteHautPageACreer = 0;
 /* code de creation des hauts de page supprime */

 /* on ne peut creer les paves de la nouvelle marque de page */
 /* que si tous les paves des elements suivants  */
 /* ont ete detruits */
 /* on tue tous les freres droits en remontant l'arbre jusqu'au */
 /* corps de page */
 pP = pEl->ElAbstractBox[VueNb -1];
 if (!ElemIsBefore && *PaveCoupeOrig != NULL)
   pP = (*PaveCoupeOrig)->AbElement->ElAbstractBox[VueNb -1];
  /* on detruit les paves a partir du premier pave qui */
  /* se trouve dans la page ou colonne courante (si il n'y */
  /* a pas de colonne, c'est le pave chaine a l'element */
  /* car il ne peut y avoir de duplique precedent sur une */
  /* colonne precedente pour cet element  */
 pP1 = pP;
 if (ElemIsBefore)
   { /* destruction des paves de l'element pEl */
     pP2 = pP1;
     while (pP2 != NULL && pP2->AbElement == pEl)
       {
         TuePave(pP2);
         SuppRfPave(pP2, &PavR, pDoc);
         if (pP2->AbNext == NULL)
           pP1 = pP2; /* pP1 = dernier pave non null */
         pP2 = pP2->AbNext;
       }
   }
 else
   {
     while (pP1->AbNext != NULL
            && pP1->AbNext->AbElement == pP->AbElement)
       pP1 = pP1->AbNext; /* on saute les paves de l'element */
     while (pP1->AbNextRepeated != NULL)
       pP1 = pP1->AbNextRepeated;
     while (pP1->AbNext != NULL
            && pP1->AbNext->AbElement == pP->AbElement)
       pP1 = pP1->AbNext; /* on saute les derniers paves de pres */
                              /* mais en laissant pP1 non nul */
   }
 /* on tue tous les paves de presentation a droite */
 /* sauf ceux des pages et des colonnes */
 /* en mettant a jour le booleen AbTruncatedTail */
 TuePresDroite(pP1, pDoc);
 /* on tue tous les paves a droite en remontant l'arbre */
 /* on cherche d'abord le premier pave a detruire */
 trouve = False;
 pP = pP1;
 pP1 = NULL;
 while (!trouve && pP != NULL)
     if (pP->AbNext != NULL)
       {
         pP1 = pP->AbNext;
         trouve = True;
       }
     else
       pP = pP->AbEnclosing;
 if (pP1 != NULL && pP1->AbElement->ElTypeNumber == PageBreak+1
		   && (pP1->AbElement->ElPageType == PgBegin
		       || pP1->AbElement->ElPageType == PgComputed
		       || pP1->AbElement->ElPageType == PgUser))
	  /* et si on est sur une colonne, on detruira bien la colonne */
	  /* suivante */
   /* si on est sur une page, on saute tous ses paves */
   /* remarque : pP = pP1->AbPrevious */
   while (pP1 != NULL && pP1->AbElement == pP->AbElement)
     pP1 = pP1->AbNext;
 if (pP1 != NULL && pP1->AbEnclosing != NULL)
   /* on ne detruit pas la racine */
   DetrPaveSuivants (pP1, pDoc);
  /* cree les paves de l'element Marque de Page qu'on vient */
  /* d'inserer */

 /* creation par appel de CreePaves et non CrPaveNouv car il n'y */
 /* a plus de localite dans l'i.a. (les paves de page sont sous */
 /* la racine) */
 if (VueAssoc(pEl))
   {
     pElRacine = pDoc->DocAssocRoot[pEl->ElAssocNum - 1];
     pRacine = pElRacine->ElAbstractBox[0];
   }
 else
   {
     pElRacine = pDoc->DocRootElement;
     pRacine = pElRacine->ElAbstractBox[VueNb - 1];
   }
 /* on nettoie d'abord l'image abstraite des paves morts */
 h = -1; /* changement de signification de la valeur de h */
 bool = ModifVue(frame, &h, pRacine);
 LibPavMort(pRacine);
 /* appel de CreePaves */
 /* TODO : a mettre en coherence ->CrPaveNouv pour appel ApplRegleRet */
 pPa1 = CreePaves(pElPage, pDoc, VueNb, True, True, &complet);
 ApplReglesRetard(pElPage, pDoc);
 /* si on a cree  une marque colonne groupees, on cree son pave */
 if (pElColG != NULL)
   {
     pPa1 = CreePaves(pElColG, pDoc, VueNb, True, True, &complet);
     ApplReglesRetard(pElColG, pDoc);
   }
 if (pElCol != NULL)
   {
     pPa1 = CreePaves(pElCol, pDoc, VueNb, True, True, &complet);
     ApplReglesRetard(pElCol, pDoc);
     /* on a cree aussi une marque colonne, on cree son pave */
   }
 /* AbstractBox modifie au niveau de la racine */
  /* signale ces paves au Mediateur, en faisant reevaluer */
    /* TODO utile ? ... a supprimer */
  /* la coupure de page pour le cas ou on traite des colonnes */
  ChangeRHPage (pRacine, pDoc, VueNb);
  Hauteurffective = HauteurCoupPage;
  bool = ModifVue(frame, &Hauteurffective, pRacine);
  pPa1 = pElPage->ElAbstractBox[VueNb-1];
 /* tous les paves de la page prec ne sont plus marques surpage et horspage */
 /* inutile .... et incomplet: il faudrait faire toute la hierarchie */
 if (pPa1 == NULL)
   printf("erreur insere marque : pas de pave page cree ", "\n");
 else
   {
     pPa1 = pPa1->AbPrevious;
     while (pPa1 != NULL)
       {
                pPa1->AbOnPageBreak = False;
                pPa1->AbAfterPageBreak = False;
         pPa1 = pPa1->AbPrevious;
       }
   }
 /* pb car lors de la creation de l'ia pour la nouvelle marque de page */
 /* et pour les elements suivants, pAb a change : c'est un descendant */
 /* du pave de la marque de page (il est dans la nouvelle page) */
 /* le code ci-dessous est-il a changer ? ... supprime  ! */
 } /* fin cas ou pP n'est pas une marquePage */

#else /* __COLPAGE__ */
  /* remplit l'element page cree' */
    pEl1->ElPageType = PgComputed;
  pEl1->ElViewPSchema = VueSch;
  /* cherche le compteur de page a appliquer */
  cpt = CptPage(pElPage, VueSch, &pSchP);
  if (cpt == 0) /* page non numerotee */
    /* on entretient un compteur de pages pour pouvoir afficher un */
    /* message indiquant la progression du formatage */
    {
      CompteurPages++;
      pEl1->ElPageNumber = CompteurPages;
    } 
  else	/* calcule le numero de page */
    pEl1->ElPageNumber = ComptVal(cpt, pElPage->ElSructSchema, pSchP,
			       pElPage, VueSch);
  /* envoie l'evenement ElemNew.Post */
#ifndef PAGINEETIMPRIME
  NotifySubTree(TteElemNew, pDoc, pElPage, 0);
#endif PAGINEETIMPRIME

  /* Si la page precedente prevoit des elements associes en haut de */
  /* page, on peut les creer maintenant */
  if (NbBoiteHautPageACreer > 0)
    /* cherche d'abord la boite du filet de separation de pages: */
    /* c'est la premiere boite contenue dans la boite de saut de page */
    /* qui n'est pas une boite de presentation. */
    {
      if (PageConcernee->ElAbstractBox[VueNb-1] != NULL)
	{
	  pP1 = PageConcernee->ElAbstractBox[VueNb-1]->AbFirstEnclosed;
	  stop = False;
	  do
	    if (pP1 == NULL)
	      stop = True;
	    else if (!pP1->AbPresentationBox)
	      stop = True;
	    else
	      pP1 = pP1->AbNext;
	  while (!(stop));
	  SauvePavPage = PageConcernee->ElAbstractBox[VueNb-1];
	  PageConcernee->ElAbstractBox[VueNb-1] = pP1;
	  RefAssocHautPage = NULL;
	  /* au retour de CrPavPres, */
	  /* RefAssocHautPage pointera sur la premiere reference */
	  /* qui designe le premier element associe' mis dans la */
	  /* boite de haut de page */
	  PavHautPage = CrPavPres(PageConcernee, pDoc, RegleCreation, 
				  PageConcernee->ElSructSchema, NULL, VueNb,
				  SchPresRegle, False, True);
	  PageConcernee->ElAbstractBox[VueNb-1] = SauvePavPage;
	  if (PavHautPage == NULL)
	    RefAssocHautPage = NULL;
	  else
	    /* signale ces paves au Mediateur, sans faire reevaluer la coupure de page. */
	    {
	      h = 0;
	      (void) ModifVue(frame, &h, PavHautPage);
	    }
	}
      NbBoiteHautPageACreer = 0;
    }
  /* cree les paves de l'element Marque de Page qu'on vient */
  /* d'inserer, mais sans les paves des eventuels elements associes */
  /* qui s'affichent en haut de page. Ceux-ci ne pourront etre crees */
  /* que lorsque la marque de page suivante sera placee, puisque les */
  /* elements associes a placer en haut de page sont ceux qui sont */
  /* designes par une reference qui se trouve entre cette marque de */
  /* page et la suivante. */
  pElPage->ElAssocHeader = False;
  /* traitement de l'insertion des pages dans les structures avec coupures speciales */
  coupe = False; /* a priori pas de coupure effectuee par l'exception */
  if (ThotLocalActions[T_Exc_Page_Break_Inserer]!= NULL)
    (*ThotLocalActions[T_Exc_Page_Break_Inserer])(pElPage, pDoc, VueNb, &coupe);
  if (!coupe)
    CrPaveNouv(pElPage, pDoc, VueNb);
  if (!VueAssoc(pEl))
    PavModifie = pDoc->DocViewModifiedAb[VueNb-1];
  else
    PavModifie = pDoc->DocAssocModifiedAb[pEl->ElAssocNum-1];
  /* signale ces paves au Mediateur, sans faire reevaluer la coupure de page.*/
  if (PavModifie != NULL)
    {
    h = 0;
    (void) ModifVue(frame, &h, PavModifie);
    pPa1 = pElPage->ElAbstractBox[VueNb-1];
    pPa1->AbOnPageBreak = False;
    pPa1->AbAfterPageBreak = False;
    /* les paves devant lesquels on a mis la marque de page ne doivent */
    /* plus etre traites. On les marque hors page. */
    stop = False;
    do
      {
      pAb->AbOnPageBreak = False;
      if (pAb == pP)
	stop = True;
      else
	pAb = pAb->AbEnclosing;
      }
    while (!(stop));
    }
#endif /* __COLPAGE__ */
  return pElPage;
}


/* ---------------------------------------------------------------------- */
/* |	DeplCoupure cherche dans les paves du sous-arbre de racine	| */
/* |		pAb, en appliquant les regles de condition de coupure	| */
/* |		NoBreak1 (ou NoBreak2, selon NoBr1), le premier pave qui| */
/* |		necessite un deplacement (vers le haut) de la frontiere	| */
/* |		de page. Retourne la nouvelle frontiere de page, en	| */
/* |		points typographiques, ou 0 si la coupure de page	| */
/* |		convient.						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static int DeplCoupure(PtrAbstractBox pAb, boolean NoBr1, int VueSch)

#else /* __STDC__ */
static int DeplCoupure(pAb, NoBr1, VueSch)
	PtrAbstractBox pAb;
	boolean NoBr1;
	int VueSch;
#endif /* __STDC__ */

{
  int             ret, Hauteur, PosV, CarCoupe, Min, i;
  PtrPRule    pRNoBr1, pRNoBr2;
  PtrAttribute     pA1, pA2;
  boolean         pavecoupe;
  PtrPRule    pRe1;
  
  ret = 0;
  /* cherche si la coupure de page convient au pave */
  if (pAb->AbOnPageBreak)

    if (!Secable(pAb, &pRNoBr1, &pA1, &pRNoBr2, &pA2, VueSch))
      /* le pave est insecable, on coupe avant ce pave */
      {
	HautCoupure(pAb, True, &Hauteur, &PosV, &CarCoupe);
	ret = PosV;
      } 
    else	/* le pave' n'est pas insecable */
      if (NoBr1)
	/* verifie si la regle NoBreak1 est respectee */ 
	{
	if (pRNoBr1 != NULL)
	  /* une regle NoBreak1 doit etre appliquee a l'element */
	  /* on ne l'applique que si le pave est complet */
	  {
	    if (pAb->AbLeafType != LtCompound)
	      pavecoupe = False;
	    else if (pAb->AbInLine)
	      pavecoupe = False;
	    else
	      pavecoupe = pAb->AbTruncatedHead;
    /* si la boite est eclatee, on ignore la regle NoBreak1 */
    if (pAb->AbBox != NULL)
       if (pAb->AbBox->BxType == BoGhost)
	  pavecoupe = True;
	    if (!pavecoupe)
	      {
		pRe1 = pRNoBr1;
		/* demande au Mediateur la position du haut du pave dans la page */
		HautCoupure(pAb, True, &Hauteur,&PosV, &CarCoupe);
		/* calcule la hauteur minimum avant coupure, en points typographiques */
		if (pRe1->PrMinAttr)
		  i = AttrValue(pA1);
		else
		  i = pRe1->PrMinValue;
		Min = PixelValue(i, pRe1->PrMinUnit, pAb);
		if (Hauteurffective-PosV < Min)
		  /* la hauteur minimum n'y est pas, on deplace la */
		  /* hauteur de page en haut du pave */
		  ret = PosV;
	      }
	  } 
	}
      else	/* verifie si la regle NoBreak2 est respectee */
	if (pRNoBr2 != NULL)
	  /* une regle NoBreak2 doit etre appliquee a l'element */
	  /* on ne l'applique que si le pave est complet */
	  {
	    if (pAb->AbLeafType != LtCompound)
	      pavecoupe = False;
#ifdef __COLPAGE__
       /* si un pave est mis en ligne, il peut etre coupe. */
       /* ce cas est retire */
#else /* __COLPAGE__ */
	    else if (pAb->AbInLine)
	      pavecoupe = False;
#endif /* __COLPAGE__ */
	    else
	      pavecoupe = pAb->AbTruncatedTail;
    /* si la boite est eclatee, on ignore la regle NoBreak2 */
    if (pAb->AbBox != NULL)
       if (pAb->AbBox->BxType == BoGhost)
	  pavecoupe = True;
	    if (!pavecoupe)
	      {
		pRe1 = pRNoBr2;
		/* demande au Mediateur ou se place le pave dans la page */
		HautCoupure(pAb, True, &Hauteur, &PosV, &CarCoupe);
		/* calcule la hauteur minimum apres coupure, en points typographiques */
		if (pRe1->PrMinAttr)
		  i = AttrValue(pA2);
		else
		  i = pRe1->PrMinValue;
		Min = PixelValue(i, pRe1->PrMinUnit, pAb);
		if (PosV + Hauteur - Hauteurffective < Min)
		  /* la frontiere de page passe trop pres du bas du */
		  /* pave, on la ramene en haut de la zone autorisee */
		  ret = PosV + Hauteur - Min;
	      }
	  }
  if (ret == 0)
    /* la coupure convient au pave, cherche si elle convient aux paves englobes. */
    {
      pAb = pAb->AbFirstEnclosed;
#ifdef __COLPAGE__
   /* on saute les paves de colonnes pour arriver a la derniere */
   /* cf. procedure CoupSurPage de pos.c (lignes #800) */
   if (pAb != NULL)
	    while (pAb->AbElement->ElTypeNumber == PageBreak+1
		   && (pAb->AbElement->ElPageType == ColBegin
		       || pAb->AbElement->ElPageType == ColComputed
		       || pAb->AbElement->ElPageType == ColUser
		       || pAb->AbElement->ElPageType == ColGroup)
	           && pAb->AbNext != NULL)
	      pAb = pAb->AbNext;      
#endif /* __COLPAGE__ */
      while (ret == 0 && pAb != NULL)
	{
	  ret = DeplCoupure(pAb, NoBr1, VueSch);
	  pAb = pAb->AbNext;
	}
    }
  return ret;
} /* end DeplCoupure */


#ifdef __COLPAGE__
 /* procedure PlaceMarque changee : on ne traite plus le cas de */
 /* CreateWith */
/* ---------------------------------------------------------------------- */
/* |	PlaceMarque place dans l'arbre de racine pAb les marques de	| */
/* |		page en fonction de la position des paves relativement	| */
/* |		a la limite de page					| */
 /*      cette procedure n'est appelee que sur un pave d'element | */
 /*      et elle saut les paves de presentation.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void PlaceMarque(PtrAbstractBox pAb, PtrElement ElRacine, PtrDocument pDoc, int VueSch, boolean *PaveTropHaut, PtrAbstractBox *PaveCoupeOrig, int VueNb, int frame, PtrElement *pPage)

#else /* __STDC__ */
static void PlaceMarque(pAb, ElRacine, pDoc, VueSch, PaveTropHaut, PaveCoupeOrig, VueNb, frame, pPage)
	PtrAbstractBox pAb;
	PtrElement ElRacine;
	PtrDocument pDoc;
	int VueSch;
	boolean *PaveTropHaut;
	PtrAbstractBox *PaveCoupeOrig;
	int VueNb;
	int frame;
        PtrElement *pPage;	
#endif /* __STDC__ */

{
  int             Hauteur, PosV, CarCoupe;
  boolean         DejaFait;
  PtrAbstractBox         pPa1;
		
  if (pAb != NULL && !pAb->AbPresentationBox)
    {
      pPa1 = pAb;
      if (pPa1->AbOnPageBreak)
	/* le pave' est traverse' par la limite de page */ 
	if (pPa1->AbFirstEnclosed == NULL)
	  /* c'est un pave' feuille */
	  {
	      /* demande au mediateur sur quel caractere a lieu la coupure */
	      /* (si ce n'est pas une feuille de texte, on placera la marque */
	      /* de page avant le pave) */
	      HautCoupure(pAb, True, &Hauteur, &PosV, &CarCoupe);
	      if (CarCoupe <= 0)
	        /* place la marque de page avant le pave */
	        *pPage = InsereMarque(pAb, frame, VueNb,
				  PaveCoupeOrig, PaveTropHaut,
				  VueSch, pDoc, ElRacine);
	      else if (CarCoupe >= pPa1->AbElement->ElTextLength)
	        /* la coupure tombe a la fin du pave */
	        {
		*pPage = NULL;
		pPa1->AbOnPageBreak = False;
	        } 
	      else
	        /* coupe l'element de texte */
	        {
		Coupe(pPa1->AbElement, CarCoupe, pDoc, VueNb);
		pPa1->AbOnPageBreak = False;
		pAb = pPa1->AbNext;
		pAb->AbAfterPageBreak = True;
		*pPage = InsereMarque(pAb, frame, VueNb,
				  PaveCoupeOrig, PaveTropHaut, 
				  VueSch, pDoc, ElRacine);
	        }
	  } 
	else
	  /* ce n'est pas un pave feuille, on examine tous les paves */
	  /* englobes par ce pave' */
	  /* sauf si c'est un pave de colonne suivi d'autres colonnes */
   /* on saute les paves de colonnes pour arriver a la derniere */
   /* cf. procedure CoupSurPage de pos.c (lignes #800) */
	  {
	    pAb = pAb->AbFirstEnclosed;
	    while (pAb->AbElement->ElTypeNumber == PageBreak+1
		   && (pAb->AbElement->ElPageType == ColBegin
		       || pAb->AbElement->ElPageType == ColComputed
		       || pAb->AbElement->ElPageType == ColUser
		       || pAb->AbElement->ElPageType == ColGroup)
	           && pAb->AbNext != NULL)
	      pAb = pAb->AbNext;
	    DejaFait = False;
     /* on s'arrete des qu'on a cree une marque de page */
     while (pAb != NULL && !DejaFait)
	      {
         /* si pAb est un pave de presentation, on passe */
         /* au pave suivant */
		if (pAb->AbOnPageBreak && !pAb->AbPresentationBox)
		  /* la frontiere de page traverse ce pave, on place une */
		  /* marque de page a l'interieur */
		  {
		    PlaceMarque(pAb, ElRacine, pDoc, VueSch, PaveTropHaut,
				PaveCoupeOrig, VueNb, frame, pPage);
		    DejaFait = *pPage != NULL;
		  } 
         else
         /* si le pave est un filet, on considere qu'il */
         /* n'est pas sur la limite. (page complete) */
         /* a voir si c'est necessaire !!! */
           if (pAb->AbOnPageBreak
              && pAb->AbElement->ElTypeNumber == PageBreak +1
              && pAb->AbLeafType == LtGraphics)
             {
               pAb->AbOnPageBreak = False;
               *pPage = pPa1->AbElement; 
               /* on retourne l'element MP prec */
               /* pour detruire l'i.a. correspondante */
             }
		  else if (pAb->AbAfterPageBreak && !pAb->AbPresentationBox)
		    if (!DejaFait)
		      /* c'est le premier pave englobe' au-dela de la */
		      /* frontiere, on pose une marque de page devant lui */
		      {
		        *pPage=InsereMarque(pAb, frame, VueNb,
			       PaveCoupeOrig, PaveTropHaut, VueSch, pDoc, ElRacine);
		        DejaFait = True;
		      }
		if (pAb != NULL && !DejaFait)
		  pAb = pAb->AbNext;
	      } /* fin du while */
	  }
    }
}

#else /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |	PlaceMarque place dans l'arbre de racine pAb la marque de	| */
/* |		page en fonction de la position des paves relativement	| */
/* |		a la limite de page					| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void PlaceMarque(PtrAbstractBox pAb, PtrElement ElRacine, PtrDocument pDoc, int VueSch, boolean *PaveTropHaut, PtrAbstractBox *PaveCoupeOrig, int VueNb, int frame, PtrElement *pPage)

#else /* __STDC__ */
static void PlaceMarque(pAb, ElRacine, pDoc, VueSch, PaveTropHaut, PaveCoupeOrig, VueNb, frame, pPage)
	PtrAbstractBox pAb;
	PtrElement ElRacine;
	PtrDocument pDoc;
	int VueSch;
	boolean *PaveTropHaut;
	PtrAbstractBox *PaveCoupeOrig;
	int VueNb;
	int frame;
	PtrElement *pPage;
#endif /* __STDC__ */

{
  int             Hauteur, PosV, CarCoupe;
  boolean         DejaFait;
  PtrAbstractBox         pPa1;
  boolean	  onCoupe;
  PtrAbstractBox	  pCreateur;
  
  if (pAb != NULL)
    {
      pPa1 = pAb;
      if (pPa1->AbOnPageBreak)
	/* le pave' est traverse' par la limite de page */ 
	if (pPa1->AbFirstEnclosed == NULL)
	  /* c'est un pave' feuille */
	  {
	    /* a priori on va le couper en deux */
	    onCoupe = True;
	    if (pAb->AbPresentationBox)
	      /* c'est un pave de presentation */
	      {
	      /* on cherche le pave qui l'a cree' pour connaitre sa regle */
	      /* de creation */
	      pCreateur = pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1];
	      while (pCreateur->AbPresentationBox)
		pCreateur = pCreateur->AbNext;
	      /* on cherche la regle de creation */
	      if (RegleCree(pDoc, pCreateur, pAb) == FnCreateWith)
		 /* c'est une regle CreateWith, on ne fait rien */
		 onCoupe = False;
	      }
	    if (onCoupe)
	      {
	      /* demande au mediateur sur quel caractere a lieu la coupure */
	      /* (si ce n'est pas une feuille de texte, on placera la marque */
	      /* de page avant le pave) */
	      HautCoupure(pAb, True, &Hauteur, &PosV, &CarCoupe);
	      if (CarCoupe <= 0)
	        /* place la marque de page avant le pave */
	        *pPage = InsereMarque(pAb, frame, VueNb,
				  PaveCoupeOrig, PaveTropHaut,
				  VueSch, pDoc, ElRacine);
	      else if (CarCoupe >= pPa1->AbElement->ElTextLength)
	        /* la coupure tombe a la fin du pave */
	        {
		pPa1->AbOnPageBreak = False;
	        } 
	      else
	        /* coupe l'element de texte */
	        {
		Coupe(pPa1->AbElement, CarCoupe, pDoc, VueNb);
		pPa1->AbOnPageBreak = False;
		pAb = pPa1->AbNext;
		pAb->AbAfterPageBreak = True;
		*pPage = InsereMarque(pAb, frame, VueNb,
				  PaveCoupeOrig, PaveTropHaut, 
				  VueSch, pDoc, ElRacine);
	        }
	      }
	  } 
	else
	  /* ce n'est pas un pave feuille, on examine tous les paves */
	  /* englobes par ce pave' */
	  {
	    pAb = pAb->AbFirstEnclosed;
	    DejaFait = False;
	    while (pAb != NULL)
	      {
		if (pAb->AbOnPageBreak)
		  /* la frontiere de page traverse ce pave, on place une */
		  /* marque de page a l'interieur */
		  {
		    PlaceMarque(pAb, ElRacine, pDoc, VueSch, PaveTropHaut,
				PaveCoupeOrig, VueNb, frame, pPage);
		    DejaFait = *pPage != NULL;
		  } 
		else if (pAb->AbAfterPageBreak)
		  if (!DejaFait)
		    /* c'est le premier pave englobe' au-dela de la */
		    /* frontiere, on pose une marque de page devant lui */
		    {
		      *pPage=InsereMarque(pAb, frame, VueNb,
				      PaveCoupeOrig, PaveTropHaut, VueSch, pDoc, ElRacine);
		      DejaFait = True;
		      pAb = NULL;
		    }
		if (pAb != NULL)
		  pAb = pAb->AbNext;
	      }
	  }
    }
}
#endif /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |	PlacePage place la marque de page en respectant la		| */
/* |		hauteur de page demandee et les conditions de		| */
/* |		coupure des paves de la page.				| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void PlacePage(PtrElement *pPage, int frame, PtrAbstractBox *PaveCoupeOrig, boolean *PaveTropHaut, PtrDocument pDoc, int VueSch, int VueNb, PtrElement ElRacine)

#else /* __STDC__ */
static void PlacePage(pPage, frame, PaveCoupeOrig, PaveTropHaut, pDoc, VueSch, VueNb, ElRacine)
	PtrElement *pPage;
	int frame;
	PtrAbstractBox *PaveCoupeOrig;
	boolean *PaveTropHaut;
	PtrDocument pDoc;
	int VueSch;
	int VueNb;
	PtrElement ElRacine;
#endif /* __STDC__ */

{
  int             tour, NouvHPage, AncienHPage;
  boolean         /* bool,*/ NoBr1;
  
  /* explore deux fois l'arbre des paves a la recherche des paves */
  /* traverses par la frontiere de page et decale la frontiere de page */
  /* vers le haut pour respecter les regles NoBreak1 et NoBreak2 des */
  /* paves traverses par la frontiere de page. Le premier tour traite */
  /* les regles NoBreak2 et le 2eme tour les regles NoBreak1. */
  for (tour = 1; tour <= 2; tour++)
    {
      NoBr1 = tour == 2;  
      /* on traite les regles NoBreak1 au 2eme tour */
      do		
	/* on commence par la racine de la vue */
	{
	  NouvHPage = DeplCoupure(ElRacine->ElAbstractBox[VueNb-1], NoBr1, VueSch);
	  if (NouvHPage != 0)
	    {
	      AncienHPage = Hauteurffective;
	      Hauteurffective = NouvHPage;
	      /* demande au mediateur de positionner la nouvelle */
	      /* frontiere de page par rapport a tous les paves de la vue */
	      (void) MarqueCoupure(ElRacine->ElAbstractBox[VueNb-1],&Hauteurffective);
	      /* verifie que le mediateur a effectivement change' la */
	      /* hauteur de page */
	      if (Hauteurffective == AncienHPage)
		/* pas de changement, inutile de continuer sinon ca va */
		/* boucler indefiniment */
		NouvHPage = 0;
	    }
	}
      while (!(NouvHPage == 0));
    }
  /* place la marque de page dans l'arbre abstrait */
  PlaceMarque(ElRacine->ElAbstractBox[VueNb-1], ElRacine, pDoc, VueSch,
	      PaveTropHaut, PaveCoupeOrig, VueNb, frame, pPage);
}


/* ---------------------------------------------------------------------- */
/* |	PoseMarque pour la vue de numero VueNb, dans le document pDoc,	| */
/* |		insere dans l'arbre abstrait de racine ElRacine un	| */
/* |		element Marque de Page a la frontiere de page et detruit| */
/* |		tous les paves qui precedent cet element.		| */
/* |		Retourne 'vrai' si l'image restante est plus petite	| */
/* |		qu'une page.						| */
 /* |    Met a jour les pointeurs pT:pPageTraitee et 	        | */
 /* | 	pAT:pPageATraiter qui sont deux parametres en plus     	| */
 /* | 	retourne l'element marque page creee et ne fait plus 	| */
 /* | 	appel a TueAvantPage					| */
/* ---------------------------------------------------------------------- */
#ifdef __COLPAGE__
#ifdef __STDC__
static void PoseMarque(PtrElement ElRacine, int VueNb, PtrDocument pDoc, int frame, PtrAbstractBox *pT, PtrAbstractBox *pAT)

#else /* __STDC__ */
static void PoseMarque(ElRacine, VueNb, pDoc, frame, pT, pAT)
	PtrElement ElRacine;
	int VueNb;
	PtrDocument pDoc;
	int frame;
	PtrAbstractBox *pT;
	PtrAbstractBox *pAT;
#endif /* __STDC__ */

#else /* __COLPAGE__ */
#ifdef __STDC__
static PtrElement PoseMarque(PtrElement ElRacine, int VueNb, PtrDocument pDoc, int frame)

#else /* __STDC__ */
static PtrElement PoseMarque(ElRacine, VueNb, pDoc, frame)
	PtrElement ElRacine;
	int VueNb;
	PtrDocument pDoc;
	int frame;
#endif /* __STDC__ */
#endif /* __COLPAGE__ */

{
  PtrAbstractBox         pAb;
  PtrElement      pPage;
  /* boolean         ret; */
  boolean         PaveTropHaut;
  PtrAbstractBox         PaveCoupeOrig;
  PtrPSchema      pSchPres;
  int             Ent;
  PtrSSchema    pSS;
  int             VueSch;
#ifndef __COLPAGE__ 
  PtrElement      pElLib;
  boolean         stop;
  PtrAbstractBox         PavPrec, PavReaff;
  int             Hauteur, PosV, PosVFilet, CarCoupe, h, dh, HPageNormale;
#endif /* __COLPAGE__ */
 
  pPage = NULL;
  PaveTropHaut = False;
  VueSch = VueAAppliquer(ElRacine, NULL, pDoc, VueNb);
  /* on recherche le pave de plus haut niveau qui soit insecable et */
  /* traverse' par la frontiere normale de page. On aura besoin de */
  /* connaitre ce pave' s'il est plus haut qu'une page et qu'on doit */
  /* inserer une marque de page apres lui. */
  PaveCoupeOrig = NULL;	/* on n'a pas encore trouve' ce pave' */
  pAb = ElRacine->ElAbstractBox[VueNb-1]; /* on commence par le pave racine */
  do
    if (pAb->AbAfterPageBreak)
      PaveCoupeOrig = pAb;	/* pave au-dela de la limite de page */
    else if (pAb->AbOnPageBreak)
      {
	if (pAb->AbPresentationBox)
	  /* pave de presentation, donc insecable */
	  PaveCoupeOrig = pAb;
	else 
	  {
	    ChSchemaPres(pAb->AbElement, &pSchPres, &Ent, &pSS);
	    if (!pSchPres->PsAcceptPageBreak[Ent-1])
	      PaveCoupeOrig = pAb;	/* pave insecable */
	  }
	if (PaveCoupeOrig == NULL)
	  if (pAb->AbFirstEnclosed == NULL)
	    PaveCoupeOrig = pAb;	/* pave feuille */
	  else	
#ifdef __COLPAGE__
   /* on saute les paves de colonnes pour arriver a la derniere */
   /* cf. procedure CoupSurPage de pos.c (lignes #800) */
	  {
	    pAb = pAb->AbFirstEnclosed;
	    while (pAb->AbElement->ElTypeNumber == PageBreak+1
		   && (pAb->AbElement->ElPageType == ColBegin
		       || pAb->AbElement->ElPageType == ColComputed
		       || pAb->AbElement->ElPageType == ColUser
		       || pAb->AbElement->ElPageType == ColGroup)
	           && pAb->AbNext != NULL)
	      pAb = pAb->AbNext;
   }
#else /* __COLPAGE__ */
	    pAb = pAb->AbFirstEnclosed;
#endif /* __COLPAGE__ */
      } 
    else	/* le pave est avant la limite de page */
      pAb = pAb->AbNext;	/* passe au pave suivant */
  while (!(PaveCoupeOrig != NULL || pAb == NULL));
  /* place les marques de page sans tenir compte des boites de haut et de */
  /* bas de page de hauteur variable (notes de bas de page par exemple) */
 /* dans la version de Vincent, ModifVue est appelee avec une */
 /* Hauteur de Page fixe : maintenamt, on va chercher a la mettre a */
 /* jour a chaque fois que on ajoutera un elt associe en bas */
 /* de page ; ce sera fait dans CreePaves */
  PlacePage(&pPage, frame, &PaveCoupeOrig, &PaveTropHaut, pDoc,
	    VueSch, VueNb, ElRacine);
#ifdef __COLPAGE__
 /* le cas des pages trop hautes est supprime */
  if (pPage != NULL
      && pPage->ElPageType == PgComputed)
 /* on met a jour les pointeurs pT (page Traitee = page precedente) */
 /* et pAT (page A Traiter = premier pave de la page suivante) */
 /* avant de detruire les paves de la page precedente */
   if (*pT != NULL)
     {
       *pAT = (*pT)->AbNext;
       /* si pas d'erreur, pAT point sur le premier pave */
       /* de l'element marque page qui vient d'etre cree, */
       /* a savoir pPage */
       /* ce n'est plus toujours vrai avec les colonnes */    
       if ((*pAT)->AbElement != pPage)
         printf("erreur PlaceMarque : les paves MP crees sont mal chaines", "\n");
       *pT = NULL; /* le pave correspondant risque d'etre detruit */
     }
#else /* __COLPAGE__ */
  if (pPage != NULL)
    /* on a insere' au moins une marque de page dans l'arbre abstrait */
    /* On verifie que la page n'est pas trop haute (il peut y avoir des */
    /* boites de haut ou de bas de page qui augmentent la hauteur de page). */
    /* Inutile d'essayer de reduire la hauteur de la page s'il y a un */
    /* pave insecable plus haut que la page */ 
    if (!PaveTropHaut)
      /* cherche d'abord la boite du filet de separation de pages: c'est */
      /* la premiere boite contenue dans la boite de page qui n'est */
      /* pas une boite de presentation. */
      {
	if (pPage->ElAbstractBox[VueNb-1] != NULL)
	   pAb = pPage->ElAbstractBox[VueNb-1]->AbFirstEnclosed;
	else
	   pAb = NULL;
	stop = False;
	do
	  if (pAb == NULL)
	    stop = True;
	  else if (!pAb->AbPresentationBox)
	    stop = True;
	  else	pAb = pAb->AbNext;
	while (!(stop));
	if (pAb == NULL)
	  PosVFilet = 0;
	else
	  /* demande au mediateur la position verticale de cette boite filet */
	  HautCoupure(pAb, True, &Hauteur, &PosVFilet, &CarCoupe);
	/* verifie la hauteur de la page */
	if (PosVFilet > HauteurPage + HauteurBasPage)
	  /* la page est trop haute */
	  /* dh: hauteur qui depasse de la page standard */
	  {
	    dh = PosVFilet-HauteurPage - HauteurBasPage;
	    /* cherche le pave qui precede la marque de page */
	    PavPrec = pPage->ElAbstractBox[VueNb-1];
	    stop = False;
	    do
	      if (PavPrec == NULL)
		stop = True;
	      else if (PavPrec->AbPrevious != NULL)
		stop = True;
	      else	
		PavPrec = PavPrec->AbEnclosing;
	    while (!(stop));
	    if (PavPrec != NULL)
	      /* la page n'est pas vide */
	      /* sauve la hauteur de page normale */
	      {
		HPageNormale = HauteurPage;
		/* reduit la hauteur de page */
		HauteurPage = Hauteurffective - dh;
		if (RefAssocHautPage != NULL)
		 if (RefAssocHautPage->ElAbstractBox[VueNb-1] != NULL)
		  /* la page deborde parce qu'on vient de creer dans la boite */
		  /* de saut de page precedente une boite de haut de page */
		  /* contenant des elements associes. On deplace la marque de */
		  /* page courante juste au-dessus de la premiere reference au */
		  /* premier element associe' du haut de page. */
		  {
		    HautCoupure(RefAssocHautPage->ElAbstractBox[VueNb-1], True,
				&Hauteur,&PosV,&CarCoupe);
		    if (HPageNormale < PosV)
		      HauteurPage = PosV;
		    else	
		      HauteurPage = HPageNormale;
		  }
		if (HauteurPage < HMinPage)
		  HauteurPage = HMinPage;
		/* detruit le saut de page et ses paves */
		DetPavVue(pPage, pDoc, False, VueNb);
		/* traitement des elements demandant des coupures speciales */
		if (ThotLocalActions[T_Exc_Page_Break_Detruit_Pave]!= NULL)
		  (*ThotLocalActions[T_Exc_Page_Break_Detruit_Pave])
		      (pPage, pDoc, VueNb);
		if (PageConcernee == pPage)
		  NbBoiteHautPageACreer = 0;
		/* signale les paves morts au Mediateur */
		if (!VueAssoc(ElRacine))
		  PavReaff = pDoc->DocViewModifiedAb[VueNb-1];
		else	
		  PavReaff = pDoc->DocAssocModifiedAb[ElRacine->ElAssocNum-1];
		Hauteurffective = HauteurPage;
		(void) ModifVue(frame, &Hauteurffective, PavReaff);
		/* libere tous les paves morts de la vue */ 
		LibPavMort(pAb);
		/* detruit la marque de page a liberer dans l'arbre abstrait */
		SupprMarquePage(pPage, pDoc, &pElLib);
		/* signale au Mediateur les paves morts par suite de */
		/* fusion des elements precedent et suivant les marques */
		/* supprimees. */
		if (!VueAssoc(ElRacine))
		  PavReaff = pDoc->DocViewModifiedAb[VueNb-1];
		else	
		  PavReaff = pDoc->DocAssocModifiedAb[ElRacine->ElAssocNum-1];
		if (PavReaff != NULL)
		  {
		    h = Hauteurffective; 
		    (void) ModifVue(frame, &h, PavReaff);
		  }
		/* libere les elements rendus inutiles par les fusions */
		DeleteElement(&pElLib);
		pPage = NULL;
		CompteurPages--;
		/* on place les marques de page plus haut */
		PlacePage(&pPage, frame, &PaveCoupeOrig, &
			  PaveTropHaut, pDoc, VueSch, VueNb, ElRacine);
		/* retablit la hauteur de page */
		HauteurPage = HPageNormale;
	      }
	  }
      }
 /* on supprime l'appel a TueAvantPage (fait dans Page) */
 return pPage;
#endif /* __COLPAGE__ */
}


#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* | HautPage met a jour les variables HauteurCoupPage et HauteurBasPage| */
/* |		selon le type de page auquel appartient l'element	| */
/* |		Marque Page pointe par pElPage.				| */
/* |		Vue indique le numero de la vue pour laquelle on	| */
/* |		construit des pages.					| */
 /*      VueSch indique le numero de la vue dans le schema.      | */
 /*      HauteurCoupPage : variable globale de thot (partagee    | */
 /*      entre page.c et crimabs.c).		     		| */
 /*      HauteurRefBasPage : variable globale du module page.	| */
 /*      Cette procedure ne fait rien si pElPage est une marque  | */
 /*      colonne.                                                | */
/* ---------------------------------------------------------------------- */


#ifdef __STDC__
static void HautPage(PtrElement pElPage, int Vue, int VueSch, int frame, PtrDocument pDoc)

#else /* __STDC__ */
static void HautPage(pElPage, Vue, VueSch, frame, pDoc)
	PtrElement pElPage;
	int Vue;
	int VueSch;
	int frame;
	PtrDocument pDoc;
#endif /* __STDC__ */

{
  PtrPRule    pR;
  PtrPSchema      pSchP;
  boolean         stop;
  PtrPSchema      pSc1;
   PtrAbstractBox   pP, pAb, pCorps;
   int             Hauteur, PosV, CarCoupe; /* pour calcul page */
   int             h;
   boolean         bool, nouveau;
   PtrPSchema      pSPR;
   PtrSSchema    pSS;
   PtrAttribute     pAttr;
   PtrPRule    pRegleDimV, pRStd;
   AbDimension    *pPavD1;
 PtrElement pSuiv;

 /* on ne fait rien si pElPage est une marque colonne car pour */
 /* l'instant, on ne permet pas la creation de haut ou bas de */
 /* colonnes */
 if (pElPage->ElPageType == PgBegin
	    || pElPage->ElPageType == PgComputed
	    || pElPage->ElPageType == PgUser)
 {
  if (pElPage->ElPageType == PgBegin)
    {
      /* Nouveau type de page, on determine la hauteur des pages */
      pR = NULL;
 /* il faut rechercher la regle sur l'element suivant */
 /* car les elements marque page debut sont places AVANT les elements*/
 /* qui portent la regle page */
 if (pElPage->ElNext != NULL)
   {
       pSuiv = pElPage->ElNext;
       /* on saute les eventuelles marques de colonnes */
       /* ou de page (pour d'autres vues par exemple ?) */  
       while (pSuiv != NULL
 		     && pSuiv->ElTypeNumber == PageBreak+1)
         pSuiv = pSuiv->ElNext; 
       /* on cherche uniquement sur pPsuiv car normalement l'element */
       /* marque page debut a ete place juste devant l'element qui */
       /* portait la regle page correspondante */
	      if (pSuiv != NULL)
         pR = ReglePage(pSuiv, pElPage->ElViewPSchema, &pSchP);
    }
	if (pR == NULL && pElPage->ElParent != NULL)
	  /* si pSuiv ne portait pas de regle, */
	  /* l'element englobant porte-t-il une regle page ? */
	  pR = ReglePage(pElPage->ElParent,
			      	pElPage->ElViewPSchema, &pSchP);

      if (pR != NULL) /* on a trouve la regle page */ 
	{
	  pSc1 = pSchP;
	  /* HauteurRefBasPage = Hauteur minimum du bas de page */
   /* correspond a la taille des elements fixes places en bas de */
   /* page */
	  HauteurRefBasPage = pSc1->PsPresentBox[pR->PrPresBox[0]-1].PbFooterHeight;
 /*  HauteurRefHautPage = pSc1->PsPresentBox[pR->PrPresBox[0]-1].PbHeaderHeight; */
   HauteurRefHautPage = HauteurRefBasPage; /* temporaire */
	  /* code inutile : le plus souvent, le compilateur ne peut pas  */
	  /* connaitre la hauteur des hauts et bas de page car ils sont  */
	  /* composes de plusieurs boites de presentation */
	  /* il faut attendre que le mediateur ait calcule leur hauteur */
	  /* pour pouvoir avoir une hauteur de reference de la page */
	  /* cherche la regle de hauteur de la boite page */
   /* c'est maintenamt la hauteur du corps */
	  pR = pSc1->PsPresentBox[pR->PrPresBox[0]-1].PbFirstPRule;
	  stop = False;
	  do
	    if (pR == NULL)
	      stop = True;
	    else if (pR->PrType >= PtHeight)
	      stop = True;
	    else
	      pR = pR->PrNextPRule;
	  while (!(stop));
	  if (pR != NULL)
	    if (pR->PrType == PtHeight)
	      if (!pR->PrDimRule.DrPosition)
         HauteurTotalePage = pR->PrDimRule.DrValue +
                             HauteurRefHautPage + HauteurRefBasPage ;
         /* HauteurTotalePage = hauteur max totale de la page */
         /* definie par l'utilisateur ; exemple A4 = 29.7 cm */
	  	/* toujours exprimee en unite fixe (verifie par le compilo) */
	}
    } /* fin pElPage->ElPageType == PgBegin */
  
 /* si les paves Haut et Bas ont ete crees, on fait reevaluer */
 /* leur hauteur par le mediateur pour positionner correctement */
 /* les differentes variables de hauteur */

 if (pElPage->ElAbstractBox[Vue-1] != NULL)
   {
     /* l'element a un pave dans la vue. On recherche ses paves */
     /* haut et bas */
     pP = pElPage->ElAbstractBox[Vue-1];
     if (pP->AbPresentationBox) 
       {
         /* il y a un haut de page : on evalue sa hauteur */
         pAb = pP;
         h = -1; /* changement de signification de h */
         /* on signale au mediateur les paves (au cas ou il ne les */
         /* ait pas encore vus) */
         /* on appelle Modifvue a partir du pave haut de page */
         bool = ModifVue(frame, &h, pAb);
         /* calcul de la hauteur du pave haut ou bas de page */
         HautCoupure(pAb, True, &Hauteur, &PosV, &CarCoupe);
         HauteurRefHautPage = Hauteur;
       }
     /* on saute le corps de page pour voir s'il y a un bas de page */
     while(pP->AbPresentationBox)
       pP = pP->AbNext;
     pCorps = pP; /* on memorise le corps de page */
     if (pP->AbNext != NULL 
         && pP->AbNext->AbElement == pElPage
         && pP->AbNext->AbLeafType == LtCompound) /* pas filet */
       {
         /* il y a un bas : on evalue sa hauteur */
         pAb = pP->AbNext;
         h = -1; /* changement de signification de h */		
         /* on signale au mediateur les paves (au cas ou il ne les */
         /* ait pas encore vus) */
         /* on appelle Modifvue a partir du pave haut de page */
         bool = ModifVue(frame, &h, pAb);
         /* calcul de la hauteur du pave haut ou bas de page */
         HautCoupure(pAb, True, &Hauteur, &PosV, &CarCoupe);
         HauteurRefBasPage = Hauteur;
       }
     /* mise a jour de la hauteur de reference de la page */
     /* mise a jour de la hauteur du corps de page */
     /* on met (ou on modifie) une regle de presentation specifique */
     /* cherche d'abord la regle qui s'applique a l'element */
     pRStd = ReglePEl(pElPage, &pSPR, &pSS, 0, NULL, VueSch,
                    PtHeight, False, True, &pAttr);
     /* cherche si l'element a deja une regle de hauteur specifique */
     pRegleDimV = ChReglePres(pElPage, PtHeight, &nouveau, pDoc, Vue);
     if (nouveau)
       /* on a cree' une regle de hauteur pour l'element */
       {
         pR = pRegleDimV->PrNextPRule; /* on recopie la regle standard */
         *pRegleDimV = *pRStd;
         pRegleDimV->PrNextPRule = pR;
         pRegleDimV->PrCond = NULL; /* pas de condition associee */
         pRegleDimV->PrViewNum = VueSch; /* inutile ? */
       }
     /* change le parametre de la regle */
     pRegleDimV->PrDimRule.DrValue =
             HauteurTotalePage - HauteurRefBasPage - HauteurRefHautPage;
     /* pCorps = pave du corps de page */
     /* applique la nouvelle regle specifique Verticale */
     if (pRegleDimV != NULL)
       Applique(pRegleDimV, pSPR, pCorps, pDoc, pAttr, &bool);
     pPavD1 = &pCorps->AbHeight;
     pPavD1->DimMinimum = True; /* regle de hauteur minimum */

   }

 /* A chaque nouvel element Marque Page, on met a jour les */
 /* hauteurs significatives de la page */
 HauteurCoupPage = HauteurTotalePage - HauteurRefBasPage;
         /* HauteurCoupPage = hauteur haut de page + hauteur du corps */
         /* Attention on suppose que le haut de page ne peut pas */
         /* depasser la valeur HauteurCoupPage ... faut-il prevoir */
         /* des controles pour le verifier (cas des schemas !) ? */

 HauteurBasPage = HauteurRefBasPage;
 HauteurHautPage = HauteurRefHautPage;
 } /* fin cas ou pElPage est une marque page */
}


#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |	DetrImAbs detruit l'image abstraite de la vue concernee et	| */
/* |		efface sa frame si la vue est une vue pour VusSch	| */
 /* Vue = numero d'elt assoc si vue associee sinon      */
 /* Vue = numero de vue si vue d'arbre principal        */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void DetrImAbs_Pages(int Vue, boolean Assoc, PtrDocument pDoc, int VueSch)

#else /* __STDC__ */
static void DetrImAbs_Pages(Vue, Assoc, pDoc, VueSch)
	int Vue;
	boolean Assoc;
	PtrDocument pDoc;
	int VueSch;
#endif /* __STDC__ */

{
  PtrAbstractBox         pAb;
  int             h;
  /* boolean        tropcourt;*/
  int            frame;
  PtrAbstractBox        PavRacine;
  boolean 	adetruire;
  
 frame = 1; /* initialisation (pour le compilateur !) */
 PavRacine = NULL; /* initialisation (pour le compilateur !) */
 if (Assoc)
   {
      /* c'est une vue d'elements associes */
      PavRacine = pDoc->DocAssocRoot[Vue - 1]->ElAbstractBox[0];
      frame = pDoc->DocAssocFrame[Vue - 1];
      adetruire =  (PavRacine != NULL);
   }
 else
    /* on verifie si c'est bien une vue correspondant a la vue du schema */
     if (pDoc->DocView[Vue - 1].DvPSchemaView == VueSch)
       {
         PavRacine = pDoc->DocViewRootAb[Vue - 1];
         frame = pDoc->DocViewFrame[Vue - 1];
	 adetruire = (PavRacine != NULL);
       }
     else 
       adetruire = False; /* rien a faire */
 if (adetruire)
   {
  /* tous les paves englobes par le pave racine de la vue sont marques*/
  /* morts */
    pAb = PavRacine->AbFirstEnclosed;
  while (pAb != NULL)
    {
      TuePave(pAb);
      pAb = pAb->AbNext;
    }
  /* ceci est signale au Mediateur */
#ifdef __COLPAGE__
 /* vide la chaine des regles en retard sur la racine */
 /* normalement doit etre deja vide ! */
 ApplReglesRetard(PavRacine->AbFirstEnclosed->AbElement, pDoc);
 /* libere tous les paves morts de la vue */
  /* ceci est signale au Mediateur */
 h = -1; /* changement de signification de h */
#else /* __COLPAGE__ */
  h = 0;
#endif /* __COLPAGE__ */
  (void) ModifVue(frame, &h, PavRacine);
  /* libere tous les paves morts de la vue */
  LibPavMort(PavRacine);
  /* indique qu'il faudra reappliquer les regles de presentation du */
  /* pave racine, par exemple pour recreer les boites de presentation */
  /* creees par lui et qui viennent d'etre detruites. */
  PavRacine->AbSize = -1;
#ifdef __COLPAGE__
 PavRacine->AbTruncatedTail = True;
#endif /* __COLPAGE__ */
  /* on marque le pave racine complet en tete pour que CreePaves */
  /* engendre effectivement les paves de presentation cree's en tete */
  /* par l'element racine (regles CreateFirst). */
  if (PavRacine->AbLeafType == LtCompound)
     PavRacine->AbTruncatedHead = False;

   } /* fin adetruire */
}


#ifdef __COLPAGE__
	/*** AjoutePageEnFin supprime' ***/
#else /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* | AjoutePageEnFin	ajoute une marque de page a la fin de la vue	| */
/* |	VueSch de l'arbre de racine pElRacine s'il n'y en a pas deja une| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void AjoutePageEnFin(PtrElement pElRacine, int VueSch, PtrDocument pDoc,
	boolean withAPP)

#else /* __STDC__ */
void AjoutePageEnFin(pElRacine, VueSch, pDoc, withAPP)
	PtrElement pElRacine;
	int VueSch;
	PtrDocument pDoc;
	boolean withAPP;
#endif /* __STDC__ */

{
  PtrElement	pEl;
  PtrElement	pElPage;
  PtrPSchema	pSchP;
  int		cpt;
  boolean	PageEnFin;
  boolean	stop, stop1, ok;
  NotifyElement	notifyEl;
  int		nbFreres;

  /* cherche d'abord s'il n'y en pas deja une */
  PageEnFin = False;	/* on n'en pas encore vu */
  if (pElRacine == NULL)
     pEl = NULL;
  else
     pEl = pElRacine->ElFirstChild;
  stop = False;		/* descend l'arbre */
  if (pEl != NULL)
  do
    if (pEl->ElTypeNumber == PageBreak + 1 && pEl->ElViewPSchema == VueSch)
      {
	/* c'est une marque de page pour cette vue */
	/* saute les marques de page suivantes, qui concernent d'autres vues */
	stop1 = False;
	do
	  if (pEl->ElNext == NULL)
	    /* dernier element de ce niveau */
	    PageEnFin = True;
	    /* la marque trouvee est bien en fin de vue */
	  else
	    {	/* examine l'element suivant */
	      pEl = pEl->ElNext;
	      if (pEl->ElTypeNumber != PageBreak+1)
		stop1 = True;
	        /* ce n'est pas une marque de page, la */
	        /* marque trouvee n'est donc pas en fin */
	    }
	while (!(stop1 || PageEnFin));
      } 
    else
      /* ce n'est pas une marque de page pour la vue */
      if (pEl->ElNext != NULL)
	pEl = pEl->ElNext; /* passe au suivant */
      else
	/* il n'y a pas d'element suivant */
	if (pEl->ElTerminal)
 	  stop = True; /* c'est une feuille, on s'arrete */
	else
	  pEl = pEl->ElFirstChild;	/* descend d'un niveau */
  while (!(stop || pEl == NULL || PageEnFin));

  pEl = pElRacine->ElFirstChild;
  if (pEl != NULL && !PageEnFin)
    /* il n'y a pas de marque de page a la fin de la vue */
    /* cree une marque de page */
    {
      /* cherche le dernier fils de la racine */
      nbFreres = 1;
      while (pEl->ElNext != NULL)
	{
	nbFreres++;
	pEl = pEl->ElNext;
	}
      if (withAPP)
	{
        /* envoie l'evenement ElemNew.Pre */
        notifyEl.event = TteElemNew;
        notifyEl.document = (Document)IdentDocument(pDoc);
        notifyEl.element = (Element)(pEl->ElParent);
        notifyEl.elementType.ElTypeNum = PageBreak+1;
        notifyEl.elementType.ElSSchema = (SSchema)(pElRacine->ElSructSchema);
        notifyEl.position = nbFreres;
        ok = !ThotSendMessage((NotifyEvent *)&notifyEl, True);
	}
      else
	ok = True;
      if (ok)
	{
	/* cree l'element marque de page */
	pElPage = NewSubtree(PageBreak+1, pElRacine->ElSructSchema, pDoc,
			     pEl->ElAssocNum, True, True, True, True);
	/* insere la nouvelle marque de page apres le dernier fils */
	InsertElementAfter(pEl, pElPage);	/* remplit cette marque de page */
	pElPage->ElPageType = PgComputed;
	pElPage->ElViewPSchema = VueSch;
	/* cherche le compteur de page a appliquer */
	cpt = CptPage(pElPage, VueSch, &pSchP);
	if (cpt == 0)
	   /* page non numerotee */
	   pElPage->ElPageNumber = 1;
	else
	   /* calcule le numero de page */
	   pElPage->ElPageNumber = ComptVal(cpt, pElPage->ElSructSchema, pSchP,
					 pElPage, VueSch);
#ifndef PAGINEETIMPRIME
	/* envoie l'evenement ElemNew.Post */
	if (withAPP)
	  NotifySubTree(TteElemNew, pDoc, pElPage, 0);
#endif PAGINEETIMPRIME
	}
    }
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__
/**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*/
/*      Nouvelle procedure pour les colonnes              */   /**CL*/
/**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*/
/* ---------------------------------------------------------------------- */
/* |	Equilibrer_Col equilibre le groupe de colonnes contenues dans le| */
/* |		dernier pave de groupe de colonnes de PavRacine         | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void Equilibrer_Col (PtrDocument pDoc, PtrAbstractBox PavRacine, int VueNb,  int VueSch)
#else /* __STDC__ */
static void Equilibrer_Col (pDoc, PavRacine, VueNb, VueSch)
      PtrDocument pDoc;
      PtrAbstractBox PavRacine;
      int VueNb;
      int VueSch;
      
#endif /* __STDC__ */
{
  PtrAbstractBox         pP;
  int             Hauteur, PosV, CarCoupe, h;
  PtrElement      pPage, ElRacine;
  boolean         bool;
  boolean         PaveTropHaut;
  PtrAbstractBox         PaveCoupeOrig;
  int             frame;
  FILE            *list;
 
  pP = PavRacine->AbFirstEnclosed;
  while (pP != NULL && pP->AbPresentationBox)
    pP = pP->AbNext; /* on est sur le corps de page */
  if (pP != NULL)
    pP = pP->AbFirstEnclosed; /* premier groupe de colonnes */
  while (pP != NULL && pP->AbNext != NULL)
    pP = pP->AbNext; /* dernier groupe de colonnes */
  if (pP->AbElement->ElTypeNumber == PageBreak+1
      && pP->AbElement->ElPageType == ColGroup)
    {
      /* version simple : le groupe de colonnes ne contient qu'une seule */
      /* colonne qu'il faut couper en deux */
      /* TODO a generaliser a plus de deux colonnes */
      frame = pDoc->DocViewFrame[VueNb - 1];
      /* on signale les paves au mediateur pour qu'il cree les boites */
      h = 0;
      bool = ModifVue(frame, &h, PavRacine);
      pP = pP->AbFirstEnclosed; /* pP pave de colonne gauche */
      /* calcul de la hauteur du pave colonne gauche */
      HautCoupure(pP, True, &Hauteur, &PosV, &CarCoupe);
      /* Hauteur = dimension verticale du pave colonne simple */
      /* on fait evaluer la coupure de colonne avec h = Hauteur / 2 */
      /* on appelle Modifvue a partir du pave colonne simple */
 ChangeRHPage (PavRacine, pDoc, VueNb);
      h = PosV + (Hauteur / 2);
      bool = ModifVue(frame, &h, PavRacine);
      /******
	list = fopen("/perles/roisin/debug/equil","w");
	if (list != NULL)
	{
	NumPav(pDoc->DocViewRootAb[VueNb-1]);
	AffPaves(pDoc->DocViewRootAb[0], 2, list);
	fclose(list);
	list = fopen("/perles/roisin/debug/btequil","w");
	if (list != NULL)
	{
	WrPav(PavRacine, 0, list);
	fclose(list);
	}
	}
	*******************/
      if (!bool)
	{
          /* on insere une marque colonne */
	  pPage = NULL;
	  PaveCoupeOrig = NULL;
	  PaveTropHaut = False;
	  ElRacine = PavRacine->AbElement;
          PlacePage(&pPage, frame, &PaveCoupeOrig, &PaveTropHaut, pDoc,
	    VueSch, VueNb, ElRacine);
	  /* on remet le volume libre a -1 */
        }
    }
 
} /* fin de Equilibrer_Col */

/**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*/
  /*    Fin de nouvelle procedure pour les colonnes        */  /**CL*/
/**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*//**CL*/
#endif /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |	Pages l'utilisateur demande le (re)decoupage en pages de la	| */
/* |		vue de numero Vue pour le document pointe' par pDoc.	| */
/* |		Si Assoc est vrai, c'est la vue d'elements associes de	| */
/* |		numero Vue qui doit etre traitee			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void Pages(PtrDocument pDoc, int Vue, boolean Assoc)

#else /* __STDC__ */
void Pages(pDoc, Vue, Assoc)
	PtrDocument pDoc;
	int Vue;
	boolean Assoc;
#endif /* __STDC__ */

{
  PtrAbstractBox	PavRacine, pP;
  int		frame, volume, volprec, VueNb, cpt;
  boolean	tropcourt;
  PtrPSchema	pSchP;
  PtrElement    pElRacine, PremPage, pPage;
#ifndef PAGINEETIMPRIME
  PtrElement	SelPrem, SelDer;
  int		SelPremCar, SelDerCar;
  boolean       sel;
#endif 
  int		VueSch;
  PtrElement	pEl1;
  int		v;
  boolean	complet;
  boolean	prempage; 
 PtrAbstractBox 	pavPagePrec; 
#ifdef __COLPAGE__
 /*rienAjoute retire */
   PtrElement    pElPage1;
   PtrAbstractBox       pCorps, pPageTraitee, pPageATraiter, PavR;
   int             Hauteur, PosV, CarCoupe, h; /* pour calcul page */
   boolean       nouveau;
   PtrPRule  pRegleDimV;
 FILE *list;
#else /* __COLPAGE__ */
boolean	rienAjoute;
int b;
PtrPSchema	pSchPage;
#endif /* __COLPAGE__ */
#ifdef PAGINEETIMPRIME
  int		h;
#endif PAGINEETIMPRIME

  PaginationEnCours = True;
#ifdef __COLPAGE__
  CompteurPages = 0;
#endif /* __COLPAGE__ */
    prempage = True;
  CompteurPages = 1;
  PremPage = NULL;
 pavPagePrec = NULL;
  NbBoiteHautPageACreer = 0;
  if (Assoc)	/* c'est une vue d'elements associes */
    		/* le nuemero d'element associe est Vue */
    {
      VueNb = 1;	/* numero dans le document de la vue a paginer */
      VueSch = 1;	/* numero dans le schema de la vue a paginer */
      pElRacine = pDoc->DocAssocRoot[Vue - 1];
      frame = pDoc->DocAssocFrame[Vue - 1];
    } 
  else
    {
      VueNb = Vue;	/* numero dans le document de la vue a paginer */
      /* numero dans le schema de la vue a paginer */
      VueSch = VueAAppliquer(pDoc->DocRootElement, NULL, pDoc, VueNb);
      pElRacine = pDoc->DocRootElement;
      frame = pDoc->DocViewFrame[Vue - 1];
    }
#ifdef __COLPAGE__
 /* si le premier fils de la racine n'est pas une marque de page, le */
 /* document n'est pas mis en page */
  /* le compilateur prs doit verifier si la racine possede une regle */
 /* page; donc si le doc est pagine, on doit toujours */
 /* trouver un premier pave corps */
 /* de page correspondant a la marque de debut d'element */
 /* (premier fils de la racine) */
   /* recherche du premier element Marque Page de la vue VueSch */
 pElPage1 = pElRacine->ElFirstChild;
 while (pElPage1 != NULL && pElPage1->ElTypeNumber == PageBreak + 1
        && pElPage1->ElViewPSchema != VueSch)
   pElPage1 = pElPage1->ElNext;
 if (pElPage1 != NULL && pElPage1->ElTypeNumber == PageBreak + 1)
   {
 /* document mis en pages, on peut continuer la procedure */
#endif /* __COLPAGE__ */

#ifndef PAGINEETIMPRIME
  sel = AnnuleSelectionPage (pDoc, VueSch, &SelPrem, &SelDer, &SelPremCar, &SelDerCar);
#endif /* PAGINEETIMPRIME */
  /* detruit l'image abstraite de la vue concernee, en conservant la racine */
  if (Assoc)
    /* le nuemero d'element associe est Vue */
    DetrImAbs_Pages(Vue, Assoc, pDoc, VueSch);
  else
    for (v = 1; v <= MAX_VIEW_DOC; v++)
      DetrImAbs_Pages(v, Assoc, pDoc, VueSch);
  /* destruction des marques de page */
  DtrMarquesDePages (pDoc, pElRacine, VueSch);

#ifdef __COLPAGE__
 /* la suite du code est different */
  HauteurCoupPage = 0;
HauteurTotalePage = 0;
  HauteurRefBasPage = 0;	/* cree les paves du debut de la vue */
 pPageTraitee = NULL;
  if (Assoc)
 {
   /* ATTENTION si Assoc est vrai, Vue est le numero d'element associe */
   pDoc->DocAssocNPages[Vue - 1] = -1; /* creation des paves par le volume */
          pDoc->DocAssocFreeVolume[Vue - 1] = pDoc->DocAssocVolume[Vue - 1];
 }
  else
 {
   pDoc->DocViewNPages[Vue - 1] = -1; /* creation des paves par le volume */
          pDoc->DocViewFreeVolume[Vue - 1] = pDoc->DocViewVolume[Vue - 1];
   /* modification de ce volume si trop faible */
   if (pDoc->DocViewFreeVolume[Vue - 1] < 100)
     pDoc->DocViewFreeVolume[Vue - 1] = 100;
 }
 /* on fait evaluer HauteurCoupPage avant d'appeler CreePaves */
 /* car HauteurCoupPage peut eventuellement dynamiquement changer si */
 /* il y a des elements en bas de page */
 /* pElPage1 contient le 1er element marquepage de cette vue */
 /* attention Vue = no d'elt associe (si vue assoc) */
 /*                 ou VueNb si arbre principal */
 /*           VueNb = vue d'affichage (tj 1 si vue assoc) */
 /* mis en commentaire pour tester l'equilibrage ?? */
 /* HautPage(pElPage1, VueNb, VueSch, frame, pDoc); */
 ArretAvantCreation = False;  /*  on veut creer la 1ere MP */
 TrouveMPHB = False; /* pour savoir la cause de l'arret de */
 A_Equilibrer = True;
 /* creation des paves: soit volume soit MP ou d'un elt asscoc HB */
  /* CreePaves doit s'arreter apres creation d'1 MP ou elt assoc */
 /* le booleen TrouveMPHB n'est jamais mis a faux par CreePaves */
 /* il est seulement affecte a vrai lorsqu'une MP ou un elt assoc */
 /* est trouve par CreePaves. C'est a Page de le remettre a faux */
     RefAssocHBPage = NULL; /* on initialise RefAssocHBPage */
  pP = CreePaves(pElRacine, pDoc, VueNb, True, True, &complet);
 /* mise a jour de PavRacine apres la creation des paves */
 /* dans le cas de l'appel depuis print, il n'y avait aucun pave cree */
  if (Assoc) 
    PavRacine = pElRacine->ElAbstractBox[0];
  else 
    PavRacine = pDoc->DocViewRootAb[Vue - 1];
  volume = 0;
 /* on change la regle des paves corps de page (sauf si MP mise */
 /* par l'utilisateur) : hauteur = celle du contenu */
 /* pour permettre a ModifVue de determiner la coupure de page */
 ChangeRHPage (PavRacine, pDoc, VueNb);
  /* fait calculer l'image par le Mediateur */
  Hauteurffective = HauteurCoupPage;
  tropcourt = ModifVue(frame, &Hauteurffective, PavRacine);
  /* cherche le pave de la premiere marque de page */
  pPageATraiter = PavRacine->AbFirstEnclosed;
 /* c'est le premier fils de la racine */
 while (pPageATraiter != NULL)  
          /* traite une page apres l'autre */
 /* pPageATraiter contient le premier pave de la page a traiter */
 /* pPageTraitee contient le dernier pave de la derniere page traitee */
          {
 pP = pPageATraiter;
 /* On prend la hauteur de ce type de page */
 /* cette hauteur change si la page est une PgBegin (nouveau */
 /* type de marque page) */
 /* sinon, HautPage positionne HauteurCoupPage a la valeur donnee */
 /* par la regle courante. Remise a jour necessaire car la page */
 /* precedente pouvait avoir des elements en bas de page qui */
 /* ont fait changer la hauteur de page. Cf. CrPavHB */
 /* HautPage(pP->AbElement, VueNb, VueSch, frame, pDoc); */
 /* TODO faut-il verifier que pPageATraiter n'est pas NULL ? */
 /* detruit les paves de la page precedente (si existe) pour */
 /* avoir une coupure correcte lors du prochain ModifVue */
 /* calcule le volume de ce qui a ete detruit */
 /* pour en regenerer autant ensuite */
	volprec = PavRacine->AbVolume;
 /* rappel : TueAvantPage appelle ModifVue apres destruction */
 tropcourt = TueAvantPage(pP, frame, pDoc, VueNb);
 PavRacine->AbTruncatedHead = True;
 /* si on a detruit des paves, la racine est coupee en tete */
 volume += volprec - PavRacine->AbVolume;
 if (pP->AbElement->ElTypeNumber != PageBreak + 1)
   printf("erreur pagination : pave <> page sous racine", "\n");
 /* on renumerote cette marque de page */
 pEl1 = pP->AbElement;
 /* cherche le compteur de page a appliquer a cette page */
 cpt = CptPage(pP->AbElement, VueSch, &pSchP);
 if (cpt == 0)
   /* page non numerotee */
   {
     CompteurPages++;
     pEl1->ElPageNumber = CompteurPages;
   }
 else        /* calcule le numero de page */
   pEl1->ElPageNumber =
     ComptVal(cpt, pEl1->ElSructSchema, pSchP, pP->AbElement,VueSch);
        /* affiche un message avec le numero de page */
 AffMsgPage (pDoc, pElRacine, pP->AbElement, VueSch, Assoc, &prempage)

        if (PremPage == NULL)
          PremPage = pP->AbElement;
 /* cherche le dernier pave de la marque de page */
 while (pP->AbNext != NULL
        && pP->AbElement == pP->AbNext->AbElement)
   {
     if (!pP->AbPresentationBox)
       pCorps = pP; /* on memorise le corps de page */
            pP = pP->AbNext;
   }
 /* remarque : pCorps existe toujours */
 /* on conserve ce pointeur pour ne pas revenir sur cette page */
 /* et pour determiner quand la page suivante sera creee */
 pPageTraitee = pP;
 pPageATraiter = pP->AbNext; /* cette page est finie de traiter */
                                 /* on passe a la suivante */
 /* si une page suivante existe, on ne la prend en compte */
 /* que si le corps de la page qui vient d'etre traitee n'est */
 /* pas coupee par la limite */
 if (pPageATraiter != NULL && pCorps->AbOnPageBreak)
   pPageATraiter = NULL;
 while (pPageATraiter == NULL && (PavRacine->AbTruncatedTail ||!tropcourt))
   {
     /* on boucle jusqu'a creer le pave de la marque page suivante */
     /* soit marque page utilisateur soit mise par PoseMarque */
     /* mais on ne veut pas en creer trop a la fois pour prendre */
     /* correctement en compte la hauteur de chaque page en fonction */
     /* des elements associes en haut et bas de page */
     /* c'est dans cette boucle que l'on cree les colonnes */
     /* car on n'utilise pas pPageATraiter pour les colonnes */
     ArretAvantCreation = True; /* var globale pour CreePaves */
                      /* CreePaves doit s'arreter AVANT creation */
                      /* d'1 MP ou d'un elt asscoc HB */
	    A_Equilibrer = True;
     while (pPageATraiter == NULL && PavRacine->AbTruncatedTail
            && tropcourt)
       /* boucle d'ajout des paves */
       {
         RefAssocHBPage = NULL; /* on reinitialise RefAssocHBPage */
         PavAssocADetruire = NULL; /* et PavAssocADetruire */
         TrouveMPHB = False; /* pour savoir la cause de l'arret de */
         /* creation des paves: soit volume soit MP soit elt assoc HB */
         /* on boucle jusqu'a avoir assez de paves pour faire */
         /* une page */
                /* on ajoute au moins 1000 caracteres a l'image */

                if (volume < 1000)
		  /* indique le volume qui peut etre cree */
                  volume = 1000;
                do
		  {
                    if (volume == 0)
		      /* cas ou on boucle et ou volume depasse la taille max*/
		      /* cas d'erreur */
               printf("erreur pagination : boucle dans creation image ", "\n");
                    if (Assoc)
               /* ATTENTION si Assoc est vrai, il faut utiliser Vue */
               /* et non VueNb, car Vue est le numero d'element associe */
                      pDoc->DocAssocFreeVolume[Vue - 1] = volume;
                    else
	              pDoc->DocViewFreeVolume[VueNb - 1] = volume;
                    volprec = PavRacine->AbVolume;
                    /* volume de la vue avant */
                    /* demande la creation de paves supplementaires */
		    ArretColGroupee = False;
             list = fopen("/perles/roisin/debug/avajoutpage","w");
             if (list != NULL)
               {
                 NumPav(PavRacine);
                 AffPaves(PavRacine, 2, list);
                 fclose(list);
              }		
                    AjoutePaves(PavRacine, pDoc, False);
		    if (A_Equilibrer && ArretColGroupee)
		      {
		        /* mise a jour de ArretColGroupee et du vollibre */
		        /* pour permettre la creation des paves dans la */
		        /* procedure d'equilibrage */
		        ArretColGroupee = False;
                        if (Assoc)
                 /* ATTENTION si Assoc est vrai, il faut utiliser Vue */
                 /* et non VueNb, car Vue est le numero d'element associe */
                          pDoc->DocAssocFreeVolume[Vue - 1] = volume;
                        else
	                  pDoc->DocViewFreeVolume[VueNb - 1] = volume;
		        Equilibrer_Col (pDoc, PavRacine, VueNb, VueSch);
		 /*       A_Equilibrer = False; */ /* TODO A revoir */
		               /* pour ne pas recommencer */
		               /* si l'equilibrage a deja ete fait */
		      }
                    if (PavRacine->AbVolume <= volprec)
                      /* rien n'a ete cree, augmente le
                         volume de ce qui peut etre cree' */
              /* et on deverrouille la creation avec */
              /* ArretAvantCreation */
              /* (cas ou le premier element a ajouter est une */
              /* MP ou un elt assoc HB) */
              {
                       volume = 2 * volume;
                ArretAvantCreation = False;
                TrouveMPHB = False;
               /* CreePaves doit s'arreter apres creation d'1 MP */
               /* ou d'un elt asscoc HB */
              }
                  }
                while (!(PavRacine->AbVolume > volprec ||
                         !PavRacine->AbTruncatedTail));
         volume = PavRacine->AbVolume; /* pour l prochain ajout de paves*/
                /* appelle ModifVue pour savoir si au moins une boite est */
                /* traversee par une frontiere de page apres l'ajout des
                   paves supplementaires */
                Hauteurffective = HauteurCoupPage;
         /* on change la regle des paves corps de page (sauf si MP mise */
         /* par l'utilisateur) : hauteur = celle du contenu */
         /* et on decale la position du bas et du filet de page */
         ChangeRHPage (PavRacine, pDoc, VueNb);
                tropcourt =
           ModifVue(frame, &Hauteurffective, PavRacine);
         /* si tropcourt, et si l'arret de creation est du^ */
         /* a un element MP ou ref assoc HB (TrouveMPHB = vrai) */
         /* on bascule ArretAvantCreation pour permettre */
         /* de continuer le calcul de l'i.a. (la creation */
         /* s'etait arretee avant ou apres une MP ou une ref */
         /* d'un elt assoc a placer en haut ou bas de page */
         if (tropcourt && TrouveMPHB)
           {
             ArretAvantCreation = !ArretAvantCreation;
             TrouveMPHB = False; /* pour le tour suivant*/
           }
         if (!tropcourt && TrouveMPHB && RefAssocHBPage != NULL)
           /* si on a cree une reference a un element associe */
           /* qui a provoque la creation des paves de cet element */
           /* associe en faisant deborder la page, il faut */
           /* forcer la coupure avant la reference et supprimer */
           /* si besoin (si c'est en haut de page et */
           /* si cet elt assoc n'est pas deja reference */
           /* dans la page) les paves de l'elt assoc et/ou de */
           /* l'englobant si pas d'autre elt assoc dans la page */
           {
            if (PavAssocADetruire != NULL)
              {
                /* on detruit le pave et ses eventuels paves de pres */
                pP = PavAssocADetruire;
                while (pP->AbPrevious != NULL 
                    && pP->AbPrevious->AbElement == pP->AbElement) 
                    {
                      pP = pP->AbPrevious;
                      TuePave(pP);
                      SuppRfPave (pP, &PavR, pDoc);
                    }
                pP = PavAssocADetruire;
                while (pP->AbNext != NULL 
                    && pP->AbNext->AbElement == pP->AbElement)
                    {
                      pP = pP->AbNext;
                      TuePave(pP);
                      SuppRfPave (pP, &PavR, pDoc);
                    }
                TuePave(PavAssocADetruire);
                SuppRfPave (PavAssocADetruire, &PavR, pDoc);
                /* on signale les paves detruits au mediateur */
                h = -1; /* changement de signification de h */
                tropcourt = ModifVue(frame, &h, PavRacine);
                /* on libere les paves */
		       pP = PavAssocADetruire->AbEnclosing;
                LibPavMort (PavAssocADetruire->AbEnclosing);
                /* on recherche le pave englobant haut ou bas de page */
		       while (pP->AbElement != pCorps->AbElement)
			 pP = pP->AbEnclosing;
	               /* calcul de la hauteur du pave haut ou bas de page */
	               HautCoupure(pP, True, &Hauteur, &PosV, &CarCoupe);
	               /* Hauteur = dim verticale du haut (ou bas) de page */
                if (pP->AbPrevious == pCorps) 
			/* des paves ont ete ajoutes en bas de page */
	                 HauteurBasPage = Hauteur;
	               else /* des paves ont ete ajoutes en haut de page */
	                 HauteurHautPage = Hauteur;
	               /* HauteurCoupPage = hauteur totale - hauteur bas */
	               HauteurCoupPage = HauteurTotalePage - HauteurBasPage;

                /* on modifie la regle de presentation specifique */
                /* du corps de page */
                pRegleDimV = ChReglePres(pCorps->AbElement,
                                PtHeight, &nouveau, pDoc, VueNb);
                /* change le parametre de la regle */
                pRegleDimV->PrDimRule.DrValue =
                    HauteurTotalePage - HauteurBasPage -HauteurHautPage;
             }
           /* on fait evaluer la position du pave reference */
            HautCoupure(RefAssocHBPage->ElAbstractBox[VueNb-1], True,
                       &Hauteur, &PosV, &CarCoupe);
           /* on force la coupure a cette hauteur */
           h = PosV;
           tropcourt = ModifVue(frame, &h, PavRacine);
           /* normalement tropcourt est tj faux */         
         } /* fin cas page trop grande a cause d'elt ref HB */
         /* si on a cree les paves de la page suivante (page */
         /* utilisateur ou nouvelle regle page), on met a jour */
         /* pPageATraiter */
         pPageATraiter = pPageTraitee->AbNext;
       } /* fin boucle d'ajout de paves */
     if (!tropcourt && pCorps->AbOnPageBreak)
       {
         /* il faut inserer une marque de page */
         /* l'image fait plus d'une hauteur de page */
         /* donc si une page a ete ajoutee (page utilisateur) */
         /* elle est trop loin, il faut donc remettre pPageATraiter */
         /* a NULL */
         pPageATraiter = NULL;
         volprec = PavRacine->AbVolume;
	        /* Insere un element marque page a la frontiere de page et */
	        /* detruit tous les paves qui precedent cette frontiere. */
         /* on repositionne ArretAvantCreation a faux pour */
         /* permettre la creation des paves du nouvel elt MP */
         ArretAvantCreation = False;
          list = fopen("/perles/roisin/debug/posemarque","w");
          if (list != NULL)
            {
              NumPav(PavRacine);
              AffPaves(PavRacine, 2, list);
              fclose(list);
            }		
         PoseMarque(pElRacine, VueNb, pDoc, frame,
	                   &pPageTraitee, &pPageATraiter);
	        /* calcule le volume qui a ete detruit pour en regenerer */
	        /* autant ensuite */
		tropcourt = True; /* pour forcer la creation de nouveaux paves */
		/* et l'appel a modifvue */
	        volume = volume + volprec - PavRacine->AbVolume;
       }
     else if (!tropcourt && !pCorps->AbOnPageBreak) 
         /* cas ou la coupure est sur le bas de page ou le filet */
         /* il faut passer a la page suivante sans inserer de */
         /* marque page supplementaire : on reboucle pour */
         /* ajouter eventuellement des paves si il n'y a pas */
         /* encore de paves de la page suivante */
         /* pour cela, on force tropcourt a vrai */
            if (pPageATraiter == NULL)
              tropcourt = True;
           /* TODO faut-il mettre a jour AbOnPageBreak et AbAfterPageBreak */
   } /* fin boucle pPageATraiter == NULL et !fin doc */
 } /* fin boucle creation des pages une a une */
 
  /* fin de la vue */
 /* ajoute une marque de page a la fin s'il n'y en a pas deja une */
 /* ce n'est plus necessaire : code supprime */

  PaginationEnCours = False;
  /* detruit l'image abstraite de la fin du document */
  DetrImAbs_Pages(Vue, Assoc, pDoc, VueSch);
  /* reconstruit l'image de la vue et l'affiche */
  Aff_Select_Pages (pDoc, PremPage, Vue, Assoc, sel, SelPrem, 
		    SelDer, SelPremCar, SelDerCar);
  /* paginer un document le modifie ... */
  pDoc->DocModified = True;
 } /* fin du cas ou le document est mis en pages */

#else /* __COLPAGE__ */

  HauteurPage = 0;
  HauteurBasPage = 0;	/* cree les paves du debut de la vue */
  if (Assoc)
    pDoc->DocAssocFreeVolume[Vue - 1] = pDoc->DocAssocVolume[Vue - 1];
  else
    pDoc->DocViewFreeVolume[Vue - 1] = pDoc->DocViewVolume[Vue - 1];
  pP = CreePaves(pElRacine, pDoc, VueNb, True, True, &complet);
  volume = 0;
 /* mise a jour de PavRacine apres la creation des paves */
 /* dans le cas de l'appel depuis print, il n'y avait aucun pave cree */
  if (Assoc) 
    PavRacine = pElRacine->ElAbstractBox[0];
  else 
    {
      PavRacine = pElRacine->ElAbstractBox[Vue - 1];
      if (pDoc->DocViewRootAb[Vue - 1] == NULL)
	        pDoc->DocViewRootAb[Vue - 1] = PavRacine;
    }
  pP = PavRacine;		
  /* cherche la 1ere marque de page dans le debut de l'image du document, */
  /* pour connaitre la hauteur des pages */
  /* cherche d'abord le 1er pave feuille ou la premiere marque de page */
  while (pP->AbFirstEnclosed != NULL &&
	 pP->AbElement->ElTypeNumber != PageBreak + 1)
    pP = pP->AbFirstEnclosed;
  if (pP->AbElement->ElTypeNumber != PageBreak + 1)
    /* le document ne commence pas par une marque de page pour cette */
    /* vue ; on cherche la premiere marque de page qui suit */
    pP = PavCherche(pP, False, PageBreak + 1, NULL, NULL);
  if (pP != NULL)
    if (pP->AbElement->ElTypeNumber == PageBreak + 1)
      /* on a trouve une marque de page, on determine */
      /* la hauteur de ce type de page */
      HautPage(pP->AbElement, VueSch, &b, &pSchPage);
  /* fait calculer l'image par le Mediateur */
  Hauteurffective = HauteurPage;
  tropcourt = ModifVue(frame, &Hauteurffective, PavRacine);
  do	
    /* traite une page apres l'autre */
    {
      pP = PavRacine;
      /* cherche le premier pave feuille ou la premiere marque de page */
      while (pP->AbFirstEnclosed != NULL 
	     && pP->AbElement->ElTypeNumber != PageBreak + 1)
	pP = pP->AbFirstEnclosed;
      do
	/* cherche les marques de page correspondant au debut d'un element */
	/* portant une regle Page ou les marques mises par l'utilisateur */
	{
	  if (pP->AbElement->ElTypeNumber == PageBreak + 1)
	    /* c'est une marque de page */
	    /* affiche un message avec le numero de page */
	    {
       pavPagePrec = pP; /* pave de la page a imprimer */
#ifndef PAGINEETIMPRIME
	      AffMsgPage (pDoc, pElRacine, pP->AbElement, VueSch, Assoc, &prempage);
#endif PAGINEETIMPRIME

	      /* On prend la hauteur de ce type de page */
	      HautPage(pP->AbElement, VueSch, &b, &pSchPage);
	      if (PremPage == NULL)
		PremPage = pP->AbElement;
	      /* cherche la derniere feuille dans la marque de page */
	      while (pP->AbFirstEnclosed != NULL)
		{
		  pP = pP->AbFirstEnclosed;
		  while (pP->AbNext != NULL)
		    pP = pP->AbNext;
		}
	    }
	  /* cherche la marque de la page suivante */
	  pP = PavCherche(pP, False, PageBreak+1, NULL, NULL);
	  if (pP != NULL)
	    /* on a trouve' une marque de page. C'est une page */
	    /* de debut d'element ou une page creee par l'utilisateur. */
	    if (pP->AbAfterPageBreak)
	      /* cette marque de page est trop loin, il faut inserer */
	      /* une marque de page avant */
	      pP = NULL;	/* pour sortir de la boucle */
	    else 
	      {
	        if (pavPagePrec == NULL)
	  	  /* cas de la premiere marque page du doc dans cette vue */
	          pavPagePrec = pP ;
  	        /* on renumerote cette marque de page */
		pEl1 = pP->AbElement;
		/* cherche le compteur de page a appliquer a cette page */
		cpt = CptPage(pP->AbElement, VueSch, &pSchP);
		if (cpt == 0)
		  /* page non numerotee */
		  {
		    CompteurPages++;
		    pEl1->ElPageNumber = CompteurPages;
		  } 
		else
		  {
		    /* calcule le numero de page */
		    pEl1->ElPageNumber = 
		      ComptVal(cpt, pEl1->ElSructSchema, pSchP, pP->AbElement, VueSch);
		    /* on met a jour les boites de presentation des compteurs des */
		    /* pages suivantes dans le cas de la pagination depuis l'impression */
		    /* cet appel est fait tout a la fin dans le cas d'une pagination */
		    /* normale */

#ifdef PAGINEETIMPRIME
		    MajNumeros(NextElement(pEl1), pEl1, pDoc, True);
		    /* serait-ce plus rapide si on faisait durectement l'appel : */
		    /* ChngBoiteCompteur(pEl1, pDoc, cpt, pSchP, pEl1->ElSructSchema, True); */
#endif PAGINEETIMPRIME
		  }
#ifndef PAGINEETIMPRIME
		/* affiche un message avec le numero de page */
		AffMsgPage (pDoc, pElRacine, pEl1, VueSch, Assoc, &prempage);
#endif PAGINEETIMPRIME
		/* On prend la hauteur de ce type de page */
		HautPage(pP->AbElement, VueSch, &b, &pSchPage);
		/* la marque de page est avant la limite de page calculee, */
		/* on detruit tous les paves qui precedent la marque de page */
		volprec = PavRacine->AbVolume;
           /* avant de detruire la page precedente, on l'imprime */
           /* si la demande a ete faite */
	   	  /* pavPagePrec contient le pave de la page precedente */
	   	  /* (sauf cas de la premiere page) */
#ifdef PAGINEETIMPRIME
           if (pavPagePrec != pP)
             PrintOnePage(pDoc, pavPagePrec, pP, Vue, Assoc);
#endif PAGINEETIMPRIME
         /* on met a jour pavPagePrec pour le tour suivant (au cas ou */
         /* on soit a la fin du document) */
         pavPagePrec = pP;
		tropcourt = TueAvantPage(pP, frame, pDoc, VueNb);
		/* calcule le volume de ce qui a ete detruit */
		/* pour en regenerer autant ensuite */
		volume += volprec - PavRacine->AbVolume;
	      }
	}
      while (pP != NULL);
      if (!tropcourt)
	/* l'image fait plus d'une hauteur de page */
	{
	  volprec = PavRacine->AbVolume;
	  /* Insere un element marque de page a la frontiere de page et */
	  /* detruit tous les paves qui precedent cette frontiere. */
	  pPage = PoseMarque(pElRacine, VueNb, pDoc, frame);
          /* une nouvelle page vient d'etre calculee, on l'imprime */
   if ((pPage != NULL) && (pPage->ElAbstractBox[VueNb-1] != NULL))
     {
       /* avant de detruire la page precedente, on l'imprime */
       /* si la demande a ete faite */
       /* pavPagePrec contient le pave de la page precedente */
#ifdef PAGINEETIMPRIME
       PrintOnePage(pDoc, pavPagePrec, pPage->ElAbstractBox[VueNb-1], 
                       Vue, Assoc);
#endif PAGINEETIMPRIME
              /* detruit tous les paves qui precedent la nouvelle frontiere */
       tropcourt = TueAvantPage(pPage->ElAbstractBox[VueNb-1], 
                                      frame, pDoc, VueNb);
       /* pavPagePrec devient la nouvelle page cree */
       pavPagePrec = pPage->ElAbstractBox[VueNb-1];
	      /* calcule le volume de ce qui a ete detruit pour en regenerer */
	      /* autant ensuite */
	      volume = volume + volprec - PavRacine->AbVolume;
     }
	}
      /* complete l'image abstraite de cette vue jusqu'a ce qu'elle */
      /* contienne une boite traversee par une frontiere de page ou qu'on */
      /* soit arrive' a la fin de la vue. */
      rienAjoute = True;  /* on n'a encore rien ajoute' a l'image */
      while (tropcourt && PavRacine->AbTruncatedTail)
	/* on ajoute au moins 100 caracteres a l'image */
	{
	  if (volume < 100)
	    /* indique le volume qui peut etre cree */
	    volume = 100;
	  do
	    {
	      if (Assoc)
		pDoc->DocAssocFreeVolume[Vue-1] = volume;
	      else
		pDoc->DocViewFreeVolume[Vue-1] = volume;
	      volprec = PavRacine->AbVolume;
	      /* volume de la vue avant */
	      /* demande la creation de paves supplementaires */
	      AjoutePaves(PavRacine, pDoc, False);
	      if (PavRacine->AbVolume <= volprec)
		/* rien n'a ete cree, augmente le
		   volume de ce qui peut etre cree' */
		volume = 2 * volume;
	      else	/* on a ajoute' de nouveaux paves */
		rienAjoute = False;
	    }
	  while (!(PavRacine->AbVolume > volprec ||
		   !PavRacine->AbTruncatedTail));
	  volume = 0;	/* plus rien a generer */
	  /* appelle ModifVue pour savoir si au moins une boite est */
	  /* traversee par une frontiere de page apres l'ajout des
	     paves supplementaires */
	  Hauteurffective = HauteurPage;
	  if (Assoc)
	    {
	      if (pDoc->DocAssocModifiedAb[Vue-1] != NULL)
	        {
	  	  tropcourt = 
		  ModifVue(frame, &Hauteurffective, pDoc->DocAssocModifiedAb[Vue-1]);
		  pDoc->DocAssocModifiedAb[Vue-1] = NULL;
	        } 
	    }
	  else if (pDoc->DocViewModifiedAb[Vue-1] != NULL)
	    {
	      tropcourt = ModifVue(frame, &Hauteurffective, pDoc->DocViewModifiedAb[Vue-1]);
	      pDoc->DocViewModifiedAb[Vue-1] = NULL;
	      /* si de nouveaux paves ont ete crees, on refait un tour pour */
	      /* traiter les marques de pages qu'ils contiennent */
	    }
	}
    }
  while (!(tropcourt && rienAjoute && !PavRacine->AbTruncatedTail));
  /* quand on sort de la boucle, pavPagePrec point sur le pave de la */
  /* derniere page (avant que l'editeur n'ait insere la marque de fin) */

  /* fin de la vue */
  PavRacine->AbTruncatedTail = False;

  /* Ajoute le saut de page qui manque eventuellement a la fin */
  AjoutePageEnFin(pElRacine, VueSch, pDoc, True);

#ifdef PAGINEETIMPRIME
 /* il faut imprimer la derniere page */
 /* on cree d'abord son pave */
	if (Assoc)
	  pDoc->DocAssocFreeVolume[Vue-1] = 100;
	else
	  pDoc->DocViewFreeVolume[Vue-1] = 100;
	PavRacine->AbTruncatedTail = True; /* il reste des paves a creer : */
	/* ce sont ceux de la nouvelle marque de page */
	AjoutePaves(PavRacine, pDoc, False);

	/* cherche la marque de page qui vient d'etre inseree */

	/* cherche la derniere feuille dans la marque de page precedente */
	pP = pavPagePrec;
	if (pP != NULL)
	  {
	  while (pP->AbFirstEnclosed != NULL)
	    {
	    pP = pP->AbFirstEnclosed;
	    while (pP->AbNext != NULL)
	     pP = pP->AbNext;
	    }
	  pP = PavCherche(pP, False, PageBreak+1, NULL, NULL);
	  }
 if (pP != NULL)
 /* on fait calculer l'image par le mediateur avant d'appeler */
 /* l'impression */
   {
     h = 0; /* on ne fait pas evaluer la hauteur de coupure */ 
     if (Assoc)
	      {
	      if (pDoc->DocAssocModifiedAb[Vue-1] != NULL)
	        {
	  	  tropcourt = 
		  ModifVue(frame, &h, pDoc->DocAssocModifiedAb[Vue-1]);
		  pDoc->DocAssocModifiedAb[Vue-1] = NULL;
	        } 
	      }
	    else if (pDoc->DocViewModifiedAb[Vue-1] != NULL)
	      {
	        tropcourt = ModifVue(frame, &h, pDoc->DocViewModifiedAb[Vue-1]);
	        pDoc->DocViewModifiedAb[Vue-1] = NULL;
	      }
     PrintOnePage(pDoc, pavPagePrec, pP, Vue, Assoc);
 }
#endif PAGINEETIMPRIME

  PaginationEnCours = False;
  /* detruit l'image abstraite de la fin du document */
  DetrImAbs_Pages(Vue, Assoc, pDoc, VueSch);
  /* reconstruit l'image de la vue et l'affiche */
#ifndef PAGINEETIMPRIME
  Aff_Select_Pages (pDoc, PremPage, Vue, Assoc, sel, SelPrem, 
		    SelDer, SelPremCar, SelDerCar);
#endif /* PAGINEETIMPRIME */
  /* paginer un document le modifie ... */
  pDoc->DocModified = True;
#endif /* __COLPAGE__ */
}
/* End Of Module page */
