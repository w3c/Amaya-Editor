/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module generates abstract boxes
 *
 * Authors: V. Quint (INRIA)
 *          C. Roisin (INRIA) - Columns and pages
 *
 */
#include "thot_gui.h"
#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "content.h"
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
#include "displayview_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "presvariables_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structlist_f.h"
#include "structmodif_f.h"
#include "tree_f.h"

#ifdef _GL
#include "animbox_f.h"
#endif /* _GL */

typedef struct _AllRules
{
  /* for each type of presentation property, selectedRule[i] records the
     latest rule selected while applying the cascade.
     schemaOfSelectedRule records the presentation schema the corresponding
     rule comes from */
  PtrPRule           selectedRule[PtPictInfo];
  PtrPSchema         schemaOfSelectedRule[PtPictInfo];
  PtrAttribute       attrOfSelectedRule[PtPictInfo];
  PtrAttributePres   attrBlockOfSelectedRule[PtPictInfo];  
} AllRules;

typedef struct _Cascade
{
  AllRules     MainElement;

  PtrPRule     ContentRuleBefore;
  PtrPSchema   schemaOfContentRuleBefore;
  PtrAttribute attrOfContentRuleBefore;
  AllRules     BeforePseudoEl;

  PtrPRule     ContentRuleAfter;
  PtrPSchema   schemaOfContentRuleAfter;
  PtrAttribute attrOfContentRuleAfter;
  AllRules     AfterPseudoEl;
} Cascade;


typedef struct _RuleQueue
{
  PtrPRule         queuePR[MAX_QUEUE_LEN];
  PtrPSchema       queuePS[MAX_QUEUE_LEN];
  PtrAttribute     queuePA[MAX_QUEUE_LEN];
  PtrAbstractBox   queuePP[MAX_QUEUE_LEN];
  AllRules*        rulesPseudo[MAX_QUEUE_LEN];
} RuleQueue;

static  PtrPRule         PriorRuleV;
static  PtrPSchema       PriorschemaOfRuleV;
static  PtrAttribute     PriorattrOfRuleV;
static  PtrAttributePres PriorattrBlockOfRuleV;  
static  PtrPRule         PriorRuleD;
static  PtrPSchema       PriorschemaOfRuleD;
static  PtrAttribute     PriorattrOfRuleD;
static  PtrAttributePres PriorattrBlockOfRuleD;  

static PresRule ListItemVisibility, ListItemListStyleType, ListItemListStyleImage, ListItemListStylePosition, ListItemVertPos, ListItemHorizPos, ListItemMarginRight, ListItemMarginLeft, ListItemSize, ListItemStyle, ListItemPtWeight, ListItemVariant, ListItemFont, ListItemOpacity, ListItemDirection, ListItemBackground, ListItemForeground;

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
  ----------------------------------------------------------------------*/
static void UpdateCSSVisibility (PtrAbstractBox pAb)
{
  // @@@@@@@@@ Not sure we should do that here
  if (pAb->AbVis == 'I')
    {
      if (pAb->AbEnclosing)
        pAb->AbVis = pAb->AbEnclosing->AbVis;
      else
        pAb->AbVis = 'V';
    }
  if (pAb->AbVis == 'C' &&
      !TypeHasException (ExcIsColHead,pAb->AbElement->ElTypeNumber,
                         pAb->AbElement->ElStructSchema) &&
      !TypeHasException (ExcIsRow, pAb->AbElement->ElTypeNumber,
                         pAb->AbElement->ElStructSchema))
    pAb->AbVis = 'V';
}

/*----------------------------------------------------------------------
  CanApplyCSSToElement returns TRUE when CSS rules apply to this element
  ----------------------------------------------------------------------*/
ThotBool CanApplyCSSToElement (PtrElement pEl)
{
  return (pEl &&
          pEl->ElTypeNumber >= pEl->ElStructSchema->SsRootElem &&
          strcmp (pEl->ElStructSchema->SsName, "Template") && // not template element
          !TypeHasException (ExcHidden, pEl->ElTypeNumber, pEl->ElStructSchema));
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
            if (pAb)
              {
                SetAbsBoxAccessMode (pAb, accessMode);
                h = 0;
                ChangeConcreteImage (pDoc->DocViewFrame[view], &h, pAb);
              }
          }
      /* Redisplay views */
      RedisplayDocViews (pDoc);
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
                  CondPresentation ((*pRSpecif)->PrCond, pEl, pAttr, pEl,
				    *pRSpecif, 1, pSS, pDoc))
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
                                        pAttr, pEl, *pRSpecif, 1, pSS, pDoc))
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
PtrPRule GetRuleView (PtrPRule *pRSpecif, PtrPRule *pRDefault,
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
                                    *pRSpecif, Vue, pSS, pDoc))
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
  pAb->AbVarNum = 0;
  pAb->AbNum = 0;
  pAb->AbVertRef.PosEdge = VertRef;
  pAb->AbVertRef.PosRefEdge = VertRef;
  pAb->AbVertRef.PosDistance = 0;
  pAb->AbVertRef.PosDistDelta = 0;
  pAb->AbVertRef.PosUnit = UnRelative;
  pAb->AbVertRef.PosDeltaUnit = UnRelative;
  pAb->AbVertRef.PosAbRef = NULL;
  pAb->AbVertRef.PosUserSpecified = FALSE;

  pAb->AbHorizRef.PosEdge = HorizRef;
  pAb->AbHorizRef.PosRefEdge = HorizRef;
  pAb->AbHorizRef.PosDistance = 0;
  pAb->AbHorizRef.PosDistDelta = 0;
  pAb->AbHorizRef.PosUnit = UnRelative;
  pAb->AbHorizRef.PosDeltaUnit = UnRelative;
  pAb->AbHorizRef.PosAbRef = NULL;
  pAb->AbHorizRef.PosUserSpecified = FALSE;

  pAb->AbVertPos.PosEdge = Top;
  pAb->AbVertPos.PosRefEdge = Top;
  pAb->AbVertPos.PosDistance = 0;
  pAb->AbVertPos.PosDistDelta = 0;
  pAb->AbVertPos.PosUnit = UnRelative;
  pAb->AbVertPos.PosDeltaUnit = UnRelative;
  pAb->AbVertPos.PosAbRef = NULL;
  pAb->AbVertPos.PosUserSpecified = FALSE;

  pAb->AbHorizPos.PosEdge = Left;
  pAb->AbHorizPos.PosRefEdge = Left;
  pAb->AbHorizPos.PosDistance = 0;
  pAb->AbHorizPos.PosDistDelta = 0;
  pAb->AbHorizPos.PosUnit = UnRelative;
  pAb->AbHorizPos.PosDeltaUnit = UnRelative;
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
  pAb->AbFontVariant = 1;
  pAb->AbSize = 1;
  pAb->AbLineWeight = 1;
  pAb->AbLineSpacing = 10;
  pAb->AbFillPattern = 0;
  pAb->AbBackground = DefaultBColor;	/* white */
  pAb->AbGradientBackground = FALSE;
  pAb->AbForeground = DefaultFColor;	/* black */
  pAb->AbColor = DefaultFColor;         /* black */
  pAb->AbStopColor = DefaultFColor;
  pAb->AbStopOpacity = 1000;   /* Full opaque*/
  pAb->AbOpacity = 1000;       /* Full opaque*/
  pAb->AbFillOpacity = 1000;   /* Full opaque*/
  pAb->AbStrokeOpacity = 1000; /* Full opaque*/
  pAb->AbMarker = NULL;        /* none */
  pAb->AbMarkerStart = NULL;   /* none */
  pAb->AbMarkerMid = NULL;     /* none */
  pAb->AbMarkerEnd = NULL;     /* none */
  pAb->AbFillRule = 'n';       /* fill-rule: nonzero */
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
  pAb->AbDisplay = 'U';
  pAb->AbVis = 'I';
  pAb->AbListStyleType = 'D';
  pAb->AbListStylePosition = 'O';
  pAb->AbListStyleImage = 'N';
  pAb->AbFloat = 'N';
  pAb->AbClear = 'N';
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
    pAb->AbReadOnly = TRUE;
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
  pAb->AbFloatChange = FALSE;
  pAb->AbChange = FALSE;

  pAb->AbInLine = FALSE/*TRUE*/;
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
      if (pConst->PdString)
        pAb->AbShape = (char)pConst->PdString[0];
      else
        pAb->AbShape = EOS;
      if (pAb->AbShape == 1 || pAb->AbShape == 'C')
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
      if (pConst->PdString)
        pAb->AbShape = (char) pConst->PdString[0];
      else
        pAb->AbShape = EOS;
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
      CopyStringToBuffer ((unsigned char *)pConst->PdString, pAb->AbText, &l);
      pAb->AbLang = TtaGetDefaultLanguage ();
      pAb->AbVolume = pAb->AbText->BuLength;
      break;
    case tt_Picture:
      NewPictInfo (pAb, pConst->PdString, UNKNOWN_FORMAT, False);
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
static void Delay (PtrPRule pR, PtrPSchema pSP, PtrAbstractBox pAbb,
                   PtrAttribute pAttr)
{
  PtrDelayedPRule     pDelR;
  PtrDelayedPRule     NpDelR;
  PtrAbstractBox      pAb;
  PtrElement          pAncest;
  int                 view;

  pAb = NULL;
  if (pAbb->AbEnclosing)
    {
      if (/* is it a position relatively to a referred element? */
          ((pR->PrType == PtHorizPos || pR->PrType == PtVertPos) &&
           pR->PrPosRule.PoRelation == RlReferred) ||

          /* is it the width or height... */
          ((pR->PrType == PtWidth || pR->PrType == PtHeight) &&
           /* ... of a rubber band box... */
           ((pR->PrDimRule.DrPosition &&
             pR->PrDimRule.DrPosRule.PoRelation == RlReferred) ||
            /* ... or defined by a reference to another element? */
            (!pR->PrDimRule.DrPosition && pR->PrDimRule.DrSameDimens &&
             pR->PrDimRule.DrRelation == RlReferred))))
        {
          if (pAttr && pAttr->AeAttrType == AtReferenceAttr &&
              pAttr->AeAttrReference &&
              pAttr->AeAttrReference->RdReferred &&
              pAttr->AeAttrReference->RdReferred->ReReferredElem)
            /* the rule is associated with a reference attribute that actually
               refers to an element */
            {
              /* get the first ancestor of this referred element that has an
                 abstract box in the same view */
              view = pAbb->AbDocView;
              pAncest = pAttr->AeAttrReference->RdReferred->ReReferredElem->ElParent;
              while (pAncest && !pAncest->ElAbstractBox[view - 1])
                pAncest = pAncest->ElParent;
              if (pAncest)
                /* link the rule to the abstract box of this ancestor for later
                   evaluation */
                pAb = pAncest->ElAbstractBox[view - 1];
            }
        }
      if (pAb == NULL)
        pAb = pAbb->AbEnclosing;
      /* si ce pave est un pave de presentation cree par la regle */
      /* FnCreateEnclosing, on met la regle en attente sur le pave englobant */
      if (pAb->AbEnclosing &&
          (pAb->AbPresentationBox &&
           pAb->AbElement == pAbb->AbElement))
        pAb = pAb->AbEnclosing;
      else if (pR->PrPresMode == PresInherit &&
               pR->PrInheritMode == InheritGrandFather &&
               pAb->AbEnclosing)
        pAb = pAb->AbEnclosing;
    }

  if (pAb)
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
void GetDelayedRule (PtrPRule *pR, PtrPSchema *pSP, PtrAbstractBox *pAbb,
                     PtrAttribute * pAttr)
{
  PtrDelayedPRule     pDelR;
  PtrAbstractBox      pAb;
  ThotBool            stop;
  PtrElement          pEl;

  pAb = *pAbb;
  /* a priori, pas de regle retardee */
  *pR = NULL;
  *pSP = NULL;
  *pAbb = NULL;
  *pAttr = NULL;
  if (pAb)
    {
      /* cherche le pave de l'element dans cette vue */
      /* saute les paves de presentation */
      stop = FALSE;
      pEl = pAb->AbElement;
      do
        if (pAb == NULL)
          stop = TRUE;
        else if (pAb->AbElement != pEl)
          {
            stop = TRUE;
            pAb = NULL;
          }
        else if (!pAb->AbPresentationBox)
          stop = TRUE;
        else
          pAb = pAb->AbNext;
      while (!stop);
      if (pAb)
        {
          pDelR = pAb->AbDelayedPRule;
          if (pDelR)
            {
              *pR = pDelR->DpPRule;
              *pSP = pDelR->DpPSchema;
              *pAbb = pDelR->DpAbsBox;
              *pAttr = pDelR->DpAttribute;
              pAb->AbDelayedPRule = pDelR->DpNext;
              FreeDifferedRule (pDelR);
            }
        }
    }
}

/*----------------------------------------------------------------------
  ApplyDelayedRules applies delayed rules of type ruleType (any
  type if ruleType is -1) attached to the current pAb abstact box.
  ----------------------------------------------------------------------*/
ThotBool ApplyDelayedRules (int ruleType, PtrAbstractBox pAb, PtrDocument pDoc)
{
  PtrDelayedPRule     pDelR, prev = NULL, next;
  ThotBool            stop;
  PtrElement          pEl;

  if (pAb)
    {
      /* cherche le pave de l'element dans cette vue */
      /* saute les paves de presentation */
      stop = FALSE;
      pEl = pAb->AbElement;
      do
        if (pAb == NULL)
          stop = TRUE;
        else if (pAb->AbElement != pEl)
          {
            stop = TRUE;
            pAb = NULL;
          }
        else if (!pAb->AbPresentationBox)
          stop = TRUE;
        else
          pAb = pAb->AbNext;
      while (!stop);

      if (pAb && pAb->AbDelayedPRule)
        {
          pDelR = pAb->AbDelayedPRule;
          while (pDelR)
            {
              next = pDelR->DpNext;
              if (pDelR->DpPRule &&
                  (ruleType == -1 || pDelR->DpPRule->PrType == ruleType))
                {
                  // try to apply that rule
                  if (ApplyRule (pDelR->DpPRule,
                                 pDelR->DpPSchema,
                                 pDelR->DpAbsBox, pDoc,
                                 pDelR->DpAttribute, pAb))
                    {
                      // that rule is now applied
                      if (prev)
                        prev->DpNext = pDelR->DpNext;
                      else
                        pAb->AbDelayedPRule = pDelR->DpNext;
                      FreeDifferedRule (pDelR);
                    }
                  else
                    prev = pDelR;
                }
              else
                prev = pDelR;
              // next delayed rule
              pDelR = next;
            }
        }
    }
  return TRUE;
}


/*----------------------------------------------------------------------
  ApplDelayedRule applique les regles retardees conservees pour  
  les paves de l'element El du document pDoc.             
  ----------------------------------------------------------------------*/
void ApplDelayedRule (PtrElement pEl, PtrDocument pDoc)
{
  PtrPRule            pRule = NULL;
  PtrPSchema          pSPres;
  PtrAttribute        pAttr;
  PtrAbstractBox      pAb, pAbb;
  int                 view;

  if (pEl == NULL || pDoc == NULL)
    return;
  for (view = 0; view < MAX_VIEW_DOC; view++)
    {
      pAb = pEl->ElAbstractBox[view];
      if (pAb)
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
                if (pRule &&
                    ApplyRule (pRule, pSPres, pAbb, pDoc, pAttr, pAb))
                  if (pAbb->AbElement != pEl && !pAbb->AbNew)
                    SetChange (pAbb, pDoc, pRule->PrType,
                               (FunctionType)pRule->PrPresFunction);
              }
            while (pRule);
          }
    }
}

/*----------------------------------------------------------------------
  ElemHasCondAttribute
  Check whether element pEl has an attribute that satisfies condition pCond.
  ----------------------------------------------------------------------*/
static ThotBool ElemHasCondAttribute (PtrElement pEl, PtrCondition pCond,
                                      PtrSSchema pSS)
{
  PtrAttribute        pA;
  unsigned char       attrVal[MAX_TXT_LEN];
  int                 i, j;
  ThotBool            found;

  pA = pEl->ElFirstAttr;
  found = FALSE;
  while (pA)
    /* check all attributes of the element */
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
            found = TRUE;
          else
            /* check the attribute value */
            {
              if (pA->AeAttrType != AtTextAttr)
                /* compare integer values */
                found = (pCond->CoAttrValue == pA->AeAttrValue);
              else
                /* it's a text attribute. Compare strings */
                {
                  if (!pA->AeAttrText)
                    /* empty attribute */
                    found = (pCond->CoAttrTextValue == NULL ||
                             pCond->CoAttrTextValue[0] == EOS);
                  else
                    {
                      CopyBuffer2MBs (pA->AeAttrText, 0, attrVal,
                                      MAX_TXT_LEN);
                      /* test the attribute value */
                      j = 0; i = 0;
                      found = FALSE;
                      if (pCond->CoTextMatch == CoSubstring)
                        /* compare strings up to an hyphen */
                        {
                          while (pCond->CoAttrTextValue[i] != EOS &&
                                 attrVal[i] != EOS &&
                                 attrVal[i] == pCond->CoAttrTextValue[i])
                            i++;
                          found = (pCond->CoAttrTextValue[i] == EOS &&
                                   (attrVal[i] == '-' ||
                                    attrVal[i] == EOS));
                        }
                      else
                        {
                          while (!found && attrVal[j] != EOS)
                            {
                              i = 0;
                              while (pCond->CoAttrTextValue[i] != EOS &&
                                     attrVal[j + i] == pCond->CoAttrTextValue[i])
                                i++;
                              found = (pCond->CoAttrTextValue[i] == EOS);
                              if (found)
                                {
                                  if (pCond->CoTextMatch == CoWord)
                                    {
                                      /* check if a word matches */
                                      i += j;
                                      found = (j == 0 || attrVal[j - 1] == SPACE) &&
                                        (attrVal[i] == EOS || attrVal[i] == SPACE);
                                    }
                                  else if (pCond->CoTextMatch == CoMatch)
                                    /* the whole attribute value must be equal */
                                    found = (attrVal[j + i] == EOS && j == 0);
                                }
                              /* prepare next search */
                              j++;
                            }
                        }
                    }
                }
            }
          /* don't check other attributes for this element */
          pA = NULL;
        }
    }
  return found;
}


/*----------------------------------------------------------------------
  ElemDoesNotCount
  return TRUE if element pEl is a Page break, a Comment, a PI, a text string
  or a hidden element that contains only such elements. 
  ----------------------------------------------------------------------*/
ThotBool ElemDoesNotCount (PtrElement pEl, ThotBool previous)
{
  ThotBool    ignore = FALSE;
  PtrElement  pChild = NULL;

  if (pEl->ElTypeNumber == PageBreak + 1)
    /* ignore a page break */
    ignore = TRUE;
  else if (pEl->ElTerminal && (pEl->ElLeafType == LtText))
    /* ignore a text leaf */
    ignore = TRUE;
  else if (TypeHasException (ExcNotAnElementNode, pEl->ElTypeNumber,
                             pEl->ElStructSchema))
    /* ignore a PI or a comment */
    ignore = TRUE;
  else if (TypeHasException (ExcHidden, pEl->ElTypeNumber,
                             pEl->ElStructSchema))
    /* hidden elements do not count, but their children count */
    {
      if (pEl->ElParent &&
          TypeHasException (ExcNotAnElementNode, pEl->ElParent->ElTypeNumber,
                            pEl->ElParent->ElStructSchema))
        /* it's a line within a comment or a PI. It counts: the P schema
           puts a "<-- " in front of the first line */
        ignore = FALSE;
      else
        /* check the children */
        if (pEl->ElTerminal || !pEl->ElFirstChild)
          /* no child. Ignore */
          ignore = TRUE;
        else
          {
            pChild = pEl->ElFirstChild;
            if (previous)
              /* get the last child */
              {
                while (pChild->ElNext)
                  pChild = pChild->ElNext;
              }
            do
              {
                ignore = ElemDoesNotCount (pChild, previous);
                if (previous)
                  pChild = pChild->ElPrevious;
                else
                  pChild = pChild->ElNext;
              }
            while (pChild && ignore);
          }
    }
  return ignore;
}

/*----------------------------------------------------------------------
  SiblingElement return the element node which is the previous or next
  (it depends on parameter previous) sibling of element pEl.
  Page breaks, Comments, PIs, text strings etc. are ignored.
  ----------------------------------------------------------------------*/
