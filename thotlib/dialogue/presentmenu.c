/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
 * presentmenu.c : Functions to modify the specific presentation
 *
 * Authors: I. Vatton (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
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

#include "actions_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "config_f.h"
#include "createabsbox_f.h"
#include "documentapi_f.h"
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
#include "viewcommands_f.h"

#ifdef _WINDOWS 
#include "wininclude.h"
#endif /* _WINDOWS */

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
static ThotBool     ChngJustif;	/* user asks to change the justification */
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
#define Apply_Justif		9
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
static ThotBool     StdJustif;	/* user asks to reset the justification */
static ThotBool     StdHyphen;	/* user asks to reset the hyphenation */
static ThotBool     StdIndent;	/* user asks to reset the indentation */
static ThotBool     StdLineSp;	/* user asks to reset the line spacing */
static ThotBool     StdLineStyle;  /* user asks to reset the line style */
static ThotBool     StdLineWeight; /* user asks to reset the line weight */
static ThotBool     StdTrame;	/* user asks to reset the pattern */

static char         FontFamily;	/* font family requested by the user */
static int          FontStyle;	/* font style requested by the user */
static int          FontWeight;	/* font weight requested by the user */
static int          UnderlineStyle; /* underline style requested by the user */
static int          UnderlineWeight;/* underline weight requested by user */
static int          FontSize;	/* body size (in points) requested by user */
static int          Align;	/* line alignment mode */
static ThotBool     Justif;	/* with or without justification */
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
static void         ResetMenus ();

#ifdef _WINDOWS 
extern int   WIN_IndentValue;
extern int   WIN_OldLineSp;
extern int   WIN_NormalLineSpacing;
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
   GetEnclosingBlock
   return the first ancestor of element pEl that has a Line presentation
   rule for the active view.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   GetEnclosingBlock (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
static PtrElement   GetEnclosingBlock (pEl, pDoc)
PtrElement          pEl;
PtrDocument	    pDoc;
#endif /* __STDC__ */
{
  PtrElement	pBlock;
  int		viewSch;
  PtrPRule	pPRule;
  PtrPSchema	pSPR;
  PtrSSchema	pSSR;
  PtrAttribute	pAttr;

  pBlock = NULL;
  viewSch = AppliedView (pEl, NULL, pDoc, SelectedView);
  while (pBlock == NULL && pEl != NULL)
     {
     pPRule = GlobalSearchRulepEl (pEl, &pSPR, &pSSR, 0, NULL, viewSch,
				   PtFunction, FnLine, FALSE, TRUE, &pAttr);
     if (pPRule != NULL)
	pBlock = pEl;
     else
	pEl = pEl->ElParent;
     }
  return pBlock;
}


/*----------------------------------------------------------------------
  ModifyGraphics applique a l'element pEl les modifications sur	
  les graphiques demandes par l'utilisateur.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ModifyGraphics (PtrElement pEl, PtrDocument pDoc, int viewToApply, ThotBool modifLineStyle, char LineStyle, ThotBool modifLineWeight, int LineWeight, TypeUnit LineWeightUnit, ThotBool modifFillPattern, int FillPattern, ThotBool modifColorBackground, int ColorBackground, ThotBool modifLineColor, int LineColor)

#else  /* __STDC__ */
static void         ModifyGraphics (pEl, pDoc, viewToApply, modifLineStyle, LineStyle, modifLineWeight, LineWeight, LineWeightUnit, modifFillPattern, FillPattern, modifColorBackground, ColorBackground, modifLineColor, LineColor)
PtrElement          pEl;
PtrDocument         pDoc;
int                 viewToApply;
ThotBool            modifLineStyle;
char                LineStyle;
ThotBool            modifLineWeight;
int                 LineWeight;
TypeUnit            LineWeightUnit;
ThotBool            modifFillPattern;
int                 FillPattern;
ThotBool            modifColorBackground;
int                 ColorBackground;
ThotBool            modifLineColor;
int                 LineColor;

#endif /* __STDC__ */

{
  TypeUnit            unit;
  PtrPRule            pPRule, pFunctRule;
  int                 viewSch, value;
  ThotBool            isNew;

  /* numero de cette view */
  viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);
  /* style des traits dans le graphique */
  if (modifLineStyle)
    {
      /*cherche la regle de presentation specifique 'LineStyle' de l'element */
      /* ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtLineStyle, 0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtLineStyle;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      value = (int) pPRule->PrChrValue;
      pPRule->PrChrValue = LineStyle;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  SetDocumentModified (pDoc, TRUE, 0);
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	}
      else if (!isNew)
	/* reset the previous value */
	pPRule->PrChrValue = value;
    }

  /* epaisseur des traits dans le graphique */
  if (modifLineWeight)
    {
      /* cherche la regle de presentation specifique 'Epaisseur Trait' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtLineWeight, 0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtLineWeight;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      unit = pPRule->PrMinUnit;
      pPRule->PrMinUnit = LineWeightUnit;
      pPRule->PrMinAttr = FALSE;
      value = pPRule->PrMinValue;
      pPRule->PrMinValue = LineWeight;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  SetDocumentModified (pDoc, TRUE, 0);
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	}
      else if (!isNew)
	{
	  /* reset the previous value */
	  pPRule->PrMinUnit = unit;
	  pPRule->PrMinValue = value;
	}
    }

  /* trame de remplissage */
  if (modifFillPattern)
    {
      /* cherche la regle de presentation specifique 'FillPattern' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtFillPattern, 0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtFillPattern;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      value = pPRule->PrIntValue;
      pPRule->PrIntValue = FillPattern;
      pPRule->PrAttrValue = FALSE;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  SetDocumentModified (pDoc, TRUE, 0);
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  
	  /* It the element is not a leaf in the abstract tree, create a
	     ShowBox rule for the element if there is none */
	  if (!pEl->ElTerminal &&
	      !TypeHasException (ExcNoShowBox, pEl->ElTypeNumber, pEl->ElStructSchema))
	    {
	      pFunctRule = SearchPresRule (pEl, PtFunction, FnShowBox, &isNew,
					   pDoc, viewToApply);
	      pFunctRule->PrType = PtFunction;
	      pFunctRule->PrViewNum = viewSch;
	      pFunctRule->PrPresMode = PresFunction;
	      pFunctRule->PrPresFunction = FnShowBox;
	      pFunctRule->PrPresBoxRepeat = FALSE;
	      pFunctRule->PrNPresBoxes = 0;
	      if (isNew)
		if (!PRuleMessagePre (pEl, pFunctRule, pDoc, isNew))
		  {
		    ApplyNewRule (pDoc, pFunctRule, pEl);
		    PRuleMessagePost (pEl, pFunctRule, pDoc, isNew);
		  }
	    }
	}
      else if (!isNew)
	/* reset the previous value */
	pPRule->PrIntValue = value;
    }

  /* couleur de fond */
  if (modifColorBackground)
    {
      /* cherche la regle de presentation specifique 'Background' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtBackground, 0, &isNew, pDoc, viewToApply);
      pPRule->PrType = PtBackground;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      value = pPRule->PrIntValue;
      pPRule->PrIntValue = ColorBackground;
      pPRule->PrAttrValue = FALSE;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  /* met les choix de l'utilisateur dans cette regle */
	  SetDocumentModified (pDoc, TRUE, 0);
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  
	  /* It the element is not a leaf in the abstract tree, create a
	     ShowBox rule for the element if there is none */
	  if (!pEl->ElTerminal &&
	      !TypeHasException (ExcNoShowBox, pEl->ElTypeNumber, pEl->ElStructSchema))
	    {
	      pFunctRule = SearchPresRule (pEl, PtFunction, FnShowBox, &isNew,
					   pDoc, viewToApply);
	      pFunctRule->PrType = PtFunction;
	      pFunctRule->PrViewNum = viewSch;
	      pFunctRule->PrPresMode = PresFunction;
	      pFunctRule->PrPresFunction = FnShowBox;
	      pFunctRule->PrPresBoxRepeat = FALSE;
	      pFunctRule->PrNPresBoxes = 0;
	      if (isNew)
		if (!PRuleMessagePre (pEl, pFunctRule, pDoc, isNew))
		  {
		    ApplyNewRule (pDoc, pFunctRule, pEl);
		    PRuleMessagePost (pEl, pFunctRule, pDoc, isNew);
		  }
	    }
	}
      else if (!isNew)
	/* reset the previous value */
	pPRule->PrIntValue = value;
    }

  /* couleur du trace' */
  if (modifLineColor)
    {
      /* cherche la regle de presentation specifique 'CouleurTrace' de */
      /* l'element ou en cree une nouvelle */
      pPRule = SearchPresRule (pEl, PtForeground, 0, &isNew, pDoc, viewToApply);
      /* met les choix de l'utilisateur dans cette regle */
      pPRule->PrType = PtForeground;
      pPRule->PrViewNum = viewSch;
      pPRule->PrPresMode = PresImmediate;
      value = pPRule->PrIntValue;
      pPRule->PrIntValue = LineColor;
      pPRule->PrAttrValue = FALSE;
      if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	{
	  SetDocumentModified (pDoc, TRUE, 0);
	  /* si le pave existe, applique la nouvelle regle au pave */
	  ApplyNewRule (pDoc, pPRule, pEl);
	  PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	}
      else if (!isNew)
	/* reset the previous value */
	pPRule->PrIntValue = value;
    }
}

