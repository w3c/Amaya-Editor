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
 * Author: I. Vatton (INRIA)
 *         R. Guetari (W3C/INRIA) - Windows routines.
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
#include "page_tv.h"
#include "edit_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"

#include "tree_f.h"
#include "attributes_f.h"
#include "config_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "viewcommands_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "presrules_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "unstructchange_f.h"
#include "actions_f.h"
#include "units_f.h"

static PtrDocument  DocModPresent;
static boolean             ChngStandardColor;	/* standard presentation colors  */
static boolean             ChngStandardGeom;	/* standard geometry */
/* user presentation choices and their values */
static boolean      ChngFontFamily; /* user asks to modify the font family */
static boolean      ChngStyle;	/* user asks to modify the style */
static boolean      ChngUnderline;  /* user asks to modify the underline */
static boolean      ChngWeight;	/* user asks to modify the underline weight */
static boolean      ChngBodySize;   /* user asks to modify the body size */
static boolean      ChngCadr;	/* user asks to modify the alignment mode */
static boolean      ChngJustif;	/* user asks to change the justification */
static boolean      ChngHyphen;	/* user asks to change the hyphenation */
static boolean      ChngIndent;	/* user asks to change the indentation */
static boolean      ChngLineSp;	/* user asks to change the line spacing */
static boolean      ChngLineStyle;  /* user asks to change the line style */
static boolean      ChngLineWeight; /* user asks to change the line weight */
static boolean      ChngTrame;	/* user asks to change the pattern */
#define Apply_All		0
#define Apply_FontFamily	1
#define Apply_Style		2
#define Apply_Underline		3
#define Apply_Weigh		4
#define Apply_BodySize		5
#define Apply_AllChars		6

#define Apply_Cadr		7
#define Apply_Justif		8
#define Apply_Hyphen		9
#define Apply_Indent		10
#define Apply_LineSp		11
#define Apply_AllFormat		12

#define Apply_LineStyle		13
#define Apply_LineWeight	14
#define Apply_Trame		15
#define Apply_AllGraphics	16

