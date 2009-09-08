/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * presentmenu.c : Functions to modify the specific presentation
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Windows version
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "dialog.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "boxes_tv.h"
#include "page_tv.h"
#include "edit_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"
#include "frame_tv.h"

#ifdef _WINGUI 
#include "wininclude.h"
#endif /* _WINGUI */

#include "actions_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "config_f.h"
#include "createabsbox_f.h"
#include "documentapi_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "presentationapi_f.h"
#include "presrules_f.h"
#include "structcommands_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "units_f.h"
#include "unstructchange_f.h"
#include "presentmenu_f.h"

static PtrDocument  DocModPresent;
static ThotBool            ChngStandardColor;	/* standard presentation colors  */
static ThotBool            ChngStandardGeom;	/* standard geometry */
/* user presentation choices and their values */
static ThotBool     ChngFontFamily; /* user asks to modify the font family */
static ThotBool     ChngFontStyle;  /* user asks to modify the font style */
static ThotBool     ChngFontWeight; /* user asks to modify the font weight */
static ThotBool     ChngUnderline;  /* user asks to modify the underline */
static ThotBool     ChngUlWeight;/* user asks to modify the underline weight */
static ThotBool     ChngFontSize;   /* user asks to modify the body size */
static ThotBool     ChngAlign;	/* user asks to modify the alignment mode */
static ThotBool     ChngHyphen;	/* user asks to change the hyphenation */
static ThotBool     ChngIndent;	/* user asks to change the indentation */
static ThotBool     ChngLineSp;	/* user asks to change the line spacing */
static ThotBool     ChngLineStyle;  /* user asks to change the line style */
static ThotBool     ChngLineWeight; /* user asks to change the line weight */
static ThotBool     ChngTrame;	/* user asks to change the pattern */
#define Apply_All		0
#define Apply_FontFamily	1
#define Apply_FontStyle		2
#define Apply_FontWeight	3
#define Apply_Underline		4
#define Apply_UlWeight		5
#define Apply_FontSize		6
#define Apply_AllChars		7

#define Apply_Align		8

#define Apply_Hyphen		10
#define Apply_Indent		11
#define Apply_LineSp		12
#define Apply_AllFormat		13

#define Apply_LineStyle		14
#define Apply_LineWeight	15
#define Apply_Trame		16
#define Apply_AllGraphics	17

static ThotBool     StdFontFamily; /* user asks to reset the font family */
static ThotBool     StdFontStyle;  /* user asks to reset the font style */
static ThotBool     StdFontWeight; /* user asks to reset the font weight */
static ThotBool     StdUnderline;  /* user asks to reset the underline */
static ThotBool     StdUlWeight;/* user asks to reset the underline weight */
static ThotBool     StdFontSize;   /* user asks to reset the body size */
static ThotBool     StdAlign;	/* user asks to reset the alignment mode */
static ThotBool     StdHyphen;	/* user asks to reset the hyphenation */
static ThotBool     StdIndent;	/* user asks to reset the indentation */
static ThotBool     StdLineSp;	/* user asks to reset the line spacing */
static ThotBool     StdLineStyle;  /* user asks to reset the line style */
static ThotBool     StdLineWeight; /* user asks to reset the line weight */
static ThotBool     StdTrame;	/* user asks to reset the pattern */

static int          FontFamily;	/* font family requested by the user */
static int          FontStyle;	/* font style requested by the user */
static int          FontWeight;	/* font weight requested by the user */
static int          UnderlineStyle; /* underline style requested by the user */
static int          UnderlineWeight;/* underline weight requested by user */
static int          FontSize;	/* body size (in points) requested by user */
static int          Align;	/* line alignment mode */
static ThotBool     Hyphenate;	/* with or without hyphenation */
static int          IndentValue;/* value in points for the 1st line indent */
static int          IndentSign;	/* the indentation sign */
static char         LineStyle;	/* requested line style */
static int          LineWeight;	/* requested line weight in points */
static int          PaintWithPattern;	/* number of the requested trame */

static RuleSet      TheRules;
static RuleSet      GeomRules;
static int          OldLineSp;
static int          NormalLineSpacing;
//static void         ResetMenus ();

#ifdef _WINGUI 
extern int   WIN_IndentValue;
extern int   WIN_OldLineSp;
extern int   WIN_NormalLineSpacing;
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  GetEnclosingBlock
  return the current element or its ancestor if they have a Line
  presentation rule for the active view.
  ----------------------------------------------------------------------*/
static PtrElement GetEnclosingBlock (PtrElement pEl, PtrDocument pDoc)
{
  PtrElement	pBlock, el;
  PtrPRule	pPRule;
  PtrPSchema	pSPR;
  PtrSSchema	pSSR;
  PtrAttribute	pAttr;
  int		viewSch;

  pBlock = NULL;
  if (pEl->ElTerminal)
    el = pEl->ElParent;
  else
    el = pEl;
  viewSch = AppliedView (pEl, NULL, pDoc, SelectedView);
  while (pBlock == NULL && el)
    {
      pPRule = GlobalSearchRulepEl (el, pDoc, &pSPR, &pSSR, FALSE, 0, NULL,
                                    viewSch, PtFunction, FnLine, FALSE, TRUE, &pAttr);
      if (pPRule)
        pBlock = el;
      else if (el == pEl)
        {
          pPRule = GlobalSearchRulepEl (el, pDoc, &pSPR, &pSSR, FALSE, 0,
                                        NULL, viewSch, PtFloat, (FunctionType)0,
                                        FALSE, TRUE, &pAttr);
          if (pPRule)
            el = NULL;
          else
            /* check if the parent element of the current selection is a block */
            el = pEl->ElParent;
        }
      else
        el = NULL;
    }
  return pBlock;
}


/*----------------------------------------------------------------------
  ModifyGraphics applique a l'element pEl les modifications sur	
  les graphiques demandes par l'utilisateur.		
  ----------------------------------------------------------------------*/