/*----------------------------------------------------------------------
  ModifyColor change la presentation specifique de la couleur	
  de fond ou de trace' (selon Background) pour tous les elements de la	
  selection courante.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         ModifyColor (int colorNum, ThotBool Background)
#else  /* __STDC__ */
void         ModifyColor (colorNum, Background)
int          colorNum;
ThotBool     Background;
#endif /* __STDC__ */
{
   PtrDocument         SelDoc;
   PtrElement          pElFirstSel, pElLastSel, pEl;
   PtrAbstractBox      pAb;
   int                 firstChar, lastChar;
   ThotBool            selok, modifFillPattern;
   int                 fillPatternNum;
   RuleSet             rulesS;

   CloseInsertion ();
   /* demande quelle est la selection courante */
   selok = GetCurrentSelection (&SelDoc, &pElFirstSel, &pElLastSel, &firstChar, &lastChar);
   if (!selok)
      /* rien n'est selectionne' */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
   else if (SelDoc->DocReadOnly)
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
   else if (SelDoc != NULL && SelDoc->DocSSchema != NULL)
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
	   IsolateSelection (SelDoc, &pElFirstSel, &pElLastSel, &firstChar,
			     &lastChar, TRUE);

	if (colorNum < 0)
	   /* standard color */
	   {
	   /* set selection to the highest level elements having the same
	      content */
	     if (ThotLocalActions[T_selectsiblings] != NULL)
	       (*ThotLocalActions[T_selectsiblings]) (&pElFirstSel, &pElLastSel, &firstChar, &lastChar);
	     if (firstChar == 0 && lastChar == 0)
	       if (pElFirstSel->ElPrevious == NULL && pElLastSel->ElNext == NULL)
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

	if (ThotLocalActions[T_openhistory] != NULL)
	  (*ThotLocalActions[T_openhistory]) (SelDoc, pElFirstSel, pElLastSel, firstChar, lastChar);
	/* parcourt les elements selectionnes */
	pEl = pElFirstSel;
	while (pEl != NULL)
	  {
	     /* on saute les elements qui sont des copies */
	     if (!pEl->ElIsCopy)
		/* on saute les elements non modifiables */
		if (!ElementIsReadOnly (pEl))
		   /* on saute les marques de page */
		   if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
		     {
			modifFillPattern = FALSE;
			fillPatternNum = 0;
			if (Background)
			   /* on change la couleur de fond avec la souris */
			  {
			     pAb = AbsBoxOfEl (pEl, SelectedView);
			     if (pAb != NULL)
				if (pAb->AbFillPattern < 2)
				   /* on force la trame backgroundcolor si la
				      trame du pave */
				   /* est nopattern ou foregroundcolor */
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
			     RemoveSpecPresTree (pEl, SelDoc, rulesS, SelectedView);
			  }
			else
			   ModifyGraphics (pEl, SelDoc, SelectedView, FALSE,
					   SPACE, FALSE, 0, FALSE,
					   modifFillPattern, fillPatternNum,
					   (ThotBool)Background, colorNum, (ThotBool)(!Background),
					   colorNum);
			/* si on est dans un element copie' par inclusion,   */
			/* on met a jour les copies de cet element. */
			RedisplayCopies (pEl, SelDoc, TRUE);
		     }
	     /* cherche l'element a traiter ensuite */
	     pEl = NextInSelection (pEl, pElLastSel);
	  }
	/* tente de fusionner les elements voisins et reaffiche les paves */
	/* modifie's et la selection */
	if (ThotLocalActions[T_closehistory] != NULL)
	  (*ThotLocalActions[T_closehistory]) (SelDoc);
	SelectRange (SelDoc, pElFirstSel, pElLastSel, firstChar, lastChar);
     }
}