PtrElement SiblingElement (PtrElement pEl, ThotBool previous)
{
  PtrElement    pSibling;
  ThotBool      stop;

  if (!pEl)
    return NULL;
  if (previous)
    pSibling = pEl->ElPrevious;
  else
    pSibling = pEl->ElNext;
  stop = FALSE;
  do
    if (pSibling == NULL)
      stop = TRUE;
    else if (ElemDoesNotCount (pSibling, previous))
      {
        if (previous)
          pSibling = pSibling->ElPrevious;
        else
          pSibling = pSibling->ElNext;
      }
    else
      stop = TRUE;
  while (!stop);
  return pSibling;
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
ThotBool CondPresentation (PtrCondition pCond, PtrElement pEl,
                           PtrAttribute pAttr, PtrElement pElAttr,
                           PtrPRule pRule, int view, PtrSSchema pSS,
                           PtrDocument pDoc)
{
  PtrPSchema          pSchP = NULL;
  PtrElement          pAsc = NULL, pElem, pRoot;
  PtrReference        pRef;
  PtrCondition        firstCondLevel;
  int                 valcompt, valmaxi, valmini;
  int                 i = 0;
  PtrSRule            pRe1;
  ThotBool            ok, found, equal;

  /* a priori les conditions sont satisfaites */
  firstCondLevel = NULL;
  ok = TRUE;
  found = FALSE;
  /* on examine toutes les conditions de la chaine */
  while (pCond && ok)
    {
      if (!pCond->CoTarget)
        pElem = pEl;
      else
        /* la condition porte sur l'element pointe' par pEl ou pAttr.
           on cherche cet element pointe' */
        {
          pElem = NULL;
          if (pAttr != NULL &&
              pAttr->AeAttrSSchema->SsAttribute->TtAttr[pAttr->AeAttrNum - 1]->AttrType == AtReferenceAttr)
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
            pElem = ReferredElement (pRef);
        }
      valcompt = 0;
      if (pElem == NULL)
        ok = FALSE;
      else if (pCond->CoCondition == PcEven || pCond->CoCondition == PcOdd ||
               pCond->CoCondition == PcOne || pCond->CoCondition == PcInterval)
        /* evalue le compteur */
        {
          pSchP = PresentationSchema (pSS, pDoc);
          if (pSchP)
            valcompt = CounterVal (pCond->CoCounter, pSS, pSchP, pElem, view,
				   pDoc);
        }
      if (pElem)
        switch (pCond->CoCondition)
          {
          case PcFirst:
            found = (SiblingElement (pElem, TRUE) == NULL);
            break;

          case PcLast:
            found = (SiblingElement (pElem, FALSE) == NULL);
            break;
       
          case PcReferred:
            /* la condition est satisfaite si l'element (ou le */
            /* premier de ses ascendants sur lequel peut porter une */
            /* reference) est reference' au moins une fois. */
            found = FALSE;
            pAsc = pElem;
            do
              {
                if (pAsc->ElReferredDescr != NULL)
                  /* l'element est reference' */
                  found = TRUE;
                if (!found)
                  /* l'element n'est pas reference' */
                  /* on examine l'element ascendant */
                  pAsc = pAsc->ElParent;
              }
            while (pAsc != NULL && !found);
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
                found = pEl->ElReference->RdPrevious == NULL;
              else
                found = pEl->ElReference->RdNext == NULL;
            else
              found = FALSE;
            break;

          case PcExternalRef:
          case PcInternalRef:
            /* la condition est satisfaite s'il s'agit d'un */
            /* element ou d'un attribut reference externe (ou interne) */
            pRef = NULL;
            found = FALSE;
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
              found = FALSE;
            else
              found = TRUE;
            break;

          case PcCopyRef:
            /* la condition est satisfaite si l'element est une copie */
            found = pElem->ElIsCopy;
            break;

          case PcAnyAttributes:
            /* la condition est satisfaite si l'element */
            /* porte des attributs */
            found = pElem->ElFirstAttr != NULL;
            break;

          case PcFirstAttr:
            /* TODO */
            /* la condition est satisfaite si le bloc */
            /* attribut pAttr est le 1er de l'element */
            if (pAttr && pElAttr)
              found = pAttr == pElAttr->ElFirstAttr;
            break;

          case PcLastAttr:
            /* TODO */
            /* la condition est satisfaite si le bloc     */
            /* attribut pAttr est le dernier de l'element */
            if (pAttr)
              found = pAttr->AeNext == NULL;
            break;

          case PcUserPage:
            /* la condition est satisfaite si l'element
               est un saut de page utilisateur */
            if (pElem->ElTypeNumber == PageBreak + 1)
              found = pElem->ElPageType == PgUser;
            break;

          case PcStartPage:
            /* la condition est satisfaite si l'element
               est un saut de page de debut */
            if (pElem->ElTypeNumber == PageBreak + 1)
              found = pElem->ElPageType == PgBegin;
            break;

          case PcComputedPage:
            /* la condition est satisfaite si l'element
               est un saut de page calcule */
            if (pElem->ElTypeNumber == PageBreak + 1)
              found = pElem->ElPageType == PgComputed;
            break;

          case PcEmpty:
            /* la condition est satisfaite si l'element est vide */
            if (pElem->ElTerminal)
              if (pElem->ElLeafType == LtReference)
                found = pElem->ElReference == NULL;
              else
                found = pElem->ElVolume == 0;
            else
              found = pElem->ElFirstChild == NULL;
            break;

          case PcRoot:
            /* la condition est satisfaite si le parent de l'element est
               le document lui-meme */
            found = (pElem->ElParent &&
                     pElem->ElParent->ElTypeNumber ==
                     pElem->ElParent->ElStructSchema->SsDocument);
            break;

          case PcEven:
            found = !(valcompt & 1);
            break;

          case PcOdd:
            found = (valcompt & 1);
            break;

          case PcOne:
            found = (valcompt == 1);
            break;

          case PcInterval:
            if (pCond->CoValCounter == CntMinVal)
              {
                /* Calcule la valeur mini du compteur */
                valmini = CounterValMinMax (pCond->CoCounter, pSS, pSchP,
                                            pElem, view, FALSE);
                found = (valmini <= pCond->CoMaxCounter) &&
                  (valmini >= pCond->CoMinCounter);
              }
            else if (pCond->CoValCounter == CntMaxVal)
              {
                /* Calcule la valeur maxi du compteur */
                valmaxi = CounterValMinMax (pCond->CoCounter, pSS, pSchP,
                                            pElem, view, TRUE);
                found = (valmaxi <= pCond->CoMaxCounter) &&
                  (valmaxi >= pCond->CoMinCounter);
              }
            else
              /* Calcule la valeur courante du compteur */
              found = (valcompt <= pCond->CoMaxCounter) &&
                (valcompt >= pCond->CoMinCounter);
            break;

          case PcWithin:
          case PcSibling:
            /* condition sur le nombre d'ancetres d'un type donne' */
            if (pCond->CoCondition == PcWithin)
              pAsc = pElem->ElParent;
            else
              pAsc = pElem->ElPrevious;
            if (pAsc == NULL)
              /* aucun ancetre, condition non satisfaite */
              found = FALSE;
            else if (pAsc->ElTypeNumber == pAsc->ElStructSchema->SsDocument)
              /* this ancestor is the meta-root. It does not count */
              {
                found = FALSE;
                pAsc = NULL;
              }
            else
              {
                i = 0;
                found = FALSE;
                if (pCond->CoImmediate)
                  /* Condition: If immediately within n element-type */
                  /* Les n premiers ancetres successifs doivent etre du */
                  /* type CoTypeAncestor, sans comporter d'elements */
                  /* d'autres type */
                  /* on compte les ancetres ou freres successifs de ce type */
                  while (pAsc && !found)
                    {
                      if (pRule &&
			  (pRule->PrCSSURL || pRule->PrCSSLine != 0) &&
                          (TypeHasException (ExcHidden, pAsc->ElTypeNumber,
                                             pAsc->ElStructSchema) ||
                          (pAsc->ElStructSchema &&
                           !strcmp (pAsc->ElStructSchema->SsName, "Template"))))
                        /* this ancestor is hidden. Skip it */
                        if (pCond->CoCondition == PcWithin)
                          pAsc = pAsc->ElParent;
                        else
                          pAsc = pAsc->ElPrevious;
                      else
                        {
                          if (pCond->CoTypeAncestor != 0)
                            /* compare type numbers */
                            equal = ((pCond->CoTypeAncestor == AnyType+1 ||
                                      pAsc->ElTypeNumber == pCond->CoTypeAncestor) &&
                                     !strcmp (pAsc->ElStructSchema->SsName,
                                              pSS->SsName));
                          else
                            /* compare type names */
                            equal = (pCond->CoAncestorName &&
                                     pAsc->ElStructSchema->SsRule->SrElem[pAsc->ElTypeNumber - 1]->SrName &&
                                     !strcmp (pCond->CoAncestorName,
                                              pAsc->ElStructSchema->SsRule->SrElem[pAsc->ElTypeNumber - 1]->SrName) &&
                                     !strcmp (pCond->CoSSchemaName,
                                              pAsc->ElStructSchema->SsName));
                          if (equal)
                            /* same types */
                            {
                              i++;
                              if (pCond->CoAncestorRel == CondGreater &&
                                  pCond->CoRelation == 0 &&
                                  pCond->CoChangeElem)
                                found = TRUE;
                              else
                                {
                                  if (pCond->CoCondition == PcWithin)
                                    pAsc = pAsc->ElParent;
                                  else
                                    pAsc = pAsc->ElPrevious;
                                }
                            }
                          else
                            pAsc = NULL;
                        }
                    }
                else
                  /* Condition: If within (or after) n elements */
                  /* count all ancestors (or previous siblings) of that type */
                  while (pAsc && !found)
                    {
                      if (pCond->CoTypeAncestor != 0)
                        /* compare type numbers */
                        equal = ((pCond->CoTypeAncestor == AnyType+1 ||
                                  pAsc->ElTypeNumber == pCond->CoTypeAncestor) &&
                                 !strcmp (pAsc->ElStructSchema->SsName,
                                          pSS->SsName));
                      else
                        /* compare type names */
                        equal = (pCond->CoAncestorName &&
                                 pAsc->ElStructSchema->SsRule->SrElem[pAsc->ElTypeNumber - 1]->SrName &&
                                 !strcmp (pCond->CoAncestorName,
                                          pAsc->ElStructSchema->SsRule->SrElem[pAsc->ElTypeNumber - 1]->SrName) &&
                                 !strcmp (pCond->CoSSchemaName,
                                          pAsc->ElStructSchema->SsName)); 
                      if (equal)
                        {
                          i++;
                          if (pCond->CoAncestorRel == CondGreater &&
                              pCond->CoRelation == 0 &&
                              pCond->CoChangeElem)
                            found = TRUE;
                        }
                      if (!found)
                        {
                          if (pCond->CoCondition == PcWithin)
                            pAsc = pAsc->ElParent;
                          else
                            pAsc = pAsc->ElPrevious;
                        }
                    }
                if (found)
                  {
                    if (pCond->CoChangeElem)
                      pEl = pAsc;
                  }
                else
                  {
                    if (pCond->CoAncestorRel == CondEquals)
                      found = i == pCond->CoRelation;
                    else if (pCond->CoAncestorRel == CondGreater)
                      found = i > pCond->CoRelation;
                    else if (pCond->CoAncestorRel == CondLess)
                      found = i < pCond->CoRelation;
                  }
              }
            break;

          case PcElemType:
            /* verifie si l'attribut est attache' a un element du
               type voulu */
            found = (pCond->CoTypeElem == AnyType+1 ||
                     (pElAttr && pElAttr->ElTypeNumber == pCond->CoTypeElem));
            if (!found)
              /* if the element type is a choice, check all options of the
                 choice */
              {
                pRe1 = pSS->SsRule->SrElem[pCond->CoTypeElem - 1];
                if (pRe1->SrConstruct == CsChoice && pRe1->SrNChoices > 0)
                  {
                    i = 0;
                    do
                      {
                        if (pRe1->SrChoice[i] == pElAttr->ElTypeNumber)
                          found = TRUE;
                        else
                          i++;
                      }
                    while (!found && i < pRe1->SrNChoices);
                  }
              }
            break;

          case PcInheritAttribute:
            /* verifie si l'element ou un de ses ascendants possede cet
               attribut, eventuellement avec la valeur voulue */
            pAsc = pElem;
            found = FALSE;
            while (pAsc)
              {
                found = ElemHasCondAttribute (pAsc, pCond, pSS);
                if (found)
                  /* don't look further */
                  pAsc = NULL;
                /* if the attribute has not been encountered yet, check next
                   ancestor */
                if (pAsc)
                  pAsc = pAsc->ElParent; 
              } 
            break;

          case PcAttribute:
            /* verifie si l'element possede cet attribut */
            found = ElemHasCondAttribute (pElem, pCond, pSS);
            /* as it's impossible to set an attribute to the PAGE */
            if (!found && pElem->ElTypeNumber == PageBreak + 1)
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
                           (pRoot->ElTypeNumber != pElem->ElStructSchema->SsRootElem ||
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
                found = ElemHasCondAttribute (pRoot, pCond, pSS);
              }
            break;

          case PcNoCondition:
            found = TRUE;
            break;
	    
          case PcDefaultCond:
            found = TRUE;
            break;
          }

      if (!pCond->CoNotNegative)
        found = !found;
      ok = ok && found;

      if (pCond->CoCondition == PcWithin || pCond->CoCondition == PcSibling)
	{
	  if (!pCond->CoImmediate && pCond->CoChangeElem && pAsc)
	    /* The condition we have just processed is the first at its level
	       in the CSS selector. Remember it */
	    firstCondLevel = pCond;
	  else
	    firstCondLevel = NULL;
	}

      if (ok)
        pCond = pCond->CoNextCondition;
      else
        /* the current condition is not satisfied */
        if (firstCondLevel)
          /* it is part of a series of conditions starting with PcWithin
             or PcSibling (not immediate). Try to apply this series of
             condition with the next ancestor or sibling */
          {
            pCond = firstCondLevel;
            ok = TRUE;
          }
    }

  return ok;
}

/*----------------------------------------------------------------------
  GetPreviousSibling
  returns the sibling element before element el, ignoring Template
  elements.
  ----------------------------------------------------------------------*/
static PtrElement GetPreviousSibling (PtrElement el)
{
  PtrSSchema           elSS;
  PtrElement           sibling, child, ancestor, prev;

  sibling = el;
  if (sibling)
    {
      elSS = el->ElStructSchema;
      do
        {
          sibling = sibling->ElPrevious;
          if (sibling)
            {
              if (!strcmp (sibling->ElStructSchema->SsName, "Template"))
                /* it's a Template element. Look for its first descendant that
                   is not a Template element */
                {
                  child = sibling;
                  do
                    {
                      if (child->ElTerminal)
                        child = NULL;
                      else
                        child = child->ElFirstChild;
                    }
                  while (child &&
                         !strcmp (child->ElStructSchema->SsName, "Template"));
                  if (child)
                    return child;
                  else
                    // ignore empty template elements
                    return GetPreviousSibling (sibling);
                }
              else
                return sibling;
            }
          else
            /* no sibling. If the ancestor is a Template element, find the last
               ancestor that is a Template element and take its next sibling */
            {
              ancestor = el->ElParent;
              prev = NULL;
              while (ancestor)
                {
                  if (strcmp (ancestor->ElStructSchema->SsName,"Template"))
                    /* this ancestor is not a Template element */
                    {
                      /* take the sibling of the previous ancestor */
                      if (prev)
                        sibling = prev->ElPrevious;
                      else
                        sibling = NULL;
                      ancestor = NULL;
                    }
                  else
                    {
                      /* this ancestor is a Template element. Remember it and
                         get the next ancestor */
                      prev = ancestor;
                      sibling = ancestor->ElPrevious;
                      if (!sibling)
                        ancestor = ancestor->ElParent;
                      else
                        {
                          if (strcmp (sibling->ElStructSchema->SsName, "Template"))
                            /* not a template element */
                            return sibling;
                          else
                            /* it's a Template element. Look for its first
                               descendant that is not a Template element */
                            {
                              child = sibling;
                              do
                                {
                                  if (child->ElTerminal)
                                    child = NULL;
                                  else
                                    child = child->ElFirstChild;
                                }
                              while (child &&
                                     !strcmp (child->ElStructSchema->SsName, "Template"));
                              return child;
                            } 
                        }
                    }
                }
            }
        } 
      while (sibling);
    }
  return sibling;
}

/*----------------------------------------------------------------------
  ComputeListItemNumber
  Compute the value of the presentation box representing a list item counter.
  Return TRUE if the value has changed.
  ----------------------------------------------------------------------*/
ThotBool ComputeListItemNumber (PtrAbstractBox pAb)
{
  char             number[20];
  int              count, length;
  CounterStyle     countStyle;
  PtrElement       pPrev, pAsc;
  PtrAttribute     pAttr;
  ThotBool         change, set;

  count = 0;
  set = FALSE;
  pPrev = pAb->AbElement;
  while (pPrev && !set)
    {
      if (pPrev->ElStructSchema == pAb->AbElement->ElStructSchema &&
          pPrev->ElTypeNumber == pAb->AbElement->ElTypeNumber)
        /* this element should be counted */
        {
          pAttr = GetAttrElementWithException (ExcSetCounter, pPrev);
          /* it has an attribute that sets the counter */
          if (pAttr && pAttr->AeAttrType == AtNumAttr)
            {
              set = TRUE;
              count += pAttr->AeAttrValue;
            }
          else
            count++;
        }
      pPrev = GetPreviousSibling (pPrev);
    }
  if (!set && pAb->AbElement && pAb->AbElement->ElParent)
    {
      /* get the parent element */
      pAsc = pAb->AbElement->ElParent;
      /* skip enclosing Template elements to find the real parent */
      while (pAsc && !strcmp (pAsc->ElStructSchema->SsName, "Template"))
        pAsc = pAsc->ElParent;
      pAttr = GetAttrElementWithException (ExcStartCounter, pAsc);
      if (pAttr && pAttr->AeAttrType == AtNumAttr)
        /* the parent element sets an initial value to the counter */
        {
          count += pAttr->AeAttrValue;
          count--;
        }
    }

  if (pAb->AbListStyleType == '1')
    countStyle = CntDecimal;
  else if (pAb->AbListStyleType == 'Z')
    countStyle = CntZLDecimal;
  else if (pAb->AbListStyleType == 'i')
    countStyle = CntLRoman;
  else if (pAb->AbListStyleType == 'I')
    countStyle = CntURoman;
  else if (pAb->AbListStyleType == 'g')
    countStyle = CntLGreek;
  else if (pAb->AbListStyleType == 'a')
    countStyle = CntLowercase;
  else if (pAb->AbListStyleType == 'A')
    countStyle = CntUppercase;
  else
    countStyle = CntDecimal;
  GetCounterValue (count, countStyle, number, &length);
  number[length++] = '.';
  number[length] = EOS;
  if (StringAndTextEqual (number, pAb->AbText))
    change = FALSE;
  else
    {
      change = TRUE;
      ClearText (pAb->AbText);
      CopyStringToBuffer ((unsigned char *)number, pAb->AbText, &length);
      pAb->AbVolume = length;
    }
  return change;
}

/*----------------------------------------------------------------------
  ListItemAbsBox   return the abstract box representing element pEl
  in the main view of document pDoc.
  ----------------------------------------------------------------------*/
static PtrAbstractBox ListItemAbsBox (PtrElement pEl, PtrDocument pDoc)
{
  int view;

  for (view = 1; view <= MAX_VIEW_DOC; view++)
    if (pDoc->DocView[view - 1].DvPSchemaView == 1)
      break;
  if (view <= MAX_VIEW_DOC)
    return (pEl->ElAbstractBox[view - 1]);
  else
    return(NULL);
}

/*----------------------------------------------------------------------
  SearchRuleListItemMarker return the rule of type ruleType that should
  be applied to a list item marker.
  ----------------------------------------------------------------------*/
PtrPRule SearchRuleListItemMarker (PRuleType ruleType, PtrElement pEl,
                                   PtrDocument pDoc)
{
  PtrPRule          pRule;
  PtrAbstractBox    pAb;

  pRule = NULL;
  switch (ruleType)
    {
    case PtVisibility:
    case PtVis:
      pRule = &ListItemVisibility;
      break;
    case PtListStyleType:
      pRule = &ListItemListStyleType;
      break;
    case PtListStyleImage:
      pRule = &ListItemListStyleImage;
      break;
    case PtListStylePosition:
      pRule = &ListItemListStylePosition;
      break;
    case PtVertPos:
      pRule = &ListItemVertPos;
      break;
    case PtHorizPos:
      pRule = &ListItemHorizPos;
      break;
    case PtMarginRight:
      pRule = &ListItemMarginRight;
      break;
    case PtMarginLeft:
      pRule = &ListItemMarginLeft;
      break;
    case PtSize:
      pRule = &ListItemSize;
      break;
    case PtStyle:
      pRule = &ListItemStyle;
      break;
    case PtWeight:
      pRule = &ListItemPtWeight;
      break;
    case PtVariant:
      pRule = &ListItemVariant;
      break;
    case PtFont:
      pRule = &ListItemFont;
      break;
    case PtDirection:
      pRule = &ListItemDirection;
      break;
    case PtBackground:
      pRule = &ListItemBackground;
      break;
    case PtForeground:
      pRule = &ListItemForeground;
      break;
    case PtOpacity:
      pRule = &ListItemOpacity;
      break;
    default:
      pRule = NULL;
      break;
    }
  if (pRule)
    {
      pRule->PrType = ruleType;
      pRule->PrNextPRule = NULL;
      pRule->PrCond = NULL;
      pRule->PrSpecifAttrSSchema = NULL;
      pRule->PrCSSURL = NULL;
      pRule->PrCSSLine = 0;
      pRule->PrViewNum = 1;
      pRule->PrSpecifAttr = 0;
      pRule->PrSpecificity = 0;
      pRule->PrImportant = FALSE;
      pRule->PrDuplicate = FALSE;
      switch (ruleType)
        {
        case PtVertPos:
          pAb = ListItemAbsBox (pEl, pDoc);
          if (pAb && pAb->AbListStylePosition == 'O')
            /* VertPos: HRef = Creator . HRef; */
            {
              pRule->PrPresMode = PresImmediate;
              pRule->PrPosRule.PoPosDef = HorizRef;
              pRule->PrPosRule.PoPosRef = HorizRef;
              pRule->PrPosRule.PoDistAttr = FALSE;
              pRule->PrPosRule.PoDistance = 0;
              pRule->PrPosRule.PoRelation = RlCreator;
            }
          else
            pRule = NULL;
          break;
        case PtHorizPos:
          pAb = ListItemAbsBox (pEl, pDoc);
          if (pAb && pAb->AbListStylePosition == 'O')
            {
              pRule->PrPresMode = PresImmediate;
              pRule->PrPosRule.PoDistAttr = FALSE;
              pRule->PrPosRule.PoDistance = 0;
              pRule->PrPosRule.PoRelation = RlCreator;
              if (pAb->AbDirection == 'L')
                /* direction is left to right */
                /* HorizPos: Right = Creator . Left */
                {
                  pRule->PrPosRule.PoPosDef = Right;
                  pRule->PrPosRule.PoPosRef = Left;
                }
              else
                /* direction is right to left */
                /* HorizPos: Left = Creator . Right */
                {
                  pRule->PrPosRule.PoPosDef = Left;
                  pRule->PrPosRule.PoPosRef = Right;		  
                }
            }
          else
            pRule = NULL;
          break;
        case PtMarginRight:
        case PtMarginLeft:
          pRule->PrMinUnit = UnRelative;
          pRule->PrMinAttr = FALSE;
          pRule->PrMinValue = 0;
          pAb = ListItemAbsBox (pEl, pDoc);
          if (pAb &&
              ((ruleType == PtMarginRight && pAb->AbDirection == 'L') ||
               (ruleType == PtMarginLeft && pAb->AbDirection == 'R')))
            pRule->PrMinValue = 6;
          break;
        case PtVisibility:
        case PtVis:
          /* Visibility: Creator = */
        case PtListStyleType:
          /* ListStyleType: Creator =; */
        case PtListStyleImage:
          /* ListStyleImage: Creator =; */
        case PtListStylePosition:
          /* ListStylePosition: Creator =; */
        case PtSize:
          /* Size: Creator = */
        case PtStyle:
          /* Style: Creator = */
        case PtWeight:
          /* Weight: Creator = */
        case PtVariant:
          /* Variant: Creator = */
        case PtFont:
          /* Font: Creator = */
        case PtBackground:
          /* Background: Creator =; */
        case PtForeground:
          /* Foreground: Creator =; */
        case PtOpacity:
          /* Opacity: Creator =; */
        case PtDirection:
          /* Direction: Creator = */
          pRule->PrPresMode = PresInherit;
          pRule->PrInheritMode = InheritCreator;
          pRule->PrInhPercent = FALSE;
          pRule->PrInhAttr = FALSE;
          pRule->PrInhDelta = 0;
          pRule->PrMinMaxAttr = FALSE;
          pRule->PrInhMinOrMax = 0;
          pRule->PrInhUnit = UnRelative;
          break;
        default:
          pRule = NULL;
          break; 
        }
    }
  return (pRule);
}

/*----------------------------------------------------------------------
  CreateListItemMarker generate a presentation box representing a
  list item marker for box pAb which has "display: list-item".
  ----------------------------------------------------------------------*/
ThotBool CreateListItemMarker (PtrAbstractBox pAb, PtrDocument pDoc,
                               PtrAttribute pAttr)
{
  PtrElement       pEl;
  PtrAbstractBox   pMarkerAb, pDescAb, pNextAb, pAscAb;
  PtrPRule         pRule;
  DocViewNumber    viewNb;
  ThotPictInfo    *imageDesc;
  ThotBool         stop, ok, done;

  if (pAb->AbListStyleType == 'N')
    /* a rule "list-style-type: none" applies */
    return TRUE;
  viewNb = pAb->AbDocView;
  pEl = pAb->AbElement;
  if (pAb->AbListStylePosition == 'I')
    /* list-style-position: inside */
    {
      if (pEl->ElHolophrast || pEl->ElTerminal)
        return TRUE;
      if (pAb->AbFirstEnclosed == NULL)
        /* no abstract box for the descendant of the element. Can't apply
           rule yet */
        return FALSE;
    }
  if (pAb->AbListStylePosition == 'O')
    /* list-style-position: outside */
    if (pAb->AbEnclosing == NULL)
      /* can't create a sibling box if it's the root box */
      return TRUE;
  if (pDoc->DocViewSubTree[viewNb -1] != NULL)
    /* this view only displays a sub-tree */
    if (ElemIsAnAncestor (pEl, pDoc->DocViewSubTree[viewNb -1]))
      /* the element is an ancestor of the displayed sub-tree */
      return TRUE;

  /* create a presentation box for the list item marker */
  pMarkerAb = InitAbsBoxes (pEl, viewNb, pAb->AbVisibility, pDoc->DocReadOnly);
  ok = TRUE;
  /* insert the presentation box in the box tree */
  if (pAb->AbListStylePosition == 'I')
    /* list-style-position: inside */
    {
      /* get the first grand child box that is not a presentation box and
         insert the marker box right before it */
      /* @@@@@ this should be improved to get the first enclosed box
         that is an inline box */
      pDescAb = pAb->AbFirstEnclosed;
      /* skip presentation boxes and page boxes */
      stop = FALSE;
      do
        {
          if (pDescAb == NULL)
            stop = TRUE;
          else
            if (pDescAb->AbPresentationBox && pDescAb->AbTypeNum == 0 &&
                !pDescAb->AbDead)
              /* the marker box already exists */
              {
                stop = TRUE;
                ok = FALSE;
              }
            else if (!pDescAb->AbPresentationBox && !pDescAb->AbDead &&
                     pDescAb->AbElement->ElTypeNumber != PageBreak + 1)
              stop = TRUE;
            else
              pDescAb = pDescAb->AbNext;
        }
      while (!stop);

      if (ok && pDescAb)
        {
          done = FALSE;
          if (pDescAb->AbFirstEnclosed == NULL) 
            /* pDescAb is empty */
            {
              if (!pDescAb->AbElement->ElTerminal)
                /* insert the marker box as the first child of pDescAb */
                {
                  pMarkerAb->AbEnclosing = pDescAb;
                  pDescAb->AbFirstEnclosed = pMarkerAb;
                  done = TRUE;
                }
            }
          else
            {
              pDescAb = pDescAb->AbFirstEnclosed;
              stop = FALSE;
              do
                {
                  if (pDescAb == NULL)
                    stop = TRUE;
                  else
                    if (pDescAb->AbPresentationBox && pDescAb->AbTypeNum == 0 &&
                        pAb->AbListStyleType == pMarkerAb->AbListStyleType &&
                        !pDescAb->AbDead)
                      /* the marker box already exists */
                      {
                        stop = TRUE;
                        ok = FALSE;
                      }
                    else if (!pDescAb->AbPresentationBox && !pDescAb->AbDead &&
                             pDescAb->AbElement->ElTypeNumber != PageBreak + 1)
                      stop = TRUE;
                    else
                      pDescAb = pDescAb->AbNext;
                }
              while (!stop);
            }
          if (ok && !done)
            {
              if (pDescAb)
                {
                  /* insert the marker box in the box tree */
                  pMarkerAb->AbEnclosing = pDescAb->AbEnclosing;
                  if (pMarkerAb->AbEnclosing->AbFirstEnclosed == pDescAb)
                    pMarkerAb->AbEnclosing->AbFirstEnclosed = pMarkerAb;
                  pMarkerAb->AbNext = pDescAb;
                  pMarkerAb->AbPrevious = pDescAb->AbPrevious;
                  pDescAb->AbPrevious = pMarkerAb;
                  if (pMarkerAb->AbPrevious)
                    pMarkerAb->AbPrevious->AbNext = pDescAb;
                }
              else
                ok = FALSE;
            }
        }
    }
  else
    /* list-style-position: outside */
    {
      /* get the main box of the element and insert the marker box right
         before */
      stop = FALSE;
      pNextAb = pEl->ElAbstractBox[viewNb - 1];
      do
        {
          if (pNextAb->AbPresentationBox && pNextAb->AbTypeNum == 0 &&
              !pNextAb->AbDead)
            /* the marker box already exists */
            {
              stop = TRUE;
              ok = FALSE;
            }
          else if ((pNextAb->AbNext == NULL) ||
                   (!pNextAb->AbPresentationBox && !pNextAb->AbDead) ||
                   AttrIsAfter (pNextAb->AbCreatorAttr, pAttr))
            stop = TRUE;
          else
            pNextAb = pNextAb->AbNext;
        }
      while (!stop);
      if (ok)
        {
          /* insert the marker box in the box tree */
          pMarkerAb->AbEnclosing = pNextAb->AbEnclosing;
          UpdateCSSVisibility (pMarkerAb);
          if (pMarkerAb->AbEnclosing->AbFirstEnclosed == pNextAb)
            pMarkerAb->AbEnclosing->AbFirstEnclosed = pMarkerAb;
          pMarkerAb->AbNext = pNextAb;
          pMarkerAb->AbPrevious = pNextAb->AbPrevious;
          pNextAb->AbPrevious = pMarkerAb;
          if (pMarkerAb->AbPrevious)
            {
              pMarkerAb->AbPrevious->AbNext = pMarkerAb;
              if (pMarkerAb->AbPrevious->AbElement != pEl)
                pEl->ElAbstractBox[viewNb - 1] = pMarkerAb;
            }
          else
            pEl->ElAbstractBox[viewNb -1] = pMarkerAb;
        }
    }

  if (!ok)
    /* the marker box can't be inserted. Release it */
    FreeAbstractBox (pMarkerAb);
  else
    /* set all properties of the marker box */
    {
      pMarkerAb->AbPSchema = NULL; /* this box is not defined in any P schema*/
      pMarkerAb->AbCreatorAttr = pAttr; /* box not created by an attribute */
      pMarkerAb->AbListStyleType = pAb->AbListStyleType;
      pMarkerAb->AbListStyleImage = pAb->AbListStyleImage;
      pMarkerAb->AbListStylePosition = pAb->AbListStylePosition;
      pMarkerAb->AbDisplay = pAb->AbDisplay;
      pMarkerAb->AbVis = pAb->AbVis;
      pMarkerAb->AbPresentationBox = TRUE;
      pMarkerAb->AbTypeNum = 0;
      pMarkerAb->AbVarNum = 0;
      pMarkerAb->AbCanBeModified = FALSE;
      
      pRule = SearchRuleListItemMarker (PtVisibility, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      /* the direction rule must be applied before position and margin rules,
         as the horizontal position and margins depend on the direction */
      pRule = SearchRuleListItemMarker (PtDirection, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtVertPos, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtHorizPos, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtMarginRight, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtMarginLeft, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtSize, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtStyle, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtWeight, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtVariant, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtFont, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtBackground, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtForeground, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);
      pRule = SearchRuleListItemMarker (PtOpacity, pEl, pDoc);
      if (pRule)
        ApplyRule (pRule, NULL, pMarkerAb, pDoc, NULL, pMarkerAb);

      done = False;
      if (pAb->AbListStyleImage == 'Y' && pAb->AbLeafType == LtCompound)
        /* display an image instead of the regular marker */
        {
          imageDesc = NULL;
          pAscAb = pAb;
          do
            {
              if (pAscAb->AbPictListStyle)
                {
                  imageDesc = (ThotPictInfo *) pAscAb->AbPictListStyle;
                  if (imageDesc->PicFileName == NULL)
                    imageDesc = NULL;
                  else if (!TtaFileExist (imageDesc->PicFileName))
                    imageDesc = NULL;
                }
              if (imageDesc == NULL)
                pAscAb = pAscAb->AbEnclosing;
            }
          while (imageDesc == NULL && pAscAb);
          if (imageDesc)
            {
              /* HorizRef: * . Bottom; */
              pMarkerAb->AbHorizRef.PosEdge = HorizRef;
              pMarkerAb->AbHorizRef.PosRefEdge = Bottom;
              pMarkerAb->AbHorizRef.PosAbRef = pMarkerAb;
              /* Height: Enclosed = */
              pMarkerAb->AbHeight.DimValue = -1;
              pMarkerAb->AbHeight.DimAbRef = NULL;
              pMarkerAb->AbHeight.DimUnit = UnRelative;
              pMarkerAb->AbHeight.DimSameDimension = TRUE;
              /* Width: Enclosed = */
              pMarkerAb->AbWidth.DimValue = -1;
              pMarkerAb->AbWidth.DimAbRef = NULL;
              pMarkerAb->AbWidth.DimUnit = UnRelative;
              pMarkerAb->AbWidth.DimSameDimension = TRUE;
              pMarkerAb->AbPictInfo = NULL;
              NewPictInfo (pMarkerAb, imageDesc->PicFileName,
                           imageDesc->PicType, False);
              pMarkerAb->AbLeafType = LtPicture;
              pMarkerAb->AbVolume = 100;
              done = True;
            }
        }

      if (!done)
	{
	  /* set content and size, according to the marker type */
	  if (pAb->AbListStyleType == 'D' ||   /* disc */
	      pAb->AbListStyleType == 'C' ||   /* circle */
	      pAb->AbListStyleType == 'S')   /* square */
	    /* content is a graphic shape */
	    {
	      /* HorizRef: * . Bottom; */
	      pMarkerAb->AbHorizRef.PosEdge = HorizRef;
	      pMarkerAb->AbHorizRef.PosRefEdge = Bottom;
	      pMarkerAb->AbHorizRef.PosAbRef = pMarkerAb;
	      /* Height: 0.4 em */
	      pMarkerAb->AbHeight.DimValue = 4;
	      pMarkerAb->AbHeight.DimAbRef = NULL;
	      pMarkerAb->AbHeight.DimUnit = UnRelative;
	      pMarkerAb->AbHeight.DimSameDimension = FALSE;
	      /* Width: 0.4 em */
	      pMarkerAb->AbWidth.DimValue = 4;
	      pMarkerAb->AbWidth.DimAbRef = NULL;
	      pMarkerAb->AbWidth.DimUnit = UnRelative;
	      pMarkerAb->AbWidth.DimSameDimension = FALSE;	  
	      /* LineWeight: 1 px */
	      pMarkerAb->AbLineWeight = 1;
	      pMarkerAb->AbLineWeightUnit = UnPixel;
	      /* FillPattern: foregroundcolor */
	      if (pAb->AbListStyleType == 'D' ||
		  pAb->AbListStyleType == 'S')
		/* disc or square */
		pMarkerAb->AbFillPattern = 1;
	      /* set content */
	      pMarkerAb->AbLeafType = LtGraphics;
	      if (pAb->AbListStyleType == 'S')
		/* square */
		pMarkerAb->AbShape = 'R';
	      else
		/* circle or disc */
		pMarkerAb->AbShape = 'c';
	      pMarkerAb->AbGraphScript = 'L';
	      pMarkerAb->AbVolume = 1;
	    }
	  else if (pAb->AbListStyleType == '1' ||  /* decimal */
		   pAb->AbListStyleType == 'Z' ||  /* decimal-leading-zero */
		   pAb->AbListStyleType == 'i' ||  /* lower-roman */
		   pAb->AbListStyleType == 'I' ||  /* Upper-Roman */
		   pAb->AbListStyleType == 'g' ||  /* lower greek */
		   pAb->AbListStyleType == 'a' ||  /* lower-latin */
		   pAb->AbListStyleType == 'A')    /* upper-latin */
	    /* it's a counter */
	    {
	      pMarkerAb->AbLeafType = LtText;
	      if (pMarkerAb->AbText == NULL)
		GetConstantBuffer (pMarkerAb);
	      pMarkerAb->AbLang = TtaGetDefaultLanguage ();
	      ComputeListItemNumber (pMarkerAb);
	    }
	}
    }
  return TRUE;
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
                               PtrPSchema pSchP, void* rulesPseudo, 
                               ThotBool completeCreator)
{
  PtrPRule            pRD, pRS;
  PtrPRule            pR, pR1, pRV;
  PtrAbstractBox      pAb, pAbb1, pAbbNext;
  PtrAbstractBox      pAbbCreated;
  PtrElement          pE, pER, pElSibling;
  PtrPresentationBox  pBox;
  FunctionType        funct;
  TypeUnit            unit;
  ThotPictInfo       *image;
  int                 view, vis, index;
  int                 viewSch, viewIndex;
  PtrPresVariable     pVar;
  PresVarItem        *pItem;
  PresConstant       *pConst;
  ThotBool            ok, stop, volok, done;

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
        ok = CondPresentation (pRCre->PrCond, pEl, pAttr, pEl, pRCre, viewSch,
			       pSS, pDoc);
    }
  /* on ne cree un pave de presentation que si le pave de l'element qui */
  /* provoque la creation existe dans la vue. */
  if (ok && pEl->ElAbstractBox[viewIndex] == NULL)
    ok = FALSE;
  /* on ne cree pas de pave fils pour un element holophraste' ou une feuille */
  funct = pRCre->PrPresFunction;
  if (ok && (pEl->ElHolophrast || pEl->ElTerminal))
    if (funct == FnCreateFirst || funct == FnCreateLast || funct == FnContent)
      ok = FALSE;

  if (ok && funct == FnContent && (pRCre->PrBoxType == BtBefore || pRCre->PrBoxType == BtAfter))
    /* it's a CSS content property. Check if we are allowed to insert some content here: it is
       for instance forbidden to create content between rows in a table or after a tbody, etc. */
    {
      pE = pEl;
      while (pE && ok)
	{
	  if (TypeHasException (ExcIsCell, pE->ElTypeNumber, pE->ElStructSchema) ||
	      TypeHasException (ExcIsCaption, pE->ElTypeNumber, pE->ElStructSchema))
	    break;
	  else if (TypeHasException (ExcIsTable, pE->ElTypeNumber, pE->ElStructSchema))
	    ok = FALSE;
	  else
	    pE = pE->ElParent;
	}
    }

  /* on ne cree pas de pave de presentation qui soit un frere ou le pere du */
  /* pave racine de la vue. */
  if (ok && pEl->ElAbstractBox[viewIndex]->AbEnclosing == NULL)
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
  index = pRCre->PrPresBox[0];
  if (ok)
    {
      if (index == 0)
        ok = FALSE;
      else if (pSchP->PsPresentBox == NULL)
        ok = FALSE;
      else if (funct != FnContent &&
               (index > pSchP->PsNPresentBoxes ||
                pSchP->PsPresentBox->PresBox[index - 1] == NULL))
        /* for function FnContent, index is the number of the
           presentation variable that will generate content, not the number
           of the presentation box to be created */
        ok = FALSE;
    }

  /* si c'est une boite de haut de page et qu'il s'agit de la derniere */
  /* marque de page du document, on ne cree pas la boite */
  if (ok && funct != FnContent)
    if (pSchP->PsPresentBox->PresBox[index - 1]->PbPageHeader)
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
      /* pRS : premiere regle de presentation specifique de la boite a creer */
      if (funct == FnContent)
        /* the first presentation box in a Pschema is the prototype of all
           CSS pseudo-elements generated by :before and :after */
        pRS = pSchP->PsPresentBox->PresBox[0]->PbFirstPRule;
      else
        pRS = pSchP->PsPresentBox->PresBox[index-1]->PbFirstPRule;
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
              /* if there is a visibility rule for that view, take it */
              /* no need to check a display rule: presentation boxes don't
                 use that kind of rule */
              pR = GetRuleView (&pRS, &pRD, PtVisibility, view, pEl, pAttr,
                                pEl->ElStructSchema, pDoc);
              if (pR == NULL)
                /* sinon, on prend celle de la vue 1 */
                pR = pR1;
            }
          if (view == viewSch)
            {
              /* c'est la regle de la vue traitee */
              vis = IntegerRule (pR, pEl, viewNb, &ok, &unit, NULL, NULL,
				 pSchP, pDoc);
              /* si la regle de visibilite n'a pas pu etre appliquee, */
              /* on prend la visibilite du pave de l'element createur */
              if (!ok)
                vis = pEl->ElAbstractBox[viewIndex]->AbVisibility;
            }
        }

      if (vis >= 1)
        /* le pave a creer est visible dans la vue */
        {
          pAb = InitAbsBoxes (pEl, viewNb, vis, pDoc->DocReadOnly);
          /* pAb: pave cree */
          pAbbCreated = pAb;
          pAb->AbPresentationBox = TRUE;
          /* c'est un pave de presentation */
          pAb->AbCreatorAttr = pAttr;
          /* on se souvient de l'attribut qui a cree' le pave' */
          if (funct == FnContent)
            /* the first presentation box in a Pschema is the prototype of all
               CSS pseudo-elements generated by :before and :after */
            pAb->AbTypeNum = 1;
          else
            pAb->AbTypeNum = index;
          pAb->AbVarNum = 0;
          pAb->AbCanBeModified = FALSE;
          pAb->AbPSchema = pSchP;
          pAbb1 = pEl->ElAbstractBox[viewIndex];
          /* pAbb1: 1er pave de l'element createur */

          /* chaine le pave cree */
          if (funct == FnCreateFirst ||
              (funct == FnContent && pRCre->PrBoxType == BtBefore))
            {
              pAb->AbSelected = FALSE;
              /* saute les paves de presentation deja crees */
              /* avec la regle CreateBefore */
              while (pAbb1->AbPresentationBox)
                pAbb1 = pAbb1->AbNext;
              pAb->AbEnclosing = pAbb1;
              UpdateCSSVisibility (pAb);
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
                    if ((pAbb1->AbPresentationBox && pAbb1->AbElement == pEl)||
                        (pAbb1->AbElement->ElTypeNumber == PageBreak + 1 &&
                         pAbb1->AbElement->ElPageType == PgBegin))
                      {
                        /* c'est un pave de presentation de l'element ou */
                        /* un saut de page de debut d'element */
                        if (pAbb1->AbPresentationBox &&
                            pAbb1->AbTypeNum == pAb->AbTypeNum &&
                            pAbb1->AbPSchema == pSchP &&
                            (pAttr == NULL || pAttr == pAbb1->AbCreatorAttr)
                            && !pAbb1->AbDead &&
                            /* all CSS pseudo-elements generated by :before
                               and :after have the same box number: 1. Check
                               that it is a :before pseudo-element */
                            (funct != FnContent ||
                             (funct == FnContent && pAb->AbPseudoElBefore)))
                          /* c'est le meme que celui qu'on veut creer */
                          {
                            pAbbCreated = NULL;
                            stop = TRUE;
                          }
                        else if (pAbb1->AbNext == NULL ||
                                 AttrIsAfter (pAbb1->AbCreatorAttr, pAttr))
                          /* le nouveau pave attache a un attribut doit
                             s'inserer avant ceux d'un attribut suivant */
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
                      if (pAbb1->AbElement == pEl ||
                          (pAbb1->AbElement->ElTypeNumber == PageBreak + 1 &&
                           pAbb1->AbElement->ElPageType == PgBegin))
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
                      if (funct == FnContent && pRCre->PrBoxType == BtBefore)
                        pAb->AbPseudoElBefore = TRUE;
                    }
                }
            }
          else if (funct == FnCreateLast ||
                   (funct == FnContent && pRCre->PrBoxType == BtAfter))
            {
              pAb->AbSelected = FALSE;
              /* saute les paves de presentation deja crees */
              /* avec la regle CreateBefore */
              while (pAbb1->AbPresentationBox)
                pAbb1 = pAbb1->AbNext;
              pAb->AbEnclosing = pAbb1;
              UpdateCSSVisibility (pAb);
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
                    if (pAbb1->AbPresentationBox == pAb->AbPresentationBox &&
                        pAbb1->AbTypeNum == pAb->AbTypeNum &&
                        pAbb1->AbPSchema == pSchP &&
                        (pAttr == NULL || pAttr == pAbb1->AbCreatorAttr) &&
                        !pAbb1->AbDead &&
                        /* all CSS pseudo-elements generated by :before
                           and :after have the same box number: 1. Check
                           that it is a :after pseudo-element */
                        (funct != FnContent ||
                         (funct == FnContent && pAb->AbPseudoElAfter)))
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
                      if (funct == FnContent && pRCre->PrBoxType == BtAfter)
                        pAb->AbPseudoElAfter = TRUE;
                    }
                }
            }
          else if (funct ==  FnCreateBefore)
            {
              /* saute les paves de presentation deja crees */
              /* avec la regle CreateBefore */
              stop = FALSE;
              do
                if (pAbb1->AbTypeNum == pAb->AbTypeNum &&
                    pAbb1->AbPresentationBox == pAb->AbPresentationBox &&
                    pAbb1->AbPSchema == pSchP &&
                    (pAttr == NULL || pAttr == pAbb1->AbCreatorAttr) &&
                    !pAbb1->AbDead)
                  /* ce pave de presentation existe deja */
                  {
                    pAbbCreated = NULL;
                    stop = TRUE;
                  }
                else if ((pAbb1->AbNext == NULL) ||
                         (!pAbb1->AbPresentationBox && !pAbb1->AbDead) ||
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
                  UpdateCSSVisibility (pAb);
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
            }
          else if (funct == FnCreateAfter || funct == FnCreateWith)
            {
              stop = FALSE;
              do
                if (pAbb1->AbNext == NULL)
                  stop = TRUE;
                else if (pAbb1->AbNext->AbElement != pEl)
                  stop = TRUE;
                else if (pAbb1->AbNext->AbPresentationBox &&
                         pAbb1->AbNext->AbTypeNum == pAb->AbTypeNum &&
                         pAbb1->AbNext->AbPresentationBox == pAb->AbPresentationBox &&
                         pAbb1->AbNext->AbPSchema == pSchP &&
                         (pAttr == NULL || pAttr == pAbb1->AbCreatorAttr) &&
                         !pAbb1->AbNext->AbDead)
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
                  UpdateCSSVisibility (pAb);
                  pAb->AbNext = pAbb1->AbNext;
                  pAb->AbPrevious = pAbb1;
                  pAbb1->AbNext = pAb;
                  if (pAb->AbNext != NULL)
                    pAb->AbNext->AbPrevious = pAb;
                }
            }
          else if (funct == FnCreateEnclosing)
            {
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
                  UpdateCSSVisibility (pAb);
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
                      UpdateCSSVisibility (pAbb1);
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
            }

          if (pAbbCreated == NULL)	/* pave deja cree' */
            {
              if (pAb->AbBox &&
                  pAb->AbLeafType == LtPicture && pAb->AbPresentationBox)
                {
                  image = (ThotPictInfo *)pAb->AbPictInfo;
                  CleanPictInfo (image);
                  TtaFreeMemory (pAb->AbPictInfo);
                  pAb->AbPictInfo = NULL;
                }

              if (pAb->AbLeafType == LtCompound && pAb->AbPictBackground)
                {
                  /* in this particular case we need to free filename */
                  image = (ThotPictInfo *)pAb->AbPictBackground;
                  TtaFreeMemory (image->PicFileName);
                  CleanPictInfo (image);
                  TtaFreeMemory (pAb->AbPictBackground);
                  pAb->AbPictBackground = NULL;
                }

              if (pAb->AbLeafType == LtCompound && pAb->AbPictListStyle)
                {
                  image = (ThotPictInfo *)pAb->AbPictBackground;
                  CleanPictInfo (image);
                  TtaFreeMemory (pAb->AbPictListStyle);
                  pAb->AbPictListStyle = NULL;
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
              if (pER)
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
                            if (rulesPseudo)
                              {
                                if (((AllRules*)rulesPseudo)->selectedRule[pRV->PrType])
                                  pRV = ((AllRules*)rulesPseudo)->selectedRule[pRV->PrType];
                              }
                            if (pRV->PrType == PtFunction &&
                                (pRV->PrPresFunction == FnCreateFirst ||
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
                                pAbb1 = CrAbsBoxesPres (pEl, pDoc, pRV, pSS,
                                                        NULL, viewNb, pSchP, NULL, TRUE);
                                /* restaure le pointeur de pave de l'elem */
                                pEl->ElAbstractBox[viewIndex] = pAbbNext;
                              }
                            else if (!completeCreator &&
                                     pRV->PrPresMode == PresInherit &&
                                     pRV->PrInheritMode == InheritCreator)
                              /* toutes les regles de presentation n'ont */
                              /* pas encore ete appliquees au pave */
                              /* et le pave cree herite du createur, on */
                              /* differe l'application de la regle */
                              Delay (pRV, pSchP, pAbbCreated, NULL);
                            else if (!ApplyRule (pRV, pSchP, pAbbCreated, pDoc,
                                                 NULL, pAbbCreated))
                              /* on n'a pas pu appliquer la regle, on */
                              /* l'appliquera lorsque le pave pere */
                              /* sera  termine' */
                              Delay (pRV, pSchP, pAbbCreated, pAttr);
                          }
                      }
                }
              while (pR);

              pAbbCreated->AbPresentationBox = TRUE;
              /* met le contenu dans le pave cree */
              if (funct == FnContent)
                /* for a rule that generates a CSS pseudo-element,
                   pRCre->PrPresBox refers to the variable to be generated */
                {
                  done = FALSE;
                  pVar = pSchP->PsVariable->PresVar[index - 1];
                  if (pVar->PvNItems == 1)
                    /* there is 1 and only 1 item in this variable */
                    {
                      pItem = &pVar->PvItem[0];
                      if (pItem->ViType == VarText)
                        {
                          /* this item contains a character string */
                          pConst = &pSchP->PsConstant[pItem->ViConstant - 1];
                          if (pConst->PdType == tt_Picture)
                            /* the char. string is a picture name */
                            {
                              pAb->AbLeafType = LtPicture;
                              NewPictInfo (pAb, pConst->PdString,
                                           UNKNOWN_FORMAT, False);
                              pAb->AbVolume = 100;
                              done = TRUE;
                            }
                        }
                    }
                  if (!done)
                    NewVariable (index, pSS, pSchP, pAb, pAttr, pDoc);
                }
              else
                {
                  pBox = pSchP->PsPresentBox->PresBox[index - 1];
                  switch (pBox->PbContent)
                    {
                    case FreeContent:
                      break;
                    case ContVariable:
                      NewVariable (pBox->PbContVariable, pSS, pSchP, pAb,
                                   pAttr, pDoc);
                      break;
                    case ContConst:
                      ConstantCopy (pBox->PbContConstant, pSchP, pAb);
                      break;
                    }
                }
              while (pR);

              /* apply delayed rules */
              ApplyDelayedRules (-1, pAbbCreated, pDoc);

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
                  ((ThotPictInfo *) (pAbbCreated->AbPictInfo))->PicPresent =
                    ReScale;
                  if (!pAbbCreated->AbWidth.DimIsPosition &&
                      pAbbCreated->AbWidth.DimValue < 0 &&
                      pAbbCreated->AbWidth.DimAbRef == NULL &&
                      pAbbCreated->AbHeight.DimIsPosition &&
                      pAbbCreated->AbHeight.DimValue < 0 &&
                      pAbbCreated->AbHeight.DimAbRef == NULL)
                    /* le pave prend la hauteur et la largeur de son contenu */
                    /* l'image doit etre affichee telle quelle */
                    ((ThotPictInfo *) (pAbbCreated->AbPictInfo))->PicPresent = RealSize;
                }
            }
        }
    }
  return (pAbbCreated);
}

