
/*
   Ce module effectue la creation des images abstraites
   V. Quint I. Vatton  C. Roisin
*/

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "message.h"
#include "language.h"
#include "appdialogue.h"

#undef EXPORT
#define EXPORT extern
#include "page.var"
#include "environ.var"
#include "edit.var"
#include "appdialogue.var"

#include "arbabs.f"
#include "attrherit.f"
#include "attribut.f"
#include "crimabs.f"
#include "crpages.f"
#include "except.f"
#include "imabs.f"
#include "imagepres.f"
#include "memory.f"
#include "modif.f"
#include "modimabs.f"
#include "pres.f"
#include "refelem.f"
#include "schemas.f"
#include "sel.f"
#include "textelem.f"
#include "varpres.f"


/* ---------------------------------------------------------------------- */
/* |    GetRule rend la regle de presentation a` appliquer pour la vue  | */
/* |            numero 1. pRSpecif est la regle specifique courante,    | */
/* |            pRDefaut est la regle par defaut courante. Au retour    | */
/* |            ces deux pointeurs sont mis a` jour pour la regle       | */
/* |            suivante.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrPRule        GetRule (PtrPRule * pRSpecif, PtrPRule * pRDefaut,
			PtrElement pEl, PtrAttribute pAttr, PtrSSchema pSS)

#else  /* __STDC__ */
PtrPRule        GetRule (pRSpecif, pRDefaut, pEl, pAttr, pSS)
PtrPRule       *pRSpecif;
PtrPRule       *pRDefaut;
PtrElement          pEl;
PtrAttribute         pAttr;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   PtrPRule        pPR;
   boolean             stop;

   pPR = NULL;
   if (*pRSpecif != NULL && *pRDefaut != NULL)
     {
	/* Il n'y a pas de fonction par defaut. Donc si on est sur une regle */
	/* specifique de type fonction, on la prend, si ses conditions */
	/* d'application sont satisfaites. */
	if ((*pRSpecif)->PrType == PtFunction &&
	    (*pRDefaut)->PrType > PtFunction)
	  {
	     stop = False;
	     do
	       {
		  if ((*pRSpecif)->PrCond == NULL ||
		  CondPresentation ((*pRSpecif)->PrCond, pEl, pAttr, 1, pSS))
		     /* condition d'application satisfaites */
		    {
		       pPR = *pRSpecif;
		       stop = True;
		    }
		  /* prochaine regle specifique a traiter */
		  *pRSpecif = (*pRSpecif)->PrNextPRule;
		  if (!stop)
		     /* la condition n'est pas satisfaite, on essaie les Fonctions */
		     /* suivantes */
		     if (*pRSpecif == NULL)
			/* pas de regle specifique suivante, on arrete */
			stop = True;
		     else if ((*pRSpecif)->PrType != PtFunction)
			/* la regle specifique suivante n'est pas une Fonction, on */
			/* arrete */
			stop = True;
	       }
	     while (!stop);
	  }
     }
   if (pPR == NULL)
      if (*pRDefaut != NULL)
	{
	   if (*pRSpecif != NULL)
	      if ((*pRSpecif)->PrType == (*pRDefaut)->PrType &&
		  (*pRSpecif)->PrViewNum == 1)
		 /* pour la vue 1, la regle specifique de meme type que la */
		 /* regle par defaut existe */
		{
		   stop = False;
		   /* on traite toutes les regles specifiques successives de ce type */
		   /* et qui concernent la vue 1, et on retient la premiere dont */
		   /* les conditions d'application sont satisfaites */
		   do
		     {
			if (pPR == NULL)
			   /* on n'a pas encore trouve' la bonne regle */
			   if ((*pRSpecif)->PrCond == NULL ||
			       CondPresentation ((*pRSpecif)->PrCond, pEl, pAttr, 1, pSS))
			      /* les conditions d'application sont satisfaites, on prend */
			      /* cette regle */
			      pPR = *pRSpecif;
			if ((*pRSpecif)->PrNextPRule == NULL)
			   stop = True;
			else if ((*pRSpecif)->PrNextPRule->PrType == (*pRDefaut)->PrType
				 && (*pRSpecif)->PrNextPRule->PrViewNum == 1)
			   *pRSpecif = (*pRSpecif)->PrNextPRule;
			else
			   stop = True;
		     }
		   while (!stop);
		   /* avance d'une regle */
		   if (*pRSpecif != NULL)
		      *pRSpecif = (*pRSpecif)->PrNextPRule;
		   *pRDefaut = (*pRDefaut)->PrNextPRule;
		}
	   if (pPR == NULL)
	      /* on prend la regle par defaut */
	      /* les regles par defaut n'ont pas de condition */
	     {
		pPR = *pRDefaut;
		*pRDefaut = (*pRDefaut)->PrNextPRule;
	     }
	}
   return pPR;
}


/* ---------------------------------------------------------------------- */
/* |    GetRuleView donne la regle du type Typ a` appliquer pour la vue | */
/* |            Vue. pRSpecif et pRDefaut sont mis a` jour. La fonction | */
/* |            retourne NULL si il n'y a pas de regle de ce type pour  | */
/* |            cette vue (dans ce cas, il faut alors appliquer la      | */
/* |            regle de la vue 1).                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrPRule        GetRuleView (PtrPRule * pRSpecif, PtrPRule * pRDefaut,
				 PRuleType Typ, int Vue, PtrElement pEl,
				 PtrAttribute pAttr, PtrSSchema pSS)

#else  /* __STDC__ */
PtrPRule        GetRuleView (pRSpecif, pRDefaut, Typ, Vue, pEl, pAttr, pSS)
PtrPRule       *pRSpecif;
PtrPRule       *pRDefaut;
PRuleType           Typ;
int              Vue;
PtrElement          pEl;
PtrAttribute         pAttr;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   PtrPRule        pPR;
   boolean             stop;

   pPR = NULL;
   if (*pRSpecif != NULL)
      if ((*pRSpecif)->PrViewNum == Vue)
	 if ((*pRSpecif)->PrType == Typ)
	   {
	      stop = False;
	      /* on traite toutes les regles successives de meme type et qui */
	      /* concernent la meme vue */
	      do
		{
		   if (pPR == NULL)
		      /* on n'a pas encore trouve' la regle qui s'applique */
		      if ((*pRSpecif)->PrCond == NULL ||
			  CondPresentation ((*pRSpecif)->PrCond, pEl, pAttr, Vue, pSS))
			 /* les conditions d'application de la regle sont satisfaites, */
			 /* on la prend */
			 pPR = *pRSpecif;
		   if ((*pRSpecif)->PrNextPRule == NULL)
		      stop = True;
		   else if ((*pRSpecif)->PrNextPRule->PrViewNum == Vue &&
			    (*pRSpecif)->PrNextPRule->PrType == Typ)
		      *pRSpecif = (*pRSpecif)->PrNextPRule;
		   else
		      stop = True;
		}
	      while (!stop);
	      /* avance d'une regle */
	      if (*pRSpecif != NULL)
		 *pRSpecif = (*pRSpecif)->PrNextPRule;
	      if (*pRDefaut != NULL)
		 if ((*pRDefaut)->PrType == Typ
		     && (*pRDefaut)->PrViewNum == Vue)
		    *pRDefaut = (*pRDefaut)->PrNextPRule;
	   }
   if (pPR == NULL)
      if (*pRDefaut != NULL)
	 if ((*pRDefaut)->PrType == Typ)
	    if ((*pRDefaut)->PrViewNum == Vue)
	       /* on prend la regle par defaut */
	      {
		 pPR = *pRDefaut;
		 *pRDefaut = (*pRDefaut)->PrNextPRule;
	      }
   return pPR;
}


/* ---------------------------------------------------------------------- */
/* |    initpave cree et initialise un pave pour l'element pEl et dans  | */
/* |            la vue nv, avec la visibilite Visib.                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrAbstractBox             initpave (PtrElement pEl, DocViewNumber nv, int Visib)

#else  /* __STDC__ */
PtrAbstractBox             initpave (pEl, nv, Visib)
PtrElement          pEl;
DocViewNumber           nv;
int                 Visib;

#endif /* __STDC__ */

{
   PtrAbstractBox             pAb;
   PtrElement          pAscend;
   boolean             stop;

   GetPave (&pAb);
   pAb->AbElement = pEl;
   pAb->AbEnclosing = NULL;
   pAb->AbNext = NULL;
   pAb->AbPrevious = NULL;
   pAb->AbFirstEnclosed = NULL;
   pAb->AbPSchema = NULL;
   pAb->AbCreatorAttr = NULL;
   pAb->AbDocView = nv;
   pAb->AbInLine = True;
   pAb->AbVisibility = Visib;

   pAb->AbHorizRef.PosEdge = HorizRef;
   pAb->AbHorizRef.PosRefEdge = HorizRef;
   pAb->AbHorizRef.PosDistance = 0;
   pAb->AbHorizRef.PosUnit = UnRelative;
   pAb->AbHorizRef.PosAbRef = NULL;
   pAb->AbHorizRef.PosUserSpecified = False;

   pAb->AbVertRef.PosEdge = VertRef;
   pAb->AbVertRef.PosRefEdge = VertRef;
   pAb->AbVertRef.PosDistance = 0;
   pAb->AbVertRef.PosUnit = UnRelative;
   pAb->AbVertRef.PosAbRef = NULL;
   pAb->AbVertRef.PosUserSpecified = False;

   pAb->AbVertPos.PosEdge = Top;
   pAb->AbVertPos.PosRefEdge = Top;
   pAb->AbVertPos.PosDistance = 0;
   pAb->AbVertPos.PosUnit = UnRelative;
   pAb->AbVertPos.PosAbRef = NULL;
   pAb->AbVertPos.PosUserSpecified = False;

   pAb->AbHorizPos.PosEdge = Left;
   pAb->AbHorizPos.PosRefEdge = Left;
   pAb->AbHorizPos.PosDistance = 0;
   pAb->AbHorizPos.PosUnit = UnRelative;
   pAb->AbHorizPos.PosAbRef = NULL;
   pAb->AbHorizPos.PosUserSpecified = False;

   pAb->AbBox = NULL;
   pAb->AbSelected = False;

   pAb->AbHeight.DimIsPosition = False;
   pAb->AbHeight.DimValue = 0;
   pAb->AbHeight.DimAbRef = NULL;
   pAb->AbHeight.DimUnit = UnRelative;
   pAb->AbHeight.DimSameDimension = True;
   pAb->AbHeight.DimUserSpecified = False;
   pAb->AbHeight.DimMinimum = False;

   pAb->AbWidth.DimIsPosition = False;
   pAb->AbWidth.DimValue = 0;
   pAb->AbWidth.DimAbRef = NULL;
   pAb->AbWidth.DimUnit = UnRelative;
   pAb->AbWidth.DimSameDimension = True;
   pAb->AbWidth.DimUserSpecified = False;
   pAb->AbWidth.DimMinimum = False;

   pAb->AbVolume = 0;
   pAb->AbBox = NULL;
   pAb->AbIndent = 0;
   pAb->AbDepth = 0;
   pAb->AbNum = 0;
   pAb->AbVisibility = Visib;
   pAb->AbFont = 'T';
   pAb->AbHighlight = 0;
   pAb->AbUnderline = 0;
   pAb->AbThickness = 0;
   pAb->AbSize = 1;
   pAb->AbSizeUnit = UnRelative;
   pAb->AbFillPattern = 0;
   pAb->AbBackground = 0;	/* blanc */
   pAb->AbForeground = 1;	/* noir */
   pAb->AbLineStyle = 'S';
   pAb->AbLineWeight = 1;
   pAb->AbLineWeightUnit = UnPoint;
   pAb->AbHorizEnclosing = True;	/* ce pave respecte les regles d'englobement */
   pAb->AbVertEnclosing = True;
   /* les copies de parametres ou d'elements inclus ne sont pas */
   /* modifiables par l'utilisateur */
   pAb->AbCanBeModified = !pEl->ElIsCopy;
   /* les constantes ne sont pas modifiables par l'utilisateur */
   if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsConstant)
      pAb->AbCanBeModified = False;
   pAb->AbSelected = False;
   pAb->AbOnPageBreak = False;
   pAb->AbAfterPageBreak = False;
   pAb->AbLineSpacing = 10;
   pAb->AbAdjust = AlignLeft;
   pAb->AbJustify = False;
   pAb->AbLineSpacingUnit = UnRelative;
   pAb->AbIndentUnit = UnRelative;
   pAb->AbAcceptLineBreak = True;
   pAb->AbAcceptPageBreak = True;
   pAb->AbNotInLine = False;
   pAb->AbSensitive = False;
   /* cherche si l'element est une reference ou une copie d'element inclus */
   if (pEl->ElTerminal)
      if (pEl->ElLeafType == LtReference)
	 /* une reference = un pave actif (double-cliquable) */
	 pAb->AbSensitive = True;
   if (pEl->ElIsCopy)
      /* un element appartenant a un element copie' = pave actif, sauf */
      /* s'il s'agit d'un parametre. */
     {
	pAb->AbSensitive = True;
	pAscend = pEl;
	while (pAscend != NULL)	/* un ascendant est-il un parametre ? */
	   if (pAscend->ElSructSchema->SsRule[pAscend->ElTypeNumber - 1].SrParamElem)
	     {
		pAscend = NULL;
		pAb->AbSensitive = False;		/* un parametre n'est pas actif */
	     }
	   else
	      pAscend = pAscend->ElParent;
     }
   pAb->AbReadOnly = False;
   if (ElementIsReadOnly (pEl))
     {
	pAb->AbCanBeModified = False;
	pAb->AbReadOnly = True;
     }
   else
     {
	/* cherche si un ascendant est un parametre */
	pAscend = pEl;
	stop = False;
	do
	   if (pAscend == NULL)
	      stop = True;
	   else if (pAscend->ElSructSchema->SsRule[pAscend->ElTypeNumber - 1].SrParamElem)
	     {
		/* le contenu d'un parametre ne peut pas etre modifie */
		stop = True;
		pAb->AbReadOnly = True;
		pAb->AbCanBeModified = False;
	     }
	   else
	      pAscend = pAscend->ElParent;
	while (!(stop));
     }
   pAb->AbHyphenate = False;
   pAb->AbPresentationBox = False;
#ifdef __COLPAGE__
   pAb->AbRepeatedPresBox = False;
#endif /* __COLPAGE__ */
   pAb->AbTypeNum = pEl->ElTypeNumber;
   /* ce pave respecte les regles d'englobement */
   pAb->AbHorizEnclosing = True;
   pAb->AbVertEnclosing = True;
   pAb->AbDelayedPRule = NULL;
   pAb->AbText = NULL;
   pAb->AbNew = True;
   pAb->AbDead = False;
   pAb->AbWidthChange = False;
   pAb->AbHeightChange = False;
   pAb->AbHorizPosChange = False;
   pAb->AbVertPosChange = False;
   pAb->AbHorizRefChange = False;
   pAb->AbVertRefChange = False;
   pAb->AbSizeChange = False;
   pAb->AbAspectChange = False;
   pAb->AbChange = False;

   if (pEl->ElTerminal && pEl->ElLeafType == LtText)
     {
	pAb->AbLeafType = LtText;
	pAb->AbLanguage = TtaGetDefaultLanguage ();
     }
   return pAb;
}


/* ---------------------------------------------------------------------- */
/* |    CopieConstante met dans le pave pAb le texte correspondant a`  | */
/* |            la constante de numero NConst dans le schema de         | */
/* |            presentation pSchP.                                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                CopieConstante (int NConst, PtrPSchema pSchP, PtrAbstractBox pAb)

#else  /* __STDC__ */
void                CopieConstante (NConst, pSchP, pAb)
int                 NConst;
PtrPSchema          pSchP;
PtrAbstractBox             pAb;

#endif /* __STDC__ */

