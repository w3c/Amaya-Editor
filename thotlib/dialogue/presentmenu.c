/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*

  presentmenu.c : Functions to modify the specific presentation

 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"

#include "dialog.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"

#undef EXPORT
#define EXPORT extern
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

#define SINGLE_LINESPACING 11

static PtrDocument  DocModPresent;

/* user presentation choices and their values */

static boolean      ChngFontFamily;	/* user asks to modify the font family */
static char         FontFamily;	/* font family requested by the user */
static boolean      ChngStyle;	/* user asks to modify the style */
static int          Style;	/* character style requested by the user */
static boolean      ChngUnderline;	/* user asks to modify the underline */
static int          UnderlineStyle;	/* underline style requested by the user */
static boolean      ChngWeight;	/* user asks to modify the underline weight */
static int          UnderlineWeight;	/* underline weight requested by the user */
static boolean      ChngBodySize;	/* user asks to modify the body size */
static int          BodySize;	/* body size (in points) requested by the user */
static boolean      ChngCadr;	/* user requests to modify the line alignment mode */
static int          Cadr;	/* line alignment mode */
static boolean      ChngJustif;	/* user asks to change the justification */
static boolean      Justif;	/* with or without justification */
static boolean      ChngHyphen;	/* user asks to change the hyphenation */
static boolean      Hyphenate;	/* with or without hyphenation */
static boolean      ChngIndent;	/* user asks to change the indentation */
static int          IndentValue;	/* value in points for the 1st line indentation */
static int          IndentSign;	/* the indentation sign */
static boolean      ChngLineSp;	/* user asks to change the line spacing */
static boolean      ChngLineStyle;	/* user asks to change the line style */
static char         LineStyle;	/* requested line style */
static boolean      ChngLineWeight;	/* user asks to change the line weight */
static int          LineWeight;	/* requested line weight in points */
static boolean      ChngTrame;	/* l'utilisateur veut changer la trame de remplissage */
static int          PaintWithPattern;	/* number of the requested trame */
static boolean      ChgnStandardChar;	/* standard presentation characters */
static boolean      ChngStandardColor;	/* standard presentation colors  */
static boolean      ChngStandardGraph;	/* standard presentation graphics  */
static boolean      ChngStandardForm;	/* standard presentation format  */
static boolean      ChngStandardGeom;	/* standard geometry */

static RuleSet      TheRules;
static RuleSet      GeomRules;
static int          OldLineSp;
static void         ResetMenus ();


/*----------------------------------------------------------------------
   ApplyPresentMod
   applies the presentation modifications that were requested by means
   of the Characters form, the Format form, the Graphics form, or the
   Standard Geometry entry of the Present menu.
  ----------------------------------------------------------------------*/