/*----------------------------------------------------------------------
  SameChar
  compare characters c1 and c2 and return TRUE if they are the same.
  If attrNum is 1 (lang attribute) comparison is case insensitive.
  ----------------------------------------------------------------------*/
static ThotBool SameChar (CHAR_T c1, CHAR_T c2, int attrNum)
{

  if (attrNum != 1)
    return (c1 == c2);
  else
    {
      if (c1 > 127 || c2 > 127)
        return (c1 == c2);
      else
        {
          if (c1 == c2)
            return TRUE;
          else
            if (c1 - c2 == 32 || c2 - c1 == 32)
              return TRUE;
            else
              return FALSE;
        }
    }
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
  match = 1 indicates that we have not yet done any string value match yet
    and it is changed to 0 to indicate that a different string match must
    be done for the next call.
  When returning, attrBlock contains a pointer to the block of presentation
  rules to which the returned rule belongs.
  ----------------------------------------------------------------------*/
PtrPRule AttrPresRule (PtrAttribute pAttr, PtrElement pEl,
                       ThotBool inheritRule, PtrAttribute pAttrComp,
                       PtrPSchema pSchP, int *valueNum, int *match,
                       PtrAttributePres *attrBlock)
{
  PtrPRule            pRule;
  PtrAttribute        pAt2;
  PtrElement          pElAttr;
  PtrSSchema	        pSSattr;
  AttributePres      *pAPRule, *pPRdef, *pPRinherit;
  NumAttrCase        *pCase;
  char                buffer[400];
  char               *attrValue, *ptr, *wordEnd;
  unsigned int        len;
  int                 i, j, k, attrNum;
  CHAR_T             *refVal;
  ThotBool            found, ok, firstVal;

  pRule = NULL;
  *attrBlock = NULL;

  if (pSchP == NULL)
    {
      *valueNum = 0;
      return (NULL);
    }

  if (!pAttr || !pAttr->AeAttrSSchema ||
      strcmp (pAttr->AeAttrSSchema->SsName, pSchP->PsStructName))
    {
      *valueNum = 0;
      return (NULL);
    }

  firstVal = (*valueNum == 1);
  pPRdef = pPRinherit = NULL;
  len = 0;
  attrValue = NULL;
  attrNum = pAttr->AeAttrNum;
  pSSattr = pAttr->AeAttrSSchema;
  pAPRule = pSchP->PsAttrPRule->AttrPres[attrNum - 1];
  /* on cherche quel est le paquet de regles qui s'applique */
  /* pPRdef designera le paquet de regles s'appliquant a tous les elements */
  /* c'est a dire celui pour lequel pAPRule->ApElemType = 0  */
  /* pPRinherit  designera le paquet pour lequel
     pAPRule->ApElemType = pEl->ElTypeNumber */

  if (pAttr->AeAttrType != AtTextAttr)
    *valueNum = 0;
  else
    {
      if (pAttr->AeAttrText)
        {
          CopyBuffer2MBs (pAttr->AeAttrText, 0, (unsigned char *)buffer, 399);
          attrValue = buffer;
        }

      if (pSSattr->SsAttribute &&
          !pSSattr->SsIsXml && /* not a generic xml schema */
          pSSattr->SsAttribute->TtAttr[attrNum - 1] &&
          !strcmp (pSSattr->SsAttribute->TtAttr[attrNum - 1]->AttrName, "xml:id"))
        {
          if (pAPRule == NULL)
            {
              /* look for rules applied to the default id attribute */
              i = GetAttrWithException(ExcCssId, pSSattr);
              if (i > 0)
                {
                  /* the id attribute with CssId exception is found */
                  attrNum = i;
                  pAPRule = pSchP->PsAttrPRule->AttrPres[attrNum - 1];
                }
            }
        }

      if (!AttrHasException (ExcCssClass, attrNum, pSSattr))
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
              wordEnd = ptr;
              /* skip the spaces following this value */
              while (*ptr <= ' ' && *ptr != EOS)
                ptr++;
              if (*ptr == EOS)
                /* that's the last value */
                *valueNum = 0;
              else
                /* there is another value after that one */
                (*valueNum)++;
              *wordEnd = EOS;
            }
        }
    }
    
  for (i = pSchP->PsNAttrPRule->Num[attrNum - 1]; i-- > 0;
       pAPRule = pAPRule->ApNextAttrPres)
    {
      if (pAPRule->ApElemType == 0 || pAPRule->ApElemType == AnyType +1 ||
          pAPRule->ApElemType == pEl->ElTypeNumber)
        {
          if (pAttr->AeAttrType == AtTextAttr && pAPRule->ApString)
            {
              if (attrValue)
                {
                  refVal = TtaConvertMbsToCHAR ((unsigned char *)pAPRule->ApString);
                  /* test the attribute value */
                  if (attrValue[0] == EOS && refVal[0] == EOS)
                    /* both strings are empty */
                    ok = TRUE;
                  else
                    {
                      j = 0;
                      ok = FALSE;
                      if (pAPRule->ApMatch == CoSubstring)
                        /* compare strings up to the first hyphen in the
                           attribute value */
                        {
                          while (attrValue[j] != EOS &&
                                 SameChar (attrValue[j], refVal[j], attrNum) &&
                                 refVal[j] != EOS)
                            j++;
                          ok = (refVal[j] == EOS &&
                                (attrValue[j] == '-' || attrValue[j] == EOS));
                        }
                      else
                        while (!ok && attrValue[j] != EOS)
                          {
                            k = 0;
                            while (refVal[k] != EOS &&
                                   SameChar (attrValue[j + k], refVal[k],
                                             attrNum))
                              k++;
                            ok = (refVal[k] == EOS);
                            if (ok)
                              {
                                /* the substring was found */
                                if (pAPRule->ApMatch == CoWord)
                                  {
                                    /* check if a word matches */
                                    k += j;
                                    ok = (j == 0 ||
                                          attrValue[j - 1] == SPACE) &&
                                      (attrValue[k] == EOS ||
                                       attrValue[k] == SPACE);
                                    if (ok && firstVal && *valueNum == 0 && *match)
                                      /* we are testing the first value and
                                         there is no other value and that's
                                         the first match we try. Stop
                                         here. Next time we will test exact
                                         match */
                                      {
                                        i = 0; /* stop */
                                        *match = 0;
                                        *valueNum = 1;
                                      }
                                  }
                                else if (pAPRule->ApMatch == CoMatch)
                                  /* the whole attribute value must be equal */
                                  ok = (attrValue[j + k] == EOS &&
                                        j == 0 && k != 0);
                              }
                            /* prepare next search */
                            j++;
                          }
                    }
                  TtaFreeMemory (refVal);
                  if (ok)
                    {
                      if (pAPRule->ApElemType == 0)
                        pPRdef = pAPRule;
                      else
                        pPRinherit = pAPRule;
                    }
                }
            }
          else if (pAPRule->ApElemType == 0)
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
    {
      *attrBlock = pAPRule;
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
                                                     pSSattr, &pElAttr);
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
                                                     pSSattr, &pElAttr);
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
                                                     pSSattr, &pElAttr);
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
              pSSattr->SsAttribute->TtAttr[attrNum - 1]->AttrNEnumValues < pAttr->AeAttrValue)
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
    }
  return pRule;
}

