/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Ce module effectue la creation des images abstraites
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "message.h"
#include "language.h"
#include "appdialogue.h"
#include "frame.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "page_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"
#include "modif_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "abspictures_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "presvariables_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structmodif_f.h"
#include "tree_f.h"

/*----------------------------------------------------------------------
   SetAbsBoxAccessMode met a` jour le mode d'acces accessMode sur  
   le pave pAb et tous ses descendants.                    
  ----------------------------------------------------------------------*/
static void SetAbsBoxAccessMode (PtrAbstractBox pAb, int accessMode)
{
  PtrAbstractBox      pAbChild;

  if (accessMode == 0)
    /* read only */
    {
      pAb->AbCanBeModified = FALSE;
      pAb->AbReadOnly = TRUE;
      pAb->AbChange = TRUE;
    }
  else if (!ElementIsReadOnly (pAb->AbElement))
    /* read write */
    /* on laisse en read only si l'element est en read only */
    {
      if (!pAb->AbPresentationBox)
	/* ce n'est pas un pave de presentation, il est donc modifiable */
	pAb->AbCanBeModified = TRUE;
      else if (PresAbsBoxUserEditable (pAb))
	pAb->AbCanBeModified = TRUE;
      pAb->AbReadOnly = FALSE;
      pAb->AbChange = TRUE;
    }

  /* on passe aux fils */
  pAbChild = pAb->AbFirstEnclosed;
  while (pAbChild != NULL)
    {
      SetAbsBoxAccessMode (pAbChild, accessMode);
      pAbChild = pAbChild->AbNext;
    }
}

/*----------------------------------------------------------------------
   SetAccessMode updates the access  mode in all abstract boxes of
  all document views.
  ----------------------------------------------------------------------*/
void SetAccessMode (PtrDocument pDoc, int accessMode)
{
  DisplayMode       displayMode;
  PtrAbstractBox    pAb;
  int               view;
  int               h;

  /* update all document views */
  displayMode = documentDisplayMode[IdentDocument (pDoc) - 1];
  if (displayMode != NoComputedDisplay && displayMode != SuspendDisplay)
    {
      for (view = 0; view < MAX_VIEW_DOC; view++)
	if (pDoc->DocView[view].DvPSchemaView > 0)
	  {
	    pAb = pDoc->DocDocElement->ElAbstractBox[view];
	    SetAbsBoxAccessMode (pAb, accessMode);
	    h = 0;
	    ChangeConcreteImage (pDoc->DocViewFrame[view], &h, pAb);
	  }
      /* Redisplay views */
      if (ThotLocalActions[T_redisplay] != NULL)
	(*ThotLocalActions[T_redisplay]) (pDoc);
    }
}

/*----------------------------------------------------------------------
   GetRule rend la regle de presentation a` appliquer pour la vue  
   numero 1. pRSpecif est la regle specifique courante,    
   pRDefault est la regle par defaut courante. Au retour    
   ces deux pointeurs sont mis a` jour pour la regle       
   suivante.                                               
  ----------------------------------------------------------------------*/
PtrPRule GetRule (PtrPRule *pRSpecif, PtrPRule * pRDefault,
		  PtrElement pEl, PtrAttribute pAttr, PtrSSchema pSS,
		  PtrDocument pDoc)
{
   PtrPRule            pPR;
   ThotBool            stop;

   pPR = NULL;
   if (*pRSpecif != NULL && *pRDefault != NULL)
     {
     /* Il n'y a pas de fonction par defaut. Donc si on est sur une regle */
     /* specifique de type fonction, on la prend, si ses conditions */
     /* d'application sont satisfaites. */
     if ((*pRSpecif)->PrType == PtFunction &&
	 (*pRDefault)->PrType > PtFunction)
	{
	stop = FALSE;
	do
	   {
	   if ((*pRSpecif)->PrCond == NULL ||
	       CondPresentation ((*pRSpecif)->PrCond, pEl, pAttr, pEl, 1, pSS,
				 pDoc))
	      /* conditions d'application satisfaites */
	      {
	      pPR = *pRSpecif;
	      stop = TRUE;
	      }
	   /* prochaine regle specifique a traiter */
	   *pRSpecif = (*pRSpecif)->PrNextPRule;
	   if (!stop)
	     {
	     /* la condition n'est pas satisfaite, on essaie les
		fonctions suivantes */
	     if (*pRSpecif == NULL)
	       /* pas de regle specifique suivante, on arrete */
	       stop = TRUE;
	     else if ((*pRSpecif)->PrType != PtFunction)
	       /* la regle specifique suivante n'est pas une Fonction,
		  on arrete */
	       stop = TRUE;
	     }
	   }
	while (!stop);
	}
     }
   if (pPR == NULL)
     if (*pRDefault != NULL)
	{
	   if (*pRSpecif != NULL)
	      if ((*pRSpecif)->PrType == (*pRDefault)->PrType &&
		  (*pRSpecif)->PrViewNum == 1)
		 /* pour la vue 1, la regle specifique de meme type que la */
		 /* regle par defaut existe */
		{
		   stop = FALSE;
		   /* on traite toutes les regles specifiques successives de ce
		      type et qui concernent la vue 1, et on retient la
		      derniere dont les conditions d'application sont OKs */
		   do
		     {
		        /* on n'a pas encore trouve' la bonne regle */
		        if ((*pRSpecif)->PrCond == NULL ||
			    CondPresentation ((*pRSpecif)->PrCond, pEl,
					      pAttr, pEl, 1, pSS, pDoc))
			   /* les conditions d'application sont satisfaites,
			      on prend cette regle */
			   pPR = *pRSpecif;
			if ((*pRSpecif)->PrNextPRule == NULL)
			   stop = TRUE;
			else if ((*pRSpecif)->PrNextPRule->PrType ==
						     (*pRDefault)->PrType &&
				 (*pRSpecif)->PrNextPRule->PrViewNum == 1)
			   *pRSpecif = (*pRSpecif)->PrNextPRule;
			else
			   stop = TRUE;
		     }
		   while (!stop);
		   /* avance d'une regle */
		   if (*pRSpecif != NULL)
		      *pRSpecif = (*pRSpecif)->PrNextPRule;
		   if (pPR != NULL)
		      *pRDefault = (*pRDefault)->PrNextPRule;
		}
	   if (pPR == NULL)
	      /* on prend la regle par defaut */
	      /* les regles par defaut n'ont pas de condition */
	     {
		pPR = *pRDefault;
		*pRDefault = (*pRDefault)->PrNextPRule;
	     }
	}
   return pPR;
}

/*----------------------------------------------------------------------
   GetRuleView donne la regle du type Typ a` appliquer pour la vue 
   Vue. pRSpecif et pRDefault sont mis a` jour. La fonction 
   retourne NULL si il n'y a pas de regle de ce type pour  
   cette vue (dans ce cas, il faut alors appliquer la      
   regle de la vue 1).                                     
  ----------------------------------------------------------------------*/
PtrPRule GetRuleView (PtrPRule *pRSpecif, PtrPRule * pRDefault,
		      PRuleType Typ, int Vue, PtrElement pEl,
		      PtrAttribute pAttr, PtrSSchema pSS,
		      PtrDocument pDoc)
{
   PtrPRule            pPR;
   ThotBool            stop;

   pPR = NULL;
   if (*pRSpecif != NULL)
     if ((*pRSpecif)->PrViewNum == Vue)
       if ((*pRSpecif)->PrType == Typ)
	  {
	  stop = FALSE;
	  /* on traite toutes les regles successives de meme type et qui */
	  /* concernent la meme vue */
	  do
	    {
	    /* on n'a pas encore trouve' la regle qui s'applique */
	    if ((*pRSpecif)->PrCond == NULL ||
		CondPresentation ((*pRSpecif)->PrCond, pEl, pAttr, pEl,
				  Vue, pSS, pDoc))
	       /* les conditions d'application de la regle sont satisfaites,
		  on prend cette regle */
	       pPR = *pRSpecif;
	    if ((*pRSpecif)->PrNextPRule == NULL)
	       stop = TRUE;
	    else if ((*pRSpecif)->PrNextPRule->PrViewNum == Vue &&
		     (*pRSpecif)->PrNextPRule->PrType == Typ)
	       *pRSpecif = (*pRSpecif)->PrNextPRule;
	    else
	       stop = TRUE;
	    }
	  while (!stop);
	  /* avance d'une regle */
	  if (*pRSpecif != NULL)
	     *pRSpecif = (*pRSpecif)->PrNextPRule;
	  if (*pRDefault != NULL)
	     if ((*pRDefault)->PrType == Typ && (*pRDefault)->PrViewNum == Vue)
		*pRDefault = (*pRDefault)->PrNextPRule;
	  }
   if (pPR == NULL)
      if (*pRDefault != NULL)
	 if ((*pRDefault)->PrType == Typ)
	    if ((*pRDefault)->PrViewNum == Vue)
	       /* on prend la regle par defaut */
	      {
		 pPR = *pRDefault;
		 *pRDefault = (*pRDefault)->PrNextPRule;
	      }
   return pPR;
}

/*----------------------------------------------------------------------
   InitAbsBoxes cree et initialise un pave pour l'element pEl et dans  
   la vue view, avec la visibilite Visib.
   ro est vrai si le document est en ReadOnly
  ----------------------------------------------------------------------*/
PtrAbstractBox InitAbsBoxes (PtrElement pEl, DocViewNumber view, int Visib,
			     ThotBool ro)
{
   PtrAbstractBox      pAb;

   GetAbstractBox (&pAb);
   pAb->AbElement = pEl;
   pAb->AbBox = NULL;
   pAb->AbEnclosing = NULL;
   pAb->AbNext = NULL;
   pAb->AbPrevious = NULL;
   pAb->AbFirstEnclosed = NULL;
   pAb->AbNextRepeated = NULL;
   pAb->AbPreviousRepeated = NULL;
   pAb->AbPSchema = NULL;
   pAb->AbDocView = view;
   pAb->AbCopyDescr = NULL;
   pAb->AbCreatorAttr = NULL;
   pAb->AbDelayedPRule = NULL;
   pAb->AbVolume = 0;
   pAb->AbTypeNum = pEl->ElTypeNumber;
   pAb->AbNum = 0;

   pAb->AbVertRef.PosEdge = VertRef;
   pAb->AbVertRef.PosRefEdge = VertRef;
   pAb->AbVertRef.PosDistance = 0;
   pAb->AbVertRef.PosUnit = UnRelative;
   pAb->AbVertRef.PosAbRef = NULL;
   pAb->AbVertRef.PosUserSpecified = FALSE;

   pAb->AbHorizRef.PosEdge = HorizRef;
   pAb->AbHorizRef.PosRefEdge = HorizRef;
   pAb->AbHorizRef.PosDistance = 0;
   pAb->AbHorizRef.PosUnit = UnRelative;
   pAb->AbHorizRef.PosAbRef = NULL;
   pAb->AbHorizRef.PosUserSpecified = FALSE;

   pAb->AbVertPos.PosEdge = Top;
   pAb->AbVertPos.PosRefEdge = Top;
   pAb->AbVertPos.PosDistance = 0;
   pAb->AbVertPos.PosUnit = UnRelative;
   pAb->AbVertPos.PosAbRef = NULL;
   pAb->AbVertPos.PosUserSpecified = FALSE;

   pAb->AbHorizPos.PosEdge = Left;
   pAb->AbHorizPos.PosRefEdge = Left;
   pAb->AbHorizPos.PosDistance = 0;
   pAb->AbHorizPos.PosUnit = UnRelative;
   pAb->AbHorizPos.PosAbRef = NULL;
   pAb->AbHorizPos.PosUserSpecified = FALSE;

   pAb->AbHeight.DimIsPosition = FALSE;
   pAb->AbHeight.DimValue = -1;
   pAb->AbHeight.DimAbRef = NULL;
   pAb->AbHeight.DimUnit = UnRelative;
   pAb->AbHeight.DimSameDimension = TRUE;
   pAb->AbHeight.DimUserSpecified = FALSE;
   pAb->AbHeight.DimMinimum = FALSE;

   pAb->AbWidth.DimIsPosition = FALSE;
   pAb->AbWidth.DimValue = -1;
   pAb->AbWidth.DimAbRef = NULL;
   pAb->AbWidth.DimUnit = UnRelative;
   pAb->AbWidth.DimSameDimension = TRUE;
   pAb->AbWidth.DimUserSpecified = FALSE;
   pAb->AbWidth.DimMinimum = FALSE;

   pAb->AbUnderline = 0;
   pAb->AbThickness = 0;
   pAb->AbIndent = 0;
   pAb->AbDepth = 0;
   pAb->AbVisibility = Visib;
   pAb->AbFontStyle = 0;
   pAb->AbFontWeight = 0;
   pAb->AbSize = 1;
   pAb->AbLineWeight = 1;
   pAb->AbLineSpacing = 10;
   pAb->AbFillPattern = 0;
   pAb->AbBackground = DefaultBColor;	/* blanc */
   pAb->AbForeground = DefaultFColor;	/* noir */

   pAb->AbTopBColor = -1;
   pAb->AbRightBColor = -1;
   pAb->AbBottomBColor = -1;
   pAb->AbLeftBColor = -1;
   pAb->AbTopStyle = 1;
   pAb->AbRightStyle = 1;
   pAb->AbBottomStyle = 1;
   pAb->AbLeftStyle = 1;

   pAb->AbTopMargin = 0;
   pAb->AbRightMargin = 0;
   pAb->AbBottomMargin = 0;
   pAb->AbLeftMargin = 0;
   pAb->AbTopPadding = 0;
   pAb->AbRightPadding = 0;
   pAb->AbBottomPadding = 0;
   pAb->AbLeftPadding = 0;
   pAb->AbTopBorder = 0;
   pAb->AbRightBorder = 0;
   pAb->AbBottomBorder = 0;
   pAb->AbLeftBorder = 0;

   pAb->AbFont = 1;
   pAb->AbLineStyle = 'S';
   pAb->AbAdjust = AlignLeft;
   pAb->AbSizeUnit = UnRelative;
   pAb->AbIndentUnit = UnRelative;
   pAb->AbLineWeightUnit = UnPoint;
   pAb->AbLineSpacingUnit = UnRelative;

   pAb->AbTopMarginUnit = UnRelative;
   pAb->AbRightMarginUnit = UnRelative;
   pAb->AbBottomMarginUnit = UnRelative;
   pAb->AbLeftMarginUnit = UnRelative;
   pAb->AbTopPaddingUnit = UnRelative;
   pAb->AbRightPaddingUnit = UnRelative;
   pAb->AbBottomPaddingUnit = UnRelative;
   pAb->AbLeftPaddingUnit = UnRelative;
   pAb->AbTopBorderUnit = UnPoint;
   pAb->AbRightBorderUnit = UnPoint;
   pAb->AbBottomBorderUnit = UnPoint;
   pAb->AbLeftBorderUnit = UnPoint;

   pAb->AbAcceptLineBreak = TRUE;
   pAb->AbAcceptPageBreak = TRUE;
   pAb->AbHyphenate = FALSE;
   pAb->AbOnPageBreak = FALSE;
   pAb->AbAfterPageBreak = FALSE;
   pAb->AbNotInLine = FALSE;
   pAb->AbHorizEnclosing = TRUE;
   pAb->AbVertEnclosing = TRUE;

   /* les copies d'elements inclus ne sont pas modifiables par l'utilisateur */
   pAb->AbCanBeModified = !pEl->ElIsCopy;
   /* les constantes ne sont pas modifiables par l'utilisateur */
   if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct ==
       CsConstant)
      pAb->AbCanBeModified = FALSE;
   pAb->AbSelected = FALSE;
   pAb->AbBox = NULL;
   pAb->AbPresentationBox = FALSE;
   pAb->AbRepeatedPresBox = FALSE;
   pAb->AbSensitive = FALSE;
   /* cherche si l'element est une reference ou une copie d'element inclus */
   if (pEl->ElTerminal)
      if (pEl->ElLeafType == LtReference)
	 /* une reference = un pave actif (double-cliquable) */
	 pAb->AbSensitive = TRUE;
   if (pEl->ElIsCopy)
      /* un element appartenant a un element copie' = pave actif */
      pAb->AbSensitive = TRUE;
   pAb->AbReadOnly = FALSE;
   if (ro || ElementIsReadOnly (pEl))
     {
	pAb->AbCanBeModified = FALSE;
	pAb->AbReadOnly = TRUE;
     }
   pAb->AbNew = TRUE;
   pAb->AbDead = FALSE;
   pAb->AbWidthChange = FALSE;
   pAb->AbHeightChange = FALSE;
   pAb->AbHorizPosChange = FALSE;
   pAb->AbVertPosChange = FALSE;
   pAb->AbHorizRefChange = FALSE;
   pAb->AbVertRefChange = FALSE;
   pAb->AbSizeChange = FALSE;
   pAb->AbAspectChange = FALSE;
   pAb->AbMBPChange = FALSE;
   pAb->AbChange = FALSE;

   pAb->AbInLine = TRUE;
   pAb->AbText = NULL;

   if (pEl->ElTerminal && pEl->ElLeafType == LtText)
     {
	pAb->AbLeafType = LtText;
	pAb->AbLang = TtaGetDefaultLanguage ();
     }
   return pAb;
}

/*----------------------------------------------------------------------
   ConstantCopy met dans le pave pAb le text correspondant a`  
   la constante de numero NConst dans le schema de         
   presentation pSchP.                                     
  ----------------------------------------------------------------------*/