static void         ApplyPresentMod ()
{
   PtrElement          pEl;
   PtrDocument         pSelDoc;
   PtrElement          pFirstSel, pLastSel;
   int                 firstChar, lastChar;
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
   TypeUnit            LocLineWeightUnit;
   boolean             locChngTrame;
   boolean             locChngCadr;
   boolean             locChngJustif;
   boolean             locChngHyphen;
   boolean             locChngIndent;
   boolean             locChngLineSp;
   PtrAbstractBox      pAb;
   int                 currentBodySize;
   int                 i;
   int                 sign;

   selectionOK = GetCurrentSelection (&pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);
   if (selectionOK && pSelDoc != NULL)
      if (pSelDoc->DocSSchema != NULL)
	 /* il y a bien une selection et le document selectionne' n'a pas */
	 /* ete ferme' */
	{
	   /* eteint la selection courante */
	   ClearAllViewSelection ();
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
	   chngChars = (!ChgnStandardChar &&
			(ChngBodySize || ChngWeight
			 || ChngUnderline || ChngStyle || ChngFontFamily));
	   chngFormat = (!ChngStandardForm &&
			 (ChngCadr || ChngJustif || ChngHyphen
			  || ChngIndent || ChngLineSp));
	   chngGraphics = (!ChngStandardGraph &&
			   (ChngLineStyle
			    || ChngLineWeight
			    || ChngTrame));

	   if (ChgnStandardChar || ChngStandardColor || ChngStandardForm ||
	       chngChars || chngGraphics || (chngFormat && ChngHyphen))
	      /* changement des caracteres */
	      /* coupe les elements du debut et de la fin de la selection */
	      /* s'ils sont partiellement selectionnes */
	      if (firstChar > 1 || lastChar > 0)
		 CutSelection (pSelDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar);

	   if (chngChars || chngFormat || chngGraphics ||
	       ChgnStandardChar || ChngStandardColor || ChngStandardForm || ChngStandardGeom || ChngStandardGraph)
	      /* il y a quelque chose a changer, on parcourt la selection */
	      /* courante et on change ce qu'a demande' l'utilisateur */
	     {
		pEl = pFirstSel;
		while (pEl != NULL)
		   /* Traite l'element courant */
		  {
		     /* evalue les difference entre le pave traite' et les demandes
		        de l'utilisateur */
		     if (pEl->ElAssocNum > 0)
			pAb = AbsBoxOfEl (pEl, 1);
		     else
			pAb = AbsBoxOfEl (pEl, SelectedView);
		     if (pAb != NULL)
		       {
			  if (pAb->AbSizeUnit == UnPoint)
			     currentBodySize = pAb->AbSize;
			  else
			     currentBodySize = FontPointSize (pAb->AbSize);

			  /* famille de polices de caracteres */
			  if (ChngFontFamily)
			     locChngFontFamily = (FontFamily != pAb->AbFont);
			  else
			     locChngFontFamily = FALSE;
			  /* style des caracteres */
			  if (ChngStyle)
			     locChngStyle = (Style != pAb->AbHighlight);
			  else
			     locChngStyle = FALSE;
			  /* style du souligne */
			  if (ChngUnderline)
			     locChngUnderline = (UnderlineStyle != pAb->AbUnderline);
			  else
			     locChngUnderline = FALSE;
			  /* epaisseur du souligne */
			  if (ChngWeight)
			     locChngWeight = (UnderlineWeight != pAb->AbThickness);
			  else
			     locChngWeight = FALSE;
			  /* corps en points typo */
			  if (ChngBodySize)
			     locChngBodySize = (BodySize != currentBodySize);
			  else
			     locChngBodySize = FALSE;
			  /* alignement des lignes */
			  if (ChngCadr)
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
			  else
			     locChngCadr = FALSE;
			  /* justification */
			  if (ChngJustif)
			     locChngJustif = (Justif != pAb->AbJustify);
			  else
			     locChngJustif = FALSE;
			  /* coupure des mots */
			  if (ChngHyphen)
			     locChngHyphen = (Hyphenate != pAb->AbHyphenate);
			  else
			     locChngHyphen = FALSE;

			  /* renfoncement de la premiere ligne */
			  if (ChngIndent)
			    {
			       locChngIndent = TRUE;
			       if (pAb->AbIndent > 0)
				  sign = 1;
			       else if (pAb->AbIndent == 0)
				  sign = 0;
			       else
				  sign = -1;
			       i = abs (pAb->AbIndent);
			       if (pAb->AbIndentUnit == UnRelative)
				  /* convertit AbIndent en points typographiques */
				 {
				    i = (currentBodySize * i) / 10;
				    if ((currentBodySize * i) % 10 >= 5)
				       i++;
				 }
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
			  else
			     locChngIndent = FALSE;

			  /* interligne */
			  if (ChngLineSp)
			    {
			       locChngLineSp = TRUE;
			       i = pAb->AbLineSpacing;
			       if (pAb->AbLineSpacingUnit == UnRelative)
				  /* convertit 'interligne en points typographiques */
				 {
				    i = (currentBodySize * i) / 10;
				    if ((currentBodySize * i) % 10 >= 5)
				       i++;
				 }
			       if (OldLineSp == i)
				  locChngLineSp = FALSE;
			    }
			  else
			     locChngLineSp = FALSE;
			  /* style des traits graphiques */
			  if (ChngLineStyle)
			     locChngLineStyle = (LineStyle != pAb->AbLineStyle);
			  else
			     locChngLineStyle = FALSE;

			  /* epaisseur des traits graphiques */
			  LocLineWeightUnit = pAb->AbLineWeightUnit;
			  if (ChngLineWeight)
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

			  /* trame de remplissage */
			  if (ChngTrame)
			     locChngTrame = (PaintWithPattern != pAb->AbFillPattern);
			  else
			     ChngTrame = FALSE;
		       }

		     if (ChgnStandardChar || ChngStandardForm
			 || ChngStandardGraph || ChngStandardColor)
			RemoveSpecPresTree (pEl, pSelDoc, TheRules, SelectedView);
		     if (chngChars)
			ModifyChar (pEl, pSelDoc, SelectedView, locChngFontFamily,
				    FontFamily, locChngStyle, Style, locChngBodySize, BodySize,
				    locChngUnderline, UnderlineStyle, locChngWeight, UnderlineWeight);
		     if (chngGraphics)
			ModifyGraphics (pEl, pSelDoc, SelectedView, locChngLineStyle,
			     LineStyle, LocLineWeightUnit, LineWeight, TRUE,
					locChngTrame, PaintWithPattern, FALSE, 0,
					FALSE, 0);
		     if (chngFormat)
			ModifyLining (pEl, pSelDoc, SelectedView, locChngCadr, Cadr,
				      locChngJustif, Justif, locChngIndent, IndentValue * IndentSign,
			locChngLineSp, OldLineSp, locChngHyphen, Hyphenate);
		     if (ChngStandardGeom)
			RemoveSpecPresTree (pEl, pSelDoc, GeomRules, SelectedView);
		     /* si on est dans un element copie' par inclusion,   */
		     /* on met a jour les copies de cet element.          */
		     RedisplayCopies (pEl, pSelDoc, TRUE);
		     /* cherche l'element a traiter ensuite */
		     pEl = NextInSelection (pEl, pLastSel);
		  }
		/* fin de la boucle de parcours et traitement des */
		/* elements selectionnes */
	     }
	   /* tente de fusionner les elements voisins et reaffiche les paves */
	   /* modifie's et la selection */
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
   ChgnStandardChar = FALSE;
   ChngStandardGraph = FALSE;
   ChngStandardForm = FALSE;
   ChngStandardColor = FALSE;
   ChngStandardGeom = FALSE;
   RuleSetClr (TheRules);
   RuleSetClr (GeomRules);
   ResetMenus ();

   /* formulaire presentation standard */
   TtaNewSheet (NumFormPresentStandard, TtaGetViewFrame (document, view), 0, 0,
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
		TtaGetMessage (LIB, TMSG_STD_PRES), 5, string, NULL, FALSE);
   /* annule toutes les options du choix multiple Presentation standard */
   TtaSetToggleMenu (NumMenuPresentStandard, -1, FALSE);
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
			   ChgnStandardChar = TRUE;
			   RuleSetPut (TheRules, PtSize);
			   RuleSetPut (TheRules, PtStyle);
			   RuleSetPut (TheRules, PtFont);
			   RuleSetPut (TheRules, PtUnderline);
			   RuleSetPut (TheRules, PtThickness);
			   break;
			case 1:
			   /* graphiques standard */
			   ChngStandardGraph = TRUE;
			   RuleSetPut (TheRules, PtLineStyle);
			   RuleSetPut (TheRules, PtLineWeight);
			   RuleSetPut (TheRules, PtFillPattern);
			   break;
			case 2:
			   /* couleurs standard */
			   ChngStandardColor = TRUE;
			   RuleSetPut (TheRules, PtBackground);
			   RuleSetPut (TheRules, PtForeground);
			   break;
			case 3:
			   /* format standard */
			   ChngStandardForm = TRUE;
			   RuleSetPut (TheRules, PtIndent);
			   RuleSetPut (TheRules, PtAdjust);
			   RuleSetPut (TheRules, PtLineSpacing);
			   RuleSetPut (TheRules, PtJustify);
			   RuleSetPut (TheRules, PtHyphenate);
			   RuleSetPut (TheRules, PtFunction);
			   break;
			case 4:
			   /* standard geometry */
			   ChngStandardGeom = TRUE;
			   RuleSetPut (GeomRules, PtVertPos);
			   RuleSetPut (GeomRules, PtHorizPos);
			   RuleSetPut (GeomRules, PtHeight);
			   RuleSetPut (GeomRules, PtWidth);
			   break;
		     }
	       break;
	    case NumFormPresentStandard:
	       if (val == 1)
		  /* retour "Appliquer" du formulaire Presentation standard */
		  ApplyPresentMod ();
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
			   c = '\0';	/* inchange' */
			   break;
			default:
			   c = 'T';
			   break;
		     }
	       if (c == '\0')	/* inchange' */
		  ChngFontFamily = FALSE;
	       else
		 {
		    /* changement de famille de caracteres */
		    ChngFontFamily = TRUE;
		    FontFamily = c;
		 }
	       break;
	    case NumMenuStyleChar:	/* style des caracteres */
	       if (val == 6)	/* entree 6: Inchange' */
		  ChngStyle = FALSE;
	       else
		 {
		    ChngStyle = TRUE;
		    Style = val;
		 }
	       break;
	    case NumMenuUnderlineType:		/* style du souligne */
	       /* l'entree 2 est supprimee dans cette version */
	       if (val == 3)	/* entree 3: Inchange' */
		  ChngUnderline = FALSE;
	       else if (val == 2)	/* entree 2->3 */
		 {
		    ChngUnderline = TRUE;
		    UnderlineStyle = val + 1;
		 }
	       else
		 {
		    ChngUnderline = TRUE;
		    UnderlineStyle = val;
		 }
	       break;
	    case NumMenuUnderlineWeight:	/* epaisseur du souligne */
	       if (val == 2)	/* entree 2: Inchange' */
		  ChngLineWeight = FALSE;
	       else
		 {
		    ChngWeight = TRUE;
		    UnderlineWeight = val;
		 }
	       break;
	    case NumMenuCharFontSize:	/* menu des corps en points typo */
	       if (val >= 0 && val < NumberOfFonts ())
		 {
		    ChngBodySize = TRUE;
		    BodySize = FontPointSize (val);
		 }
	       else
		  ChngBodySize = FALSE;
	       break;
	    case NumMenuAlignment:	/* alignement des lignes */
	       if (val == 3)	/* entree 3: Inchange' */
		  ChngCadr = FALSE;
	       else
		 {
		    ChngCadr = TRUE;
		    Cadr = val + 1;
		 }
	       break;
	    case NumMenuJustification:		/* justification */
	       if (val == 2)	/* entree 2: Inchange' */
		  ChngJustif = FALSE;
	       else
		 {
		    ChngJustif = TRUE;
		    Justif = (val == 0);
		 }
	       break;
	    case NumMenuWordBreak:	/* coupure des mots */
	       if (val == 2)	/* entree 2: Inchange' */
		  ChngHyphen = FALSE;
	       else
		 {
		    ChngHyphen = TRUE;
		    Hyphenate = (val == 0);
		 }
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
	       break;
	    case NumMenuRecessSense:	/* sens du renfoncement de la premiere ligne */
	       if (val == 2)	/* entree 2: Inchange' */
		  ChngIndent = FALSE;
	       else
		 {
		    if (val == 0)
		       IndentSign = 1;
		    else
		       IndentSign = 0;
		    ChngIndent = TRUE;
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
	       break;
	    case NumZoneLineSpacing:	/* interligne */
	       ChngLineSp = TRUE;
	       if (OldLineSp != val)
		 {
		    OldLineSp = val;
		    if (val <= (SINGLE_LINESPACING * 3) / 2)
		       i = 0;
		    else if (val >= SINGLE_LINESPACING * 2)
		       i = 2;
		    else
		       i = 1;
		    TtaSetMenuForm (NumMenuLineSpacing, i);
		 }
	       break;
	    case NumMenuLineSpacing:	/* saisie de l'interligne par un menu */
	       if (val == 3)	/* entree 3: Inchange' */
		  ChngLineSp = FALSE;
	       else
		 {
		    ChngLineSp = TRUE;
		    /* l'utilisateur demande a changer l'interligne */
		    OldLineSp = ((val + 2) * SINGLE_LINESPACING) / 2;
		    TtaSetNumberForm (NumZoneLineSpacing, OldLineSp);
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
			   c = '\0';	/* inchange' */
			default:
			   c = 'S';	/* trait continu */
			   break;
		     }
	       if (c == '\0')	/* inchange' */
		  ChngLineStyle = FALSE;
	       else
		 {
		    /* changement effectif de style de trait */
		    ChngLineStyle = TRUE;
		    LineStyle = c;
		 }
	       break;
	    case NumZoneStrokeWeight:
	       ChngLineWeight = TRUE;
	       LineWeight = val;
	       break;
	    case NumToggleWidthUnchanged:
	       ChngLineWeight = FALSE;
	       break;
	    case NumSelectPattern:
	       i = PatternNumber (txt);
	       if (i >= 0)
		 {
		    ChngTrame = TRUE;
		    PaintWithPattern = i;
		 }
	       break;
	    case NumTogglePatternUnchanged:
	       ChngTrame = FALSE;
	       break;
	    case NumFormPresGraphics:
	    case NumFormPresChar:
	    case NumFormPresFormat:
	       /* le formulaire Format lui-meme */
	       if (val > 0)
		  ApplyPresentMod ();
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
   int                 i, nbItems;
   int                 max, bodyRelatSize, bodyPointSize;
   PtrDocument         pSelDoc;
   PtrElement          pFirstSel, pLastSel;
   int                 firstChar, lastChar;
   boolean             selectionOK;
   PtrAbstractBox      pAb;
   PtrElement          pEl;
   char                string[MAX_TXT_LEN];
   char               *s;
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

	     /* formulaire Presentation Caracteres */
	     TtaNewSheet (NumFormPresChar, TtaGetViewFrame (document, view), 0, 0,
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
	     TtaGetMessage (LIB, TMSG_FONT_FAMILY), 4, string, NULL, FALSE);

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
		   TtaGetMessage (LIB, TMSG_STYLE), 7, string, NULL, FALSE);

	     /* sous-menu type de Souligne */
	     i = 0;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_NORMAL));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNDERLINE));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_CROSS_OUT));
	     i += strlen (&string[i]) + 1;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewSubmenu (NumMenuUnderlineType, NumFormPresChar, 0,
		    TtaGetMessage (LIB, TMSG_LINE), 4, string, NULL, FALSE);
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
			    TtaGetMessage (LIB, TMSG_BODY_SIZE_PTS), nbItems, string, NULL, FALSE);
	     /* initialise la zone 'Famille de caracteres' */
	     switch (pAb->AbFont)
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
			 i = 0;
			 break;
		   }
	     TtaSetMenuForm (NumMenuCharFamily, i - 1);
	     /* initialise le catalogue 'Style des caracteres' */
	     TtaSetMenuForm (NumMenuStyleChar, pAb->AbHighlight);
	     TtaSetMenuForm (NumMenuUnderlineType, pAb->AbUnderline);
	     /* initialise le catalogue 'Epaisseur du souligne' */
	     TtaSetMenuForm (NumMenuUnderlineWeight, pAb->AbThickness);
	     /* initialise le sous-menu 'Corps des caracteres' */
	     if (pAb->AbSizeUnit == UnPoint)
		/* convertit la taille */
		i = FontRelSize (pAb->AbSize);
	     else
		i = pAb->AbSize;
	     TtaSetMenuForm (NumMenuCharFontSize, i);
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
	     TtaNewSheet (NumFormPresGraphics, TtaGetViewFrame (document, view), 0, 0,
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
	      TtaGetMessage (LIB, TMSG_LINE_STYLE), 4, string, NULL, FALSE);
	     /* change la police des 3 premieres entrees du style des traits */
	     for (i = 0; i < 3; i++)
		TtaRedrawMenuEntry (NumMenuStrokeStyle, i, "icones", ThotColorNone, -1);
	     /* initialise le sous-menu style des traits */
	     switch (pAb->AbLineStyle)
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
		       TtaGetMessage (LIB, TMSG_LINE_WEIGHT), 0, 72, FALSE);
	     TtaSetNumberForm (NumZoneStrokeWeight, 1);
	     /* Toggle button Epaisseur des traits inchange'e */
	     sprintf (string, "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewToggleMenu (NumToggleWidthUnchanged, NumFormPresGraphics,
			       NULL, 1, string, NULL, FALSE);
	     /* initialise la zone de saisie epaisseur des traits */
	     if (pAb->AbLineWeightUnit == UnPoint)
		i = pAb->AbLineWeight;
	     else
	       {
		  if (pAb->AbSizeUnit == UnPoint)
		     currentBodySize = pAb->AbSize;
		  else
		     currentBodySize = FontPointSize (pAb->AbSize);
		  i = (currentBodySize * pAb->AbLineWeight) / 10;
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
				  nbItems, string, i, NULL, TRUE, FALSE);
		  /* initialise le selecteur sur sa premiere entree */
		  TtaSetSelector (NumSelectPattern, pAb->AbFillPattern, "");
	       }
	     /* Toggle button Motif de remplissage inchange' */
	     i = 0;
	     sprintf (&string[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_UNCHANGED));
	     TtaNewToggleMenu (NumTogglePatternUnchanged, NumFormPresGraphics,
			       NULL, 1, string, NULL, FALSE);
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
   int                 i;
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

	     ResetMenus ();
	     /* formulaire Presentation Format */
	     TtaNewSheet (NumFormPresFormat, TtaGetViewFrame (document, view), 0, 0,
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
		   TtaGetMessage (LIB, TMSG_ALIGN), 4, string, NULL, FALSE);
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
	     if (pAb->AbSizeUnit == UnPoint)
		currentBodySize = pAb->AbSize;
	     else
		currentBodySize = FontPointSize (pAb->AbSize);
	     IndentValue = abs (pAb->AbIndent);
	     if (pAb->AbIndentUnit == UnRelative)
		/* convertit IndentValue en points typographiques */
	       {
		  IndentValue = (currentBodySize * IndentValue) / 10;
		  if ((currentBodySize * IndentValue) % 10 >= 5)
		     IndentValue++;
	       }
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
		 TtaGetMessage (LIB, TMSG_JUSTIFY), 3, string, NULL, FALSE);
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
	     if (pAb->AbSizeUnit == UnPoint)
		currentBodySize = pAb->AbSize;
	     else
		currentBodySize = FontPointSize (pAb->AbSize);
	     i = pAb->AbLineSpacing;
	     if (pAb->AbLineSpacingUnit == UnRelative)
		/* convertit l'interligne en points typographiques */
	       {
		  i = (currentBodySize * i) / 10;
		  if ((currentBodySize * i) % 10 >= 5)
		     i++;
	       }
	     OldLineSp = i;
	     TtaSetNumberForm (NumZoneLineSpacing, i);

	     /* saisie de l'interligne par un menu */
	     if (OldLineSp <= (SINGLE_LINESPACING * 3) / 2)
		i = 0;
	     else if (pAb->AbLineSpacing >= SINGLE_LINESPACING * 2)
		i = 2;
	     else
		i = 1;
	     TtaSetMenuForm (NumMenuLineSpacing, i);
	     DocModPresent = pDoc;
	     TtaShowDialogue (NumFormPresFormat, TRUE);
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
     }
   ChgnStandardChar = FALSE;
   ChngStandardColor = FALSE;
   ChngStandardGraph = FALSE;
   ChngStandardGeom = FALSE;
   ChngStandardForm = FALSE;
   ChngLineStyle = FALSE;
   ChngLineWeight = FALSE;
   ChngTrame = FALSE;
   ChngFontFamily = FALSE;
   ChngUnderline = FALSE;
   ChngWeight = FALSE;
   ChngStyle = FALSE;
   ChngBodySize = FALSE;
   ChngCadr = FALSE;
   ChngJustif = FALSE;
   ChngHyphen = FALSE;
   ChngIndent = FALSE;
   ChngLineSp = FALSE;
}
/* End Of Module modpres */