/*----------------------------------------------------------------------
  ModifyChar applique a l'element pEl les modifications sur	
  les caracteres demandes par l'utilisateur.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ModifyChar (PtrElement pEl, PtrDocument pDoc, int viewToApply, ThotBool modifFamily, char family, ThotBool modifStyle, int charStyle, ThotBool modifWeight, int charWeight, ThotBool modifsize, int size, ThotBool modifUnderline, int underline, ThotBool modifUlWeight, int weightUnderline)

#else  /* __STDC__ */
static void         ModifyChar (pEl, pDoc, viewToApply, modifFamily, family, modifStyle, charStyle, modifWeight, charWeight, modifsize, size, modifUnderline, underline, modifUlWeight, weightUnderline)
PtrElement          pEl;
PtrDocument         pDoc;
int                 viewToApply;
ThotBool            modifFamily;
char                family;
ThotBool            modifStyle;
int                 charStyle;
ThotBool	    modifWeight;
int		    charWeight;
ThotBool            modifsize;
int                 size;
ThotBool            modifUnderline;
int                 underline;
ThotBool            modifUlWeight;
int                 weightUnderline;

#endif /* __STDC__ */

{
   ThotBool            isNew;
   PtrPRule            pPRule;
   int                 viewSch;
   int                 intValue;
   char                value;

   /* numero de cette view */
   viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);
   /* applique les choix de l'utilisateur */
   /* family de polices de caracteres */
   if (modifFamily)
     {
	/* cherche la regle de presentation specifique 'Fonte' de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtFont, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtFont;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrChrValue;
	pPRule->PrChrValue = family;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrChrValue = value;
     }
   /* Style de caracteres */
   if (modifStyle)
     {
	/* cherche la regle de presentation specifique 'Style' de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtStyle, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtStyle;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrChrValue;
	switch (charStyle)
	  {
	  case 0:
	    pPRule->PrChrValue = 'R';	/* roman */
	    break;
	  case 1:
	    pPRule->PrChrValue = 'I';	/* italic */
	    break;
	  case 2:
	    pPRule->PrChrValue = 'O';	/* oblique */
	    break;
	  default:
	    pPRule->PrChrValue = 'R';
	    break;
	  }
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrChrValue = value;
     }
   /* Graisse des caracteres */
   if (modifWeight)
     {
	/* cherche la regle de presentation specifique 'Weight' de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtWeight, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtWeight;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrChrValue;
	switch (charWeight)
	  {
	  case 0:
	    pPRule->PrChrValue = 'N';	/* normal */
	    break;
	  case 1:
	    pPRule->PrChrValue = 'B';	/* bold */
	    break;
	  default:
	    pPRule->PrChrValue = 'N';
	    break;
	  }
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrChrValue = value;
     }
   /* Taille des caracteres */
   if (modifsize)
     {
	/* cherche la regle de presentation specifique 'Corps' de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtSize, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtSize;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	pPRule->PrMinUnit = UnPoint;
	pPRule->PrMinAttr = FALSE;
	intValue = pPRule->PrMinValue;
	pPRule->PrMinValue = size;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrMinValue = intValue;
     }

   /* Souligne' */
   if (modifUnderline)
     {
	/* cherche la regle de presentation specifique 'Souligne' de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtUnderline, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtUnderline;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrChrValue;
	switch (underline)
	  {
	  case 0:
	    pPRule->PrChrValue = 'N';	/* sans souligne */
	    break;
	  case 1:
	    pPRule->PrChrValue = 'U';	/* souligne continu */
	    break;
	  case 2:
	    pPRule->PrChrValue = 'O';	/* surligne */
	    break;
	  case 3:
	    pPRule->PrChrValue = 'C';	/* biffer */
	    break;
	  default:
	    pPRule->PrChrValue = 'N';
	    break;
	  }
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrChrValue = value;
     }
   /* Epaisseur du souligne */
   if (modifUlWeight)
     {
	/* cherche la regle de presentation specifique weightUnderline de l'element */
	/* ou en cree une nouvelle */
	pPRule = SearchPresRule (pEl, PtThickness, 0, &isNew, pDoc, viewToApply);
	/* met les choix de l'utilisateur dans cette regle */
	pPRule->PrType = PtThickness;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrChrValue;
	switch (weightUnderline)
	  {
	  case 0:
	    pPRule->PrChrValue = 'N';	/* souligne mince */
	    break;
	  case 1:
	    pPRule->PrChrValue = 'T';	/* souligne epais */
	    break;
	  default:
	    pPRule->PrChrValue = 'N';
	    break;
	  }
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     /* si le pave existe, applique la nouvelle regle au pave */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrChrValue = value;
     }
}


/*----------------------------------------------------------------------
   	ModifyLining applique a l'element pEl les modifications		
   		sur la mise en ligne demandes par l'utilisateur.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ModifyLining (PtrElement pEl, PtrDocument pDoc, int viewToApply, ThotBool modifAdjust, int Adjust, ThotBool modifJustif, ThotBool Justif, ThotBool modifIndent, int ValIndent, ThotBool modifLineSpacing, int LineSpacing, ThotBool modifHyphen, ThotBool Hyphenate)
#else  /* __STDC__ */
static void         ModifyLining (pEl, pDoc, viewToApply, modifAdjust, Adjust, modifJustif, Justif, modifIndent, ValIndent, modifLineSpacing, LineSpacing, modifHyphen, Hyphenate)
PtrElement          pEl;
PtrDocument         pDoc;
int                 viewToApply;
ThotBool            modifAdjust;
int                 Adjust;
ThotBool            modifJustif;
ThotBool            Justif;
ThotBool            modifIndent;
int                 ValIndent;
ThotBool            modifLineSpacing;
int                 LineSpacing;
ThotBool            modifHyphen;
ThotBool            Hyphenate;