static boolean      StdFontFamily; /* user asks to reset the font family */
static boolean      StdStyle;	/* user asks to reset the style */
static boolean      StdUnderline;  /* user asks to reset the underline */
static boolean      StdWeight;	/* user asks to reset the underline weight */
static boolean      StdBodySize;   /* user asks to reset the body size */
static boolean      StdCadr;	/* user asks to reset the alignment mode */
static boolean      StdJustif;	/* user asks to reset the justification */
static boolean      StdHyphen;	/* user asks to reset the hyphenation */
static boolean      StdIndent;	/* user asks to reset the indentation */
static boolean      StdLineSp;	/* user asks to reset the line spacing */
static boolean      StdLineStyle;  /* user asks to reset the line style */
static boolean      StdLineWeight; /* user asks to reset the line weight */
static boolean      StdTrame;	/* user asks to reset the pattern */
static char         FontFamily;	/* font family requested by the user */
static int          Style;	/* character style requested by the user */
static int          UnderlineStyle; /* underline style requested by the user */
static int          UnderlineWeight;/* underline weight requested by user */
static int          BodySize;	/* body size (in points) requested by user */
static int          Cadr;	/* line alignment mode */
static boolean      Justif;	/* with or without justification */
static boolean      Hyphenate;	/* with or without hyphenation */
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
  PtrElement          pEl, pFirstSel, pLastSel, pElem, pBlock, pPrevBlock;
  PtrDocument         pSelDoc;
  PtrAbstractBox      pAb;
  TypeUnit            LocLineWeightUnit;
  int                 firstChar, lastChar;
  int                 currentBodySize;
  int                 i;
  int                 sign;
  boolean             selectionOK;
  boolean             chngChars;
  boolean             chngFormat;
  boolean             chngGraphics;
  boolean             locChngFontFamily;
  boolean             locChngStyle;
  boolean             locChngBodySize;
  boolean             locChngUnderline;
  boolean             locChngWeight;
  boolean             locChngLineStyle;
  boolean             locChngLineWeight;
  boolean             locChngTrame;
  boolean             locChngCadr;
  boolean             locChngJustif;
  boolean             locChngHyphen;
  boolean             locChngIndent;
  boolean             locChngLineSp;
  boolean	      addPresRule;

  selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
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
	locChngBodySize = ((StdBodySize || ChngBodySize)
			   && (applyDomain == Apply_BodySize
			        || applyDomain == Apply_AllChars
			       || applyDomain == Apply_All));
	addPresRule = addPresRule || (ChngBodySize
			   && (applyDomain == Apply_BodySize
			        || applyDomain == Apply_AllChars
			       || applyDomain == Apply_All));
	locChngWeight = ((StdWeight || ChngWeight)
			 && (applyDomain == Apply_Weigh
			     || applyDomain == Apply_AllChars
			     || applyDomain == Apply_All));
	addPresRule = addPresRule || (ChngWeight
			 && (applyDomain == Apply_Weigh
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
	locChngStyle = ((StdStyle || ChngStyle)
			&& (applyDomain == Apply_Style
			    || applyDomain == Apply_AllChars
			    || applyDomain == Apply_All));
	addPresRule = addPresRule || (ChngStyle
			&& (applyDomain == Apply_Style
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
	chngChars = (locChngBodySize || locChngWeight || locChngUnderline || locChngStyle || locChngFontFamily);

	/* Set chngFormat indicator */
	locChngCadr = ((StdCadr || ChngCadr)
		       && (applyDomain == Apply_Cadr
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
	chngFormat = (locChngCadr || locChngJustif || locChngHyphen || locChngIndent || locChngLineSp);

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
	     currentBodySize = PixelToPoint(PixelValue (pAb->AbSize,
							pAb->AbSizeUnit, pAb));
	    
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
	    if (locChngStyle)
	      {
		if (StdStyle)
		  {
		    RuleSetPut (TheRules, PtStyle);
		    locChngStyle = FALSE;
		  }
		else
		  locChngStyle = (Style != pAb->AbHighlight);
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
	    if (locChngWeight)
	      {
		if (StdWeight)
		  {
		    RuleSetPut (TheRules, PtThickness);
		    StdWeight = FALSE;
		  }
		else
		  locChngWeight = (UnderlineWeight != pAb->AbThickness);
	      }

	    /* corps en points typo */
	    if (locChngBodySize)
	      {
		if (StdBodySize)
		  {
		    RuleSetPut (TheRules, PtSize);
		    locChngBodySize = FALSE;
		  }
		else
		  locChngBodySize = (BodySize != currentBodySize);
	      }

	    /* alignement des lignes */
	    if (locChngCadr)
	      {
		if (StdCadr)
		  {
		    RuleSetPut (TheRules, PtAdjust);
		    locChngCadr = FALSE;
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
		    locChngCadr = (i != Cadr);
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
		    i = PixelToPoint(PixelValue (abs (pAb->AbIndent),
						 pAb->AbIndentUnit, pAb));
		    if (sign == IndentSign && i == IndentValue)
		      /* pas de changement */
		      locChngIndent = FALSE;
		    else
		      {
			if (IndentSign != 0 && IndentValue == 0)
			  IndentValue = 15;
			TtaSetNumberForm (NumZoneRecess, IndentValue);
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
		    i = PixelToPoint(PixelValue (pAb->AbLineSpacing,
						 pAb->AbLineSpacingUnit, pAb));
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
			i = (currentBodySize * pAb->AbLineWeight) / 10;
			if ((currentBodySize * i) % 10 >= 5)
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
	    pEl = pFirstSel;
	    pPrevBlock = NULL;
	    while (pEl != NULL)
	      /* Traite l'element courant */
	      {
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
		  ModifyChar (pEl, pSelDoc, SelectedView, locChngFontFamily,
			      FontFamily, locChngStyle, Style, locChngBodySize, BodySize,
			      locChngUnderline, UnderlineStyle, locChngWeight, UnderlineWeight);

		/* Graphic properties */
		if (chngGraphics)
		  ModifyGraphics (pEl, pSelDoc, SelectedView, locChngLineStyle,
				  LineStyle, locChngLineWeight, LineWeight, LocLineWeightUnit,
				  locChngTrame, PaintWithPattern, FALSE, 0,
				  FALSE, 0);

		/* Format properties */
		if (chngFormat)
		  if (pBlock != NULL)
		     ModifyLining (pBlock, pSelDoc, SelectedView, locChngCadr, Cadr,
				locChngJustif, Justif, locChngIndent, IndentValue * IndentSign,
				locChngLineSp, OldLineSp, locChngHyphen, Hyphenate);
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
		/* cherche l'element a traiter ensuite */
		pEl = NextInSelection (pEl, pLastSel);
	      }
	    /* fin de la boucle de parcours et traitement des */
	    /* elements selectionnes */

	    RuleSetClr (TheRules);
	  }
	/* tente de fusionner les elements voisins et reaffiche les paves */
	/* modifie's et la selection */
	MergeAndSelect (pSelDoc, pFirstSel, pLastSel, firstChar, lastChar);
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
  boolean             selectionOK;

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
	MergeAndSelect (pSelDoc, pFirstSel, pLastSel, firstChar, lastChar);
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
   int                 i;
   char                string[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   ResetMenus ();

   /* formulaire presentation standard */
   TtaNewSheet (NumFormPresentStandard, TtaGetViewFrame (document, view), 
		TtaGetMessage (LIB, TMSG_STD_PRES),
		1, TtaGetMessage (LIB, TMSG_APPLY), TRUE, 1, 'L', D_DONE);

   /* choix multiple presentation standard */
   i = 0;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_STD_CHAR));
   i += strlen (&string[i]) + 1;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_STD_GRAPHICS));
   i += strlen (&string[i]) + 1;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_STD_COLORS));
   i += strlen (&string[i]) + 1;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_STD_FORMAT));
   i += strlen (&string[i]) + 1;
   sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_STD_GEOMETRY));
   TtaNewToggleMenu (NumMenuPresentStandard, NumFormPresentStandard,
		TtaGetMessage (LIB, TMSG_STD_PRES), 5, string, NULL, TRUE);
   /* annule toutes les options du choix multiple Presentation standard */