{
   PresConstant          *pConst;

   pConst = &pSchP->PsConstant[NConst - 1];
   switch (pConst->PdType)
	 {
	    case GraphicElem:
	       pAb->AbLeafType = LtGraphics;
	       pAb->AbShape = pConst->PdString[0];
	       pAb->AbGraphAlphabet = 'G';
	       pAb->AbVolume = 1;
	       break;
	    case Symbol:
	       pAb->AbLeafType = LtSymbol;
	       pAb->AbShape = pConst->PdString[0];
	       pAb->AbGraphAlphabet = 'G';
	       if (pAb->AbShape == '\0')
		  pAb->AbVolume = 0;
	       else
		  pAb->AbVolume = 1;
	       break;
	    case CharString:
	       if (pAb->AbText == NULL)
		  GetBufConst (pAb);
	       strncpy(pAb->AbText->BuContent, pConst->PdString, MAX_CHAR-1);
	       pAb->AbText->BuContent[MAX_CHAR-1] = '\0';
	       pAb->AbText->BuLength = strlen(pAb->AbText->BuContent);
	       pAb->AbLanguage = TtaGetLanguageIdFromAlphabet (pConst->PdAlphabet);
	       pAb->AbVolume = pAb->AbText->BuLength;
	       pAb->AbLeafType = LtText;
	       break;
	    case Picture:
	       NewImageDescriptor (pAb, pConst->PdString, UNKNOWN_FORMAT);
	       pAb->AbLeafType = LtPicture;
	       pAb->AbVolume = 100;
	       break;
	    default:
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |    VueAssoc retourne vrai si l'element pEl s'affiche dans une vue  | */
/* |            d'elements associes.                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             VueAssoc (PtrElement pEl)

#else  /* __STDC__ */
boolean             VueAssoc (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   int         nR;
   boolean             vueassoc;

   vueassoc = False;
   if (pEl != NULL)
      if (pEl->ElAssocNum != 0)
	 /* l'element est dans un element associe */
	{
	   /* on remonte a la racine de l'arbre associe' */
	   while (pEl->ElParent != NULL)
	      pEl = pEl->ElParent;
	   nR = pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrListItem;
	   /* si l'element associe ne s'affiche pas en haut ou en bas de */
	   /* page, il s'affiche donc dans une vue specifique */
	   if (pEl->ElSructSchema->SsPSchema != NULL)
	      vueassoc = !pEl->ElSructSchema->SsPSchema->PsInPageHeaderOrFooter[nR - 1];
	}
   return vueassoc;
}


/* ---------------------------------------------------------------------- */
/* |    VueExiste retourne vrai si la vue de numero VueNb ou` doit      | */
/* |            s'afficher l'element pEl existe.                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             VueExiste (PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb)

#else  /* __STDC__ */
boolean             VueExiste (pEl, pDoc, VueNb)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber           VueNb;

#endif /* __STDC__ */

{
   boolean             vueexiste;

   if (VueAssoc (pEl))
     {
	/* c'est une vue d'elements associes, elle existe */
	/* si la fenetre correspondante existe */
	vueexiste = pDoc->DocAssocVolume[pEl->ElAssocNum - 1] != 0;
     }
   else
     {
	/* c'est une vue de l'arbre principal du document, elle existe */
	/* si l'entree correspondante de la table des vues du document */
	/* n'est pas libre. */
	vueexiste = pDoc->DocView[VueNb - 1].DvPSchemaView != 0;
     }
   return vueexiste;
}

/* ---------------------------------------------------------------------- */
/* |    VueAAppliquer retourne le numero de la vue (numero dans le      | */
/* |            schema de presentation qui s'applique a` l'element pEl  | */
/* |            ou a` l'attribut pAttr) dont il faut appliquer les      | */
/* |            regles de presentation a` l'element pEl pour obtenir    | */
/* |            son image dans la vue VueNb du document pDoc.           | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int              VueAAppliquer (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc,
				   DocViewNumber VueNb)
#else  /* __STDC__ */
int              VueAAppliquer (pEl, pAttr, pDoc, VueNb)
PtrElement          pEl;
PtrAttribute         pAttr;
PtrDocument         pDoc;
DocViewNumber           VueNb;

#endif /* __STDC__ */
{
   int              VueSch;
   PtrElement          pAsc;
   PtrSSchema        pSS;
   DocViewDescr         *pView;

   VueSch = 0;
   if (pEl->ElAssocNum != 0)
     {
	/* c'est un element associe', on applique */
	/* toujours les regles de la vue 1. */
	VueSch = 1;
     }
   else if (pDoc->DocView[VueNb - 1].DvPSchemaView != 0)
     {
	if (pAttr != NULL)
	   pSS = pAttr->AeAttrSSchema;
	else if (pEl != NULL)
	   pSS = pEl->ElSructSchema;
	else
	   pSS = NULL;
	if (pSS != NULL)
	  {
	     pView = &pDoc->DocView[VueNb - 1];
	     if (pView->DvSSchema == pDoc->DocSSchema)
		if (pSS == pDoc->DocSSchema)
		  {
		     /* c'est une element du document lui-meme */
		     VueSch = pView->DvPSchemaView;
		     /* numero de la vue dans le schema de */
		     /* presentation qui la definit */
		  }
		else
		   /* c'est un element d'un objet d'une autre nature */
		   VueSch = 1;
	     /* c'est une vue propre a une nature d'objets */
	     else if (pSS->SsPSchema == pView->DvSSchema->SsPSchema
		      || pEl->ElParent == NULL)
		/* c'est un objet de cette nature ou la racine du document */
		/* numero de la vue dans le schema de */
		/* presentation qui la definit */
		VueSch = pView->DvPSchemaView;
	     /* on applique les regles de presentation de la vue */
	     /* 1 de cette nature pour toutes les vues du document */
	     else
		/* l'element est-il dans un objet de cette nature ? */
		/* a priori non, donc l'element ne doit pas apparaitre dans */
		/* cette vue */
	       {
		  pAsc = pEl;
		  while (pAsc != NULL && VueSch == 0)
		     if (pAsc->ElSructSchema->SsPSchema == pView->DvSSchema->SsPSchema)
			/* on est dans un objet de cette nature */
		        /* on applique les regles de la vue 1 */
			VueSch = 1;
		     else
			pAsc = pAsc->ElParent;
	       }
	  }
     }
   return VueSch;
}


/* ---------------------------------------------------------------------- */
/* |    Retarde met une regle de presentation en attente au niveau du   | */
/* |            pave englobant le pave pPRP. Cette regle sera           | */
/* |            reappliquee lorsque tous les paves freres de pPRP auront| */
/* |            ete crees.                                              | */
/* |            - pR: SRule de presentation a` mettre en attente.       | */
/* |            - pSP: schema de presentation auquel appartient cette   | */
/* |            regle.                                                  | */
/* |            - pP: pave auquel doit s'appliquer la regle pR.         | */
/* |            - pAttr: attribut auquel correspond cette regle ou NULL | */
/* |            si ce n'est pas une regle de presentation d'attribut.   | */
/* |            Si l'englobant est un pave page ou un pave duplique,    | */
/* |            on met la regle en attente au niveau de la racine.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                Retarde (PtrPRule pR, PtrPSchema pSP, PtrAbstractBox pP, PtrAttribute pAttr,
			     PtrAbstractBox pPRP)

#else  /* __STDC__ */
void                Retarde (pR, pSP, pP, pAttr, pPRP)
PtrPRule        pR;
PtrPSchema          pSP;
PtrAbstractBox             pP;
PtrAttribute         pAttr;
PtrAbstractBox             pPRP;

#endif /* __STDC__ */

{
   PtrDelayedPRule     pDelR;
   PtrDelayedPRule     NpDelR;
   PtrAbstractBox             pAb;

   pAb = NULL;
   if (pPRP->AbEnclosing != NULL)
     {
	pAb = pPRP->AbEnclosing;
	/* si ce pave est un pave de presentation cree par la regle */
	/* FnCreateEnclosing, on met la regle en attente sur le pave englobant */
	if (pAb->AbEnclosing != NULL)
	   if (pAb->AbPresentationBox &&
	       pAb->AbElement == pPRP->AbElement)
	      pAb = pAb->AbEnclosing;
     }
#ifdef __COLPAGE__
   /* si pPRP->AbEnclosing est NULL, on retarde sur ce pave */
   else
      pAb = pPRP;
   /* si l'englobant est un pave de page, */
   /* on retarde la regle sur le pave racine */
   /* si le pave englobant est un pave duplique, on fait de meme */
   if ((pAb->AbElement->ElTerminal && pAb->AbElement->ElLeafType == LtPageColBreak)
       || pAb->AbNextRepeated != NULL || pAb->AbPreviousRepeated != NULL)
      while (pAb->AbEnclosing != NULL)
	 pAb = pAb->AbEnclosing;	/* pAb = pave racine */
#endif /* __COLPAGE__ */

   if (pAb != NULL)
     {
	GetRRetard (&NpDelR);
	NpDelR->DpPRule = pR;
	NpDelR->DpPSchema = pSP;
	NpDelR->DpAbsBox = pP;
	NpDelR->DpNext = NULL;
	NpDelR->DpAttribute = pAttr;
	pDelR = pAb->AbDelayedPRule;
	if (pDelR == NULL)
	   pAb->AbDelayedPRule = NpDelR;
	else
	  {
	     while (pDelR->DpNext != NULL)
		pDelR = pDelR->DpNext;
	     pDelR->DpNext = NpDelR;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    GetRet recupere une regle de presentation qui etait retardee    | */
/* |            au niveau du pave pP. Au retour pR contient la regle    | */
/* |            retardee, pSP contient le schema de presentation auquel | */
/* |            appartient cette regle, pP contient le pave auquel      | */
/* |            s'applique la regle retardee et pAttr contient          | */
/* |            l'attribut auquel correspond la regle, si c'est une     | */
/* |            regle d'attribut (NULL sinon). S'il n'y a pas de regle  | */
/* |            retardee, pR et pP contiennent NULL au retour.          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetRet (PtrPRule * pR, PtrPSchema * pSP, PtrAbstractBox * pP, PtrAttribute * pAttr)

#else  /* __STDC__ */
void                GetRet (pR, pSP, pP, pAttr)
PtrPRule       *pR;
PtrPSchema         *pSP;
PtrAbstractBox            *pP;
PtrAttribute        *pAttr;

#endif /* __STDC__ */

{
   PtrDelayedPRule     pDelR;
   PtrAbstractBox             pAb2;
   boolean             stop;
   PtrElement          pEl;

   pAb2 = *pP;
   /* a priori, pas de regle retardee */
   *pR = NULL;
   *pSP = NULL;
   *pP = NULL;
   *pAttr = NULL;
   if (pAb2 != NULL)
     {
	/* cherche le pave de l'element dans cette vue */
	/* saute les paves de presentation */
	stop = False;
	pEl = pAb2->AbElement;
	do
	   if (pAb2 == NULL)
	      stop = True;
	   else if (pAb2->AbElement != pEl)
	     {
		stop = True;
		pAb2 = NULL;
	     }
	   else if (!pAb2->AbPresentationBox)
	      stop = True;
	   else
	      pAb2 = pAb2->AbNext;
	while (!(stop));
	if (pAb2 != NULL)
	  {
	     pDelR = pAb2->AbDelayedPRule;
	     if (pDelR != NULL)
	       {
		  *pR = pDelR->DpPRule;
		  *pSP = pDelR->DpPSchema;
		  *pP = pDelR->DpAbsBox;
		  *pAttr = pDelR->DpAttribute;
		  pAb2->AbDelayedPRule = pDelR->DpNext;
		  FreeRRetard (pDelR);
	       }
	  }
     }
}

#ifdef __COLPAGE__

/* ---------------------------------------------------------------------- */
/* |    ApplReglesRetard applique les regles retardees conservees pour  | */
/* |            les paves de l'element El du document pDoc.             | */
/* |            on remonte les ascendants jusqu'a la racine             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ApplReglesRetard (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                ApplReglesRetard (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule        pRegle;
   PtrPSchema          pSPres;
   PtrAttribute         pAttr;
   PtrAbstractBox             pAb;
   int                 vue, lastview;
   PtrAbstractBox             pP;
   boolean             bool;

   if (VueAssoc (pEl))
      /* vue d'elements associes */
      lastview = 1;
   else
      /* nombre de vues du document */
      lastview = MAX_VIEW_DOC;
   for (vue = 0; vue < lastview; vue++)
     {
	pAb = pEl->ElAbstractBox[vue];
	if (pAb != NULL)
	   /* l'element a un pave dans cette vue */
	  {
	     pAb = pEl->ElAbstractBox[vue]->AbEnclosing;
	     /* pAb 1er ascendant sur lequel chercher une regle */
	     while (pAb != NULL)
	       {
		  do
		    {
		       /* la procedure Applique modifie pP, on le retablit */
		       pP = pAb;
		       GetRet (&pRegle, &pSPres, &pP, &pAttr);
		       if (pRegle != NULL)
			  Applique (pRegle, pSPres, pP, pDoc, pAttr, &bool);
		    }
		  while (pRegle != NULL);
		  pAb = pAb->AbEnclosing;
	       }
	  }
     }
}

#else  /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    ApplReglesRetard applique les regles retardees conservees pour  | */
/* |            les paves de l'element El du document pDoc.             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ApplReglesRetard (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                ApplReglesRetard (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule        pRegle;
   PtrPSchema          pSPres;
   PtrAttribute         pAttr;
   PtrAbstractBox             pAb, pP;
   int                 vue, lastview;

   if (VueAssoc (pEl))
      /* vue d'elements associes */
      lastview = 1;
   else
      /* nombre de vues du document */
      lastview = MAX_VIEW_DOC;
   for (vue = 0; vue < lastview; vue++)
     {
	pAb = pEl->ElAbstractBox[vue];
	if (pAb != NULL)
	   /* l'element a un pave dans cette vue */
	   if (pAb->AbEnclosing != NULL)
	      /* et ce pave a un pave englobant */
	     {
		if (pAb->AbEnclosing->AbPresentationBox &&
		    pAb->AbEnclosing->AbElement == pEl)
		   /* le pave englobant a ete cree' par la regle FnCreateEnclosing */
		   /* les regles retardees sont sur le pave' englobant */
		   pAb = pAb->AbEnclosing->AbEnclosing;
		else
		   pAb = pAb->AbEnclosing;
		do
		  {
		     /* la procedure Applique modifie pAb, on le retablit */
		     pP = pAb;
		     GetRet (&pRegle, &pSPres, &pP, &pAttr);
		     if (pRegle != NULL)
			Applique (pRegle, pSPres, pP, pDoc, pAttr);
		  }
		while (pRegle != NULL);
	     }
     }
}
#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    CondPresentation evalue les conditions d'application d'une      | */
/* |            regle de presentation qui s'applique a` l'element pEl,  | */
/* |            pour la vue de numero Vue.                              | */
/* |            pCond est la premiere condition de la chaine des        | */
/* |            conditions qui s'appliquent a la regle de presentation. | */
/* |            pSS est le schema de structure correspondant au schema  | */
/* |            de presentation auquel appartient la regle.             | */
/* |            Retourne vrai si les conditions sont toutes satisfaites.| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             CondPresentation (PtrCondition pCond, PtrElement pEl, PtrAttribute pAttr,
				      int Vue, PtrSSchema pSS)

#else  /* __STDC__ */
boolean             CondPresentation (pCond, pEl, pAttr, Vue, pSS)
PtrCondition        pCond;
PtrElement          pEl;
PtrAttribute         pAttr;
int                 Vue;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   boolean             ok, condcour, stop, egal;
   int                 valcompt, valmaxi, valmini, i;
   PtrPSchema          pSchP;
   PtrElement          pVois, pAsc, pElem;
   PtrReference        pRef;
   DocumentIdentifier     IDocExt;
   PtrDocument         pDocExt;
   PtrAttribute         pA;

   /* a priori les conditions sont satisfaites */
   ok = True;
   /* on examine toutes les conditions de la chaine */
   while (pCond != NULL && ok)
     {
	if (!pCond->CoTarget)
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
		pElem = ReferredElement (pRef, &IDocExt, &pDocExt);
	  }
	valcompt = 0;
	if (pElem == NULL)
	   ok = False;
	else if (pCond->CoCondition == PcEven || pCond->CoCondition == PcOdd
	    || pCond->CoCondition == PcOne || pCond->CoCondition == PcInterval)
	   /* evalue le compteur */
	  {
	     pSchP = pSS->SsPSchema;
	     if (pSchP != NULL)
		valcompt = ComptVal (pCond->CoCounter, pSS, pSchP, pElem, Vue);
	  }
	if (pElem != NULL)
	   switch (pCond->CoCondition)
		 {

		    case PcFirst:
		       /* on saute les marques de page precedentes */
		       pVois = pElem->ElPrevious;
		       stop = False;
		       do
			  if (pVois == NULL)
			     stop = True;
			  else if (pVois->ElTypeNumber == PageBreak + 1)
			     pVois = pVois->ElPrevious;
			  else
			     stop = True;
		       while (!(stop));
		       condcour = pVois == NULL;
		       break;

		    case PcLast:
		       /* on saute les marques de page suivantes */
		       pVois = pElem->ElNext;
		       stop = False;
		       do
			  if (pVois == NULL)
			     stop = True;
			  else if (pVois->ElTypeNumber == PageBreak + 1)
			     pVois = pVois->ElNext;
			  else
			     stop = True;
		       while (!(stop));
		       condcour = pVois == NULL;
		       /* traitement particulier pour les lignes de tableau */
		       if (ThotLocalActions[T_Tableau_Cond_Dernier]!= NULL)
			 (*ThotLocalActions[T_Tableau_Cond_Dernier])(pElem, &condcour);
		       break;

		    case PcReferred:
		       /* la condition est satisfaite si l'element (ou le */
		       /* premier de ses ascendants sur lequel peut porter une */
		       /* reference) est reference' au moins une fois. */
		       condcour = False;
		       pAsc = pElem;
		       do
			 {
			    if (pAsc->ElReferredDescr != NULL)
			       /* l'element est reference' */
			       condcour = True;
			    if (!condcour)
			       /* l'element n'est pas reference' */
			       /* on examine l'element ascendant */
			       pAsc = pAsc->ElParent;
			 }
		       while (pAsc != NULL && !condcour);
		       break;

		    case PcFirstRef:
		    case PcLastRef:
		       /* la condition est satisfaite s'il s'agit de la premiere ou
		          de la derniere reference a l'element reference' */
		       pRef = NULL;
		       if (pAttr != NULL &&
			   pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType == AtReferenceAttr)
			  /* c'est un attribut reference */
			  pRef = pAttr->AeAttrReference;
		       else if (pElem->ElTerminal && pElem->ElLeafType == LtReference)
			  /* l'element est une reference */
			  pRef = pElem->ElReference;
		       if (pRef != NULL)
			  if (pCond->CoCondition == PcFirstRef)
			     condcour = pEl->ElReference->RdPrevious == NULL;
			  else
			     condcour = pEl->ElReference->RdNext == NULL;
		       else
			  condcour = False;
		       break;

		    case PcExternalRef:
		    case PcInternalRef:
		       /* la condition est satisfaite s'il s'agit d'un */
		       /* element ou d'un attribut reference externe (ou interne) */
		       pRef = NULL;
		       condcour = False;
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
		       if (pRef == NULL)
			  condcour = False;
		       else if (pCond->CoCondition == PcInternalRef)
			  condcour = pRef->RdInternalRef;
		       else
			  condcour = !pRef->RdInternalRef;
		       break;

		    case PcCopyRef:
		       /* la condition est satisfaite si l'element est une copie */
		       condcour = pElem->ElIsCopy;
		       break;

		    case PcAnyAttributes:
		       /* la condition est satisfaite si l'element */
		       /* porte des attributs */
		       condcour = pElem->ElFirstAttr != NULL;
		       break;

		    case PcFirstAttr:
		       /* TODO */
		       /* la condition est satisfaite si le bloc */
		       /* attribut pAttr est le 1er de l'element */
		       if (pAttr != NULL)
			  condcour = pAttr == pEl->ElFirstAttr;
		       break;

		    case PcLastAttr:
		       /* TODO */
		       /* la condition est satisfaite si le bloc     */
		       /* attribut pAttr est le dernier de l'element */
		       if (pAttr != NULL)
			  condcour = pAttr->AeNext == NULL;
		       break;

		    case PcUserPage:
		       /* la condition est satisfaite si l'element
		          est un saut de page utilisateur */
		       if (pElem->ElTypeNumber == PageBreak + 1)
			  condcour = pElem->ElPageType == PgUser;
		       break;

		    case PcStartPage:
		       /* la condition est satisfaite si l'element
		          est un saut de page de debut */
		       if (pElem->ElTypeNumber == PageBreak + 1)
			  condcour = pElem->ElPageType == PgBegin;
		       break;

		    case PcComputedPage:
		       /* la condition est satisfaite si l'element
		          est un saut de page calcule */
		       if (pElem->ElTypeNumber == PageBreak + 1)
			  condcour = pElem->ElPageType == PgComputed;
		       break;

		    case PcEmpty:
		       /* la condition est satisfaite si l'element est vide */
		       if (pElem->ElTerminal)
			  if (pElem->ElLeafType == LtReference)
			     condcour = pElem->ElReference == NULL;
			  else
			     condcour = pElem->ElVolume == 0;
		       else
			  condcour = pElem->ElFirstChild == NULL;
		       break;

		    case PcEven:
		       condcour = !(valcompt & 1);
		       break;

		    case PcOdd:
		       condcour = (valcompt & 1);
		       break;

		    case PcOne:
		       condcour = (valcompt == 1);
		       break;

		    case PcInterval:
		       if (pCond->CoValCounter == CntMinVal)
			 {
			    /* Calcule la valeur mini du compteur */
			    valmini = MinMaxComptVal (pCond->CoCounter, pSS, pSchP, pElem,
						      Vue, False);
			    condcour = (valmini <= pCond->CoMaxCounter) &&
			       (valmini >= pCond->CoMinCounter);
			 }
		       else if (pCond->CoValCounter == CntMaxVal)
			 {
			    /* Calcule la valeur maxi du compteur */
			    valmaxi = MinMaxComptVal (pCond->CoCounter, pSS, pSchP, pElem,
						      Vue, True);
			    condcour = (valmaxi <= pCond->CoMaxCounter) &&
			       (valmaxi >= pCond->CoMinCounter);
			 }
		       else
			  /* Calcule la valeur courante du compteur */
			  condcour = (valcompt <= pCond->CoMaxCounter) &&
			     (valcompt >= pCond->CoMinCounter);
		       break;
		    case PcWithin:
		       /* condition sur le nombre d'ancetres d'un type donne' */
		       pAsc = pElem->ElParent;
		       if (pAsc == NULL)
			  /* aucun ancetre, condition non satisfaite */
			  condcour = False;
		       else
			 {
			    i = 0;
			    if (pCond->CoImmediate)
			       /* Condition: If immediately within n element-type */
			       /* Les n premiers ancetres successifs doivent etre du type */
			       /* CoTypeAncestor, sans comporter d'elements d'autres type */
			       /* on compte les ancetres successifs de ce type */
			       while (pAsc != NULL)
				 {
				    if (pCond->CoTypeAncestor != 0)
				       egal = ((pAsc->ElTypeNumber == pCond->CoTypeAncestor) &&
					       (pAsc->ElSructSchema->SsCode == pSS->SsCode));
				    else
				       egal = (strcmp (pCond->CoAncestorName, pAsc->ElSructSchema->SsRule[pAsc->ElTypeNumber - 1].SrName) == 0 &&
					       strcmp (pCond->CoSSchemaName, pAsc->ElSructSchema->SsName) == 0);
				    if (egal)
				      {
					 i++;
					 pAsc = pAsc->ElParent;
				      }
				    else
				       pAsc = NULL;
				 }
			    else
			       /* Condition: If within n element-type */
			       /* on compte tous les ancetres de ce type */
			       while (pAsc != NULL)
				 {
				    if (pCond->CoTypeAncestor != 0)
				       egal = ((pAsc->ElTypeNumber == pCond->CoTypeAncestor) &&
					       (pAsc->ElSructSchema->SsCode == pSS->SsCode));
				    else
				       egal = (strcmp (pCond->CoAncestorName, pAsc->ElSructSchema->SsRule[pAsc->ElTypeNumber - 1].SrName) == 0 &&
					       strcmp (pCond->CoSSchemaName, pAsc->ElSructSchema->SsName) == 0);
				    if (egal)
				       i++;
				    pAsc = pAsc->ElParent;	/* passe a l'element ascendant */
				 }
			    if (pCond->CoAncestorRel == CondEquals)
			       condcour = i == pCond->CoRelation;
			    else if (pCond->CoAncestorRel == CondGreater)
			       condcour = i > pCond->CoRelation;
			    else if (pCond->CoAncestorRel == CondLess)
			       condcour = i < pCond->CoRelation;
			 }
		       break;

		    case PcElemType:
		       /* verifie si l'attribut est attache' a un element du type voulu */
		       condcour = (pElem->ElTypeNumber == pCond->CoTypeElAttr);
		       break;

		    case PcAttribute:
		       /* verifie si l'element possede cet attribut */
		       pA = pElem->ElFirstAttr;
		       condcour = False;
		       while (pA != NULL && !condcour)
			  /* boucle sur les attributs de l'element */
			 {
			    if (pA->AeAttrNum == pCond->CoTypeElAttr)
			       condcour = True;
			    else
			       pA = pA->AeNext;	/* attribut suivant */
			 }
		       break;

		    case PcNoCondition:
		       condcour = True;
		       break;

		    case PcDefaultCond:
		       condcour = True;
		       break;
		 }

	if (!pCond->CoNotNegative)
	   condcour = !condcour;
	ok = ok && condcour;
	pCond = pCond->CoNextCondition;
     }

   return ok;
}

#ifndef __COLPAGE__

/* ---------------------------------------------------------------------- */
/* |    ElRefSuiv cherche (en arriere si Arriere est vrai, en avant     | */
/* |            sinon) a` partir de l'element pElRef, un element du     | */
/* |            type TypEl defini dans le schema pSchStr ou une marque  | */
/* |            de page concernant la vue traitee. Si une marque de     | */
/* |            page est trouvee, la fonction retourne NULL. Sinon      | */
/* |            elle retourne un pointeur sur l'element reference' par  | */
/* |            l'element trouve' et pElRef est l'element trouve'.      | */
/* |            L'element recherche' (TypEl) doit etre une reference.   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static PtrElement   ElRefSuiv (PtrElement * pElRef, int TypEl,
			 PtrSSchema pSchStr, boolean Arriere, int *VueSch)

#else  /* __STDC__ */
static PtrElement   ElRefSuiv (pElRef, TypEl, pSchStr, Arriere, VueSch)
PtrElement         *pElRef;
int                 TypEl;
PtrSSchema        pSchStr;
boolean             Arriere;
int                *VueSch;

#endif /* __STDC__ */

{
   PtrElement          Ret;
   boolean             fin;
   boolean             premier;
   PtrReference        pRef;

   Ret = NULL;
   fin = False;
   do
     {
	if (Arriere)
	  {
	     /* cherche en arriere */
	     *pElRef = BackSearchElem2Types (*pElRef, PageBreak + 1, TypEl, NULL, pSchStr);
	  }
	else
	  {
	     /* cherche en avant */
	     *pElRef = FwdSearchElem2Types (*pElRef, PageBreak + 1, TypEl, NULL, pSchStr);
	  }
	if (*pElRef == NULL)	/* on n'a rien trouve'. Fin sans succes. */
	   fin = True;
	else if ((*pElRef)->ElTypeNumber == PageBreak + 1
		 && (*pElRef)->ElViewPSchema == *VueSch)
	   /* on a trouve' une marque de page pour la vue traitee. */
	   /* Fin sans succes. */
	   fin = True;
	if (!fin)
	   /* on a trouve une marque de page pour une autre vue que la */
	   /* vue traitee (dans ce cas on continue la recherche), ou on a */
	   /* trouve' le type d'element cherche' */
	   if ((*pElRef)->ElTypeNumber == TypEl)
	      /* c'est le type d'element cherche', qui doit etre une */
	      /* reference. Cherche l'element reference' : Ret */
	      if ((*pElRef)->ElReference != NULL)
		{
		   premier = True;
		   /* cherche les references precedentes au meme element */
		   /* et qui ne sont pas dans des elements associes */
		   pRef = (*pElRef)->ElReference;
		   while (pRef->RdPrevious != NULL && premier)
		     {
			pRef = pRef->RdPrevious;
			if (pRef->RdElement != NULL)
			   if (!DansTampon (pRef->RdElement))
			      if (pRef->RdElement->ElAssocNum == 0)
				 /* il y a une reference precedente qui */
				 /* n'est pas dans le tampon */
				 /* et qui n'est pas un element associe' */
				 premier = False;
		     }
		   /* on ne prend en compte que la premiere reference a */
		   /* l'element qui n'est pas dans un element associe' */
		   if (premier)
		      if ((*pElRef)->ElReference->RdReferred != NULL)
			 /* si l'element reference' est dans le tampon */
			 /* couper/coller, on l'ignore */
			{
			   if ((*pElRef)->ElReference->RdReferred->ReExternalRef)
			      Ret = NULL;
			   else
			      Ret = (*pElRef)->ElReference->RdReferred->ReReferredElem;
			   if (Ret != NULL)
			      if (DansTampon (Ret))
				 Ret = NULL;
			   /* l'element reference' est */
			   /* dans le buffer couper-coller */
			      else
				{
				   fin = True;	/* on a trouve' */
				   if (RefAssocHautPage == NULL && !Arriere)
				      RefAssocHautPage = *pElRef;
				   /* on se souvient de l'element */
				   /* reference pour le module page */
				}
			}
		}
     }
   while (!(fin));

   return Ret;
}
#endif /* __COLPAGE__ */


#ifdef __COLPAGE__
/* ---------------------------------------------------------------------- */
/* | initpavpres  procedure appelee par CrPavPres pour initialiser      | */
/* |       le contenu du pave pAb de presentation nouvellement cree    | */
/* |       avec la regle pRCre                                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         initpavpres (PtrAbstractBox pAb, PtrDocument pDoc, PtrPRule pRCre,
		       PtrPSchema pSchP, PtrSSchema pSS, PtrPRule pRD,
				 PtrPRule pRS, PtrAttribute pAttr,
				 boolean CreateurComplet)

#else  /* __STDC__ */
static void         initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS, pAttr,
				 CreateurComplet)
PtrAbstractBox             pAb;
PtrDocument         pDoc;
PtrPRule        pRCre;
PtrPSchema          pSchP;
PtrSSchema        pSS;
PtrPRule        pRD;
PtrPRule        pRS;
PtrAttribute         pAttr;
boolean             CreateurComplet;

#endif /* __STDC__ */

{
   PtrPRule        pR, pRV;
   PtrAbstractBox             pAb1, pPavSv;
   PtrPSchema          pSP;
   PtrAttribute         pA;
   PtrElement          pEl;
   PresentationBox             *pBox;
   int                 VueNb, vue;
   int                 VueSch;
   boolean             ok;
   boolean             bool;

   if (pAb != NULL)
      /* rend non modifiable le pave de presentation */
     {
	pEl = pAb->AbElement;
	VueNb = pAb->AbDocView;
	VueSch = pDoc->DocView[VueNb - 1].DvPSchemaView;
	pAb->AbPresentationBox = True;
	/* c'est un pave de presentation */
	pAb->AbCreatorAttr = pAttr;
	/* on se souvient de l'attribut qui a cree' le pave' */
	pAb->AbTypeNum = pRCre->PrPresBox[0];
	pAb->AbCanBeModified = False;
	pAb->AbPSchema = pSchP;
	pAb->AbAcceptLineBreak =
	   pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbAcceptLineBreak;
	pAb->AbAcceptPageBreak =
	   pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbAcceptPageBreak;
	pAb->AbNotInLine =
	   pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbNotInLine;
	if (pRCre->PrPresBoxRepeat)
	   pAb->AbRepeatedPresBox = True;
	pAb->AbLeafType = LtCompound;
	pAb->AbVolume = 0;
	pAb->AbInLine = False;
	pAb->AbTruncatedHead = False;
	pAb->AbTruncatedTail = False;
	/* suppression code relatif aux elements associes en haut ou bas */
	/* applique les regles de presentation de la boite creee. */
	do
	  {
	     pR = GetRule (&pRS, &pRD, pEl, NULL, pEl->ElSructSchema);
	     /* pointeur sur la regle a  appliquer pour la vue 1 */
	     if (pR != NULL)
		/* cherche et applique les regles de tous les types */
		/* pour la vue */
		for (vue = 1; vue <= MAX_VIEW; vue++)
		  {
		     if (vue == 1)
			pRV = NULL;
		     else
			pRV = GetRuleView (&pRS, &pRD, pR->PrType, vue, pEl, NULL,
					   pEl->ElSructSchema);
		     if (vue == VueSch)
			/* applique la regle au pave cree'. */
		       {
			  if (pRV == NULL)
			     pRV = pR;
			  if (pRV->PrType == PtFunction
			      && (pRV->PrPresFunction == FnCreateFirst || pRV->PrPresFunction == FnCreateLast))
			     /* le pave cree' cree un pave de presentation */
			    {
			       /* cas boite bas de page supprime */
			       /* sauve le pointeur de pave de l'element */
			       pPavSv = pEl->ElAbstractBox[VueNb - 1];
			       /* change le pointeur de pave de l'element, */
			       /* pour un chainage correct du pave a creer */
			       pEl->ElAbstractBox[VueNb - 1] = pAb;
			       pAb->AbPresentationBox = False;
			       /* cree le pave de presentation */
			       pAb1 = CrPavPres (pEl, pDoc, pRV, pSS, NULL, VueNb, pSchP, False, True);
			       /* restaure le pointeur de pave de  l'element */
			       pEl->ElAbstractBox[VueNb - 1] = pPavSv;
			    }
			  else if (!CreateurComplet && pRV->PrPresMode == PresInherit
				   && pRV->PrInheritMode == InheritCreator)
			     /* toutes les regles de presentation n'ont */
			     /* pas encore ete appliquees au pave */
			     /* et le pave cree herite du createur, on */
			     /* differe l'application de la regle */
			     Retarde (pRV, pSchP, pAb, NULL, pAb);
			  else if (!Applique (pRV, pSchP, pAb, pDoc, NULL, &bool))
			     /* on n'a pas pu appliquer la regle, on */
			     /* l'appliquera lorsque le pave pere */
			     /* sera  termine' */
			     Retarde (pRV, pSchP, pAb, NULL, pAb);
		       }
		  }
	  }
	while (!(pR == NULL));
	pAb->AbPresentationBox = True;
	/* met le contenu dans le pave cree */
	pBox = &pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1];
	switch (pBox->PbContent)
	      {
		 case FreeContent:
		    break;
		 case ContVariable:
		    ok = NouvVariable (pBox->PbContVariable, pSS, pSchP, pAb, pDoc);
		    break;
		 case ContConst:
		    CopieConstante (pBox->PbContConstant, pSchP, pAb);
		    break;
		 case ContElement:
		    /* une boite qui regroupe des elements associes */
		    /* rend modifiable le pave de presentation */
		    pAb->AbCanBeModified = True;
		    /* la creation des elements reference's */
		    /* n'est pas traitee ici */
		    break;
	      }
	/* plus de creation stockee en attente dans queuePR */
	/* applique les regles retardees */
	do
	  {
	     pAb1 = pAb;
	     GetRet (&pR, &pSP, &pAb1, &pA);
	     if (pR != NULL)
		if (!Applique (pR, pSP, pAb1, pDoc, pA, &bool))
		   Retarde (pR, pSP, pAb1, pA, pAb);
	  }
	while (!(pR == NULL));
	/* ajoute le volume du pave cree' a celui de tous ses */
	/* englobants */
	if (pAb->AbVolume > 0)
	  {
	     pAb1 = pAb->AbEnclosing;
	     while (pAb1 != NULL)
	       {
		  pAb1->AbVolume += pAb->AbVolume;
		  pAb1 = pAb1->AbEnclosing;
	       }
	  }
	/* met a jour le volume libre restant dans la vue */
	MajVolLibre (pAb, pDoc);
     }
}
#endif /* __COLPAGE__ */

#ifdef __COLPAGE__

/* ---------------------------------------------------------------------- */
/* |    CrPavPres applique a` la vue VueNb la regle de creation de boite| */
/* |            de presentation pRCre dans le document pDoc, pour       | */
/* |            l'element pEl. Cette regle vient du schema de           | */
/* |            presentation associe' au schema de structure pSS.       | */
/* |            Retourne le pave cree'. DansBoiteAssoc indique si le    | */
/* |            createur est une boite de haut ou de bas de page        | */
/* |            affichant des elements associes.                        | */
/* |            CreateurComplet indique si toutes les regles de         | */
/* |            presentation ont deja ete appliquees au pave createur.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrAbstractBox             CrPavPres (PtrElement pEl, PtrDocument pDoc, PtrPRule pRCre,
		       PtrSSchema pSS, PtrAttribute pAttr, DocViewNumber VueNb,
			       PtrPSchema pSchP, boolean DansBoiteAssoc,
			       boolean CreateurComplet)

#else  /* __STDC__ */
PtrAbstractBox             CrPavPres (pEl, pDoc, pRCre, pSS, pAttr, VueNb, pSchP, DansBoiteAssoc,
			       CreateurComplet)