#endif /* __STDC__ */
{
   ThotBool            isNew;
   PtrPRule            pPRule;
   BAlignment          value;
   int                 viewSch;
   int                 intValue;
   ThotBool            bValue;
   viewSch = AppliedView (pEl, NULL, pDoc, viewToApply);	/* Le type de cette view */
   /* applique les choix de l'utilisateur */
   if (modifAdjust && Adjust > 0)
     {
	pPRule = SearchPresRule (pEl, PtAdjust, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtAdjust;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	value = pPRule->PrAdjust;
	switch (Adjust)
	  {
	  case 1:
	    pPRule->PrAdjust = AlignLeft;
	    break;
	  case 2:
	    pPRule->PrAdjust = AlignRight;
	    break;
	  case 3:
	    pPRule->PrAdjust = AlignCenter;
	    break;
	  case 4:
	    pPRule->PrAdjust = AlignLeftDots;
	    break;
	  default:
	    pPRule->PrAdjust = AlignLeft;
	    break;
	  }
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrAdjust = value;
     }
   /* Justification */
   if (modifJustif)
     {
	pPRule = SearchPresRule (pEl, PtJustify, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtJustify;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	bValue = pPRule->PrJustify;
	pPRule->PrJustify = Justif;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrJustify = bValue;
     }
   /* Coupure des mots */
   if (modifHyphen)
     {
	pPRule = SearchPresRule (pEl, PtHyphenate, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtHyphenate;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	bValue = pPRule->PrJustify;
	pPRule->PrJustify = Hyphenate;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrJustify = bValue;
     }
   /* Renfoncement de la 1ere ligne */
   if (modifIndent)
     {
	pPRule = SearchPresRule (pEl, PtIndent, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtIndent;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	pPRule->PrMinUnit = UnPoint;
	pPRule->PrMinAttr = FALSE;
	intValue = pPRule->PrMinValue;
	pPRule->PrMinValue = ValIndent;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     /* le document est modifie' */
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrMinValue = intValue;
     }
   /* Interligne */
   if (modifLineSpacing)
     {
	pPRule = SearchPresRule (pEl, PtLineSpacing, 0, &isNew, pDoc, viewToApply);
	pPRule->PrType = PtLineSpacing;
	pPRule->PrViewNum = viewSch;
	pPRule->PrPresMode = PresImmediate;
	pPRule->PrMinUnit = UnPoint;
	pPRule->PrMinAttr = FALSE;
	intValue = pPRule->PrMinValue;
	pPRule->PrMinValue = LineSpacing;
	if (!PRuleMessagePre (pEl, pPRule, pDoc, isNew))
	  {
	     SetDocumentModified (pDoc, TRUE, 0);
	     ApplyNewRule (pDoc, pPRule, pEl);
	     PRuleMessagePost (pEl, pPRule, pDoc, isNew);
	  }
	else if (!isNew)
	  /* reset the previous value */
	  pPRule->PrMinValue = intValue;
     }
}

/*----------------------------------------------------------------------
   ApplyPresentMod
   applies the presentation modifications that were requested by means
   of the Characters form, the Format form, the Graphics form, or the
   Standard Geometry entry of the Present menu.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ApplyPresentMod (int applyDomain)
#else  /* __STDC__ */
static void         ApplyPresentMod (applyDomain)
int                 applyDomain;
#endif /* __STDC__ */
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
  ThotBool            locChngJustif;
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
	locChngJustif = ((StdJustif || ChngJustif)
			 && (applyDomain == Apply_Justif
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
	chngFormat = (locChngAlign || locChngJustif || locChngHyphen || locChngIndent || locChngLineSp);

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
	     IsolateSelection (pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar, TRUE);
	if (!addPresRule)
	   /* only changes to standard presentation */
	   {
	   /* set selection to the highest level elements having the same
	      content */
	     if (ThotLocalActions[T_selectsiblings] != NULL)
	       (*ThotLocalActions[T_selectsiblings]) (&pFirstSel, &pLastSel, &firstChar, &lastChar);
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
	   }
	
	/* evalue les difference entre le pave traite' et les demandes
	   de l'utilisateur */
	if (pFirstSel->ElAssocNum > 0)
	  pAb = AbsBoxOfEl (pFirstSel, 1);
	else
	  pAb = AbsBoxOfEl (pFirstSel, SelectedView);
	if (pAb != NULL)
	  {
	     currentFontSize = PixelToPoint(PixelValue (pAb->AbSize,
							pAb->AbSizeUnit, pAb, ViewFrameTable[ActiveFrame - 1].FrMagnification));
	    
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
		      case AlignLeftDots:
			i = 4;
			break;
		      default:
			i = 1;
			break;
		      }
		    locChngAlign = (i != Align);
		  }
	      }

	    /* justification */
	    if (locChngJustif)
	      {
		if (StdJustif)
		  {
		    RuleSetPut (TheRules, PtJustify);
		    locChngJustif = FALSE;
		  }
		else
		  locChngJustif = (Justif != pAb->AbJustify);
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
#           ifndef _WINDOWS
			TtaSetNumberForm (NumZoneRecess, IndentValue);
#           endif /* !_WINDOWS */
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

	if (chngChars || chngFormat || chngGraphics
	     || ChngStandardColor || ChngStandardGeom)
	   /* il y a quelque chose a changer, on parcourt la selection */
	   /* courante et on change ce qu'a demande' l'utilisateur */
	  {
	    OpenHistorySequence (pSelDoc, pFirstSel, pLastSel, firstChar, lastChar);
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
		  if (pPrevBlock != NULL && ElemIsWithinSubtree (pEl, pPrevBlock))
		     pBlock = NULL;
		  else
		     {
		     pBlock = GetEnclosingBlock (pEl, pSelDoc);
		     if (pBlock == NULL)
		        if (!pEl->ElTerminal)
			   pBlock = pEl;
		     }
		  if (pBlock != NULL)
		     {
		     pPrevBlock = pBlock;
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
		if (chngFormat)
		  if (pBlock != NULL)
		     ModifyLining (pBlock, pSelDoc, SelectedView,
				   locChngAlign, Align,
				   locChngJustif, Justif,
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
	/* tente de fusionner les elements voisins et reaffiche les paves */
	/* modifie's et la selection */
	CloseHistorySequence (pSelDoc);
	SelectRange (pSelDoc, pFirstSel, pLastSel, firstChar, lastChar);
      }
}

/*----------------------------------------------------------------------
   TtcStandardGeometry
   handles the return of the Standard Geometry entry of the Present
   menu.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcStandardGeometry (Document document, View view)
#else  /* __STDC__ */
void                TtcStandardGeometry (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
  PtrElement          pEl, pFirstSel, pLastSel;
  PtrDocument         pSelDoc;
  int		      firstChar, lastChar;
  ThotBool            selectionOK;

  /* just to avoid deadlocks */
  TtaHandlePendingEvents ();
  selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
  if (selectionOK && pSelDoc != NULL)
    if (pSelDoc->DocSSchema != NULL)
      /* il y a bien une selection et le document selectionne' n'a pas */
      /* ete ferme' */
      {
	/* eteint la selection courante */
	TtaClearViewSelections ();
	/* set selection to the highest level elements having the same
	   content */
	if (ThotLocalActions[T_selectsiblings] != NULL)
	  (*ThotLocalActions[T_selectsiblings]) (&pFirstSel, &pLastSel, &firstChar, &lastChar);
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
#ifdef __STDC__
void                TtcStandardPresentation (Document document, View view)
#else  /* __STDC__ */
void                TtcStandardPresentation (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
#ifndef _WINDOWS
   int                 i;
   CHAR_T              string[200];
#endif
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   ResetMenus ();

#  ifndef _WINDOWS
   /* formulaire presentation standard */
   TtaNewSheet (NumFormPresentStandard, TtaGetViewFrame (document, view), 
		TtaGetMessage (LIB, TMSG_STD_PRES),
		1, TtaGetMessage (LIB, TMSG_APPLY), TRUE, 1, 'L', D_DONE);

   /* choix multiple presentation standard */
   i = 0;
   usprintf (&string[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_STD_CHAR));
   i += ustrlen (&string[i]) + 1;
   usprintf (&string[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_STD_GRAPHICS));
   i += ustrlen (&string[i]) + 1;
   usprintf (&string[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_STD_COLORS));
   i += ustrlen (&string[i]) + 1;
   usprintf (&string[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_STD_FORMAT));
   i += ustrlen (&string[i]) + 1;
   usprintf (&string[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_STD_GEOMETRY));

   TtaNewToggleMenu (NumMenuPresentStandard, NumFormPresentStandard,
		TtaGetMessage (LIB, TMSG_STD_PRES), 5, string, NULL, TRUE);
   /* annule toutes les options du choix multiple Presentation standard */
   TtaSetToggleMenu (NumMenuPresentStandard, -1, FALSE);
#  endif /* _WINDOWS */
   /* active le formulaire "Presentation standard" */
   DocModPresent = pDoc;
#  ifndef _WINDOWS
   TtaShowDialogue (NumFormPresentStandard, TRUE);
#  endif /* !_WINDOWS */
}

/*----------------------------------------------------------------------
   CallbackStdPresMenu
   callback handler for the Standard Presentation popup menu.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackStdPresMenu (int ref, int val)
#else  /* __STDC__ */
void                CallbackStdPresMenu (ref, val)
int                 ref;
int                 val;

#endif /* __STDC__ */
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
	  StdJustif = TRUE;
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
	  StdJustif = FALSE;
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
#ifdef __STDC__
void                CallbackPresMenu (int ref, int val, STRING txt)
#else  /* __STDC__ */
void                CallbackPresMenu (ref, val, txt)
int                 ref;
int                 val;
STRING              txt;
#endif /* __STDC__ */
{
  char                c;
  int                 i;

  switch (ref)
    {
    case NumMenuCharFamily:	/* famille de polices de caracteres */
      switch (val)
	{
	case 0:
	  c = 'T';	/* Times */
	  break;
	case 1:
	  c = 'H';	/* Helvetica */
	  break;
	case 2:
	  c = 'C';	/* Courier */
	  break;
	case 3:
	  c = EOS;	/* standard */
	  break;
	default:
	  c = 'T';
	  break;
	}
      if (c == EOS)	/* standard */
	{
	  ChngFontFamily = FALSE;
	  StdFontFamily = TRUE;
	}
      else
	{
	  /* changement de famille de caracteres */
	  ChngFontFamily = TRUE;
	  StdFontFamily = FALSE;
	  FontFamily = c;
	}
      ApplyPresentMod (Apply_FontFamily);
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
      ApplyPresentMod (Apply_FontStyle);
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
      ApplyPresentMod (Apply_FontWeight);
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
      ApplyPresentMod (Apply_Underline);
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
      ApplyPresentMod (Apply_UlWeight);
      break;
    case NumMenuCharFontSize:	/* menu des corps en points typo */
      if (val >= 0 && val < NumberOfFonts ())
	{
	  ChngFontSize = TRUE;
	  StdFontSize = FALSE;
	  FontSize = FontPointSize (val);
	}
      else
	{
	  ChngFontSize = FALSE;
	  StdFontSize = TRUE;
	}
      ApplyPresentMod (Apply_FontSize);
      break;
    case NumMenuAlignment:	/* alignement des lignes */
      if (val == 3)	/* entree 3: Standard */
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
      ApplyPresentMod (Apply_Align);
      break;
    case NumMenuJustification:		/* justification */
      if (val == 2)	/* entree 2: Standard */
	{
	  ChngJustif = FALSE;
	  StdJustif = TRUE;
	}
      else
	{
	  ChngJustif = TRUE;
	  StdJustif = FALSE;
	  Justif = (val == 0);
	}
      ApplyPresentMod (Apply_Justif);
      break;
    case NumMenuWordBreak:	/* coupure des mots */
      if (val == 2)	/* entree 2: Standard */
	{
	  ChngHyphen = FALSE;
	  StdHyphen = TRUE;
	}
      else
	{
	  ChngHyphen = TRUE;
	  StdHyphen = FALSE;
	  Hyphenate = (val == 0);
	}
      ApplyPresentMod (Apply_Hyphen);
      break;
    case NumZoneRecess:	/* renfoncement de la premiere ligne */
      ChngIndent = TRUE;
      IndentValue = val;
#     ifdef _WINDOWS 
      WIN_IndentValue = val;
#     endif /* _WINDOWS */
      if (IndentSign != 0 && IndentValue == 0)
	{
	  IndentSign = 0;
#     ifndef _WINDOWS
	  TtaSetMenuForm (NumMenuRecessSense, 1);
#     endif /* !_WINDOWS */
	}
      else if (IndentSign == 0 && IndentValue != 0)
	{
	  IndentSign = 1;
#     ifndef _WINDOWS
	  TtaSetMenuForm (NumMenuRecessSense, 0);
#     endif /* !_WINDOWS */
	}
      ApplyPresentMod (Apply_Indent);
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
#         ifdef _WINDOWS
          WIN_IndentValue = 15;
#         else  /* _WINDOWS */
	      TtaSetNumberForm (NumZoneRecess, 15);
#         endif /* !_WINDOWS */
	    }
	  else if (IndentSign == 0 && IndentValue != 0)
	    {
	      IndentValue = 0;
#         ifdef _WINDOWS 
          WIN_IndentValue = 0;
#         else  /* _WINDOWS */
	      TtaSetNumberForm (NumZoneRecess, 0);
#         endif /* !_WINDOWS */
	    }
	}
      ApplyPresentMod (Apply_Indent);
      break;
    case NumZoneLineSpacing:	/* interligne */
      ChngLineSp = TRUE;
      if (OldLineSp != val)
	{
	  OldLineSp = val;
#     ifdef _WINDOWS 
      WIN_OldLineSp = val;
#     endif /* _WINDOWS */
	  if (val < (NormalLineSpacing * 3) / 2)
	    i = 0;
	  else if (val >= NormalLineSpacing * 2)
	    i = 2;
	  else
	    i = 1;
#     ifndef _WINDOWS 
	  TtaSetMenuForm (NumMenuLineSpacing, i);
#     endif /* !_WINDOWS */
	}
      ApplyPresentMod (Apply_LineSp);
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
#     ifdef _WINDOWS
      WIN_OldLineSp = OldLineSp;
#     else  /* _WINDOWS */
	  TtaSetNumberForm (NumZoneLineSpacing, OldLineSp);
#     endif /* !_WINDOWS */
	}
      ApplyPresentMod (Apply_LineSp);
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
      ApplyPresentMod (Apply_LineStyle);
      break;
    case NumZoneStrokeWeight:
      ChngLineWeight = TRUE;
      StdLineWeight = FALSE;
      LineWeight = val;
      ApplyPresentMod (Apply_LineWeight);
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
      ApplyPresentMod (Apply_Trame);
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
   TtcChangeCharacters
   user requests to modify the specific character presentation for the
   view "view" of document pDoc (if Assoc = False) or for the elements
   associated to view number "view" (if Assoc = True).
   Initializes and activates the corresponding form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcChangeCharacters (Document document, View view)
#else  /* __STDC__ */
void                TtcChangeCharacters (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pSelDoc;
   PtrDocument         pDoc;
   PtrElement          pFirstSel, pLastSel;
   PtrElement          pEl;
   PtrAbstractBox      pAb;
#  ifndef _WINDOWS 
   int                 nbItems;
   int                 max, bodyRelatSize, bodyPointSize;
   STRING              s;
   CHAR_T                string[MAX_TXT_LEN];
#  else  /* _WINDOWS */
   int                 fontNum;
   static CHOOSEFONT   cf ; 
   static LOGFONT      lf ;
#  endif /* !_WINDOWS */
   int                 i;
   int                 firstChar, lastChar;
   ThotBool            selectionOK;

   pDoc = LoadedDocument[document - 1];

   /* demande quelle est la selection courante */
   selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (!selectionOK)
     {
	/* par defaut la racine du document */
	pSelDoc = pDoc;
	pFirstSel = pDoc->DocRootElement;
	pLastSel = pFirstSel;
	selectionOK = TRUE;
     }

   if (selectionOK && pSelDoc == pDoc)
      /* il y a une selection */
     {
	/* cherche si l'un des elements selectionne's est protege' */
	pEl = pFirstSel;
	selectionOK = TRUE;
	while (selectionOK && pEl != NULL)
	   if (ElementIsReadOnly (pEl))
	      selectionOK = FALSE;
	   else
	      pEl = NextInSelection (pEl, pLastSel);
     }

   if (selectionOK)
      /* pas d'element protege', on peut modifier la presentation */
     {
	if (view > 100)
	   pAb = AbsBoxOfEl (pFirstSel, 1);
	else
	   pAb = AbsBoxOfEl (pFirstSel, view);

	if (pAb != NULL)
	  {
	     /* annule les etats memorises */
	     ResetMenus ();

#        ifndef _WINDOWS 
	     /* formulaire Presentation Caracteres */
	     TtaNewSheet (NumFormPresChar, TtaGetViewFrame (document, view), 
			  TtaGetMessage (LIB, TMSG_CHAR),
		 1, TtaGetMessage (LIB, TMSG_APPLY), FALSE, 2, 'L', D_DONE);
	     /* sous-menu Famille de caracteres */
	     i = 0;
	     sprintf (&string[i], "%s", "BTimes");
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "BHelvetica");
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "BCourier");
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuCharFamily, NumFormPresChar, 0,
	     TtaGetMessage (LIB, TMSG_FONT_FAMILY), 4, string, NULL, TRUE);

	     /* sous-menu style de caracteres */
	     i = 0;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_ROMAN));
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_ITALIC));
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_OBLIQUE));
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuCharFontStyle, NumFormPresChar, 0,
		   TtaGetMessage (LIB, TMSG_STYLE), 4, string, NULL, TRUE);

	     /* sous-menu type de Souligne */
	     i = 0;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_NORMAL));
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNDERLINE));
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_OVERLINE));
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_CROSS_OUT));
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuUnderlineType, NumFormPresChar, 0,
		    TtaGetMessage (LIB, TMSG_LINE), 5, string, NULL, TRUE);

	     /* sous-menu graisse des caracteres */
	     i = 0;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_NOT_BOLD));
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_BOLD));
	     i += ustrlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuCharFontWeight, NumFormPresChar, 0,
		   TtaGetMessage (LIB, TMSG_BOLDNESS), 3, string, NULL, TRUE);

	     /* sous-menus des corps disponibles, en points typographiques */
	     nbItems = 0;
	     i = 0;
	     max = NumberOfFonts ();
	     s = TtaGetMessage (LIB, TMSG_TYPOGRAPHIC_POINTS);
	     /* demande au mediateur la valeur en points typographiques de tous */
	     /* les corps relatifs successivement */
	     for (bodyRelatSize = 0; bodyRelatSize < max && i < MAX_TXT_LEN - 30; bodyRelatSize++)
	       {
		  bodyPointSize = FontPointSize (bodyRelatSize);
		  /* ajoute ce nouveau corps dans le buffer du menu */
		  sprintf (&string[i], "%s%d %s", "B", bodyPointSize, s);
		  i += ustrlen (&string[i]) + 1;
		  nbItems++;
	       }
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     nbItems++;
	     TtaNewSubmenu (NumMenuCharFontSize, NumFormPresChar, 0,
			    TtaGetMessage (LIB, TMSG_BODY_SIZE_PTS), nbItems, string, NULL, TRUE);