/*----------------------------------------------------------------------
  ApplCrPresRule determine et applique les regles de creation a appliquer    
  au pave pAb en fonction de head.
  S'il y a des regles de creation de pseudo-elements CSS, met
  pseudoElem a TRUE.
  ----------------------------------------------------------------------*/
static void ApplCrPresRule (PtrSSchema pSS, PtrPSchema pSP,
                            PtrAbstractBox * pAbbCreated,
                            PtrAttribute pAttr, PtrDocument pDoc,
                            PtrAbstractBox pAb, ThotBool head, PtrPRule pRule,
                            ThotBool *pseudoElem)
{
  PtrAbstractBox      pAbb, pAbbR;
  ThotBool            stop;

  /* saute les regles precedant les fonctions */
  stop = FALSE;
  do
    if (pRule == NULL)
      stop = TRUE;
    else if (pRule->PrType > PtFunction)
      /* pas de fonction de presentation */
      {
        stop = TRUE;
        pRule = NULL;
      }
    else if (pRule->PrType == PtFunction)
      /* first presentation function */
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
        /*  */
        if (pRule->PrPresFunction == FnContent)
          /* it's a rule creating a CSS pseudo-element */
          {
            if ((head && pRule->PrBoxType == BtBefore) ||
                (!head && pRule->PrBoxType == BtAfter))
              *pseudoElem = TRUE;
          }
        else if ((head &&
                  (pRule->PrPresFunction == FnCreateBefore ||
                   pRule->PrPresFunction == FnCreateFirst)) ||
                 (!head &&
                  (pRule->PrPresFunction == FnCreateAfter ||
                   pRule->PrPresFunction == FnCreateLast)))
          /* it's a creation function for the end of interest. Apply it. */
          {
            pAbb = CrAbsBoxesPres (pAb->AbElement, pDoc, pRule, pSS, pAttr,
                                   pAb->AbDocView, pSP, NULL, TRUE);
            if (pAbb != NULL)
              {
                if (!head)
                  *pAbbCreated = pAbb;
                else if (*pAbbCreated == NULL)
                  *pAbbCreated = pAbb;
                /* modifie les paves environnant */
                /* qui dependent du pave cree */
                ApplyRefAbsBoxNew (*pAbbCreated, *pAbbCreated, &pAbbR, pDoc);
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
      ApplyRule (pRule, pSchP, pAb, pDoc, pAttr, pAb);
      if (pAb->AbTopMargin)
        {
          ApplyInherit (PtMarginTop, pAb, pDoc, FALSE);
          result = TRUE;
        }
      pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtBorderTopWidth, FnAny, TRUE,
                             &pAttr);
     ApplyRule (pRule, pSchP, pAb, pDoc, pAttr, pAb);
      if (pAb->AbTopBorder)
        {
          ApplyInherit (PtBorderTopWidth, pAb, pDoc, FALSE);
          result = TRUE;
        }
      pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtPaddingTop, FnAny, TRUE,
                             &pAttr);
      ApplyRule (pRule, pSchP, pAb, pDoc, pAttr, pAb);
      if (pAb->AbTopPadding)
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
      ApplyRule (pRule, pSchP, pAb, pDoc, pAttr, pAb);
      if (pAb->AbBottomMargin)
        {
          ApplyInherit (PtMarginBottom, pAb, pDoc, FALSE);
          result = TRUE;
        }
      pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtBorderBottomWidth, FnAny,
                             TRUE, &pAttr);
      ApplyRule (pRule, pSchP, pAb, pDoc, pAttr, pAb);
      if (pAb->AbBottomBorder)
        {
          ApplyInherit (PtBorderBottomWidth, pAb, pDoc, FALSE);
          result = TRUE;
        }
      pRule = SearchRulepAb (pDoc, pAb, &pSchP, PtPaddingBottom, FnAny, TRUE,
                             &pAttr);
      ApplyRule (pRule, pSchP, pAb, pDoc, pAttr, pAb);
      if (pAb->AbBottomPadding)
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
  l'extremite qui devient complete. Retourne un pointeur
  sur le dernier pave de presentation cree ou NULL si
  aucun pave n'est cree.
  ----------------------------------------------------------------------*/