void ConstantCopy (int NConst, PtrPSchema pSchP, PtrAbstractBox pAb)
{
  PresConstant       *pConst;
  int                 l;

  pConst = &pSchP->PsConstant[NConst - 1];
  switch (pConst->PdType)
    {
    case GraphicElem:
      pAb->AbLeafType = LtGraphics;
      pAb->AbShape = (char)pConst->PdString[0];
      if (pAb->AbShape == 'C')
	/* rectangle with rounded corners */
	{
	  pAb->AbRx = 5;
	  pAb->AbRxUnit = UnPoint;
	  pAb->AbRy = 5;
	  pAb->AbRyUnit = UnPoint;
	}
      pAb->AbGraphScript = 'L';
      pAb->AbVolume = 1;
      break;
    case Symbol:
      pAb->AbLeafType = LtSymbol;
      pAb->AbShape = (char) pConst->PdString[0];
      pAb->AbGraphScript = 'G';
      if (pAb->AbShape == EOS)
	pAb->AbVolume = 0;
      else
	pAb->AbVolume = 1;
      break;
    case CharString:
      pAb->AbLeafType = LtText;
      if (pAb->AbText == NULL)
	GetConstantBuffer (pAb);
      CopyStringToBuffer (pConst->PdString, pAb->AbText, &l);
      pAb->AbLang = TtaGetDefaultLanguage ();
      pAb->AbVolume = pAb->AbText->BuLength;
      break;
    case Picture:
      NewPictInfo (pAb, pConst->PdString, UNKNOWN_FORMAT);
      pAb->AbLeafType = LtPicture;
      pAb->AbVolume = 100;
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
   AttrIsAfter retourne vrai si attribut pAttr est un attribut qui suit
   l'attribut pRefAttr.
  ----------------------------------------------------------------------*/
static ThotBool AttrIsAfter (PtrAttribute pAttr, PtrAttribute pRefAttr)
{
  PtrAttribute        pA;

  if (pAttr == NULL || pRefAttr == NULL)
    return FALSE;
  else
    {
      pA = pRefAttr->AeNext;
      while (pA != NULL && pA != pAttr)
	pA = pA->AeNext;
      if (pA == NULL)
	return FALSE;
      else
	return TRUE;
    }
}

/*----------------------------------------------------------------------
   DoesViewExist retourne vrai si la vue de numero viewNb ou` doit      
   s'afficher l'element pEl existe.                        
  ----------------------------------------------------------------------*/
ThotBool DoesViewExist (PtrElement pEl, PtrDocument pDoc, DocViewNumber viewNb)
{
  /* la vue existe si l'entree correspondante de la table des vues
     du document n'est pas libre. */
  return (pDoc->DocView[viewNb - 1].DvPSchemaView > 0);
}

/*----------------------------------------------------------------------
   GetGestView retourne le numero de la vue (numero dans le      
   schema de presentation qui s'applique au schema de structure pSS)
   dont il faut appliquer les regles de presentation aux elements
   et attributs definis dans le schema pSS pour la vue de document
   decrite par pView.  On tient compte de la clause "MERGE With"
   du schema de presentation.
  ----------------------------------------------------------------------*/
static int GetGestView (DocViewDescr *pView, PtrSSchema pSS)
{
    PtrGuestViewDescr  pGuestView;
    int                viewSch;
    ThotBool           found;

    /* par defaut, on applique les regles de presentation de la vue 1 */
    viewSch = 1;
    /* on cherche dans les guest views */
    pGuestView = pView->DvFirstGuestView;
    found = FALSE;
    while (pGuestView && !found)
       if (!strcmp (pGuestView->GvSSchema->SsName, pSS->SsName))
	  {
	  found = TRUE;
	  viewSch = pGuestView->GvPSchemaView;
	  }
       else
          pGuestView = pGuestView->GvNextGuestView;
    return viewSch;
}

/*----------------------------------------------------------------------
   AppliedView retourne le numero de la vue (numero dans le      
   schema de presentation qui s'applique a` l'element pEl  
   ou a` l'attribut pAttr) dont il faut appliquer les      
   regles de presentation a` l'element pEl pour obtenir    
   son image dans la vue viewNb du document pDoc.           
  ----------------------------------------------------------------------*/
int AppliedView (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc,
		 DocViewNumber viewNb)
{
   int                 viewSch;
   PtrElement          pAsc;
   PtrSSchema          pSS;
   DocViewDescr       *pView;

   viewSch = 0;
   if (pDoc->DocView[viewNb - 1].DvPSchemaView != 0)
      {
      if (pAttr != NULL)
	 pSS = pAttr->AeAttrSSchema;
      else if (pEl != NULL)
	 pSS = pEl->ElStructSchema;
      else
	 pSS = NULL;
      if (pSS != NULL)
	 {
	 pView = &pDoc->DocView[viewNb - 1];
	 if (pView->DvSSchema == pDoc->DocSSchema)
	    /* c'est une vue du document lui-me^me */
	    if (!strcmp (pSS->SsName, pDoc->DocSSchema->SsName))
	       /* c'est un element du document lui-meme. On prend le numero
		  de la vue dans le schema de presentation qui la definit */
	       viewSch = pView->DvPSchemaView;
	    else
	       /* c'est un element d'un objet d'une autre nature */
	       viewSch = GetGestView (pView, pSS);
	 else
	    /* c'est une vue propre a une nature d'objets */
	    if (pEl->ElParent == NULL ||
		PresentationSchema (pSS, pDoc) ==
		PresentationSchema (pView->DvSSchema, pDoc))
	       /* c'est un objet de cette nature ou la racine du document */
	       /* numero de la vue dans le schema de presentation qui la
		  definit */
	       viewSch = pView->DvPSchemaView;
	    else
	       /* l'element est-il dans un objet de cette nature ? */
	       /* Si ce n'est pas le cas, il ne doit pas apparaitre dans */
	       /* cette vue */
	       {
	       pAsc = pEl;
	       while (pAsc != NULL && viewSch == 0)
		  if (PresentationSchema (pAsc->ElStructSchema, pDoc) ==
		      PresentationSchema (pView->DvSSchema, pDoc))
		     /* on est dans un objet de cette nature */
		     viewSch = GetGestView (pView, pSS);
		  else 
		     pAsc = pAsc->ElParent;
	       }
	 }
      }
   return viewSch;
}

/*----------------------------------------------------------------------
   Delay met une regle de presentation en attente au niveau du   
   pave englobant le pave pPRP. Cette regle sera           
   reappliquee lorsque tous les paves freres de pPRP auront
   ete crees.                                              
   - pR: SRule de presentation a` mettre en attente.       
   - pSP: schema de presentation auquel appartient cette   
   regle.                                                  
   - pAbb: pave auquel doit s'appliquer la regle pR.         
   - pAttr: attribut auquel correspond cette regle ou NULL 
   si ce n'est pas une regle de presentation d'attribut.   
   Si l'englobant est un pave page ou un pave duplique,    
   on met la regle en attente au niveau de la racine.      
  ----------------------------------------------------------------------*/
void Delay (PtrPRule pR, PtrPSchema pSP, PtrAbstractBox pAbb,
	    PtrAttribute pAttr, PtrAbstractBox pPRP)
{
   PtrDelayedPRule     pDelR;
   PtrDelayedPRule     NpDelR;
   PtrAbstractBox      pAb;

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

   if (pAb != NULL)
     {
	GetDifferedRule (&NpDelR);
	NpDelR->DpPRule = pR;
	NpDelR->DpPSchema = pSP;
	NpDelR->DpAbsBox = pAbb;
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

/*----------------------------------------------------------------------
   GetDelayedRule recupere une regle de presentation qui etait retardee    
   au niveau du pave pAbb. Au retour pR contient la regle    
   retardee, pSP contient le schema de presentation auquel 
   appartient cette regle, pAbb contient le pave auquel      
   s'applique la regle retardee et pAttr contient          
   l'attribut auquel correspond la regle, si c'est une     
   regle d'attribut (NULL sinon). S'il n'y a pas de regle  
   retardee, pR et pAbb contiennent NULL au retour.          
  ----------------------------------------------------------------------*/
void GetDelayedRule (PtrPRule * pR, PtrPSchema * pSP, PtrAbstractBox * pAbb,
		     PtrAttribute * pAttr)
{
   PtrDelayedPRule     pDelR;
   PtrAbstractBox      pAb2;
   ThotBool            stop;
   PtrElement          pEl;

   pAb2 = *pAbb;
   /* a priori, pas de regle retardee */
   *pR = NULL;
   *pSP = NULL;
   *pAbb = NULL;
   *pAttr = NULL;
   if (pAb2 != NULL)
     {
	/* cherche le pave de l'element dans cette vue */
	/* saute les paves de presentation */
	stop = FALSE;
	pEl = pAb2->AbElement;
	do
	   if (pAb2 == NULL)
	      stop = TRUE;
	   else if (pAb2->AbElement != pEl)
	     {
		stop = TRUE;
		pAb2 = NULL;
	     }
	   else if (!pAb2->AbPresentationBox)
	      stop = TRUE;
	   else
	      pAb2 = pAb2->AbNext;
	while (!stop);
	if (pAb2 != NULL)
	  {
	     pDelR = pAb2->AbDelayedPRule;
	     if (pDelR != NULL)
	       {
		  *pR = pDelR->DpPRule;
		  *pSP = pDelR->DpPSchema;
		  *pAbb = pDelR->DpAbsBox;
		  *pAttr = pDelR->DpAttribute;
		  pAb2->AbDelayedPRule = pDelR->DpNext;
		  FreeDifferedRule (pDelR);
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   ApplDelayedRule applique les regles retardees conservees pour  
   les paves de l'element El du document pDoc.             
  ----------------------------------------------------------------------*/
void ApplDelayedRule (PtrElement pEl, PtrDocument pDoc)
{
   PtrPRule            pRule;
   PtrPSchema          pSPres;
   PtrAttribute        pAttr;
   PtrAbstractBox      pAb, pAbb;
   int                 view;

   if (pEl == NULL || pDoc == NULL)
      return;
   for (view = 0; view < MAX_VIEW_DOC; view++)
     {
	pAb = pEl->ElAbstractBox[view];
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
		     /* la procedure ApplyRule modifie pAb, on le retablit */
		     pAbb = pAb;
		     GetDelayedRule (&pRule, &pSPres, &pAbb, &pAttr);
		     if (pRule != NULL)
			ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr);
		  }
		while (pRule != NULL);
	     }
     }
}

/*----------------------------------------------------------------------
   CondPresentation evalue les conditions d'application d'une regle de    
   presentation qui s'applique a` l'element pEl ou a l'attribut pAttr
   pour la vue de numero View.                              
   pCond est la premiere condition de la chaine des conditions qui
   s'appliquent a la regle de presentation. 
   pElAttr est l'element qui porte l'attribut pAttr, si pAttr != NULL.
   pSS est le schema de structure correspondant au schema de presentation
   auquel appartient la regle.             
   Retourne vrai si les conditions sont toutes satisfaites.
  ----------------------------------------------------------------------*/
ThotBool            CondPresentation (PtrCondition pCond, PtrElement pEl,
				      PtrAttribute pAttr, PtrElement pElAttr,
				      int view, PtrSSchema pSS,
				      PtrDocument pDoc)
{
  ThotBool            ok, currentCond, stop, equal;
  int                 valcompt, valmaxi, valmini, i;
  PtrPSchema          pSchP = NULL;
  PtrElement          pElSibling, pAsc, pElem, pRoot;
  PtrReference        pRef;
  DocumentIdentifier  IDocExt;
  PtrDocument         pDocExt;
  PtrAttribute        pA;
  unsigned char       attrVal[MAX_TXT_LEN];

  /* a priori les conditions sont satisfaites */
  ok = TRUE;
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
	      pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrType ==
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
	ok = FALSE;
      else if (pCond->CoCondition == PcEven || pCond->CoCondition == PcOdd ||
	       pCond->CoCondition == PcOne || pCond->CoCondition == PcInterval)
	/* evalue le compteur */
	{
	  pSchP = PresentationSchema (pSS, pDoc);
	  if (pSchP != NULL)
	    valcompt = CounterVal (pCond->CoCounter, pSS, pSchP, pElem, view);
	}
      if (pElem != NULL)
	switch (pCond->CoCondition)
	  {

	  case PcFirst:
	    /* on saute les marques de page precedentes */
	    pElSibling = pElem->ElPrevious;
	    stop = FALSE;
	    do
	      if (pElSibling == NULL)
		stop = TRUE;
	      else if (pElSibling->ElTypeNumber == PageBreak + 1)
		pElSibling = pElSibling->ElPrevious;
	      else
		stop = TRUE;
	    while (!stop);
	    currentCond = pElSibling == NULL;
	    break;

	  case PcLast:
	    /* on saute les marques de page suivantes */
	    pElSibling = pElem->ElNext;
	    stop = FALSE;
	    do
	      if (pElSibling == NULL)
		stop = TRUE;
	      else if (pElSibling->ElTypeNumber == PageBreak + 1)
		pElSibling = pElSibling->ElNext;
	      else
		stop = TRUE;
	    while (!stop);
	    currentCond = pElSibling == NULL;
	    /* traitement particulier pour les lignes de tableau */
	    if (ThotLocalActions[T_condlast] != NULL)
	      (*ThotLocalActions[T_condlast]) (pElem, &currentCond);
	    break;
       
	  case PcReferred:
	    /* la condition est satisfaite si l'element (ou le */
	    /* premier de ses ascendants sur lequel peut porter une */
	    /* reference) est reference' au moins une fois. */
	    currentCond = FALSE;
	    pAsc = pElem;
	    do
	      {
		if (pAsc->ElReferredDescr != NULL)
		  /* l'element est reference' */
		  currentCond = TRUE;
		if (!currentCond)
		  /* l'element n'est pas reference' */
		  /* on examine l'element ascendant */
		  pAsc = pAsc->ElParent;
	      }
	    while (pAsc != NULL && !currentCond);
	    break;

	  case PcFirstRef:
	  case PcLastRef:
	    /* la condition est satisfaite s'il s'agit de la premiere ou
	       de la derniere reference a l'element reference' */
	    pRef = NULL;
	    if (pAttr != NULL &&
		pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrType == AtReferenceAttr)
	      /* c'est un attribut reference */
	      pRef = pAttr->AeAttrReference;
	    else if (pElem->ElTerminal && pElem->ElLeafType == LtReference)
	      /* l'element est une reference */
	      pRef = pElem->ElReference;
	    if (pRef != NULL)
	      if (pCond->CoCondition == PcFirstRef)
		currentCond = pEl->ElReference->RdPrevious == NULL;
	      else
		currentCond = pEl->ElReference->RdNext == NULL;
	    else
	      currentCond = FALSE;
	    break;

	  case PcExternalRef:
	  case PcInternalRef:
	    /* la condition est satisfaite s'il s'agit d'un */
	    /* element ou d'un attribut reference externe (ou interne) */
	    pRef = NULL;
	    currentCond = FALSE;
	    if (pAttr != NULL &&
		pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrType == AtReferenceAttr)
	      /* c'est un attribut reference */
	      pRef = pAttr->AeAttrReference;
	    else if (pElem->ElTerminal && pElem->ElLeafType == LtReference)
	      /* l'element est-il une reference ? */
	      pRef = pElem->ElReference;
	    else
	      /* c'est peut-etre une inclusion */
	      pRef = pElem->ElSource;
	    if (pRef == NULL)
	      currentCond = FALSE;
	    else if (pCond->CoCondition == PcInternalRef)
	      currentCond = pRef->RdInternalRef;
	    else
	      currentCond = !pRef->RdInternalRef;
	    break;

	  case PcCopyRef:
	    /* la condition est satisfaite si l'element est une copie */
	    currentCond = pElem->ElIsCopy;
	    break;

	  case PcAnyAttributes:
	    /* la condition est satisfaite si l'element */
	    /* porte des attributs */
	    currentCond = pElem->ElFirstAttr != NULL;
	    break;

	  case PcFirstAttr:
	    /* TODO */
	    /* la condition est satisfaite si le bloc */
	    /* attribut pAttr est le 1er de l'element */
	    if (pAttr && pElAttr)
	      currentCond = pAttr == pElAttr->ElFirstAttr;
	    break;

	  case PcLastAttr:
	    /* TODO */
	    /* la condition est satisfaite si le bloc     */
	    /* attribut pAttr est le dernier de l'element */
	    if (pAttr)
	      currentCond = pAttr->AeNext == NULL;
	    break;

	  case PcUserPage:
	    /* la condition est satisfaite si l'element
	       est un saut de page utilisateur */
	    if (pElem->ElTypeNumber == PageBreak + 1)
	      currentCond = pElem->ElPageType == PgUser;
	    break;

	  case PcStartPage:
	    /* la condition est satisfaite si l'element
	       est un saut de page de debut */
	    if (pElem->ElTypeNumber == PageBreak + 1)
	      currentCond = pElem->ElPageType == PgBegin;
	    break;

	  case PcComputedPage:
	    /* la condition est satisfaite si l'element
	       est un saut de page calcule */
	    if (pElem->ElTypeNumber == PageBreak + 1)
	      currentCond = pElem->ElPageType == PgComputed;
	    break;

	  case PcEmpty:
	    /* la condition est satisfaite si l'element est vide */
	    if (pElem->ElTerminal)
	      if (pElem->ElLeafType == LtReference)
		currentCond = pElem->ElReference == NULL;
	      else
		currentCond = pElem->ElVolume == 0;
	    else
	      currentCond = pElem->ElFirstChild == NULL;
	    break;

	  case PcRoot:
	    /* la condition est satisfaite si le parent de l'element est
	       le document lui-meme */
	    currentCond = (pElem->ElParent &&
			   pElem->ElParent->ElTypeNumber ==
			   pElem->ElParent->ElStructSchema->SsDocument);
	    break;

	  case PcEven:
	    currentCond = !(valcompt & 1);
	    break;

	  case PcOdd:
	    currentCond = (valcompt & 1);
	    break;

	  case PcOne:
	    currentCond = (valcompt == 1);
	    break;

	  case PcInterval:
	    if (pCond->CoValCounter == CntMinVal)
	      {
		/* Calcule la valeur mini du compteur */
		valmini = CounterValMinMax (pCond->CoCounter, pSS, pSchP,
					    pElem, view, FALSE);
		currentCond = (valmini <= pCond->CoMaxCounter) &&
		  (valmini >= pCond->CoMinCounter);
	      }
	    else if (pCond->CoValCounter == CntMaxVal)
	      {
		/* Calcule la valeur maxi du compteur */
		valmaxi = CounterValMinMax (pCond->CoCounter, pSS, pSchP,
					    pElem, view, TRUE);
		currentCond = (valmaxi <= pCond->CoMaxCounter) &&
		  (valmaxi >= pCond->CoMinCounter);
	      }
	    else
	      /* Calcule la valeur courante du compteur */
	      currentCond = (valcompt <= pCond->CoMaxCounter) &&
		(valcompt >= pCond->CoMinCounter);
	    break;

	  case PcWithin:
	    /* condition sur le nombre d'ancetres d'un type donne' */
	    pAsc = pElem->ElParent;
	    if (pAsc == NULL)
	      /* aucun ancetre, condition non satisfaite */
	      currentCond = FALSE;
	    else
	      {
		i = 0;
		if (pCond->CoImmediate)
		  /* Condition: If immediately within n element-type */
		  /* Les n premiers ancetres successifs doivent etre du */
		  /* type CoTypeAncestor, sans comporter d'elements */
		  /* d'autres type */
		  /* on compte les ancetres successifs de ce type */
		  while (pAsc != NULL)
		    {
		      if (pCond->CoTypeAncestor != 0)
			equal = (pAsc->ElTypeNumber == pCond->CoTypeAncestor &&
				 !strcmp (pAsc->ElStructSchema->SsName,
					  pSS->SsName));
		      else
			equal = (!strcmp (pCond->CoAncestorName,
					   pAsc->ElStructSchema->SsRule->SrElem[pAsc->ElTypeNumber - 1]->SrName) &&
				 !strcmp (pCond->CoSSchemaName,
					  pAsc->ElStructSchema->SsName));
		      if (equal)
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
			equal = (pAsc->ElTypeNumber == pCond->CoTypeAncestor &&
				 !strcmp (pAsc->ElStructSchema->SsName,
					  pSS->SsName));
		      else
			equal = (!strcmp (pCond->CoAncestorName,
					  pAsc->ElStructSchema->SsRule->SrElem[pAsc->ElTypeNumber - 1]->SrName) &&
				 !strcmp (pCond->CoSSchemaName,
					  pAsc->ElStructSchema->SsName)); 
		      if (equal)
			i++;
		      pAsc = pAsc->ElParent;  /* passe a l'element ascendant */
		    }
		if (pCond->CoAncestorRel == CondEquals)
		  currentCond = i == pCond->CoRelation;
		else if (pCond->CoAncestorRel == CondGreater)
		  currentCond = i > pCond->CoRelation;
		else if (pCond->CoAncestorRel == CondLess)
		  currentCond = i < pCond->CoRelation;
	      }
	    break;

	  case PcElemType:
	    /* verifie si l'attribut est attache' a un element du
	       type voulu */
	    currentCond = (pElAttr->ElTypeNumber == pCond->CoTypeElem);
	    break;

	  case PcInheritAttribute:
	    /* verifie si l'element ou un de ses ascendants possede cet
	       attribut, eventuellement avec la valeur voulue */
	    pAsc = pElem;
	    currentCond = FALSE;
	    while (pAsc)
	      {
		pA = pAsc->ElFirstAttr;
		while (pA)
		  /* boucle sur les attributs de l'element */
		  {
		    if (pA->AeAttrNum != pCond->CoTypeAttr ||
			!strcmp (pA->AeAttrSSchema->SsName, pSS->SsName) == 0)
		      /* that's not the attribute we are looking for */
		      /* check next attribute of that ancestor */
		      pA = pA->AeNext; 
		    else
		      /* that's the attribute we are looking for */
		      {
			if (!pCond->CoTestAttrValue)
			  /* we don't care about the attribute value */
			  currentCond = TRUE;
			else
			  /* test the attribute value */
			  {
			    if (pA->AeAttrType == AtTextAttr)
			      /* it's a text attribute. Compare strings */
			      {
				if (!pA->AeAttrText)
				  currentCond = (pCond->CoAttrTextValue[0] == EOS);
				else
				  {
				    CopyBuffer2MBs (pA->AeAttrText, 0, attrVal,
						    MAX_TXT_LEN);
				    currentCond = !strcmp (pCond->CoAttrTextValue,
							   attrVal);
				  }
			      }
			    else
			      currentCond = (pCond->CoAttrValue == pA->AeAttrValue);
			  }
			/* don't check other attributes for this element */
			pA = NULL;
			if (currentCond)
			  /* don't look further */
			  pAsc = NULL;
		      }
		  }
		/* if the attribute has not been encountered yet, check next
		   ancestor */
		if (pAsc)
		  pAsc = pAsc->ElParent; 
	      } 
	    break;

	  case PcAttribute:
	    /* verifie si l'element possede cet attribut */
	    pA = pElem->ElFirstAttr;
	    currentCond = FALSE;
	    while (pA != NULL && !currentCond)
	      /* boucle sur les attributs de l'element */
	      {
		if (pA->AeAttrNum == pCond->CoTypeAttr &&
		    strcmp (pA->AeAttrSSchema->SsName, pSS->SsName) == 0)
		  /* that's the attribute we are looking for */
		  {
		    if (!pCond->CoTestAttrValue)
		      /* we don't care about the attribute value */
		      currentCond = TRUE;
		    else
		      /* test the attribute value */
		      {
			if (pA->AeAttrType == AtTextAttr)
			  /* it's a text attribute. Compare strings */
			  {
			    if (!pA->AeAttrText)
			      currentCond = (pCond->CoAttrTextValue[0] == EOS);
			    else
			      {
				CopyBuffer2MBs (pA->AeAttrText, 0, attrVal,
						MAX_TXT_LEN);
				currentCond = !strcmp (pCond->CoAttrTextValue,
						       attrVal);
			      }
			  }
			else
			  currentCond = (pCond->CoAttrValue == pA->AeAttrValue);
		      } 
		    pA = NULL;
		  }
		else
		  pA = pA->AeNext;		/* attribut suivant */
	      }
	    /* as it's impossible to set an attribute to the PAGE */
	    if (!currentCond && pElem->ElTypeNumber == PageBreak + 1)
	      {
		/* get the root element */
		if (pElem->ElParent && pElem->ElParent->ElTypeNumber ==
		    pElem->ElParent->ElStructSchema->SsDocument)
		  /* parent of PAGE element is the document element */
		  {
		    /* get the root element from the children of the
		       document element */
		    pRoot = pElem->ElParent->ElFirstChild;
		    while (pRoot && 
			   (pRoot->ElTypeNumber !=
			    pElem->ElStructSchema->SsRootElem ||
			    pRoot->ElStructSchema != pElem->ElStructSchema))
		      pRoot = pRoot->ElNext;
		  }
		else
		  {
		    pAsc = pElem;
		    pRoot = pElem;
		    while (pAsc->ElParent != NULL)
		      {
			pRoot = pAsc;
			pAsc = pAsc->ElParent;
		      }
		  }
		pA = pRoot->ElFirstAttr;
		/* check the list of attributes of the root element */
		while (pA != NULL && !currentCond)
		  /* boucle sur les attributs de l'element */
		  {
		    if (pA->AeAttrNum == pCond->CoTypeAttr)
		      {
			if (!pCond->CoTestAttrValue)
			  /* we don't care about the attribute value */
			  currentCond = TRUE;
			else
			  /* test the attribute value */
			  {
			    if (pA->AeAttrType == AtTextAttr)
			      /* it's a text attribute. Compare strings */
			      {
				if (!pA->AeAttrText)
				  currentCond = (pCond->CoAttrTextValue[0] == EOS);
				else
				  {
				    CopyBuffer2MBs (pA->AeAttrText, 0, attrVal,
						    MAX_TXT_LEN);
				    currentCond = !strcmp (pCond->CoAttrTextValue,
							   attrVal);
				  }
			      }
			    else
			      currentCond = (pCond->CoAttrValue == pA->AeAttrValue);
			  } 
			pA = NULL;
		      }
		    else
		      pA = pA->AeNext;	/* attribut suivant */
		  }
	      }
	    break;

	  case PcNoCondition:
	    currentCond = TRUE;
	    break;
	    
	  case PcDefaultCond:
	    currentCond = TRUE;
	    break;
	  }

      if (!pCond->CoNotNegative)
	currentCond = !currentCond;
      ok = ok && currentCond;
      pCond = pCond->CoNextCondition;
    }

  return ok;
}

/*----------------------------------------------------------------------
  CrAbsBoxesPres applique a` la vue viewNb la regle de creation de boite
  de presentation pRCre dans le document pDoc, pour l'element pEl.
  Cette regle vient du schema de presentation associe au schema de structure
  pSS.
  completeCreator indique si toutes les regles de presentation ont deja ete
  appliquees au pave createur.
  Retourne le pave cree'.
  ----------------------------------------------------------------------*/
PtrAbstractBox CrAbsBoxesPres (PtrElement pEl, PtrDocument pDoc,
			       PtrPRule pRCre, PtrSSchema pSS,
			       PtrAttribute pAttr, DocViewNumber viewNb,
			       PtrPSchema pSchP, ThotBool completeCreator)
{
  PtrPRule            pRD, pRS;
  PtrPRule            pR, pR1, pRV;
  PtrAbstractBox      pAb, pAbb1, pAbbNext;
  PtrAbstractBox      pAbbCreated;
  PtrElement          pE, pER, pElSibling;
  PtrPRule            queuePR[MAX_QUEUE_LEN];
  PtrPSchema          pSP;
  PtrAttribute        pSelAttr;
  PtrPresentationBox  pBox;
  FunctionType        funct;
  TypeUnit            unit;
  PictInfo           *image;
  int                 view, vis;
  int                 viewSch, viewIndex;
  int                 lqueue, pqueue;
  ThotBool            ok, stop, volok;

  pAbbCreated = NULL;
  pAb = NULL;
  ok = FALSE;
  pER = NULL;
  viewIndex = viewNb - 1;
  viewSch = 0;

  if (DoesViewExist (pEl, pDoc, viewNb))	/* la vue existe */
    {
      viewSch = AppliedView (pEl, pAttr, pDoc, viewNb);
      /* faut-il reellement creer ce pave ? */
      if (pRCre->PrCond == NULL)
	ok = TRUE;
      else
	/* On verifie les conditions d'application de la regle de creation */
	ok = CondPresentation (pRCre->PrCond, pEl, pAttr, pEl, viewSch, pSS,
			       pDoc);
    }
  /* on ne cree un pave de presentation que si le pave de l'element qui */
  /* provoque la creation existe dans la vue. */
  if (ok)
    if (pEl->ElAbstractBox[viewIndex] == NULL)
      ok = FALSE;
  /* on ne cree pas de pave fils pour un element holophraste' ou une feuille */
  funct = pRCre->PrPresFunction;
  if (ok)
    if (pEl->ElHolophrast || pEl->ElTerminal)
      if (funct == FnCreateFirst || funct == FnCreateLast)
	ok = FALSE;
  /* on ne cree pas de pave de presentation qui soit un frere ou le pere du */
  /* pave racine de la vue. */
  if (ok)
    if (pEl->ElAbstractBox[viewIndex]->AbEnclosing == NULL)
       switch (funct)
	 {
	 case FnCreateBefore:
	   funct = FnCreateFirst;
	   break;
	 case FnCreateAfter:
	   funct = FnCreateLast;
	   break;
	 case FnCreateWith:
           ok = FALSE;
	   break;
	 case FnCreateEnclosing:
           ok = FALSE;
	   break;
	 default:
	   break;
	 }
  /* si c'est une boite de haut de page et qu'il s'agit de la derniere */
  /* marque de page du document, on ne cree pas la boite */
  if (ok)
    if (pSchP->PsPresentBox->PresBox[pRCre->PrPresBox[0] - 1]->PbPageHeader)
      /* c'est une boite de haut de page */
      {
	pE = pEl;
	do
	  /* cherche le 1er frere suivant de pE qui ne */
	  /* soit pas un saut de page */
	  {
	    pElSibling = pE->ElNext;
	    stop = FALSE;
	    do
	      if (pElSibling == NULL)
		{
		  stop = TRUE;
		  /* c'etait le dernier frere */
		}
	      else
		{
		  /* il y a un frere suivant */
		  if (pElSibling->ElTerminal && pElSibling->ElTypeNumber == PageBreak + 1)
		    /* c'est une marque de page, on passe au suivant */
		    pElSibling = pElSibling->ElNext;
		  else
		    /* ce n'est pas une marque de page, on a trouve' */
		    stop = TRUE;
		}
	    while (!stop);

	    if (pElSibling == NULL)
	      /* on n'a pas trouve de suivant qui ne soit */
	      /* pas une marque de page */
	      {
		pE = pE->ElParent;
		/* cherche au niveau superieur */
		if (pE == NULL)
		  /* on est a la racine, c'est donc la */
		  /* derniere marque de page du document */
		  ok = FALSE;
		/* on ne cree pas la boite */
	      }
	    else
	      /* il y a un suivant, on arrete */
	      pE = NULL;
	  }
	while (pE != NULL);
      }

  if (ok)
    /* s'il s'agit d'une vue affichant seulement un sous-arbre, on */
    /* ne cree pas les paves de presentation des elements ascendants */
    /* du sous-arbre en question. */
    if (pDoc->DocViewSubTree[viewIndex] != NULL)
      /* on n'affiche qu'un sous-arbre dans cette vue */
      if (ElemIsAnAncestor (pEl, pDoc->DocViewSubTree[viewIndex]))
	/* l'element createur englobe le sous-arbre affichable */
	/* on ne cree pas le pave' de presentation */
	ok = FALSE;

   if (ok)
     {
       /* on cree le pave (ou non, selon sa visibilite) */
       /* pRS : premiere regle de presentation */
       /* specifique de la boite a creer */
       pRS = pSchP->PsPresentBox->PresBox[pRCre->PrPresBox[0] - 1]->PbFirstPRule;
       /* pRD : premiere regle de presentation par defaut du schema de */
       /* presentation */
       pRD = pSchP->PsFirstDefaultPRule;
       /* le volume des paves englobant ne comprend pas le volume du pave */
       /* que l'on va creer */
       volok = FALSE;
       /* parcourt toutes les vues pour trouver toutes les regles de */
       /* visibilite */
       vis = 0;
       pR1 = NULL;
       for (view = 1; view <= MAX_VIEW; view++)
	 {
	   if (view == 1)
	     {
	       /* vue principale */
	       pR1 = GetRule (&pRS, &pRD, pEl, pAttr, pEl->ElStructSchema, pDoc);
	       /* regle de visibilite pour la vue 1 */
	       pR = pR1;
	     }
	   else
	     {
	       /* s'il y a une regle de visibilite pour cette vue, on la prend */
	       pR = GetRuleView (&pRS, &pRD, PtVisibility, view, pEl, pAttr,
				 pEl->ElStructSchema, pDoc);
	       if (pR == NULL)
		 /* sinon, on prend celle de la vue 1 */
		 pR = pR1;
	     }
	   if (view == viewSch)
	     {
	       /* c'est la regle de la vue traitee */
	       vis = IntegerRule (pR, pEl, viewNb, &ok, &unit, NULL, NULL);
	       /* si la regle de visibilite n'a pas pu etre appliquee, */
	       /* on prend la visibilite du pave de l'element createur */
	       if (!ok)
		 vis = pEl->ElAbstractBox[viewIndex]->AbVisibility;
	     }
	 }

       if (vis >= 1)
	 /* le pave a creer est visible dans la vue */
	 {
	   lqueue = 0;
	   pqueue = 0;
	   pAb = InitAbsBoxes (pEl, viewNb, vis, pDoc->DocReadOnly);
	   /* pAb: pave cree */
	   pAbbCreated = pAb;
	   pAb->AbPresentationBox = TRUE;
	   /* c'est un pave de presentation */
	   pAb->AbCreatorAttr = pAttr;
	   /* on se souvient de l'attribut qui a cree' le pave' */
	   pAb->AbTypeNum = pRCre->PrPresBox[0];
	   pAb->AbCanBeModified = FALSE;
	   pAb->AbPSchema = pSchP;
	   pAbb1 = pEl->ElAbstractBox[viewIndex];
	   /* pAbb1: 1er pave de l'element createur */
	   /* chaine le pave cree */
	   switch (funct)
	     {

	     case FnCreateFirst:
	       pAb->AbSelected = FALSE;
	       /* saute les paves de presentation deja crees */
	       /* avec la regle CreateBefore */
	       while (pAbb1->AbPresentationBox)
		 pAbb1 = pAbb1->AbNext;
	       pAb->AbEnclosing = pAbb1;
	       pAb->AbReadOnly = pAbb1->AbReadOnly;
	       if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
		 /* c'est le premier pave englobe' */
		 pAb->AbEnclosing->AbFirstEnclosed = pAb;
	       else
		 /* il y a deja des paves englobes */
		 {
		   pAbb1 = pAb->AbEnclosing->AbFirstEnclosed;
		   /* saute les paves deja crees par une regle */
		   /* FnCreateFirst et les paves des marques de page */
		   /* de debut d'element */
		   /* et verifie si le pave de presentation existe deja */
		   stop = FALSE;
		   do
		     if ((pAbb1->AbPresentationBox && pAbb1->AbElement == pEl)
			 || (pAbb1->AbElement->ElTypeNumber == PageBreak + 1
			     && pAbb1->AbElement->ElPageType == PgBegin))
		       {
			 /* c'est un pave de presentation de l'element ou */
			 /* un saut de page de debut d'element */
			 if (pAbb1->AbPresentationBox &&
			     pAbb1->AbTypeNum == pAb->AbTypeNum &&
			     pAbb1->AbPSchema == pSchP &&
			     (pAttr == NULL || pAttr == pAbb1->AbCreatorAttr)
			     && !pAbb1->AbDead)
			   /* c'est le meme que celui qu'on veut creer */
			   {
			     pAbbCreated = NULL;
			     stop = TRUE;
			   }
			 else if (pAbb1->AbNext == NULL ||
				  AttrIsAfter (pAbb1->AbCreatorAttr, pAttr))
			   /* le nouveau pave attache a un attribut doit s'inserer
			      avant ceux d'un attribut suivant */
			   stop = TRUE;
			 else
			   pAbb1 = pAbb1->AbNext;
		       }
		     else
		       /* ce n'est ni pave de presentation de l'element */
		       /* ni un saut de page de debut d'element */
		       stop = TRUE;
		   while (!stop);
		   if (pAbbCreated != NULL)
		     {
		     if (pAbb1->AbElement == pEl
			 || (pAbb1->AbElement->ElTypeNumber == PageBreak + 1
			     && pAbb1->AbElement->ElPageType == PgBegin))
		       /* chaine le nouveau pave apres le dernier pave */
		       /* de presentation cree par une regle FnCreateFirst */
		       /* ou la derniere marque de page de debut d'elem. */
		       {
			 pAb->AbPrevious = pAbb1;
			 pAb->AbNext = pAbb1->AbNext;
			 pAb->AbPrevious->AbNext = pAb;
			 if (pAb->AbNext != NULL)
			   pAb->AbNext->AbPrevious = pAb;
		       }
		     else
		       /* chaine le nouveau pave avant le premier pave */
		       /* du premier fils du createur */
		       {
			 pAb->AbNext = pAbb1;
			 pAb->AbPrevious = pAbb1->AbPrevious;
			 pAb->AbNext->AbPrevious = pAb;
			 if (pAb->AbPrevious != NULL)
			   pAb->AbPrevious->AbNext = pAb;
			 if (pAb->AbEnclosing->AbFirstEnclosed == pAbb1)
			   pAb->AbEnclosing->AbFirstEnclosed = pAb;
		       }
		     }
		 }
	       break;

	     case FnCreateLast:
	       pAb->AbSelected = FALSE;
	       /* saute les paves de presentation deja crees */
	       /* avec la regle CreateBefore */
	       while (pAbb1->AbPresentationBox)
		 pAbb1 = pAbb1->AbNext;
	       pAb->AbEnclosing = pAbb1;
	       pAb->AbReadOnly = pAbb1->AbReadOnly;
	       if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
		 pAb->AbEnclosing->AbFirstEnclosed = pAb;
	       else
		 {
		   pAbb1 = pAb->AbEnclosing->AbFirstEnclosed;
		   /* cherche le dernier fils du pave createur */
		   /* et verifie si le pave de presentation existe deja */
		   stop = FALSE;
		   do
		     if (pAbb1->AbPresentationBox == pAb->AbPresentationBox
			 && pAbb1->AbTypeNum == pAb->AbTypeNum
			 && pAbb1->AbPSchema == pSchP
			 && (pAttr == NULL || pAttr == pAbb1->AbCreatorAttr)
			 && !pAbb1->AbDead)
		       /* ce pave de presentation existe deja */
		       {
			 pAbbCreated = NULL;
			 stop = TRUE;
		       }
		     else if (pAbb1->AbNext == NULL ||
			      AttrIsAfter (pAbb1->AbNext->AbCreatorAttr,pAttr))
		       /* le nouveau pave attache a un attribut doit s'inserer
			  avant ceux d'un attribut suivant */
		       stop = TRUE;
		     else
		       pAbb1 = pAbb1->AbNext;
		   while (!stop);
		   /* chaine le nouveau pave apres le dernier pave */
		   /* fils du pave createur */
		   if (pAbbCreated != NULL)
		     {
		       pAb->AbPrevious = pAbb1;
		       pAb->AbNext = pAbb1->AbNext;
		       pAb->AbPrevious->AbNext = pAb;
		     }
		 }
	       break;

	     case FnCreateBefore:
	       /* saute les paves de presentation deja crees */
	       /* avec la regle CreateBefore */
	       stop = FALSE;
	       do
		 if (pAbb1->AbTypeNum == pAb->AbTypeNum
		     && pAbb1->AbPresentationBox == pAb->AbPresentationBox
		     && pAbb1->AbPSchema == pSchP
		     && (pAttr == NULL || pAttr == pAbb1->AbCreatorAttr)
		     && !pAbb1->AbDead)
		   /* ce pave de presentation existe deja */
		   {
		     pAbbCreated = NULL;
		     stop = TRUE;
		   }
		 else if ((!pAbb1->AbPresentationBox && !pAbb1->AbDead) ||
			  AttrIsAfter (pAbb1->AbCreatorAttr, pAttr))
		   /* le nouveau pave attache a un attribut doit
		      s'inserer avant ceux d'un attribut suivant */
		   stop = TRUE;
		 else
		   pAbb1 = pAbb1->AbNext;
	       while (!stop);
	       if (pAbbCreated != NULL)
		 {
		   pAb->AbReadOnly = pAbb1->AbReadOnly;
		   pAb->AbEnclosing = pAbb1->AbEnclosing;
		   if (pAb->AbEnclosing->AbFirstEnclosed == pAbb1)
		     pAb->AbEnclosing->AbFirstEnclosed = pAb;
		   pAb->AbNext = pAbb1;
		   pAb->AbPrevious = pAbb1->AbPrevious;
		   pAbb1->AbPrevious = pAb;
		   if (pAb->AbPrevious != NULL)
		     {
		       pAb->AbPrevious->AbNext = pAb;
		       if (pAb->AbPrevious->AbElement != pEl)
			 pEl->ElAbstractBox[viewIndex] = pAb;
		     }
		   else
		     pEl->ElAbstractBox[viewIndex] = pAb;
		 }
	       break;

	     case FnCreateAfter:
	     case FnCreateWith:
	       stop = FALSE;
	       do
		 if (pAbb1->AbNext == NULL)
		   stop = TRUE;
		 else if (pAbb1->AbNext->AbElement != pEl)
		   stop = TRUE;
		 else if (pAbb1->AbNext->AbPresentationBox
			  && pAbb1->AbNext->AbTypeNum == pAb->AbTypeNum
			  && pAbb1->AbNext->AbPresentationBox == pAb->AbPresentationBox
			  && pAbb1->AbNext->AbPSchema == pSchP
			  && (pAttr == NULL || pAttr == pAbb1->AbCreatorAttr)
			  && !pAbb1->AbNext->AbDead)
		   /* ce pave de presentation existe deja */
		   {
		     pAbbCreated = NULL;
		     stop = TRUE;
		   }
		 else if (AttrIsAfter (pAbb1->AbNext->AbCreatorAttr, pAttr))
		   /* le nouveau pave attache a un attribut doit s'inserer
		      avant ceux d'un attribut suivant */
		   stop = TRUE;
		 else
		   pAbb1 = pAbb1->AbNext;
	       while (!stop);
	       if (pAbbCreated != NULL)
		 {
		   pAb->AbReadOnly = pAbb1->AbReadOnly;
		   pAb->AbEnclosing = pAbb1->AbEnclosing;
		   pAb->AbNext = pAbb1->AbNext;
		   pAb->AbPrevious = pAbb1;
		   pAbb1->AbNext = pAb;
		   if (pAb->AbNext != NULL)
		     pAb->AbNext->AbPrevious = pAb;
		 }
	       break;

	     case FnCreateEnclosing:
	       if (pAbb1->AbEnclosing != NULL &&
		   pAbb1->AbEnclosing->AbPresentationBox &&
		   pAbb1->AbEnclosing->AbElement == pEl)
		 /* l'element a deja un pave de presentation englobant. */
		 /* on refuse d'en creer un autre */
		 pAbbCreated = NULL;
	       if (pAbbCreated != NULL)
		 {
		   pAb->AbReadOnly = pAbb1->AbReadOnly;
		   pAb->AbEnclosing = pAbb1->AbEnclosing;
		   if (pAb->AbEnclosing->AbFirstEnclosed == pAbb1)
		     pAb->AbEnclosing->AbFirstEnclosed = pAb;
		   pAb->AbPrevious = pAbb1->AbPrevious;
		   pAbb1->AbPrevious = NULL;
		   if (pAb->AbPrevious != NULL)
		     pAb->AbPrevious->AbNext = pAb;
		   pAb->AbFirstEnclosed = pAbb1;
		   /* traite les paves de presentation deja crees par les */
		   /* regles CreateBefore et CreateAfter */
		   stop = FALSE;
		   while (!stop)
		     {
		       pAbb1->AbEnclosing = pAb;
		       if (pAbb1->AbNext == NULL)
			 stop = TRUE;
		       else if (pAbb1->AbNext->AbElement != pEl)
			 stop = TRUE;
		       else
			 pAbb1 = pAbb1->AbNext;
		     }
		   /* traite le dernier pave' de l'element */
		   pAb->AbNext = pAbb1->AbNext;
		   pAbb1->AbNext = NULL;
		   if (pAb->AbNext != NULL)
		     pAb->AbNext->AbPrevious = pAb;
		 }
	       break;
	     default:
	       break;
	     }

	   if (pAbbCreated == NULL)	/* pave deja cree' */
	     {
	       if (pAb->AbBox &&
		   pAb->AbLeafType == LtPicture && pAb->AbPresentationBox)
		 {
		   image = (PictInfo *)pAb->AbPictInfo;
		   CleanPictInfo (image);
		   TtaFreeMemory (pAb->AbPictInfo);
		   pAb->AbPictInfo = NULL;
		 }

	       if (pAb->AbLeafType == LtCompound && pAb->AbPictBackground)
		 {
		   /* in this particular case we need to free filename */
		   image = (PictInfo *)pAb->AbPictBackground;
		   TtaFreeMemory (image->PicFileName);
		   CleanPictInfo (image);
		   TtaFreeMemory (pAb->AbPictBackground);
		   pAb->AbPictBackground = NULL;
		 }
	       /* free the abstract box */
	       FreeAbstractBox (pAb);
	     }
	   else
	     /* rend non modifiable le pave de presentation */
	     {
	       pAb->AbCanBeModified = FALSE;
	       pAb->AbLeafType = LtCompound;
	       pAb->AbVolume = 0;
	       pAb->AbInLine = FALSE;
	       pAb->AbTruncatedHead = FALSE;
	       pAb->AbTruncatedTail = FALSE;
	     }

	   if (pAbbCreated != NULL)
	     {
	       if (pER != NULL)
		 /* change le pointeur de pave de l'element englobant les */
		 /* elements associes a mettre dans la boite */
		 {
		   pER->ElParent->ElAbstractBox[viewIndex] = pAbbCreated;
		   pAbbCreated->AbPresentationBox = FALSE;
		 }
	       /* applique les regles de presentation de la boite creee. */
	       do
		 {
		   pR = GetRule (&pRS, &pRD, pEl, pAttr, pEl->ElStructSchema,
				 pDoc);
		   /* pointeur sur la regle a  appliquer pour la vue 1 */
		   if (pR != NULL)
		     /* cherche et applique les regles de tous les types */
		     /* pour la vue */
		     for (view = 1; view <= MAX_VIEW; view++)
		       {
			 if (view == 1)
			   pRV = NULL;
			 else
			   pRV = GetRuleView (&pRS, &pRD, pR->PrType, view, pEl,
					      pAttr, pEl->ElStructSchema, pDoc);
			 if (view == viewSch)
			   /* applique la regle au pave cree'. */
			   {
			     if (pRV == NULL)
			       pRV = pR;
			     if (pRV->PrType == PtFunction
				 && (pRV->PrPresFunction == FnCreateFirst ||
				     pRV->PrPresFunction == FnCreateLast))
			       /* le pave cree' cree un pave de presentation */
			       {
				 /* sauve le pointeur de pave de l'element */
				 pAbbNext = pEl->ElAbstractBox[viewIndex];
				 /* change le pointeur de pave de l'element */
				 /* pour un chainage correct du pave a creer */
				 pEl->ElAbstractBox[viewIndex] = pAbbCreated;
				 pAbbCreated->AbPresentationBox = FALSE;
				 /* cree le pave de presentation */
				 pAbb1 = CrAbsBoxesPres (pEl, pDoc, pRV, pSS, NULL,
							 viewNb, pSchP, TRUE);
				 /* restaure le pointeur de pave de l'elem */
				 pEl->ElAbstractBox[viewIndex] = pAbbNext;
			       }
			     else if (!completeCreator && pRV->PrPresMode == PresInherit
				      && pRV->PrInheritMode == InheritCreator)
			       /* toutes les regles de presentation n'ont */
			       /* pas encore ete appliquees au pave */
			       /* et le pave cree herite du createur, on */
			       /* differe l'application de la regle */
			       Delay (pRV, pSchP, pAbbCreated, NULL, pAbbCreated);
			     else if (!ApplyRule (pRV, pSchP, pAbbCreated, pDoc, NULL))
			       /* on n'a pas pu appliquer la regle, on */
			       /* l'appliquera lorsque le pave pere */
			       /* sera  termine' */
			       Delay (pRV, pSchP, pAbbCreated, NULL, pAbbCreated);
			   }
		       }
		 }
	       while (pR != NULL);

	       pAbbCreated->AbPresentationBox = TRUE;
	       /* met le contenu dans le pave cree */
	       pBox = pSchP->PsPresentBox->PresBox[pRCre->PrPresBox[0] - 1];
	       switch (pBox->PbContent)
		 {
		 case FreeContent:
		   break;
		 case ContVariable:
		   ok = NewVariable (pBox->PbContVariable, pSS, pSchP, pAb,
				     pAttr, pDoc);
		   break;
		 case ContConst:
		   ConstantCopy (pBox->PbContConstant, pSchP, pAb);
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
		     pAbbNext = pEl->ElAbstractBox[viewIndex];
		     /* change le pointeur de pave de l'element, pour un */
		     /* chainage correct du pave a creer */
		     pEl->ElAbstractBox[viewIndex] = pAbbCreated;
		     pAbbCreated->AbPresentationBox = FALSE;
		     /* cree le pave de presentation */
		     pAbb1 = CrAbsBoxesPres (pEl, pDoc, pR, pSS, NULL, viewNb,
					     pSchP, TRUE);
		     /* restaure le pointeur de pave de l'element */
		     pEl->ElAbstractBox[viewIndex] = pAbbNext;
		   }
	       while (pR != NULL);
	       do		/* applique les regles retardees */
		 {
		   pAbb1 = pAbbCreated;
		   GetDelayedRule (&pR, &pSP, &pAbb1, &pSelAttr);
		   if (pR != NULL)
		     if (!ApplyRule (pR, pSP, pAbb1, pDoc, pSelAttr))
		       Delay (pR, pSP, pAbb1, pSelAttr, pAbbCreated);
		 }
	       while (pR != NULL);
	       /* retablit AbPresentationBox qui a ete modifie' pour les boites de */
	       /* haut ou de bas de page qui regroupent des elements associes */
	       pAbbCreated->AbPresentationBox = TRUE;
	       /* ajoute le volume du pave cree' a celui de tous ses */
	       /* englobants */
	       if (pAbbCreated->AbVolume > 0 && !volok)
		 {
		   pAbb1 = pAbbCreated->AbEnclosing;
		   while (pAbb1 != NULL)
		     {
		       pAbb1->AbVolume += pAbbCreated->AbVolume;
		       pAbb1 = pAbb1->AbEnclosing;
		     }
		 }
	       /* met a jour le volume libre restant dans la vue */
	       UpdateFreeVol (pAbbCreated, pDoc);
	       if (pEl->ElTypeNumber == PageBreak + 1)
		 /* c'est une boite de haut ou bas de page. Sa creation */
		 /* affecte peut-etre les autres boites de haut ou bas de */
		 /* page deja creees. */
		 ApplyRefAbsBoxNew (pAbbCreated, pAbbCreated, &pAbb1, pDoc);

	       /* si c'est une boite contenant une image, choisit le mode */
	       /* de presentation de l'image en accord avec les regle de */
	       /* dimensions du pave */
	       if (pAbbCreated->AbLeafType == LtPicture)
		 {
		   /* a priori l'image prendra les dimensions de son pave' */
		   ((PictInfo *) (pAbbCreated->AbPictInfo))->PicPresent = ReScale;
		   if (!pAbbCreated->AbWidth.DimIsPosition
		       && pAbbCreated->AbWidth.DimValue < 0
		       && pAbbCreated->AbWidth.DimAbRef == NULL
		       && !pAbbCreated->AbHeight.DimIsPosition
		       && pAbbCreated->AbHeight.DimValue < 0
		       && pAbbCreated->AbHeight.DimAbRef == NULL)
		     /* le pave prend la hauteur et la largeur de son
			contenu */
		     /* l'image doit etre affichee telle quelle */
		     ((PictInfo *) (pAbbCreated->AbPictInfo))->PicPresent = RealSize;
		 }
	     }
	 }
     }
   return (pAbbCreated);
}

/*----------------------------------------------------------------------
   AttrPresRule retourne la premiere regle de la chaine des regles
   de presentation a` appliquer pour l'attribut pAttr.     
   - si inheritRule = true on ne s'interesse pas aux regles par defaut
     (pRPdef)
   - si inheritRule = false on ne s'interesse pas aux regles d'heritage
     (pRPherit).                                  
   - si PAttrComp != NULL alors si l'attribut est numerique et que ses regles
     sont des comparaisons on utilisera pAttrComp au lieu de rechercher
     l'attribut de comparaison dans les ascendants de pEl.  
   valueNum indicates the rank of the value to be taken into account
   when the attribute is a text attribute whose content is considered
   as a list of space separated values. Should be 1 for the first call.
   valueNum is updated to indicate the rank of the next value to be
   processed. 0 indicates that there is no more values.
  ----------------------------------------------------------------------*/
PtrPRule AttrPresRule (PtrAttribute pAttr, PtrElement pEl,
		       ThotBool inheritRule, PtrAttribute pAttrComp,
		       PtrPSchema pSchP, int *valueNum)
{
  ThotBool            found;
  PtrPRule            pRule;
  PtrAttribute        pAt2;
  PtrElement          pElAttr;
  AttributePres      *pAPRule, *pPRdef, *pPRinherit;
  NumAttrCase        *pCase;
  char                buffer[400];
  char               *attrValue, *ptr;
  unsigned int        len;
  int                 i;

  pRule = NULL;
  if (pSchP == NULL)
    return (NULL);
  pAPRule = pSchP->PsAttrPRule->AttrPres[pAttr->AeAttrNum - 1];

  /* on cherche quel est le paquet de regles qui s'applique */
  /* pPRdef designera le paquet de regles s'appliquant a tous les elements */
  /* c'est a dire celui pour lequel pAPRule->ApElemType = 0  */
  /* pPRinherit  designera le paquet pour lequel
     pAPRule->ApElemType = pEl->ElTypeNumber */

  pPRdef = pPRinherit = NULL;
  len = 0;
  attrValue = NULL;

  if (pAttr->AeAttrType != AtTextAttr)
    *valueNum = 0;
  else
    {
    if (pAttr->AeAttrText)
      {
	CopyBuffer2MBs (pAttr->AeAttrText, 0, buffer, 399);
	attrValue = buffer;
      }
    if (!AttrHasException (ExcCssClass, pAttr->AeAttrNum,pAttr->AeAttrSSchema))
      /* the content of the attribute is considered as a single value */
      {
      *valueNum = 0;
      if (attrValue)
        len = strlen (attrValue);
      else
	len = 0;
      }
    else
      /* the content of this text attribute is considered as a sequence of
	 space separated values */
      {
	if (*valueNum == 0)
	  return (NULL);
	else if (!pAttr->AeAttrText)
	  /* this attribute has no value at all */
	  *valueNum = 0;
	else
	  {
	    /* skip the values already processed */
	    for (i = 1; i < *valueNum; i++)
	      {
		while (*attrValue > ' ')
		  attrValue++;
		while (*attrValue <= ' ' && *attrValue != EOS)
		  attrValue++;
	      }
	    /* get the length of the current value */
	    ptr = attrValue;
	    len = 0;
	    while (*ptr > ' ')
	      {
	        ptr++;
		len++;
	      }
	    /* skip the spaces following this value */
	    while (*ptr <= ' ' && *ptr != EOS)
	      ptr++;
	    if (*ptr == EOS)
	      /* that's the last value */
	      *valueNum = 0;
	    else
	      /* there is another value after that one */
	      (*valueNum)++;
	  }
      }
    }
    
  for (i = pSchP->PsNAttrPRule->Num[pAttr->AeAttrNum - 1]; i-- > 0;
       pAPRule = pAPRule->ApNextAttrPres)
    {
      if (pAPRule->ApElemType == 0 || pAPRule->ApElemType == pEl->ElTypeNumber)
	{
	  if ((pAttr->AeAttrType == AtTextAttr) &&
	      (pAPRule->ApString[0] != EOS))
	    {
	      if (pAttr->AeAttrText != NULL)
		{
		/**** should be StringAndTextEqual instead of strncmp,
		 as the value may span over several buffers ****/
		if (strlen (pAPRule->ApString) == len &&
		    !strncmp (pAPRule->ApString, attrValue, len))
		  {
		    if (pAPRule->ApElemType == 0)
		      pPRdef = pAPRule;
		    else
		      pPRinherit = pAPRule;
		  }
		}
	    }
	  else
	    if (pAPRule->ApElemType == 0)
	      pPRdef = pAPRule;
	    else
	      pPRinherit = pAPRule;
	}
    }

  if (inheritRule)
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
  if (pAPRule)
    switch (pAttr->AeAttrType)
      {

      case AtNumAttr:
	i = 1;
	found = FALSE;
	while (i <= pAPRule->ApNCases && !found)
	  {
	    pCase = &pAPRule->ApCase[i - 1];
	    if (pCase->CaComparType == ComparConstant)
	      {
		/* la valeur de comparaison est une cste */
		if (pAttr->AeAttrValue >= pCase->CaLowerBound &&
		    pAttr->AeAttrValue <= pCase->CaUpperBound)
		  {
		    found = TRUE;
		    pRule = pCase->CaFirstPRule;
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
			  found = TRUE;
			  pRule = pCase->CaFirstPRule;
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
			  found = TRUE;
			  pRule = pCase->CaFirstPRule;
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
			  found = TRUE;
			  pRule = pCase->CaFirstPRule;
			}
		  }
	      }
	    i++;
	  }
	break;

      case AtTextAttr:
	pRule = pAPRule->ApTextFirstPRule;
	break;

      case AtReferenceAttr:
	pRule = pAPRule->ApRefFirstPRule;
	break;

      case AtEnumAttr:
	/* on verifie que la valeur est correcte */
	if (pAttr->AeAttrValue < 0 ||
	    pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrNEnumValues < pAttr->AeAttrValue)
	  /* valeur incorrecte, on prend les regles qui s'appliquent a */
	  /* n'importe quelle valeur */
	  pRule = pAPRule->ApEnumFirstPRule[0];
	else if (pAPRule->ApEnumFirstPRule[pAttr->AeAttrValue] == NULL)
	  /* pas de regles pour cette valeur, on prend les regles */
	  /* qui s'appliquent a n'importe quelle valeur */
	  pRule = pAPRule->ApEnumFirstPRule[0];
	else
	  /* on prend les regles qui s'appliquent a cette valeur */
	  pRule = pAPRule->ApEnumFirstPRule[pAttr->AeAttrValue];
	break;

      default:
	pRule = NULL;
	break;
      }

  return pRule;
}

/*----------------------------------------------------------------------
   ApplCrPresRule determine les regles de creation a appliquer    
   au pave pAb en fonction de head                        
  ----------------------------------------------------------------------*/
static void ApplCrPresRule (PtrSSchema pSS, PtrPSchema pSP,
			    PtrAbstractBox * pAbbCreated,
			    PtrAttribute pAttr, PtrDocument pDoc,
			    PtrAbstractBox pAb, ThotBool head, PtrPRule pRule)
{
  PtrAbstractBox      pAbb, pAbbR;
  ThotBool            stop;

  /* saute les regles precedant les fonctions */
  stop = FALSE;
  do
    if (pRule == NULL)
      stop = TRUE;
    else if (pRule->PrType > PtFunction)
      {
	stop = TRUE;
	pRule = NULL;
	/* pas de fonction de presentation */
      }
    else if (pRule->PrType == PtFunction)
      stop = TRUE;
    else
      pRule = pRule->PrNextPRule;
  while (!stop);
  /* cherche toutes les fonctions de creation */
  stop = FALSE;
  do
    if (pRule == NULL)
      stop = TRUE;
    else if (pRule->PrType != PtFunction)
      stop = TRUE;
    else
      {
	/* applique les fonctions de creation qui correspondent a */
	/* l'extremite concernee */
	/* si la regle de creation possede l'indication de repetition */
	/* on appelle la procedure de creation systematiquement */
	if ((head &&
	     (pRule->PrPresFunction == FnCreateBefore ||
	      pRule->PrPresFunction == FnCreateFirst)) ||
	    (!head &&
	     (pRule->PrPresFunction == FnCreateAfter ||
	      pRule->PrPresFunction == FnCreateLast)))
	  {
	    pAbb = CrAbsBoxesPres (pAb->AbElement, pDoc, pRule, pSS, pAttr,
				   pAb->AbDocView, pSP, TRUE);
	    if (pAbb != NULL)
	      /* TODO : valeur de pAbb si plusieurs paves crees avec Rep ?? */
	      {
		if (!head)
		  *pAbbCreated = pAbb;
		else if (*pAbbCreated == NULL)
		  *pAbbCreated = pAbb;
		/* modifie les paves environnant */
		/* qui dependent du pave cree */
		/* TODO : si Rep est vrai, plusieurs paves ont ete crees */
		/* faut-il appeler ApplyRefAbsBoxNew dans CrAbsBoxesPres ?? */
		ApplyRefAbsBoxNew (*pAbbCreated, *pAbbCreated, &pAbbR, pDoc);
		/* passe a la regle suivante */
	      }
	  }
	pRule = pRule->PrNextPRule;
      }
  while (!stop);
}

/*----------------------------------------------------------------------
   SetVerticalSpace
   Generate all vertical spaces (margin, border and padding) at the
   top of abstract box pAb if head is TRUE, at its bottom if head
   is FALSE.
  ----------------------------------------------------------------------*/
static void  SetVerticalSpace (PtrAbstractBox pAb, ThotBool head,
			       PtrDocument pDoc)
{
   PtrPRule            pRule;
   PtrPSchema          pSchP;
   PtrAttribute        pAttr;
   ThotBool            result;

   result = FALSE;
   if (head)
     /* generate space at the top of the abstract box */
     {
     pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtMarginTop, FnAny, TRUE,
			    &pAttr);
     ApplyRule (pRule, pSchP, pAb, pDoc, pAttr);
     if (pAb->AbTopMargin != 0)
       {
	 ApplyInherit (PtMarginTop, pAb, pDoc, FALSE);
	 result = TRUE;
       }
     pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtBorderTopWidth, FnAny, TRUE,
			    &pAttr);
     ApplyRule (pRule, pSchP, pAb, pDoc, pAttr);
     if (pAb->AbTopBorder != 0)
       {
	 ApplyInherit (PtBorderTopWidth, pAb, pDoc, FALSE);
	 result = TRUE;
       }
     pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtPaddingTop, FnAny, TRUE,
			    &pAttr);
     ApplyRule (pRule, pSchP, pAb, pDoc, pAttr);
     if (pAb->AbTopPadding != 0)
       {
	 ApplyInherit (PtPaddingTop, pAb, pDoc, FALSE);
	 result = TRUE;
       }
     }
   else
     /* generate space at the bottom of the abstract box */
     {
     pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtMarginBottom, FnAny, TRUE,
			    &pAttr);
     ApplyRule (pRule, pSchP, pAb, pDoc, pAttr);
     if (pAb->AbBottomMargin != 0)
       {
	 ApplyInherit (PtMarginBottom, pAb, pDoc, FALSE);
	 result = TRUE;
       }
     pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtBorderBottomWidth, FnAny,
			    TRUE, &pAttr);
     ApplyRule (pRule, pSchP, pAb, pDoc, pAttr);
     if (pAb->AbBottomBorder != 0)
       {
	 ApplyInherit (PtBorderBottomWidth, pAb, pDoc, FALSE);
	 result = TRUE;
       }
     pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtPaddingBottom, FnAny, TRUE,
			    &pAttr);
     ApplyRule (pRule, pSchP, pAb, pDoc, pAttr);
     if (pAb->AbBottomPadding != 0)
       {
	 ApplyInherit (PtPaddingBottom, pAb, pDoc, FALSE);
	 result = TRUE;
       }
    }
   if (result && !pAb->AbNew)
      pAb->AbMBPChange = TRUE;
}