#        endif /* !_WINDOWS */
	     /* initialise la zone 'Famille de caracteres' */
	     ChngFontFamily = TRUE;
	     StdFontFamily = FALSE;
	     FontFamily = pAb->AbFont;
	     switch (FontFamily)
	       {
	       case 't':
	       case 'T':
		 i = 1;	/* Times */
		 break;
	       case 'h':
	       case 'H':
		 i = 2;	/* Helvetica */
		 break;
	       case 'c':
	       case 'C':
		 i = 3;	/* Courier */
		 break;
	       default:
		 FontFamily = 'T';
		 i = 0;
		 break;
	       }
#        ifdef _WINDOWS 
         fontNum = i;
#        else /* !_WINDOWS */
	     TtaSetMenuForm (NumMenuCharFamily, i - 1);
#        endif /* !_WINDOWS */
	     /* initialise le catalogue 'Style des caracteres' */
	     ChngFontStyle = TRUE;
	     StdFontStyle = FALSE;
	     FontStyle = pAb->AbFontStyle;
#        ifndef _WINDOWS 
	     TtaSetMenuForm (NumMenuCharFontStyle, FontStyle);
#        endif /* !_WINDOWS */
	     /* initialise le catalogue 'Graisse des caracteres' */
	     ChngFontWeight = TRUE;
	     StdFontWeight = FALSE;
	     FontWeight = pAb->AbFontWeight;