PtrElement          pEl;
PtrDocument         pDoc;
PtrPRule        pRCre;
PtrSSchema        pSS;
PtrAttribute         pAttr;
DocViewNumber           VueNb;
PtrPSchema          pSchP;
boolean             DansBoiteAssoc;
boolean             CreateurComplet;

#endif /* __STDC__ */

{
   int                 vue, vis;
   int                 VueSch;
   PtrPRule        pRD, pRS;
   PtrPRule        pR, pR1;
   PtrAbstractBox             pAb, pP1;
   boolean             ok, stop;
   TypeUnit            unit;
   PtrAbstractBox             pPavElem;
   boolean             dejacree;

   pAb = NULL;
   ok = False;
   if (VueExiste (pEl, pDoc, VueNb))	/* la vue existe */
     {
	VueSch = VueAAppliquer (pEl, pAttr, pDoc, VueNb);
	/* faut-il reellement creer ce pave ? */
	if (pRCre->PrCond == NULL)
	   /* creation inconditionnelle */
	   ok = True;
	else
	   /* On verifie les conditions d'application de la regle de creation */
	   ok = CondPresentation (pRCre->PrCond, pEl, pAttr, VueSch, pSS);
     }
   /* on ne cree un pave de presentation que si le pave de l'element qui */
   /* provoque la creation existe dans la vue. */
   if (ok)
      if (pEl->ElAbstractBox[VueNb - 1] == NULL)
	 ok = False;
   /* on ne cree pas de pave fils pour un element holophraste' */
   if (ok)
      if (pEl->ElHolophrast)
	 if (pRCre->PrPresFunction == FnCreateFirst || pRCre->PrPresFunction == FnCreateLast)
	    ok = False;
   /* on ne cree pas de pave de presentation fils pour un element terminal */
   if (ok)
      if (pEl->ElTerminal)
	 if (pRCre->PrPresFunction == FnCreateFirst || pRCre->PrPresFunction == FnCreateLast)
	    ok = False;
   /* on ne cree pas de pave de presentation qui soit un frere ou le pere du */
   /* pave racine de la vue. */
   if (ok)
      if (pEl->ElAbstractBox[VueNb - 1]->AbEnclosing == NULL
	  && (pRCre->PrPresFunction == FnCreateBefore || pRCre->PrPresFunction == FnCreateAfter ||
	      pRCre->PrPresFunction == FnCreateEnclosing))
	 ok = False;

   /* si c'est une boite de haut ou de bas de page qui regroupe les */
   /* elements associes reference's dans la page, on verifie qu'il */
   /* existe bien de tels elements */
   /* on ne cree jamais le pave correspondant a la racine des */
   /* elements associes, meme s'il y en a dans la page */
   /* car les elements associes seront places au fur et a */
   /* mesure de l'apparition de references dans la page. */
   /* lors de la creation dans la page du premier pave reference, */
   /* on creera le pave pere (pave de l'element racine) */
   /* on cree les paves de presentation de haut et bas de page */
   /* avant, donc sans savoir ce qu'il y a dans la page */
   /* Remarque : pas de traitement differencie entre haut */
   /* et bas de page */
   /* plus rien a faire car la regle content est dans la */
   /* boite de haut ou bas de page */
   /* il n'y a plus de boite de presentation correspondant */
   /* a la racine des elements associes : c'est un pave */
   /* d'element normal : il n'apparait pas ici */

   if (ok)
      /* s'il s'agit d'une vue affichant seulement un sous-arbre, on */
      /* ne cree pas les paves de presentation des elements ascendants */
      /* du sous-arbre en question. */
      if (pEl->ElAssocNum > 0)
	{
	   /* element associe' */
	   if (pDoc->DocAssocSubTree[pEl->ElAssocNum - 1] != NULL)
	      /* on n'affiche qu'un sous-arbre de ces elements associes */
	      if (ElemIsAnAncestor (pEl, pDoc->DocAssocSubTree[pEl->ElAssocNum - 1]))
		{
		   /* l'element createur englobe le sous-arbre affichable */
		   ok = False;
		   /* on ne cree pas le pave' de presentation */
		}
	}
      else
	{
	   /* vue de l'arbre principal */
	   if (pDoc->DocViewSubTree[VueNb - 1] != NULL)
	      /* on n'affiche qu'un sous-arbre dans cette vue */
	      if (ElemIsAnAncestor (pEl, pDoc->DocViewSubTree[VueNb - 1]))
		{
		   /* l'element createur englobe le sous-arbre affichable */
		   ok = False;
		   /* on ne cree pas le pave' de presentation */
		}
	}
   if (ok)
     {
	/* on cree le pave (ou non, selon sa visibilite) */
	/* pRS : premiere regle de presentation */
	/* specifique de la boite a creer */
	pRS = pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1].PbFirstPRule;
	/* pRD : premiere regle de presentation par defaut du schema de */
	/* presentation */
	pRD = pSchP->PsFirstDefaultPRule;
	/* le volume des paves englobant ne comprend pas le volume du pave */
	/* que l'on va creer */

	/* parcourt toutes les vues pour trouver toutes les regles de */
	/* visibilite */
	vis = 0;
	for (vue = 1; vue <= MAX_VIEW; vue++)
	  {
	     if (vue == 1)
	       {
		  /* vue principale */
		  pR1 = GetRule (&pRS, &pRD, pEl, NULL, pEl->ElSructSchema);
		  /* regle de visibilite pour la vue 1 */
		  pR = pR1;
	       }
	     else
	       {
		  /* s'il y a une regle de visibilite pour cette vue, on la  *prend */
		  pR = GetRuleView (&pRS, &pRD, PtVisibility, vue, pEl, NULL,
				    pEl->ElSructSchema);
		  if (pR == NULL)
		     /* sinon, on prend celle de la vue 1 */
		     pR = pR1;
	       }
	     if (vue == VueSch)
	       {
		  /* c'est la regle de la vue traitee */
		  vis = valintregle (pR, pEl, VueNb, &ok, &unit, NULL);
		  /* si la regle de visibilite n'a pas pu etre appliquee, */
		  /* on prend la visibilite du pave de l'element createur */
		  if (!ok)
		     vis = pEl->ElAbstractBox[VueNb - 1]->AbVisibility;
	       }
	  }

	if (vis >= 1)
	   /* le pave a creer est visible dans la vue */
	  {
	     pPavElem = pEl->ElAbstractBox[VueNb - 1];
	     /* pPavElem:1er pave de l'element createur */
	     switch (pRCre->PrPresFunction)
		   {
		      case FnCreateFirst:
			 pP1 = pPavElem;
			 /* cas des paves de presentation de la racine, */
			 /* il faut les placer */
			 /* sous le pave corps de page s'il existe */
			 if (pP1->AbEnclosing == NULL && pP1->AbFirstEnclosed != NULL
			     && pP1->AbFirstEnclosed->AbElement->ElTypeNumber ==
			     PageBreak + 1)
			   {
			      pP1 = pP1->AbFirstEnclosed;
			      /* recherche du premier pave corps de page (on saute le */
			      /* haut de page s'il existe */
			      while (pP1 != NULL && pP1->AbPresentationBox)
				 pP1 = pP1->AbNext;
			      /* on descend d'un niveau pour les colonnes */
			      while (pP1 != NULL &&
				     pP1->AbFirstEnclosed != NULL &&
				     pP1->AbFirstEnclosed->AbElement->ElTypeNumber
				     == PageBreak + 1)
				{
				   pP1 = pP1->AbFirstEnclosed;
				   /* on saute les eventuels paves de presentation */
				   while (pP1 != NULL && pP1->AbPresentationBox)
				      pP1 = pP1->AbNext;
				}
			      if (pP1 == NULL)
				{
				   printf ("erreur i.a. creation pav pres racine  \n");
				   AffPaveDebug (pEl->ElAbstractBox[VueNb - 1]);
				}
			   }
			 else
			    /* saute les paves de presentation deja crees */
			    /* avec la regle CreateBefore */
			    while (pP1->AbPresentationBox)
			       pP1 = pP1->AbNext;
			 /* pP1 : pave de la chaine des paves dupliques */
			 pPavElem = pP1;	/* pour boucle sur les paves dupliques */
			 /* boucle sur les paves dupliques de l'element si la regle */
			 /* a l'option REP */
			 do
			   {
			      dejacree = False;
			      pP1 = pPavElem;
			      if (pP1->AbFirstEnclosed != NULL)
				 /* il y a deja des paves englobes */
				{
				   pP1 = pP1->AbFirstEnclosed;
				   /* saute les paves deja crees par une regle */
				   /* FnCreateFirst et les paves des marques de page */
				   /* de debut d'element */
				   /* et verifie si le pave de presentation existe deja */
				   stop = False;
				   do
				      if (pP1->AbPresentationBox && pP1->AbElement == pEl)
					 /* suppression de la condition sur le pave marque de page */
					 /* c'est un pave de presentation de l'element */
					 if (pP1->AbTypeNum == pRCre->PrPresBox[0]
					     && pP1->AbPresentationBox
					     && pP1->AbPSchema == pSchP
					     && (!pP1->AbDead))
					    /* c'est le meme que celui qu'on veut creer */
					   {
					      dejacree = True;
					      stop = True;
					   }
					 else if (pP1->AbNext == NULL)
					    stop = True;
					 else
					    pP1 = pP1->AbNext;
				      else
					 /* ce n'est ni pave de presentation de l'element */
					 /* ni un saut de page de debut d'element */
					 stop = True;
				   while (!stop);
				}
			      if (!dejacree)
				{
				   /* on cree le pave */
				   pAb = initpave (pEl, VueNb, vis);
				   pAb->AbEnclosing = pPavElem;
				   pAb->AbReadOnly = pPavElem->AbReadOnly;
				   if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
				      /* c'est le premier pave englobe' */
				      pPavElem->AbFirstEnclosed = pAb;
				   else
				      /* pP1 est le pave a cote de qui il faut chainer pAb */
				   if (pP1->AbElement == pEl)
				      /* chaine le nouveau pave apres le dernier pave */
				      /* de presentation cree par une regle FnCreateFirst */
				      /* ou la derniere marque de page de debut d'elem. */
				     {
					pAb->AbPrevious = pP1;
					pAb->AbNext = pP1->AbNext;
					pAb->AbPrevious->AbNext = pAb;
					if (pAb->AbNext != NULL)
					   pAb->AbNext->AbPrevious = pAb;
				     }
				   else
				      /* chaine le nouveau pave avant le premier pave */
				      /* du premier fils du createur */
				      /* ce traitement est correct meme pout les paves */
				      /* de presentation fils de la racine car ils sont */
				      /* toujours crees apres les paves du */
				      /* premier element sous le premier corps de page */
				     {
					pAb->AbNext = pP1;
					pAb->AbPrevious = pP1->AbPrevious;
					pAb->AbNext->AbPrevious = pAb;
					if (pAb->AbPrevious != NULL)
					   pAb->AbPrevious->AbNext = pAb;
					if (pAb->AbEnclosing->AbFirstEnclosed == pP1)
					   pAb->AbEnclosing->AbFirstEnclosed = pAb;
				     }
				   initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS,
						pAttr, CreateurComplet);
				}
			      if (pRCre->PrPresBoxRepeat)
				 pPavElem = pPavElem->AbNextRepeated;
			      else
				 pPavElem = NULL;
			   }
			 while (pPavElem != NULL);	/* fin boucle de parcours des dup */
			 break;
		      case FnCreateLast:
			 pP1 = pPavElem;
			 /* cas des paves de presentation de la racine, il faut les placer */
			 /* sous le dernier pave corps de page s'il en existe */
			 if (pP1->AbEnclosing == NULL && pP1->AbFirstEnclosed != NULL
			     && pP1->AbFirstEnclosed->AbElement->ElTypeNumber ==
			     PageBreak + 1)
			   {
			      pP1 = pP1->AbFirstEnclosed;
			      /* recherche du dernier pave corps de page  */
			      while (pP1->AbNext != NULL)
				 pP1 = pP1->AbNext;
			      /* on saute les paves bas et filet de page s'ils existent */
			      while (pP1 != NULL
				     && (pP1->AbPresentationBox || pP1->AbLeafType != LtCompound))
				 pP1 = pP1->AbPrevious;

			      /* on descend la hierarchie des colonnes */
			      while (pP1->AbFirstEnclosed != NULL &&
				     pP1->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
				{
				   pP1 = pP1->AbFirstEnclosed;
				   while (pP1->AbNext != NULL)
				      pP1 = pP1->AbNext;
				   while (pP1->AbPresentationBox)
				      pP1 = pP1->AbPrevious;
				}
			   }
			 else
			   {
			      /* saute les paves de presentation deja crees */
			      /* avec la regle CreateBefore */
			      while (pP1->AbPresentationBox)
				 pP1 = pP1->AbNext;
			      /* on se positionne sur le dernier pave duplique de l'element */
			      while (pP1->AbNextRepeated != NULL)
				 pP1 = pP1->AbNextRepeated;
			   }
			 pPavElem = pP1;	/* pour parcours des dup */
			 do
			   {	/* parcours des dup si le booleen pRCre->PrPresBoxRepeat est vrai */
			      dejacree = False;		/* a priori ce pave n'est pas deja cree */
			      pP1 = pPavElem;
			      /* on verifie que le pave de presentation n'existe pas deja */
			      if (pPavElem->AbFirstEnclosed != NULL)
				{
				   pP1 = pPavElem->AbFirstEnclosed;
				   /* cherche le dernier fils du pave createur */
				   /* et verifie si le pave de presentation existe deja */
				   stop = False;
				   do
				      if (pP1->AbTypeNum == pRCre->PrPresBox[0]
					  && pP1->AbPresentationBox
					  && pP1->AbPSchema == pSchP
					  && (!pP1->AbDead))
					 /* ce pave de presentation existe deja */
					{
					   dejacree = True;
					   stop = True;
					}
				      else if (pP1->AbNext == NULL)
					 stop = True;
				      else
					 pP1 = pP1->AbNext;
				   while (!stop);
				}
			      /* chaine le nouveau pave apres le dernier pave */
			      /* fils du pave createur */
			      /* ce traitement reste correct meme dans le cas des paves */
			      /* de presentation fils de la racine car ils seront */
			      /* toujours crees apres les paves de presentation du */
			      /* dernier element sous le dernier corps de page */
			      if (!dejacree)
				{
				   /* on cree le pave */
				   pAb = initpave (pEl, VueNb, vis);
				   pAb->AbEnclosing = pPavElem;
				   pAb->AbReadOnly = pPavElem->AbReadOnly;
				   if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
				      /* c'est le premier pave englobe' */
				      pPavElem->AbFirstEnclosed = pAb;
				   else
				      /* pP1 est le pave a cote de qui il faut chainer pAb */
				     {
					pAb->AbPrevious = pP1;
					pAb->AbNext = pP1->AbNext;
					pP1->AbNext = pAb;
				     }
				   initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS,
						pAttr, CreateurComplet);
				}
			      if (pRCre->PrPresBoxRepeat)
				 pPavElem = pPavElem->AbPreviousRepeated;
			      else
				 pPavElem = NULL;
			   }
			 while (pPavElem != NULL);	/* fin boucle de parcours des dup */
			 break;
		      case FnCreateBefore:
			 do
			   {	/* boucle des dup si pRCre->PrPresBoxRepeat est vrai */
			      dejacree = False;		/* a priori ce pave n'est pas deja cree */
			      pP1 = pPavElem;
			      /* saute les paves de presentation deja crees */
			      /* avec la regle CreateBefore */
			      while (pP1->AbPresentationBox)
				{
				   if (pP1->AbTypeNum == pRCre->PrPresBox[0]
				       && pP1->AbPresentationBox
				       && pP1->AbPSchema == pSchP
				       && (!pP1->AbDead))
				      /* ce pave de presentation existe deja */
				      dejacree = True;
				   pP1 = pP1->AbNext;
				}
			      pPavElem = pP1;
			      if (!dejacree)
				{
				   /* on cree le pave */
				   pAb = initpave (pEl, VueNb, vis);
				   pAb->AbReadOnly = pP1->AbReadOnly;
				   pAb->AbEnclosing = pP1->AbEnclosing;
				   if (pAb->AbEnclosing->AbFirstEnclosed == pP1)
				      pAb->AbEnclosing->AbFirstEnclosed = pAb;
				   pAb->AbNext = pP1;
				   pAb->AbPrevious = pP1->AbPrevious;
				   pP1->AbPrevious = pAb;
				   if (pAb->AbPrevious != NULL)
				      pAb->AbPrevious->AbNext = pAb;
				   if (pP1->AbPreviousRepeated == NULL)
				      if (pAb->AbPrevious == NULL)
					 pEl->ElAbstractBox[VueNb - 1] = pAb;
				      else if (pAb->AbPrevious->AbElement != pEl)
					 pEl->ElAbstractBox[VueNb - 1] = pAb;
				   initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS,
						pAttr, CreateurComplet);
				}
			      if (pRCre->PrPresBoxRepeat)
				{
				   pPavElem = pPavElem->AbNextRepeated;
				   if (pPavElem != NULL)
				      while (pPavElem->AbPrevious != NULL
					     && pPavElem->AbPrevious->AbElement == pEl)
					 pPavElem = pPavElem->AbPrevious;
				   /* on se positionne sur le premier pave de pEl */
				   /* pour verifier si le pave de presentation */
				   /* n'est pas deja cree */
				}
			      else
				 pPavElem = NULL;
			   }
			 while (pPavElem != NULL);	/* fin boucle de parcours des dup */
			 break;
		      case FnCreateAfter:
			 pP1 = pPavElem;
			 /* on se positionne sur le dernier pave duplique de l'element */
			 while (pP1->AbPresentationBox)	/* on saute les paves de pres avant */
			    pP1 = pP1->AbNext;
			 while (pP1->AbNextRepeated != NULL)
			    pP1 = pP1->AbNextRepeated;
			 pPavElem = pP1;	/* pour parcours des dup */
			 do
			   {	/* boucle des dup si pRCre->PrPresBoxRepeat est vrai */
			      dejacree = False;		/* a priori ce pave n'est pas deja cree */
			      pP1 = pPavElem;
			      stop = False;
			      do
				 if (pP1->AbNext == NULL)
				    stop = True;
				 else if (pP1->AbNext->AbElement != pEl)
				    stop = True;
				 else
				   {
				      if (pP1->AbNext->AbPresentationBox)
					 if (pP1->AbNext->AbTypeNum == pRCre->PrPresBox[0]
					     && pP1->AbNext->AbPresentationBox
					     && pP1->AbNext->AbPSchema == pSchP
					     && (!pP1->AbNext->AbDead))
					    /* ce pave de presentation existe deja */
					    dejacree = True;
				      pP1 = pP1->AbNext;
				   }
			      while (!(stop));
			      if (!dejacree)
				{
				   /* on cree le pave */
				   pAb = initpave (pEl, VueNb, vis);
				   pAb->AbReadOnly = pP1->AbReadOnly;
				   pAb->AbEnclosing = pP1->AbEnclosing;
				   pAb->AbNext = pP1->AbNext;
				   pAb->AbPrevious = pP1;
				   pP1->AbNext = pAb;
				   if (pAb->AbNext != NULL)
				      pAb->AbNext->AbPrevious = pAb;
				   initpavpres (pAb, pDoc, pRCre, pSchP, pSS, pRD, pRS,
						pAttr, CreateurComplet);
				}
			      if (pRCre->PrPresBoxRepeat)
				 pPavElem = pPavElem->AbPreviousRepeated;
			      else
				 pPavElem = NULL;
			   }
			 while (pPavElem != NULL);	/* fin boucle de parcours des dup */
			 break;
		      default:;
			 break;
		   }

	  }
     }
   return pAb;
}

#else  /* __COLPAGE__ */

/* ---------------------------------------------------------------------- */
/* |    CrPavPres applique a` la vue VueNb la regle de creation de boite| */
/* |            de presentation pRCre dans le document pDoc, pour       | */
/* |            l'element pEl. Cette regle vient du schema de           | */
/* |            presentation associe' au schema de structure pSS.       | */
/* |            Retourne le pave cree'. DansBoiteAssoc indique si le    | */
/* |            createur est une boite de haut ou de bas de page        | */
/* |            affichant des elements associes.                        | */
/* |            CreateurComplet indique si toutes les regles de         | */
/* |            presentation ont deja ete appliquees au pave createur.  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrAbstractBox             CrPavPres (PtrElement pEl, PtrDocument pDoc, PtrPRule pRCre,
		       PtrSSchema pSS, PtrAttribute pAttr, DocViewNumber VueNb,
			       PtrPSchema pSchP, boolean DansBoiteAssoc,
			       boolean CreateurComplet)

#else  /* __STDC__ */
PtrAbstractBox             CrPavPres (pEl, pDoc, pRCre, pSS, pAttr, VueNb, pSchP, DansBoiteAssoc,
			       CreateurComplet)
PtrElement          pEl;
PtrDocument         pDoc;
PtrPRule        pRCre;
PtrSSchema        pSS;
PtrAttribute         pAttr;
DocViewNumber           VueNb;
PtrPSchema          pSchP;
boolean             DansBoiteAssoc;
boolean             CreateurComplet;

#endif /* __STDC__ */