/*----------------------------------------------------------------------
   TruncateOrCompleteAbsBox
   Coupe ou complete le pave pAb. pAb est coupe' si    
   truncate est vrai ou complete si truncate est faux. La coupure 
   ou la completion concerne la tete du pave si head est   
   vrai ou la queue du pave si head est faux. Marque cette 
   information dans le pave. S'il s'agit d'un pave qui     
   devient complet, cree ses paves de presentation a`      
   l'extremite qui devient complet. Retourne un pointeur  
   sur le dernier pave de presentation cree ou NULL si     
   aucun pave n'est cree.                                  
  ----------------------------------------------------------------------*/
PtrAbstractBox TruncateOrCompleteAbsBox (PtrAbstractBox pAb, ThotBool truncate,
					 ThotBool head, PtrDocument pDoc)
{
   PtrPRule            pRule;
   PtrPSchema          pSchP;
   int                 index;
   PtrSSchema          pSchS;
   PtrAbstractBox      pAbbCreated;
   PtrAttribute        pAttr;
   PtrElement          pElAttr;
   int                 l, valNum;
   InheritAttrTable   *inheritTable;
   PtrHandlePSchema    pHd;

   pAbbCreated = NULL;
   if (pAb != NULL)
     if (pAb->AbLeafType == LtCompound)
       {
       if (pAb->AbInLine)
	 {
	   if (!truncate)
	     {
	     if (head)
	       pAb->AbTruncatedHead = FALSE;
	     else
	       pAb->AbTruncatedTail = FALSE;
	     }
	 }
       else if ((head && pAb->AbTruncatedHead != truncate)
		|| (!head && pAb->AbTruncatedTail != truncate))
	 /* il y a effectivement changement */
	 {
	   if (truncate)
	     /* the abstract box is now truncated */
	     {
	     /* remove all vertical space (margin, border and padding)
	        at the end that is changed */
	     if (head)
	       {
		 if (pAb->AbTopMargin != 0)
		   {
		     pAb->AbTopMargin = 0;
		     pAb->AbMBPChange = TRUE;
		   }
		 if (pAb->AbTopBorder != 0)
		   {
		     pAb->AbTopBorder = 0;
		     pAb->AbMBPChange = TRUE;
		   }
		 if (pAb->AbTopPadding != 0)
		   {
		     pAb->AbTopPadding = 0;
		     pAb->AbMBPChange = TRUE;
		   }
	       }
	     else
	       {
		 if (pAb->AbBottomMargin != 0)
		   {
		     pAb->AbBottomMargin = 0;
		     pAb->AbMBPChange = TRUE;
		   }
		 if (pAb->AbBottomBorder != 0)
		   {
		     pAb->AbBottomBorder = 0;
		     pAb->AbMBPChange = TRUE;
		   }
		 if (pAb->AbBottomPadding != 0)
		   {
		     pAb->AbBottomPadding = 0;
		     pAb->AbMBPChange = TRUE;
		   }
	       }
	     }
	   else
	     /* le pave n'est plus coupe' a` une extremite. */
	     {
	       /* Cree les paves de presentation a` cette extremite. */
	       /* cherche la 1ere regle de presentation associee a ce type */
	       /* d'element */
	       SearchPresSchema (pAb->AbElement, &pSchP, &index, &pSchS, pDoc);
	       if (pSchS != NULL && pSchS != pAb->AbElement->ElStructSchema)
		 /* il s'agit de l'element racine d'une nature qui
		    utilise le schema de presentation de son englobant*/
		 if (pDoc->DocView[pAb->AbDocView - 1].DvSSchema !=
		     pDoc->DocSSchema)
		   {
		     pSchS = pAb->AbElement->ElStructSchema;
		     pSchP = PresentationSchema (pSchS, pDoc);
		     index = pAb->AbElement->ElTypeNumber;
		   }
	       pRule = pSchP->PsElemPRule->ElemPres[index - 1];
	       
	       /* traite les regles de creation associees au type de l'element */
	       pAttr = NULL;
	       ApplCrPresRule (pSchS, pSchP, &pAbbCreated, NULL, pDoc,
			       pAb, head, pRule);
	       /* traite les regles de creation dues a */
	       /* l'heritage des attributs */
	       pSchP = PresentationSchema (pAb->AbElement->ElStructSchema,
					   pDoc);
	       if (pSchP != NULL)
		 if (pSchP->PsNInheritedAttrs->Num[pAb->AbElement->ElTypeNumber -1])
		   {
				/* il y a heritage possible */
		     if ((inheritTable = pSchP->
			  PsInheritedAttr->ElInherit[pAb->AbElement->ElTypeNumber - 1])
			 == NULL)
		       {
			 /* cette table n'existe pas on la genere */
			 CreateInheritedAttrTable (pAb->AbElement, pDoc);
			 inheritTable = pSchP->PsInheritedAttr->ElInherit[pAb->AbElement->ElTypeNumber - 1];
		       }
		     for (l = 1; l <= pAb->AbElement->ElStructSchema->SsNAttributes; l++)
		       if ((*inheritTable)[l - 1])
			 /* pAb->AbElement herite de l'attribut l */
			 if ((pAttr = GetTypedAttrAncestor (pAb->AbElement, l,
			    pAb->AbElement->ElStructSchema, &pElAttr)) != NULL)
			   /* cherche si l existe au dessus */
			   {
			     /* on cherchera d'abord dans le schema de */
			     /* presentation principal de l'attribut */
			     pSchP = PresentationSchema (pAttr->AeAttrSSchema,
							 pDoc);
			     pHd = NULL;
			     while (pSchP != NULL)
			       {
				 /* process all values of the attribute, in
				    case of a text attribute with multiple
				    values */
				 valNum = 1;
				 do
				   {
				   pRule = AttrPresRule (pAttr, pAb->AbElement,
						   TRUE, NULL, pSchP, &valNum);
				   ApplCrPresRule (pAttr->AeAttrSSchema, pSchP,
					       &pAbbCreated, pAttr, pDoc, pAb,
					       head, pRule);
				   }
				 while (valNum > 0);

				 if (pHd == NULL)
				   /* on n'a pas encore cherche' dans les schemas
				   de presentation additionnels. On prend le
				   premier schema additionnel si on travaille
				   pour la vue principale, sinon on ignore les
				   schemas additionnels */
				   {
				     if (pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView == 1)
				       pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema, pDoc);
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
		   pSchP = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
		   pHd = NULL;
		   while (pSchP != NULL)
		     {
		       /* process all values of the attribute, in case of a
			  text attribute with multiple values */
		       valNum = 1;
		       do
			 {
			   pRule = AttrPresRule (pAttr, pAb->AbElement, FALSE,
						 NULL, pSchP, &valNum);
			   ApplCrPresRule (pAttr->AeAttrSSchema, pSchP,
				  &pAbbCreated, pAttr, pDoc, pAb, head, pRule);
			 }
		       while (valNum > 0);

		       if (pHd == NULL)
			 /* on n'a pas encore cherche' dans les schemas de
			 presentation additionnels. On prend le premier schema
			 additionnel si on travaille pour la vue principale,
			 sinon on ignore les schemas additionnels. */
			 {
			   if (pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView == 1)
			     pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema,
							  pDoc);
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
	   if (head)
	     pAb->AbTruncatedHead = truncate;
	   else
	     pAb->AbTruncatedTail = truncate;
	   if (!truncate)
	     /* generate all vertical spaces (margin, border and padding)
	        at the end that is changed */
	     SetVerticalSpace (pAb, head, pDoc);
	 }
       }
   return (pAbbCreated);
}

/*----------------------------------------------------------------------
   IsViewFull retourne vrai si la vue viewNb est pleine.             
  ----------------------------------------------------------------------*/
ThotBool IsViewFull (DocViewNumber viewNb, PtrDocument pDoc, PtrElement pEl)
{
  int               i;

  i =  viewNb - 1;
  return (pDoc->DocView[i].DvPSchemaView == 0 ||
	  pDoc->DocViewFreeVolume[i] <= 0);
}

/*----------------------------------------------------------------------
   WaitingRule met en attente la regle pR appartenant au schema de     
   presentation pSP. Cette regle sera reappliquee au pave  
   pAbb lorsque la descendance de ce pave aura ete creee.    
   pA est l'attribut auquel correspond la regle (NULL si   
   ce n'est pas une regle d'attribut).                     
  ----------------------------------------------------------------------*/
void WaitingRule (PtrPRule pR, PtrAbstractBox pAbb, PtrPSchema pSP,
		  PtrAttribute pA, PtrAttribute queuePA[MAX_QUEUE_LEN],
		  PtrPSchema queuePS[MAX_QUEUE_LEN],
		  PtrAbstractBox queuePP[MAX_QUEUE_LEN],
		  PtrPRule queuePR[MAX_QUEUE_LEN], int *lqueue)
{
   if (*lqueue >= MAX_QUEUE_LEN)
      printf ("Increase MAX_QUEUE_LEN\n");
   else
     {
	(*lqueue)++;
	queuePR[*lqueue - 1] = pR;
	queuePP[*lqueue - 1] = pAbb;
	queuePS[*lqueue - 1] = pSP;
	queuePA[*lqueue - 1] = pA;
     }
}

/*----------------------------------------------------------------------
   GetAtt recupere une regle de presentation qui etait en attente. 
  ----------------------------------------------------------------------*/
static void GetAtt (PtrPRule * pR, PtrAbstractBox * pAbb, PtrPSchema * pSP,
		    PtrAttribute * pA, PtrAttribute queuePA[MAX_QUEUE_LEN],
		    PtrPSchema queuePS[MAX_QUEUE_LEN],
		    PtrAbstractBox queuePP[MAX_QUEUE_LEN],
		    PtrPRule queuePR[MAX_QUEUE_LEN], int *lqueue, int *pqueue)
{
   *pR = NULL;
   while (*pR == NULL && *pqueue < *lqueue)
     {
	(*pqueue)++;
	*pR = queuePR[*pqueue - 1];
	*pAbb = queuePP[*pqueue - 1];
	*pSP = queuePS[*pqueue - 1];
	*pA = queuePA[*pqueue - 1];
     }
}

/*----------------------------------------------------------------------
   ApplCrRule verifie que la regle pRuleCr appartenant au   
   schema de presentation pSchPres (correspondant au       
   schema de structure pSS), est une regle de creation et, 
   si oui, tente de l'appliquer a` l'element pEl. La       
   fonction retourne Vrai s'il s'agit bien d'une regle de  
   creation. pA est l'attribut auquel correspond la regle, 
   s'il s'agit d'une regle de presentation d'attribut (NULL
   sinon).                                                 
  ----------------------------------------------------------------------*/
static ThotBool ApplCrRule (PtrPRule pRuleCr, PtrSSchema pSS,
			    PtrPSchema pSchPres, PtrAttribute pA,
			    PtrAbstractBox * pAbbReturn, DocViewNumber viewNb,
			    PtrDocument pDoc, PtrElement pEl,
			    ThotBool forward, int *lqueue,
			    PtrPRule queuePR[MAX_QUEUE_LEN],
			    PtrAbstractBox queuePP[MAX_QUEUE_LEN],
			    PtrPSchema queuePS[MAX_QUEUE_LEN],
			    PtrAttribute queuePA[MAX_QUEUE_LEN],
			    PtrAbstractBox pNewAbbox)
{
   ThotBool            result, toCreate;
   PtrAbstractBox      pAbb;

   if (pRuleCr->PrType != PtFunction)
     /* ce n'est pas une regle de creation */
     result = FALSE;
   else
     {
       /* c'est une fonction de presentation */
       result = pRuleCr->PrPresFunction == FnCreateFirst ||
	        pRuleCr->PrPresFunction == FnCreateLast ||
	        pRuleCr->PrPresFunction == FnCreateBefore ||
	        pRuleCr->PrPresFunction == FnCreateWith ||
	        pRuleCr->PrPresFunction == FnCreateAfter ||
	        pRuleCr->PrPresFunction == FnCreateEnclosing;
       toCreate = FALSE;	/* a priori il n' y a pas de pave a creer */
       if (pNewAbbox != NULL)
	 switch (pRuleCr->PrPresFunction)
	   {
	   case FnCreateFirst:
	     if (pNewAbbox->AbLeafType != LtCompound ||
		 pNewAbbox->AbInLine || !pNewAbbox->AbTruncatedHead)
	       toCreate = TRUE;
	     break;
	   case FnCreateLast:
	     if (pNewAbbox->AbLeafType != LtCompound ||
		 pNewAbbox->AbInLine || !pNewAbbox->AbTruncatedTail)
	       /* on appliquera la regle de creation quand tous les paves */
	       /* descendants de l'element seront crees */
	       WaitingRule (pRuleCr, pNewAbbox, pSchPres, pA, queuePA, queuePS,
			    queuePP, queuePR, lqueue);
	     break;
	   case FnCreateBefore:
	     if (forward)
	       /* on ne cree le pave de presentation que si */
	       /* le pave de l'element est complete en tete */
	       {
		 if (pNewAbbox->AbLeafType != LtCompound ||
		     pNewAbbox->AbInLine || !pNewAbbox->AbTruncatedHead)
		   toCreate = TRUE;
	       }
	     else if (pNewAbbox->AbLeafType != LtCompound ||
		      pNewAbbox->AbInLine)
	       /* on appliquera la regle de creation quand */
	       /* tous les paves descendants de l'element */
	       /* seront crees */
	       WaitingRule (pRuleCr, pNewAbbox, pSchPres, pA, queuePA, queuePS,
			    queuePP, queuePR, lqueue);
	     break;
	   case FnCreateWith:
	   case FnCreateEnclosing:
	     toCreate = TRUE;
	     break;
	   case FnCreateAfter:
	     if (forward)
	       {
		 if (pNewAbbox->AbLeafType != LtCompound ||
		     pNewAbbox->AbInLine)
		   /* on appliquera la regle de creation quand tous les */
		   /* paves  descendants de l'element seront crees */
		   WaitingRule (pRuleCr, pNewAbbox, pSchPres, pA, queuePA,
				queuePS, queuePP, queuePR, lqueue);
	       }
	     else
	       /* on ne cree le pave de presentation que si */
	       /* le pave de l'element est complete en queue */
	       if (pNewAbbox->AbLeafType != LtCompound ||
		   pNewAbbox->AbInLine || !pNewAbbox->AbTruncatedTail)
		 toCreate = TRUE;
	     break;
	   default:
	     break;
	   }
       /* c'est une creation */
       if (toCreate)
	 /* on cree le pave de presentation */
	 {
	   pAbb = CrAbsBoxesPres (pEl, pDoc, pRuleCr, pSS, pA, viewNb,
				  pSchPres, FALSE);
	   if (pAbb != NULL)
	     /* le pave de presentation a ete cree */
	     if (pRuleCr->PrPresFunction == FnCreateBefore ||
		 pRuleCr->PrPresFunction == FnCreateEnclosing)
	       if (pEl->ElParent != NULL)
		 /* il a ete cree devant */
		 if (*pAbbReturn == pNewAbbox)
		   /* on s'appretait a retourner un pointeur sur le pave */
		   /* createur. Oon retourne un pointeur sur la pave cree, */
		   /* qui est le premier pave de l'element */
		   *pAbbReturn = pAbb;
	 }
     }
   return result;
}

/*----------------------------------------------------------------------
   DescVisible cherche le premier descendant de l'element pE qui   
   ait un pave dans la vue viewNb.                          
  ----------------------------------------------------------------------*/
static PtrElement DescVisible (PtrElement pE, DocViewNumber viewNb,
			       ThotBool forward)
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
	   if (forward)
	      while (pElem->ElNext != NULL)
		 pElem = pElem->ElNext;
	   do
	      if (pElem->ElAbstractBox[viewNb - 1] != NULL)
		 /* l'element a un pave dans la vue, on a trouve' */
		 pDesc = pElem;
	      else
		 /* l'element n'a pas de pave dans la vue cherche le premier */
		 /* descendant qui ait un pave dans la vue */
		{
		   pDesc = DescVisible (pElem, viewNb, forward);
		   if (pDesc == NULL)
		     {
		      /* aucun descendant n'a de pave, on passe a l'element */
		      /* suivant */
		      if (forward)
			 pElem = pElem->ElPrevious;
		      else
			 pElem = pElem->ElNext;
		     }
		   /* on arrete quand on a trouve ou quand on a traite' tous les */
		   /* fils de pE. */
		}
	   while (pDesc == NULL && pElem != NULL);
	}
   return pDesc;
}

/*----------------------------------------------------------------------
   GetNextAttrPresRule
   Get the next presentation rule associated with attribute pAttr (which
   belongs to element pElAttr) that applies to element pEl.
  ----------------------------------------------------------------------*/
static PtrPRule GetNextAttrPresRule (PtrPRule *pR, PtrSSchema pSS,
				     PtrAttribute pAttr, PtrElement pElAttr,
				     PtrDocument pDoc, PtrElement pEl,
				     int view)
{
  PtrPRule            pRuleView1, pRuleToApply;

  /* on cherche une regle qui concerne la vue ou defaut la regle pour la vue 1*/
  pRuleView1 = NULL;
  pRuleToApply = NULL;
  if ((*pR)->PrViewNum == 1)
    {
      /* la regle pour la vue 1 */
      if ((*pR)->PrCond == NULL ||
	  CondPresentation ((*pR)->PrCond, pEl, pAttr, pElAttr, 1, pSS, pDoc))
	/* la condition d'application est satisfaite */
	{
	  /* On la conserve au cas ou on ne trouve pas mieux */
	  pRuleView1 = *pR;
	  if (view == 1)
	    /* on est dans la vue 1. Donc c'est la bone regle */
	    pRuleToApply = *pR;
	  else
	    {
	      /* on cherche s'il existe une regle de meme type pour la vue
		 view, dont les conditions d'application soient satisfaites */
	      while ((*pR)->PrNextPRule &&
		     (*pR)->PrNextPRule->PrType == pRuleView1->PrType)
		{
		  /* la boucle parcourt toutes les regles de meme type */
		  *pR = (*pR)->PrNextPRule;
		  if ((*pR)->PrViewNum == view)
		    if ((*pR)->PrCond == NULL ||
			CondPresentation ((*pR)->PrCond, pEl, pAttr, pElAttr,
					  view, pSS, pDoc))
		      pRuleToApply = *pR;
		}
	      if (pRuleToApply == NULL)
		/* il n'y a pas de regle specifique pour la vue view */
		/* On prend la vue 1 */
		pRuleToApply = pRuleView1;
	    }
	}
    }
  else
    /* ce n'est pas une regle pour la vue 1. Cette regle */
    /* ne s'applique que si le numero de vue correspond */
    if (view == (*pR)->PrViewNum)
      if ((*pR)->PrCond == NULL ||
	  CondPresentation ((*pR)->PrCond, pEl, pAttr, pElAttr, view, pSS, pDoc))
	pRuleToApply = *pR;

  return pRuleToApply;
}

/*----------------------------------------------------------------------
   ApplyVisibRuleAttr modifie le parametre vis selon la regle de   
   visibilite de pAttr.                                    
  ----------------------------------------------------------------------*/
static void ApplyVisibRuleAttr (PtrElement pEl, PtrAttribute pAttr,
				PtrElement pElAttr, PtrDocument pDoc, int *vis,
				DocViewNumber viewNb, ThotBool *ok,
				ThotBool inheritRule)
{
  PtrPRule            pR, pRuleView1;
  int                 view, valNum;
  ThotBool            stop, useView1;
  PtrPSchema          pSchP;
  PtrHandlePSchema    pHd;
  TypeUnit            unit;

  /* on cherchera d'abord dans le schema de presentation principal de */
  /* l'attribut */
  pSchP = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
  pHd = NULL;
  /* on examine le schema de presentation principal, puis les schemas */
  /* additionnels */
  while (pSchP != NULL)
    {
      /* process all values of the attribute, in case of a text attribute
	 with multiple values */
      valNum = 1;
      do
	{
	  /* cherche la premiere regle de presentation pour cette valeur */
	  /* de l'attribut, dans ce schema de presentation */
	  pR = AttrPresRule (pAttr, pEl, inheritRule, NULL, pSchP, &valNum);
	  pRuleView1 = NULL;
	  if (pR != NULL)
	    if (pR->PrType == PtVisibility)
	      /* cette valeur d'attribut a une regle de visibilite' */
	      /* calcule le numero de la vue concernee par l'attribut */
	      {
		view = AppliedView (pEl, pAttr, pDoc, viewNb);
		stop = FALSE;
		useView1 = TRUE;
		
		/* cherche s'il y a une regle de visibilite pour la vue */
		while (!stop)
		  {
		    if (pR->PrViewNum == 1)
		      if (pR->PrCond == NULL ||
			  CondPresentation (pR->PrCond, pEl, pAttr, pElAttr,
					    1, pAttr->AeAttrSSchema, pDoc))
			{
			  pRuleView1 = pR;
			  if (view == 1)
			    stop = TRUE;
			  else
				/* saute les regles de visibilite' suivantes
				   de la vue 1 */
			    while (pR->PrNextPRule != NULL &&
				   pR->PrNextPRule->PrType == PtVisibility &&
				   pR->PrNextPRule->PrViewNum == 1)
			      pR = pR->PrNextPRule;
			}
		    if (!stop)
		      {
			if (pR->PrViewNum == view &&
			    CondPresentation (pR->PrCond, pEl, pAttr, pElAttr,
					     view, pAttr->AeAttrSSchema, pDoc))
			  {
			    /* regle trouvee, on l'evalue */
			    *vis = IntegerRule (pR, pEl, viewNb, ok, &unit,
						pAttr, NULL);
			    useView1 = FALSE;
			    stop = TRUE;
			  }
			else if (pR->PrNextPRule == NULL)
			  stop = TRUE;
			else
			  {
			    pR = pR->PrNextPRule;
			    if (pR->PrType != PtVisibility)
			      stop = TRUE;
			  }
		      }
		  }
		
		if (useView1 && pRuleView1 != NULL)
		  /* on n'a pas trouve de regle specifique pour la vue view */
		  /* On utilise la regle de visibilite de la vue 1 si elle
		     existe */
		  *vis = IntegerRule (pRuleView1, pEl, viewNb, ok, &unit,
				      pAttr, NULL);
	      }
	}
      while (valNum > 0);
      
      if (pHd == NULL)
	{
	  /* on n'a pas encore traite' les schemas de presentation additionnels
	     On prend le premier schema additionnel si on travaille pour la vue
	     principale, sinon on ignore les schemas additionnels. */
	  if (pDoc->DocView[viewNb - 1].DvPSchemaView == 1)
	    pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema, pDoc);
	}
      else
	/* passe au schema additionnel suivant */
	pHd = pHd->HdNextPSchema;
      if (pHd == NULL)
	/* il n'y a pas (ou plus) de schemas additionnels a prendre en compte*/
	pSchP = NULL;
      else
	pSchP = pHd->HdPSchema;
    }
}

/*----------------------------------------------------------------------
   ComputeVisib determine la visibilite de l'element a creer en         
   fonction de la regle de visibilite associee au type de     
   l'element ou a ses attributs                               
   pAbbReturnne la visibilite calculee dans vis                   
   Met a jour vis, pRSpec, pRDef, TypeP et pSchPPage          
  ----------------------------------------------------------------------*/
static void ComputeVisib (PtrElement pEl, PtrDocument pDoc,
			  DocViewNumber viewNb, int viewSch, PtrPRule * pRSpec,
			  PtrPRule * pRDef, int *vis,
			  ThotBool * ignoreDescent, ThotBool * complete,
			  int *TypeP, PtrPSchema * pSchPPage)
{
   int                 view, l;
   PtrPRule            pRule, pRegleV;
   PtrElement          pPrevious, pNext, pElAssociatedPage, pAsc, pElAttr;
   PtrAttribute        pAttr;
   PtrPSchema          pSP;
   InheritAttrTable   *inheritTable;
   ThotBool            ok, stop;
   TypeUnit            unit;

   /* si un element ascendant est rendu invisible, notre element a une */
   /* visibilite' nulle */
   *vis = 0;
   pAsc = pEl;
   while (pAsc)
     if (pAsc->ElAccess == AccessHidden)
       return;
     else
       pAsc = pAsc->ElParent;

   /* cherche parmi les regles de presentation specifique de l'element */
   pRule = pEl->ElFirstPRule;
   while (pRule)
     /* applique une regle si elle concerne la vue */
     /* et si ce n'est pas une hauteur de page */
     {
       if (pRule->PrType == PtVisibility && pRule->PrViewNum == viewSch)
	 {
	   if (pRule->PrSpecifAttr == 0)
	     /* cette regle ne depend pas d'un attribut */
	     pAttr = NULL;
	   else
	     /* cherche l'attribut dont depend la regle */
	     {
	       pAttr = pEl->ElFirstAttr;
	       stop = FALSE;
	       while (pAttr != NULL && !stop)
		 if (pAttr->AeAttrNum == pRule->PrSpecifAttr &&
		     !strcmp (pAttr->AeAttrSSchema->SsName,
			      pRule->PrSpecifAttrSSchema->SsName))
		   stop = TRUE;
		 else
		   pAttr = pAttr->AeNext;
	     }
	   *vis = IntegerRule (pRule, pEl, viewNb, &ok, &unit, pAttr, NULL);
	   if (ok)
	     return;
	 }
       pRule = pRule->PrNextPRule;
     }

   /* cherche les regles de visibilite du schema de presentation */
   pRule = GetRule (pRSpec, pRDef, pEl, NULL, pEl->ElStructSchema, pDoc);
   /* pointeur sur la 1ere regle a appliquer */

   /* la premiere regle est la regle de visiblite pour la vue 1 */
   *vis = 0;
   /* parcourt toutes les vues definies dans le schema de presentation */
   /* pour trouver la regle de visibilite pour la vue traitee */
   for (view = 1; view <= MAX_VIEW; view++)
     {
	/* Cherche la regle de visibilite a appliquer */
	if (view == 1)
	   pRegleV = NULL;
	else
	   pRegleV = GetRuleView (pRSpec, pRDef, PtVisibility, view, pEl, NULL,
				  pEl->ElStructSchema, pDoc);
	if (view == viewSch && DoesViewExist (pEl, pDoc, viewNb))
	  {
	   /* s'il y a une regle de visibilite pour cette vue, on */
	   /* la prend */
	   if (pRegleV != NULL)
	      *vis = IntegerRule (pRegleV, pEl, viewNb, &ok, &unit, NULL,
				  NULL);
	   /* sinon, on prend celle de la vue 1 */
	   else
	      *vis = IntegerRule (pRule, pEl, viewNb, &ok, &unit, NULL, NULL);
	  }
     }

   /* cherche si les attributs herites par l'element modifient la */
   /* visibilite */
   pSP = PresentationSchema (pEl->ElStructSchema, pDoc);
   if (pSP->PsNInheritedAttrs->Num[pEl->ElTypeNumber - 1])
     {
	/* il y a heritage possible */
	if ((inheritTable = pSP->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1]) == NULL)
	  {
	     /* cette table n'existe pas on la genere */
	     CreateInheritedAttrTable (pEl, pDoc);
	     inheritTable = pSP->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1];
	  }
	for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
	   if ((*inheritTable)[l - 1])	/* pEl herite de l'attribut l */
	      if ((pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElStructSchema,
						 &pElAttr)) != NULL)
		 /* cherche si l existe au dessus */
		 ApplyVisibRuleAttr (pEl, pAttr, pElAttr, pDoc, vis, viewNb,
				     &ok, TRUE);
     }

   /* cherche si les attributs de l'element modifient la visibilite */
   pAttr = pEl->ElFirstAttr;
   while (pAttr != NULL)
      /* boucle sur les attributs de l'element */
     {
	ApplyVisibRuleAttr (pEl, pAttr, pEl, pDoc, vis, viewNb, &ok, FALSE);
	pAttr = pAttr->AeNext;	/* attribut suivant de l'element */
     }

   /* force la visibilite du pave racine si elle n'a pas ete evaluee, */
   if (!ok && pEl->ElParent == NULL)
     *vis = 10;

   /* on ne cree une marque de page que si elle concerne la vue et */
   /* s'il existe des regles de presentation de la page */
   if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
     {
      /* c'est une marque de page */
      if (pEl->ElViewPSchema != viewSch)
	 /* cette marque ne concerne pas la vue traitee, on ne cree */
	 /* rien */
	{
	   *vis = 0;
	   /* on ne pourra jamais creer de pave' pour cet element dans */
	   /* cette vue */
	   *complete = TRUE;
	}
      else
	 /* cherche le type de boite page ou colonne a creer en  */
	 /* cherchant les regles Page ou Column des elements englobants */
	 /* ou precedants */
	{
	   *TypeP = GetPageBoxType (pEl, pDoc, viewSch, pSchPPage);
	   if (*TypeP == 0)
	      /* pas de page definie, on ne cree rien */
	      *vis = 0;
	   else if (*vis <= 0)
	      *vis = 1;
	}
     }

   /* si la vue n'affiche qu'un sous-arbre, l'element n'est visible */
   /* que s'il est dans le sous-arbre en question ou sur le chemin */
   /* entre la racine et le sous-arbre. */
   if (pDoc->DocViewSubTree[viewNb - 1] != NULL)
     /* on n'affiche qu'un sous-arbre dans cette vue */
     {
       if (!ElemIsAnAncestor (pDoc->DocViewSubTree[viewNb - 1], pEl) &&
	   pDoc->DocViewSubTree[viewNb - 1] != pEl)
	 /* l'elem. traite' n'est pas dans le sous-arbre affichable */
	 if (!ElemIsAnAncestor (pEl, pDoc->DocViewSubTree[viewNb - 1]))
	   /* il n'englobe pas le sous-arbre affichable */
	   *vis = 0;	/* on ne cree pas son pave' */
     }

   /* si l'element est une copie, qu'il porte une exception
      ExcPageBreakRepetition ou ExcPageBreakRepBefore, qu'il est associe a
      un saut de page et que ce saut de page n'appartient pas a la vue
      concernee, alors on ne cree pas son pave. */
   if (pEl->ElSource)
     {
	pElAssociatedPage = NULL;
	if (TypeHasException (ExcPageBreakRepBefore, pEl->ElTypeNumber,
			      pEl->ElStructSchema))
	  {
	     pNext = pEl->ElNext;
	     while (pNext)
	       {
		  if (pNext->ElTypeNumber == PageBreak + 1)
		    {
		       pElAssociatedPage = pNext;
		       break;
		    }
		  else if (pNext->ElSource)
		     pNext = pNext->ElNext;
		  else
		     break;
	       }
	  }

	else if (TypeHasException (ExcPageBreakRepetition, pEl->ElTypeNumber,
				   pEl->ElStructSchema))
	  {
	     pPrevious = pEl->ElPrevious;
	     while (pPrevious)
	       {
		  if (pPrevious->ElTypeNumber == PageBreak + 1)
		    {
		       pElAssociatedPage = pPrevious;
		       break;
		    }
		  else if (pPrevious->ElSource)
		     pPrevious = pPrevious->ElPrevious;
		  else
		     break;
	       }
	  }
	if (pElAssociatedPage)
	  {
	   /* La copie est associee a un saut de page */
	   if (pElAssociatedPage->ElViewPSchema != viewSch)
	     {
		/* cette marque ne concerne pas la vue traitee, */
		/* on ne cree rien */
		*ignoreDescent = TRUE;
		*vis = 0;
	     }
	   else
	     {
		/* cherche le type de boite page a creer en cherchant */
		/* les regles Page des elements englobants */
		*TypeP = GetPageBoxType (pElAssociatedPage, pDoc, viewSch,
					 pSchPPage);
		if (*TypeP == 0)
		  {
		     /* pas de page definie, on ne cree rien */
		     *ignoreDescent = TRUE;
		     *vis = 0;
		  }
	     }
	  }
     }
}