#        ifndef _WINDOWS 
	     TtaSetMenuForm (NumMenuCharFontWeight, FontWeight);
#        endif /* !_WINDOWS */

	     /* initialise le catalogue 'Epaisseur du souligne' */
	     ChngUnderline = TRUE;
	     StdUnderline = FALSE;
	     UnderlineStyle = pAb->AbUnderline;
#        ifndef _WINDOWS 
	     TtaSetMenuForm (NumMenuUnderlineType, UnderlineStyle);
#        endif /* !_WINDOWS */
	     ChngLineWeight = TRUE;
	     StdLineWeight = FALSE;
	     UnderlineWeight = pAb->AbThickness;
	     /*TtaSetMenuForm (NumMenuUnderlineWeight, UnderlineWeight);*/

	     /* initialise le sous-menu 'Corps des caracteres' */
	     ChngFontSize = TRUE;
	     StdFontSize = FALSE;
	     FontSize = pAb->AbSize;
	     if (pAb->AbSizeUnit == UnPoint)
		/* convertit la taille */
		i = FontRelSize (FontSize);
	     else
		i = pAb->AbSize;
#        ifndef _WINDOWS 
	     TtaSetMenuForm (NumMenuCharFontSize, i);
#       else  /* _WINDOWS */
		CreateCharacterDlgWindow (TtaGetViewFrame (document, view), fontNum, FontStyle, FontWeight, UnderlineStyle, FontSize);