{
   int                 vue, vis;
   int                 VueSch;
   int                 volume;
   PtrPRule        pRD, pRS;
   PtrPRule        pR, pR1, pRV;
   PtrAbstractBox             pAb, pP1, pPavSv;
   PtrAbstractBox             PavCree;
   PtrElement          pE, pER, pVoisin;
   boolean             ok, stop, fin, volok;
   PtrPRule        queuePR[MAX_QUEUE_LEN];
   int                 lqueue, pqueue;
   PtrPSchema          pSP;
   PtrAttribute         pA;
   PresentationBox             *pBox;
   boolean             complet;
   TypeUnit            unit;

   PavCree = NULL;
   pAb = NULL;
   ok = False;
   pER = NULL;
   if (VueExiste (pEl, pDoc, VueNb))	/* la vue existe */
     {
	VueSch = VueAAppliquer (pEl, pAttr, pDoc, VueNb);
	/* faut-il reellement creer ce pave ? */
	if (pRCre->PrCond == NULL)
	   ok = True;
	else
	   /* On verifie les conditions d'application de la regle de creation */
	   ok = CondPresentation (pRCre->PrCond, pEl, pAttr, VueSch, pSS);
     }
   /* on ne cree un pave de presentation que si le pave de l'element qui */
   /* provoque la creation existe dans la vue. */
   if (ok)
      if (pEl->ElAbstractBox[VueNb - 1] == NULL)
	 ok = False;
   /* on ne cree pas de pave fils pour un element holophraste' */
   if (ok)
      if (pEl->ElHolophrast)
	 if (pRCre->PrPresFunction == FnCreateFirst || pRCre->PrPresFunction == FnCreateLast)
	    ok = False;
   /* on ne cree pas de pave de presentation qui soit un frere ou le pere du */
   /* pave racine de la vue. */
   if (ok)
      if (pEl->ElAbstractBox[VueNb - 1]->AbEnclosing == NULL
	  && (pRCre->PrPresFunction == FnCreateBefore || pRCre->PrPresFunction == FnCreateAfter ||
	      pRCre->PrPresFunction == FnCreateWith || pRCre->PrPresFunction == FnCreateEnclosing))
	 ok = False;
   /* si c'est une boite de haut de page et qu'il s'agit de la derniere */
   /* marque de page du document, on ne cree pas la boite */
   if (ok)
      if (pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1].PbPageHeader)
	 /* c'est une boite de haut de page */
	{
	   pE = pEl;
	   do
	      /* cherche le 1er frere suivant de pE qui ne */
	      /* soit pas un saut de page */
	     {
		pVoisin = pE->ElNext;
		stop = False;
		do
		   if (pVoisin == NULL)
		     {
			stop = True;
			/* c'etait le dernier frere */
		     }
		   else
		     {
			/* il y a un frere suivant */
			if (pVoisin->ElTerminal && pVoisin->ElTypeNumber == PageBreak + 1)
			   /* c'est une marque de page, on passe au suivant */
			   pVoisin = pVoisin->ElNext;
			else
			   /* ce n'est pas une marque de page, on a trouve' */
			   stop = True;
		     }
		while (!(stop));
		if (pVoisin == NULL)
		   /* on n'a pas trouve de suivant qui ne soit */
		   /* pas une marque de page */
		  {
		     pE = pE->ElParent;
		     /* cherche au niveau superieur */
		     if (pE == NULL)
			/* on est a la racine, c'est donc la */
			/* derniere marque de page du document */
			ok = False;
		     /* on ne cree pas la boite */
		  }
		else
		   /* il y a un suivant, on arrete */
		   pE = NULL;
	     }
	   while (!(pE == NULL));
	}
   /* si c'est une boite de haut ou de bas de page qui regroupe les */
   /* elements associes reference's dans la page, on verifie qu'il */
   /* existe bien de tels elements */
   if (ok)
     {
	pBox = &pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1];
	if (pBox->PbContent == ContElement)
	   /* il faut creer une boite qui regroupe des elements associes */
	   /* on ne fait rien si c'est une boite de haut de page et que */
	   /* son contenu ne doit pas etre cree */
	   if (pBox->PbPageHeader && !pEl->ElAssocHeader)
	     {
		ok = False;
		/* on ne cree pas la boite */
		/* indique qu'il faudra creer cette boite plus tard. */
		/* Ce sera fait par le module page. */
		NbBoiteHautPageACreer = 1;
		PageConcernee = pEl;
		RegleCreation = pRCre;
		SchPresRegle = pSchP;
		pEl->ElAssocHeader = True;
	     }
	   else
	     {
		/* cherche s'il y a dans la page une reference a un element
		   associe de ce type */
		pE = pEl;
		stop = False;
		do
		  {
		     pER = ElRefSuiv (&pE, pBox->PbContRefElem, pEl->ElSructSchema,
				      pBox->PbPageFooter, &VueSch);
		     if (pER == NULL)
			/* pas de reference dans la page */
		       {
			  stop = True;
			  ok = False;
			  /* on ne cree pas la boite de haut ou bas de page */
		       }
		     else
			/* on a trouve' dans la page une reference */
		     if (DansTampon (pER))
			/* l'element reference' est dans le buffer des elements */
			/* coupe's, il n'apparait donc pas dans la boite a creer */
			/* et on cherche un autre element reference' dans la page */
			pER = NULL;
		     else
		       {
			  /* l'element reference' existe bien, il faut creer la */
			  /* boite de bas ou de haut de page */
			  stop = True;
		       }
		  }
		while (!(stop));
	     }
     }

   if (ok)
      /* s'il s'agit d'une vue affichant seulement un sous-arbre, on */
      /* ne cree pas les paves de presentation des elements ascendants */
      /* du sous-arbre en question. */
      if (pEl->ElAssocNum > 0)
	{
	   /* element associe' */
	   if (pDoc->DocAssocSubTree[pEl->ElAssocNum - 1] != NULL)
	      /* on n'affiche qu'un sous-arbre de ces elements associes */
	      if (ElemIsAnAncestor (pEl, pDoc->DocAssocSubTree[pEl->ElAssocNum - 1]))
		{
		   /* l'element createur englobe le sous-arbre affichable */
		   ok = False;
		   /* on ne cree pas le pave' de presentation */
		}
	}
      else
	{
	   /* vue de l'arbre principal */
	   if (pDoc->DocViewSubTree[VueNb - 1] != NULL)
	      /* on n'affiche qu'un sous-arbre dans cette vue */
	      if (ElemIsAnAncestor (pEl, pDoc->DocViewSubTree[VueNb - 1]))
		{
		   /* l'element createur englobe le sous-arbre affichable */
		   ok = False;
		   /* on ne cree pas le pave' de presentation */
		}
	}
   if (ok)
     {
	/* on cree le pave (ou non, selon sa visibilite) */
	/* pRS : premiere regle de presentation */
	/* specifique de la boite a creer */
	pRS = pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1].PbFirstPRule;
	/* pRD : premiere regle de presentation par defaut du schema de */
	/* presentation */
	pRD = pSchP->PsFirstDefaultPRule;
	/* le volume des paves englobant ne comprend pas le volume du pave */
	/* que l'on va creer */
	volok = False;
	/* parcourt toutes les vues pour trouver toutes les regles de */
	/* visibilite */
	vis = 0;
	pR1 = NULL;
	for (vue = 1; vue <= MAX_VIEW; vue++)
	  {
	     if (vue == 1)
	       {
		  /* vue principale */
		  pR1 = GetRule (&pRS, &pRD, pEl, pAttr, pEl->ElSructSchema);
		  /* regle de visibilite pour la vue 1 */
		  pR = pR1;
	       }
	     else
	       {
		  /* s'il y a une regle de visibilite pour cette vue, on la prend */
		  pR = GetRuleView (&pRS, &pRD, PtVisibility, vue, pEl, pAttr,
				    pEl->ElSructSchema);
		  if (pR == NULL)
		     /* sinon, on prend celle de la vue 1 */
		     pR = pR1;
	       }
	     if (vue == VueSch)
	       {
		  /* c'est la regle de la vue traitee */
		  vis = valintregle (pR, pEl, VueNb, &ok, &unit, NULL);
		  /* si la regle de visibilite n'a pas pu etre appliquee, */
		  /* on prend la visibilite du pave de l'element createur */
		  if (!ok)
		     vis = pEl->ElAbstractBox[VueNb - 1]->AbVisibility;
	       }
	  }

	if (vis >= 1)
	   /* le pave a creer est visible dans la vue */
	  {
	     lqueue = 0;
	     pqueue = 0;
	     pAb = initpave (pEl, VueNb, vis);
	     /* pAb: pave cree */
	     PavCree = pAb;
	     pAb->AbPresentationBox = True;
	     /* c'est un pave de presentation */
	     pAb->AbCreatorAttr = pAttr;
	     /* on se souvient de l'attribut qui a cree' le pave' */
	     pAb->AbTypeNum = pRCre->PrPresBox[0];
	     pAb->AbCanBeModified = False;
	     pAb->AbPSchema = pSchP;
	     pAb->AbAcceptLineBreak =
		pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbAcceptLineBreak;
	     pAb->AbAcceptPageBreak =
		pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbAcceptPageBreak;
	     pAb->AbNotInLine =
		pSchP->PsPresentBox[pAb->AbTypeNum - 1].PbNotInLine;
	     pP1 = pEl->ElAbstractBox[VueNb - 1];
	     /* pP1: 1er pave de l'element createur */
	     /* chaine le pave cree */
	     switch (pRCre->PrPresFunction)
		   {
		      case FnCreateFirst:
			 pAb->AbSelected = False;
			 /* saute les paves de presentation deja crees */
			 /* avec la regle CreateBefore */
			 while (pP1->AbPresentationBox)
			    pP1 = pP1->AbNext;
			 pAb->AbEnclosing = pP1;
			 pAb->AbReadOnly = pP1->AbReadOnly;
			 if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
			    /* c'est le premier pave englobe' */
			    pAb->AbEnclosing->AbFirstEnclosed = pAb;
			 else
			    /* il y a deja des paves englobes */
			   {
			      pP1 = pAb->AbEnclosing->AbFirstEnclosed;
			      /* saute les paves deja crees par une regle */
			      /* FnCreateFirst et les paves des marques de page */
			      /* de debut d'element */
			      /* et verifie si le pave de presentation existe deja */
			      stop = False;
			      do
				 if ((pP1->AbPresentationBox && pP1->AbElement == pEl)
				     || (pP1->AbElement->ElTypeNumber == PageBreak + 1
					 && pP1->AbElement->ElPageType == PgBegin))
				    /* c'est un pave de presentation de l'element ou */
				    /* un saut de page de debut d'element */
				    if (pP1->AbTypeNum == pAb->AbTypeNum
					&& pP1->AbPresentationBox
					&& pP1->AbPSchema == pSchP
					&& (!pP1->AbDead))
				       /* c'est le meme que celui qu'on veut creer */
				      {
					 PavCree = NULL;
					 stop = True;
				      }
				    else if (pP1->AbNext == NULL)
				       stop = True;
				    else
				       pP1 = pP1->AbNext;
				 else
				    /* ce n'est ni pave de presentation de l'element */
				    /* ni un saut de page de debut d'element */
				    stop = True;
			      while (!stop);

			      if (PavCree != NULL)
				 if (pP1->AbElement == pEl
				     || (pP1->AbElement->ElTypeNumber == PageBreak + 1
					 && pP1->AbElement->ElPageType == PgBegin))
				    /* chaine le nouveau pave apres le dernier pave */
				    /* de presentation cree par une regle FnCreateFirst */
				    /* ou la derniere marque de page de debut d'elem. */
				   {
				      pAb->AbPrevious = pP1;
				      pAb->AbNext = pP1->AbNext;
				      pAb->AbPrevious->AbNext = pAb;
				      if (pAb->AbNext != NULL)
					 pAb->AbNext->AbPrevious = pAb;
				   }
				 else
				    /* chaine le nouveau pave avant le premier pave */
				    /* du premier fils du createur */
				   {
				      pAb->AbNext = pP1;
				      pAb->AbPrevious = pP1->AbPrevious;
				      pAb->AbNext->AbPrevious = pAb;
				      if (pAb->AbPrevious != NULL)
					 pAb->AbPrevious->AbNext = pAb;
				      if (pAb->AbEnclosing->AbFirstEnclosed == pP1)
					 pAb->AbEnclosing->AbFirstEnclosed = pAb;
				   }
			   }
			 break;
		      case FnCreateLast:
			 pAb->AbSelected = False;
			 /* saute les paves de presentation deja crees */
			 /* avec la regle CreateBefore */
			 while (pP1->AbPresentationBox)
			    pP1 = pP1->AbNext;
			 pAb->AbEnclosing = pP1;
			 pAb->AbReadOnly = pP1->AbReadOnly;
			 if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
			    pAb->AbEnclosing->AbFirstEnclosed = pAb;
			 else
			   {
			      pP1 = pAb->AbEnclosing->AbFirstEnclosed;
			      /* cherche le dernier fils du pave createur */
			      /* et verifie si le pave de presentation existe deja */
			      stop = False;
			      do
				 if (pP1->AbTypeNum == pAb->AbTypeNum
				     && pP1->AbPresentationBox == pAb->AbPresentationBox
				     && pP1->AbPSchema == pSchP
				     && (!pP1->AbDead))
				    /* ce pave de presentation existe deja */
				   {
				      PavCree = NULL;
				      stop = True;
				   }
				 else if (pP1->AbNext == NULL)
				    stop = True;
				 else
				    pP1 = pP1->AbNext;
			      while (!stop);
			      /* chaine le nouveau pave apres le dernier pave */
			      /* fils du pave createur */
			      if (PavCree != NULL)
				{
				   pAb->AbPrevious = pP1;
				   pAb->AbNext = pP1->AbNext;
				   pAb->AbPrevious->AbNext = pAb;
				}
			   }
			 break;
		      case FnCreateBefore:
			 /* saute les paves de presentation deja crees */
			 /* avec la regle CreateBefore */
			 while (pP1->AbPresentationBox)
			   {
			      if ((pP1->AbTypeNum == pAb->AbTypeNum)
				  && (pP1->AbPresentationBox == pAb->AbPresentationBox)
				  && (pP1->AbPSchema == pSchP)
				  && (!pP1->AbDead))
				 /* ce pave de presentation existe deja */
				 PavCree = NULL;
			      pP1 = pP1->AbNext;
			   }
			 if (PavCree != NULL)
			   {
			      pAb->AbReadOnly = pP1->AbReadOnly;
			      pAb->AbEnclosing = pP1->AbEnclosing;
			      if (pAb->AbEnclosing->AbFirstEnclosed == pP1)
				 pAb->AbEnclosing->AbFirstEnclosed = pAb;
			      pAb->AbNext = pP1;
			      pAb->AbPrevious = pP1->AbPrevious;
			      pP1->AbPrevious = pAb;
			      if (pAb->AbPrevious != NULL)
				 pAb->AbPrevious->AbNext = pAb;
			      if (pAb->AbPrevious == NULL)
				 pEl->ElAbstractBox[VueNb - 1] = pAb;
			      else if (pAb->AbPrevious->AbElement != pEl)
				 pEl->ElAbstractBox[VueNb - 1] = pAb;
			   }
			 break;
		      case FnCreateAfter:
		      case FnCreateWith:
			 stop = False;
			 do
			    if (pP1->AbNext == NULL)
			       stop = True;
			    else if (pP1->AbNext->AbElement != pEl)
			       stop = True;
			    else
			      {
				 if (pP1->AbNext->AbPresentationBox)
				    if (pP1->AbNext->AbTypeNum == pAb->AbTypeNum
					&& pP1->AbNext->AbPresentationBox == pAb->AbPresentationBox
				     && pP1->AbNext->AbPSchema == pSchP
					&& (!pP1->AbNext->AbDead))
				       /* ce pave de presentation existe deja */
				       PavCree = NULL;
				 pP1 = pP1->AbNext;
			      }
			 while (!(stop));
			 if (PavCree != NULL)
			   {
			      pAb->AbReadOnly = pP1->AbReadOnly;
			      pAb->AbEnclosing = pP1->AbEnclosing;
			      pAb->AbNext = pP1->AbNext;
			      pAb->AbPrevious = pP1;
			      pP1->AbNext = pAb;
			      if (pAb->AbNext != NULL)
				 pAb->AbNext->AbPrevious = pAb;
			   }
			 break;
		      case FnCreateEnclosing:
			 if (pP1->AbEnclosing != NULL)
			    if (pP1->AbEnclosing->AbPresentationBox)
			       if (pP1->AbEnclosing->AbElement == pEl)
				  /* l'element a deja un pave de presentation englobant. */
				  /* on refuse d'en creer un autre */
				  PavCree = NULL;
			 if (PavCree != NULL)
			   {
			      pAb->AbReadOnly = pP1->AbReadOnly;
			      pAb->AbEnclosing = pP1->AbEnclosing;
			      if (pAb->AbEnclosing->AbFirstEnclosed == pP1)
				 pAb->AbEnclosing->AbFirstEnclosed = pAb;
			      pAb->AbPrevious = pP1->AbPrevious;
			      pP1->AbPrevious = NULL;
			      if (pAb->AbPrevious != NULL)
				 pAb->AbPrevious->AbNext = pAb;
			      pAb->AbFirstEnclosed = pP1;
			      /* traite les paves de presentation deja crees par les */
			      /* regles CreateBefore et CreateAfter */
			      stop = False;
			      while (!stop)
				{
				   pP1->AbEnclosing = pAb;
				   if (pP1->AbNext == NULL)
				      stop = True;
				   else if (pP1->AbNext->AbElement != pEl)
				      stop = True;
				   else
				      pP1 = pP1->AbNext;
				}
			      /* traite le dernier pave' de l'element */
			      pAb->AbNext = pP1->AbNext;
			      pP1->AbNext = NULL;
			      if (pAb->AbNext != NULL)
				 pAb->AbNext->AbPrevious = pAb;
			   }
			 break;
		      default:
			 break;
		   }

	     if (PavCree == NULL)	/* pave deja cree' */
	       {
		  if (pAb->AbBox != NULL)
		    {
		       /* libere le pave */
		       if (pAb->AbLeafType == LtPicture)
			 {
			    if (!pAb->AbElement->ElTerminal ||
				pAb->AbElement->ElLeafType != LtPicture)
			       /* ce n'est pas un element image */
			       FreeImageDescriptor (pAb->AbPictInfo);
			    pAb->AbPictInfo = NULL;
			 }
		       FreePave (pAb);
		    }
		  else if (pAb)
		     FreePave (pAb);
	       }
	     else
		/* rend non modifiable le pave de presentation */
	       {
		  pAb->AbCanBeModified = False;
		  pAb->AbLeafType = LtCompound;
		  pAb->AbVolume = 0;
		  pAb->AbInLine = False;
		  pAb->AbTruncatedHead = False;
		  pAb->AbTruncatedTail = False;
	       }
	     if (PavCree != NULL)
	       {
		  if (pER != NULL)
		     /* change le pointeur de pave de l'element englobant les */
		     /* elements associes a mettre dans la boite */
		    {
		       pER->ElParent->ElAbstractBox[VueNb - 1] = PavCree;
		       PavCree->AbPresentationBox = False;
		    }
		  /* applique les regles de presentation de la boite creee. */
		  do
		    {
		       pR = GetRule (&pRS, &pRD, pEl, pAttr, pEl->ElSructSchema);
		       /* pointeur sur la regle a  appliquer pour la vue 1 */
		       if (pR != NULL)
			  /* cherche et applique les regles de tous les types */
			  /* pour la vue */
			  for (vue = 1; vue <= MAX_VIEW; vue++)
			    {
			       if (vue == 1)
				  pRV = NULL;
			       else
				  pRV = GetRuleView (&pRS, &pRD, pR->PrType, vue, pEl,
						   pAttr, pEl->ElSructSchema);
			       if (vue == VueSch)
				  /* applique la regle au pave cree'. */
				 {
				    if (pRV == NULL)
				       pRV = pR;
				    if (pRV->PrType == PtFunction
					&& (pRV->PrPresFunction == FnCreateFirst ||
					    pRV->PrPresFunction == FnCreateLast))
				       /* le pave cree' cree un pave de presentation */
				      {
					 pBox = &pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1];
					 if (pBox->PbContent == ContElement
					     && (pBox->PbPageFooter || pBox->PbPageHeader))
					    /* une boite de bas de page qui affiche des */
					    /* elements associes, on ne creera la boite */
					    /* a creer que lorsque les elements */
					    /* associes  auront ete crees. */
					   {
					      if (lqueue < MAX_QUEUE_LEN)
						{
						   lqueue++;
						   queuePR[lqueue - 1] = pRV;
						}
					   }
					 else
					    /* sauve le pointeur de pave de l'element */
					   {
					      pPavSv = pEl->ElAbstractBox[VueNb - 1];
					      /* change le pointeur de pave de l'element */
					      /* pour un chainage correct du pave a creer */
					      pEl->ElAbstractBox[VueNb - 1] = PavCree;
					      PavCree->AbPresentationBox = False;
					      /* cree le pave de presentation */
					      pP1 = CrPavPres (pEl, pDoc, pRV, pSS, NULL,
						 VueNb, pSchP, False, True);
					      /* restaure le pointeur de pave de l'elem */
					      pEl->ElAbstractBox[VueNb - 1] = pPavSv;
					   }
				      }
				    else if (!CreateurComplet && pRV->PrPresMode == PresInherit
					 && pRV->PrInheritMode == InheritCreator)
				       /* toutes les regles de presentation n'ont */
				       /* pas encore ete appliquees au pave */
				       /* et le pave cree herite du createur, on */
				       /* differe l'application de la regle */
				       Retarde (pRV, pSchP, PavCree, NULL, PavCree);
				    else if (!Applique (pRV, pSchP, PavCree, pDoc, NULL))
				       /* on n'a pas pu appliquer la regle, on */
				       /* l'appliquera lorsque le pave pere */
				       /* sera  termine' */
				       Retarde (pRV, pSchP, PavCree, NULL, PavCree);
				 }
			    }
		    }
		  while (!(pR == NULL));
		  PavCree->AbPresentationBox = True;
		  /* met le contenu dans le pave cree */
		  pBox = &pSchP->PsPresentBox[pRCre->PrPresBox[0] - 1];
		  switch (pBox->PbContent)
			{
			   case FreeContent:
			      break;
			   case ContVariable:
			      ok = NouvVariable (pBox->PbContVariable, pSS, pSchP, pAb, pDoc);
			      break;
			   case ContConst:
			      CopieConstante (pBox->PbContConstant, pSchP, pAb);
			      break;
			   case ContElement:
			      /* une boite qui regroupe des elements associes */
			      /* rend modifiable le pave de presentation */
			      PavCree->AbCanBeModified = True;
			      /* cherche les references a ces elements jusqu'a la */
			      /* marque de page precedente qui concerne cette vue, et */
			      /* cree les paves des elements reference's. */
			      stop = False;
			      pE = pEl;
			      pP1 = NULL;
			      do
				{
				   pER = ElRefSuiv (&pE, pBox->PbContRefElem, pEl->ElSructSchema,
						    pBox->PbPageFooter, &VueSch);
				   if (pER == NULL)
				      /* il n'y a plus de reference dans la page */
				      stop = True;
				   else
				      /* on a trouve' une reference */
				   if (!DansTampon (pER))
				      /* on ne traite pas l'element reference' s'il fait */
				      /* partie des elements qui ont ete coupe's */
				      if (pER->ElAbstractBox[VueNb - 1] == NULL)
					 /* les paves de l'element reference' n'ont pas */
					 /* encore ete cree's. */
					 /* Volume libre infini pour que tout le contenu */
					 /* de ces elem. soit cree' dans l'image abstraite */
					{
					   if (VueAssoc (pER))
					     {
						volume = pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1];
						pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1] = THOT_MAXINT;
					     }
					   else
					     {
						volume = pDoc->DocViewFreeVolume[VueNb - 1];
						pDoc->DocViewFreeVolume[VueNb - 1] = THOT_MAXINT;
					     }
					   /* cree les paves de l'element reference' */
					   pP1 = CreePaves (pER, pDoc, VueNb, True, True,
							    &complet);
					   /* verifie les elements associes voisins */
					   pVoisin = pER;
					   fin = False;
					   do
					     {
						if (pBox->PbPageFooter)
						   pVoisin = pVoisin->ElNext;
						else
						   pVoisin = pVoisin->ElPrevious;
						if (pVoisin == NULL)
						   fin = True;
						else
						  {
						     if (pVoisin->ElReferredDescr == NULL)

							/* l'element voisin n'est pas reference', on */
							/* va creer ses paves */
							ok = True;
						     else if (pVoisin->ElReferredDescr->ReFirstReference == NULL)
							ok = True;
						     else if (pVoisin->ElReferredDescr->ReFirstReference->RdElement == NULL)
							ok = True;
						     else if (pVoisin->ElReferredDescr->ReFirstReference->RdElement->
							      ElAssocNum == pVoisin->ElAssocNum)
							/* l'element voisin est reference' depuis */
							/* un element associe' de meme */

							/* type; on cree ses paves */
							ok = True;
						     else
							/* inutile de creer les paves du voisin, il seront */
							/* crees lorsqu'on rencontrera sa 1ere reference */
							ok = False;
						     if (ok)
							/* cree les paves du voisin */
							pP1 = CreePaves (pVoisin, pDoc, VueNb, True, True, &complet);
						     else
							/* on arrete de traiter les voisins */
							fin = True;
						  }
					     }
					   while (!(fin));
					   /* retablit le volume libre reel */
					   if (VueAssoc (pER))
					      pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1]
						 = pDoc->DocAssocFreeVolume[pER->ElAssocNum - 1] - THOT_MAXINT + volume;
					   else
					      pDoc->DocViewFreeVolume[VueNb - 1] =
						 pDoc->DocViewFreeVolume[VueNb - 1] - THOT_MAXINT + volume;
					}
				}
			      while (!(stop));
			      /* le volume des paves englobants prend deja en compte le */
			      /* volume de la boite cree */
			      volok = True;
			      break;
			}
		  do
		     if (pqueue >= lqueue)
			pR = NULL;
		     else
		       {
			  pqueue++;
			  pR = queuePR[pqueue - 1];
			  /* sauve le pointeur de pave de l'element */
			  pPavSv = pEl->ElAbstractBox[VueNb - 1];
			  /* change le pointeur de pave de l'element, pour un */
			  /* chainage correct du pave a creer */
			  pEl->ElAbstractBox[VueNb - 1] = PavCree;
			  PavCree->AbPresentationBox = False;
			  /* cree le pave de presentation */
			  pP1 = CrPavPres (pEl, pDoc, pR, pSS, NULL, VueNb, pSchP,
					   True, True);
			  /* restaure le pointeur de pave de l'element */
			  pEl->ElAbstractBox[VueNb - 1] = pPavSv;
		       }
		  while (!(pR == NULL));
		  do		/* applique les regles retardees */
		    {
		       pP1 = PavCree;
		       GetRet (&pR, &pSP, &pP1, &pA);
		       if (pR != NULL)
			  if (!Applique (pR, pSP, pP1, pDoc, pA))
			     Retarde (pR, pSP, pP1, pA, PavCree);
		    }
		  while (!(pR == NULL));
		  /* retablit AbPresentationBox qui a ete modifie' pour les boites de */
		  /* haut ou de bas de page qui regroupent des elements associes */
		  PavCree->AbPresentationBox = True;
		  /* ajoute le volume du pave cree' a celui de tous ses */
		  /* englobants */
		  if (PavCree->AbVolume > 0 && !volok)
		    {
		       pP1 = PavCree->AbEnclosing;
		       while (pP1 != NULL)
			 {
			    pP1->AbVolume += PavCree->AbVolume;
			    pP1 = pP1->AbEnclosing;
			 }
		    }
		  /* met a jour le volume libre restant dans la vue */
		  MajVolLibre (PavCree, pDoc);
		  if (pEl->ElTypeNumber == PageBreak + 1)
		     /* c'est une boite de haut ou bas de page. Sa creation */
		     /* affecte peut-etre les autres boites de haut ou bas de page */
		     /* deja creees. */
		     if (DansBoiteAssoc && !PavCree->AbEnclosing->AbPresentationBox)
		       {
			  PavCree->AbEnclosing->AbPresentationBox = True;
			  NouvRfPave (PavCree, PavCree, &pP1, pDoc);
			  PavCree->AbEnclosing->AbPresentationBox = False;
		       }
		     else
			NouvRfPave (PavCree, PavCree, &pP1, pDoc);

		  /* si c'est une boite contenant une image, choisit le mode */
		  /* de presentation de l'image en accord avec les regle de */
		  /* dimensions du pave */
		  if (PavCree->AbLeafType == LtPicture)
		    {
		       /* a priori l'image prendra les dimensions de son pave' */
		       ((PictInfo *) (PavCree->AbPictInfo))->PicPresent =
			  ReScale;
		       if (!PavCree->AbWidth.DimIsPosition)
			  if (PavCree->AbWidth.DimValue == 0)
			     if (PavCree->AbWidth.DimAbRef == NULL)
				if (!PavCree->AbHeight.DimIsPosition)
				   if (PavCree->AbHeight.DimValue == 0)
				      if (PavCree->AbHeight.DimAbRef == NULL)
					 /* le pave prend la hauteur et la largeur de son
					    contenu */
					 /* l'image doit etre affichee telle quelle */
					 ((PictInfo *) (PavCree->AbPictInfo))->PicPresent = RealSize;
		    }
	       }
	  }
     }

   return PavCree;
}
#endif /* __COLPAGE__ */


/* ---------------------------------------------------------------------- */
/* |    ReglePresAttr retourne la premiere regle de la chaine des regles| */
/* |            de presentation a` appliquer pour l'attribut pAttr.     | */
/* |            - si heritage = true on ne s'interesse pas aux regles   | */
/* |            par defaut (pRPdef).                                    | */
/* |            - si heritage = false on ne s'interesse pas aux regles  | */
/* |            d'heritage (pRPherit).                                  | */
/* |            - si PAttrComp != NULL alors si l'attribut est numerique| */
/* |            et que ses regles sont des comparaisons on utilisera    | */
/* |            pAttrComp au lieu de rechercher l'attribut de           | */
/* |            comparaison dans les ascendants de pEl.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrPRule        ReglePresAttr (PtrAttribute pAttr, PtrElement pEl, boolean heritage,
				   PtrAttribute pAttrComp, PtrPSchema pSchP)

#else  /* __STDC__ */
PtrPRule        ReglePresAttr (pAttr, pEl, heritage, pAttrComp, pSchP)
PtrAttribute         pAttr;
PtrElement          pEl;
boolean             heritage;
PtrAttribute         pAttrComp;
PtrPSchema          pSchP;

#endif /* __STDC__ */