PtrAbstractBox TruncateOrCompleteAbsBox (PtrAbstractBox pAb, ThotBool truncate,
                                         ThotBool head, PtrDocument pDoc)
{
  PtrPRule            pRule;
  PtrPSchema          pSchP, pSchPattr, pSchPSav;
  int                 index;
  PtrSSchema          pSchS, pSSattr;
  PtrAbstractBox      pAbbCreated, pAbbReturn;
  PtrAttribute        pAttr;
  PtrElement          pEl, pElAttr, pFirstAncest;
  int                 l, valNum, match, lqueue;
  InheritAttrTable   *inheritTable;
  PtrHandlePSchema    pHd;
  PtrAttributePres    attrBlock;
  PtrPRule            pRSpec, pRDef;
  RuleQueue           rQueue;
  ThotBool            pseudoElem;

  pAbbCreated = NULL;
  if (pAb && pAb->AbLeafType == LtCompound)
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
                  if (pAb->AbTopMargin)
                    {
                      pAb->AbTopMargin = 0;
                      pAb->AbMBPChange = TRUE;
                    }
                  if (pAb->AbTopBorder)
                    {
                      pAb->AbTopBorder = 0;
                      pAb->AbMBPChange = TRUE;
                    }
                  if (pAb->AbTopPadding)
                    {
                      pAb->AbTopPadding = 0;
                      pAb->AbMBPChange = TRUE;
                    }
                }
              else
                {
                  if (pAb->AbBottomMargin)
                    {
                      pAb->AbBottomMargin = 0;
                      pAb->AbMBPChange = TRUE;
                    }
                  if (pAb->AbBottomBorder)
                    {
                      pAb->AbBottomBorder = 0;
                      pAb->AbMBPChange = TRUE;
                    }
                  if (pAb->AbBottomPadding)
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
              pseudoElem = FALSE;
              pEl = pAb->AbElement;
              SearchPresSchema (pEl, &pSchP, &index, &pSchS, pDoc);
              if (pSchS != NULL && pSchS != pEl->ElStructSchema)
                /* il s'agit de l'element racine d'une nature qui
                   utilise le schema de presentation de son englobant*/
                if (pDoc->DocView[pAb->AbDocView - 1].DvSSchema !=
                    pDoc->DocSSchema)
                  {
                    pSchS = pEl->ElStructSchema;
                    pSchP = PresentationSchema (pSchS, pDoc);
                    index = pEl->ElTypeNumber;
                  }
              pSchPSav = pSchP;
              /* handle creation rules associated with the element type */
              pRule = pSchP->PsElemPRule->ElemPres[index - 1];
              ApplCrPresRule (pSchS, pSchP, &pAbbCreated, NULL, pDoc, pAb,
                              head, pRule, &pseudoElem);
              /* handle creation rules associated with any element type in
                 all schema extensions */
              /* We need to do that only if the element is not a basic or
                 hidden element */
              if (CanApplyCSSToElement (pEl))
                {
                  pHd = FirstPSchemaExtension (pSchS, pDoc, pEl);
                  while (pHd)
                    {
                      pSchP = pHd->HdPSchema;
                      pRule = pSchP->PsElemPRule->ElemPres[AnyType];
                      ApplCrPresRule (pSchS, pSchP, &pAbbCreated, NULL, pDoc,
                                      pAb, head, pRule, &pseudoElem);
                      pHd = pHd->HdNextPSchema;
                    }
                }

              pAttr = NULL;
              pSchP = PresentationSchema (pEl->ElStructSchema, pDoc);
              pHd = NULL;
              while (pSchP)
                {
                  /* first, get rules associated with the element type only
                     if it's not the main presentation schema */
                  if (pHd)
                    {
                      pRule = pSchP->PsElemPRule->ElemPres[pEl->ElTypeNumber - 1];
                      ApplCrPresRule (pSchS, pSchP, &pAbbCreated, NULL, pDoc,
                                      pAb, head, pRule, &pseudoElem);
                    }

                  /* handle the rules associated with attributes of ancestors
                     that apply to the element */
                  if (pSchP->PsNInheritedAttrs->Num[pEl->ElTypeNumber -1])
                    /* the element type inherits some attribute */
                    {
                      inheritTable = pSchP->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1];
                      if (!inheritTable)
                        {
                          /* inheritance table does not exist. Create it */
                          CreateInheritedAttrTable (pEl->ElTypeNumber,
                                                    pEl->ElStructSchema, pSchP, pDoc);
                          inheritTable = pSchP->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1];
                        }
                      for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
                        if ((*inheritTable)[l - 1])
                          /* pEl inherits attribute l */
                          {
                            /* is this attribute present on an ancestor? */
                            if ((*inheritTable)[l - 1] == 'S')
                              pFirstAncest = pEl;
                            else
                              pFirstAncest = pEl->ElParent;
                            if ((pAttr = GetTypedAttrAncestor (pFirstAncest, l,
                                                               pEl->ElStructSchema, &pElAttr)) != NULL)
                              {
                                /* process all values of the attribute, in case of
                                   a text attribute with multiple values */
                                valNum = 1; match = 1;
                                do
                                  {
                                    pRule = AttrPresRule (pAttr, pEl, TRUE, NULL,
                                                          pSchP, &valNum, &match, &attrBlock);
                                    if (pRule && !pRule->PrDuplicate)
                                      ApplCrPresRule (pAttr->AeAttrSSchema, pSchP,
                                                      &pAbbCreated, pAttr, pDoc,
                                                      pAb, head, pRule, &pseudoElem);
                                  }
                                while (valNum > 0);
                              }
                          }
                    }

                  if (pEl->ElTypeNumber > MAX_BASIC_TYPE &&
                      pSchP->PsNInheritedAttrs->Num[AnyType])
                    /* some attributes are inherited by all element types */
                    {
                      inheritTable = pSchP->PsInheritedAttr->ElInherit[AnyType];
                      if (!inheritTable)
                        {
                          /* cette table n'existe pas on la genere */
                          CreateInheritedAttrTable (AnyType+1,
                                                    pEl->ElStructSchema, pSchP, pDoc);
                          inheritTable = pSchP->PsInheritedAttr->ElInherit[AnyType];
                        }
                      for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
                        if ((*inheritTable)[l - 1])
                          /* pEl inherits attribute l */
                          {
                            /* is this attribute present on an ancestor? */
                            if ((*inheritTable)[l - 1] == 'S')
                              pFirstAncest = pEl;
                            else
                              pFirstAncest = pEl->ElParent;
                            if ((pAttr = GetTypedAttrAncestor (pFirstAncest, l,
                                                               pEl->ElStructSchema, &pElAttr)) != NULL)
                              {
                                /* process all values of the attribute, in case of
                                   a text attribute with multiple values */
                                valNum = 1; match = 1;
                                do
                                  {
                                    pRule = AttrPresRule (pAttr, pEl, TRUE, NULL,
                                                          pSchP, &valNum, &match, &attrBlock);
                                    if (pRule && !pRule->PrDuplicate)
                                      ApplCrPresRule (pAttr->AeAttrSSchema, pSchP,
                                                      &pAbbCreated, pAttr, pDoc,
                                                      pAb, head, pRule, &pseudoElem);
                                  }
                                while (valNum > 0);
                              }
                          }
                    }

                  /* handle the creation rules associated with the attributes
                     of the element */
                  pAttr = pEl->ElFirstAttr;     /* first attribute of element */
                  /* check all attributes of element */
                  while (pAttr)
                    {
                      if (pHd == NULL)
                        /* main presentation schema. Take the one associated
                           with the attribute S schema */
                        pSchPattr = PresentationSchema (pAttr->AeAttrSSchema,
                                                        pDoc);
                      else
                        pSchPattr = pSchP;
                      pSSattr = pAttr->AeAttrSSchema;
                      if (pHd && /* this is a P schema extension */
                          /* if it's an ID, class or pseudo-class attribute,
                             take P schema extensions associated with the
                             document S schema */
                          (AttrHasException (ExcCssClass, pAttr->AeAttrNum,
                                             pAttr->AeAttrSSchema) ||
                           AttrHasException (ExcCssId, pAttr->AeAttrNum,
                                             pAttr->AeAttrSSchema) ||
                           AttrHasException (ExcCssPseudoClass, pAttr->AeAttrNum,
                                             pAttr->AeAttrSSchema)))
                        pSSattr = pDoc->DocSSchema;
                      /* process all values of the attribute, in case of a text
                         attribute with multiple values */
                      valNum = 1; match = 1;
                      do
                        {
                          /* first rule for this value of the attribute */
                          pRule = AttrPresRule (pAttr, pEl, FALSE, NULL,
                                                pSchPattr, &valNum, &match, &attrBlock);
                          ApplCrPresRule (pAttr->AeAttrSSchema, pSchP,
                                          &pAbbCreated, pAttr, pDoc, pAb, head,
                                          pRule, &pseudoElem);
                        }
                      while (valNum > 0);
                      /* get the next attribute of the element */
                      pAttr = pAttr->AeNext;
                    }

                  if (pHd)
                     /* next style sheet (P schema extension) */
                    pHd = pHd->HdNextPSchema;
                  else if (pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView == 1 &&
                           CanApplyCSSToElement (pEl))
                    /* it was the main P schema, get the first schema extension */
                    pHd = FirstPSchemaExtension (pEl->ElStructSchema, pDoc, pEl);

                  if (pHd == NULL)
                    /* no schema any more. stop */
                    pSchP = NULL;
                  else
                    pSchP = pHd->HdPSchema;
                }

              if (pseudoElem)
                /* A CSS pseudo-element has to be created for this end of the box */
                {
                  if (head)
                    pAb->AbTruncatedHead = truncate;
                  else
                    pAb->AbTruncatedTail = truncate;
                  pSchP = pSchPSav;
                  /* pRSpec: premiere regle de presentation associee au type
                     de l'element */
                  pRSpec = pSchP->PsElemPRule->ElemPres[index - 1];
                  /* premiere regle de presentation par defaut */
                  pRDef = pSchP->PsFirstDefaultPRule;
                  /* initialise la file des regles qui n'ont pas pu etre
                     appliquees*/
                  lqueue = 0;
                  pAbbReturn = NULL;
                  ApplyPresRules (pEl, pDoc, pAb->AbDocView,
                                  pDoc->DocView[pAb->AbDocView - 1].DvPSchemaView,
                                  pSchS, pSchP, &pRSpec, &pRDef, &pAbbReturn,
                                  !head, &lqueue, &rQueue, pAb, NULL, NULL, TRUE);
                  pAbbCreated = pAbbReturn;
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
static void WaitingRule (PtrPRule pR, PtrAbstractBox pAbb, PtrPSchema pSP,
                         PtrAttribute pA, AllRules* rulesPseudoEl,
                         RuleQueue* queue, int *lqueue)
{
  if (*lqueue >= MAX_QUEUE_LEN)
    printf ("Increase MAX_QUEUE_LEN\n");
  else
    {
      (*lqueue)++;
      queue->queuePR[*lqueue - 1] = pR;
      queue->queuePS[*lqueue - 1] = pSP;
      queue->queuePA[*lqueue - 1] = pA;
      queue->queuePP[*lqueue - 1] = pAbb;
      queue->rulesPseudo[*lqueue - 1] = rulesPseudoEl;
    }
}

/*----------------------------------------------------------------------
  GetAtt recupere une regle de presentation qui etait en attente. 
  ----------------------------------------------------------------------*/
static void GetAtt (PtrPRule *pR, PtrAbstractBox *pAbb, PtrPSchema *pSP,
                    PtrAttribute *pA, AllRules** rPseudo, RuleQueue* queue,
                    int *lqueue, int *pqueue)
{
  *pR = NULL;
  if (queue)
    while (*pR == NULL && *pqueue < *lqueue)
      {
        (*pqueue)++;
        *pR = queue->queuePR[*pqueue - 1];
        *pSP = queue->queuePS[*pqueue - 1];
        *pA = queue->queuePA[*pqueue - 1];
        *pAbb = queue->queuePP[*pqueue - 1];
        *rPseudo = queue->rulesPseudo[*pqueue - 1];
      }
}

/*----------------------------------------------------------------------
  ApplCrRule checks if the rule pRuleCr of the the presentation schema
  pSchPres (associated to the structure schema pSS), is a creation rule.
  If TRUE tries to apply it if fileDescriptor is NULL or displays the rule
  and returns TRUE else returns FALSE.
  The parameter pA gives the attribute which generates the rule or NULL.
  ----------------------------------------------------------------------*/
static ThotBool ApplCrRule (PtrPRule pRuleCr, PtrSSchema pSS,
                            PtrPSchema pSchPres, PtrAttribute pA,
                            PtrAbstractBox *pAbbReturn, DocViewNumber viewNb,
                            PtrDocument pDoc, PtrElement pEl,
                            ThotBool forward, int *lqueue, RuleQueue* queue,
                            PtrAbstractBox pNewAbbox, FILE *fileDescriptor)
{
  ThotBool            result, toCreate;
  PtrAbstractBox      pAbb;

  result = FALSE;
  if (pRuleCr->PrType == PtFunction &&
      (pRuleCr->PrPresFunction == FnCreateFirst ||
       pRuleCr->PrPresFunction == FnCreateLast ||
       pRuleCr->PrPresFunction == FnCreateBefore ||
       pRuleCr->PrPresFunction == FnCreateWith ||
       pRuleCr->PrPresFunction == FnCreateAfter ||
       pRuleCr->PrPresFunction == FnCreateEnclosing))
    /* it's a creation rule */
    {
      result = TRUE;
      toCreate = FALSE;	/* by default no creation */
      if (fileDescriptor)
        DisplayPRule (pRuleCr, fileDescriptor, pEl, pSchPres, 0);
      else if (pNewAbbox)
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
              WaitingRule (pRuleCr, pNewAbbox, pSchPres, pA, NULL, queue,
                           lqueue);
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
              WaitingRule (pRuleCr, pNewAbbox, pSchPres, pA, NULL, queue,
                           lqueue);
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
                  WaitingRule (pRuleCr, pNewAbbox, pSchPres, pA, NULL, queue,
                               lqueue);
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
                                 pSchPres, NULL, FALSE);
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
PtrPRule GetNextAttrPresRule (PtrPRule *pR, PtrSSchema pSS,
                              PtrAttribute pAttr, PtrElement pElAttr,
                              PtrDocument pDoc, PtrElement pEl,
                              int view)
{
  PtrPRule            pRuleView1, pRule;

  /* on cherche une regle qui concerne la vue ou defaut la regle pour la vue 1*/
  pRuleView1 = NULL;
  pRule = NULL;
  if ((*pR)->PrViewNum == 1)
    {
      /* la regle pour la vue 1 */
      if ((*pR)->PrCond == NULL ||
          CondPresentation ((*pR)->PrCond, pEl, pAttr, pElAttr, *pR, 1, pSS,
			    pDoc))
        /* la condition d'application est satisfaite */
        {
          /* On la conserve au cas ou on ne trouve pas mieux */
          pRuleView1 = *pR;
          if (view == 1)
            /* on est dans la vue 1. Donc c'est la bone regle */
            pRule = *pR;
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
                                          *pR, view, pSS, pDoc))
                      pRule = *pR;
                }
              if (pRule == NULL)
                /* il n'y a pas de regle specifique pour la vue view */
                /* On prend la vue 1 */
                pRule = pRuleView1;
            }
        }
    }
  else
    /* ce n'est pas une regle pour la vue 1. Cette regle */
    /* ne s'applique que si le numero de vue correspond */
    if (view == (*pR)->PrViewNum)
      if ((*pR)->PrCond == NULL ||
          CondPresentation ((*pR)->PrCond, pEl, pAttr, pElAttr, *pR, view,
			    pSS, pDoc))
        pRule = *pR;

  return pRule;
}

/*----------------------------------------------------------------------
  CascadeVisibility
  If rule pRule is a candidate for the cascade, register it.
  ----------------------------------------------------------------------*/
static void CascadeVisibility (PtrPRule pRule, PtrPSchema pSchP,
                                PtrAttribute pAttr, PtrAttributePres pAttrBlk)
{
  if (pRule)
    {
      /* register the rule in the appropriate rule table */
      if (pRule->PrType == PtVisibility)
	{
	  if (RuleHasHigherPriority (pRule, pSchP, pAttrBlk,
				     PriorRuleV,
				     PriorschemaOfRuleV, PriorattrBlockOfRuleV))
	    {
	      // keep the right rule
	      PriorRuleV = pRule;
	      PriorschemaOfRuleV = pSchP;      
	      PriorattrOfRuleV = pAttr;
	      PriorattrBlockOfRuleV = pAttrBlk;
	    }
	}
      else
	{
	  if (RuleHasHigherPriority (pRule, pSchP, pAttrBlk,
				     PriorRuleD,
				     PriorschemaOfRuleD, PriorattrBlockOfRuleD))
	    {
	      // keep the right rule
	      PriorRuleD = pRule;
	      PriorschemaOfRuleD = pSchP;      
	      PriorattrOfRuleD = pAttr;
	      PriorattrBlockOfRuleD = pAttrBlk;
	    }
	}
    }
}

/*----------------------------------------------------------------------
  ApplyVisibRuleAttr modifie le parametre vis selon la regle de   
  visibilite de pAttr.
  Compare thre priority with previous visibility and display rules
  ----------------------------------------------------------------------*/