#       endif /* _WINDOWS */
	  }
	DocModPresent = pDoc;
#   ifndef _WINDOWS 
	TtaShowDialogue (NumFormPresChar, TRUE);
#   endif /* !_WINDOWS */
     }
}

/*----------------------------------------------------------------------
  ModPresentGraphiques
  user requests to modify the specific graphics presentation for the
  view "view" of document pDoc (if Assoc = False) or for the elements
  associated to view number "view" (if Assoc = True).  Initializes
  and activates the corresponding form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcChangeGraphics (Document document, View view)
#else  /* __STDC__ */
void                TtcChangeGraphics (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pSelDoc;
   PtrDocument         pDoc;
   PtrElement          pFirstSel, pLastSel;
   PtrAbstractBox      pAb;
   CHAR_T                string[MAX_TXT_LEN];
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
	pFirstSel = pDoc->DocRootElement;
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
	     ResetMenus ();

#        ifndef _WINDOWS
	     /* feuille de dialogue Presentation Graphiques */
	     TtaNewSheet (NumFormPresGraphics, TtaGetViewFrame (document, view), 
			  TtaGetMessage (LIB, TMSG_GRAPHICS_PRES),
		 1, TtaGetMessage (LIB, TMSG_APPLY), FALSE, 3, 'L', D_DONE);
#        endif /* !_WINDOWS */

	     /* sous-menu style des traits */
	     i = 0;
	     usprintf (&string[i], TEXT("%s"), TEXT("Bsssss"));	/* Traits_continu */
	     i += ustrlen (&string[i]) + 1;
	     usprintf (&string[i], TEXT("%s"), TEXT("Bttttt"));	/* Traits_tirete */
	     i += ustrlen (&string[i]) + 1;
	     usprintf (&string[i], TEXT("%s"), TEXT("Buuuuu"));	/* Traits_pointilles */
	     i += ustrlen (&string[i]) + 1;
	     usprintf (&string[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuStrokeStyle, NumFormPresGraphics, 0, TtaGetMessage (LIB, TMSG_LINE_STYLE), 4, string, NULL, TRUE);
	     /* change la police des 3 premieres entrees du style des traits */
#        ifndef _WINDOWS
	     for (i = 0; i < 3; i++)
             TtaRedrawMenuEntry (NumMenuStrokeStyle, i, TEXT("icones"), ThotColorNone, -1);
#        endif /* !_WINDOWS */
	     /* initialise le sous-menu style des traits */
	     ChngLineStyle = TRUE;
	     StdLineStyle = FALSE;
	     LineStyle = pAb->AbLineStyle;
	     switch (LineStyle)
	       {
	       case TEXT('S'):
		 i = 1;	/* trait continu (Solid) */
		 break;
	       case TEXT('-'):
		 i = 2;	/* tirete' */
		 break;
	       case TEXT('.'):
		 i = 3;	/* pointille' */
		 break;
	       default:
		 i = 0;
		 break;
	       }
#        ifndef _WINDOWS
	     TtaSetMenuForm (NumMenuStrokeStyle, i - 1);

	     /* zone de saisie epaisseur des traits */
	     TtaNewNumberForm (NumZoneStrokeWeight, NumFormPresGraphics, TtaGetMessage (LIB, TMSG_LINE_WEIGHT), 0, 72, TRUE);
#        endif /* !_WINDOWS */
	     ChngLineWeight = TRUE;
	     StdLineWeight = FALSE;
	     LineWeight = pAb->AbLineWeight;
	     /* Toggle button Epaisseur des traits standard */
	     usprintf (string, TEXT("B%s"), TtaGetMessage (LIB, TMSG_UNCHANGED));
#        ifndef _WINDOWS
	     TtaNewToggleMenu (NumToggleWidthUnchanged, NumFormPresGraphics,
			       NULL, 1, string, NULL, TRUE);
#        endif /* !_WINDOWS */
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
#        ifndef _WINDOWS
	     TtaSetNumberForm (NumZoneStrokeWeight, i);
#        endif /* !_WINDOWS */

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

#         ifndef _WINDOWS 
		  TtaNewSelector (NumSelectPattern, NumFormPresGraphics, TtaGetMessage (LIB, TMSG_FILL_PATTERN), nbItems, string, i, NULL, TRUE, TRUE);
		  /* initialise le selecteur sur sa premiere entree */
		  TtaSetSelector (NumSelectPattern, pAb->AbFillPattern, TEXT(""));
#         endif /* _WINDOWS */
	       }
	     /* Toggle button Motif de remplissage standard */
#        ifndef _WINDOWS
	     i = 0;
	     usprintf (&string[i], TEXT("B%s"), TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewToggleMenu (NumTogglePatternUnchanged, NumFormPresGraphics,
			       NULL, 1, string, NULL, TRUE);
#         endif /* !_WINDOWS */
	     DocModPresent = pDoc;
#        ifndef _WINDOWS
	     TtaShowDialogue (NumFormPresGraphics, TRUE);
#        endif /* !_WINDOWS */
	  }
     }
}