{
   int                 i;
   boolean             trouve;
   PtrPRule        pRegle;
   PtrAttribute         pAt2;
   PtrElement          pElAttr;
   AttributePres      *pAPRule, *pPRdef, *pPRinherit, *pPRclass;
   NumAttrCase         *pCase;

   pRegle = NULL;
   if (pSchP == NULL)
      return (NULL);
   pAPRule = pSchP->PsAttrPRule[pAttr->AeAttrNum - 1];

   /* on cherche quel est le paquet de regles qui s'applique */
   /* pPRdef designera le paquet de regles s'appliquant a tous les elements */
   /* c'est a dire celui pour lequel pAPRule->ApElemType = 0  */
   /* pPRinherit  designera le paquet pour lequel pAPRule->ApElemType = pEl->ElTypeNumber */
   /* pPRclass  designera le paquet pour lequel pAPRule->ApString et      */
   /* pAttr->AeAttrText sont des chaines identiques         */

   pPRclass = pPRdef = pPRinherit = NULL;
   for (i = pSchP->PsNAttrPRule[pAttr->AeAttrNum - 1]; i-- > 0;
	pAPRule = pAPRule->ApNextAttrPres)
     {
	if (pAPRule->ApElemType == 0)
	   pPRdef = pAPRule;
	else if (pAPRule->ApElemType == pEl->ElTypeNumber)
	   pPRinherit = pAPRule;
	if ((pAttr->AeAttrType == AtTextAttr) &&
	    (pAttr->AeAttrText != NULL) &&
	    (StringAndTextEqual (pAPRule->ApString, pAttr->AeAttrText)))
	   pPRclass = pAPRule;
     }

   if (heritage)
     {
	if (pPRinherit != NULL)
	   pAPRule = pPRinherit;
	else
	   /* ce n'est pas la peine de continuer */
	   return (NULL);
     }
   else
     {
	if (pPRdef != NULL)
	   pAPRule = pPRdef;
	else
	   /* ce n'est pas la peine de continuer */
	   return (NULL);
     }

   /* selon le type de l'attribut on cherche le debut de la chaine  */
   /* de regles de presentation */
   switch (pAttr->AeAttrType)
	 {
	    case AtNumAttr:
	       i = 1;
	       trouve = False;
	       while (i <= pAPRule->ApNCases && !trouve)
		 {
		    pCase = &pAPRule->ApCase[i - 1];
		    if (pCase->CaComparType == ComparConstant)
		      {
			 /* la valeur de comparaison est une cste */
			 if (pAttr->AeAttrValue >= pCase->CaLowerBound &&
			     pAttr->AeAttrValue <= pCase->CaUpperBound)
			   {
			      trouve = True;
			      pRegle = pCase->CaFirstPRule;
			   }
		      }
		    else
		      {
			 /* la valeur de comparaison est un attribut */
			 if ((pCase->CaLowerBound != -1) && (pCase->CaUpperBound != -1))
			   {
			      /* attr = enclosing */
			      if (pAttrComp != NULL)
				 pAt2 = pAttrComp;
			      else
				 pAt2 = GetTypedAttrAncestor (pEl, pCase->CaLowerBound,
					     pAttr->AeAttrSSchema, &pElAttr);
			      if (pAt2 != NULL)
				 if (pAttr->AeAttrValue == pAt2->AeAttrValue)
				   {
				      trouve = True;
				      pRegle = pCase->CaFirstPRule;
				   }
			   }
			 else if (pCase->CaLowerBound == -1)
			   {
			      /* attr < enclosing */
			      if (pAttrComp != NULL)
				 pAt2 = pAttrComp;
			      else
				 pAt2 = GetTypedAttrAncestor (pEl, pCase->CaUpperBound,
					     pAttr->AeAttrSSchema, &pElAttr);
			      if (pAt2 != NULL)
				 if (pAttr->AeAttrValue < pAt2->AeAttrValue)
				   {
				      trouve = True;
				      pRegle = pCase->CaFirstPRule;
				   }
			   }
			 else
			   {
			      /* attr > enclosing */
			      if (pAttrComp != NULL)
				 pAt2 = pAttrComp;
			      else
				 pAt2 = GetTypedAttrAncestor (pEl, pCase->CaLowerBound,
					     pAttr->AeAttrSSchema, &pElAttr);
			      if (pAt2 != NULL)
				 if (pAttr->AeAttrValue > pAt2->AeAttrValue)
				   {
				      trouve = True;
				      pRegle = pCase->CaFirstPRule;
				   }
			   }
		      }
		    i++;
		 }
	       break;
	    case AtTextAttr:
	       if (pPRclass)
		  pRegle = pPRclass->ApTextFirstPRule;
	       else if (pAPRule->ApString[0] == '\0')
		  pRegle = pAPRule->ApTextFirstPRule;
	       break;
	    case AtReferenceAttr:
	       pRegle = pAPRule->ApRefFirstPRule;
	       break;
	    case AtEnumAttr:
	       /* on verifie que la valeur est correcte */
	       if (pAttr->AeAttrValue < 0 || pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrNEnumValues < pAttr->AeAttrValue)
		  /* valeur incorrecte, on prend les regles qui s'appliquent a */
		  /* n'importe quelle valeur */
		  pRegle = pAPRule->ApEnumFirstPRule[0];
	       else if (pAPRule->ApEnumFirstPRule[pAttr->AeAttrValue] == NULL)
		  /* pas de regles pour cette valeur, on prend les regles */
		  /* qui s'appliquent a n'importe quelle valeur */
		  pRegle = pAPRule->ApEnumFirstPRule[0];
	       else
		  /* on prend les regles qui s'appliquent a cette valeur */
		  pRegle = pAPRule->ApEnumFirstPRule[pAttr->AeAttrValue];
	       break;
	    default:
	       pRegle = NULL;
	       break;
	 }

   return pRegle;
}


/* ---------------------------------------------------------------------- */
/* |     TraiteCreation determine les regles de creation a appliquer    | */
/* |            au pave pAb en fonction de Tete                        | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         TraiteCreation (PtrSSchema pSS, PtrPSchema pSP, PtrAbstractBox * PavCree,
			  PtrAttribute pAttr, PtrDocument pDoc, PtrAbstractBox pAb,
				    boolean Tete, PtrPRule pRegle)

#else  /* __STDC__ */
static void         TraiteCreation (pSS, pSP, PavCree, pAttr, pDoc, pAb, Tete, pRegle)
PtrSSchema        pSS;
PtrPSchema          pSP;
PtrAbstractBox            *PavCree;
PtrAttribute         pAttr;
PtrDocument         pDoc;
PtrAbstractBox             pAb;
boolean             Tete;
PtrPRule        pRegle;

#endif /* __STDC__ */

{
   PtrAbstractBox             pP, PavR;
   boolean             stop;

   /* saute les regles precedant les fonctions */
   stop = False;
   do
      if (pRegle == NULL)
	 stop = True;
      else if (pRegle->PrType > PtFunction)
	{
	   stop = True;
	   pRegle = NULL;
	   /* pas de fonction de presentation */
	}
      else if (pRegle->PrType == PtFunction)
	 stop = True;
      else
	 pRegle = pRegle->PrNextPRule;
   while (!(stop));
   /* cherche toutes les fonctions de creation */
   stop = False;
   do
      if (pRegle == NULL)
	 stop = True;
      else if (pRegle->PrType != PtFunction)
	 stop = True;
      else
	{
	   /* applique les fonctions de creation qui correspondent a */
	   /* l'extremite concernee */
	   /* si la regle de creation possede l'indication de repetition */
	   /* on appelle la procedure de creation systematiquement */
#ifdef __COLPAGE__
	   if ((Tete
		&& (pRegle->PrPresFunction == FnCreateBefore
		    || pRegle->PrPresFunction == FnCreateFirst))
	       || (!Tete
		   && (pRegle->PrPresFunction == FnCreateAfter
		       || pRegle->PrPresFunction == FnCreateLast))
	       || (pRegle->PrPresBoxRepeat
		   && (pRegle->PrPresFunction == FnCreateBefore
		       || pRegle->PrPresFunction == FnCreateFirst
		       || pRegle->PrPresFunction == FnCreateAfter
		       || pRegle->PrPresFunction == FnCreateLast)))
#else  /* __COLPAGE__ */
	   if ((Tete
		&& (pRegle->PrPresFunction == FnCreateBefore
		    || pRegle->PrPresFunction == FnCreateFirst))
	       || (!Tete
		   && (pRegle->PrPresFunction == FnCreateAfter
		       || pRegle->PrPresFunction == FnCreateLast)))
#endif /* __COLPAGE__ */
	     {
		pP = CrPavPres (pAb->AbElement, pDoc, pRegle,
				pSS, pAttr, pAb->AbDocView, pSP, False, True);
		if (pP != NULL)
		   /* TODO : valeur de pP si plusieurs paves crees avec Rep ?? */
		  {
		     if (!Tete)
			*PavCree = pP;
		     else if (*PavCree == NULL)
			*PavCree = pP;
		     /* modifie les paves environnant */
		     /* qui dependent du pave cree */
		     /* TODO : si Rep est vrai, plusieurs paves ont ete crees */
		     /* faut-il appeler NouvRfPave dans CrPavPres ?? */
		     NouvRfPave (*PavCree, *PavCree, &PavR, pDoc);
		     /* passe a la regle suivante */
		  }
	     }
	   pRegle = pRegle->PrNextPRule;
	}
   while (!(stop));
}


/* ---------------------------------------------------------------------- */
/* |    PaveCoupe coupe ou complete le pave pAb. pAb est coupe' si    | */
/* |            Coupe est vrai ou complet si Coupe est faux. La coupure | */
/* |            ou la completion concerne la tete du pave si Tete est   | */
/* |            vrai ou la queue du pave si Tete est faux. Marque cette | */
/* |            information dans le pave. S'il s'agit d'un pave qui     | */
/* |            devient complet, cree ses paves de presentation a`      | */
/* |            l'extremite qui devient complete. Retourne un pointeur  | */
/* |            sur le dernier pave de presentation cree ou NULL si     | */
/* |            aucun pave n'est cree.                                  | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrAbstractBox             PaveCoupe (PtrAbstractBox pAb, boolean Coupe, boolean Tete, PtrDocument pDoc)

#else  /* __STDC__ */
PtrAbstractBox             PaveCoupe (pAb, Coupe, Tete, pDoc)
PtrAbstractBox             pAb;
boolean             Coupe;
boolean             Tete;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPRule        pRegle;
   PtrPSchema          pSchP;
   int                 Entree;
   PtrSSchema        pSchS;
   PtrAbstractBox             PavCree;
   PtrAttribute         pAttr;
   PtrElement          pElAttr;
   int                 l;
   InheritAttrTable     *tableherites;
   PtrHandlePSchema    pHd;

   PavCree = NULL;
   if (pAb != NULL)
#ifdef __COLPAGE__
      if (!(pAb->AbElement->ElTerminal &&
	    pAb->AbElement->ElLeafType == LtPageColBreak))
	{
#endif /* __COLPAGE__ */
	   if (pAb->AbLeafType == LtCompound)
	      if (pAb->AbInLine)
		{
		   if (!Coupe)
		      if (Tete)
			 pAb->AbTruncatedHead = False;
		      else
			 pAb->AbTruncatedTail = False;
		}
	      else if ((Tete && pAb->AbTruncatedHead != Coupe)
		       || (!Tete && pAb->AbTruncatedTail != Coupe))
		 /* il y a effectivement changement */
		{
		   if (!Coupe)
		     {
			/* le pave n'est plus coupe' a` une extremite. Cree les */
			/* paves de presentation a` cette extremite. */
			/* cherche la 1ere regle de presentation associee a ce type */
			/* d'element */
			ChSchemaPres (pAb->AbElement, &pSchP, &Entree, &pSchS);
			pRegle = pSchP->PsElemPRule[Entree - 1];

			/* traite les regles de creation associees au type de l'element */
			pAttr = NULL;
			TraiteCreation (pSchS, pSchP, &PavCree, NULL, pDoc,
					pAb, Tete, pRegle);
			/* traite les regles de creation dues a */
			/* l'heritage des attributs */
			if (pAb->AbElement->ElSructSchema->SsPSchema != NULL)
			   if (pAb->AbElement->ElSructSchema->SsPSchema->
			       PsNInheritedAttrs[pAb->AbElement->ElTypeNumber - 1])
			     {
				/* il y a heritage possible */
				if ((tableherites = pAb->AbElement->ElSructSchema->SsPSchema->
				     PsInheritedAttr[pAb->AbElement->ElTypeNumber - 1])
				    == NULL)
				  {
				     /* cette table n'existe pas on la genere */
				     CreateInheritedAttrTable (pAb->AbElement);
				     tableherites = pAb->AbElement->ElSructSchema->
					SsPSchema->PsInheritedAttr[pAb->AbElement->ElTypeNumber - 1];
				  }
				for (l = 1; l <= pAb->AbElement->ElSructSchema->SsNAttributes; l++)
				   if ((*tableherites)[l - 1])
				      /* pAb->AbElement herite de l'attribut l */
				      if ((pAttr = GetTypedAttrAncestor (pAb->AbElement, l,
								       pAb->AbElement->ElSructSchema, &pElAttr)) != NULL)
					 /* cherche si l existe au dessus */
					{
					   /* on cherchera d'abord dans le schema de */
					   /* presentation principal de l'attribut */
					   pSchP = pAttr->AeAttrSSchema->SsPSchema;
					   pHd = NULL;
					   while (pSchP != NULL)
					     {
						pRegle = ReglePresAttr (pAttr, pAb->AbElement,
							 True, NULL, pSchP);
						TraiteCreation (pAttr->AeAttrSSchema, pSchP,
						&PavCree, pAttr, pDoc, pAb,
							      Tete, pRegle);
						if (pHd == NULL)
						  {
						     /* on n'a pas encore cherche' dans les schemas
						        de presentation additionnels. On prend le
						        premier schema additionnel si on travaille
						        pour la vue principale, sinon on ignore les
						        schemas additionnels */
						     if (pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView == 1)
							pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
						  }
						else
						   /* passe au schema additionnel suivant */
						   pHd = pHd->HdNextPSchema;
						if (pHd == NULL)
						   /* il n'y a pas (ou plus) de schemas
						      additionnels a prendre en compte */
						   pSchP = NULL;
						else
						   pSchP = pHd->HdPSchema;
					     }
					}
			     }
			/* traite les regles de creation associees aux attributs de */
			/* l'element */
			pAttr = pAb->AbElement->ElFirstAttr;
			/* 1er attribut de l'element */
			/* boucle sur les attributs de l'element */
			while (pAttr != NULL)
			  {
			     /* cherche le debut des regles de presentation a */
			     /* appliquer pour l'attribut */
			     /* on cherchera d'abord dans le schema de */
			     /* presentation principal de l'attribut */
			     pSchP = pAttr->AeAttrSSchema->SsPSchema;
			     pHd = NULL;
			     while (pSchP != NULL)
			       {
				  pRegle = ReglePresAttr (pAttr, pAb->AbElement, False,
							  NULL, pSchP);
				  TraiteCreation (pAttr->AeAttrSSchema, pSchP, &PavCree, pAttr,
						  pDoc, pAb, Tete, pRegle);
				  if (pHd == NULL)
				    {
				       /* on n'a pas encore cherche' dans les schemas de
				          presentation additionnels. On prend le premier schema
				          additionnel si on travaille pour la vue principale,
				          sinon on ignore les schemas additionnels. */
				       if (pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView == 1)
					  pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
				    }
				  else
				     /* passe au schema additionnel suivant */
				     pHd = pHd->HdNextPSchema;
				  if (pHd == NULL)
				     /* il n'y a pas (ou plus) de schemas additionnels a
				        prendre en compte */
				     pSchP = NULL;
				  else
				     pSchP = pHd->HdPSchema;
			       }
			     /* passe a l'attribut suivant de l'element */
			     pAttr = pAttr->AeNext;
			  }
		     }
		   if (Tete)
		      pAb->AbTruncatedHead = Coupe;
		   else
		      pAb->AbTruncatedTail = Coupe;
		}
#ifdef __COLPAGE__
	}
#endif /* __COLPAGE__ */

   return PavCree;
}



/* ---------------------------------------------------------------------- */
/* |    VuePleine retourne vrai si la vue VueNb est pleine.             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             VuePleine (DocViewNumber VueNb, PtrDocument pDoc, PtrElement pEl)

#else  /* __STDC__ */
boolean             VuePleine (VueNb, pDoc, pEl)
DocViewNumber           VueNb;
PtrDocument         pDoc;
PtrElement          pEl;

#endif /* __STDC__ */

{
   boolean             pleine;

   if (!VueAssoc (pEl))
      /* ce n'est pas une vue d'elements associes */
      pleine = (pDoc->DocView[VueNb - 1].DvPSchemaView == 0
		|| pDoc->DocViewFreeVolume[VueNb - 1] <= 0);
   else
      /* c'est une vue d'elements associes */
      pleine = (pDoc->DocAssocFrame[pEl->ElAssocNum - 1] == 0
		|| pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] < 0);
   return pleine;
}