/*----------------------------------------------------------------------
  RuleHasHigherPriority
  return TRUE if presentation rule pRule1 (which belongs to presentation
  schema pPS1) has a higher priority in the CSS cascade than pRule2 (which
  belongs to presentation schema pPS2).
  ----------------------------------------------------------------------*/
ThotBool RuleHasHigherPriority (PtrPRule pRule1, PtrPSchema pPS1,
				PtrPRule pRule2, PtrPSchema pPS2)
{
  ThotBool     higher;

  higher = TRUE;
  if (pRule2 && pPS2)
    {
      /* check origin first */
      if (pPS1->PsOrigin != pPS2->PsOrigin)
	/* rules have different origins */
	{
	  /* check importance */
	  if (pRule1->PrImportant || pRule2->PrImportant)
	    /* one rule at least is important */
	    {
	      if (pRule1->PrImportant && pRule2->PrImportant)
		/* both rules are important. User wins */
		higher = (pPS1->PsOrigin == User);
	      else
		/* only one rule is important. This one wins */
		higher = pRule1->PrImportant;
	    }
	  else
	    /* no rule is important */
	    higher = (pPS1->PsOrigin > pPS2->PsOrigin);
	}
      else
	/* rules have same origin */
	{
	  /* check importance */
	  if ((pRule1->PrImportant || pRule2->PrImportant) &&
	      !(pRule1->PrImportant && pRule2->PrImportant))
	    /* one rule and only one is important, it wins */
	    higher = pRule1->PrImportant;
	  else
	    /* no rule is important, or both are */
	    /* take selectivity into account */
	    higher = (pRule1->PrSpecificity >= pRule2->PrSpecificity);
	}
    }
  return higher;
}