static void ModifyGraphics (PtrElement pEl, PtrDocument pDoc,
                            int viewToApply, ThotBool modifLineStyle,
                            char LineStyle, ThotBool modifLineWeight,
                            int LineWeight, TypeUnit LineWeightUnit,
                            ThotBool modifFillPattern, int FillPattern,
                            ThotBool modifColorBackground,
                            int ColorBackground,
                            ThotBool modifLineColor, int LineColor)
{
  PtrPRule            pPRule, pFunctRule;
  int                 viewSch, value;
  ThotBool            isNew;

  /* view number */
  viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);
  /* graphics line style */
  if (modifLineStyle)
    {
      /* look for a specific presentation rule 'LineStyle' */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtLineStyle, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* get user choices */
      pPRule->PrType = PtLineStyle;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrChrValue = LineStyle;
      if (!PRuleMessagePre (pEl, pPRule, LineStyle, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }

  /* graphics line weight */
  if (modifLineWeight)
    {
      /* cherche la regle de presentation specifique 'Epaisseur Trait' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtLineWeight, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtLineWeight;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrMinAttr = FALSE;
      pPRule->PrMinValue = LineWeight;
      pPRule->PrMinUnit = LineWeightUnit;
      if (!PRuleMessagePre (pEl, pPRule, LineWeight, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }

  /* couleur de fond */
  if (modifColorBackground)
    {
      /* cherche la regle de presentation specifique 'Background' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtBackground, (FunctionType)0, &isNew, pDoc, viewToApply);
      pPRule->PrType = PtBackground;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrValueType = PrNumValue;
      pPRule->PrIntValue = ColorBackground;
      if (!PRuleMessagePre (pEl, pPRule, ColorBackground, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          /* met les choix de l'utilisateur dans cette regle */
          SetDocumentModified (pDoc, TRUE, 0);
	  
          /* It the element is not a leaf in the abstract tree, create a
             ShowBox rule for the element if there is none */
          if (!pEl->ElTerminal &&
              !TypeHasException (ExcNoShowBox, pEl->ElTypeNumber, pEl->ElStructSchema))
            {
              pFunctRule = SearchPresRule (pEl, PtFunction, (FunctionType)FnShowBox, &isNew,
                                           pDoc, viewToApply);
              pFunctRule->PrType = PtFunction;
              pFunctRule->PrViewNum = viewSch;
              pFunctRule->PrPresMode = PresFunction;
              pFunctRule->PrPresFunction = FnShowBox;
              pFunctRule->PrPresBoxRepeat = FALSE;
              pFunctRule->PrNPresBoxes = 0;
              if (isNew &&
                  !PRuleMessagePre (pEl, pFunctRule,
                                    TtaGetPRuleValue ((PRule) pFunctRule), pDoc, isNew))
                {
                  ApplyNewRule (pDoc, pFunctRule, pEl);
                  PRuleMessagePost (pEl, pFunctRule, pDoc, isNew);
                }
            }
        }
    }

  /* graphics fill pattern */
  if (modifFillPattern)
    {
      /* cherche la regle de presentation specifique 'FillPattern' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtFillPattern, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtFillPattern;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrValueType = PrNumValue;
      value = pPRule->PrIntValue;
      pPRule->PrIntValue = FillPattern;
      pPRule->PrIntValue = FillPattern;
      if (!PRuleMessagePre (pEl, pPRule, FillPattern, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
	  
          /* It the element is not a leaf in the abstract tree, create a
             ShowBox rule for the element if there is none */
          if (!pEl->ElTerminal &&
              !TypeHasException (ExcNoShowBox, pEl->ElTypeNumber, pEl->ElStructSchema))
            {
              pFunctRule = SearchPresRule (pEl, PtFunction, (FunctionType)FnShowBox, &isNew,
                                           pDoc, viewToApply);
              pFunctRule->PrType = PtFunction;
              pFunctRule->PrViewNum = viewSch;
              pFunctRule->PrPresMode = PresFunction;
              pFunctRule->PrPresFunction = FnShowBox;
              pFunctRule->PrPresBoxRepeat = FALSE;
              pFunctRule->PrNPresBoxes = 0;
              if (isNew)
                if (!PRuleMessagePre (pEl, pFunctRule, TtaGetPRuleValue ((PRule) pFunctRule), pDoc, isNew))
                  {
                    ApplyNewRule (pDoc, pFunctRule, pEl);
                    PRuleMessagePost (pEl, pFunctRule, pDoc, isNew);
                  }
            }
        }
    }

  /* couleur du trace' */
  if (modifLineColor)
    {
      /* cherche la regle de presentation specifique 'CouleurTrace' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtForeground, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtForeground;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrValueType = PrNumValue;
      pPRule->PrIntValue = LineColor;
      if (!PRuleMessagePre (pEl, pPRule, LineColor, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }
}

/*----------------------------------------------------------------------
  ModifyColor change la presentation specifique de la couleur	
  de fond ou de trace' (selon Background) pour tous les elements de la	
  selection courante.						
  ----------------------------------------------------------------------*/
void ModifyColor (int colorNum, ThotBool Background)
{
  PtrDocument         pSelDoc;
  PtrElement          pElFirstSel, pElLastSel, oldFirstSel, oldLastSel, pEl;
  PtrAbstractBox      pAb;
  int                 firstChar, lastChar, oldFirstChar, oldLastChar;
  ThotBool            selok, modifFillPattern;
  int                 fillPatternNum;
  RuleSet             rulesS;

  CloseTextInsertion ();
  /* demande quelle est la selection courante */
  selok = GetCurrentSelection (&pSelDoc, &pElFirstSel, &pElLastSel, 
                               &firstChar, &lastChar);
  if (selok && pSelDoc && !pSelDoc->DocReadOnly && pSelDoc->DocSSchema != NULL)
    /* le document selectionne' n'a pas ete ferme' */
    {
      /* eteint la selection courante */
      TtaClearViewSelections ();

      /* si une chaine de caracteres complete est selectionnee, */
      /* selectionne l'element TEXTE */
      if (pElFirstSel->ElTerminal && pElFirstSel->ElLeafType == LtText &&
          firstChar <= 1)
        if (pElLastSel != pElFirstSel ||
            (pElFirstSel == pElLastSel &&
             lastChar > pElFirstSel->ElTextLength))
          firstChar = 0;
      if (pElLastSel->ElTerminal && pElLastSel->ElLeafType == LtText &&
          lastChar > pElLastSel->ElTextLength)
        if (pElLastSel != pElFirstSel ||
            (pElFirstSel == pElLastSel && firstChar == 0))
          lastChar = 0;

      /* Coupe les elements du debut et de la fin de la selection */
      /* s'ils sont partiellement selectionnes */
      if (firstChar > 1 || lastChar > 0)
        IsolateSelection (pSelDoc, &pElFirstSel, &pElLastSel, &firstChar,
                          &lastChar, TRUE);
      /* save the current selection */
      oldFirstSel = pElFirstSel;
      oldLastSel = pElLastSel;
      oldFirstChar = firstChar;
      oldLastChar = lastChar;
      if (colorNum < 0)
        /* standard color */
        {
          /* set selection to the highest level elements having the same
             content */
          SelectSiblings (&pElFirstSel, &pElLastSel, &firstChar, &lastChar);
          if (firstChar == 0 && lastChar == 0)
            if (pElFirstSel->ElPrevious == NULL &&
                pElLastSel->ElNext == NULL)
              if (pElFirstSel->ElParent != NULL &&
                  pElFirstSel->ElParent == pElLastSel->ElParent)
                {
                  pElFirstSel = pElFirstSel->ElParent;
                  while (pElFirstSel->ElPrevious == NULL &&
                         pElFirstSel->ElNext == NULL &&
                         pElFirstSel->ElParent != NULL)
                    pElFirstSel = pElFirstSel->ElParent;
                  pElLastSel = pElFirstSel;
                }
        }

      OpenHistorySequence (pSelDoc, pElFirstSel, pElLastSel, NULL, firstChar, lastChar);
      /* parcourt les elements selectionnes */
      pEl = pElFirstSel;
      while (pEl != NULL)
        {
          /* on saute les elements qui sont des copies */
          if (!pEl->ElIsCopy &&
              /* on saute les elements non modifiables */
              !ElementIsReadOnly (pEl) &&
              /* on saute les marques de page */
              (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak))
            {
              modifFillPattern = FALSE;
              fillPatternNum = 0;
              if (Background)
                /* on change la couleur de fond avec la souris */
                {
                  pAb = AbsBoxOfEl (pEl, SelectedView);
                  if (pAb != NULL && pAb->AbFillPattern < 2)
                    /* on force la trame backgroundcolor si la trame du */
                    /* pave est nopattern ou foregroundcolor */
                    {
                      modifFillPattern = TRUE;
                      fillPatternNum = 2;
                    }
                }
              if (colorNum == -1)
                {
                  /* Couleur standard */
                  RuleSetClr (rulesS);
                  if (Background)
                    {
                      RuleSetPut (rulesS, PtFillPattern);
                      RuleSetPut (rulesS, PtBackground);
                      RuleSetPut (rulesS, PtFunction);
                    }
                  else
                    RuleSetPut (rulesS, PtForeground);
                  RemoveSpecPresTree (pEl, pSelDoc, rulesS, SelectedView);
                }
              else
                ModifyGraphics (pEl, pSelDoc, SelectedView, FALSE,
                                SPACE, FALSE, 0, (TypeUnit)FALSE,
                                modifFillPattern, fillPatternNum,
                                (ThotBool)Background, colorNum, (ThotBool)(!Background),
                                colorNum);
              /* si on est dans un element copie' par inclusion,   */
              /* on met a jour les copies de cet element. */
              RedisplayCopies (pEl, pSelDoc, TRUE);
            }
          /* cherche l'element a traiter ensuite */
          pEl = NextInSelection (pEl, pElLastSel);
        }
      CloseHistorySequence (pSelDoc);
      /* try to collapse text elements except in TextFile
         where the presentation generates strings */
      if (pSelDoc && pSelDoc->DocSSchema &&
          strcmp (pSelDoc->DocSSchema->SsName, "TextFile"))
        SelectRange (pSelDoc, oldFirstSel, oldLastSel, oldFirstChar,
                     oldLastChar);
    }
}


/*----------------------------------------------------------------------
  ModifyChar applique a l'element pEl les modifications sur	
  les caracteres demandes par l'utilisateur.		
  ----------------------------------------------------------------------*/
static void  ModifyChar (PtrElement pEl, PtrDocument pDoc, int viewToApply,
                         ThotBool modifFamily, int family,
                         ThotBool modifStyle, int charStyle,
                         ThotBool modifWeight, int charWeight,
                         ThotBool modifsize, int size,
                         ThotBool modifUnderline, int underline,
                         ThotBool modifUlWeight, int weightUnderline)
{
  ThotBool            isNew;
  PtrPRule            pPRule;
  int                 viewSch;
  char                value;
  int                 intvalue;

  /* numero de cette view */
  viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);
  /* applique les choix de l'utilisateur */
  /* family de polices de caracteres */
  if (modifFamily)
    {
      /* cherche la regle de presentation specifique 'Fonte' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtFont, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtFont;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      switch (family)
        {
        case 1:
          value = 'T';	/* Times */
          intvalue = FontTimes;
          break;
        case 2:
          value = 'H';	/* Helvetica */
          intvalue = FontHelvetica;
          break;
        case 3:
          value = 'C';	/* Courier */
          intvalue = FontCourier;
          break;
        default:
          value = 'T';
          intvalue = FontTimes;
          break;
        }
      pPRule->PrChrValue = value;
      if (!PRuleMessagePre (pEl, pPRule, intvalue, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }
  /* Style de caracteres */
  if (modifStyle)
    {
      /* cherche la regle de presentation specifique 'Style' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtStyle, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtStyle;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      value = pPRule->PrChrValue;
      switch (charStyle)
        {
        case 0:
          value = 'R';	/* roman */
          intvalue = StyleRoman;	    
          break;
        case 1:
          value = 'I';	/* italic */
          intvalue = StyleItalics;
          break;
        case 2:
          value = 'O';	/* oblique */
          intvalue = StyleOblique;
          break;
        default:
          value = 'R';
          intvalue = StyleRoman;	    
          break;
        }
      pPRule->PrChrValue = value;
      if (!PRuleMessagePre (pEl, pPRule, intvalue, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }
  /* Graisse des caracteres */
  if (modifWeight)
    {
      /* cherche la regle de presentation specifique 'Weight' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtWeight, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtWeight;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      switch (charWeight)
        {
        case 0:
          value = 'N';	/* normal */
          intvalue = WeightNormal;
          break;
        case 1:
          value = 'B';	/* bold */
          intvalue = WeightBold;
          break;
        default:
          value = 'N';
          intvalue = WeightNormal;
          break;
        }
      pPRule->PrChrValue = value;
      if (!PRuleMessagePre (pEl, pPRule, intvalue, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }
  /* Taille des caracteres */
  if (modifsize)
    {
      /* cherche la regle de presentation specifique 'Corps' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtSize, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtSize;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrMinUnit = UnPoint;
      pPRule->PrMinAttr = FALSE;
      pPRule->PrMinValue = size;
      if (!PRuleMessagePre (pEl, pPRule, size, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }

  /* Souligne' */
  if (modifUnderline)
    {
      /* cherche la regle de presentation specifique 'Souligne' de l'element*/
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtUnderline, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtUnderline;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      switch (underline)
        {
        case 0:
          value = 'N';	/* sans souligne */
          intvalue = NoUnderline;
          break;
        case 1:
          value = 'U';	/* souligne continu */
          intvalue = Underline;
          break;
        case 2:
          value = 'O';	/* surligne */
          intvalue = Overline;
          break;
        case 3:
          value = 'C';	/* biffer */
          intvalue = CrossOut;
          break;
        default:
          value = 'N';
          intvalue = NoUnderline;
          break;
        }
      pPRule->PrChrValue = value;
      if (!PRuleMessagePre (pEl, pPRule, intvalue, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }
  /* Epaisseur du souligne */
  if (modifUlWeight)
    {
      /* cherche la regle de presentation specifique weightUnderline de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtThickness, (FunctionType)0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtThickness;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      switch (weightUnderline)
        {
        case 0:
          value = 'N';	/* souligne mince */
          intvalue = ThinUnderline;
          break;
        case 1:
          value = 'T';	/* souligne epais */
          intvalue = ThickUnderline;
          break;
        default:
          value = 'N';
          intvalue = ThinUnderline;
          break;
        }
      pPRule->PrChrValue = value;
      if (!PRuleMessagePre (pEl, pPRule, intvalue, pDoc, isNew))
        {
          /* si le pave existe, applique la nouvelle regle au pave */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }
}

/*----------------------------------------------------------------------
  ModifyLining applies line modifications to the pEl element.
  ----------------------------------------------------------------------*/
static void ModifyLining (PtrElement pEl, PtrDocument pDoc,
                          int viewToApply, ThotBool modifAdjust,
                          int Adjust, ThotBool modifIndent, int ValIndent,
                          ThotBool modifLineSpacing, int LineSpacing,
                          ThotBool modifHyphen, ThotBool Hyphenate)
{
  PtrElement          pParent, pGParent;
  PtrPRule            pPRule, pWRule;
  PtrPSchema	      pSPR;
  PtrSSchema	      pSSR;
  PtrAttribute	      pAttr;
  int                 value;
  int                 viewSch;
  ThotBool            isNew;

  viewSch = AppliedView (pEl, NULL, pDoc, viewToApply); /* view type*/
  /* apply changes */
  if (modifAdjust && Adjust > 0)
    {
      /* look for the Width presentation rule applied to the element */
      pWRule = GlobalSearchRulepEl (pEl, pDoc, &pSPR, &pSSR, FALSE, 0, NULL,
                                    viewSch, PtWidth, (FunctionType)0, FALSE, TRUE, &pAttr);
      pParent = pEl->ElParent;
      if (pParent)
        pGParent = pParent->ElParent;
      else
        pGParent = NULL;
      /* look for the Line presentation rule */
      pPRule = GlobalSearchRulepEl (pEl, pDoc, &pSPR, &pSSR, FALSE, 0, NULL,
                                    viewSch, PtFunction, FnLine, FALSE, TRUE, &pAttr);
      if (pPRule || pEl->ElTerminal ||
          /* the element width depends on the enclosing box */
          (pWRule && pWRule->PrDimRule.DrRelation == RlEnclosing &&
           !TypeHasException (ExcIsTable, pEl->ElTypeNumber, pEl->ElStructSchema)) ||
          /* align within a cell, a row or a table body */
          TypeHasException (ExcIsCell, pEl->ElTypeNumber, pEl->ElStructSchema) ||
          TypeHasException (ExcIsRow, pEl->ElTypeNumber, pEl->ElStructSchema) ||
          /* is it a table body element */
          (pParent &&
           TypeHasException (ExcIsTable, pParent->ElTypeNumber, pParent->ElStructSchema)) ||
          (pGParent &&
           TypeHasException (ExcIsTable, pGParent->ElTypeNumber, pGParent->ElStructSchema)))
        {
          pPRule = SearchPresRule (pEl, PtAdjust, (FunctionType)0, &isNew,
                                   pDoc, viewToApply);
          pPRule->PrType = PtAdjust;
          pPRule->PrViewNum = viewSch;
          /* this rule will be translated into style attribute for the element */
          pPRule->PrSpecificity = 100;
          pPRule->PrPresMode = PresImmediate;
          switch (Adjust)
            {
            case 2:
              value = AlignRight;
              break;
            case 3:
              value = AlignCenter;
              break;
            case 4:
              value = AlignJustify;
              break;
            case 5:
              value = AlignLeftDots;
              break;
            default:
              value = AlignLeft;
              break;
            }
          pPRule->PrAdjust = (BAlignment)value;
          if (!PRuleMessagePre (pEl, pPRule, Adjust, pDoc, isNew))
            {
              ApplyNewRule (pDoc, pPRule, pEl);
              PRuleMessagePost (pEl, pPRule, pDoc, isNew);
              SetDocumentModified (pDoc, TRUE, 0);
            }
        }
      else
        {
          /* compound element without lines */
          pPRule = SearchPresRule (pEl, PtMarginLeft, (FunctionType)0, &isNew,
                                   pDoc, viewToApply);
          pPRule->PrType = PtMarginLeft;
          pPRule->PrViewNum = viewSch;
          /* this rule will be translated into style attribute for the element */
          pPRule->PrSpecificity = 100;
          pPRule->PrPresMode = PresImmediate;
          pPRule->PrMinValue = 0;
          value = 0;
          switch (Adjust)
            {
            case 2: /* right */
            case 3: /* center */
            case 4: /* justify */
              pPRule->PrMinUnit = UnAuto;
              break;
            default: /* left */
              pPRule->PrMinUnit = UnPixel;
              break;
            }
          if (!PRuleMessagePre (pEl, pPRule, Adjust, pDoc, isNew))
            {
              ApplyNewRule (pDoc, pPRule, pEl);
              PRuleMessagePost (pEl, pPRule, pDoc, isNew);
              SetDocumentModified (pDoc, TRUE, 0);
            }
          pPRule = SearchPresRule (pEl, PtMarginRight, (FunctionType)0, &isNew,
                                   pDoc, viewToApply);
          pPRule->PrType = PtMarginRight;
          pPRule->PrViewNum = viewSch;
          /* this rule will be translated into style attribute for the element */
          pPRule->PrSpecificity = 100;
          pPRule->PrPresMode = PresImmediate;
          switch (Adjust)
            {
            case 2: /* right */
              pPRule->PrMinUnit = UnPixel;
              break;
            default: /* left *//* center *//* justify */
              pPRule->PrMinUnit = UnAuto;
              break;
            }
          if (!PRuleMessagePre (pEl, pPRule, Adjust, pDoc, isNew))
            {
              ApplyNewRule (pDoc, pPRule, pEl);
              PRuleMessagePost (pEl, pPRule, pDoc, isNew);
              SetDocumentModified (pDoc, TRUE, 0);
            }	  
        }
    }

  if (modifHyphen)
    {
      pPRule = SearchPresRule (pEl, PtHyphenate, (FunctionType)0, &isNew,
                               pDoc, viewToApply);
      pPRule->PrType = PtHyphenate;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrBoolValue = Hyphenate;
      if (!PRuleMessagePre (pEl, pPRule, Hyphenate, pDoc, isNew))
        {
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }

  if (modifIndent)
    {
      pPRule = SearchPresRule (pEl, PtIndent, (FunctionType)0, &isNew,
                               pDoc, viewToApply);
      pPRule->PrType = PtIndent;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrMinUnit = UnPoint;
      pPRule->PrMinAttr = FALSE;
      pPRule->PrMinValue = ValIndent;
      if (!PRuleMessagePre (pEl, pPRule, ValIndent, pDoc, isNew))
        {
          /* le document est modifie' */
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }

  if (modifLineSpacing)
    {
      pPRule = SearchPresRule (pEl, PtLineSpacing, (FunctionType)0, &isNew,
                               pDoc, viewToApply);
      pPRule->PrType = PtLineSpacing;
      pPRule->PrViewNum = viewSch;
      /* this rule will be translated into style attribute for the element */
      pPRule->PrSpecificity = 100;
      pPRule->PrPresMode = PresImmediate;
      pPRule->PrMinUnit = UnPoint;
      pPRule->PrMinAttr = FALSE;
      pPRule->PrMinValue = LineSpacing;
      if (!PRuleMessagePre (pEl, pPRule, LineSpacing, pDoc, isNew))
        {
          ApplyNewRule (pDoc, pPRule, pEl);
          PRuleMessagePost (pEl, pPRule, pDoc, isNew);
          SetDocumentModified (pDoc, TRUE, 0);
        }
    }
}

/*----------------------------------------------------------------------
  ApplyPresentMod
  applies the presentation modifications that were requested by means
  of the Characters form, the Format form, the Graphics form, or the
  Standard Geometry entry of the Present menu.
  ----------------------------------------------------------------------*/
static void         ApplyPresentMod (int applyDomain)
{
  PtrElement          pEl, pFirstSel, pLastSel, pElem, pBlock, pPrevBlock,
    pElNext;
  PtrDocument         pSelDoc;
  PtrAbstractBox      pAb;
  TypeUnit            LocLineWeightUnit;
  int                 firstChar, lastChar;
  int                 currentFontSize;
  int                 i;
  int                 sign;
  ThotBool            selectionOK;
  ThotBool            chngChars;
  ThotBool            chngFormat;
  ThotBool            chngGraphics;
  ThotBool            locChngFontFamily;
  ThotBool            locChngFontStyle;
  ThotBool            locChngFontWeight;
  ThotBool            locChngFontSize;
  ThotBool            locChngUnderline;
  ThotBool            locChngUlWeight;
  ThotBool            locChngLineStyle;
  ThotBool            locChngLineWeight;
  ThotBool            locChngTrame;
  ThotBool            locChngAlign;
  ThotBool            locChngHyphen;
  ThotBool            locChngIndent;
  ThotBool            locChngLineSp;
  ThotBool	      addPresRule;

  selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
  pBlock = NULL;
  LocLineWeightUnit = UnPoint;
  if (selectionOK && pSelDoc != NULL)
    if (pSelDoc->DocSSchema != NULL)
      /* il y a bien une selection et le document selectionne' n'a pas */
      /* ete ferme' */
      {
        /* eteint la selection courante */
        TtaClearViewSelections ();

        /* si une chaine de caracteres complete est selectionnee, */
        /* selectionne l'element TEXTE */
        if (pFirstSel->ElTerminal && pFirstSel->ElLeafType == LtText &&
            firstChar <= 1)
          if (pLastSel != pFirstSel ||
              (pFirstSel == pLastSel && lastChar > pFirstSel->ElTextLength))
            firstChar = 0;
        if (pLastSel->ElTerminal && pLastSel->ElLeafType == LtText &&
            lastChar > pLastSel->ElTextLength)
          if (pLastSel != pFirstSel || (pFirstSel == pLastSel && firstChar == 0))
            lastChar = 0;

        addPresRule = FALSE;
        /* Set chngChars indicator */
        locChngFontSize = ((StdFontSize || ChngFontSize)
                           && (applyDomain == Apply_FontSize
                               || applyDomain == Apply_AllChars
                               || applyDomain == Apply_All));
        addPresRule = addPresRule || (ChngFontSize
                                      && (applyDomain == Apply_FontSize
                                          || applyDomain == Apply_AllChars
                                          || applyDomain == Apply_All));
        locChngUlWeight = ((StdUlWeight || ChngUlWeight)
                           && (applyDomain == Apply_UlWeight
                               || applyDomain == Apply_AllChars
                               || applyDomain == Apply_All));
        addPresRule = addPresRule || (ChngUlWeight
                                      && (applyDomain == Apply_UlWeight
                                          || applyDomain == Apply_AllChars
                                          || applyDomain == Apply_All));
        locChngUnderline = ((StdUnderline || ChngUnderline)
                            && (applyDomain == Apply_Underline
                                || applyDomain == Apply_AllChars
                                || applyDomain == Apply_All));
        addPresRule = addPresRule || (ChngUnderline
                                      && (applyDomain == Apply_Underline
                                          || applyDomain == Apply_AllChars
                                          || applyDomain == Apply_All));
        locChngFontStyle = ((StdFontStyle || ChngFontStyle)
                            && (applyDomain == Apply_FontStyle
                                || applyDomain == Apply_AllChars
                                || applyDomain == Apply_All));
        addPresRule = addPresRule || (ChngFontStyle
                                      && (applyDomain == Apply_FontStyle
                                          || applyDomain == Apply_AllChars
                                          || applyDomain == Apply_All));

        locChngFontWeight = ((StdFontWeight || ChngFontWeight)
                             && (applyDomain == Apply_FontWeight
                                 || applyDomain == Apply_AllChars
                                 || applyDomain == Apply_All));
        addPresRule = addPresRule || (ChngFontWeight
                                      && (applyDomain == Apply_FontWeight
                                          || applyDomain == Apply_AllChars
                                          || applyDomain == Apply_All));

        locChngFontFamily = ((StdFontFamily || ChngFontFamily)
                             && (applyDomain == Apply_FontFamily
                                 || applyDomain == Apply_AllChars
                                 || applyDomain == Apply_All));
        addPresRule = addPresRule || (ChngFontFamily
                                      && (applyDomain == Apply_FontFamily
                                          || applyDomain == Apply_AllChars
                                          || applyDomain == Apply_All));
        chngChars = (locChngFontSize || locChngUlWeight || locChngUnderline ||
                     locChngFontStyle || locChngFontWeight || locChngFontFamily);

        /* Set chngFormat indicator */
        locChngAlign = ((StdAlign || ChngAlign)
                        && (applyDomain == Apply_Align
                            || applyDomain == Apply_AllFormat
                            || applyDomain == Apply_All));
        locChngHyphen = ((StdHyphen || ChngHyphen)
                         && (applyDomain == Apply_Hyphen
                             || applyDomain == Apply_AllFormat
                             || applyDomain == Apply_All));
        locChngIndent = ((StdIndent || ChngIndent)
                         && (applyDomain == Apply_Indent
                             || applyDomain == Apply_AllFormat
                             || applyDomain == Apply_All));
        locChngLineSp = ((StdLineSp || ChngLineSp)
                         && (applyDomain == Apply_LineSp
                             || applyDomain == Apply_AllFormat
                             || applyDomain == Apply_All));
        chngFormat = (locChngAlign || locChngHyphen ||
                      locChngIndent || locChngLineSp);

        /* Set chngGraphics indicator */
        locChngLineStyle = ((StdLineStyle || ChngLineStyle)
                            && (applyDomain == Apply_LineStyle
                                || applyDomain == Apply_AllGraphics
                                || applyDomain == Apply_All));
        addPresRule = addPresRule || (ChngLineStyle
                                      && (applyDomain == Apply_LineStyle
                                          || applyDomain == Apply_AllGraphics
                                          || applyDomain == Apply_All));
        locChngLineWeight = ((StdLineWeight || ChngLineWeight)
                             && (applyDomain == Apply_LineWeight
                                 || applyDomain == Apply_AllGraphics
                                 || applyDomain == Apply_All));
        addPresRule = addPresRule || (ChngLineWeight
                                      && (applyDomain == Apply_LineWeight
                                          || applyDomain == Apply_AllGraphics
                                          || applyDomain == Apply_All));
        locChngTrame = ((StdTrame || ChngTrame)
                        && (applyDomain == Apply_Trame
                            || applyDomain == Apply_AllGraphics
                            || applyDomain == Apply_All));
        addPresRule = addPresRule || (ChngTrame
                                      && (applyDomain == Apply_Trame
                                          || applyDomain == Apply_AllGraphics
                                          || applyDomain == Apply_All));
        chngGraphics = (locChngLineStyle || locChngLineWeight || locChngTrame);

        if (ChngStandardColor || chngChars || chngGraphics || locChngHyphen)
          /* changement des caracteres */
          /* coupe les elements du debut et de la fin de la selection */
          /* s'ils sont partiellement selectionnes */
          if (firstChar > 1 || lastChar > 0)
            IsolateSelection (pSelDoc, &pFirstSel, &pLastSel, &firstChar,
                              &lastChar, TRUE);
        if (!addPresRule)
          /* only changes to standard presentation */
          SelectSiblings (&pFirstSel, &pLastSel, &firstChar, &lastChar);
	
        /* evalue les difference entre le pave traite' et les demandes
           de l'utilisateur */
        pAb = AbsBoxOfEl (pFirstSel, SelectedView);
        if (pAb != NULL)
          {
            currentFontSize = PixelToPoint(PixelValue (pAb->AbSize,
                                                       pAb->AbSizeUnit,
                                                       pAb,
                                                       ViewFrameTable[ActiveFrame - 1].FrMagnification));
	    
            /* famille de polices de caracteres */
            if (locChngFontFamily)
              {
                if (StdFontFamily)
                  {
                    RuleSetPut (TheRules, PtFont);
                    locChngFontFamily = FALSE;
                  }
                else
                  locChngFontFamily = (FontFamily != pAb->AbFont);
              }
            /* style des caracteres */
            if (locChngFontStyle)
              {
                if (StdFontStyle)
                  {
                    RuleSetPut (TheRules, PtStyle);
                    locChngFontStyle = FALSE;
                  }
                else
                  locChngFontStyle = (FontStyle != pAb->AbFontStyle);
              }
            /* graisse des caracteres */
            if (locChngFontWeight)
              {
                if (StdFontWeight)
                  {
                    RuleSetPut (TheRules, PtWeight);
                    locChngFontWeight = FALSE;
                  }
                else
                  locChngFontWeight = (FontWeight != pAb->AbFontWeight);
              }

            /* style du souligne */
            if (locChngUnderline)
              {
                if (StdUnderline)
                  {
                    RuleSetPut (TheRules, PtUnderline);
                    locChngUnderline = FALSE;
                  }
                else
                  locChngUnderline = (UnderlineStyle != pAb->AbUnderline);
              }

            /* epaisseur du souligne */
            if (locChngUlWeight)
              {
                if (StdUlWeight)
                  {
                    RuleSetPut (TheRules, PtThickness);
                    StdUlWeight = FALSE;
                  }
                else
                  locChngUlWeight = (UnderlineWeight != pAb->AbThickness);
              }

            /* corps en points typo */
            if (locChngFontSize)
              {
                if (StdFontSize)
                  {
                    RuleSetPut (TheRules, PtSize);
                    locChngFontSize = FALSE;
                  }
                else
                  locChngFontSize = (FontSize != currentFontSize);
              }

            /* alignement des lignes */
            if (locChngAlign)
              {
                if (StdAlign)
                  {
                    RuleSetPut (TheRules, PtAdjust);
                    locChngAlign = FALSE;
                  }
                else
                  {
                    switch (pAb->AbAdjust)
                      {
                      case AlignLeft:
                        i = 1;
                        break;
                      case AlignRight:
                        i = 2;
                        break;
                      case AlignCenter:
                        i = 3;
                        break;
                      case AlignJustify:
                        i = 4;
                        break;
                      case AlignLeftDots:
                        i = 5;
                        break;
                      default:
                        i = 1;
                        break;
                      }
                    locChngAlign = (i != Align);
                  }
              }

            /* coupure des mots */
            if (locChngHyphen)
              {
                if (StdHyphen)
                  {
                    RuleSetPut (TheRules, PtHyphenate);
                    locChngHyphen = FALSE;
                  }
                else
                  locChngHyphen = (Hyphenate != pAb->AbHyphenate);
              }

            /* renfoncement de la premiere ligne */
            if (locChngIndent)
              {
                if (StdIndent)
                  {
                    RuleSetPut (TheRules, PtIndent);
                    locChngIndent = FALSE;
                  }
                else
                  {
                    if (pAb->AbIndent > 0)
                      sign = 1;
                    else if (pAb->AbIndent == 0)
                      sign = 0;
                    else
                      sign = -1;
                    i = PixelToPoint(PixelValue (abs (pAb->AbIndent), pAb->AbIndentUnit,
                                                 pAb, ViewFrameTable[ActiveFrame - 1].FrMagnification));
                    if (sign == IndentSign && i == IndentValue)
                      /* pas de changement */
                      locChngIndent = FALSE;
                    else
                      {
                        if (IndentSign != 0 && IndentValue == 0)
                          IndentValue = 15;
#ifdef _GTK
                        TtaSetNumberForm (NumZoneRecess, IndentValue);
#endif /* _GTK */
                      }
                  }
              }
	    
            /* interligne */
            if (locChngLineSp)
              {
                if (StdLineSp)
                  {
                    RuleSetPut (TheRules, PtLineSpacing);
                    locChngLineSp = FALSE;
                  }
                else
                  {
                    /* convertit 'interligne en points typographiques */
                    i = PixelToPoint(PixelValue (pAb->AbLineSpacing, pAb->AbLineSpacingUnit,
                                                 pAb, ViewFrameTable[ActiveFrame - 1].FrMagnification));
                    if (OldLineSp == i)
                      locChngLineSp = FALSE;
                  }
              }

            /* style des traits graphiques */
            if (locChngLineStyle)
              {
                if (StdLineStyle)
                  {
                    RuleSetPut (TheRules, PtLineStyle);
                    locChngLineStyle = FALSE;
                  }
                else
                  locChngLineStyle = (LineStyle != pAb->AbLineStyle);
              }

            /* epaisseur des traits graphiques */
            LocLineWeightUnit = pAb->AbLineWeightUnit;
            if (locChngLineWeight)
              {
                if (StdLineWeight)
                  {
                    RuleSetPut (TheRules, PtLineWeight);
                    locChngLineWeight = FALSE;
                  }
                else
                  {
                    if (pAb->AbLineWeightUnit == UnPoint)
                      i = pAb->AbLineWeight;
                    else
                      {
                        i = (currentFontSize * pAb->AbLineWeight) / 10;
                        if ((currentFontSize * i) % 10 >= 5)
                          i++;
                      }
                    if (LineWeight != i)
                      LocLineWeightUnit = UnPoint;
                  }
              }
	    
            /* trame de remplissage */
            if (locChngTrame)
              {
                if (StdTrame)
                  {
                    RuleSetPut (TheRules, PtFillPattern);
                    locChngTrame = FALSE;
                  }
                else
                  locChngTrame = (PaintWithPattern != pAb->AbFillPattern);
              }
          }

        if (chngChars || chngFormat || chngGraphics ||
            ChngStandardColor || ChngStandardGeom)
          /* il y a quelque chose a changer, on parcourt la selection */
          /* courante et on change ce qu'a demande' l'utilisateur */
          {
            OpenHistorySequence (pSelDoc, pFirstSel, pLastSel, NULL, firstChar, lastChar);
            pEl = pFirstSel;
            pPrevBlock = NULL;
            while (pEl != NULL)
              /* Traite l'element courant */
              {
                /* cherche l'element a traiter ensuite */
                pElNext = NextInSelection (pEl, pLastSel);

                pElem = pEl;
                if (chngFormat)
                  /* Format properties apply to block elements only. If the
                     selected element is not a block, apply the properties to
                     the first enclosing block (i.e. the first ancestor with a
                     Line presentation rule for the selected view).  If there
                     is no enclosing block, but the selected element is a
                     compound element, apply the properties to this selected
                     element, otherwise, don't apply format properties */
                  {
                    if (pSelDoc && pSelDoc->DocSSchema &&
                        !strcmp (pSelDoc->DocSSchema->SsName, "TextFile"))
                      {
                        pBlock = NULL;
                      }
                    else if (pPrevBlock && ElemIsWithinSubtree (pEl, pPrevBlock))
                      pBlock = NULL;
                    else
                      {
                        pBlock = GetEnclosingBlock (pEl, pSelDoc);
                        if (pBlock == NULL && !pEl->ElTerminal)
                          pBlock = pEl;
                      }
                    if (pBlock)
                      {
                        pPrevBlock = pBlock;
                        if (pBlock->ElParent &&
                            pBlock->ElPrevious == NULL &&
                            pBlock->ElNext == NULL &&
                            (TypeHasException (ExcHidden,
                                               pBlock->ElTypeNumber,
                                               pBlock->ElStructSchema)))
                          /* specific rules are linked to the parent element */
                          pElem = pBlock->ElParent;
                        else
                          pElem = pBlock;
                      }
                  }

                /* Standard properties */
                if (ChngStandardColor)
                  {
                    RuleSetPut (TheRules, PtBackground);
                    RuleSetPut (TheRules, PtForeground);
                  }
                RemoveSpecPresTree (pElem, pSelDoc, TheRules, SelectedView);

                /* Character properties */
                if (chngChars)
                  ModifyChar (pEl, pSelDoc, SelectedView,
                              locChngFontFamily, FontFamily,
                              locChngFontStyle, FontStyle,
                              locChngFontWeight, FontWeight,
                              locChngFontSize, FontSize,
                              locChngUnderline, UnderlineStyle,
                              locChngUlWeight, UnderlineWeight);

                /* Graphic properties */
                if (chngGraphics)
                  ModifyGraphics (pEl, pSelDoc, SelectedView,
                                  locChngLineStyle, LineStyle,
                                  locChngLineWeight, LineWeight, LocLineWeightUnit,
                                  locChngTrame, PaintWithPattern, FALSE, 0,
                                  FALSE, 0);

                /* Format properties */
                if (pBlock && chngFormat &&
                    /* don't apply to a pseudo paragraph or a table */
                    (!TypeHasException (ExcHidden,
                                        pBlock->ElTypeNumber,
                                        pBlock->ElStructSchema)/* &&
                                                                  !TypeHasException (ExcIsTable,
                                                                  pBlock->ElTypeNumber,
                                                                  pBlock->ElStructSchema)*/))
                  ModifyLining (pBlock, pSelDoc, SelectedView,
                                locChngAlign, Align,
                                locChngIndent, IndentValue * IndentSign,
                                locChngLineSp, OldLineSp,
                                locChngHyphen, Hyphenate);
                else if (chngFormat)
                  ModifyLining (pEl, pSelDoc, SelectedView,
                                locChngAlign, Align,
                                locChngIndent, IndentValue * IndentSign,
                                locChngLineSp, OldLineSp,
                                locChngHyphen, Hyphenate);
                /* Standard geometry */
                if (ChngStandardGeom)
                  {
                    RuleSetPut (GeomRules, PtVertPos);
                    RuleSetPut (GeomRules, PtHorizPos);
                    RuleSetPut (GeomRules, PtHeight);
                    RuleSetPut (GeomRules, PtWidth);
                    RemoveSpecPresTree (pEl, pSelDoc, GeomRules, SelectedView);
                    RuleSetClr (GeomRules);
                  }
                /* si on est dans un element copie' par inclusion,   */
                /* on met a jour les copies de cet element.          */
                RedisplayCopies (pEl, pSelDoc, TRUE);
                /* passe a l'element a traiter ensuite */
                pEl = pElNext;
              }
            /* fin de la boucle de parcours et traitement des */
            /* elements selectionnes */

            RuleSetClr (TheRules);
          }
        CloseHistorySequence (pSelDoc);
        /* try to collapse text elements except in TextFile
           where the presentation generates strings */
        if (pSelDoc && pSelDoc->DocSSchema &&
            strcmp (pSelDoc->DocSSchema->SsName, "TextFile"))
          SelectRange (pSelDoc, pFirstSel, pLastSel, firstChar, lastChar);
      }
}

/*----------------------------------------------------------------------
  TtcStandardGeometry
  handles the return of the Standard Geometry entry of the Present
  menu.
  ----------------------------------------------------------------------*/
void TtcStandardGeometry (Document document, View view)
{
  PtrElement          pEl, pFirstSel, pLastSel;
  PtrDocument         pSelDoc;
  int		      firstChar, lastChar;
  ThotBool            selectionOK;

  /* just to avoid deadlocks */
  TtaHandlePendingEvents ();
  selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel,
                                     &pLastSel, &firstChar, &lastChar);
  if (selectionOK && pSelDoc != NULL)
    if (pSelDoc->DocSSchema != NULL)
      /* il y a bien une selection et le document selectionne' n'a pas */
      /* ete ferme' */
      {
        /* eteint la selection courante */
        TtaClearViewSelections ();
        /* set selection to the highest level elements having the same
           content */
        SelectSiblings (&pFirstSel, &pLastSel, &firstChar, &lastChar);
        if (firstChar == 0 && lastChar == 0)
          if (pFirstSel->ElPrevious == NULL && pLastSel->ElNext == NULL)
            if (pFirstSel->ElParent != NULL &&
                pFirstSel->ElParent == pLastSel->ElParent)
              {
                pFirstSel = pFirstSel->ElParent;
                while (pFirstSel->ElPrevious == NULL &&
                       pFirstSel->ElNext == NULL &&
                       pFirstSel->ElParent != NULL)
                  pFirstSel = pFirstSel->ElParent;
                pLastSel = pFirstSel;
              }
        pEl = pFirstSel;
        while (pEl != NULL)
          /* Traite l'element courant */
          {
            RuleSetPut (GeomRules, PtVertPos);
            RuleSetPut (GeomRules, PtHorizPos);
            RuleSetPut (GeomRules, PtHeight);
            RuleSetPut (GeomRules, PtWidth);
            RemoveSpecPresTree (pEl, pSelDoc, GeomRules, SelectedView);
            RuleSetClr (GeomRules);
	    
            /* si on est dans un element copie' par inclusion,   */
            /* on met a jour les copies de cet element.          */
            RedisplayCopies (pEl, pSelDoc, TRUE);
            /* cherche l'element a traiter ensuite */
            pEl = NextInSelection (pEl, pLastSel);
          }
        SelectRange (pSelDoc, pFirstSel, pLastSel, firstChar, lastChar);
      }
}

/*----------------------------------------------------------------------
  TtcStandardPresentation
  handles the return of the Standard Presentation entry of the Present
  menu.
  ----------------------------------------------------------------------*/
void TtcStandardPresentation (Document document, View view)
{
  PtrDocument         pDoc;

  pDoc = LoadedDocument[document - 1];
  ResetPresentMenus ();

  /* active le formulaire "Presentation standard" */
  DocModPresent = pDoc;
}

/*----------------------------------------------------------------------
  CallbackStdPresMenu
  callback handler for the Standard Presentation popup menu.
  ----------------------------------------------------------------------*/
void CallbackStdPresMenu (int ref, int val)
{
  switch (ref)
    {
    case NumMenuPresentStandard:
      /* retour du choix multiple Presentation standard */
      switch (val)
        {
        case 0:
          /* caracteres standard */
          StdFontFamily = TRUE;
          StdFontStyle = TRUE;
          StdFontWeight = TRUE;
          StdUnderline = TRUE;
          StdUlWeight = TRUE;
          StdFontSize = TRUE;
          break;
        case 1:
          /* format standard */
          StdAlign = TRUE;
          StdHyphen = TRUE;
          StdIndent = TRUE;
          StdLineSp = TRUE;
          break;
        case 2:
          /* couleurs standard */
          ChngStandardColor = TRUE;
          break;
        case 3:
          /* graphiques standard */
          StdLineStyle = TRUE;
          StdLineWeight = TRUE;
          StdTrame = TRUE;
          break;
        case 4:
          /* standard geometry */
          ChngStandardGeom = TRUE;
          break;
        }
      break;
    case NumFormPresentStandard:
      if (val == 1)
        {
          /* retour "Appliquer" du formulaire Presentation standard */
          ApplyPresentMod (Apply_All);
          StdFontFamily = FALSE;
          StdFontStyle = FALSE;
          StdFontWeight = FALSE;
          StdUnderline = FALSE;
          StdUlWeight = FALSE;
          StdFontSize = FALSE;
          StdAlign = FALSE;
          StdHyphen = FALSE;
          StdIndent = FALSE;
          StdLineSp = FALSE;
          StdLineStyle = FALSE;
          StdLineWeight = FALSE;
          StdTrame = FALSE;
        }
      else
        /* retour "Terminer" */
        TtaDestroyDialogue(NumFormPresentStandard);
      break;
    }
}

/*----------------------------------------------------------------------
  CallbackPresMenu
  callback handler for the Presentation forms.
  ----------------------------------------------------------------------*/
void CallbackPresMenu (int ref, int val, char *txt)
{
  char                c;
  int                 i;

  switch (ref)
    {
    case NumMenuCharFamily:	/* famille de polices de caracteres */
      switch (val)
        {
        case 0:	/* Times */
          ChngFontFamily = TRUE;
          StdFontFamily = FALSE;
          FontFamily = 1;
          break;
        case 1:	/* Helvetica */
          ChngFontFamily = TRUE;
          StdFontFamily = FALSE;
          FontFamily = 2;
          break;
        case 2:	/* Courier */
          ChngFontFamily = TRUE;
          StdFontFamily = FALSE;
          FontFamily = 3;
          break;
        case 3:	/* standard */
          ChngFontFamily = FALSE;
          StdFontFamily = TRUE;
          break;
        default:
          ChngFontFamily = TRUE;
          StdFontFamily = FALSE;
          FontFamily = 1;
          break;
        }
      break;

    case NumMenuCharFontStyle:	/* style des caracteres */
      if (val == 3)	/* entree 3: Standard */
        {
          ChngFontStyle = FALSE;
          StdFontStyle = TRUE;
        }
      else
        {
          ChngFontStyle = TRUE;
          StdFontStyle = FALSE;
          FontStyle = val;
        }
      break;

    case NumMenuCharFontWeight:	/* graisse des caracteres */
      if (val == 2)	/* entree 2: Standard */
        {
          ChngFontWeight = FALSE;
          StdFontWeight = TRUE;
        }
      else
        {
          ChngFontWeight = TRUE;
          StdFontWeight = FALSE;
          FontWeight = val;
        }
      break;
    case NumMenuUnderlineType:		/* style du souligne */
      /* l'entree 2 est supprimee dans cette version */
      if (val == 4)	/* entree 4: Standard */
        {
          ChngUnderline = FALSE;
          StdUnderline = TRUE;
        }
      /*******
              else if (val == 3)
              {
              ChngUnderline = TRUE;
              StdUnderline = FALSE;
              UnderlineStyle = val + 1;
              }
              else if (val == 2)
              {
              ChngUnderline = TRUE;
              StdUnderline = FALSE;
              UnderlineStyle = val + 1;
              }
      *******/
      else
        {
          ChngUnderline = TRUE;
          StdUnderline = FALSE;
          UnderlineStyle = val;
        }
      break;
    case NumMenuUnderlineWeight:	/* epaisseur du souligne */
      if (val == 2)	/* entree 2: Standard */
        {
          ChngLineWeight = FALSE;
          StdLineWeight = TRUE;
        }
      else
        {
          ChngLineWeight = TRUE;
          StdLineWeight = FALSE;
          UnderlineWeight = val;
        }
      break;
    case NumMenuCharFontSize:	/* menu des corps en points typo */
      if (val >= 0 && val < NumberOfFonts ())
        {
          ChngFontSize = TRUE;
          StdFontSize = FALSE;
          FontSize = ThotFontPointSize (val);
        }
      else
        {
          ChngFontSize = FALSE;
          StdFontSize = TRUE;
        }
      break;
    case NumMenuAlignment:	/* alignement des lignes */
      if (val == 4)	/* entree 4: Standard */
        {
          ChngAlign = FALSE;
          StdAlign = TRUE;
        }
      else
        {
          ChngAlign = TRUE;
          StdAlign = FALSE;
          Align = val + 1;
        }
      break;
    case NumZoneRecess:	/* renfoncement de la premiere ligne */
      ChngIndent = TRUE;
      IndentValue = val;
#ifdef _WINGUI 
      WIN_IndentValue = val;
#endif /* _WINGUI */
      if (IndentSign != 0 && IndentValue == 0)
        {
          IndentSign = 0;
#ifdef _GTK
          TtaSetMenuForm (NumMenuRecessSense, 1);
#endif /* _GTK */
        }
      else if (IndentSign == 0 && IndentValue != 0)
        {
          IndentSign = 1;
#ifdef _GTK
          TtaSetMenuForm (NumMenuRecessSense, 0);
#endif /* _GTK */
        }
      break;
    case NumMenuRecessSense:	/* sens du renfoncement de la premiere ligne */
      if (val == 2)	/* entree 2: Standard */
        {
          ChngIndent = FALSE;
          StdIndent = TRUE;
        }
      else
        {
          if (val == 0)
            IndentSign = 1;
          else
            IndentSign = 0;
          ChngIndent = TRUE;
          StdIndent = FALSE;
          if (IndentSign != 0 && IndentValue == 0)
            {
              IndentValue = 15;
#ifdef _WINGUI
              WIN_IndentValue = 15;
#endif /* _WINGUI */
#ifdef _GTK
              TtaSetNumberForm (NumZoneRecess, 15);
#endif /* _GTK */
            }
          else if (IndentSign == 0 && IndentValue != 0)
            {
              IndentValue = 0;
#ifdef _WINGUI 
              WIN_IndentValue = 0;
#endif  /* _WINGUI */
#ifdef _GTK
              TtaSetNumberForm (NumZoneRecess, 0);
#endif /* _GTK */
            }
        }
      break;
    case NumZoneLineSpacing:	/* interligne */
      ChngLineSp = TRUE;
      if (OldLineSp != val)
        {
          OldLineSp = val;
#ifdef _WINGUI 
          WIN_OldLineSp = val;
#endif /* _WINGUI */
          if (val < (NormalLineSpacing * 3) / 2)
            i = 0;
          else if (val >= NormalLineSpacing * 2)
            i = 2;
          else
            i = 1;
        }
      break;
    case NumMenuLineSpacing:	/* saisie de l'interligne par un menu */
      if (val == 3)	/* entree 3: Standard */
        {
          ChngLineSp = FALSE;
          StdLineSp = TRUE;
        }
      else
        {
          ChngLineSp = TRUE;
          StdLineSp = FALSE;
          /* l'utilisateur demande a changer l'interligne */
          OldLineSp = ((val + 2) * NormalLineSpacing) / 2;
#ifdef _WINGUI
          WIN_OldLineSp = OldLineSp;
#endif  /* _WINGUI */
        }
      break;
    case NumMenuStrokeStyle:
      switch (val)
        {
        case 0:
          c = 'S';	/* trait continu */
          break;
        case 1:
          c = '-';	/* tirets longs */
          break;
        case 2:
          c = '.';	/* tirets courts */
          break;
        case 3:
          c = EOS;	/* standard */
        default:
          c = 'S';	/* trait continu */
          break;
        }
      if (c == EOS)	/* standard */
        {
          ChngLineStyle = FALSE;
          StdLineStyle = TRUE;
        }
      else
        {
          /* changement effectif de style de trait */
          ChngLineStyle = TRUE;
          StdLineStyle = FALSE;
          LineStyle = c;
        }
      break;
    case NumZoneStrokeWeight:
      ChngLineWeight = TRUE;
      StdLineWeight = FALSE;
      LineWeight = val;
      break;
    case NumToggleWidthUnchanged:
      ChngLineWeight = FALSE;
      StdLineWeight = TRUE;
      break;
    case NumSelectPattern:
      i = PatternNumber (txt);
      if (i >= 0)
        {
          ChngTrame = TRUE;
          StdTrame = FALSE;
          PaintWithPattern = i;
        }
      break;
    case NumTogglePatternUnchanged:
      ChngTrame = FALSE;
      StdTrame = TRUE;
      break;

    case NumFormPresGraphics:
      /* le formulaire Format lui-meme */
      if (val > 0)
        /* modify graphics */
        ApplyPresentMod (Apply_AllGraphics);
      else
        TtaDestroyDialogue (ref);
      break;
    case NumFormPresChar:
      /* active the form */
      if (val > 0)
        /* modify characters */
        ApplyPresentMod (Apply_AllChars);
      else
        TtaDestroyDialogue (ref);
      break;
    case NumFormPresFormat:
      /* le formulaire Format lui-meme */
      if (val > 0)
        /* modify format */
        ApplyPresentMod (Apply_AllFormat);
      else
        TtaDestroyDialogue (ref);
      break;
    default:
      break;
    }
}


/*----------------------------------------------------------------------
  ModPresentGraphiques
  user requests to modify the specific graphics presentation for the
  view "view" of document pDoc (if Assoc = False) or for the elements
  associated to view number "view" (if Assoc = True).  Initializes
  and activates the corresponding form.
  ----------------------------------------------------------------------*/
void TtcChangeGraphics (Document document, View view)
{
  PtrDocument         pSelDoc;
  PtrDocument         pDoc;
  PtrElement          pFirstSel, pLastSel;
  PtrAbstractBox      pAb;
  char                string[MAX_TXT_LEN];
  int                 currentFontSize;
  int                 i, nbItems;
  int                 firstChar, lastChar;
  ThotBool            selectionOK;

  pDoc = LoadedDocument[document - 1];

  /* demande quelle est la selection courante */
  selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
  if (!selectionOK)
    {
      /* par defaut la racine du document */
      pSelDoc = pDoc;
      pFirstSel = pDoc->DocDocElement;
      selectionOK = TRUE;
    }

  if (selectionOK && pSelDoc == pDoc)
    /* il y a une selection */
    {
      /* recherche le pave concerne */
      if (view > 100)
        pAb = AbsBoxOfEl (pFirstSel, 1);
      else
        pAb = AbsBoxOfEl (pFirstSel, view);

      if (pAb != NULL)
        {
          /* annule les etats memorises */
          ResetPresentMenus ();

	     /* sous-menu style des traits */
          i = 0;
          sprintf (&string[i], "%s", "Bsssss");	/* Traits_continu */
          i += strlen (&string[i]) + 1;
          sprintf (&string[i], "%s", "Bttttt");	/* Traits_tirete */
          i += strlen (&string[i]) + 1;
          sprintf (&string[i], "%s", "Buuuuu");	/* Traits_pointilles */
          i += strlen (&string[i]) + 1;
          sprintf (&string[i], "B%s", TtaGetMessage (LIB, TMSG_UNCHANGED));
          TtaNewSubmenu (NumMenuStrokeStyle, NumFormPresGraphics, 0,
                         TtaGetMessage (LIB, TMSG_LINE_STYLE), 4, string,
                         NULL, 0, TRUE);
       
	     /* initialise le sous-menu style des traits */
          ChngLineStyle = TRUE;
          StdLineStyle = FALSE;
          LineStyle = pAb->AbLineStyle;
          switch (LineStyle)
            {
            case 'S':
              i = 1;	/* trait continu (Solid) */
              break;
            case '-':
              i = 2;	/* tirete' */
              break;
            case '.':
              i = 3;	/* pointille' */
              break;
            default:
              i = 0;
              break;
            }
       
          ChngLineWeight = TRUE;
          StdLineWeight = FALSE;
          LineWeight = pAb->AbLineWeight;
          /* Toggle button Epaisseur des traits standard */
          sprintf (string, "B%s", TtaGetMessage (LIB, TMSG_UNCHANGED));

	     /* initialise la zone de saisie epaisseur des traits */
          if (pAb->AbLineWeightUnit == UnPoint)
            i = LineWeight;
          else
            {
              currentFontSize = PixelToPoint(PixelValue (pAb->AbSize,pAb->AbSizeUnit,
                                                         pAb, ViewFrameTable[ActiveFrame - 1].FrMagnification));
              i = (currentFontSize * LineWeight) / 10;
              if ((currentFontSize * i) % 10 >= 5)
                i++;
            }
	     /* selecteur motif de remplissage */
          nbItems = MakeMenuPattern (string, MAX_TXT_LEN);
          if (nbItems > 0)
            /* il y a des motifs de remplissage definis */
            /* on cree un selecteur */
            {
              if (nbItems >= 5)
                i = 5;
              else
                i = nbItems;

            }

          /* Toggle buttons */
          DocModPresent = pDoc;
        }
    }
}

/*----------------------------------------------------------------------
  ResetPresentMenus
  ----------------------------------------------------------------------*/
void ResetPresentMenus ()
{
  if (ThotLocalActions[T_presentstd] == NULL)
    {
      /* Connecte les actions liees au traitement de la TtcSplit */
      TteConnectAction (T_presentstd, (Proc) CallbackStdPresMenu);
      TteConnectAction (T_present, (Proc) CallbackPresMenu);
      ChngFontFamily = FALSE;
      StdFontFamily = FALSE;
      ChngFontStyle = FALSE;
      StdFontStyle = FALSE;
      ChngFontWeight = FALSE;
      StdFontWeight = FALSE;
      ChngUnderline = FALSE;
      StdUnderline = FALSE;
      ChngUlWeight = FALSE;
      StdUlWeight = FALSE;
      ChngFontSize = FALSE;
      StdFontSize = FALSE;
      ChngAlign = FALSE;
      StdAlign = FALSE;
      ChngHyphen = FALSE;
      StdHyphen = FALSE;
      ChngIndent = FALSE;
      StdIndent = FALSE;
      ChngLineSp = FALSE;
      StdLineSp = FALSE;
      ChngLineStyle = FALSE;
      StdLineStyle = FALSE;
      ChngLineWeight = FALSE;
      StdLineWeight = FALSE;
      ChngTrame = FALSE;
      StdTrame = FALSE;
      ChngStandardColor = FALSE;
      ChngStandardGeom = FALSE;
    }
  RuleSetClr (TheRules);
  RuleSetClr (GeomRules);
}
/* End Of Module modpres */