/* ---------------------------------------------------------------------- */
/* |    Attente met en attente la regle pR appartenant au schema de     | */
/* |            presentation pSP. Cette regle sera reappliquee au pave  | */
/* |            pP lorsque la descendance de ce pave aura ete creee.    | */
/* |            pA est l'attribut auquel correspond la regle (NULL si   | */
/* |            ce n'est pas une regle d'attribut).                     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                Attente (PtrPRule pR, PtrAbstractBox pP, PtrPSchema pSP, PtrAttribute pA,
	    PtrAttribute queuePA[MAX_QUEUE_LEN], PtrPSchema queuePS[MAX_QUEUE_LEN],
	      PtrAbstractBox queuePP[MAX_QUEUE_LEN], PtrPRule queuePR[MAX_QUEUE_LEN],
			     int *lqueue)

#else  /* __STDC__ */
void                Attente (pR, pP, pSP, pA, queuePA, queuePS, queuePP, queuePR, lqueue)
PtrPRule        pR;
PtrAbstractBox             pP;
PtrPSchema          pSP;
PtrAttribute         pA;
PtrAttribute         queuePA[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAbstractBox             queuePP[MAX_QUEUE_LEN];
PtrPRule        queuePR[MAX_QUEUE_LEN];
int                *lqueue;

#endif /* __STDC__ */

{
   if (*lqueue >= MAX_QUEUE_LEN)
      printf ("Increase LgMaxQueue (crimabs.c)\n");
   else
     {
	(*lqueue)++;
	queuePR[*lqueue - 1] = pR;
	queuePP[*lqueue - 1] = pP;
	queuePS[*lqueue - 1] = pSP;
	queuePA[*lqueue - 1] = pA;
     }
}


/* ---------------------------------------------------------------------- */
/* |    GetAtt recupere une regle de presentation qui etait en attente. | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         GetAtt (PtrPRule * pR, PtrAbstractBox * pP, PtrPSchema * pSP,
			  PtrAttribute * pA, PtrAttribute queuePA[MAX_QUEUE_LEN],
		PtrPSchema queuePS[MAX_QUEUE_LEN], PtrAbstractBox queuePP[MAX_QUEUE_LEN],
		 PtrPRule queuePR[MAX_QUEUE_LEN], int *lqueue, int *pqueue)

#else  /* __STDC__ */
static void         GetAtt (pR, pP, pSP, pA, queuePA, queuePS, queuePP, queuePR, lqueue,
			    pqueue)
PtrPRule       *pR;
PtrAbstractBox            *pP;
PtrPSchema         *pSP;
PtrAttribute        *pA;
PtrAttribute         queuePA[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAbstractBox             queuePP[MAX_QUEUE_LEN];
PtrPRule        queuePR[MAX_QUEUE_LEN];
int                *lqueue;
int                *pqueue;

#endif /* __STDC__ */

{
   *pR = NULL;
   while (*pR == NULL && *pqueue < *lqueue)
     {
	(*pqueue)++;
	*pR = queuePR[*pqueue - 1];
	*pP = queuePP[*pqueue - 1];
	*pSP = queuePS[*pqueue - 1];
	*pA = queuePA[*pqueue - 1];
     }
}


/* ---------------------------------------------------------------------- */
/* |    ApplRegleCreation verifie que la regle pRegleC appartenant au   | */
/* |            schema de presentation pSchPres (correspondant au       | */
/* |            schema de structure pSS), est une regle de creation et, | */
/* |            si oui, tente de l'appliquer a` l'element pEl. La       | */
/* |            fonction retourne Vrai s'il s'agit bien d'une regle de  | */
/* |            creation. pA est l'attribut auquel correspond la regle, | */
/* |            s'il s'agit d'une regle de presentation d'attribut (NULL| */
/* |            sinon).                                                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      ApplRegleCreation (PtrPRule pRegleC, PtrSSchema pSS,
				       PtrPSchema pSchPres, PtrAttribute pA,
				       PtrAbstractBox * Retour, DocViewNumber VueNb,
				       PtrDocument pDoc, PtrElement pEl,
				       boolean EnAvant, int *lqueue,
				       PtrPRule queuePR[MAX_QUEUE_LEN],
				       PtrAbstractBox queuePP[MAX_QUEUE_LEN],
				       PtrPSchema queuePS[MAX_QUEUE_LEN],
				       PtrAttribute queuePA[MAX_QUEUE_LEN],
				       PtrAbstractBox NouvPave)

#else  /* __STDC__ */
static boolean      ApplRegleCreation (pRegleC, pSS, pSchPres, pA, Retour, VueNb,
			       pDoc, pEl, EnAvant, lqueue, queuePR, queuePP,
				       queuePS, queuePA, NouvPave)
PtrPRule        pRegleC;
PtrSSchema        pSS;
PtrPSchema          pSchPres;
PtrAttribute         pA;
PtrAbstractBox            *Retour;
DocViewNumber           VueNb;
PtrDocument         pDoc;
PtrElement          pEl;
boolean             EnAvant;
int                *lqueue;
PtrPRule        queuePR[MAX_QUEUE_LEN];
PtrAbstractBox             queuePP[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAttribute         queuePA[MAX_QUEUE_LEN];
PtrAbstractBox             NouvPave;

#endif /* __STDC__ */

{
   boolean             retour, creer;
   PtrAbstractBox             pP;

   if (pRegleC->PrType != PtFunction)
     {
	retour = False;
	/* ce n'est pas une regle de creation */
     }
   else
     {
	/* c'est une fonction de presentation */
	retour = pRegleC->PrPresFunction == FnCreateFirst
	   || pRegleC->PrPresFunction == FnCreateLast
	   || pRegleC->PrPresFunction == FnCreateBefore
	   || pRegleC->PrPresFunction == FnCreateWith
	   || pRegleC->PrPresFunction == FnCreateAfter
	   || pRegleC->PrPresFunction == FnCreateEnclosing;
	creer = False;		/* a priori il n' y a pas de pave a creer */
	if (NouvPave != NULL)
	   switch (pRegleC->PrPresFunction)
		 {
		    case FnCreateFirst:
		       if (NouvPave->AbLeafType != LtCompound
			   || NouvPave->AbInLine
#ifdef __COLPAGE__
			   || pRegleC->PrPresBoxRepeat
#endif /* __COLPAGE__ */
			   || !NouvPave->AbTruncatedHead)
			  creer = True;
		       break;
		    case FnCreateLast:
#ifdef __COLPAGE__
		       if (pRegleC->PrPresBoxRepeat)
			  creer = True;
		       else
#endif /* __COLPAGE__ */
			  if (NouvPave->AbLeafType != LtCompound
			      || NouvPave->AbInLine
			      || !NouvPave->AbTruncatedTail)
			  /* on appliquera la regle de creation quand */
			  /* tous les paves descendants de l'element */
			  /* seront crees */
			  Attente (pRegleC, NouvPave, pSchPres, pA, queuePA, queuePS, queuePP,
				   queuePR, lqueue);
		       break;
		    case FnCreateBefore:
#ifdef __COLPAGE__
		       if (pRegleC->PrPresBoxRepeat)
			  creer = True;
		       else
#endif /* __COLPAGE__ */
		       if (EnAvant)
			  /* on ne cree le pave de presentation que si */
			  /* le pave de l'element est complet en tete */
			 {
			    if (NouvPave->AbLeafType != LtCompound
				|| NouvPave->AbInLine
				|| !NouvPave->AbTruncatedHead)
			       creer = True;
			 }
		       else if (NouvPave->AbLeafType != LtCompound
				|| NouvPave->AbInLine)
			  /* on appliquera la regle de creation quand */
			  /* tous les paves descendants de l'element */
			  /* seront crees */
			  Attente (pRegleC, NouvPave, pSchPres, pA, queuePA, queuePS, queuePP,
				   queuePR, lqueue);
		       break;
		    case FnCreateWith:
		    case FnCreateEnclosing:
		       creer = True;
		       break;
		    case FnCreateAfter:
#ifdef __COLPAGE__
		       if (pRegleC->PrPresBoxRepeat)
			  creer = True;
		       else
#endif /* __COLPAGE__ */
		       if (EnAvant)
			 {
			    if (NouvPave->AbLeafType != LtCompound || NouvPave->AbInLine)
			       /* on appliquera la regle de creation quand */
			       /* tous les paves descendants de l'element */
			       /* seront crees */
			       Attente (pRegleC, NouvPave, pSchPres, pA, queuePA, queuePS,
					queuePP, queuePR, lqueue);
			 }
		       else
			  /* on ne cree le pave de presentation que si */
			  /* le pave de l'element est complet en queue */
			  if (NouvPave->AbLeafType != LtCompound || NouvPave->AbInLine ||
			      !NouvPave->AbTruncatedTail)
			  creer = True;
		       break;
		    default:
		       break;
		 }
	/* c'est une creation */
	if (creer)
	   /* on cree le pave de presentation */
	  {
	     pP = CrPavPres (pEl, pDoc, pRegleC, pSS, pA, VueNb, pSchPres, False,
			     False);
	     if (pP != NULL)
		/* le pave de presentation a ete cree */
		if (pRegleC->PrPresFunction == FnCreateBefore ||
		    pRegleC->PrPresFunction == FnCreateEnclosing)
		   /* il a ete cree devant */
		   if (*Retour == NouvPave)
		      /* on s'appretait a retourner */
		      /* un pointeur sur le pave createur */
		      /* on retourne un pointeur sur la pave cree, qui est */
		      /* le premier pave de l'element */
		      *Retour = pP;
	  }
     }
   return retour;
}


/* ---------------------------------------------------------------------- */
/* |    DescVisible cherche le premier descendant de l'element pE qui   | */
/* |            ait un pave dans la vue VueNb.                          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static PtrElement   DescVisible (PtrElement pE, DocViewNumber VueNb, boolean EnAvant)
#else  /* __STDC__ */
static PtrElement   DescVisible (pE, VueNb, EnAvant)
PtrElement          pE;
DocViewNumber           VueNb;
boolean             EnAvant;

#endif /* __STDC__ */
{
   PtrElement          pElem, pDesc;

   pDesc = NULL;
   /* a priori, pas de descendant visible */

   if (pE != NULL)
      if (!pE->ElTerminal && pE->ElFirstChild != NULL)
	 /* l'element a bien une descendance */
	 /* on va traiter tous ses fils, en commencant par le premier, si */
	 /* la creation a lieu en arriere... */
	{
	   pElem = pE->ElFirstChild;
	   /* ...ou par le dernier si elle a lieu en avant */
	   if (EnAvant)
	      while (pElem->ElNext != NULL)
		 pElem = pElem->ElNext;
#ifdef __COLPAGE__
	   do
	     {
		/* on saute les elements marque page si !EnAvant car l'ordre n'est */
		/* plus respecte */
		if (pElem->ElAbstractBox[VueNb - 1] == NULL
		    || (!EnAvant && pElem->ElTypeNumber == PageBreak + 1))
		   /* l'element n'a pas de pave dans la vue cherche le premier */
		   /* descendant qui ait un pave dans la vue */
		   pDesc = DescVisible (pElem, VueNb, EnAvant);
		else
		   /* l'element a un pave dans la vue, on a trouve' */
		   pDesc = pElem;
		if (pDesc == NULL)
		   /* aucun descendant n'a de pave, on passe a l'element */
		   /* suivant */
		   if (EnAvant)
		      pElem = pElem->ElPrevious;
		   else
		      pElem = pElem->ElNext;
		/* on arrete quand on a trouve ou quand on a traite' tous les */
		/* fils de pE. */
	     }
	   while (!(pDesc != NULL || pElem == NULL));
#else  /* __COLPAGE__ */
	   do
	      if (pElem->ElAbstractBox[VueNb - 1] != NULL)
		 /* l'element a un pave dans la vue, on a trouve' */
		 pDesc = pElem;
	      else
		 /* l'element n'a pas de pave dans la vue cherche le premier */
		 /* descendant qui ait un pave dans la vue */
		{
		   pDesc = DescVisible (pElem, VueNb, EnAvant);
		   if (pDesc == NULL)
		      /* aucun descendant n'a de pave, on passe a l'element */
		      /* suivant */
		      if (EnAvant)
			 pElem = pElem->ElPrevious;
		      else
			 pElem = pElem->ElNext;
		   /* on arrete quand on a trouve ou quand on a traite' tous les */
		   /* fils de pE. */
		}
	   while (!(pDesc != NULL || pElem == NULL));
#endif /* __COLPAGE__ */
	}
   return pDesc;
}


/* ---------------------------------------------------------------------- */
/* |    ApplRegleAttributs applique a` l'element toutes les regles de   | */
/* |            presentation de l'attribut pAttr.                       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ApplRegleAttributs (PtrSSchema pSS, PtrPSchema pSchPres,
					PtrAttribute pAttr, PtrAbstractBox * Retour,
					DocViewNumber VueNb, PtrDocument pDoc,
			       PtrElement pEl, boolean EnAvant, int *lqueue,
					PtrPRule queuePR[MAX_QUEUE_LEN],
					PtrAbstractBox queuePP[MAX_QUEUE_LEN],
					PtrPSchema queuePS[MAX_QUEUE_LEN],
					PtrAttribute queuePA[MAX_QUEUE_LEN],
					PtrAbstractBox NouvPave, boolean heritage)
#else  /* __STDC__ */
static void         ApplRegleAttributs (pSS, pSchPres, pAttr, Retour, VueNb, pDoc, pEl,
				 EnAvant, lqueue, queuePR, queuePP, queuePS,
					queuePA, NouvPave, heritage)
PtrSSchema        pSS;
PtrPSchema          pSchPres;
PtrAttribute         pAttr;
PtrAbstractBox            *Retour;
DocViewNumber           VueNb;
PtrDocument         pDoc;
PtrElement          pEl;
boolean             EnAvant;
int                *lqueue;
PtrPRule        queuePR[MAX_QUEUE_LEN];
PtrAbstractBox             queuePP[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAttribute         queuePA[MAX_QUEUE_LEN];
PtrAbstractBox             NouvPave;
boolean             heritage;

#endif /* __STDC__ */

{
   PtrPRule        pR, pRVue1, RegleAAppliquer;
   int                 nv, i;
   PtrPSchema          pSchP;
   PtrHandlePSchema    pHd;
   boolean        applique;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

   applique = True;
   /* exceptions pour les attributs d'un tableau */
   if (ThotLocalActions[T_Tableau_ApplRegleAttribut]!= NULL)
       (*ThotLocalActions[T_Tableau_ApplRegleAttribut])
	 (pEl, pAttr, pDoc, &applique);
   if (applique)
     {
	nv = VueAAppliquer (pEl, pAttr, pDoc, VueNb);
	/* on cherchera d'abord dans le schema de presentation principal de */
	/* l'attribut */
	pSchP = pAttr->AeAttrSSchema->SsPSchema;
	pHd = NULL;
	/* on examine le schema de presentation principal, puis les schemas */
	/* additionnels */
	while (pSchP != NULL)
	  {
	     pRVue1 = NULL;
	     /* 1ere regle de presentation pour cette valeur de l'attribut */
	     pR = ReglePresAttr (pAttr, pEl, heritage, NULL, pSchP);

	     /* boucle sur la liste de regles de presentation associees a cette */
	     /* valeur de l'attribut */
	     while (pR != NULL)
	       {
		  /* verifie si c'est une regle de creation et si oui applique */
		  /* la regle de creation */
		  if (!ApplRegleCreation (pR, pAttr->AeAttrSSchema, pSchP, pAttr, Retour,
				 VueNb, pDoc, pEl, EnAvant, lqueue, queuePR,
				       queuePP, queuePS, queuePA, NouvPave))
		    {
		       /* ce n'est pas une regle de creation, applique la */
		       /* regle si elle concerne la vue */
		       /* ou applique la regle pour la vue 1 si elle existe */
		       RegleAAppliquer = NULL;

		       if (pR->PrViewNum == 1)
			 {
			    /* la regle pour la vue 1 */
			    if (pR->PrCond == NULL ||
				CondPresentation (pR->PrCond, pEl, pAttr, 1, pSS))
			       /* la condition d'application est satisfaite */
			      {
				 /* On la conserve au cas ou on ne trouve pas mieux */
				 pRVue1 = pR;
				 if (nv == 1)
				    /* on est dans la vue 1. Donc c'est la bone regle */
				    RegleAAppliquer = pR;
				 else
				   {
				      /* on cherche s'il existe une regle de meme type pour la */
				      /* vue nv, dont les conditions d'application soient */
				      /* satisfaites */
				      while (pR->PrNextPRule && pR->PrNextPRule->PrType ==
					     pRVue1->PrType)
					{
					   /* la boucle parcourt toutes les regles de meme type */
					   pR = pR->PrNextPRule;
					   if (pR->PrViewNum == nv)
					      if (pR->PrCond == NULL ||
						  CondPresentation (pR->PrCond, pEl, pAttr, nv, pSS))
						 RegleAAppliquer = pR;
					}
				      if (RegleAAppliquer == NULL)
					 /* il n'y a pas de regle specifique pour la vue nv. */
					 /* on prend la vue 1 */
					 RegleAAppliquer = pRVue1;
				   }
			      }
			 }
		       else
			  /* ce n'est pas une regle pour la vue 1 */
			  /* cette regle ne s'applique que si le numero de vue correspond */
		       if (nv == pR->PrViewNum)
			  if (pR->PrCond == NULL ||
			  CondPresentation (pR->PrCond, pEl, pAttr, nv, pSS))
			     RegleAAppliquer = pR;

		       if (RegleAAppliquer && VueExiste (pEl, pDoc, VueNb))
			 {
			    /* Desapplique la regle associee au type de l'element */
			    if (RegleAAppliquer->PrType == PtVertPos)
			      {
				 NouvPave->AbVertPos.PosEdge = Top;
				 NouvPave->AbVertPos.PosRefEdge = Top;
				 NouvPave->AbVertPos.PosDistance = 0;
				 NouvPave->AbVertPos.PosUnit = UnRelative;
				 NouvPave->AbVertPos.PosAbRef = NULL;
				 NouvPave->AbVertPos.PosUserSpecified = False;
			      }
			    if (RegleAAppliquer->PrType == PtHorizPos)
			      {
				 NouvPave->AbHorizPos.PosEdge = Left;
				 NouvPave->AbHorizPos.PosRefEdge = Left;
				 NouvPave->AbHorizPos.PosDistance = 0;
				 NouvPave->AbHorizPos.PosUnit = UnRelative;
				 NouvPave->AbHorizPos.PosAbRef = NULL;
				 NouvPave->AbHorizPos.PosUserSpecified = False;
			      }
			    /* retire la regle associee au type de */
			    /* l'element si elle est en attente */
			    for (i = 1; i <= *lqueue; i++)
			       if (queuePR[i - 1] != NULL)
				  if (queuePR[i - 1]->PrType == RegleAAppliquer->PrType)
				     if (queuePP[i - 1] == NouvPave)
					queuePR[i - 1] = NULL;
			    /* applique la regle */
#ifdef __COLPAGE__
			    if (!Applique (RegleAAppliquer, pSchP, NouvPave, pDoc, pAttr,
					   &bool))
#else  /* __COLPAGE__ */
			    if (!Applique (RegleAAppliquer, pSchP, NouvPave, pDoc, pAttr))
#endif /* __COLPAGE__ */

			       /* la regle n'a pas pu etre appliquee, on l'appliquera */
			       /* quand les paves de la descendance auront ete crees. */
			       Attente (RegleAAppliquer, NouvPave, pSchP, pAttr, queuePA,
					queuePS, queuePP, queuePR, lqueue);
			 }
		    }
		  pR = pR->PrNextPRule;
	       }
	     if (pHd == NULL)
	       {
		  /* on n'a pas encore traite' les schemas de presentation additionnels.
		     On prend le premier schema additionnel si on travaille pour la vue
		     principale, sinon on ignore les schemas additionnels. */
		  if (pDoc->DocView[VueNb - 1].DvPSchemaView == 1)
		     pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
	       }
	     else
		/* passe au schema additionnel suivant */
		pHd = pHd->HdNextPSchema;
	     if (pHd == NULL)
		/* il n'y a pas (ou plus) de schemas additionnels a prendre en compte */
		pSchP = NULL;
	     else
		pSchP = pHd->HdPSchema;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    VisibiliteAttribut modifie le parametre vis selon la regle de   | */
/* |            visibilite de pAttr.                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         VisibiliteAttribut (PtrElement pEl, PtrAttribute pAttr,
				PtrDocument pDoc, int *vis, DocViewNumber VueNb,
					boolean * ok, boolean heritage)
#else  /* __STDC__ */
static void         VisibiliteAttribut (pEl, pAttr, pDoc, vis, VueNb, ok, heritage)
PtrElement          pEl;
PtrAttribute         pAttr;
PtrDocument         pDoc;
int                *vis;
DocViewNumber           VueNb;
boolean            *ok;
boolean             heritage;

#endif /* __STDC__ */

{
   PtrPRule        pR, pRVue1;
   int                 nv;
   boolean             stop, UtiliseVue1;
   PtrPSchema          pSchP;
   PtrHandlePSchema    pHd;
   TypeUnit            unit;

   /* on cherchera d'abord dans le schema de presentation principal de */
   /* l'attribut */
   pSchP = pAttr->AeAttrSSchema->SsPSchema;
   pHd = NULL;
   /* on examine le schema de presentation principal, puis les schemas */
   /* additionnels */
   while (pSchP != NULL)
     {
	/* cherche la premiere regle de presentation pour cette valeur */
	/* de l'attribut, dans ce schema de presentation */
	pR = ReglePresAttr (pAttr, pEl, heritage, NULL, pSchP);
	pRVue1 = NULL;
	if (pR != NULL)
	   if (pR->PrType == PtVisibility)
	      /* cette valeur d'attribut a une regle de visibilite' */
	      /* calcule le numero de la vue concernee par l'attribut */
	     {
		nv = VueAAppliquer (pEl, pAttr, pDoc, VueNb);
		stop = False;
		UtiliseVue1 = True;

		/* cherche s'il y a une regle de visibilite pour la vue */
		while (!stop)
		  {
		     if (pR->PrViewNum == 1)
			if (pR->PrCond == NULL ||
			    CondPresentation (pR->PrCond, pEl, pAttr, 1,
					      pAttr->AeAttrSSchema))
			  {
			     pRVue1 = pR;
			     if (nv == 1)
				stop = True;
			     else
				/* saute les regles de visibilite' suivantes de la vue 1 */
				while (pR->PrNextPRule != NULL &&
				       pR->PrNextPRule->PrType == PtVisibility &&
				       pR->PrNextPRule->PrViewNum == 1)
				   pR = pR->PrNextPRule;
			  }
		     if (!stop)
			if (pR->PrViewNum == nv &&
			    CondPresentation (pR->PrCond, pEl, pAttr, nv,
					      pAttr->AeAttrSSchema))
			  {
			     /* regle trouvee, on l'evalue */
			     *vis = valintregle (pR, pEl, VueNb, ok, &unit, pAttr);
			     UtiliseVue1 = False;
			     stop = True;
			  }
			else if (pR->PrNextPRule == NULL)
			   stop = True;
			else
			  {
			     pR = pR->PrNextPRule;
			     if (pR->PrType != PtVisibility)
				stop = True;
			  }
		  }

		if (UtiliseVue1 && pRVue1 != NULL)
		   /* on n'a pas trouve de regle specifique pour la vue nv */
		   /* On utilise la regle de visibilite de la vue 1 si elle existe */
		   *vis = valintregle (pRVue1, pEl, VueNb, ok, &unit, pAttr);
	     }
	if (pHd == NULL)
	  {
	     /* on n'a pas encore traite' les schemas de presentation additionnels.
	        On prend le premier schema additionnel si on travaille pour la vue
	        principale, sinon on ignore les schemas additionnels. */
	     if (pDoc->DocView[VueNb - 1].DvPSchemaView == 1)
		pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
	  }
	else
	   /* passe au schema additionnel suivant */
	   pHd = pHd->HdNextPSchema;
	if (pHd == NULL)
	   /* il n'y a pas (ou plus) de schemas additionnels a prendre en compte */
	   pSchP = NULL;
	else
	   pSchP = pHd->HdPSchema;
     }
}

/* ------------------------------------------------------------------------ */
/* | ChercheVisib determine la visibilite de l'element a creer en         | */
/* |           fonction de la regle de visibilite associee au type de     | */
/* |           l'element ou a ses attributs                               | */
/* |           Retourne la visibilite calculee dans vis                   | */
/* |           Met a jour vis, pRSpec, pRDef, TypeP et pSchPPage          | */
/* ------------------------------------------------------------------------ */
#ifdef __STDC__
static void         ChercheVisib (PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb,
				  int VueSch, PtrPRule * pRSpec,
				  PtrPRule * pRDef, int *vis,
			     boolean * IgnoreDescendance, boolean * complet,
				  int *TypeP, PtrPSchema * pSchPPage)
#else  /* __STDC__ */
static void         ChercheVisib (pEl, pDoc, VueNb, VueSch, pRSpec, pRDef, vis,
			       IgnoreDescendance, complet, TypeP, pSchPPage)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber           VueNb;
int                 VueSch;
PtrPRule       *pRSpec;
PtrPRule       *pRDef;
int                *vis;
boolean            *IgnoreDescendance;
boolean            *complet;
int                *TypeP;
PtrPSchema         *pSchPPage;

#endif /* __STDC__ */
{
   int                 nv, l;
   PtrPRule        pRegle, pRegleV;
   PtrElement          pPrec, pNext, pPageAssociee, pAsc, pElAttr;
   PtrAttribute         pAttr;
   InheritAttrTable     *tableherites;
   boolean             ok;
   TypeUnit            unit;

#ifdef __COLPAGE__
   int                 NbCol;
   PtrElement          pEl1;

#endif /* __COLPAGE__ */

   pRegle = GetRule (pRSpec, pRDef, pEl, NULL, pEl->ElSructSchema);
   /* pointeur sur la 1ere regle a appliquer */

   /* la premiere regle est la regle de visiblite pour la vue 1 */
   *vis = 0;
   /* parcourt toutes les vues definies dans le schema de presentation */
   /* pour trouver la regle de visibilite pour la vue traitee */
   for (nv = 1; nv <= MAX_VIEW; nv++)
     {
	/* Cherche la regle de visibilite a appliquer */
	if (nv == 1)
	   pRegleV = NULL;
	else
	   pRegleV = GetRuleView (pRSpec, pRDef, PtVisibility, nv, pEl, NULL,
				  pEl->ElSructSchema);
	if (nv == VueSch && VueExiste (pEl, pDoc, VueNb))
	   /* s'il y a une regle de visibilite pour cette vue, on */
	   /* la prend */
	   if (pRegleV != NULL)
	      *vis = valintregle (pRegleV, pEl, VueNb, &ok, &unit, NULL);
	/* sinon, on prend celle de la vue 1 */
	   else
	      *vis = valintregle (pRegle, pEl, VueNb, &ok, &unit, NULL);
     }

   /* cherche si les attributs herites par l'element modifient la */
   /* visibilite */
   if (pEl->ElSructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
     {
	/* il y a heritage possible */
	if ((tableherites = pEl->ElSructSchema->SsPSchema->
	     PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
	  {
	     /* cette table n'existe pas on la genere */
	     CreateInheritedAttrTable (pEl);
	     tableherites = pEl->ElSructSchema->SsPSchema->
		PsInheritedAttr[pEl->ElTypeNumber - 1];
	  }
	for (l = 1; l <= pEl->ElSructSchema->SsNAttributes; l++)
	   if ((*tableherites)[l - 1])	/* pEl herite de l'attribut l */
	      if ((pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElSructSchema,
					       &pElAttr)) != NULL)
		 /* cherche si l existe au dessus */
		 VisibiliteAttribut (pEl, pAttr, pDoc, vis, VueNb, &ok, True);
     }

   /* cherche si les attributs de l'element modifient la visibilite */
   pAttr = pEl->ElFirstAttr;
   while (pAttr != NULL)
      /* boucle sur les attributs de l'element */
     {
	VisibiliteAttribut (pEl, pAttr, pDoc, vis, VueNb, &ok, False);
	pAttr = pAttr->AeNext;	/* attribut suivant de l'element */
     }

   /* force la visibilite du pave racine si elle n'a pas ete evaluee, */
   /* mais seulement pour l'arbre principal ou la vue 1 des elements */
   /* associes. */
   if (!ok && pEl->ElParent == NULL)
      if (pEl->ElAssocNum == 0 || VueSch == 1)
	 *vis = 10;

   /* Si c'est un element associe', il n'est visible que dans la */
   /* vue principale, meme s'il s'affiche en haut ou bas de page. */
   /* NOTE : seule la vue principale peut afficher des */
   /* elements associes en haut ou bas de page */
   if (pEl->ElAssocNum != 0)
      if (!VueAssoc (pEl))
	 if (pDoc->DocView[VueNb - 1].DvPSchemaView != 1)
	    *vis = 0;

   /* on ne cree une marque de page que si elle concerne la vue et */
   /* s'il existe des regles de presentation de la page */
   if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
      /* c'est une marque de page */
      if (pEl->ElViewPSchema != VueSch)
	 /* cette marque ne concerne pas la vue traitee, on ne cree */
	 /* rien */
	{
	   *vis = 0;
	   /* on ne pourra jamais creer de pave' pour cet element dans */
	   /* cette vue */
	   *complet = True;
	}
      else
	 /* cherche le type de boite page ou colonne a creer en  */
	 /* cherchant les regles Page ou Column des elements englobants */
	 /* ou precedants */
	{
#ifdef __COLPAGE__
	   NbCol = 0;
	   /* NbCol indique si pEl est une marque page (=0) */
	   /* ou une marque colonne (>1) */
	   if (pEl->ElPageType == PgBegin
	       || pEl->ElPageType == PgComputed
	       || pEl->ElPageType == PgUser)
	      *TypeP = TypeBPage (pEl, VueSch, pSchPPage);
	   else			/* (pEl->ElPageType == ColBegin */
	      /*  || pEl->ElPageType == ColComputed */
	      /* || pPage->ElPageType == ColGroup */
	      /*  || pEl->ElPageType == ColUser) */
	      *TypeP = TypeBCol (pEl, VueSch, pSchPPage, &NbCol);
#else  /* __COLPAGE__ */
	   *TypeP = TypeBPage (pEl, VueSch, pSchPPage);
#endif /* __COLPAGE__ */
	   if (*TypeP == 0)
	      /* pas de page definie, on ne cree rien */
	      *vis = 0;
	   else if (*vis <= 0)
#ifdef __COLPAGE__
	      *vis = 10;
#else  /* __COLPAGE__ */
	      *vis = 1;
#endif /* __COLPAGE__ */
	}

   /* si la vue n'affiche qu'un sous-arbre, l'element n'est visible */
   /* que s'il est dans le sous-arbre en question ou sur le chemin */
   /* entre la racine et le sous-arbre. */
   if (pEl->ElAssocNum > 0)
     {				/* element associe' */
	if (pDoc->DocAssocSubTree[pEl->ElAssocNum - 1] != NULL)
	  {
	     /* on n'affiche qu'un sous-arbre de ces elements associes */
	     if (!ElemIsAnAncestor (pDoc->DocAssocSubTree[pEl->ElAssocNum - 1], pEl) &&
		 pDoc->DocAssocSubTree[pEl->ElAssocNum - 1] != pEl)
		/* l'elem. traite' n'est pas dans le sous-arbre affichable */
		if (!ElemIsAnAncestor (pEl, pDoc->DocAssocSubTree[pEl->ElAssocNum - 1]))
		   /* il n'englobe pas le sous-arbre affichable */
		   *vis = 0;	/* on ne cree pas son pave' */
	  }
     }
   else
     {				/* vue de l'arbre principal */
	if (pDoc->DocViewSubTree[VueNb - 1] != NULL)
	  {			/* on n'affiche qu'un sous-arbre dans cette vue */
	     if (!ElemIsAnAncestor (pDoc->DocViewSubTree[VueNb - 1], pEl) &&
		 pDoc->DocViewSubTree[VueNb - 1] != pEl)
		/* l'elem. traite' n'est pas dans le sous-arbre affichable */
		if (!ElemIsAnAncestor (pEl, pDoc->DocViewSubTree[VueNb - 1]))
		   /* il n'englobe pas le sous-arbre affichable */
		   *vis = 0;	/* on ne cree pas son pave' */
	  }
     }

   /* si l'element est une copie, qu'il porte une exception
      ExcPageBreakRepetition ou ExcPageBreakRepBefore
      qu'il est associe a un saut de page et que ce saut 
      de page n'appartient pas a la vue concernee, alors
      on ne cree pas son pave. */
   if (pEl->ElSource)
     {
	pPageAssociee = NULL;
	if (TypeHasException (ExcPageBreakRepBefore, pEl->ElTypeNumber,
			    pEl->ElSructSchema))
	  {
	     pNext = pEl->ElNext;
	     while (pNext)
	       {
		  if (pNext->ElTypeNumber == PageBreak + 1)
		    {
		       pPageAssociee = pNext;
		       break;
		    }
		  else if (pNext->ElSource)
		     pNext = pNext->ElNext;
		  else
		     break;
	       }
	  }

	else if (TypeHasException (ExcPageBreakRepetition, pEl->ElTypeNumber,
				 pEl->ElSructSchema))
	  {
	     pPrec = pEl->ElPrevious;
	     while (pPrec)
	       {
		  if (pPrec->ElTypeNumber == PageBreak + 1)
		    {
		       pPageAssociee = pPrec;
		       break;
		    }
		  else if (pPrec->ElSource)
		     pPrec = pPrec->ElPrevious;
		  else
		     break;
	       }
	  }
	if (pPageAssociee)
	   /* La copie est associee a un saut de page */
	   if (pPageAssociee->ElViewPSchema != VueSch)
	     {
		/* cette marque ne concerne pas la vue traitee, */
		/* on ne cree rien */
		*IgnoreDescendance = True;
		*vis = 0;
	     }
	   else
	     {
		/* cherche le type de boite page a creer en cherchant */
		/* les regles Page des elements englobants */
#ifdef __COLPAGE__
		pEl1 = pPageAssociee;
		NbCol = 0;
		/* NbCol indique si pEl est une marque page (=0) */
		/* ou une marque colonne (>1) */
		if (pEl1->ElPageType == PgBegin
		    || pEl1->ElPageType == PgComputed
		    || pEl1->ElPageType == PgUser)
		   *TypeP = TypeBPage (pEl1, VueSch, pSchPPage);
		else		/* (pEl1->ElPageType == ColBegin */
		   /*  || pEl1->ElPageType == ColComputed */
		   /*  || pPage->ElPageType == ColGroup */
		   /*  || pEl1->ElPageType == ColUser) */
		   *TypeP = TypeBCol (pEl1, VueSch, pSchPPage, &NbCol);
#else  /* __COLPAGE__ */
		*TypeP = TypeBPage (pPageAssociee, VueSch, pSchPPage);
#endif /* __COLPAGE__ */
		if (*TypeP == 0)
		  {
		     /* pas de page definie, on ne cree rien */
		     *IgnoreDescendance = True;
		     *vis = 0;
		  }
	     }
     }
   /* si un element ascendant est rendu invisible, on annule la */
   /* visibilite' de l'element */
   pAsc = pEl;
   while (pAsc != NULL)
      if (pAsc->ElAccess == AccessHidden)
	{
	   *vis = 0;
	   pAsc = NULL;
	}
      else
	 pAsc = pAsc->ElParent;
}


/* ---------------------------------------------------------------------- */
/* | ApplReglesPres   Applique les regles de presentation au pave cree  | */
/* |                                                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         ApplReglesPres (PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb,
			   int VueSch, PtrSSchema pSchS, PtrPSchema pSchP,
				PtrPRule * pRSpec, PtrPRule * pRDef,
			     PtrAbstractBox * Retour, boolean EnAvant, int *lqueue,
				    PtrPRule queuePR[MAX_QUEUE_LEN],
				    PtrAbstractBox queuePP[MAX_QUEUE_LEN],
				    PtrPSchema queuePS[MAX_QUEUE_LEN],
			  PtrAttribute queuePA[MAX_QUEUE_LEN], PtrAbstractBox NouvPave)

#else  /* __STDC__ */
static void         ApplReglesPres (pEl, pDoc, VueNb, VueSch, pSchS, pSchP, pRSpec,
			   pRDef, Retour, EnAvant, lqueue, queuePR, queuePP,
				    queuePS, queuePA, NouvPave)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber           VueNb;
int                 VueSch;
PtrSSchema        pSchS;
PtrPSchema          pSchP;
PtrPRule       *pRSpec;
PtrPRule       *pRDef;
PtrAbstractBox            *Retour;
boolean             EnAvant;
int                *lqueue;
PtrPRule        queuePR[MAX_QUEUE_LEN];
PtrAbstractBox             queuePP[MAX_QUEUE_LEN];
PtrPSchema          queuePS[MAX_QUEUE_LEN];
PtrAttribute         queuePA[MAX_QUEUE_LEN];
PtrAbstractBox             NouvPave;

#endif /* __STDC__ */

{
   int                 nv, l;
   PtrPRule        pRegle, pRegleV, pR;
   PtrAttribute         pAttr;
   PtrElement          pElAttr;
   InheritAttrTable     *tableherites;
   boolean             stop;
   PtrHandlePSchema    pHd;
   PtrPSchema          pSchPadd;

#ifdef __COLPAGE__
   boolean             bool;

#endif /* __COLPAGE__ */

   do
     {
	pRegle = GetRule (pRSpec, pRDef, pEl, NULL, pSchS);
	/* pointeur sur la regle a appliquer pour la vue 1 */
	if (pRegle != NULL)
	   /* si c'est une regle de creation, on l'applique */
	   if (!ApplRegleCreation (pRegle, pSchS, pSchP, NULL, Retour, VueNb,
			       pDoc, pEl, EnAvant, lqueue, queuePR, queuePP,
				   queuePS, queuePA, NouvPave))
	      /* ce n'est pas une regle de creation */
	      /* cherche et applique la regle de meme type pour la vue */
	      for (nv = 1; nv <= MAX_VIEW; nv++)
		{
		   if (nv == 1)
		      pRegleV = NULL;
		   else
		      pRegleV = GetRuleView (pRSpec, pRDef, pRegle->PrType, nv, pEl,
					     NULL, pSchS);
		   if (nv == VueSch && NouvPave != NULL &&
		       VueExiste (pEl, pDoc, VueNb))
		     {
			if (pRegleV == NULL)
			   pRegleV = pRegle;
#ifdef __COLPAGE__
			if (!Applique (pRegleV, pSchP, NouvPave, pDoc, NULL, &bool))
#else  /* __COLPAGE__ */
			if (!Applique (pRegleV, pSchP, NouvPave, pDoc, NULL))
#endif /* __COLPAGE__ */
			   Attente (pRegleV, NouvPave, pSchP, NULL,
				    queuePA, queuePS, queuePP,
				    queuePR, lqueue);
		     }
		}
     }
   while (!(pRegle == NULL));

   /* Applique les regles de presentation pour ce type d'element contenues */
   /* dans les schemas de presentation additionnels du document */
   /* On n'applique les schemas additionnels que pour la vue principale d'un */
   /* document */
   if (NouvPave != NULL && VueSch == 1)
     {
	pHd = pEl->ElSructSchema->SsFirstPSchemaExtens;
	while (pHd != NULL)
	  {
	     pSchPadd = pHd->HdPSchema;
	     if (pSchPadd != NULL)
	       {
		  /* applique toutes les regles de presentation associees au type de
		     l'element */
		  pRegle = pSchPadd->PsElemPRule[pEl->ElTypeNumber - 1];
		  while (pRegle != NULL)
		    {
		       if (pRegle->PrCond == NULL ||
			   CondPresentation (pRegle->PrCond, pEl, NULL, 1, pEl->ElSructSchema))

			  /* les conditions d'application de la regle sont satisfaites, */

#ifdef __COLPAGE__
			  if (!Applique (pRegle, pSchPadd, NouvPave, pDoc, NULL, &bool))
#else  /* __COLPAGE__ */
			  if (!Applique (pRegle, pSchPadd, NouvPave, pDoc, NULL))
#endif /* __COLPAGE__ */
			     Attente (pRegleV, NouvPave, pSchP, NULL, queuePA, queuePS,
				      queuePP, queuePR, lqueue);
		       pRegle = pRegle->PrNextPRule;
		    }
	       }
	     pHd = pHd->HdNextPSchema;
	  }
     }

   /* Applique les regles de presentation heritees des attributs  */
   /* poses sur les elements englobants s'il y a heritage, */
   /* alors la table a deja ete calcule precedemment */
   /* on remet l'affectation pour decouper le code */
   tableherites = pEl->ElSructSchema->SsPSchema->PsInheritedAttr[pEl->ElTypeNumber - 1];
   if (NouvPave != NULL)
      if (pEl->ElSructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
	 /* il y a heritage possible */
	 for (l = 1; l <= pEl->ElSructSchema->SsNAttributes; l++)
	    if ((*tableherites)[l - 1])		/* pEl herite de l'attribut l */
	       /* cherche si l'attribut l existe au dessus */
	       if ((pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElSructSchema, &pElAttr)) != NULL)
		  ApplRegleAttributs (pAttr->AeAttrSSchema,
				      pAttr->AeAttrSSchema->SsPSchema,
				      pAttr, Retour, VueNb, pDoc, pEl,
				      EnAvant, lqueue, queuePR,
				      queuePP, queuePS, queuePA,
				      NouvPave, True);
   /* Applique les regles de presentation des attributs de l'element. */
   pAttr = pEl->ElFirstAttr;	/* 1er attribut de l'element */
   if (NouvPave != NULL)
      while (pAttr != NULL)	/* boucle sur les attributs de l'element */
	{
	   ApplRegleAttributs (pAttr->AeAttrSSchema,
			       pAttr->AeAttrSSchema->SsPSchema,
			       pAttr, Retour, VueNb, pDoc, pEl,
			       EnAvant, lqueue, queuePR,
			       queuePP, queuePS, queuePA, NouvPave, False);
	   pAttr = pAttr->AeNext;
	}

   /* Applique les regles de presentation specifiques associees a cet */
   /* element */
   pR = pEl->ElFirstPRule;
   while (pR != NULL)
      /* applique une regle si elle concerne la vue */
      /* et si ce n'est pas une hauteur de page */
     {
	if (!(pEl->ElTypeNumber == PageBreak + 1 && pR->PrType == PtHeight))
	   if ((pR->PrViewNum == VueSch
		|| pR->PrType == PtPictInfo)
	       && NouvPave != NULL
	       && VueExiste (pEl, pDoc, VueNb))
	     {
		if (pR->PrSpecifAttr == 0)
		   pAttr = NULL;
		/* cette regle ne depend pas d'un attribut */
		else
		   /* cherche l'attribut dont depend la regle */
		  {
		     pAttr = pEl->ElFirstAttr;
		     stop = False;
		     while (pAttr != NULL && !stop)
			if (pAttr->AeAttrNum == pR->PrSpecifAttr
			    && pAttr->AeAttrSSchema->SsCode ==
			    pR->PrSpecifAttrSSchema->SsCode)
			   stop = True;
			else
			   pAttr = pAttr->AeNext;
		  }
#ifdef __COLPAGE__
		if (!Applique (pR, pSchP, NouvPave, pDoc, pAttr, &bool))
#else  /* __COLPAGE__ */
		if (!Applique (pR, pSchP, NouvPave, pDoc, pAttr))
#endif /* __COLPAGE__ */
		   Attente (pR, NouvPave, pSchP, pAttr, queuePA,
			    queuePS, queuePP, queuePR, lqueue);
	     }
	pR = pR->PrNextPRule;
     }
}


/* ---------------------------------------------------------------------- */
/* |    CreePaves cree les paves correspondant au sous-arbre de         | */
/* |            l'element pEl du document pDoc, uniquement pour la vue  | */
/* |            VueNb. Certains paves peuvent deja exister; ils ne sont | */
/* |            pas touche's par CreePaves. Si Desc est faux, seuls les | */
/* |            paves de l'element pEl sont crees, sinon, on cree les   | */
/* |            paves de tout le sous arbre. La creation de paves       | */
/* |            s'arrete lorsque tous les paves du sous-arbre sont      | */
/* |            crees ou de`s qu'il n'y a plus de volume libre dans la  | */
/* |            vue. En ElemIsBefore indique si la creation a lieu vers l'avant| */
/* |            ou vers l'arriere. Retourne le dernier (dans l'ordre    | */
/* |            defini par EnAvant) pave de plus haut niveau cree'.     | */
/* |            Au retour, complet indique si l'extremite' (indiquee    | */
/* |            par EnAvant) de l'image abstraite de l'element a pu     | */
/* |            etre creee ou non.                                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrAbstractBox             CreePaves (PtrElement pEl, PtrDocument pDoc, DocViewNumber VueNb,
			   boolean EnAvant, boolean Desc, boolean * complet)

#else  /* __STDC__ */
PtrAbstractBox             CreePaves (pEl, pDoc, VueNb, EnAvant, Desc, complet)
PtrElement          pEl;
PtrDocument         pDoc;
DocViewNumber           VueNb;
boolean             EnAvant;
boolean             Desc;
boolean            *complet;

#endif /* __STDC__ */

{
   PtrPSchema          pSchP, pSchPPage, pSPres;
   PtrPRule        pRegle, pRDef, pRSpec;
   PtrElement          pFils, pPere, pAsc;
   PtrAbstractBox             pP, PavFils, NouvPave, Retour, PavPres;
   PtrAbstractBox             pPRP, PereAssoc;
   int                 vis, TypeP;
   int                 VueSch;
   int                 Entree;
   PtrPRule        queuePR[MAX_QUEUE_LEN];
   PtrAbstractBox             queuePP[MAX_QUEUE_LEN];
   PtrPSchema          queuePS[MAX_QUEUE_LEN];
   PtrAttribute         queuePA[MAX_QUEUE_LEN];
   int                 lqueue, pqueue;
   boolean             stop, ok, crepav, coupe, insecable, IgnoreDescendance;
   boolean             Creation, ApplRegles, PcFirst, PcLast;
   PtrSSchema        pSchS;
   PtrAttribute         pAttr;
   PtrAbstractBox             pAb1;
   boolean             FilsComplet;

#ifdef __COLPAGE__
   int                 nv;
   boolean             bool;
   PtrElement          pElSauv, pElRacine, pEl1, ElRef, pElPage;
   PtrAbstractBox             PavRacine, pAb, pP1;
   PtrPRule        pRegleV;

#endif /* __COLPAGE__ */

   Retour = NULL;
   if (pEl != NULL)
     {
	VueSch = VueAAppliquer (pEl, NULL, pDoc, VueNb);
	PereAssoc = NULL;
	/* pointeur sur le pave qui sera cree' pour l'element */
	NouvPave = NULL;
	ApplRegles = False;
	IgnoreDescendance = False;
	PavPres = NULL;
	insecable = False;
	*complet = False;	/*  a priori, les paves de cet element ne sont */
	/* pas tous crees */
	Creation = False;	/* a priori rien a creer */
#ifdef __COLPAGE__
	pElSauv = NULL;		/* pour memoriser pEl lors de la creation des pages */
	/* arriere */
	if (VueAssoc (pEl))
	  {
	     pElRacine = pDoc->DocAssocRoot[pEl->ElAssocNum - 1];
	     PavRacine = pElRacine->ElAbstractBox[0];
	  }
	else
	  {
	     pElRacine = pDoc->DocRootElement;
	     PavRacine = pElRacine->ElAbstractBox[VueNb - 1];
	  }
#endif /* __COLPAGE__ */
#ifdef __COLPAGE__
	InitPageCol (&pEl, pDoc, VueNb, VueSch, EnAvant, &Creation, complet, &ApplRegles, &pElSauv);
#else  /* __COLPAGE__ */
	pAb1 = pEl->ElAbstractBox[VueNb - 1];
	if (pAb1 != NULL)
	   /* le pave existe deja pour cette vue */
	  {
	     Creation = False;
	     /* on saute les paves de presentation crees par CreateWith */
	     while (pAb1->AbPresentationBox && pAb1->AbNext != NULL)
		pAb1 = pAb1->AbNext;
	     if (pAb1->AbLeafType != LtCompound || pAb1->AbInLine)
		/* c'est une feuille ou un pave' mis en lignes, il */
		/* a deja tout son contenu */
		*complet = True;
	     else if (EnAvant)
		*complet = !pAb1->AbTruncatedTail;
	     else
		*complet = !pAb1->AbTruncatedHead;
	     if (pAb1->AbSize == -1)
		/* il faut lui appliquer ses regles de presentation */
		ApplRegles = True;
	  }
	else
	   /* le pave n'existe pas pour cette vue, on essaiera de le creer. */
	  {
	     Creation = True;
	     /* on ne sait pas si on pourra creer toute son image */
	     *complet = False;
	  }
#endif /* __COLPAGE__ */
	/* on ne cherche a creer que les paves qui n'existent pas deja */
	if (Creation || ApplRegles)
	   /* cherche le schema de presentation a appliquer */
	  {
	     ChSchemaPres (pEl, &pSchP, &Entree, &pSchS);
	     /* pRSpec: premiere regle de presentation specifique. */
	     pRSpec = pSchP->PsElemPRule[Entree - 1];
	     /* premiere regle de presentation par defaut */
	     pRDef = pSchP->PsFirstDefaultPRule;
	     /* TODO code inutile pour la V4 ? */
	     if (pSchP->PsInPageHeaderOrFooter[Entree - 1])
		/* c'est un element associe a afficher dans une boite */
		/* de haut ou de bas de page */
		/* cherche le pave contenant ces elements associes */
	       {
		  if (pEl->ElParent != NULL)
		     PereAssoc = pEl->ElParent->ElAbstractBox[VueNb - 1];
		  if (PereAssoc != NULL)
		     if (PereAssoc->AbPresentationBox)
			/* Il devient temporairement un pave d'element structure' */
			/* pour appliquer correctement les regles de l'element */
			PereAssoc->AbPresentationBox = False;
		     else
			/* ca a deja ete fait par CrPavPres */
			PereAssoc = NULL;
	       }
	     /* TODO fin de code inutile pour la V4  */
	     /* initialise la file des regles qui n'ont pas pu etre appliquees */
	     lqueue = 0;
	     pqueue = 0;

	     /* determine la visibilite du pave a creer */
	     ChercheVisib (pEl, pDoc, VueNb, VueSch, &pRSpec, &pRDef, &vis,
			   &IgnoreDescendance, complet, &TypeP, &pSchPPage);

	     /* si l'element est visible dans cette vue ou si c'est la racine, */
	     /* on cree son pave, si ce n'est deja fait */
	     if (vis >= 1 ||
	     (pEl->ElParent == NULL && (pEl->ElAssocNum == 0 || VueSch == 1)))
		if (ApplRegles)
		   /* on applique seulement les regles de presentation a un */
		   /* pave qui existe deja */
		   /* pour appliquer les regles, on initialise NouvPave au */
		   /* pave de l'element */
		  {
		     NouvPave = pEl->ElAbstractBox[VueNb - 1];
		     NouvPave->AbVisibility = vis;
		     /* Si c'est un element racine qui n'a pas de descendants, */
		     /* alors son pave est complet */
		     if (Desc)
			if (pEl->ElParent == NULL)
			   if (pEl->ElTerminal || pEl->ElFirstChild == NULL)
			     {
				*complet = True;
				if (NouvPave->AbLeafType == LtCompound)
#ifndef __COLPAGE__
				   if (!NouvPave->AbInLine)
#endif /* __COLPAGE__ */
				      if (EnAvant)
					 /* on creera au moins le 1er pave inclus */
					 NouvPave->AbTruncatedHead = False;
				      else
					 /* on creera au moins le dernier pave inclus */
					 NouvPave->AbTruncatedTail = False;
			     }
		  }
		else
		   /* cree et initialise un nouveau pave */
		   /* plusieurs paves sont crees si l'element est une marque */
		   /* de pages : paves corps de page, plus paves dupliques (V4)  */
		  {
		     NouvPave = initpave (pEl, VueNb, vis);
		     NouvPave->AbPSchema = pSchP;
		     NouvPave->AbAcceptLineBreak = pSchP->PsAcceptLineBreak[pEl->ElTypeNumber - 1];
		     NouvPave->AbAcceptPageBreak = pSchP->PsAcceptPageBreak[pEl->ElTypeNumber - 1];
		     NouvPave->AbNotInLine = pSchP->PsNotInLine[pEl->ElTypeNumber - 1];
		     if (pDoc->DocReadOnly)
		       {
			  /* document en lecture seule */
			  NouvPave->AbReadOnly = True;
			  /* on rend les feuilles non modifiables par le Mediateur */
			  if (pEl->ElTerminal)
			     NouvPave->AbCanBeModified = False;
		       }
#ifdef __COLPAGE__
		     if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
		       {
			  if (pEl->ElViewPSchema == VueSch)
			     /* creation des paves de page et duplication  */
			     /* des paves situes dessous les paves PavCorps */
			     CreePageCol (pEl, &NouvPave, pDoc, VueNb, VueSch, EnAvant,
				complet, &lqueue, queuePR, queuePP, queuePS,
				 queuePA, pElSauv, pSchPPage, TypeP, &stop);
			  if (stop)
			     /* on teste stop pour savoir si une le pave a ete */
			     /* detruit suite a l'application d'une regle */
			     /* page : il faut sortir de CreePaves pour */
			     /* recommencer la creation des fils du pere de pEl */
			     /* car un fils marque page a ete ajoute avant pEl */
			     /* si stop est vrai, la destruction de NouvPave a ete faite */
			    {
			       Retour = NULL;
			       return Retour;	/* on sort de CreePaves */
			       /* TODO : est-ce correct ? y-a-il des regles */
			       /* en attente ou retardees ??? */
			    }
		       }
		     else
			/* chaine le nouveau pave dans l'arbre de l'image abstraite */

			Chaine (NouvPave, pEl, VueNb, VueSch, pDoc, EnAvant);

		     /* si ce pave modifie la position de paves voisins, on applique */
		     /* les regles correspondantes : appel de NouvRfPave */
		     /* y compris dans le cas ou NouvPave est un pave corps de page */
		     if (NouvPave != NULL &&
			 ((NouvPave->AbPrevious != NULL &&
			   !NouvPave->AbPrevious->AbNew)
			  || (NouvPave->AbEnclosing != NULL &&
			      !NouvPave->AbEnclosing->AbNew)
			  || (NouvPave->AbNext != NULL &&
			      !NouvPave->AbNext->AbNew)))
			NouvRfPave (NouvPave, NouvPave, &pP, pDoc);	/* pP inutilise */
		     /* si le pave cree est la racine, on met a jour PavRacine */
		     if (PavRacine == NULL && NouvPave != NULL
			 && pEl->ElParent == NULL)
			PavRacine = NouvPave;
#else  /* __COLPAGE__ */
		     /* chaine le nouveau pave dans l'arbre de l'image abstraite */
		     Chaine (NouvPave, pEl, VueNb, pDoc);
#endif /* __COLPAGE__ */
		     Retour = NouvPave;
		     if (Desc)	/* on va creer les paves inclus */
		       {
			  pAb1 = NouvPave;
			  if (pAb1->AbLeafType == LtCompound)
			     if (!pAb1->AbInLine)
				if (EnAvant)
				   /* on creera au moins le 1er pave inclus */
				   pAb1->AbTruncatedHead = False;
				else
				   /* on creera au moins le dernier pave inclus */
				   pAb1->AbTruncatedTail = False;
			     else
				/* pave mis en ligne, on cree tout */
				*complet = True;
			  else
			     /* pave feuille, on cree tout */
			     *complet = True;
		       }
		  }
	  }
	if (NouvPave == NULL && Creation && !IgnoreDescendance && !pEl->ElHolophrast)
	  {
	     /* cet element n'est pas visible dans la vue, on cherche a creer les */
	     /* paves d'un descendant visible */
	     if (Desc)
		if (pEl->ElTerminal)
		   *complet = True;
		else if (pEl->ElFirstChild == NULL)
		   *complet = True;
		else
		   /* cherche d'abord le premier descendant visible dans la vue */
		  {
		     pFils = DescVisible (pEl, VueNb, EnAvant);
		     if (pFils == NULL)
			/* pas de descendant visible, on traite les fils */
		       {
			  pFils = pEl->ElFirstChild;
			  /* premier fils de l'element */
			  /* cherche le dernier fils si la creation a lieu en arriere */
			  if (!EnAvant && pFils != NULL)
			     while (pFils->ElNext != NULL)
				pFils = pFils->ElNext;
		       }
		     if (pFils != NULL)
			/* on cherche si un ascendant a un pave insecable dans la vue */
		       {
			  insecable = False;	/* a priori, non */
			  pAsc = pEl;
			  pP = NULL;
			  while (pAsc != NULL && pP == NULL)
			    {
			       pP = pAsc->ElAbstractBox[VueNb - 1];
			       /* on cherche le pave principal de cet ascendant dans la vue */
			       stop = False;
			       do
				  if (pP == NULL)
				     stop = True;
				  else if (pP->AbPresentationBox)
				     pP = pP->AbNext;
				  else
				     stop = True;
			       while (!(stop));
			       if (pP != NULL)
				  /* cet ascendant a un pave, est-il secable ? */
				  insecable = NonSecable (pP);
			       else
				  /* pas de pave, on passe a l'ascendant du dessus */
				  pAsc = pAsc->ElParent;
			    }
		       }
		     while (pFils != NULL)
			/* cree les paves d'un descendant */
		       {
			  pP = CreePaves (pFils, pDoc, VueNb, EnAvant, Desc, &FilsComplet);
			  if (pP != NULL)
			     Retour = pP;
			  /* passe au fils suivant ou precedent, selon le sens de */
			  /* creation */
			  if (EnAvant)
			     pFils = pFils->ElNext;
			  else
			     pFils = pFils->ElPrevious;
			  if (pFils == NULL)
			     /* on a cree' les paves du dernier fils */
			     *complet = FilsComplet;
			  else
			     /* verifie que la vue n'est pas pleine, mais si un */
			     /* ascendant est insecable, on continue quand meme */
			  if (VuePleine (VueNb, pDoc, pEl) && !insecable)
			     /* vue pleine, on arrete la creation des paves des fils */
			     pFils = NULL;
		       }
		  }
	  }			/* fin if (Desc) */
	else if (!IgnoreDescendance)
	   /* l'element est visible dans la vue */
	  {
	     if ((Creation || ApplRegles) && Desc)
	       {
#ifdef __COLPAGE__
		  if (!(pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak))
		     /* dans la V4 les regles de la page ont deja */
		     /* ete appliquees apres duplication dans CreePageCol */
#endif /* __COLPAGE__ */
		     ApplReglesPres (pEl, pDoc, VueNb, VueSch, pSchS,
				   pSchP, &pRSpec, &pRDef, &Retour, EnAvant,
		     &lqueue, queuePR, queuePP, queuePS, queuePA, NouvPave);

		  /* traitement particulier aux sauts de page (il faut prendre */
		  /* le bon schema de presentation) */
		  /* uniquement pour la V3 car dans la V4 les regles ont deja */
		  /* ete appliquees apres duplication dans CreePageCol */
#ifndef __COLPAGE__
		  if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak &&
		      !pEl->ElHolophrast)
		     /* c'est une marque de page */
		     if (pEl->ElViewPSchema == VueSch)
			/* cette marque concerne la vue traitee */
			ApplPage (pEl, pDoc, VueNb, VueSch, TypeP, pSchPPage,
				  NouvPave);
#endif /* __COLPAGE__ */
	       }
	     if (pEl->ElTerminal)
		*complet = True;
	     if (Desc && !pEl->ElTerminal && !pEl->ElHolophrast)
		/* cree les paves des fils de l'element, dans la limite de la */
		/* capacite de la vue */
		/* determine d'abord si l'element est secable et pour cela */
		/* cherche le 1er pave de l'element qui ne soit pas un pave de */
		/* presentation */
	       {
		  PavPres = NULL;
		  pP = pEl->ElAbstractBox[VueNb - 1];
		  stop = False;
		  do
		     if (pP == NULL)
			stop = True;
		     else if (pP->AbPresentationBox)
			pP = pP->AbNext;
		     else
			stop = True;
		  while (!(stop));
		  insecable = NonSecable (pP);
		  /* determine le 1er pave fils a creer */
		  pFils = pEl->ElFirstChild;	/* premier fils de l'element */
		  if (pFils == NULL)
		     /* le pave n'a pas de descendance */
		     /* pave vide, il n'est pas coupe' */
		    {
		       *complet = True;
		       PavPres = PaveCoupe (NouvPave, False, !EnAvant, pDoc);
		       if (PavPres != NULL)
			  /* on a cree des paves de presentation */
			  if (PavPres->AbEnclosing != NouvPave)
			     Retour = PavPres;
		    }
		  else if (Creation || insecable)
		     /* il faut creer les paves de tous les fils */
		     /* creation en avant: on commence par le 1er fils */
		     /* creation en reculant: commence par le dernier fils */
		    {
		       if (!EnAvant)
			 {
			    stop = False;
			    do
			       if (pFils == NULL)
				  stop = True;
			       else if (pFils->ElNext == NULL)
				  stop = True;
			       else
				  pFils = pFils->ElNext;
			    while (!(stop));
			 }
		    }
		  else
		     /* il y a peut-etre deja un pave fils */
		     /* on ne cree des paves qu'a partir de l'element descendant */
		     /* qui possede deja un pave dans la vue */
		     /* cherche le premier descendant qui ait un pave dans la vue */
		    {
		       pFils = DescVisible (pEl, VueNb, EnAvant);
		       if (pFils == NULL)
			  /* aucun descendant n'a de pave dans la vue */
			  /* on cree les paves de tous les fils */
			 {
			    pFils = pEl->ElFirstChild;
			    /* premier fils de l'element */
			    if (!EnAvant)
			       /* creation en reculant ; cherche le dernier fils */
			      {
				 stop = False;
				 do
				    if (pFils == NULL)
				       stop = True;
				    else if (pFils->ElNext == NULL)
				       stop = True;
				    else
				       pFils = pFils->ElNext;
				 while (!(stop));
			      }
			 }
		    }
		  /* cree les paves des fils successifs */
		  while (pFils != NULL)
		     /* verifie que la vue n'est pas pleine */
#ifdef __COLPAGE__
		    {
		       PagePleine (VueNb, pDoc, pFils, EnAvant);
		       /* on arrete la creation des paves si la vue et pleine */
		       /* ET si la page en cours est pleine : c'est a dire */
		       /* si l'element suivant a creer est une marque de page */
		       /* test fait dans PagePleine : on lui passe pFils */
		       /* on appelle PagePleine pour positionner le volume */
		       /* a -1 si la page en cours de creation est pleine */
		       /* PagePleine detruit eventuellement des paves vides */
#endif /* __COLPAGE__ */
		       if (VuePleine (VueNb, pDoc, pEl) && !insecable)
			  /* vue pleine, on arrete la creation des paves des fils */
			 {
			    pFils = NULL;
			    /* le pave englobant des fils est incomplet */
			    pPere = pEl;
			    /* cherche le 1er ascendant visible dans la vue */
			    stop = False;
			    do
			       if (pPere == NULL)
				  stop = True;
			       else if (pPere->ElAbstractBox[VueNb - 1] != NULL)
				  stop = True;
			       else
				  pPere = pPere->ElParent;
			    while (!(stop));
			    /* marque ce pave coupe' */
			    if (pPere != NULL)
#ifdef __COLPAGE__
			      {
				 /* la recherche se fait par l'arbre abstrait, il faut */
				 /* considerer la chaine des dupliques pour trouver */
				 /* le pave a couper */
				 pP = pPere->ElAbstractBox[VueNb - 1];
				 /* on saute les eventuels paves de presentation */
				 while (pP->AbPresentationBox)
				    pP = pP->AbNext;
				 if (EnAvant)
				    while (pP->AbNextRepeated != NULL)
				       pP = pP->AbNextRepeated;
				 PavPres = PaveCoupe (pP, True, !EnAvant, pDoc);
			      }
#else  /* __COLPAGE__ */
			       PavPres = PaveCoupe (pPere->ElAbstractBox[VueNb - 1], True, !EnAvant, pDoc);
#endif /* __COLPAGE__ */
			 }
		       else
			  /* Cree les paves d'un fils et passe a un autre fils */
			 {
			    ok = True;	/* a priori on va creer les paves du fils */
			    if (!ApplRegles)	/* verifie si le fils est deja complet */
			       if (pFils->ElAbstractBox[VueNb - 1] != NULL)
				  /* le fils a deja des paves dans cette vue */
				 {
				    PavFils = pFils->ElAbstractBox[VueNb - 1];
#ifdef __COLPAGE__
				    /* on saute les paves de presentation pour tester */
				    /* CT et CQ correctement : en effet le pave  */
				    /* principal peut etre coupe tout en ayant des */
				    /* paves de presentation (si marque Page est premier */
				    /* ou dernier fils. cf. destruction lors de la dup */
				    /* cas des paves de presentation repetes */
				    while (PavFils->AbPresentationBox)
				       PavFils = PavFils->AbNext;
				    if (EnAvant)
				       /* cherche le dernier pave du fils */
				       while (PavFils->AbNextRepeated != NULL)
					  PavFils = PavFils->AbNextRepeated;
				    /* est-ce ce que l'on veut ? */
				    pAb1 = PavFils;
				    /* on appellera CreePaves s'il faut appliquer les */
				    /* regles de presentation de ce fils */
				    if (pAb1->AbSize != -1
					&& !(!EnAvant && pFils->ElTypeNumber == PageBreak + 1))
				       /* si !EnAvant, il faut appeler CreePaves si */
				       /* l'element est une marque de page */
				       if (pAb1->AbLeafType != LtCompound)
					  /* supprime                  || pAb1->AbInLine) */
					  ok = False;	/* pave fils complet */
				       else if (EnAvant)
					 {
					    if (!pAb1->AbTruncatedTail)
					       ok = False;	/* pave fils complet */
					 }
				       else if (!pAb1->AbTruncatedHead)
					  ok = False;	/* pave fils complet */
#else  /* __COLPAGE__ */
				    /* saute les paves de presentation du fils */
				    stop = False;
				    do
				       if (PavFils->AbNext == NULL)
					  stop = True;
				       else if (PavFils->AbPresentationBox)
					  PavFils = PavFils->AbNext;
				       else
					  stop = True;
				    while (!(stop));
				    /* on appellera CreePaves s'il faut appliquer les */
				    /* regles de presentation de ce fils */
				    if (PavFils->AbSize != -1)
				       if (PavFils->AbLeafType != LtCompound ||
					   PavFils->AbInLine)
					  ok = False;	/* pave fils complet */
				       else if (EnAvant)
					 {
					    if (!PavFils->AbTruncatedTail)
					       ok = False;	/* pave fils complet */
					 }
				       else if (!PavFils->AbTruncatedHead)
					  ok = False;	/* pave fils complet */
#endif /* __COLPAGE__ */
				 }
			    if (ok)
#ifdef __COLPAGE__
			      {
#endif /* __COLPAGE__ */
				 /* on cree effectivement les paves du fils */
				 PavFils = CreePaves (pFils, pDoc, VueNb, EnAvant, Desc, &FilsComplet);
#ifdef __COLPAGE__
				 /* cas particulier ou le fils avait une regle page */
				 /* qui a provoque la creation d'un element marque page */
				 /* avant l'element pFils : les paves de pFils ont ete */
				 /* detruits dans la procedure applPage_Applique et ils */
				 /* faut recommencer l'appel de CreePaves pour pFils */
				 /* pour qu'ils soient correctement places sous la nouvelle */
				 /* page */
				 /* (les paves de la nouvelle page ont deja ete crees */
				 /* sauf si la vue etait pleine ) */
				 if (PavFils == NULL && pFils->ElPrevious != NULL)
				   {
				      pEl1 = pFils->ElPrevious;
				      if (pEl1->ElTerminal
					  && pEl1->ElLeafType == LtPageColBreak
					  && pEl1->ElViewPSchema == VueSch
					  && (pEl1->ElPageType == PgBegin
					   || pEl1->ElPageType == ColBegin))
					{
					   PagePleine (VueNb, pDoc, pFils, EnAvant);
					   if (VuePleine (VueNb, pDoc, pFils))
					      pFils = NULL;
					   else
					      PavFils = CreePaves (pFils, pDoc, VueNb,
					       EnAvant, Desc, &FilsComplet);
					}
				      /* remarque: il se peut que pFils soit invisible */
				      /* pour cette vue et que PavFils soit null a ce */
				      /* retour de procedure */
				   }
			      }
#endif /* __COLPAGE__ */
			    else
			       /* le pave du fils etait deja complet */
			      {
				 PavFils = NULL;
				 FilsComplet = True;
			      }
#ifdef __COLPAGE__
			    if (pFils != NULL)
			      {
#endif /* __COLPAGE__ */
				 if (NouvPave == NULL)
				    if (PavFils != NULL)
				       Retour = PavFils;
#ifdef __COLPAGE__
				 /* cas particulier de la pagination : si on appelle */
				 /* CreePaves pour un fils non cree a cause du booleen */
				 /* ArretAvantCreation, il ne faut pas executer le code */
				 /* premier ou dernier */
				 if (ok && PaginationEnCours && ArretAvantCreation
				     && pFils->ElTerminal && !FilsComplet)
				    /* pFils est soit une marque page soit une reference */
				    /* a un element associe en haut ou bas de page */
				   {
				      PcFirst = False;
				      PcLast = False;
				   }
				 else
#endif /* __COLPAGE__ */
				 if (EnAvant)
				   {
				      PcLast = (pFils->ElNext == NULL);
				      PcFirst = False;
				   }
				 else
				   {
				      PcFirst = (pFils->ElPrevious == NULL);
				      PcLast = False;
				   }
				 pAsc = pFils->ElParent;
				 while (pAsc != pEl)
				   {
				      if (PcFirst)
					 PcFirst = (pAsc->ElPrevious == NULL);
				      if (PcLast)
					 PcLast = (pAsc->ElNext == NULL);
				      pAsc = pAsc->ElParent;
				   }

				 pPere = pFils->ElParent;
				 if (EnAvant)
				    if (pFils->ElNext != NULL)
				       /* passe au fils suivant */
				       pFils = pFils->ElNext;
				    else
				       /* pas de fils suivant */
				      {
					 /* on cherche le premier englobant qui ne soit pas */
					 /* l'element traite' et qui ait un voisin */
					 stop = False;
					 do
					    if (pFils->ElParent == pEl)
					       /* on traitait le pere, on s'arrete */
					      {
						 pFils = NULL;
						 stop = True;
					      }
					    else
					       /* on traite le frere suivant du pere */
					      {
						 pFils = pFils->ElParent;
						 if (pFils == NULL)
						    stop = True;
						 else if (pFils->ElNext != NULL)
						   {
						      pFils = pFils->ElNext;
						      stop = True;
						   }
					      }
					 while (!(stop));
				      }
				 else if (pFils->ElPrevious != NULL)
				    pFils = pFils->ElPrevious;
				 /* passe au fils precedent */
				 else
				    /* pas de fils precedent */
				   {
				      /* on cherche le premier englobant qui ne soit pas */
				      /* l'element traite' et qui ait un voisin */
				      stop = False;
				      do
					 if (pFils->ElParent == pEl)
					    /* on traitait le pere, on s'arrete */
					   {
					      pFils = NULL;
					      stop = True;
					   }
					 else
					    /* on traite le frere precedent du pere */
					   {
					      pFils = pFils->ElParent;
					      if (pFils == NULL)
						 stop = True;
					      else if (pFils->ElPrevious != NULL)
						{
						   pFils = pFils->ElPrevious;
						   stop = True;
						}
					   }
				      while (!(stop));
				   }
				 if (PcLast || PcFirst)
				    /* on a cree' les paves du premier (ou dernier) fils */
				    /* de pEl, le pave englobant des fils de pEl est  */
				    /* complet si le pave du premier (ou dernier) fils est */
				    /* complet ou s'il ne contient rien (tous ses */
				    /* descendants ont une visibilite' nulle) */
				   {
				      stop = False;
				      do
					 if (pPere != NULL)
					    if (pPere->ElAbstractBox[VueNb - 1] != NULL)
					       stop = True;
					    else if ((EnAvant && pPere->ElNext == NULL)
						     || (!EnAvant
					     && pPere->ElPrevious == NULL))
					       pPere = pPere->ElParent;
					    else
					       pPere = NULL;
				      while (!(stop || pPere == NULL)) ;
				      if (pPere != NULL)
					{
					   pP = pPere->ElAbstractBox[VueNb - 1];
					   /* saute les paves de presentation crees par */
					   /* FnCreateBefore */
					   stop = False;
					   do
					      if (pP == NULL)
						 stop = True;
					      else if (!pP->AbPresentationBox)
						 stop = True;
					      else
						 pP = pP->AbNext;
					   while (!(stop));
					   if (pP != NULL)
					     {
						PavFils = pP->AbFirstEnclosed;
						PavPres = NULL;
						if (PcLast)
						  {
#ifdef __COLPAGE__
						     /* on se positionne sur le dernier pave */
						     /* de l'element pPere */
						     while (pP->AbNextRepeated != NULL)
							pP = pP->AbNextRepeated;
						     PavFils = pP->AbFirstEnclosed;
						     /* cas particulier de la racine: il faut */
						     /* aller sous le dernier corps de page */
						     if (PavFils != NULL
							 && PavFils->AbElement->ElTypeNumber
						     == PageBreak + 1)
						       {
							  while (PavFils->AbNext != NULL)
							     PavFils = PavFils->AbNext;
							  /* PavFils  dernier fils de la racine */
							  while (PavFils->AbPresentationBox)
							     PavFils = PavFils->AbPrevious;
							  /* PavFils est le dernier corps de page
							     on descend la hierarchie des colonnes */
							  while (PavFils->AbFirstEnclosed != NULL &&
								 PavFils->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
							    {
							       PavFils = PavFils->AbFirstEnclosed;
							       while (PavFils->AbNext != NULL)
								  PavFils = PavFils->AbNext;
							       while (PavFils->AbPresentationBox)
								  PavFils = PavFils->AbPrevious;
							    }
							  PavFils = PavFils->AbFirstEnclosed;
						       }
#endif /* __COLPAGE__ */
						     if (PavFils == NULL)
							/* tous les descendants ont une visibilite' */
							/* nulle; le pave est donc complet en queue */
							coupe = False;
						     else
						       {
							  /* cherche le dernier pave' fils */
							  while (PavFils->AbNext != NULL)
							     PavFils = PavFils->AbNext;
							  /* ignore les paves de presentation */
							  while (PavFils->AbPresentationBox &&
								 PavFils->AbPrevious != NULL)
							     PavFils = PavFils->AbPrevious;
							  if (PavFils->AbDead)
							     coupe = True;
							  else
							     coupe = !FilsComplet;

						       }
						     if (PereAssoc != NULL)
							PereAssoc->AbPresentationBox = True;
						     PavPres = PaveCoupe (pP, coupe, False, pDoc);
						     if (EnAvant && !coupe)
							*complet = True;
						     if (PereAssoc != NULL)
							PereAssoc->AbPresentationBox = False;
						  }
						if (PcFirst)
						  {
						     PavFils = pP->AbFirstEnclosed;
#ifdef __COLPAGE__
						     /* cas particulier de la racine : il faut */
						     /* aller sous le premier corps de page */
						     if (PavFils != NULL
							 && PavFils->AbElement->ElTypeNumber
						     == PageBreak + 1)
						       {
							  while (PavFils->AbPresentationBox)
							     PavFils = PavFils->AbNext;
							  /* PavFils premier corps de page. On */
							  /* descend la hierarchie des colonnes */
							  while (PavFils->AbFirstEnclosed != NULL &&
								 PavFils->AbFirstEnclosed->AbElement->ElTypeNumber == PageBreak + 1)
							    {
							       PavFils = PavFils->AbFirstEnclosed;
							       while (PavFils->AbPresentationBox)
								  PavFils = PavFils->AbNext;
							    }
							  PavFils = PavFils->AbFirstEnclosed;
						       }
#endif /* __COLPAGE__ */
						     if (PavFils == NULL)
							/* tous les descendants ont une visibilite' */
							/* nulle ; le pave est donc complet en tete */
							coupe = False;
						     else
						       {
							  /* ignore les paves de presentation */
							  while (PavFils->AbPresentationBox &&
								 PavFils->AbNext != NULL)
							     PavFils = PavFils->AbNext;
							  if (PavFils->AbDead)
							     coupe = True;
							  else
							     coupe = !FilsComplet;
						       }
						     if (PereAssoc != NULL)
							PereAssoc->AbPresentationBox = True;
						     PavPres = PaveCoupe (pP, coupe, True, pDoc);
						     if (!EnAvant && !coupe)
							*complet = True;
						     if (PereAssoc != NULL)
							PereAssoc->AbPresentationBox = False;
						  }
						if (PavPres != NULL)
						   /* on a cree des paves de presentation */
#ifdef __COLPAGE__
						   /* retour doit etre le pave de plus haut */
						   /* niveau cree : mais attention le pave */
						   /* de presentation cree est peut etre sur */
						   /* une autre page */
						   if (Retour == NULL ||
						       (!Creation || (PavPres->AbEnclosing != pP
						       && pP != PavRacine)))
#else  /* __COLPAGE__ */
						   if (!Creation || PavPres->AbEnclosing != pP)
#endif /* __COLPAGE__ */
						      Retour = PavPres;
					     }
					}
				   }
#ifdef __COLPAGE__
			      }	/* fin pFils != NULL apres appel creation fils */
#endif /* __COLPAGE__ */
			 }	/* fin: Cree les paves d'un fils et passe a un autre fils */
#ifdef __COLPAGE__
		    }		/* fin while( pFils != NULL) */
#endif /* __COLPAGE__ */
		  /* met les valeurs par defaut des axes de reference si aucune */
		  /* regle de positionnement des axes de reference n'a ete  */
		  /* appliquee */
		  if (Creation)
		     if (NouvPave != NULL)
		       {
			  if (NouvPave->AbVertRef.PosAbRef == NULL)
			     NouvPave->AbVertRef.PosAbRef = NouvPave->AbFirstEnclosed;
			  if (NouvPave->AbHorizRef.PosAbRef == NULL)
			     NouvPave->AbHorizRef.PosAbRef = NouvPave->AbFirstEnclosed;
		       }
	       }		/* fin creation des fils */
	     if ((Creation || ApplRegles) && Desc)
		/* applique les regles en attente */
		do
		  {
		     GetAtt (&pRegle, &pP, &pSPres, &pAttr, queuePA, queuePS, queuePP,
			     queuePR, &lqueue, &pqueue);
		     /* recupere une regle en attente */
#ifdef __COLPAGE__
		     if (pRegle != NULL && !pP->AbDead)
#else  /* __COLPAGE__ */
		     if (pRegle != NULL)
#endif /* __COLPAGE__ */
		       {
			  crepav = False;
			  /* a priori ce n'est pas une regle de creation */
			  if (pRegle->PrType == PtFunction)
			     /* on applique les regles de creation systematiquement:
			        on a verifie' lors de la mise en attente que le pave
			        createur n'etait pas coupe'. */
			    {
			       if (pRegle->PrPresFunction == FnCreateBefore
				   || pRegle->PrPresFunction == FnCreateWith
				   || pRegle->PrPresFunction == FnCreateAfter
				   || pRegle->PrPresFunction == FnCreateEnclosing
				   || pRegle->PrPresFunction == FnCreateFirst
				   || pRegle->PrPresFunction == FnCreateLast)
				 {
				    crepav = True;
				    if (pAttr != NULL)
				       PavPres = CrPavPres (pEl, pDoc, pRegle,
					  pAttr->AeAttrSSchema, pAttr, VueNb,
					 pAttr->AeAttrSSchema->SsPSchema,
							    False, True);
				    else
				       PavPres = CrPavPres (pEl, pDoc, pRegle,
					      pEl->ElSructSchema, NULL, VueNb,
						       pSPres, False, True);
				 }
			       switch (pRegle->PrPresFunction)
				     {
					case FnCreateBefore:
					   if (!EnAvant && PavPres != NULL)
					      Retour = PavPres;
					   break;
					case FnCreateAfter:
					case FnCreateWith:
					   if (EnAvant && PavPres != NULL)
					      Retour = PavPres;
					   break;
					case FnCreateEnclosing:
					   Retour = PavPres;
					   break;
					default:
					   break;
				     }
			    }
			  if (!crepav)
			     /* ce n'est pas une regle de creation */
#ifdef __COLPAGE__
			     if (!Applique (pRegle, pSPres, pP, pDoc, pAttr, &bool))
				/* regles retardees a la racine */
				/* TODO est-ce necessaire ? */
			       {
				  if (VueAssoc (pEl))
				     pAb =
					pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0];
				  else
				     pAb = pDoc->DocRootElement->ElAbstractBox[VueNb - 1];
				  Retarde (pRegle, pSPres, pP, pAttr, pAb);
			       }
#else  /* __COLPAGE__ */
			     if (!Applique (pRegle, pSPres, pP, pDoc, pAttr))
				Retarde (pRegle, pSPres, pP, pAttr, pP);
#endif /* __COLPAGE__ */
		       }
		  }
		while (!(pRegle == NULL));
#ifdef __COLPAGE__
	     if ((Creation || ApplRegles) && Desc
		 && EnAvant && pEl->ElParent == NULL
		 && pEl->ElAbstractBox[VueNb - 1] != NULL
		 && pEl->ElAbstractBox[VueNb - 1]->AbTruncatedHead)
	       {
		  /* cas des regles de creation des boites de presentation */
		  /* de la racine qui ont ete detruites par la creation de */
		  /* pages ou de colonnes */
		  do
		    {
		       pRegle = GetRule (&pRSpec, &pRDef, pEl, NULL, pSchS);

		       /* pointeur sur la regle a */
		       /* appliquer pour la vue 1 */
		       if (pRegle != NULL)
			  /* si c'est une regle de creation, on l'applique */
			  if (!ApplRegleCreation (pRegle, pSchS, pSchP, NULL, &Retour,
						  VueNb, pDoc, pEl, EnAvant,
						  &lqueue, queuePR, queuePP,
						queuePS, queuePA, NouvPave))
			     /* ce n'est pas une regle de creation */
			     /* cherche et applique la regle de meme type pour la vue */
			     for (nv = 1; nv <= MAX_VIEW; nv++)
			       {
				  if (nv == 1)
				     pRegleV = NULL;
				  else
				     pRegleV = GetRuleView (&pRSpec, &pRDef, pRegle->PrType, nv, pEl, NULL, pSchS);
				  if (nv == VueSch && NouvPave != NULL
				      && VueExiste (pEl, pDoc, VueNb))
				    {
				       if (pRegleV == NULL)
					  pRegleV = pRegle;
				       if (!Applique (pRegleV, pSchP, NouvPave, pDoc, NULL, &bool))
					  Attente (pRegleV, NouvPave, pSchP, NULL,
						   queuePA, queuePS, queuePP,
						   queuePR, &lqueue);
				       else
					  /* la regle a ete appliquee */
					  /* on teste bool pour savoir si une le pave a ete */
					  /* detruit suite a l'application d'une regle */
					  /* page : il faut sortir de CreePaves pour */
					  /* recommencer la creation des fils du pere de pEl */
													/* car un fils marque page a ete ajoute avant pEl */ if (bool)
													/* la destruction de NouvPave a ete faite */
					 {
					    Retour = NULL;
					    return Retour;	/* on sort de CreePaves */
					    /* TODO : est-ce correct ? y-a-il des regles */
					    /* en attente ou retardees ??? */
					 }
				    }
			       }
		    }
		  while (!(pRegle == NULL));
	       }
#endif /* __COLPAGE__ */
	     /* applique toutes les regles en retard des descendants */
	     if (Desc)
#ifdef __COLPAGE__
	       {
		  /* applique toutes les regles en retard des descendants */
		  /* y compris les regles des descendants des paves dupliques */
		  pP = pEl->ElAbstractBox[VueNb - 1];
		  /* saute les paves de presentation crees par FnCreateBefore */
		  stop = False;
		  do
		     if (pP == NULL)
			stop = True;
		     else if (!pP->AbPresentationBox)
			stop = True;
		     else if (pP->AbFirstEnclosed != NULL &&
			      pP->AbFirstEnclosed->AbElement == pEl)
			/* pave cree' par la regle FnCreateEnclosing */
			stop = True;
		     else
			pP = pP->AbNext;
		  while (!(stop));
		  /* on initialise le pave sur lequel des regles seront */
		  /* mises les regles retardees a nouveau */
		  if (VueAssoc (pEl))
		     pAb = pDoc->DocAssocRoot[pEl->ElAssocNum - 1]->ElAbstractBox[0];
		  else
		     pAb = pDoc->DocRootElement->ElAbstractBox[VueNb - 1];
		  pPRP = pP;
		  /* boucle de parcours de la chaine des paves dupliques */
		  while (pPRP != NULL)
		    {
		       do
			 {
			    pP = pPRP;	/* pP a ete modifie dans la boucle */
			    GetRet (&pRegle, &pSPres, &pP, &pAttr);
			    if (pRegle != NULL && !pP->AbDead)
			       if (pRegle->PrType == PtFunction)
				  /* cas des regles de creation des boites de pres */
				  /* de la racine qu'il faut recreer */
				  pP1 = CrPavPres (pP->AbElement, pDoc, pRegle,
					 pP->AbElement->ElSructSchema, pAttr,
					       VueNb, pSPres, False, False);
			       else if (!Applique (pRegle, pSPres, pP, pDoc, pAttr, &bool))
				  /* cette regle n'a pas pu etre appliquee           */
				  /* c'est une regle correspondant a un attribut, on */
				  /* l'appliquera lorsque l'englobant sera complet   */
				  /* regles retardees a la racine */
				 {
				    if (pPRP != pAb)
				       /* pour ne pas boucler sur la racine */
				       Retarde (pRegle, pSPres, pP, pAttr, pAb);
				 }
			 }
		       while (!(pRegle == NULL));
		       pPRP = pPRP->AbNextRepeated;
		    }
	       }
#else  /* __COLPAGE__ */
		do
		  {
		     pP = pEl->ElAbstractBox[VueNb - 1];
		     /* saute les paves de presentation crees par FnCreateBefore */
		     stop = False;
		     do
			if (pP == NULL)
			   stop = True;
			else if (!pP->AbPresentationBox)
			   stop = True;
			else if (pP->AbFirstEnclosed != NULL &&
				 pP->AbFirstEnclosed->AbElement == pEl)
			   /* pave cree' par la regle FnCreateEnclosing */
			   stop = True;
			else
			   pP = pP->AbNext;
		     while (!(stop));
		     pPRP = pP;
		     GetRet (&pRegle, &pSPres, &pP, &pAttr);
		     if (pRegle != NULL)
			if (!Applique (pRegle, pSPres, pP, pDoc, pAttr))
			   /* cette regle n'a pas pu etre appliquee           */
			   /* c'est une regle correspondant a un attribut, on */
			   /* l'appliquera lorsque l'englobant sera complet   */
			   Retarde (pRegle, pSPres, pP, pAttr, pPRP);
		  }
		while (!(pRegle == NULL));
#endif /* __COLPAGE__ */
	  }			/* fin de !IgnoreDescendance */
	if (PereAssoc != NULL)
	   /* retablit AbPresentationBox qui a ete modifie' pour une boite de haut */
	   /* ou bas de page qui regroupe des elements associes. */
	   PereAssoc->AbPresentationBox = True;
#ifdef __COLPAGE__
	if (NouvPave != NULL && !NouvPave->AbDead)
	   if (!PaveCorrect (NouvPave))
	      printf ("erreur fin CreePaves \n");
	/* si on a cree une marque de page precedente, il faut */
	/* repositionner pEl a l'ancienne valeur. Utile ? */
	if (pElSauv != NULL)
	   pEl = pElSauv;
	/* creation des elements associes en haut ou bas de page */
	/* si l'element pEl est un element reference vers ce type */
	/* d'element associe et si l'elt associe n'est pas deja cree */
	ElRef = NULL;
	if (pEl->ElAbstractBox[VueNb - 1] != NULL)
	   ElRef = TestElHB (pEl, VueNb);
	if (ElRef != NULL)
	  {
	     /* recherche de l'element marque de page precedent */
	     stop = False;
	     pElPage = pEl;
	     do
	       {
		  pElPage = BackSearchTypedElem (pElPage, PageBreak + 1, NULL);
		  if (pElPage == NULL)
		     stop = True;
		  else if (pElPage->ElViewPSchema == VueSch &&
			   (pElPage->ElPageType == PgBegin ||
			    pElPage->ElPageType == PgComputed ||
			    pElPage->ElPageType == PgUser))
		     stop = True;
	       }
	     while (!stop);
	     if (pElPage != NULL)
	       {
		  CrPavHB (ElRef, pDoc, VueNb, pElPage, EnAvant);
		  if (PaginationEnCours && !ArretAvantCreation)
		    {
		       /* on memorise la reference a l'elt assoc */
		       RefAssocHBPage = pEl;
		       /* et on positionne le booleen TrouveMPHB */
		       /* pour provoquer l'arret de la creation */
		       TrouveMPHB = True;
		    }
	       }
	  }
	if (Retour != NULL && Retour->AbDead)
	   Retour = NULL;
	/* si pagination en cours et que l'element est une marque page */
	/* ou un element reference sur un element associe place en haut */
	/* ou bas de page, on s'arrete si ArretAvantCreation est FAUX */
	/* (c-a-d l'arret se fait apres creation) */
	/* l'arret est remonte de la pile des appels en mettant le */
	/* volume libre negatif (cf. VuePleine) */
	if (PaginationEnCours && !TrouveMPHB && !ArretAvantCreation)
	   if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak
	       && (pEl->ElPageType == PgBegin
		   || pEl->ElPageType == PgComputed
		   || pEl->ElPageType == PgUser)
	       && pEl->ElAbstractBox[VueNb - 1] != NULL)
	      if (pEl->ElViewPSchema == VueSch)
		 TrouveMPHB = True;
	if (PaginationEnCours && TrouveMPHB)
	   /* volume mis a -1 : arret creation */
	   if (VueAssoc (pEl))
	      pDoc->DocAssocFreeVolume[pEl->ElAssocNum - 1] = -1;
	   else
	      pDoc->DocViewFreeVolume[VueNb - 1] = -1;
#endif /* __COLPAGE__ */
     }
   return Retour;
}