#  ifndef _WINDOWS 
   TtaSetToggleMenu (NumMenuPresentStandard, -1, FALSE);
#  endif /* _WINDOWS */
   /* active le formulaire "Presentation standard" */
   DocModPresent = pDoc;
   TtaShowDialogue (NumFormPresentStandard, TRUE);
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
	  StdStyle = TRUE;
	  StdUnderline = TRUE;
	  StdWeight = TRUE;
	  StdBodySize = TRUE;
	  break;
	case 1:
	  /* format standard */
	  StdCadr = TRUE;
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
	  StdStyle = FALSE;
	  StdUnderline = FALSE;
	  StdWeight = FALSE;
	  StdBodySize = FALSE;
	  StdCadr = FALSE;
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
void                CallbackPresMenu (int ref, int val, char *txt)
#else  /* __STDC__ */
void                CallbackPresMenu (ref, val, txt)
int                 ref;
int                 val;
char               *txt;
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
    case NumMenuStyleChar:	/* style des caracteres */
      if (val == 6)	/* entree 6: Standard */
	{
	  ChngStyle = FALSE;
	  StdStyle = TRUE;
	}
      else
	{
	  ChngStyle = TRUE;
	  StdStyle = FALSE;
	  Style = val;
	}
      ApplyPresentMod (Apply_Style);
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
      ApplyPresentMod (Apply_Weigh);
      break;
    case NumMenuCharFontSize:	/* menu des corps en points typo */
      if (val >= 0 && val < NumberOfFonts ())
	{
	  ChngBodySize = TRUE;
	  StdBodySize = FALSE;
	  BodySize = FontPointSize (val);
	}
      else
	{
	  ChngBodySize = FALSE;
	  StdBodySize = TRUE;
	}
      ApplyPresentMod (Apply_BodySize);
      break;
    case NumMenuAlignment:	/* alignement des lignes */
      if (val == 3)	/* entree 3: Standard */
	{
	  ChngCadr = FALSE;
	  StdCadr = TRUE;
	}
      else
	{
	  ChngCadr = TRUE;
	  StdCadr = FALSE;
	  Cadr = val + 1;
	}
      ApplyPresentMod (Apply_Cadr);
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
      if (IndentSign != 0 && IndentValue == 0)
	{
	  IndentSign = 0;
	  TtaSetMenuForm (NumMenuRecessSense, 1);
	}
      else if (IndentSign == 0 && IndentValue != 0)
	{
	  IndentSign = 1;
	  TtaSetMenuForm (NumMenuRecessSense, 0);
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
	      TtaSetNumberForm (NumZoneRecess, 15);
	    }
	  else if (IndentSign == 0 && IndentValue != 0)
	    {
	      IndentValue = 0;
	      TtaSetNumberForm (NumZoneRecess, 0);
	    }
	}
      ApplyPresentMod (Apply_Indent);
      break;
    case NumZoneLineSpacing:	/* interligne */
      ChngLineSp = TRUE;
      if (OldLineSp != val)
	{
	  OldLineSp = val;
	  if (val < (NormalLineSpacing * 3) / 2)
	    i = 0;
	  else if (val >= NormalLineSpacing * 2)
	    i = 2;
	  else
	    i = 1;
	  TtaSetMenuForm (NumMenuLineSpacing, i);
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
	  TtaSetNumberForm (NumZoneLineSpacing, OldLineSp);
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
#  ifndef _WINDOWS 
   int                 nbItems;
   int                 max, bodyRelatSize, bodyPointSize;
   char               *s;
   char                string[MAX_TXT_LEN];
#  else  /* _WINDOWS */
   int                 fontNum;
#  endif /* !_WINDOWS */
   int                 i;
   PtrDocument         pSelDoc;
   PtrElement          pFirstSel, pLastSel;
   int                 firstChar, lastChar;
   boolean             selectionOK;
   PtrAbstractBox      pAb;
   PtrElement          pEl;
   PtrDocument         pDoc;

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
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "BHelvetica");
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "BCourier");
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuCharFamily, NumFormPresChar, 0,
	     TtaGetMessage (LIB, TMSG_FONT_FAMILY), 4, string, NULL, TRUE);

	     /* sous-menu style de caracteres */
	     i = 0;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_ROMAN));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_BOLD));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_ITALIC));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_OBLIQUE));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_BOLD_ITALIC));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_BOLD_OBLIQUE));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuStyleChar, NumFormPresChar, 0,
		   TtaGetMessage (LIB, TMSG_STYLE), 7, string, NULL, TRUE);

	     /* sous-menu type de Souligne */
	     i = 0;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_NORMAL));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNDERLINE));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_OVERLINE));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_CROSS_OUT));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuUnderlineType, NumFormPresChar, 0,
		    TtaGetMessage (LIB, TMSG_LINE), 5, string, NULL, TRUE);
	     TtaNewLabel (NumMenuUnderlineWeight, NumFormPresChar, " ");
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
		  i += strlen (&string[i]) + 1;
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
	     ChngStyle = TRUE;
	     StdStyle = FALSE;
	     Style = pAb->AbHighlight;