/*----------------------------------------------------------------------
   ModPresentFormat    
   user requests to modify the specific format presentation for the
   view "view" of document pDoc (if Assoc = False) or for the elements
   associated to view number "view" (if Assoc = True).  Initializes
   and activates the corresponding form.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcChangeFormat (Document document, View view)
#else  /* __STDC__ */
void                TtcChangeFormat (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pSelDoc;
   PtrDocument         pDoc;
   PtrElement          pFirstSel, pLastSel;
   PtrAbstractBox      pAb;
   int                 firstChar, lastChar;
   int                 i;
#  ifdef _WINDOWS 
   int                 alignNum;
   int                 justificationNum;
   int                 lineSpacingNum;
   int                 indentNum;
#  else  /* _WINDOWS */
   CHAR_T                string[MAX_TXT_LEN];
#  endif /* !_WINDOWS */
   ThotBool            selectionOK;

   pDoc = LoadedDocument[document - 1];

   /* demande quelle est la selection courante */
   selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (!selectionOK) {
      /* par defaut la racine du document */
      pSelDoc = pDoc;
      pFirstSel = pDoc->DocRootElement;
      selectionOK = TRUE;
   }

   if (selectionOK && pSelDoc == pDoc) {
      /* il y a une selection */
      /* recherche le pave concerne */
      if (view > 100)
         pAb = AbsBoxOfEl (pFirstSel, 1);
      else
          pAb = AbsBoxOfEl (pFirstSel, view);

      if (pAb != NULL) {

         ResetMenus ();
#        ifndef _WINDOWS
         /* formulaire Presentation Format */
         TtaNewSheet (NumFormPresFormat, TtaGetViewFrame (document, view), 
                      TtaGetMessage (LIB, TMSG_FORMAT),
                      1, TtaGetMessage (LIB, TMSG_APPLY), TRUE, 3, 'L', D_DONE);

         /* sous-menu Alignement */
         i = 0;
         sprintf (&string[i], "%s", "Bmiidn");	/* gauche */
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s", "Bmeiin");	/* droite */
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s", "Bmfogn");	/* Centrer */
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));	/* Inchange */
         TtaNewSubmenu (NumMenuAlignment, NumFormPresFormat, 0,
         TtaGetMessage (LIB, TMSG_ALIGN), 4, string, NULL, TRUE);
         /* change la police des 3 premieres entrees */
         for (i = 0; i < 3; i++)
             TtaRedrawMenuEntry (NumMenuAlignment, i, "icones", ThotColorNone, -1);
         /* initialise le menu de cadrage des lignes */
#        endif _WINDOWS
         switch (pAb->AbAdjust) {
                case AlignLeft:   i = 1;
                                  break;
                case AlignRight:  i = 2;
                                  break;
                case AlignCenter: i = 3;
                                  break;
                default:          i = 1;
                                  break;
		 }
#        ifdef _WINDOWS
         alignNum = i - 1;
#        else  /* _WINDOWS */
         TtaSetMenuForm (NumMenuAlignment, i - 1);
         /* zone de saisie du renfoncement en points typo */
         TtaNewNumberForm (NumZoneRecess, NumFormPresFormat,
         TtaGetMessage (LIB, TMSG_INDENT_PTS), 0, 300, TRUE);
         /* initialise la valeur du renfoncement */
#        endif /* _WINDOWS */
         IndentValue = PixelToPoint (PixelValue (abs (pAb->AbIndent),
                                     pAb->AbIndentUnit, pAb, ViewFrameTable[ActiveFrame - 1].FrMagnification));
#        ifndef _WINDOWS
         TtaSetNumberForm (NumZoneRecess, IndentValue);

         /* sous-menu sens de renfoncement */
         i = 0;
         sprintf (&string[i], "%s", "Bm_`an");
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s", "Bmb`an");
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
         TtaNewSubmenu (NumMenuRecessSense, NumFormPresFormat, 0,
                        TtaGetMessage (LIB, TMSG_INDENT), 3, string, NULL, TRUE);
         /* change la police des 3 premieres entrees du sous-menu */
         for (i = 0; i < 2; i++)
         TtaRedrawMenuEntry (NumMenuRecessSense, i, "icones", ThotColorNone, -1);
         /* initialise le sens de renfoncement */
#        endif /* _WINDOWS */
         if (pAb->AbIndent > 0)
            i = 0;
         else
             i = 1;
#        ifdef _WINDOWS
         indentNum = i;
#        else  /* _WINDOWS */
         TtaSetMenuForm (NumMenuRecessSense, i);

         /* sous-menu de justification */
         i = 0;
         sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_LIB_YES));
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_LIB_NO));
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
         TtaNewSubmenu (NumMenuJustification, NumFormPresFormat, 0,
                        TtaGetMessage (LIB, TMSG_JUSTIFY), 3, string, NULL, TRUE);
#        endif /* _WINDOWS */
         /* menu de justification */
         if (pAb->AbJustify)
            i = 1;  /* avec justification */
         else
             i = 2; /* sans justification */
#        ifdef _WINDOWS 
         justificationNum = i - 1;
#        else /* _WINDOWS */
         TtaSetMenuForm (NumMenuJustification, i - 1);

         /* zone de saisie de l'interligne en points typo */
         TtaNewNumberForm (NumZoneLineSpacing, NumFormPresFormat,
                           TtaGetMessage (LIB, TMSG_LINE_SPACING_PTS), 1, 200, TRUE);

         /* sous-menu Interligne (Grandeur) */
         i = 0;
         sprintf (&string[i], "%s", "BmTTTn");	/* 'Normal%' */
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s", "BmWWWn");	/* 'Double%' */
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s", "BmZZZn");	/* 'Triple%' */
         i += ustrlen (&string[i]) + 1;
         sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
         TtaNewSubmenu (NumMenuLineSpacing, NumFormPresFormat, 0,
                        TtaGetMessage (LIB, TMSG_LINE_SPACING), 4, string, NULL, TRUE);
         /* change la police des 3 premieres entrees */
         for (i = 0; i < 3; i++)
             TtaRedrawMenuEntry (NumMenuLineSpacing, i, "icones", ThotColorNone, -1);
#        endif /* _WINDOWS */
         /* initialise l'interligne en points typographiques */
         OldLineSp = PixelToPoint (PixelValue (pAb->AbLineSpacing,
                                     pAb->AbLineSpacingUnit, pAb, ViewFrameTable[ActiveFrame - 1].FrMagnification));
#        ifndef _WINDOWS
         TtaSetNumberForm (NumZoneLineSpacing, OldLineSp);
#        endif /* _WINDOWS */
         NormalLineSpacing = PixelToPoint(PixelValue (10, UnRelative, pAb, ViewFrameTable[ActiveFrame - 1].FrMagnification));
#        ifdef _WINDOWS
         WIN_NormalLineSpacing = NormalLineSpacing;
#        endif /* _WINDOWS */
         /* saisie de l'interligne par un menu */
         if (OldLineSp < (NormalLineSpacing * 3) / 2)
            i = 0;
         else if (OldLineSp >= NormalLineSpacing * 2)
              i = 2;
         else
              i = 1;
#        ifdef _WINDOWS
         lineSpacingNum = i;
         CreateChangeFormatDlgWindow (NumZoneRecess, NumZoneLineSpacing, alignNum, IndentValue, indentNum, justificationNum, OldLineSp, lineSpacingNum);
#        else  /* _WINDOWS */
         TtaSetMenuForm (NumMenuLineSpacing, i);
         DocModPresent = pDoc;
         TtaShowDialogue (NumFormPresFormat, TRUE);
#        endif /* _WINDOWS */
      }
   }	
}


/*----------------------------------------------------------------------
  ResetMenus
  ----------------------------------------------------------------------*/
static void         ResetMenus ()
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
	ChngJustif = FALSE;
	StdJustif = FALSE;
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