static ThotBool ApplyVisibRuleAttr (PtrElement pEl, PtrAttribute pAttr,
                                    PtrElement pElAttr, PtrDocument pDoc,
                                    int *vis, DocViewNumber viewNb,
                                    ThotBool *ok, ThotBool inheritRule)
{
  PtrPRule            pR, pRuleView1;
  PtrPSchema          pSchP;
  PtrHandlePSchema    pHd;
  PtrAttributePres    attrBlock;
  int                 view, valNum, match;
  ThotBool            stop, useView1, cssUndisplay = FALSE;

  /* on cherchera d'abord dans le schema de presentation principal de */
  /* l'attribut */
  pSchP = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
  pHd = NULL;
  *ok = FALSE;
  view = AppliedView (pEl, pAttr, pDoc, viewNb);
  /* on examine le schema de presentation principal, puis les schemas */
  /* additionnels */
  while (pSchP)
    {
      /* process all values of the attribute, in case of a text attribute
         with multiple values */
      valNum = 1; match = 1;
      do
        {
          /* cherche la premiere regle de presentation pour cette valeur */
          /* de l'attribut, dans ce schema de presentation */
          pR = AttrPresRule (pAttr, pEl, inheritRule, NULL, pSchP, &valNum,
                             &match, &attrBlock);
          while (pR != NULL)
            {
              if (pR->PrType == PtVisibility)
                /* cette valeur d'attribut a une regle de visibilite' */
                /* calcule le numero de la vue concernee par l'attribut */
                {
                  stop = FALSE;
                  useView1 = TRUE;
                  pRuleView1 = NULL;
                    /* si on n'a pas trouve de regle specifique pour la vue view */
                    /* On utilise la regle de visibilite de la vue 1 si elle
                       existe */
                  /* cherche s'il y a une regle de visibilite pour la vue */
                  while (!stop)
                    {
                      if (pR->PrViewNum == 1)
                        if (pR->PrCond == NULL ||
                            CondPresentation (pR->PrCond, pEl, pAttr, pElAttr,
                                              pR, 1, pAttr->AeAttrSSchema, pDoc))
                          {
                            pRuleView1 = pR;
                            if (view == 1)
                              stop = TRUE;
                            else
                              /* saute les regles de visibilite suivantes de la vue 1 */
                              while (pR->PrNextPRule &&
                                     pR->PrNextPRule->PrType == PtVisibility &&
                                     pR->PrNextPRule->PrViewNum == 1)
                                pR = pR->PrNextPRule;
                          }
                      if (!stop)
                        {
                          if (pR->PrViewNum == view &&
                              CondPresentation (pR->PrCond, pEl, pAttr, pElAttr,
                                                pR, view, pAttr->AeAttrSSchema, pDoc))
                            {

                              /* regle trouvee, on l'evalue */
                              CascadeVisibility (pR, pSchP, pAttr, attrBlock);
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
		
                  if (useView1 && pRuleView1)
                    CascadeVisibility (pRuleView1, pSchP, pAttr, attrBlock);
                }
              else if (pR->PrType == PtDisplay && pR->PrBoxType == BtElement)
                /* this is a Display rule that applies to the element itslef,
                   not to a pseudo belement generated by :before or :after */
                {
                  if (pR->PrViewNum == view &&
                      CondPresentation (pR->PrCond, pEl, pAttr, pElAttr,
                                        pR, view, pAttr->AeAttrSSchema, pDoc))
                    CascadeVisibility (pR, pSchP, pAttr, attrBlock);
                }
              pR = pR->PrNextPRule;
            }
        }
      while (valNum > 0);

      if (pHd)
        /* passe au schema additionnel suivant */
        pHd = pHd->HdNextPSchema;
      else if (pDoc->DocView[viewNb - 1].DvPSchemaView == 1 &&     /* main view */
               CanApplyCSSToElement (pEl))
        /* on n'a pas encore traite' les schemas de presentation additionnels
           On prend le premier schema additionnel si on travaille pour la vue
           principale, sinon on ignore les schemas additionnels. */
        pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema, pDoc, pEl);

      if (pHd == NULL)
        /* il n'y a pas (ou plus) de schemas additionnels a prendre en compte*/
        pSchP = NULL;
      else
        pSchP = pHd->HdPSchema;
    }
  return cssUndisplay;
}

/*----------------------------------------------------------------------
  ComputeVisib computes the element visibility according to the
  ElAccess, the PtVisibility and PtDisplay rules, 
  The parameter vis returns the visibility value.            
  Other pRSpec, pRDef, ignoreDescent, complete, pType, and
  pSchPPage parameters are updated.
  The function returns TRUE when visibility is set to 0 by a css rule.
  ----------------------------------------------------------------------*/
static ThotBool ComputeVisib (PtrElement pEl, PtrDocument pDoc,
                              DocViewNumber viewNb, int viewSch,
                              PtrPRule *pRSpec, PtrPRule *pRDef, int *vis,
                              ThotBool *ignoreDescent, ThotBool *complete,
                              int *pType, PtrPSchema *pSchPPage)
{
  PtrPRule            pRule, pRuleV, pRuleDisplay;
  PtrElement          pPrevious, pNext, pElAssociatedPage, pAsc;
  PtrElement          pElAttr, pFirstAncest;
  PtrAttribute        pAttr;
  PtrPSchema          pSP;
  PtrHandlePSchema    pHd;
  InheritAttrTable   *inheritTable;
  TypeUnit            unit;
  int                 view, l;
  char                val;
  ThotBool            ok, stop, cssUndisplay;

 PriorRuleV = PriorRuleD = NULL;
 PriorschemaOfRuleV = PriorschemaOfRuleD = NULL;
 PriorattrOfRuleV = PriorattrOfRuleD = NULL;
 PriorattrBlockOfRuleV = PriorattrBlockOfRuleD = NULL;
  *vis = 0;
  cssUndisplay = FALSE;
  /* if an ancestor is hidden (Thot Access), the element is invisible */
  pAsc = pEl;
  while (pAsc)
    if (pAsc->ElAccess == Hidden)
      return cssUndisplay;
    else
      pAsc = pAsc->ElParent;

  pSP = PresentationSchema (pEl->ElStructSchema, pDoc);
  /* look for visibility rules in the main presentation schema */
  pRule = GetRule (pRSpec, pRDef, pEl, NULL, pEl->ElStructSchema, pDoc);
  /* first rule to be applied */
  /* the first rule is the visibility rule for view 1 (formatted view) */
  /* check all views defined in the presentation schema, to find the visibility
     rule for the view of interest */
  for (view = 1; view <= MAX_VIEW; view++)
    {
      /* get the visibility rule to be applied */
      if (view == 1)
        pRuleV = NULL;
      else
        pRuleV = GetRuleView (pRSpec, pRDef, PtVisibility, view, pEl, NULL,
                              pEl->ElStructSchema, pDoc);
      if (view == viewSch && DoesViewExist (pEl, pDoc, viewNb))
        {
          /* if there is a visibility rule for this view, we take it */
          if (pRuleV == NULL)
            pRuleV = pRule;
          if (pRuleV)
            // keep the right rule
            CascadeVisibility (pRuleV, pSP, NULL, NULL);

          /* is there a display rule with value none? */
          pRuleDisplay = GetRuleView (pRSpec, pRDef, PtDisplay, view, pEl,
                                      NULL, pEl->ElStructSchema, pDoc);
          if (pRuleDisplay && pRuleDisplay->PrBoxType == BtElement)
            /* this is a Display rule that applies to the element itslef,
               not to a pseudo element generated by :before or :after */
            CascadeVisibility (pRuleDisplay, pSP, NULL, NULL);
        }
    }

  pHd = NULL;
  while (pSP)
    {
      if (viewSch == 1 && pHd)
        /* we are interested in the main view and it's not the default style
           sheet */
        {
          /* look at the rules that apply to the element type in this
             P schema extension */
          pRule = pSP->PsElemPRule->ElemPres[pEl->ElTypeNumber - 1];
          while (pRule)
            {
              if ((pRule->PrType == PtVisibility || pRule->PrType == PtDisplay)
                  && pRule->PrBoxType == BtElement)
                {
                  if (pRule->PrCond == NULL ||
                      CondPresentation (pRule->PrCond, pEl, NULL, NULL, pRule,
                                        1, pEl->ElStructSchema, pDoc))
                    /* conditions are ok */
                    {
                      CascadeVisibility (pRule, pSP, NULL, NULL);
                    }
                }
              /* next rule for the element type in the same P schema extens. */
              pRule = pRule->PrNextPRule;
            }
        }

      /* do the inherited attributes change visibility ? */
      if (pSP->PsNInheritedAttrs->Num[pEl->ElTypeNumber - 1])
        {
          /* there is a possibility of inheritance */
          inheritTable = pSP->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1];
          if (inheritTable == NULL)
            {
              /* the inheritance table does not exist. Create it */
              CreateInheritedAttrTable (pEl->ElTypeNumber,
                                        pEl->ElStructSchema, pSP, pDoc);
              inheritTable = pSP->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1];
            }
          for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
            if ((*inheritTable)[l - 1])
              /* pEl inherits attribute l */
              {
                if ((*inheritTable)[l - 1] == 'S')
                  pFirstAncest = pEl;
                else
                  pFirstAncest = pEl->ElParent;	       
                if ((pAttr = GetTypedAttrAncestor (pFirstAncest, l,
                                                   pEl->ElStructSchema,
                                                   &pElAttr)) != NULL)
                  /* does attribute l exist on the ancestors? */
                  cssUndisplay = ApplyVisibRuleAttr (pEl, pAttr, pElAttr,
                                                     pDoc, vis, viewNb,
                                                     &ok, TRUE);
              }
        }
      if (pEl->ElTypeNumber > MAX_BASIC_TYPE &&
          pSP->PsNInheritedAttrs->Num[AnyType])
        {
          /* there is a possibility of inheritance */
          inheritTable = pSP->PsInheritedAttr->ElInherit[AnyType];
          if (inheritTable == NULL)
            {
              /* the inheritance table does not exist. Create it */
              CreateInheritedAttrTable (AnyType+1, pEl->ElStructSchema,
                                        pSP, pDoc);
              inheritTable = pSP->PsInheritedAttr->ElInherit[AnyType];
            }
          for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
            if ((*inheritTable)[l - 1])
              /* pEl inherits attribute l */
              {
                if ((*inheritTable)[l - 1] == 'S')
                  pFirstAncest = pEl;
                else
                  pFirstAncest = pEl->ElParent;	       
                if ((pAttr = GetTypedAttrAncestor (pFirstAncest, l,
                                                   pEl->ElStructSchema,
                                                   &pElAttr)) != NULL)
                  /* does attribute l exist on the ancestors? */
                  cssUndisplay = ApplyVisibRuleAttr (pEl, pAttr, pElAttr,
                                                     pDoc, vis, viewNb,
                                                     &ok, TRUE);
              }
        }

      /* next P schema */
      if (pHd)
        /* get the next extension schema */
        pHd = pHd->HdNextPSchema;
      else if (viewSch == 1 &&     /* main view */
               CanApplyCSSToElement (pEl))
        /* extension schemas have not been checked yet */
        /* get the first extension schema */
        pHd = FirstPSchemaExtension (pEl->ElStructSchema, pDoc, pEl);
      if (pHd == NULL)
        /* no more extension schemas. Stop */
        pSP = NULL;
      else
        pSP = pHd->HdPSchema;
    }

  /* do the attributes of the element change visibility? */
  pAttr = pEl->ElFirstAttr;
    /* check all attributes associated to the element */
  while (pAttr != NULL)
    {
      cssUndisplay = ApplyVisibRuleAttr (pEl, pAttr, pEl, pDoc, vis,
                                         viewNb, &ok, FALSE);
      pAttr = pAttr->AeNext;	/* next attribute for the element */
    }

  /* finally, look at specific presentation rules attached to the element */
  pRule = pEl->ElFirstPRule;
  while (pRule)
    /* apply a rule if it is related to the view */
    {
      if ((pRule->PrType == PtVisibility || pRule->PrType == PtDisplay)
          && pRule->PrViewNum == viewSch)
        {
          if (pRule->PrSpecifAttr == 0)
            /* this rule does not depend on an attribute */
            pAttr = NULL;
          else
            /* get the attribute the rule depends on */
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
          // keep the right rule
          CascadeVisibility (pRule, NULL, NULL, NULL);
        }
      pRule = pRule->PrNextPRule;
    }

  // apply the selected rule
  if (PriorRuleV)
    *vis = IntegerRule (PriorRuleV, pEl, viewNb, &ok, &unit, PriorattrOfRuleV,
			NULL, NULL, pDoc);
  if (PriorRuleD)
    {
      val = CharRule (PriorRuleD, pEl, viewNb, &ok);
      if (val == 'N' && ok)
        {
          /* rule "display: none" */
          *vis = 0;
          cssUndisplay = TRUE;
        }
    }

  /* if it's the root element and its visibility has not been set yet,
     set it to the default value */
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
          *pType = GetPageBoxType (pEl, pDoc, viewSch, pSchPPage);
          if (*pType == 0)
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
              *pType = GetPageBoxType (pElAssociatedPage, pDoc, viewSch,
                                       pSchPPage);
              if (*pType == 0)
                {
                  /* pas de page definie, on ne cree rien */
                  *ignoreDescent = TRUE;
                  *vis = 0;
                }
            }
        }
    }
  return cssUndisplay;
}

/*----------------------------------------------------------------------
  RuleHasHigherPriority
  return TRUE if presentation rule pRule1 (which belongs to presentation
  schema pPS1) has a higher priority in the CSS cascade than pRule2 (which
  belongs to presentation schema pPS2).
  If pPS1 or pPS2 is NULL, it means that the corresponding rule has no
  presentation schema, i.e. it is a specific rule attached to an element
  in a document.
  attrBlock1 and attrBlock2 point at the block of presentation rules to which
  the rules belong, if they are rules for attributes. NULL otherwise.
  ----------------------------------------------------------------------*/
ThotBool RuleHasHigherPriority (PtrPRule pRule1, PtrPSchema pPS1,
                                PtrAttributePres attrBlock1,
                                PtrPRule pRule2, PtrPSchema pPS2,
                                PtrAttributePres attrBlock2)
{
  ThotBool          higher;
  StyleSheetOrigin  Origin1, Origin2;
  PtrAttributePres  pBlock;

  higher = TRUE;
  if (pRule2)
    {
      if (pPS1)
        Origin1 = pPS1->PsOrigin;
      else
        Origin1 = Author;
      if (pPS2)
        Origin2 = pPS2->PsOrigin;
      else
        Origin2 = Author;
      /* check origin first */
      if (Origin1 != Origin2)
        /* rules have different origins */
        {
          /* check importance */
          if ((pRule1->PrImportant || pRule2->PrImportant) &&
              !(pRule1->PrImportant && pRule2->PrImportant))
            /* only one rule is important. This one wins */
            higher = pRule1->PrImportant;
          else
            /* both rules have the same importance */
            /* compare origins */
            higher = (Origin1 > Origin2);
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
            {
              /* take selectivity into account */
              if (pRule1->PrSpecificity != pRule2->PrSpecificity)
                higher = (pRule1->PrSpecificity > pRule2->PrSpecificity);
              else
                /* both rules have the same specificity. Take the order into
                   account */
                {
                  if (pPS1 != pPS2)
                    /* they come from different P schemas, the rule from the
                       latest P schema wins */
                    higher = TRUE;
                  else
                    /* both rules come from the same P schema */
                    {
                      if (pRule1->PrCSSURL == pRule2->PrCSSURL)
                        /* they come from the same CSS file, compare the line
                           numbers */
                        higher = (pRule1->PrCSSLine >= pRule2->PrCSSLine);
                      else
                        /* at least one rule comes from an imported CSS file */
                        {
                          /* priority is given by the order of the attribute
                             blocks */
                          if (attrBlock1 && attrBlock2)
                            {
                              pBlock = attrBlock1;
                              while (pBlock)
                                {
                                  if (pBlock == attrBlock2)
                                    /* the 2nd rule has a higher priority */
                                    {
                                      higher = FALSE;
                                      pBlock = NULL;
                                    }
                                  else
                                    pBlock = pBlock->ApNextAttrPres;
                                }
                            }
                        } 
                    }
                }
            }
        }
    }
  return higher;
}

/*----------------------------------------------------------------------
  CascRegistered
  If rule pRule is a candidate for the cascade, register it and return TRUE.
  ----------------------------------------------------------------------*/
static ThotBool CascRegistered (PtrPRule pRule, PtrPSchema pSchP,
                                PtrAttribute pAttr, PtrAttributePres pAttrBlk,
                                Cascade *casc)
{
  AllRules    *table;
  ThotBool    ret = FALSE;

  if (pRule->PrType == PtFunction && pRule->PrPresFunction == FnContent)
    {
      if (pRule->PrBoxType == BtBefore)
        {
          if (RuleHasHigherPriority (pRule, pSchP, pAttrBlk,
                                     casc->ContentRuleBefore, casc->schemaOfContentRuleBefore, NULL))
            {
              casc->ContentRuleBefore = pRule;
              casc->schemaOfContentRuleBefore = pSchP;
              casc->attrOfContentRuleBefore = pAttr;
            }
        }
      else if (pRule->PrBoxType == BtAfter)
        {
          if (RuleHasHigherPriority (pRule, pSchP, pAttrBlk,
                                     casc->ContentRuleAfter, casc->schemaOfContentRuleAfter, NULL))
            {
              casc->ContentRuleAfter = pRule;
              casc->schemaOfContentRuleAfter = pSchP;
              casc->attrOfContentRuleAfter = pAttr;
            }
        }
      ret = TRUE;
    }
  else
    /* presentation function are applied immediately */
    /* except BackgroundPicture */
    if (pRule->PrType != PtFunction ||
        (pRule->PrType == PtFunction &&
         pRule->PrPresFunction == FnBackgroundPicture))
      {
        if (pRule->PrBoxType == BtElement)
          table = &(casc->MainElement);
        else if (pRule->PrBoxType == BtBefore)
          table = &(casc->BeforePseudoEl);
        else if (pRule->PrBoxType == BtAfter)
          table = &(casc->AfterPseudoEl);
        else
          return FALSE;
        if (RuleHasHigherPriority (pRule, pSchP, pAttrBlk,
                                   table->selectedRule[pRule->PrType],
                                   table->schemaOfSelectedRule[pRule->PrType],
                                   table->attrBlockOfSelectedRule[pRule->PrType]))
          {
            /* register the rule in the appropriate rule table */
            table->selectedRule[pRule->PrType] = pRule;
            table->schemaOfSelectedRule[pRule->PrType] = pSchP;      
            table->attrOfSelectedRule[pRule->PrType] = pAttr;
            table->attrBlockOfSelectedRule[pRule->PrType] = pAttrBlk;
          }
        ret = TRUE;
      }
  return ret;
}

/*----------------------------------------------------------------------
  GetRulesFromInheritedAttributes
  get all presentation rules that element El inherits from attribute l
  belonging to ist ancestors, starting at element pFirstAncest
  ----------------------------------------------------------------------*/
static void GetRulesFromInheritedAttributes (PtrElement pEl,
                                             PtrElement pFirstAncest, int l, DocViewNumber viewNb,
                                             int viewSch, PtrDocument pDoc, PtrPSchema pSchPres,
                                             ThotBool extens, Cascade *casc,
                                             PtrAbstractBox *pAbbReturn, ThotBool forward,
                                             int *lqueue, RuleQueue* queue,
                                             PtrAbstractBox pNewAbbox, FILE *fileDescriptor)
{
  PtrAttribute       pAttr = NULL;
  PtrElement         pElAttr;
  PtrSSchema	       pSSattr;
  PtrAttributePres   attrBlock;
  PtrPRule           pRule, pR;
  int                view, valNum, match;

  do
    {
      pAttr = GetTypedAttrAncestor (pFirstAncest, l, pEl->ElStructSchema, &pElAttr);
      if (pAttr != NULL)
        {
          pFirstAncest = pElAttr->ElParent;
          view = AppliedView (pEl, pAttr, pDoc, viewNb);
          pSSattr = pAttr->AeAttrSSchema;
          if (pDoc->DocView[viewNb-1].DvPSchemaView == 1 && extens &&
              /* it's a P schema extension */
              /* if it's an ID, class or pseudo-class attribute, take P schema
                 extensions associated with the document S schema*/
              (AttrHasException (ExcCssClass, pAttr->AeAttrNum,
                                 pAttr->AeAttrSSchema) ||
               AttrHasException (ExcCssId, pAttr->AeAttrNum,
                                 pAttr->AeAttrSSchema) ||
               AttrHasException (ExcCssPseudoClass, pAttr->AeAttrNum,
                                 pAttr->AeAttrSSchema)))
            pSSattr = pDoc->DocSSchema;
          /* process all values of the attribute, in case of a
             text attribute with a list of values */
          valNum = 1; match = 1;
          do
            {
              /* first rule for this value of the attribute */
              pR = AttrPresRule (pAttr, pEl, TRUE, NULL, pSchPres, &valNum,
                                 &match, &attrBlock);
              /* look at all rules associated with this value */
              while (pR != NULL)
                {
                  pRule = NULL;
                  if (!pR->PrDuplicate &&
                      /* if it's a creation rule, apply it now */
                      !ApplCrRule (pR, pSSattr, pSchPres, pAttr, pAbbReturn,
                                   viewNb, pDoc, pEl, forward, lqueue, queue,
                                   pNewAbbox, fileDescriptor))
                    /* not a creation rule, get the right rule*/
                    pRule = GetNextAttrPresRule (&pR, pAttr->AeAttrSSchema,
                                                 pAttr, pElAttr, pDoc, pEl,
                                                 view);
                  if (pRule && DoesViewExist (pEl, pDoc, viewNb))
                    /* this rule applies to the element */
                    {
                      if (viewSch != 1 ||
                          !CascRegistered (pRule, pSchPres, pAttr, attrBlock,
                                           casc))
			{
			  if (fileDescriptor)
			    DisplayPRule (pRule, fileDescriptor, pEl, pSchPres,
					  0);
			  else if (!ApplyRule (pRule, pSchPres, pNewAbbox, pDoc,
					       pAttr, pNewAbbox))
			    /* not the main view, apply the rule now */
			    WaitingRule (pRule, pNewAbbox, pSchPres, pAttr, NULL,
					 queue, lqueue);
			}
                    }
                  /* next rule associated with this value of the attribute */
                  pR = pR->PrNextPRule;
                }
            }
          while (valNum > 0);

          /* look for more ancestors having this attribute only if it's a
             P schema extension and if the attribute is id, class or
             pseudo-class */
          if (pDoc->DocView[viewNb-1].DvPSchemaView != 1 || !extens ||
              !(AttrHasException (ExcCssClass, pAttr->AeAttrNum,
                                  pAttr->AeAttrSSchema) ||
                AttrHasException (ExcCssId, pAttr->AeAttrNum,
                                  pAttr->AeAttrSSchema) ||
                AttrHasException (ExcCssPseudoClass, pAttr->AeAttrNum,
                                  pAttr->AeAttrSSchema)))
            pAttr = NULL;
        }
    }
  while (pAttr);
}

/*----------------------------------------------------------------------
  CounterRuleForElem
  Generate in the file the CSS syntax of counting rules from schema pSchP
  that are triggered by element pEl.
  ----------------------------------------------------------------------*/