#        ifndef _WINDOWS 
	     TtaSetMenuForm (NumMenuStyleChar, Style);
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
	     ChngBodySize = TRUE;
	     StdBodySize = FALSE;
	     BodySize = pAb->AbSize;
	     if (pAb->AbSizeUnit == UnPoint)
		/* convertit la taille */
		i = FontRelSize (BodySize);
	     else
		i = pAb->AbSize;
#        ifndef _WINDOWS 
	     TtaSetMenuForm (NumMenuCharFontSize, i);
#       else  /* _WINDOWS */
		CreateCharacterDlgWindow (TtaGetViewFrame (document, view), fontNum, Style, UnderlineStyle, BodySize);
#       endif /* _WINDOWS */
	  }
	DocModPresent = pDoc;
	TtaShowDialogue (NumFormPresChar, TRUE);
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
   int                 i, nbItems;
   PtrDocument         pSelDoc;
   PtrElement          pFirstSel, pLastSel;
   int                 firstChar, lastChar;
   boolean             selectionOK;
   PtrAbstractBox      pAb;
   int                 currentBodySize;
   char                string[MAX_TXT_LEN];
   PtrDocument         pDoc;

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

	     /* feuille de dialogue Presentation Graphiques */
	     TtaNewSheet (NumFormPresGraphics, TtaGetViewFrame (document, view), 
			  TtaGetMessage (LIB, TMSG_GRAPHICS_PRES),
		 1, TtaGetMessage (LIB, TMSG_APPLY), FALSE, 3, 'L', D_DONE);

	     /* sous-menu style des traits */
	     i = 0;
	     sprintf (&string[i], "%s", "Bsssss");	/* Traits_continu */
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "Bttttt");	/* Traits_tirete */
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "Buuuuu");	/* Traits_pointilles */
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuStrokeStyle, NumFormPresGraphics, 0,
	      TtaGetMessage (LIB, TMSG_LINE_STYLE), 4, string, NULL, TRUE);
	     /* change la police des 3 premieres entrees du style des traits */
	     for (i = 0; i < 3; i++)
		TtaRedrawMenuEntry (NumMenuStrokeStyle, i, "icones", ThotColorNone, -1);
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
	     TtaSetMenuForm (NumMenuStrokeStyle, i - 1);

	     /* zone de saisie epaisseur des traits */
	     TtaNewNumberForm (NumZoneStrokeWeight, NumFormPresGraphics,
		       TtaGetMessage (LIB, TMSG_LINE_WEIGHT), 0, 72, TRUE);
	     ChngLineWeight = TRUE;
	     StdLineWeight = FALSE;
	     LineWeight = pAb->AbLineWeight;
	     /* Toggle button Epaisseur des traits standard */
	     sprintf (string, "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewToggleMenu (NumToggleWidthUnchanged, NumFormPresGraphics,
			       NULL, 1, string, NULL, TRUE);
	     /* initialise la zone de saisie epaisseur des traits */
	     if (pAb->AbLineWeightUnit == UnPoint)
		i = LineWeight;
	     else
	       {
	          currentBodySize = PixelToPoint(PixelValue (pAb->AbSize,
							pAb->AbSizeUnit, pAb));
		  i = (currentBodySize * LineWeight) / 10;
		  if ((currentBodySize * i) % 10 >= 5)
		     i++;
	       }
	     TtaSetNumberForm (NumZoneStrokeWeight, i);

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
		  TtaNewSelector (NumSelectPattern, NumFormPresGraphics,
				  TtaGetMessage (LIB, TMSG_FILL_PATTERN),
				  nbItems, string, i, NULL, TRUE, TRUE);
		  /* initialise le selecteur sur sa premiere entree */
		  TtaSetSelector (NumSelectPattern, pAb->AbFillPattern, "");
	       }
	     /* Toggle button Motif de remplissage standard */
	     i = 0;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewToggleMenu (NumTogglePatternUnchanged, NumFormPresGraphics,
			       NULL, 1, string, NULL, TRUE);
	     DocModPresent = pDoc;
	     TtaShowDialogue (NumFormPresGraphics, TRUE);
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
   PtrElement          pFirstSel, pLastSel;
   int                 firstChar, lastChar;
   boolean             selectionOK;
   PtrAbstractBox      pAb;