/*----------------------------------------------------------------------
  ApplyPresRules applies all presentation rules to the new abstract box
  ----------------------------------------------------------------------*/
static void  ApplyPresRules (PtrElement pEl, PtrDocument pDoc,
			    DocViewNumber viewNb,
			    int viewSch, PtrSSchema pSchS, PtrPSchema pSchP,
			    PtrPRule * pRSpec, PtrPRule * pRDef,
			    PtrAbstractBox * pAbbReturn, ThotBool forward,
			    int *lqueue, PtrPRule queuePR[MAX_QUEUE_LEN],
			    PtrAbstractBox queuePP[MAX_QUEUE_LEN],
			    PtrPSchema queuePS[MAX_QUEUE_LEN],
			    PtrAttribute queuePA[MAX_QUEUE_LEN],
			    PtrAbstractBox pNewAbbox)
{
  /* for each type of presentation property, selectedRule[i] records the
     latest selected rule while applying the cascade and schemaOfSelectedRule
     records the presentation schema the corresponding rule comes from */
  PtrPRule           selectedRule[PtPictInfo];
  PtrPSchema         schemaOfSelectedRule[PtPictInfo];
  PtrAttribute       attrOfSelectedRule[PtPictInfo];
  int                i, view, l, valNum;
  PtrPRule           pRuleView, pRule, ruleToApply, pR;
  PtrHandlePSchema   pHd;
  PtrPSchema         pSchPres, pSchPattr, pSP;
  PtrAttribute       pAttr;
  PtrElement         pElAttr;
  PtrSSchema	     pSSattr;
  InheritAttrTable   *inheritTable;
  ThotBool           stop, apply;

  /* no rule selected yet */
  for (i = 0; i < PtPictInfo; i++)
    {
      selectedRule[i] = NULL;
      schemaOfSelectedRule[i] = NULL;
      attrOfSelectedRule[i] = NULL;
    }

  /* get all rules associated with the element type in the main presentation */
  /* schema (default stylesheet of the user agent in CSS terms) */
  pRuleView = NULL;
  do
    {
      /* get the rule to be applied for view 1 (main view) */
      pRule = GetRule (pRSpec, pRDef, pEl, NULL, pSchS, pDoc);
      if (pRule)
	/* if its a rule that creates a presentation box, apply it */
	if (!ApplCrRule (pRule, pSchS, pSchP, NULL, pAbbReturn, viewNb,
			 pDoc, pEl, forward, lqueue, queuePR, queuePP,
			 queuePS, queuePA, pNewAbbox))
	  /* it's not a creation rule */
	  /* get the rules for the same property in all other views */
	  for (view = 1; view <= MAX_VIEW; view++)
	    {
	      if (view == 1)
		pRuleView = NULL;
	      else
		pRuleView = GetRuleView (pRSpec, pRDef, pRule->PrType, view,
					 pEl, NULL, pSchS, pDoc);
	      if (view == viewSch && pNewAbbox != NULL &&
		  DoesViewExist (pEl, pDoc, viewNb))
		{
		  if (pRuleView == NULL)
		    /* no rule for this view. Take the rule for view 1 */
		    pRuleView = pRule;
		  /* if it's the main view, record the rule for the cascade */
		  /* but presentation function are applied immediately */
		  if (view == 1 && pRuleView->PrType != PtFunction)
		    {
		      selectedRule[pRuleView->PrType] = pRuleView;
		      schemaOfSelectedRule[pRuleView->PrType] = pSchP;      
		      attrOfSelectedRule[pRuleView->PrType] = NULL;
		    }
		  else
		    /* it's not the main view or it's a presentation funtion,
		       apply the rule immediately */
		    if (!ApplyRule (pRuleView, pSchP, pNewAbbox, pDoc, NULL))
		      WaitingRule (pRuleView, pNewAbbox, pSchP, NULL,
				   queuePA, queuePS, queuePP, queuePR, lqueue);
		}
	    }
    }
  while (pRule != NULL);

  if (!pNewAbbox)
    return;

  /* look at all additional P schemas (CSS style sheets) in the order of
     their weight, but look first at the main presentation schema to
     get presentation rules associated with attributes */
  pHd = NULL;
  /* look at the main presentation schema first */
  pSP = PresentationSchema (pEl->ElStructSchema, pDoc);
  pSchPres = pSP;
  while (pSchPres != NULL)
    {
      /* first, get rules associated with the element type only if it's not the
         main presentation schema (pHd is NULL when it's the main P schema) */
      /* Note that schema extensions (pHd != NULL), aka CSS stylesheets,
	 apply only to the main view (viewSch = 1) */
      if (viewSch == 1 && pHd)
	/* we are interested in the main view and it's not the default
	   style sheet */
        {
	  /* first rule associated with the element type in this P schema
	     extension */
	  pRule = pSchPres->PsElemPRule->ElemPres[pEl->ElTypeNumber - 1];
	  while (pRule != NULL)
	    {
	      if (pRule->PrCond == NULL ||
		  CondPresentation (pRule->PrCond, pEl, NULL, NULL, 1,
				    pEl->ElStructSchema, pDoc))
		/* conditions are ok */
		{
		  /* keep that rule only if it has a higher priority than the
		     rule for the same property we have already encountered */
		  if (RuleHasHigherPriority (pRule, pSchPres,
					  selectedRule[pRule->PrType],
					  schemaOfSelectedRule[pRule->PrType]))
		    {
		      selectedRule[pRule->PrType] = pRule;
		      schemaOfSelectedRule[pRule->PrType] = pSchPres;      
		      attrOfSelectedRule[pRule->PrType] = NULL;
		    }
		}
	      /* next rule for the element type in the same P schema extens. */
	      pRule = pRule->PrNextPRule;
	    }
        }

      /* now, get the rules associated with attributes of ancestors that apply
         to the element, for all views if it's the main P schema, but only
         for view 1 if it's a P schema extension */
      if (viewSch == 1 || pHd == NULL)
	{
	  if (pSP->PsNInheritedAttrs->Num[pEl->ElTypeNumber - 1])
	    /* the element type inherit some attributes */
	    {
	      inheritTable = pSP->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1];
	      if (!inheritTable)
		{
		  CreateInheritedAttrTable (pEl, pDoc);
		  inheritTable = pSP->PsInheritedAttr->ElInherit[pEl->ElTypeNumber-1];
		}
	      for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
		if ((*inheritTable)[l - 1])  /* pEl inherit attribute l */
		  /* is this attribute present on an ancestor? */
		  if ((pAttr = GetTypedAttrAncestor (pEl, l, pEl->ElStructSchema,
						     &pElAttr)) != NULL)
		    {
		      apply = TRUE;
		      /* exceptions for attributes related to tables */
		      if (ThotLocalActions[T_ruleattr] != NULL)
			(*ThotLocalActions[T_ruleattr])
			  (pEl, pAttr, pDoc, &apply);
		      if (apply)
			{
			  view = AppliedView (pEl, pAttr, pDoc, viewNb);
			  pSSattr = pAttr->AeAttrSSchema;
			  if (pDoc->DocView[viewNb-1].DvPSchemaView == 1 && pHd)
			    /* it's a P schema extension */
			    /* if it's an ID ou class attribute, take P schema
			       extensions associated with the document S schema*/
			    if (AttrHasException (ExcCssClass, pAttr->AeAttrNum,
						  pAttr->AeAttrSSchema) ||
				AttrHasException (ExcCssId, pAttr->AeAttrNum,
						  pAttr->AeAttrSSchema))
			      pSSattr = pDoc->DocSSchema;
			  /* process all values of the attribute, in case of a
			     text attribute with a list of values */
			  valNum = 1;
			  do
			    {
			      /* first rule for this value of the attribute */
			      pR = AttrPresRule (pAttr, pEl, TRUE, NULL,
						 pSchPres, &valNum);
			      /* look at all rules associated with this value */
			      while (pR != NULL)
				{
				  ruleToApply = NULL;
				  if (!pR->PrDuplicate)
				    /* if it's a creation rule, apply it now */
				    if (!ApplCrRule (pR, pSSattr, pSchPres, pAttr,
					    pAbbReturn, viewNb, pDoc, pEl, forward,
                                            lqueue, queuePR, queuePP, queuePS, queuePA,
                                            pNewAbbox))
				      /* not a creation rule, get the right rule*/
				      ruleToApply = GetNextAttrPresRule (&pR,
							    pAttr->AeAttrSSchema, pAttr,
						            pElAttr, pDoc, pEl, view);
				  if (ruleToApply &&
				      DoesViewExist (pEl, pDoc, viewNb))
				    /* this rule applies to the element */
				    {
				      if (viewSch == 1 &&
					  ruleToApply->PrType != PtFunction)
					/* main view. Record the rule for cascade */
					/* but apply presentation function */
					/* immediately */
					{
					  if (RuleHasHigherPriority (ruleToApply,
					     pSchPres,
					     selectedRule[ruleToApply->PrType],
					     schemaOfSelectedRule[ruleToApply->PrType]))
					    {
					      selectedRule[ruleToApply->PrType] = ruleToApply;
					      schemaOfSelectedRule[ruleToApply->PrType] = pSchPres;      
					      attrOfSelectedRule[ruleToApply->PrType] = pAttr;
					    }
					}
				      else
					/* not the main view, apply the rule now */
					if (!ApplyRule (ruleToApply, pSchPres,
							pNewAbbox, pDoc, pAttr))
					  WaitingRule (ruleToApply, pNewAbbox, pSchPres,
						       pAttr, queuePA, queuePS, queuePP,
						       queuePR, lqueue);
				    }
				/* next rule associated with this value of the
				   attribute */
				  pR = pR->PrNextPRule;
				}
			    }
			  while (valNum > 0);
			}
		    }
	    }

	  /* now get the rules associated with the attributes of the element */
	  pAttr = pEl->ElFirstAttr;	/* first attribute of element */
	  /* check all attributes of element */
	  while (pAttr != NULL)
	    {
	      if (pHd == NULL)
		/* main presentation schema. Take the one associated with
		   the attribute S schema */
		pSchPattr = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
	      else
		pSchPattr = pSchPres;
	      apply = TRUE;
	      /* exceptions for the attributes of a table */
	      if (ThotLocalActions[T_ruleattr] != NULL)
		(*ThotLocalActions[T_ruleattr])
		  (pEl, pAttr, pDoc, &apply);
	      if (apply)
		{
		  view = AppliedView (pEl, pAttr, pDoc, viewNb);
		  pSSattr = pAttr->AeAttrSSchema;
		  if (pDoc->DocView[viewNb - 1].DvPSchemaView == 1 && pHd)
		    /* this is a P schema extension */
		    /* if it's an ID ou class attribute, take P schema
		       extensions associated with the document S schema */
		    if (AttrHasException (ExcCssClass, pAttr->AeAttrNum,
					  pAttr->AeAttrSSchema) ||
			AttrHasException (ExcCssId, pAttr->AeAttrNum,
					  pAttr->AeAttrSSchema))
		      pSSattr = pDoc->DocSSchema;
		  /* process all values of the attribute, in case of a text
		     attribute with multiple values */
		  valNum = 1;
		  do
		    {
		      /* first rule for this value of the attribute */
		      pR = AttrPresRule (pAttr, pEl, FALSE, NULL, pSchPattr,
					 &valNum);
		      /* look for all rules associated with this value */
		      while (pR != NULL)
			{
			  ruleToApply = NULL;
			  if (!pR->PrDuplicate)
			    /* if it's a creation rule, apply it now */
			    if (!ApplCrRule (pR, pSSattr, pSchPattr, pAttr,
				   pAbbReturn, viewNb, pDoc, pEl, forward,
                                   lqueue, queuePR, queuePP, queuePS, queuePA,
                                   pNewAbbox))
			      /* not a creation rule, get the right rule */
			      ruleToApply = GetNextAttrPresRule (&pR,
                                                  pAttr->AeAttrSSchema, pAttr,
                                                  pEl, pDoc, pEl, view);
			  if (ruleToApply && DoesViewExist (pEl, pDoc, viewNb))
			    /* this rule applies to the element */
			    {
			      if (viewSch == 1 &&
				  ruleToApply->PrType != PtFunction)
				/* main view. Record the rule for the cascade*/
				/* but apply presentation functions
				   immediately */
				{
				  if (RuleHasHigherPriority (ruleToApply,
				           pSchPattr,
					   selectedRule[ruleToApply->PrType],
					   schemaOfSelectedRule[ruleToApply->PrType]))
				    {
				      selectedRule[ruleToApply->PrType] = ruleToApply;
				      schemaOfSelectedRule[ruleToApply->PrType] = pSchPattr;      
				      attrOfSelectedRule[ruleToApply->PrType] = pAttr;
				    }
				}
			      else
				/* not the main view, apply the rule now */
				if (!ApplyRule (ruleToApply, pSchPattr,
                                                pNewAbbox, pDoc, pAttr))
				  WaitingRule (ruleToApply, pNewAbbox,
                                        pSchPattr, pAttr, queuePA, queuePS,
                                        queuePP, queuePR, lqueue);
			    }
			  /* next rule associated with this value of the attr*/
			  pR = pR->PrNextPRule;
			}
		    }
		  while (valNum > 0);
		}
	      /* get the next attribute of the element */
	      pAttr = pAttr->AeNext;
	    }
	}
      /* next style sheet (P schema extension) */
      if (pHd)
	pHd = pHd->HdNextPSchema;
      else
	/* it was the main P schema, get the first schema extension */
	pHd = FirstPSchemaExtension (pEl->ElStructSchema, pDoc);
      if (pHd)
	pSchPres = pHd->HdPSchema;
      else
	/* no schema any more. stop */
	pSchPres = NULL;
    }

  /* get all specific presentation rules attached to the element itself */
  pRule = pEl->ElFirstPRule;
  while (pRule != NULL)
    /* get the rule if it applies to the view and if it's not a page height */
    {
      if (!(pEl->ElTypeNumber == PageBreak + 1 && pRule->PrType == PtHeight))
	if ((pRule->PrViewNum == viewSch || pRule->PrType == PtPictInfo) &&
	    DoesViewExist (pEl, pDoc, viewNb))
	  {
	    if (!selectedRule[pRule->PrType] ||
		(!selectedRule[pRule->PrType]->PrImportant &&
		 selectedRule[pRule->PrType]->PrSpecificity <= pRule->PrSpecificity))
	      {
		if (pRule->PrSpecifAttr == 0)
		  /* this rule does not depend on an attribute */
		  pAttr = NULL;
		else
		  /* look for the attribute related to the rule */
		  {
		    pAttr = pEl->ElFirstAttr;
		    stop = FALSE;
		    while (pAttr != NULL && !stop)
		      if (pAttr->AeAttrNum == pRule->PrSpecifAttr &&
			  !strcmp (pAttr->AeAttrSSchema->SsName,
				   pRule->PrSpecifAttrSSchema->SsName))
			stop = TRUE;
		      else
			pAttr = pAttr->AeNext;
		  }
		selectedRule[pRule->PrType] = pRule;
		schemaOfSelectedRule[pRule->PrType] = pSchP;      
		attrOfSelectedRule[pRule->PrType] = pAttr;
	      }
	  }
      pRule = pRule->PrNextPRule;
    }

  /* apply all selected rules */
  for (i = 0; i < PtPictInfo; i++)
    if (selectedRule[i])
      if (!ApplyRule (selectedRule[i], schemaOfSelectedRule[i], pNewAbbox,
		      pDoc, attrOfSelectedRule[i]))
	WaitingRule (selectedRule[i], pNewAbbox, schemaOfSelectedRule[i],
		     attrOfSelectedRule[i], queuePA, queuePS, queuePP,
		     queuePR, lqueue);
}