static void CounterRuleForElem (FILE *fileDescriptor, PtrElement pEl,
				PtrPSchema pSchP, PtrSSchema pSchS,
				PtrDocument pDoc, int view)
{
  int       i, j;

  for (i = 0; i < pSchP->PsNCounters; i++)
    {
      for (j = 0; j < pSchP->PsCounter[i].CnNItems; j++)
	{
	  if ((pSchP->PsCounter[i].CnItem[j].CiCntrOp == CntrSet ||
	      pSchP->PsCounter[i].CnItem[j].CiCntrOp == CntrAdd) &&
	      pSchP->PsCounter[i].CnItem[j].CiElemType == pEl->ElTypeNumber &&
	      pSchS == pEl->ElStructSchema)
	    {
	      if (!pSchP->PsCounter[i].CnItem[j].CiCond ||
		  CondPresentation (pSchP->PsCounter[i].CnItem[j].CiCond, pEl,
				    NULL, NULL, NULL, view, pSchS, pDoc))
		DisplayCounterRule (i, j, fileDescriptor, pEl, pSchP);
	    }
	}
    }
}

/*----------------------------------------------------------------------
  ApplyPresRules applies all presentation rules to a new abstract box
  if fileDescriptor is NULL, otherwise displays the origin of presentation
  rules.
  ----------------------------------------------------------------------*/