#  ifndef _WINDOWS 
   int                 i;
   char                string[MAX_TXT_LEN];
#  endif /* !_WINDOWS */
   PtrDocument         pDoc;

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

	     ResetMenus ();
#            ifndef _WINDOWS
	     /* formulaire Presentation Format */
	     TtaNewSheet (NumFormPresFormat, TtaGetViewFrame (document, view), 
			  TtaGetMessage (LIB, TMSG_FORMAT),
		  1, TtaGetMessage (LIB, TMSG_APPLY), TRUE, 3, 'L', D_DONE);

	     /* sous-menu Alignement */
	     i = 0;
	     sprintf (&string[i], "%s", "Bmiidn");	/* gauche */
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "Bmeiin");	/* droite */
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "Bmfogn");	/* Centrer */
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));	/* Inchange */
	     TtaNewSubmenu (NumMenuAlignment, NumFormPresFormat, 0,
		   TtaGetMessage (LIB, TMSG_ALIGN), 4, string, NULL, TRUE);
	     /* change la police des 3 premieres entrees */
	     for (i = 0; i < 3; i++)
		TtaRedrawMenuEntry (NumMenuAlignment, i, "icones", ThotColorNone, -1);
	     /* initialise le menu de cadrage des lignes */
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
		      default:
			 i = 1;
			 break;
		   }
	     TtaSetMenuForm (NumMenuAlignment, i - 1);

	     /* zone de saisie du renfoncement en points typo */
	     TtaNewNumberForm (NumZoneRecess, NumFormPresFormat,
			TtaGetMessage (LIB, TMSG_INDENT_PTS), 0, 300, TRUE);
	     /* initialise la valeur du renfoncement */
	     IndentValue = PixelToPoint(PixelValue (abs (pAb->AbIndent),
					pAb->AbIndentUnit, pAb));
	     TtaSetNumberForm (NumZoneRecess, IndentValue);

	     /* sous-menu sens de renfoncement */
	     i = 0;
	     sprintf (&string[i], "%s", "Bm_`an");
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "Bmb`an");
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuRecessSense, NumFormPresFormat, 0,
		   TtaGetMessage (LIB, TMSG_INDENT), 3, string, NULL, TRUE);
	     /* change la police des 3 premieres entrees du sous-menu */
	     for (i = 0; i < 2; i++)
		TtaRedrawMenuEntry (NumMenuRecessSense, i, "icones", ThotColorNone, -1);
	     /* initialise le sens de renfoncement */
	     if (pAb->AbIndent > 0)
		i = 0;
	     else
		i = 1;
	     TtaSetMenuForm (NumMenuRecessSense, i);

	     /* sous-menu de justification */
	     i = 0;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_LIB_YES));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_LIB_NO));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuJustification, NumFormPresFormat, 0,
		 TtaGetMessage (LIB, TMSG_JUSTIFY), 3, string, NULL, TRUE);
	     /* menu de justification */
	     if (pAb->AbJustify)
		i = 1;		/* avec justification */
	     else
		i = 2;		/* sans justification */
	     TtaSetMenuForm (NumMenuJustification, i - 1);

	     /* zone de saisie de l'interligne en points typo */
	     TtaNewNumberForm (NumZoneLineSpacing, NumFormPresFormat,
		  TtaGetMessage (LIB, TMSG_LINE_SPACING_PTS), 1, 200, TRUE);

	     /* sous-menu Interligne (Grandeur) */
	     i = 0;
	     sprintf (&string[i], "%s", "BmTTTn");	/* 'Normal%' */
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "BmWWWn");	/* 'Double%' */
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s", "BmZZZn");	/* 'Triple%' */
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuLineSpacing, NumFormPresFormat, 0,
	     TtaGetMessage (LIB, TMSG_LINE_SPACING), 4, string, NULL, TRUE);
	     /* change la police des 3 premieres entrees */
	     for (i = 0; i < 3; i++)
		TtaRedrawMenuEntry (NumMenuLineSpacing, i, "icones", ThotColorNone, -1);
	     /* initialise l'interligne en points typographiques */
	     OldLineSp = PixelToPoint(PixelValue (pAb->AbLineSpacing,
					  pAb->AbLineSpacingUnit, pAb));
	     TtaSetNumberForm (NumZoneLineSpacing, OldLineSp);

	     NormalLineSpacing = PixelToPoint(PixelValue (10, UnRelative, pAb));
	     /* saisie de l'interligne par un menu */
	     if (OldLineSp < (NormalLineSpacing * 3) / 2)
		i = 0;
	     else if (OldLineSp >= NormalLineSpacing * 2)
		i = 2;
	     else
		i = 1;
	     TtaSetMenuForm (NumMenuLineSpacing, i);
	     DocModPresent = pDoc;
	     TtaShowDialogue (NumFormPresFormat, TRUE);
#            else  /* _WINDOWS */
             CreateChangeFormatDlgWindow (TtaGetViewFrame (document, view));
#            endif /* _WINDOWS */
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
	ChngStyle = FALSE;
	StdStyle = FALSE;
	ChngUnderline = FALSE;
	StdUnderline = FALSE;
	ChngWeight = FALSE;
	StdWeight = FALSE;
	ChngBodySize = FALSE;
	StdBodySize = FALSE;
	ChngCadr = FALSE;
	StdCadr = FALSE;
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