/*----------------------------------------------------------------------
   	Attach	chaine et remplit le pave pointe par pAb, correspondant
   		a l'element pointe par pEl dans la vue nv du document	
   		dont le contexte est pointe par pDoc.			
  ----------------------------------------------------------------------*/
static void Attach (PtrAbstractBox pAb, PtrElement pEl, DocViewNumber nv,
		    PtrDocument pDoc)
{
   PtrElement          pE;
   PtrAbstractBox      pP;
   ThotBool            stop;
   PtrAbstractBox      pPa1;
   PtrElement          pEl1;

   pPa1 = pAb;
   /* cherche dans cette vue le premier element ascendant qui ait un pave' */
   pP = NULL;
   pE = pEl;
   while (pE->ElParent != NULL && pP == NULL)
     {
	pE = pE->ElParent;
	pP = pE->ElAbstractBox[nv - 1];
     }
   if (pP == NULL)
      /* il n'y a pas de pave englobant pAb */
     {
	if (pEl->ElAbstractBox[nv - 1] == NULL)
	   pEl->ElAbstractBox[nv - 1] = pAb;
     }
   else
      /* pAb n'est pas le pave le plus englobant pour cette vue */
      /*  pP: 1er pave de l'element ascendant pour cette vue */
      /* saute les paves de presentation que l'ascendant a crees par la */
      /* regle CreateBefore */
     {
	while (pP->AbPresentationBox && pP->AbElement == pE && pP->AbNext != NULL)
	   pP = pP->AbNext;
	/* le premier pave qui n'est pas de presentation est l'englobant */
	pPa1->AbEnclosing = pP;
	if (pPa1->AbEnclosing->AbFirstEnclosed == NULL)
	   /* c'est le premier pave englobe */
	  {
	     pPa1->AbEnclosing->AbFirstEnclosed = pAb;
	     if (pEl->ElAbstractBox[nv - 1] == NULL)
		pEl->ElAbstractBox[nv - 1] = pAb;
	     /* 1er pave de l'element */
	  }
	else
	   /* il y a deja des paves englobes */
	   if (pEl->ElTypeNumber == PageBreak + 1
	       && pEl->ElPageType == PgBegin)
	   /* c'est une marque de page de debut d'element, on la chaine */
	   /* en tete */
	  {
	     pPa1->AbNext = pPa1->AbEnclosing->AbFirstEnclosed;
	     if (pPa1->AbNext != NULL)
		pPa1->AbNext->AbPrevious = pAb;
	     pPa1->AbEnclosing->AbFirstEnclosed = pAb;
	  }
	else
	  {
	     if (pEl->ElAbstractBox[nv - 1] != NULL)
		/* cet element a deja des paves dans cette vue */
		/* saute les paves de l'element */
	       {
		  pP = pEl->ElAbstractBox[nv - 1];
		  stop = FALSE;
		  do
		     if (pP->AbNext == NULL)
			stop = TRUE;
		     else if (pP->AbNext->AbElement != pEl)
			stop = TRUE;
		     else
			pP = pP->AbNext;
		  while (!(stop));
		  /* insere le nouveau pave apres */
		  pPa1->AbPrevious = pP;
		  pPa1->AbNext = pP->AbNext;
	       }
	     else
		/* cet element n'a pas encore de paves dans cette vue */
	       {
		  pEl->ElAbstractBox[nv - 1] = pAb;
		  /* 1er pave de l'element */
		  /* cherche l'element precedent ayant un pave dans la vue */
		  pE = BackSearchVisibleElem (pPa1->AbEnclosing->AbElement, pEl, nv);
		  if (pE != NULL)
		     /* verifie si le pave found pour un element precedent */
		     /* est bien inclus dans le meme pave englobant. */
		     /* Par exemple deux notes de bas de page successives */
		     /* peuvent avoir leurs paves dans des boites de bas de */
		     /* page differentes */
		    {
		       pP = pE->ElAbstractBox[nv - 1];
		       /* pave de l'element precedent */
		       do
			  pP = pP->AbEnclosing;
		       while (!(pP == pPa1->AbEnclosing || pP == NULL));
		       if (pP == NULL)
			  /* ils n'ont pas le meme pave englobant, on ne */
			  /* chainera pas le pave au pave de l'element precedent */
			  pE = NULL;
		    }
		  if (pE != NULL)
		     if (pE->ElTypeNumber == PageBreak + 1
			 && pE->ElPageType == PgBegin)
			/* le precedent est une marque de page de debut */
			/* d'element, on verifie si elle est suivie par des */
			/* paves de presentation de l'englobant */
		       {
			  pP = pE->ElAbstractBox[nv - 1];
			  /* pave de l'element precedent */
			  if (pP->AbNext != NULL)
			     if (pP->AbNext->AbElement ==
				 pPa1->AbEnclosing->AbElement)
				/* la marque de page est suivie par un pave cree */
				/* par l'englobant */
				pE = NULL;
		       }
		  if (pE == NULL)
		    /* pas de pave d'element precedent */
		    {
		      pP = pPa1->AbEnclosing->AbFirstEnclosed;
		      /* saute les eventuelles marques de page de debut */
		      /* d'element */
		      stop = FALSE;
		      do
			if (pP == NULL)
			  stop = TRUE;
			else
			  {
			    pEl1 = pP->AbElement;
			    if (pEl1->ElTypeNumber == PageBreak + 1 &&
				pEl1->ElPageType == PgBegin)
			      pP = pP->AbNext;
			    else
			      stop = TRUE;
			  }
		      while (!stop);
		      if (pP != NULL)
			{
			  if (pP->AbElement == pPa1->AbEnclosing->AbElement)
			    {
			      if (TypeCreatedRule (pDoc, pPa1->AbEnclosing, pP) == FnCreateLast)
				/* le pave existant doit etre le dernier, on insere */
				/* le nouveau pave devant lui */
				{
				  pPa1->AbNext = pP;
				  pPa1->AbEnclosing->AbFirstEnclosed = pAb;
				}
			      else
				/* on saute les paves crees par une regle */
				/* CreateFirst de l'englobant */
				{
				  stop = FALSE;
				  do
				    if (pP->AbNext == NULL)
				      stop = TRUE;
				    else if (pP->AbNext->AbElement !=
					     pPa1->AbEnclosing->AbElement)
				      stop = TRUE;
				    else if (TypeCreatedRule (pDoc,
							      pPa1->AbEnclosing, pP->AbNext) == FnCreateLast)
				      /* le pave suivant doit etre le dernier */
				      stop = TRUE;
				    else
				      pP = pP->AbNext;
				  while (!(stop));
				  /* on insere le nouveau pave apres */
				  pPa1->AbPrevious = pP;
				  pPa1->AbNext = pP->AbNext;
				}
			    }
			  else
			    /* insere le nouveau pave en tete */
			    {
			      pPa1->AbNext = pP;
			      pPa1->AbEnclosing->AbFirstEnclosed = pAb;
			    }
			}
		    }
		  else
		     /* il y a un pave d'un element precedent */
		    {
		       pP = pE->ElAbstractBox[nv - 1];
		       if (pP->AbEnclosing != NULL &&
			   pP->AbEnclosing->AbPresentationBox &&
			   pP->AbEnclosing->AbElement == pE)
			  /* cet element a cree' un pave englobant par la regle */
			  /* FnCreateEnclosing, on se place au niveau de ce pave' */
			  /* englobant */
			  pP = pP->AbEnclosing;
		       else
			 {
			    /* on cherche le dernier pave de l'element precedent */
			    stop = FALSE;
			    do
			       if (pP->AbNext == NULL)
				  stop = TRUE;
			       else if (pP->AbNext->AbElement != pE)
				  stop = TRUE;
			       else
				  pP = pP->AbNext;
			    while (!(stop));
			 }
		       /* insere le nouveau pave apres pP */
		       pPa1->AbPrevious = pP;
		       pPa1->AbNext = pP->AbNext;
		    }
	       }
	     if (pPa1->AbPrevious != NULL)
		pPa1->AbPrevious->AbNext = pAb;
	     if (pPa1->AbNext != NULL)
		pPa1->AbNext->AbPrevious = pAb;
	  }
     }
   if (pEl->ElHolophrast || (pEl->ElTerminal && pEl->ElLeafType != LtPageColBreak))
      /* met le contenu de l'element dans le pave, sauf si c'est un */
      /* element de haut ou de bas de page */
     {
	FillContent (pEl, pAb, pDoc);
	/* ajoute le volume du pave a celui de tous ses englobants */
	if (pPa1->AbVolume > 0)
	  {
	     pP = pPa1->AbEnclosing;
	     while (pP != NULL)
	       {
		  pP->AbVolume += pPa1->AbVolume;
		  pP = pP->AbEnclosing;
	       }
	  }
     }
   else
     {
	pPa1->AbLeafType = LtCompound;
	pPa1->AbVolume = 0;
	pPa1->AbInLine = FALSE;
	pPa1->AbTruncatedHead = TRUE;
	pPa1->AbTruncatedTail = TRUE;
     }
}