void ApplyPresRules (PtrElement pEl, PtrDocument pDoc,
                     DocViewNumber viewNb,
                     int viewSch, PtrSSchema pSchS, PtrPSchema pSchP,
                     PtrPRule *pRSpec, PtrPRule *pRDef,
                     PtrAbstractBox *pAbbReturn, ThotBool forward,
                     int *lqueue, void* rQueue,
                     PtrAbstractBox pNewAbbox, void* CSScasc,
                     FILE *fileDescriptor, ThotBool pseudoElOnly)
{
  int                 i, view, l, valNum, match;
  PtrPRule            pRuleView, pRule = NULL, pR;
  PtrHandlePSchema    pHd;
  PtrPSchema          pSchPres, pSchPattr;
  PtrAttribute        pAttr;
  PtrElement          pFirstAncest;
  PtrSSchema	        pSSattr;
  PtrAttributePres    attrBlock;
  InheritAttrTable   *inheritTable;
  Cascade            *casc;
  Cascade             casc1;
  RuleQueue          *queue;
  ThotBool            stop;

  queue = (RuleQueue*)rQueue;
  /* initialize the cascade: no rule selected yet */
  if (!CSScasc)
    casc = &casc1;
  else
    casc = (Cascade*)CSScasc;
  memset (casc, 0, sizeof (Cascade));
  /* get all rules associated with the element type in the main presentation */
  /* schema (default stylesheet of the user agent in CSS terms) */
  pRuleView = NULL;
  do
    {
      /* get the rule to be applied for view 1 (main view) */
      pRule = GetRule (pRSpec, pRDef, pEl, NULL, pSchS, pDoc);
      if (pRule)
        {
        /* if it's a rule that creates a presentation box, apply it */
        if (!ApplCrRule (pRule, pSchS, pSchP, NULL, pAbbReturn, viewNb, pDoc,
                         pEl, forward, lqueue, queue, pNewAbbox,
                         fileDescriptor))
          /* it's not a creation rule */
          /* get the rules for the same property in all other views */
          for (view = 1; view <= MAX_VIEW; view++)
            {
              if (view == 1)
                pRuleView = NULL;
              else
                pRuleView = GetRuleView (pRSpec, pRDef, pRule->PrType, view,
                                         pEl, NULL, pSchS, pDoc);
              if (view == viewSch && pNewAbbox &&
                  DoesViewExist (pEl, pDoc, viewNb))
                {
                  if (pRuleView == NULL)
                    /* no rule for this view. Take the rule for view 1 */
                    pRuleView = pRule;
                  /* if it's the main view, register the rule for the cascade*/
                  if (!CascRegistered (pRuleView, pSchP, NULL, NULL, casc))
                    {
                    if (fileDescriptor)
                      DisplayPRule (pRuleView, fileDescriptor, pEl, pSchP, 0);
                    else if (!ApplyRule (pRuleView, pSchP, pNewAbbox, pDoc,
                                         NULL, pNewAbbox))
                      /* it's a presentation function, apply the rule now */
                      WaitingRule (pRuleView, pNewAbbox, pSchP, NULL, NULL,
                                   queue, lqueue);
                    }
                }
            }
        } 
    }
  while (pRule);

  if (pNewAbbox == NULL && fileDescriptor == NULL)
    return;

  /* fetch all rules that apply to any element type in all extensions of the
     document's P schema */
  /* We need to do that only if we are working for the main view and if
     the element is not a basic or hidden element */
  if (viewSch == 1 &&     /* main view */
      CanApplyCSSToElement (pEl))
    {
      /* get the first P schema extension */
      pHd = FirstPSchemaExtension (pDoc->DocSSchema, pDoc, pEl);
      while (pHd)
        {
          pSchPres = pHd->HdPSchema;
	  if (fileDescriptor)
	    /* check all counters defined in the P schema that may be affected
	       by the element */
	    CounterRuleForElem (fileDescriptor, pEl, pSchPres, pSchS, pDoc,
				viewSch);
          /* look at the rules that apply to any element type in this
             P schema extension */
          pRule = pSchPres->PsElemPRule->ElemPres[AnyType];
          while (pRule)
            {
              if (pRule->PrCond == NULL ||
                  CondPresentation (pRule->PrCond, pEl, NULL, NULL, pRule, 1,
                                    pEl->ElStructSchema, pDoc))
                /* conditions are ok */
                {
                  /* keep that rule only if it has a higher priority than the
                     the rule for the same property we have already
                     encountered */
                  if (!CascRegistered (pRule, pSchPres, NULL, NULL, casc))
		    {
		      if (fileDescriptor)
			DisplayPRule (pRule, fileDescriptor, pEl, pSchPres, 0);
		      else 
			/* if it's a creation rule, apply it now */
			if (!ApplCrRule (pRule, pSchS, pSchPres, NULL,pAbbReturn,
					 viewNb, pDoc, pEl, forward, lqueue,
					 queue, pNewAbbox, fileDescriptor) &&
			    !ApplyRule (pRule, pSchPres, pNewAbbox, pDoc, NULL,
					pNewAbbox))
			  WaitingRule (pRule, pNewAbbox, pSchPres, NULL, NULL,
				       queue, lqueue);
		    }
                }
              /* next rule for all element types in the same P schema
                 extension */
              pRule = pRule->PrNextPRule;
            }
          /* next P schema extension */
          pHd = pHd->HdNextPSchema;
        }
    }

  /* look at all additional P schemas (CSS style sheets) in the order of
     their weight, but look first at the main presentation schema to
     get presentation rules associated with attributes */
  pHd = NULL;
  /* look at the main presentation schema first */
  pSchPres = PresentationSchema (pEl->ElStructSchema, pDoc);
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
          /* look at the rules that apply to the element type in this
             P schema extension */
          pRule = pSchPres->PsElemPRule->ElemPres[pEl->ElTypeNumber - 1];
          while (pRule)
            {
              if (pRule->PrCond == NULL ||
                  CondPresentation (pRule->PrCond, pEl, NULL, NULL, pRule, 1,
                                    pEl->ElStructSchema, pDoc))
                /* conditions are ok */
                {
                  /* keep that rule only if it has a higher priority than the
                     rule for the same property we have already encountered */
                  if (!CascRegistered (pRule, pSchPres, NULL, NULL, casc))
		    {
		      if (fileDescriptor)
			DisplayPRule (pRule, fileDescriptor, pEl, pSchPres, 0);
		      else
			/* if it's a creation rule, apply it now */
			if (!ApplCrRule (pRule, pSchS, pSchPres, NULL,pAbbReturn,
					 viewNb, pDoc, pEl, forward, lqueue, queue, pNewAbbox,
					 fileDescriptor) && 
			    !ApplyRule (pRule, pSchPres, pNewAbbox, pDoc, NULL, pNewAbbox))
                          WaitingRule (pRule, pNewAbbox, pSchPres, NULL, NULL,
                                       queue, lqueue);
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
          if (pSchPres->PsNInheritedAttrs->Num[pEl->ElTypeNumber - 1])
            /* the element type inherits some attributes */
            {
              inheritTable = pSchPres->PsInheritedAttr->ElInherit[pEl->ElTypeNumber - 1];
              if (!inheritTable)
                /* inheritance table does not exist. Create it */
                {
                  CreateInheritedAttrTable (pEl->ElTypeNumber,
                                            pEl->ElStructSchema, pSchPres, pDoc);
                  inheritTable = pSchPres->PsInheritedAttr->ElInherit[pEl->ElTypeNumber-1];
                }
              for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
                if ((*inheritTable)[l - 1])
                  /* pEl inherits attribute l */
                  {
                    /* is this attribute present on an ancestor? */
                    if ((*inheritTable)[l - 1] == 'S')
                      pFirstAncest = pEl;
                    else
                      pFirstAncest = pEl->ElParent;
                    /* look for all ancestors having this attribute */
                    GetRulesFromInheritedAttributes (pEl, pFirstAncest, l, viewNb, viewSch, pDoc, pSchPres, (pHd != NULL), casc, pAbbReturn, forward, lqueue, queue, pNewAbbox, fileDescriptor);
                  }
            }

          if (pEl->ElTypeNumber > MAX_BASIC_TYPE &&
              pSchPres->PsNInheritedAttrs->Num[AnyType])
            /* some attributes are inherited by all element types */
            {
              inheritTable = pSchPres->PsInheritedAttr->ElInherit[AnyType];
              if (!inheritTable)
                /* inheritance table does not exist. Create it */
                {
                  CreateInheritedAttrTable (AnyType+1, pEl->ElStructSchema,
                                            pSchPres, pDoc);
                  inheritTable = pSchPres->PsInheritedAttr->ElInherit[AnyType];
                }
              for (l = 1; l <= pEl->ElStructSchema->SsNAttributes; l++)
                if ((*inheritTable)[l - 1])
                  /* pEl inherits attribute l */
                  {
                    /* is this attribute present on an ancestor? */
                    if ((*inheritTable)[l - 1] == 'S')
                      pFirstAncest = pEl;
                    else
                      pFirstAncest = pEl->ElParent;
                    /* look for all ancestors having this attribute */
                    GetRulesFromInheritedAttributes (pEl, pFirstAncest, l, viewNb, viewSch, pDoc, pSchPres, (pHd != NULL), casc, pAbbReturn, forward, lqueue, queue, pNewAbbox, fileDescriptor);
                  }
            }

          /* now get the rules associated with the attributes of the element */
          pAttr = pEl->ElFirstAttr;	/* first attribute of element */
          /* check all attributes of element */
          while (pAttr)
            {
              if (pHd == NULL)
                /* main presentation schema. Take the one associated with
                   the attribute S schema */
                pSchPattr = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
              else
                pSchPattr = pSchPres;
              view = AppliedView (pEl, pAttr, pDoc, viewNb);
              pSSattr = pAttr->AeAttrSSchema;
              if (pDoc->DocView[viewNb - 1].DvPSchemaView == 1 && pHd &&
                  /* this is a P schema extension */
                  /* if it's an ID, class or pseudo-class attribute, take
                     P schema extensions associated with the document
                     S schema */
                  (AttrHasException (ExcCssClass, pAttr->AeAttrNum, pSSattr) ||
                   AttrHasException (ExcCssId, pAttr->AeAttrNum, pSSattr) ||
                   AttrHasException (ExcCssPseudoClass, pAttr->AeAttrNum, pSSattr)))
                pSSattr = pDoc->DocSSchema;
              /* process all values of the attribute, in case of a text
                 attribute with multiple values */
              valNum = 1; match = 1;
              do
                {
                  /* first rule for this value of the attribute */
                  pR = AttrPresRule (pAttr, pEl, FALSE, NULL, pSchPattr,
                                     &valNum, &match, &attrBlock);
                  /* look for all rules associated with this value */
                  while (pR)
                    {
                      pRule = NULL;
                      if (!pR->PrDuplicate &&
                          /* if it's a creation rule, apply it now */
                          !ApplCrRule (pR, pSSattr, pSchPattr, pAttr,
                                       pAbbReturn, viewNb, pDoc, pEl, forward,
                                       lqueue, queue, pNewAbbox, fileDescriptor))
                        /* not a creation rule, get the right rule */
                        pRule = GetNextAttrPresRule (&pR,
                                                     pSSattr, pAttr,
                                                     pEl, pDoc, pEl, view);
                      if (pRule && DoesViewExist (pEl, pDoc, viewNb))
                        /* this rule applies to the element */
                        {
                          if (viewSch != 1 ||
                              !CascRegistered (pRule, pSchPattr, pAttr,
                                               attrBlock, casc))
			    {
			      if (fileDescriptor)
				DisplayPRule (pRule, fileDescriptor, pEl,
					      pSchPattr, 0);
			      else if (!ApplyRule (pRule, pSchPattr,
						   pNewAbbox, pDoc, pAttr, pNewAbbox))
				/* not the main view, apply the rule now */
				WaitingRule (pRule, pNewAbbox, pSchPattr,
					     pAttr, NULL, queue, lqueue);
			    }
                        }
                      /* next rule associated with this value of the attr*/
                      pR = pR->PrNextPRule;
                    }
                }
              while (valNum > 0);
              /* get the next attribute of the element */
              pAttr = pAttr->AeNext;
            }
        }
      if (pHd)
        /* next style sheet (P schema extension) */
        pHd = pHd->HdNextPSchema;
      else if (viewSch == 1 &&     /* main view */
               CanApplyCSSToElement (pEl))
        /* it was the main P schema, get the first schema extension */
        pHd = FirstPSchemaExtension (pEl->ElStructSchema, pDoc, pEl);
      if (pHd)
        pSchPres = pHd->HdPSchema;
      else
        /* no schema any more. stop */
        pSchPres = NULL;
    }

  /* get all specific presentation rules attached to the element itself */
  pRule = pEl->ElFirstPRule;
  while (pRule)
    /* get the rule if it applies to the view and if it's not a page height */
    {
      if (!(pEl->ElTypeNumber == PageBreak + 1 && pRule->PrType == PtHeight))
        if ((pRule->PrViewNum == viewSch || pRule->PrType == PtPictInfo) &&
            DoesViewExist (pEl, pDoc, viewNb))
          {
            if (viewSch != 1 ||
                (pRule->PrType == PtFunction &&
                 pRule->PrPresFunction != FnBackgroundPicture))
              {
                if (fileDescriptor)
                  DisplayPRule (pRule, fileDescriptor, pEl, pSchP, 0);
                else if (!ApplyRule (pRule, pSchP, pNewAbbox, pDoc, NULL,
                                     pNewAbbox))
                  /* not the main view, apply the rule now */
                  WaitingRule (pRule, pNewAbbox, pSchP, NULL, NULL, queue,
                               lqueue);
              }
            else if (!casc->MainElement.selectedRule[pRule->PrType] ||
                     RuleHasHigherPriority (pRule, NULL, NULL,
                                            casc->MainElement.selectedRule[pRule->PrType],
                                            casc->MainElement.schemaOfSelectedRule[pRule->PrType],
                                            casc->MainElement.attrBlockOfSelectedRule[pRule->PrType]))
              {
                if (pRule->PrSpecifAttr == 0)
                  /* this rule does not depend on an attribute */
                  pAttr = NULL;
                else
                  /* look for the attribute related to the rule */
                  {
                    pAttr = pEl->ElFirstAttr;
                    stop = FALSE;
                    while (pAttr && !stop)
                      if (pAttr->AeAttrNum == pRule->PrSpecifAttr &&
                          !strcmp (pAttr->AeAttrSSchema->SsName,
                                   pRule->PrSpecifAttrSSchema->SsName))
                        stop = TRUE;
                      else
                        pAttr = pAttr->AeNext;
                  }
                casc->MainElement.selectedRule[pRule->PrType] = pRule;
                casc->MainElement.schemaOfSelectedRule[pRule->PrType] = pSchP;
                casc->MainElement.attrOfSelectedRule[pRule->PrType] = pAttr;
                casc->MainElement.attrBlockOfSelectedRule[pRule->PrType] = NULL;
              }
          }
      pRule = pRule->PrNextPRule;
    }

  /* apply all selected rules */
  if (!pseudoElOnly)
    for (i = 0; i < PtPictInfo; i++)
      if (casc->MainElement.selectedRule[i])
        {
          if (fileDescriptor)
            DisplayPRule (casc->MainElement.selectedRule[i], fileDescriptor,
                          pEl, casc->MainElement.schemaOfSelectedRule[i], 0);
          else if (!ApplyRule (casc->MainElement.selectedRule[i],
                               casc->MainElement.schemaOfSelectedRule[i],
                               pNewAbbox, pDoc,
                               casc->MainElement.attrOfSelectedRule[i],
                               pNewAbbox))
            WaitingRule (casc->MainElement.selectedRule[i], pNewAbbox,
                         casc->MainElement.schemaOfSelectedRule[i],
                         casc->MainElement.attrOfSelectedRule[i], 
                         NULL, queue, lqueue);
        }
  if (fileDescriptor)
    {
      if (casc->ContentRuleBefore && casc->ContentRuleBefore->PrPresBox[0] > 0)
        {
          fprintf (fileDescriptor, ":before\n");
          DisplayPRule (casc->ContentRuleBefore, fileDescriptor, pEl,
                        casc->schemaOfContentRuleBefore, 3);
          for (i = 0; i < PtPictInfo; i++)
            DisplayPRule (casc->BeforePseudoEl.selectedRule[i], fileDescriptor,
                          pEl, casc->BeforePseudoEl.schemaOfSelectedRule[i], 3);
        }
      if (casc->ContentRuleAfter && casc->ContentRuleAfter->PrPresBox[0] > 0)
        {
          fprintf (fileDescriptor, ":after\n");
          DisplayPRule (casc->ContentRuleAfter, fileDescriptor, pEl,
                        casc->schemaOfContentRuleAfter, 3);
          for (i = 0; i < PtPictInfo; i++)
            DisplayPRule (casc->AfterPseudoEl.selectedRule[i], fileDescriptor,
                          pEl, casc->AfterPseudoEl.schemaOfSelectedRule[i], 3);
        }
    }
  else if (pNewAbbox)
    {
      if (casc->ContentRuleBefore &&
          casc->ContentRuleBefore->PrPresBox[0] > 0 &&
          (!pseudoElOnly || !forward))
        /* create a :before pseudo-element if element pEl is complete */
        {
          if (pNewAbbox->AbLeafType != LtCompound ||
              pNewAbbox->AbInLine || !pNewAbbox->AbTruncatedHead)
            CrAbsBoxesPres (pEl, pDoc, casc->ContentRuleBefore, pSchS,
                            casc->attrOfContentRuleBefore, viewNb,
                            casc->schemaOfContentRuleBefore,
                            &(casc->BeforePseudoEl), FALSE);
        }
      if (casc->ContentRuleAfter &&
          casc->ContentRuleAfter->PrPresBox[0] > 0)
        /* create a :after pseudo-element if element pEl is complete */
        {
          if (pseudoElOnly)
            {
              if (forward)
                CrAbsBoxesPres (pEl, pDoc, casc->ContentRuleAfter, pSchS,
                                casc->attrOfContentRuleAfter, viewNb,
                                casc->schemaOfContentRuleAfter,
                                &(casc->AfterPseudoEl), FALSE);
            }
          else if (pNewAbbox->AbLeafType != LtCompound ||
                   pNewAbbox->AbInLine || !pNewAbbox->AbTruncatedTail)
            /* on appliquera la regle de creation quand tous les paves */
            /* descendants de l'element seront crees */
            WaitingRule (casc->ContentRuleAfter, pNewAbbox,
                         casc->schemaOfContentRuleAfter,
                         casc->attrOfContentRuleAfter,
                         &(casc->AfterPseudoEl), queue, lqueue);
        }
    }
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

  /* cherche dans cette vue le premier element ascendant qui ait un pave' */
  pP = NULL;
  pE = pEl;
  while (pE->ElParent && pP == NULL)
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
      pAb->AbEnclosing = pP;
      UpdateCSSVisibility (pAb);
      if (pAb->AbEnclosing->AbFirstEnclosed == NULL)
        /* c'est le premier pave englobe */
        {
          pAb->AbEnclosing->AbFirstEnclosed = pAb;
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
            pAb->AbNext = pAb->AbEnclosing->AbFirstEnclosed;
            if (pAb->AbNext != NULL)
              pAb->AbNext->AbPrevious = pAb;
            pAb->AbEnclosing->AbFirstEnclosed = pAb;
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
                pAb->AbPrevious = pP;
                pAb->AbNext = pP->AbNext;
              }
            else
              /* cet element n'a pas encore de paves dans cette vue */
              {
                pEl->ElAbstractBox[nv - 1] = pAb;
                /* 1er pave de l'element */
                /* cherche l'element precedent ayant un pave dans la vue */
                pE = BackSearchVisibleElem (pAb->AbEnclosing->AbElement, pEl, nv);
                if (pE)
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
                    while (!(pP == pAb->AbEnclosing || pP == NULL));
                    if (pP == NULL)
                      /* ils n'ont pas le meme pave englobant, on ne */
                      /* chainera pas le pave au pave de l'element precedent */
                      pE = NULL;
                  }
                if (pE && pE->ElTypeNumber == PageBreak + 1 &&
                    pE->ElPageType == PgBegin)
                  /* le precedent est une marque de page de debut */
                  /* d'element, on verifie si elle est suivie par des */
                  /* paves de presentation de l'englobant */
                  {
                    pP = pE->ElAbstractBox[nv - 1];
                    /* pave de l'element precedent */
                    if (pP->AbNext &&
                        pP->AbNext->AbElement == pAb->AbEnclosing->AbElement)
                      /* la marque de page est suivie par un pave cree */
                      /* par l'englobant */
                      pE = NULL;
                  }
                if (pE == NULL)
                  /* pas de pave d'element precedent */
                  {
                    pP = pAb->AbEnclosing->AbFirstEnclosed;
                    /* saute les eventuelles marques de page de debut */
                    /* d'element */
                    stop = FALSE;
                    do
                      if (pP == NULL)
                        stop = TRUE;
                      else
                        {
                           if (pP->AbElement->ElTypeNumber == PageBreak + 1 &&
                              pP->AbElement->ElPageType == PgBegin)
                            pP = pP->AbNext;
                          else
                            stop = TRUE;
                        }
                    while (!stop);
                    if (pP)
                      {
                        if (pP->AbElement == pAb->AbEnclosing->AbElement)
                          {
                            if (TypeCreatedRule (pDoc, pAb->AbEnclosing, pP) == FnCreateLast)
                              /* le pave existant doit etre le dernier, on insere */
                              /* le nouveau pave devant lui */
                              {
                                pAb->AbNext = pP;
                                pAb->AbEnclosing->AbFirstEnclosed = pAb;
                              }
                            else
                              /* on saute les paves crees par une regle */
                              /* CreateFirst de l'englobant */
                              {
                                stop = FALSE;
                                do
                                  if (pP->AbNext == NULL)
                                    stop = TRUE;
                                  else if (pP->AbNext->AbPresentationBox &&
                                           pP->AbNext->AbTypeNum == 0 &&
                                           pP->AbNext->AbListStylePosition == 'I')
                                    /* list item marker inside */
                                    pP = pP->AbNext;
                                  else if (pP->AbNext->AbElement !=
                                           pAb->AbEnclosing->AbElement)
                                    stop = TRUE;
                                  else if (TypeCreatedRule (pDoc,
                                                            pAb->AbEnclosing, pP->AbNext) == FnCreateLast)
                                    /* le pave suivant doit etre le dernier */
                                    stop = TRUE;
                                  else
                                    pP = pP->AbNext;
                                while (!(stop));
                                /* on insere le nouveau pave apres */
                                pAb->AbPrevious = pP;
                                pAb->AbNext = pP->AbNext;
                              }
                          }
                        else if (pP->AbPresentationBox &&
                                 pP->AbTypeNum == 0 &&
                                 pP->AbListStylePosition == 'I')
                          /* this is a list item marker inside the item,
                             skip it */
                          {
                            pAb->AbPrevious = pP;
                            pAb->AbNext = pP->AbNext;
                          }
                        else
                          /* insere le nouveau pave en tete */
                          {
                            pAb->AbNext = pP;
                            pAb->AbEnclosing->AbFirstEnclosed = pAb;
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
                    pAb->AbPrevious = pP;
                    pAb->AbNext = pP->AbNext;
                  }
              }
            if (pAb->AbPrevious != NULL)
              pAb->AbPrevious->AbNext = pAb;
            if (pAb->AbNext != NULL)
              pAb->AbNext->AbPrevious = pAb;
          }
    }
  if (pEl->ElHolophrast || (pEl->ElTerminal && pEl->ElLeafType != LtPageColBreak))
    /* met le contenu de l'element dans le pave, sauf si c'est un */
    /* element de haut ou de bas de page */
    {
      FillContent (pEl, pAb, pDoc);
      /* ajoute le volume du pave a celui de tous ses englobants */
      if (pAb->AbVolume > 0)
        {
          pP = pAb->AbEnclosing;
          while (pP != NULL)
            {
              pP->AbVolume += pAb->AbVolume;
              pP = pP->AbEnclosing;
            }
        }
    }
  else
    {
      pAb->AbLeafType = LtCompound;
      pAb->AbVolume = 0;
      pAb->AbInLine = FALSE;
      pAb->AbTruncatedHead = TRUE;
      pAb->AbTruncatedTail = TRUE;
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
  PtrPRule            pRule = NULL, pRDef, pRSpec;
  PtrElement          pElChild, pElParent, pAsc;
  PtrAbstractBox      pAbChild, pNewAbbox, pAbReturn, pAbPres;
  PtrAbstractBox      pAb, pAbParent;
  PtrSSchema          pSchS, savePSS;
  PtrAttribute        pAttr;
  RuleQueue           queue;
  Cascade             casc;
  AllRules*           rulesPseudo = NULL;
  int                 vis, typePres = 0;
  int                 viewSch;
  int                 index;
  int                 lqueue, pqueue;
  ThotBool            completeChild;
  ThotBool            stop, ok, crAbsBox, truncate;
  ThotBool            notBreakable, ignoreDescent;
  ThotBool            Creation, ApplyRules;
  ThotBool            pFirst, pLast, cssUndisplay;

  pAbReturn = NULL;
  lqueue = 0;
  pqueue = 0;
  /* Abstract boxes of the element are not created */
  *complete = FALSE;
  if (pEl && pEl->ElStructSchema)
    {
      viewSch = AppliedView (pEl, NULL, pDoc, viewNb);
      /* pointeur sur le pave qui sera cree' pour l'element */
      pNewAbbox = NULL;
      ApplyRules = FALSE;
      ignoreDescent = FALSE;
      pAbPres = NULL;
      notBreakable = FALSE;
      /* pas tous crees */
      Creation = FALSE;	/* a priori rien a creer */
      pAb = pEl->ElAbstractBox[viewNb - 1];
      if (pAb && !pAb->AbDead)
        {
          /* le pave existe deja pour cette vue */
          Creation = FALSE;
          /* on saute les paves de presentation crees par CreateWith */
          while (pAb->AbPresentationBox && pAb->AbNext != NULL)
            pAb = pAb->AbNext;
          if (pAb->AbLeafType != LtCompound || pAb->AbInLine)
            /* c'est une feuille ou un pave' mis en lignes, il */
            /* a deja tout son contenu */
            *complete = TRUE;
          else if (forward)
            *complete = !pAb->AbTruncatedTail;
          else
            *complete = !pAb->AbTruncatedHead;
          if (pAb->AbSize == -1)
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

          if (!pSchP)
            /* no presentation schema. Can't create a box for this element */
            return NULL;

          /* pRSpec: premiere regle de presentation associee au type de
             l'element */
          pRSpec = pSchP->PsElemPRule->ElemPres[index - 1];
          /* premiere regle de presentation par defaut */
          pRDef = pSchP->PsFirstDefaultPRule;
          /* initialise la file des regles qui n'ont pas pu etre appliquees*/
          lqueue = 0;
          pqueue = 0;
	    
          /* determine la visibilite du pave a creer */
          cssUndisplay = ComputeVisib (pEl, pDoc, viewNb, viewSch, &pRSpec,
                                       &pRDef, &vis, &ignoreDescent,
                                       complete, &typePres, &pSchPPage);
	    
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
                  /* alors son pave est complet */
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
                  // generate the CSS visibility of the top element
                  if (pEl->ElParent == NULL)
                    pNewAbbox->AbVis = 'V';
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
                  pAbReturn = pNewAbbox;
                  if (descent)	/* on va creer les paves inclus */
                    {
                      pAb = pNewAbbox;
                      if (pAb->AbLeafType == LtCompound)
                        if (!pAb->AbInLine)
                          if (forward)
                            /* on creera au moins le 1er pave inclus */
                            pAb->AbTruncatedHead = FALSE;
                          else
                            /* on creera au moins le dernier pave inclus */
                            pAb->AbTruncatedTail = FALSE;
                        else
                          /* pave mis en ligne, on cree tout */
                          *complete = TRUE;
                      else
                        /* pave feuille, on cree tout */
                        *complete = TRUE;
                    }
                }
            }
          else if (cssUndisplay && pEl->ElPrevious && pEl->ElNext &&
                   TypeHasException (ExcHidden, pEl->ElPrevious->ElTypeNumber,
                                     pEl->ElPrevious->ElStructSchema) &&
                   TypeHasException (ExcHidden, pEl->ElNext->ElTypeNumber,
                                     pEl->ElNext->ElStructSchema))
            {
              // set inline the enclosing abstract box
              pElParent = pEl->ElParent;
              while (pElParent && pElParent->ElStructSchema &&
                     pElParent->ElStructSchema->SsName &&
                     !strcmp (pElParent->ElStructSchema->SsName, "Template"))
                {
                  // Skip template elements
                  pAbParent = pElParent->ElAbstractBox[viewNb - 1];
                  if (pAbParent)
                    pAbParent->AbBuildAll = TRUE;
                  pElParent = pElParent->ElParent;
                }
              if (pElParent && pElParent->ElAbstractBox[viewNb - 1])
                {
                  pAbParent = pElParent->ElAbstractBox[viewNb - 1];
                  pAbParent->AbInLine = TRUE;
                  pAbParent->AbBuildAll = TRUE;
                  pAbParent->AbChange = TRUE;
                }
            }
        }
	
      if (pNewAbbox == NULL && Creation &&
          !ignoreDescent && !pEl->ElHolophrast)
        {
          /* cet element n'est pas visible dans la vue, on cherche a creer */
          /* les paves d'un descendant visible */
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
                      pAb = NULL;
                      while (pAsc != NULL && pAb == NULL)
                        {
                          pAb = pAsc->ElAbstractBox[viewNb - 1];
                          /* on cherche le pave principal de cet ascendant dans la vue */
                          stop = FALSE;
                          do
                            if (pAb == NULL)
                              stop = TRUE;
                            else if (pAb->AbPresentationBox)
                              pAb = pAb->AbNext;
                            else
                              stop = TRUE;
                          while (!stop);
                          if (pAb != NULL)
                            /* cet ascendant a un pave, est-il secable ? */
                            notBreakable = !(IsBreakable (pAb, pDoc));
                          else
                            /* pas de pave, on passe a l'ascendant du dessus */
                            pAsc = pAsc->ElParent;
                        }
                    }
                  while (pElChild != NULL)
                    /* cree les paves d'un descendant */
                    {
                      pAb = AbsBoxesCreate (pElChild, pDoc, viewNb, forward, descent, &completeChild);
                      if (pAb != NULL)
                        pAbReturn = pAb;
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
                              &pRSpec, &pRDef, &pAbReturn, forward, &lqueue,
                              &queue, pNewAbbox, &casc, NULL, FALSE);
		
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
              pAbPres = NULL;
              pAb = pEl->ElAbstractBox[viewNb - 1];
              stop = FALSE;
              do
                if (pAb == NULL)
                  stop = TRUE;
                else if (pAb->AbPresentationBox)
                  pAb = pAb->AbNext;
                else
                  stop = TRUE;
              while (!stop);
              /* determine le 1er pave fils a creer */
              pElChild = pEl->ElFirstChild;  /* premier fils de l'element */
              if (pElChild == NULL)
                /* le pave n'a pas de descendance */
                /* pave vide, il n'est pas coupe' */
                {
                  *complete = TRUE;
                  pAbPres = TruncateOrCompleteAbsBox (pNewAbbox, FALSE, (ThotBool)(!forward), pDoc);
                  if (pAbPres != NULL)
                    /* on a cree des paves de presentation */
                    if (pAbPres->AbEnclosing != pNewAbbox)
                      pAbReturn = pAbPres;
                }
              else if (Creation || !IsBreakable (pAb, pDoc))
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

              pAbParent = pAb;
              /* cree les paves des fils successifs */
              while (pElChild != NULL)
                /* verifie que la vue n'est pas pleine */
                if (IsBreakable (pAbParent, pDoc) && IsViewFull (viewNb, pDoc, pEl))
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
                    if (pElParent)
                      pAbPres = TruncateOrCompleteAbsBox (pElParent->ElAbstractBox[viewNb - 1], TRUE, (ThotBool)(!forward), pDoc);
                  }
                else
                  /* Cree les paves d'un fils et passe a un autre fils */
                  {
                    /* a priori on va creer les paves du fils */
                    ok = TRUE;
                    if (!ApplyRules)
                      /* verifie si le fils est deja completn */
                      if (pElChild->ElAbstractBox[viewNb - 1] != NULL)
                        /* le fils a deja des paves dans cette vue */
                        {
                          pAbChild = pElChild->ElAbstractBox[viewNb - 1];
                          /* saute les paves de presentation du fils */
                          stop = FALSE;
                          do
                            if (pAbChild->AbNext == NULL)
                              stop = TRUE;
                            else if (pAbChild->AbPresentationBox)
                              pAbChild = pAbChild->AbNext;
                            else
                              stop = TRUE;
                          while (!stop);
                          /* on appellera AbsBoxesCreate s'il faut appliquer les */
                          /* regles de presentation de ce fils */
                          if (pAbChild->AbSize != -1)
                            {
                              if (pAbChild->AbLeafType != LtCompound ||
                                  pAbChild->AbInLine)
                                ok = FALSE;	/* pave fils complete */
                              else if (forward)
                                {
                                  if (!pAbChild->AbTruncatedTail)
                                    ok = FALSE;	/* pave fils complete */
                                }
                              else if (!pAbChild->AbTruncatedHead)
                                ok = FALSE;	/* pave fils complete */
                            }
                        }
                    if (ok)
                      /* on cree effectivement les paves du fils */
                      pAbChild = AbsBoxesCreate (pElChild, pDoc, viewNb, forward, descent, &completeChild);
                    else
                      /* le pave du fils etait deja complet */
                      {
                        pAbChild = NULL;
                        completeChild = TRUE;
                      }

                    if (pNewAbbox == NULL)
                      if (pAbChild != NULL)
                        pAbReturn = pAbChild;
                    if (forward)
                      {
                        pLast = (pElChild->ElNext == NULL);
                        if (ApplyRules)
                          pFirst = (pElChild->ElPrevious == NULL);
                        else
                          pFirst = FALSE;
                      }
                    else
                      {
                        pFirst = (pElChild->ElPrevious == NULL);
                        if (ApplyRules)
                          pLast = (pElChild->ElNext == NULL);
                        else
                          pLast = FALSE;
                      }
                    pAsc = pElChild->ElParent;
                    while (pAsc && pAsc != pEl)
                      {
                        if (pFirst)
                          pFirst = (pAsc->ElPrevious == NULL);
                        if (pLast)
                          pLast = (pAsc->ElNext == NULL);
                        pAsc = pAsc->ElParent;
                      }
		       
                    pElParent = pElChild->ElParent;
                    if (forward)
                      if (pElChild->ElNext)
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
                    if (pLast || pFirst)
                      /* on a cree' les paves du premier (ou dernier) fils */
                      /* de pEl, le pave englobant des fils de pEl est  */
                      /* complete si le pave du premier (ou dernier) fils est */
                      /* complete ou s'il ne contient rien (tous ses */
                      /* descendants ont une visibilite' nulle) */
                      {
                        stop = FALSE;
                        do
                          if (pElParent)
                            {
                              if (pElParent->ElAbstractBox[viewNb - 1])
                                stop = TRUE;
                              else if ((forward && pElParent->ElNext == NULL)
                                       || (!forward
                                           && pElParent->ElPrevious == NULL))
                                pElParent = pElParent->ElParent;
                              else
                                pElParent = NULL;
                            }
                        while (!stop && pElParent);
                        if (pElParent)
                          {
                            pAb = pElParent->ElAbstractBox[viewNb - 1];
                            /* saute les paves de presentation crees par */
                            /* FnCreateBefore */
                            stop = FALSE;
                            do
                              if (pAb == NULL)
                                stop = TRUE;
                              else if (!pAb->AbPresentationBox)
                                stop = TRUE;
                              else
                                pAb = pAb->AbNext;
                            while (!stop);
                            if (pAb != NULL)
                              {
                                pAbChild = pAb->AbFirstEnclosed;
                                pAbPres = NULL;
                                if (pLast)
                                  {
                                    if (pAbChild == NULL)
                                      /* tous les descendants ont une visibilite' */
                                      /* nulle; le pave est donc complete en queue */
                                      truncate = FALSE;
                                    else
                                      {
                                        /* cherche le dernier pave' fils */
                                        while (pAbChild->AbNext)
                                          pAbChild = pAbChild->AbNext;
                                        /* ignore les paves de presentation */
                                        while (pAbChild->AbPresentationBox &&
                                               pAbChild->AbPrevious != NULL)
                                          pAbChild = pAbChild->AbPrevious;
                                        if (pAbChild->AbDead && pAbChild->AbVisibility)
                                          truncate = TRUE;
                                        else
                                          if (forward)
                                            truncate = !completeChild;
                                          else
                                            truncate = pAbChild->AbTruncatedTail;
                                      }
                                    pAbPres = TruncateOrCompleteAbsBox (pAb, truncate, FALSE, pDoc);
                                    if (forward && !truncate)
                                      *complete = TRUE;
                                  }
                                if (pFirst)
                                  {
                                    pAbChild = pAb->AbFirstEnclosed;
                                    if (pAbChild == NULL)
                                      /* tous les descendants ont une visibilite' */
                                      /* nulle ; le pave est donc complete en tete */
                                      truncate = FALSE;
                                    else
                                      {
                                        /* ignore les paves de presentation */
                                        while (pAbChild->AbPresentationBox &&
                                               pAbChild->AbNext != NULL)
                                          pAbChild = pAbChild->AbNext;
                                        if (pAbChild->AbDead)
                                          truncate = TRUE;
                                        else
                                          if (!forward)
                                            truncate = !completeChild;
                                          else
                                            truncate = pAbChild->AbTruncatedHead;
                                      }
                                    pAbPres = TruncateOrCompleteAbsBox (pAb, truncate, TRUE, pDoc);
                                    if (!forward && !truncate)
                                      *complete = TRUE;
                                  }
                                if (pAbPres != NULL &&
                                    /* on a cree des paves de presentation */
                                    (!Creation || pAbPres->AbEnclosing != pAb))
                                  pAbReturn = pAbPres;
                              }
                          }
                      }
                  }
              /* fin: Cree les paves d'un fils et passe a un autre fils */
              /* met les valeurs par defaut des axes de reference si aucune */
              /* regle de positionnement des axes de reference n'a ete  */
              /* appliquee */
              if (Creation)
                if (pNewAbbox)
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
                GetAtt (&pRule, &pAb, &pSPres, &pAttr, &rulesPseudo, &queue,
                        &lqueue, &pqueue);
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
                        if (pRule->PrPresFunction == FnCreateBefore ||
                            pRule->PrPresFunction == FnCreateWith ||
                            pRule->PrPresFunction == FnCreateAfter ||
                            pRule->PrPresFunction == FnCreateEnclosing ||
                            pRule->PrPresFunction == FnCreateFirst ||
                            pRule->PrPresFunction == FnCreateLast ||
                            pRule->PrPresFunction == FnContent)
                          {
                            crAbsBox = TRUE;
                            if (pAttr != NULL)
                              pAbPres = CrAbsBoxesPres (pEl, pDoc, pRule,
                                                        pAttr->AeAttrSSchema, pAttr,
                                                        viewNb, pSPres, rulesPseudo, TRUE);
                            else
                              pAbPres = CrAbsBoxesPres (pEl, pDoc, pRule,
                                                        pEl->ElStructSchema, NULL,
                                                        viewNb, pSPres, rulesPseudo, TRUE);
                          }
                        switch (pRule->PrPresFunction)
                          {
                          case FnCreateBefore:
                            if (!forward && pAbPres != NULL &&
                                pEl->ElParent)
                              pAbReturn = pAbPres;
                            break;
                          case FnCreateAfter:
                          case FnCreateWith:
                            if (forward && pAbPres != NULL &&
                                pEl->ElParent)
                              pAbReturn = pAbPres;
                            break;
                          case FnCreateEnclosing:
                            pAbReturn = pAbPres;
                            break;
                          default:
                            break;
                          }
                      }
                    if (!crAbsBox &&
                        /* not a creation rule */
                        !ApplyRule (pRule, pSPres, pAb, pDoc, pAttr, pAb))
                      Delay (pRule, pSPres, pAb, pAttr);
                  }
              }
            while (pRule);
          /* applique toutes les regles en retard des descendants */
          if (descent)
            {
              pAb = pEl->ElAbstractBox[viewNb - 1];
              /* saute les paves de presentation crees par FnCreateBefore */
              stop = FALSE;
              do
                if (pAb == NULL)
                  stop = TRUE;
                else if (!pAb->AbPresentationBox)
                  stop = TRUE;
                else if (pAb->AbFirstEnclosed != NULL &&
                         pAb->AbFirstEnclosed->AbElement == pEl)
                  /* pave cree' par la regle FnCreateEnclosing */
                  stop = TRUE;
                else
                  pAb = pAb->AbNext;
              while (!stop);

              /* apply delayed rules */
              ApplyDelayedRules (-1, pAb, pDoc);
            }
        }
      /* fin de !ignoreDescent */
    }
  return pAbReturn;
}