/*----------------------------------------------------------------------
   AbsBoxesCreate cree les paves correspondant au sous-arbre de         
   l'element pEl du document pDoc, uniquement pour la vue  
   viewNb. Certains paves peuvent deja exister; ils ne sont 
   pas touche's par AbsBoxesCreate. Si Desc est faux, seuls les 
   paves de l'element pEl sont crees, sinon, on cree les   
   paves de tout le sous arbre. La creation de paves       
   s'arrete lorsque tous les paves du sous-arbre sont      
   crees ou de`s qu'il n'y a plus de volume libre dans la  
   vue. En ElemIsBefore indique si la creation a lieu vers l'avant
   ou vers l'arriere. pAbbReturnne le dernier (dans l'ordre    
   defini par forward) pave de plus haut niveau cree'.     
   Au retour, complete indique si l'extremite' (indiquee    
   par forward) de l'image abstraite de l'element a pu     
   etre creee ou non.                                      
  ----------------------------------------------------------------------*/
PtrAbstractBox AbsBoxesCreate (PtrElement pEl, PtrDocument pDoc,
			       DocViewNumber viewNb, ThotBool forward,
			       ThotBool descent, ThotBool *complete)
{
   PtrPSchema          pSchP, pSchPPage, pSPres;
   PtrPRule            pRule, pRDef, pRSpec;
   PtrElement          pElChild, pElParent, pAsc;
   PtrAbstractBox      pAbb, pAbbChild, pNewAbbox, pAbbReturn, pAbbPres;
   PtrAbstractBox      pPRP, pAb1;
   PtrSSchema          pSchS, savePSS;
   PtrAttribute        pAttr;
   PtrPRule            queuePR[MAX_QUEUE_LEN];
   PtrAbstractBox      queuePP[MAX_QUEUE_LEN];
   PtrPSchema          queuePS[MAX_QUEUE_LEN];
   PtrAttribute        queuePA[MAX_QUEUE_LEN];
   int                 vis, typePres;
   int                 viewSch;
   int                 index;
   int                 lqueue, pqueue;
   ThotBool            completeChild;
   ThotBool            stop, ok, crAbsBox, truncate;
   ThotBool            notBreakable, ignoreDescent;
   ThotBool            Creation, ApplyRules;
   ThotBool            PcFirst, PcLast;

   pAbbReturn = NULL;
   lqueue = 0;
   pqueue = 0;
   /* Abstract boxes of the element are not created */
   *complete = FALSE;
   if (pEl != NULL && pEl->ElStructSchema != NULL)
     {
	viewSch = AppliedView (pEl, NULL, pDoc, viewNb);
	/* pointeur sur le pave qui sera cree' pour l'element */
	pNewAbbox = NULL;
	ApplyRules = FALSE;
	ignoreDescent = FALSE;
	pAbbPres = NULL;
	notBreakable = FALSE;
	/* pas tous crees */
	Creation = FALSE;	/* a priori rien a creer */
	pAb1 = pEl->ElAbstractBox[viewNb - 1];
	if (pAb1 != NULL && !pAb1->AbDead)
	  {
	    /* le pave existe deja pour cette vue */
	    Creation = FALSE;
	    /* on saute les paves de presentation crees par CreateWith */
	    while (pAb1->AbPresentationBox && pAb1->AbNext != NULL)
	      pAb1 = pAb1->AbNext;
	    if (pAb1->AbLeafType != LtCompound || pAb1->AbInLine)
	      /* c'est une feuille ou un pave' mis en lignes, il */
	      /* a deja tout son contenu */
	      *complete = TRUE;
	    else if (forward)
	      *complete = !pAb1->AbTruncatedTail;
	    else
	      *complete = !pAb1->AbTruncatedHead;
	    if (pAb1->AbSize == -1)
	      /* il faut lui appliquer ses regles de presentation */
	      ApplyRules = TRUE;
	  }
	else
	  {
	    /* le pave n'existe pas pour cette vue, on essaiera de le creer. */
	    Creation = TRUE;
	    /* on ne sait pas si on pourra creer toute son image */
	    *complete = FALSE;
	  }
	/* on ne cherche a creer que les paves qui n'existent pas deja */
	if (Creation || ApplyRules)
	  /* cherche le schema de presentation a appliquer */
	  {
	    SearchPresSchema (pEl, &pSchP, &index, &pSchS, pDoc);
	    /*********/
	    if (pSchS != NULL && pSchS != pEl->ElStructSchema)
	      {
	      /* il s'agit de l'element racine d'une nature qui utilise le
		 schema de presentation de son englobant */
	      if (pDoc->DocView[viewNb - 1].DvSSchema == pDoc->DocSSchema)
		{
		  /* il faut utiliser le schema de presentation de
		     l'environnement englobant. On reevalue la vue a appliquer */
		  savePSS = pEl->ElStructSchema;
		  pEl->ElStructSchema = pSchS;
		  viewSch = AppliedView (pEl, NULL, pDoc, viewNb);
		  pEl->ElStructSchema = savePSS;
		}
	      else
		{
		  pSchS = pEl->ElStructSchema;
		  pSchP = PresentationSchema (pSchS, pDoc);
		  index = pEl->ElTypeNumber;
		}
	      }
	    /*********/
	    /* pRSpec: premiere regle de presentation associee au type de
	       l'element */
	    pRSpec = pSchP->PsElemPRule->ElemPres[index - 1];
	    /* premiere regle de presentation par defaut */
	    pRDef = pSchP->PsFirstDefaultPRule;
	    /* initialise la file des regles qui n'ont pas pu etre appliquees*/
	    lqueue = 0;
	    pqueue = 0;
	    
	    /* determine la visibilite du pave a creer */
	    ComputeVisib (pEl, pDoc, viewNb, viewSch, &pRSpec, &pRDef, &vis,
			  &ignoreDescent, complete, &typePres, &pSchPPage);
	    
	    /* si l'element est visible dans cette vue ou si c'est la racine,*/
	    /* on cree son pave, si ce n'est deja fait */
	    if (vis >= 1 || pEl->ElParent == NULL)
	      {
	      if (ApplyRules)
		/* on applique seulement les regles de presentation a un */
		/* pave qui existe deja */
		/* pour appliquer les regles, on initialise pNewAbbox au */
		/* pave de l'element */
		{
		  pNewAbbox = pEl->ElAbstractBox[viewNb - 1];
		  pNewAbbox->AbVisibility = vis;
		  /* Si c'est un element racine qui n'a pas de descendants, */
		  /* alors son pave est complete */
		  if (descent &&
		      pEl->ElParent == NULL &&
		      (pEl->ElTerminal || pEl->ElFirstChild == NULL))
		    {
		      *complete = TRUE;
		      if (pNewAbbox->AbLeafType == LtCompound &&
			  !pNewAbbox->AbInLine)
			{
			if (forward)
			  /* on creera au moins le 1er pave inclus */
			  pNewAbbox->AbTruncatedHead = FALSE;
			else
			  /* on creera au moins le dernier pave inclus */
			  pNewAbbox->AbTruncatedTail = FALSE;
			}
		    }
		}
	      else
		/* cree et initialise un nouveau pave */
		/* plusieurs paves sont crees si l'element est une marque */
		/* de pages : paves corps de page, plus paves dupliques (V4)  */
		{
		  pNewAbbox = InitAbsBoxes (pEl, viewNb, vis, pDoc->DocReadOnly);
		  pNewAbbox->AbPSchema = pSchP;
		  if (pDoc->DocReadOnly)
		    {
		      /* document en lecture seule */
		      pNewAbbox->AbReadOnly = TRUE;
		      /* on rend les feuilles non modifiables par le Mediateur */
		      if (pEl->ElTerminal)
			pNewAbbox->AbCanBeModified = FALSE;
		    }
		  /* chaine le nouveau pave dans l'arbre de l'image abstraite */
		  Attach (pNewAbbox, pEl, viewNb, pDoc);
		  pAbbReturn = pNewAbbox;
		  if (descent)	/* on va creer les paves inclus */
		    {
		      pAb1 = pNewAbbox;
		      if (pAb1->AbLeafType == LtCompound)
			if (!pAb1->AbInLine)
			  if (forward)
			    /* on creera au moins le 1er pave inclus */
			    pAb1->AbTruncatedHead = FALSE;
			  else
			    /* on creera au moins le dernier pave inclus */
			    pAb1->AbTruncatedTail = FALSE;
			else
			  /* pave mis en ligne, on cree tout */
			  *complete = TRUE;
		      else
			/* pave feuille, on cree tout */
			*complete = TRUE;
		    }
		}
	      }
	  }
	
	if (pNewAbbox == NULL && Creation &&
	    !ignoreDescent && !pEl->ElHolophrast)
	  {
	    /* cet element n'est pas visible dans la vue, on cherche a creer */
	    /* creer les paves d'un descendant visible */
	    if (descent)
	      {
	      if (pEl->ElTerminal)
		*complete = TRUE;
	      else if (pEl->ElFirstChild == NULL)
		*complete = TRUE;
	      else
		/* cherche d'abord le premier descendant visible dans la vue */
		{
		  pElChild = DescVisible (pEl, viewNb, forward);
		  if (pElChild == NULL)
		    /* pas de descendant visible, on traite les fils */
		    {
		      pElChild = pEl->ElFirstChild;
		      /* premier fils de l'element cherche */
		      /* le dernier fils si la creation a lieu en arriere */
		      if (!forward && pElChild != NULL)
			while (pElChild->ElNext != NULL)
			  pElChild = pElChild->ElNext;
		    }
		  if (pElChild != NULL)
		    /* on cherche si un ascendant a un pave insecable dans la vue */
		    {
		      notBreakable = FALSE;		/* a priori, non */
		      pAsc = pEl;
		      pAbb = NULL;
		      while (pAsc != NULL && pAbb == NULL)
			{
			  pAbb = pAsc->ElAbstractBox[viewNb - 1];
			  /* on cherche le pave principal de cet ascendant dans la vue */
			  stop = FALSE;
			  do
			    if (pAbb == NULL)
			      stop = TRUE;
			    else if (pAbb->AbPresentationBox)
			      pAbb = pAbb->AbNext;
			    else
			      stop = TRUE;
			  while (!stop);
			  if (pAbb != NULL)
			    /* cet ascendant a un pave, est-il secable ? */
			    notBreakable = !(IsBreakable (pAbb, pDoc));
			  else
			    /* pas de pave, on passe a l'ascendant du dessus */
			    pAsc = pAsc->ElParent;
			}
		    }
		  while (pElChild != NULL)
		    /* cree les paves d'un descendant */
		    {
		      pAbb = AbsBoxesCreate (pElChild, pDoc, viewNb, forward, descent, &completeChild);
		      if (pAbb != NULL)
			pAbbReturn = pAbb;
		      /* passe au fils suivant ou precedent, selon le */
		      /* sens de creation */
		      if (forward)
			pElChild = pElChild->ElNext;
		      else
			pElChild = pElChild->ElPrevious;
		      if (pElChild == NULL)
			/* on a cree' les paves du dernier fils */
			*complete = completeChild;
		      else
			/* verifie que la vue n'est pas pleine, mais si un */
			/* ascendant est insecable, on continue quand meme */
			if (IsViewFull (viewNb, pDoc, pEl) && !notBreakable)
			  /* vue pleine, on arrete la creation des paves des fils */
			  pElChild = NULL;
		    }
		}
	      }
	  }			/* fin if (descent) */
	else if (!ignoreDescent)
	  /* l'element est visible dans la vue */
	  {
	    if ((Creation || ApplyRules) && descent)
	      {
		/* on applique toutes les regles de presentation pertinentes */
		ApplyPresRules (pEl, pDoc, viewNb, viewSch, pSchS, pSchP,
				&pRSpec, &pRDef, &pAbbReturn, forward, &lqueue,
				queuePR, queuePP, queuePS, queuePA, pNewAbbox);
		
		/* traitement particulier aux sauts de page (il faut prendre */
		/* le bon schema de presentation) */
		/* uniquement pour la V3 car dans la V4 les regles ont deja */
		/* ete appliquees apres duplication dans CreePageCol */
		if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak &&
		    !pEl->ElHolophrast)
		  /* c'est une marque de page */
		  if (pEl->ElViewPSchema == viewSch)
		    /* cette marque concerne la vue traitee */
		    ApplPage (pEl, pDoc, viewNb, viewSch, typePres, pSchPPage,
			      pNewAbbox);
	      }
	     if (pEl->ElTerminal)
		*complete = TRUE;
	     if (descent && !pEl->ElTerminal && !pEl->ElHolophrast)
	       /* cree les paves des fils de l'element, dans la limite */
	       /* de la capacite de la vue */
	       /* determine d'abord si l'element est secable et pour */
	       /* cela cherche le 1er pave de l'element qui ne soit pas */
	       /* un pave de presentation */
	       {
		 pAbbPres = NULL;
		 pAbb = pEl->ElAbstractBox[viewNb - 1];
		 stop = FALSE;
		 do
		   if (pAbb == NULL)
		     stop = TRUE;
		   else if (pAbb->AbPresentationBox)
		     pAbb = pAbb->AbNext;
		   else
		     stop = TRUE;
		 while (!stop);
		 notBreakable = !(IsBreakable (pAbb, pDoc));
		 /* determine le 1er pave fils a creer */
		 /* premier fils de l'element */
		 pElChild = pEl->ElFirstChild;
		 if (pElChild == NULL)
		   /* le pave n'a pas de descendance */
		   /* pave vide, il n'est pas coupe' */
		   {
		     *complete = TRUE;
		     pAbbPres = TruncateOrCompleteAbsBox (pNewAbbox, FALSE, (ThotBool)(!forward), pDoc);
		     if (pAbbPres != NULL)
		       /* on a cree des paves de presentation */
		       if (pAbbPres->AbEnclosing != pNewAbbox)
			 pAbbReturn = pAbbPres;
		   }
		 else if (Creation || notBreakable)
		   /* il faut creer les paves de tous les fils */
		   /* creation en avant: on commence par le 1er fils */
		   /* creation en reculant: commence par le dernier fils */
		   {
		     if (!forward)
		       {
			 stop = FALSE;
			 do
			   if (pElChild == NULL)
			     stop = TRUE;
			   else if (pElChild->ElNext == NULL)
			     stop = TRUE;
			   else
			     pElChild = pElChild->ElNext;
			 while (!stop);
		       }
		   }
		 else
		   /* il y a peut-etre deja un pave fils */
		   /* on ne cree des paves qu'a partir de l'element descendant */
		   /* qui possede deja un pave dans la vue */
		   /* cherche le premier descendant qui ait un pave dans la vue */
		   {
		     pElChild = DescVisible (pEl, viewNb, forward);
		     if (pElChild == NULL)
		       /* aucun descendant n'a de pave dans la vue */
		       /* on cree les paves de tous les fils */
		       {
			 pElChild = pEl->ElFirstChild;
			 /* premier fils de l'element */
			 if (!forward)
			   /* creation en reculant ; cherche le dernier fils */
			   {
			     stop = FALSE;
			     do
			       if (pElChild == NULL)
				 stop = TRUE;
			       else if (pElChild->ElNext == NULL)
				 stop = TRUE;
			       else
				 pElChild = pElChild->ElNext;
			     while (!stop);
			   }
		       }
		   }

		 /* cree les paves des fils successifs */
		 while (pElChild != NULL)
		   /* verifie que la vue n'est pas pleine */
		   if (IsViewFull (viewNb, pDoc, pEl) && !notBreakable)
		     /* vue pleine, on arrete la creation des paves des fils */
		     {
		       pElChild = NULL;
		       /* le pave englobant des fils est incomplet */
		       pElParent = pEl;
		       /* cherche le 1er ascendant visible dans la vue */
		       stop = FALSE;
		       do
			 if (pElParent == NULL)
			   stop = TRUE;
			 else if (pElParent->ElAbstractBox[viewNb - 1] != NULL)
			   stop = TRUE;
			 else
			   pElParent = pElParent->ElParent;
		       while (!stop);
		       /* marque ce pave coupe' */
		       if (pElParent != NULL)
			 pAbbPres = TruncateOrCompleteAbsBox (pElParent->ElAbstractBox[viewNb - 1], TRUE, (ThotBool)(!forward), pDoc);
		     }
		   else
		     /* Cree les paves d'un fils et passe a un autre fils */
		     {
		       /* a priori on va creer les paves du fils */
		       ok = TRUE;
		       if (!ApplyRules)
			 /* verifie si le fils est deja complete */
			 if (pElChild->ElAbstractBox[viewNb - 1] != NULL)
			   /* le fils a deja des paves dans cette vue */
			   {
			     pAbbChild = pElChild->ElAbstractBox[viewNb - 1];
			     /* saute les paves de presentation du fils */
			     stop = FALSE;
			     do
			       if (pAbbChild->AbNext == NULL)
				 stop = TRUE;
			       else if (pAbbChild->AbPresentationBox)
				 pAbbChild = pAbbChild->AbNext;
			       else
				 stop = TRUE;
			     while (!stop);
			     /* on appellera AbsBoxesCreate s'il faut appliquer les */
			     /* regles de presentation de ce fils */
			     if (pAbbChild->AbSize != -1)
			       {
			       if (pAbbChild->AbLeafType != LtCompound ||
				   pAbbChild->AbInLine)
				 ok = FALSE;	/* pave fils complete */
			       else if (forward)
				 {
				   if (!pAbbChild->AbTruncatedTail)
				     ok = FALSE;	/* pave fils complete */
				 }
			       else if (!pAbbChild->AbTruncatedHead)
				 ok = FALSE;	/* pave fils complete */
			       }
			   }
		       if (ok)
			 /* on cree effectivement les paves du fils */
			 pAbbChild = AbsBoxesCreate (pElChild, pDoc, viewNb, forward, descent, &completeChild);
		       else
			 /* le pave du fils etait deja complete */
			 {
			   pAbbChild = NULL;
			   completeChild = TRUE;
			 }

		       if (pNewAbbox == NULL)
			 if (pAbbChild != NULL)
			   pAbbReturn = pAbbChild;
		       if (forward)
			 {
			   PcLast = (pElChild->ElNext == NULL);
			   PcFirst = FALSE;
			 }
		       else
			 {
			   PcFirst = (pElChild->ElPrevious == NULL);
			   PcLast = FALSE;
			 }
		       pAsc = pElChild->ElParent;
		       while (pAsc != pEl)
			 {
			   if (PcFirst)
			     PcFirst = (pAsc->ElPrevious == NULL);
			   if (PcLast)
			     PcLast = (pAsc->ElNext == NULL);
			   pAsc = pAsc->ElParent;
			 }
		       
		       pElParent = pElChild->ElParent;
		       if (forward)
			 if (pElChild->ElNext != NULL)
			   /* passe au fils suivant */
			   pElChild = pElChild->ElNext;
			 else
			   /* pas de fils suivant */
			   {
			     /* on cherche le premier englobant qui ne soit pas */
			     /* l'element traite' et qui ait un voisin */
			     stop = FALSE;
			     do
			       if (pElChild->ElParent == pEl)
				 /* on traitait le pere, on s'arrete */
				 {
				   pElChild = NULL;
				   stop = TRUE;
				 }
			       else
				 /* on traite le frere suivant du pere */
				 {
				   pElChild = pElChild->ElParent;
				   if (pElChild == NULL)
				     stop = TRUE;
				   else if (pElChild->ElNext != NULL)
				     {
				       pElChild = pElChild->ElNext;
				       stop = TRUE;
				     }
				 }
			     while (!stop);
			   }
		       else if (pElChild->ElPrevious != NULL)
			 pElChild = pElChild->ElPrevious;
		       /* passe au fils precedent */
		       else
			 /* pas de fils precedent */
			 {
			   /* on cherche le premier englobant qui ne soit */
			   /* pas l'element traite' et qui ait un voisin */
			   stop = FALSE;
			   do
			     if (pElChild->ElParent == pEl)
			       /* on traitait le pere, on s'arrete */
			       {
				 pElChild = NULL;
				 stop = TRUE;
			       }
			     else
			       /* on traite le frere precedent du pere */
			       {
				 pElChild = pElChild->ElParent;
				 if (pElChild == NULL)
				   stop = TRUE;
				 else if (pElChild->ElPrevious != NULL)
				   {
				     pElChild = pElChild->ElPrevious;
				     stop = TRUE;
				   }
			       }
			   while (!stop);
			 }
		       if (PcLast || PcFirst)
			 /* on a cree' les paves du premier (ou dernier) fils */
			 /* de pEl, le pave englobant des fils de pEl est  */
			 /* complete si le pave du premier (ou dernier) fils est */
			 /* complete ou s'il ne contient rien (tous ses */
			 /* descendants ont une visibilite' nulle) */
			 {
			   stop = FALSE;
			   do
			     if (pElParent != NULL)
			       {
			       if (pElParent->ElAbstractBox[viewNb - 1] != NULL)
				 stop = TRUE;
			       else if ((forward && pElParent->ElNext == NULL)
					|| (!forward
					    && pElParent->ElPrevious == NULL))
				 pElParent = pElParent->ElParent;
			       else
				 pElParent = NULL;
			       }
			   while (!stop && pElParent != NULL);
			   if (pElParent != NULL)
			     {
			       pAbb = pElParent->ElAbstractBox[viewNb - 1];
			       /* saute les paves de presentation crees par */
			       /* FnCreateBefore */
			       stop = FALSE;
			       do
				 if (pAbb == NULL)
				   stop = TRUE;
				 else if (!pAbb->AbPresentationBox)
				   stop = TRUE;
				 else
				   pAbb = pAbb->AbNext;
			       while (!stop);
			       if (pAbb != NULL)
				 {
				   pAbbChild = pAbb->AbFirstEnclosed;
				   pAbbPres = NULL;
				   if (PcLast)
				     {
				       if (pAbbChild == NULL)
					 /* tous les descendants ont une visibilite' */
					 /* nulle; le pave est donc complete en queue */
					 truncate = FALSE;
				       else
					 {
					   /* cherche le dernier pave' fils */
					   while (pAbbChild->AbNext != NULL)
					     pAbbChild = pAbbChild->AbNext;
					   /* ignore les paves de presentation */
					   while (pAbbChild->AbPresentationBox &&
						  pAbbChild->AbPrevious != NULL)
					     pAbbChild = pAbbChild->AbPrevious;
					   if (pAbbChild->AbDead)
					     truncate = TRUE;
					   else
					     truncate = !completeChild;
					   
					 }
				       pAbbPres = TruncateOrCompleteAbsBox (pAbb, truncate, FALSE, pDoc);
				       if (forward && !truncate)
					 *complete = TRUE;
				     }
				   if (PcFirst)
				     {
				       pAbbChild = pAbb->AbFirstEnclosed;
				       if (pAbbChild == NULL)
					 /* tous les descendants ont une visibilite' */
					 /* nulle ; le pave est donc complete en tete */
					 truncate = FALSE;
				       else
					 {
					   /* ignore les paves de presentation */
					   while (pAbbChild->AbPresentationBox &&
						  pAbbChild->AbNext != NULL)
					     pAbbChild = pAbbChild->AbNext;
					   if (pAbbChild->AbDead)
					     truncate = TRUE;
					   else
					     truncate = !completeChild;
					 }
				       pAbbPres = TruncateOrCompleteAbsBox (pAbb, truncate, TRUE, pDoc);
				       if (!forward && !truncate)
					 *complete = TRUE;
				     }
				   if (pAbbPres != NULL &&
				     /* on a cree des paves de presentation */
				       (!Creation || pAbbPres->AbEnclosing != pAbb))
				     pAbbReturn = pAbbPres;
				 }
			     }
			 }
		     }
		 /* fin: Cree les paves d'un fils et passe a un autre fils */
		 /* met les valeurs par defaut des axes de reference si aucune */
		 /* regle de positionnement des axes de reference n'a ete  */
		 /* appliquee */
		 if (Creation)
		   if (pNewAbbox != NULL)
		     {
		       if (pNewAbbox->AbVertRef.PosAbRef == NULL)
			 pNewAbbox->AbVertRef.PosAbRef = pNewAbbox->AbFirstEnclosed;
		       if (pNewAbbox->AbHorizRef.PosAbRef == NULL)
			 pNewAbbox->AbHorizRef.PosAbRef = pNewAbbox->AbFirstEnclosed;
		     }
	       }		/* fin creation des fils */
	     if ((Creation || ApplyRules) && descent)
	       /* applique les regles en attente */
	       do
		 {
		   GetAtt (&pRule, &pAbb, &pSPres, &pAttr, queuePA, queuePS, queuePP,
			   queuePR, &lqueue, &pqueue);
		   /* recupere une regle en attente */
		   if (pRule != NULL)
		     {
		       crAbsBox = FALSE;
		       /* a priori ce n'est pas une regle de creation */
		       if (pRule->PrType == PtFunction)
			 /* on applique les regles de creation systematiquement:
			    on a verifie' lors de la mise en attente que le pave
			    createur n'etait pas coupe'. */
			 {
			   if (pRule->PrPresFunction == FnCreateBefore
			       || pRule->PrPresFunction == FnCreateWith
			       || pRule->PrPresFunction == FnCreateAfter
			       || pRule->PrPresFunction == FnCreateEnclosing
			       || pRule->PrPresFunction == FnCreateFirst
			       || pRule->PrPresFunction == FnCreateLast)
			     {
			       crAbsBox = TRUE;
			       if (pAttr != NULL)
				 pAbbPres = CrAbsBoxesPres (pEl, pDoc, pRule,
							    pAttr->AeAttrSSchema, pAttr, viewNb,
							    PresentationSchema (pAttr->AeAttrSSchema, pDoc),
							    TRUE);
			       else
				 pAbbPres = CrAbsBoxesPres (pEl, pDoc, pRule,
							    pEl->ElStructSchema, NULL, viewNb,
							    pSPres, TRUE);
			     }
			   switch (pRule->PrPresFunction)
			     {
			     case FnCreateBefore:
			       if (!forward && pAbbPres != NULL &&
				   pEl->ElParent)
				 pAbbReturn = pAbbPres;
			       break;
			     case FnCreateAfter:
			     case FnCreateWith:
			       if (forward && pAbbPres != NULL &&
				   pEl->ElParent)
				 pAbbReturn = pAbbPres;
			       break;
			     case FnCreateEnclosing:
			       pAbbReturn = pAbbPres;
			       break;
			     default:
			       break;
			     }
			 }
		       if (!crAbsBox)
			 /* ce n'est pas une regle de creation */
			 if (!ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr))
			   Delay (pRule, pSPres, pAbb, pAttr, pAbb);
		     }
		 }
	       while (pRule != NULL);
	     /* applique toutes les regles en retard des descendants */
	     if (descent)
	       {
		 pAbb = pEl->ElAbstractBox[viewNb - 1];
		 /* saute les paves de presentation crees par FnCreateBefore */
		 stop = FALSE;
		 do
		   if (pAbb == NULL)
		     stop = TRUE;
		   else if (!pAbb->AbPresentationBox)
		     stop = TRUE;
		   else if (pAbb->AbFirstEnclosed != NULL &&
			    pAbb->AbFirstEnclosed->AbElement == pEl)
		     /* pave cree' par la regle FnCreateEnclosing */
		     stop = TRUE;
		   else
		     pAbb = pAbb->AbNext;
		 while (!stop);
		 do
		   {
		     pPRP = pAbb;
		     GetDelayedRule (&pRule, &pSPres, &pPRP, &pAttr);
		     if (pRule != NULL)
		       if (!ApplyRule (pRule, pSPres, pPRP, pDoc, pAttr))
			 /* cette regle n'a pas pu etre appliquee. C'est  */
			 /* une regle correspondant a un attribut, on */
			 /* l'appliquera lorsque l'englobant sera complete */
			 Delay (pRule, pSPres, pPRP, pAttr, pAbb);
		   }
		 while (pRule != NULL);
	       }
	  }
	/* fin de !ignoreDescent */
     }
   return pAbbReturn;
}
