
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   modpres.c -- Modifications de la presentation specifique.
   Modifications de la presentation specifique.
   V. Quint     Mai 1992
   Major changes:
   I. Vatton Mai 1996
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

#define INTERLGN_SIMPLE 11

static PtrDocument  DocModPresent;
/* Les choix effectues par l'utilisateur */
static boolean      ChngFamille;	/* l'utilisateur demande a modifier la famille */
static char         Famille;	/* famille de police demandee par l'utilisateur */
static boolean      ChngStyle;	/* l'utilisateur demande a modifier le style */
static int          Style;	/* style de caractere demande par l'utilisateur */
static boolean      ChngSouligne;	/* l'utilisateur demande a modifier le souligne */
static boolean      ChngEpais;	/* demande a modifier l'epaisseur du souligne */
static int          SoulStyle;	/* style de souligne demande par l'utilisateur */
static int          SoulEpais;	/* epaisseur de souligne demande par l'utilisateur */
static boolean      ChngCorps;	/* l'utilisateur demande a modifier le corps */
static int          Corps;	/* corps demande par l'utilisateur en pt */
static boolean      ChngCadr;	/* l'utilisateur demande a modifier le cadrage */
static int          Cadr;	/* mode de cadrage des lignes */
static boolean      ChngJustif;	/* l'utilisateur demande a modifier la justif. */
static boolean      Justif;	/* avec ou sans justification */
static boolean      ChngHyphen;	/* l'utilisateur demande a modifier la coupure des mots */
static boolean      Hyphenate;	/* avec ou sans coupure de mots */
static boolean      ChngIndent;	/* l'utilisateur demande a modifier le renfonc */
static int          ValIndent;	/* valeur du renfoncement de la 1ere ligne en pt */
static int          SigneIndent;	/* Le signe du renfoncement */
static boolean      ChngInterL;	/* l'utilisateur demande a modifier l'interligne */
static boolean      ChngStyleTrait;	/* l'utilisateur veut changer le style des traits */
static char         StyleTrait;	/* style de trait demnande' */
static boolean      ChngEpaisTrait;	/* l'utilisateur veut changer l'epaisseur des traits */
static int          EpaisTrait;	/* epaisseur de trait demandee en pt */
static boolean      ChngTrame;	/* l'utilisateur veut changer la trame de remplissage */
static int          Trame;	/* numero de la trame demandee */
static boolean      ChngCarStandard;	/* presentation caracteres standard */
static boolean      ChngCoulStandard;	/* presentation couleurs standard */
static boolean      ChngGraphStandard;	/* presentation graphiques standard */
static boolean      ChngFormStandard;	/* presentation format standard */
static boolean      ChngGeomStandard;	/* geometrie standard */

static RuleSet      LesRegles;
static RuleSet      ReglesGeom;
static int          oldinterligne;
static void         RazRetoursMenus ();


/* ---------------------------------------------------------------------- */
/* | AppliqueModPresent applique les modifications de presentation      | */
/* |    demandees par le formulaire Caracteres, le formulaire Format,   | */
/* |    le formulaire Graphiques, ou l'entree Geometrie Standard du     | */
/* |    menu Presenter.                                                 | */
/* ---------------------------------------------------------------------- */
static void         AppliqueModPresent ()
{
   PtrElement          pEl;
   PtrDocument         SelDoc;
   PtrElement          PremSel, DerSel;
   int                 premcar, dercar;
   boolean             selok;
   boolean             ChngCaracteres;
   boolean             ChngFormat;
   boolean             ChngGraphiques;
   boolean             LocChngFamille;
   boolean             LocChngStyle;
   boolean             LocChngCorps;
   boolean             LocChngSouligne;
   boolean             LocChngEpais;
   boolean             LocChngStyleTrait;
   TypeUnit            LocEpaisTraitUnit;
   boolean             LocChngTrame;
   boolean             LocChngCadr;
   boolean             LocChngJustif;
   boolean             LocChngHyphen;
   boolean             LocChngIndent;
   boolean             LocChngInterL;
   PtrAbstractBox             pAb;
   int                 CorpsCourant;
   int                 i;
   int                 Signe;

   selok = GetCurrentSelection (&SelDoc, &PremSel, &DerSel, &premcar, &dercar);
   if (selok && SelDoc != NULL)
      if (SelDoc->DocSSchema != NULL)
	 /* il y a bien une selection et le document selectionne' n'a pas */
	 /* ete ferme' */
	{
	   /* eteint la selection courante */
	   ClearAllViewSelection ();
	   /* si une chaine de caracteres complete est selectionnee, */
	   /* selectionne l'element TEXTE */
	   if (PremSel->ElTerminal && PremSel->ElLeafType == LtText &&
	       premcar <= 1)
	      if (DerSel != PremSel ||
		  (PremSel == DerSel && dercar > PremSel->ElTextLength))
		 premcar = 0;
	   if (DerSel->ElTerminal && DerSel->ElLeafType == LtText &&
	       dercar > DerSel->ElTextLength)
	      if (DerSel != PremSel || (PremSel == DerSel && premcar == 0))
		 dercar = 0;
	   ChngCaracteres = (!ChngCarStandard &&
			     (ChngCorps || ChngEpais
			      || ChngSouligne || ChngStyle || ChngFamille));
	   ChngFormat = (!ChngFormStandard &&
			 (ChngCadr || ChngJustif || ChngHyphen
			  || ChngIndent || ChngInterL));
	   ChngGraphiques = (!ChngGraphStandard &&
			     (ChngStyleTrait
			      || ChngEpaisTrait
			      || ChngTrame));

	   if (ChngCarStandard || ChngCoulStandard || ChngFormStandard ||
	     ChngCaracteres || ChngGraphiques || (ChngFormat && ChngHyphen))
	      /* changement des caracteres */
	      /* coupe les elements du debut et de la fin de la selection */
	      /* s'ils sont partiellement selectionnes */
	      if (premcar > 1 || dercar > 0)
		 CutSelection (SelDoc, &PremSel, &DerSel, &premcar, &dercar);

	   if (ChngCaracteres || ChngFormat || ChngGraphiques ||
	       ChngCarStandard || ChngCoulStandard || ChngFormStandard || ChngGeomStandard || ChngGraphStandard)
	      /* il y a quelque chose a changer, on parcourt la selection */
	      /* courante et on change ce qu'a demande' l'utilisateur */
	     {
		pEl = PremSel;
		while (pEl != NULL)
		   /* Traite l'element courant */
		  {
		     /* evalue les difference entre le pave traite' et les demandes
		        de l'utilisateur */
		     if (pEl->ElAssocNum > 0)
			pAb = PaveDeElem (pEl, 1);
		     else
			pAb = PaveDeElem (pEl, SelectedView);
		     if (pAb != NULL)
		       {
			  if (pAb->AbSizeUnit == UnPoint)
			     CorpsCourant = pAb->AbSize;
			  else
			     CorpsCourant = FontPointSize (pAb->AbSize);

			  /* famille de polices de caracteres */
			  if (ChngFamille)
			     LocChngFamille = (Famille != pAb->AbFont);
			  else
			     LocChngFamille = FALSE;
			  /* style des caracteres */
			  if (ChngStyle)
			     LocChngStyle = (Style != pAb->AbHighlight);
			  else
			     LocChngStyle = FALSE;
			  /* style du souligne */
			  if (ChngSouligne)
			     LocChngSouligne = (SoulStyle != pAb->AbUnderline);
			  else
			     LocChngSouligne = FALSE;
			  /* epaisseur du souligne */
			  if (ChngEpais)
			     LocChngEpais = (SoulEpais != pAb->AbThickness);
			  else
			     LocChngEpais = FALSE;
			  /* corps en points typo */
			  if (ChngCorps)
			     LocChngCorps = (Corps != CorpsCourant);
			  else
			     LocChngCorps = FALSE;
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
			       LocChngCadr = (i != Cadr);
			    }
			  else
			     LocChngCadr = FALSE;
			  /* justification */
			  if (ChngJustif)
			     LocChngJustif = (Justif != pAb->AbJustify);
			  else
			     LocChngJustif = FALSE;
			  /* coupure des mots */
			  if (ChngHyphen)
			     LocChngHyphen = (Hyphenate != pAb->AbHyphenate);
			  else
			     LocChngHyphen = FALSE;

			  /* renfoncement de la premiere ligne */
			  if (ChngIndent)
			    {
			       LocChngIndent = TRUE;
			       if (pAb->AbIndent > 0)
				  Signe = 1;
			       else if (pAb->AbIndent == 0)
				  Signe = 0;
			       else
				  Signe = -1;
			       i = abs (pAb->AbIndent);
			       if (pAb->AbIndentUnit == UnRelative)
				  /* convertit AbIndent en points typographiques */
				 {
				    i = (CorpsCourant * i) / 10;
				    if ((CorpsCourant * i) % 10 >= 5)
				       i++;
				 }
			       if (Signe == SigneIndent && i == ValIndent)
				  /* pas de changement */
				  LocChngIndent = FALSE;
			       else
				 {
				    if (SigneIndent != 0 && ValIndent == 0)
				       ValIndent = 15;
				    TtaSetNumberForm (NumZoneRecess, ValIndent);
				 }
			    }
			  else
			     LocChngIndent = FALSE;

			  /* interligne */
			  if (ChngInterL)
			    {
			       LocChngInterL = TRUE;
			       i = pAb->AbLineSpacing;
			       if (pAb->AbLineSpacingUnit == UnRelative)
				  /* convertit 'interligne en points typographiques */
				 {
				    i = (CorpsCourant * i) / 10;
				    if ((CorpsCourant * i) % 10 >= 5)
				       i++;
				 }
			       if (oldinterligne == i)
				  LocChngInterL = FALSE;
			    }
			  else
			     LocChngInterL = FALSE;
			  /* style des traits graphiques */
			  if (ChngStyleTrait)
			     LocChngStyleTrait = (StyleTrait != pAb->AbLineStyle);
			  else
			     LocChngStyleTrait = FALSE;

			  /* epaisseur des traits graphiques */
			  LocEpaisTraitUnit = pAb->AbLineWeightUnit;
			  if (ChngEpaisTrait)
			    {
			       if (pAb->AbLineWeightUnit == UnPoint)
				  i = pAb->AbLineWeight;
			       else
				 {
				    i = (CorpsCourant * pAb->AbLineWeight) / 10;
				    if ((CorpsCourant * i) % 10 >= 5)
				       i++;
				 }
			       if (EpaisTrait != i)
				  LocEpaisTraitUnit = UnPoint;
			    }

			  /* trame de remplissage */
			  if (ChngTrame)
			     LocChngTrame = (Trame != pAb->AbFillPattern);
			  else
			     ChngTrame = FALSE;
		       }

		     if (ChngCarStandard || ChngFormStandard
			 || ChngGraphStandard || ChngCoulStandard)
			SupprPresSpec (pEl, SelDoc, LesRegles, SelectedView);
		     if (ChngCaracteres)
			ModifCaracteres (pEl, SelDoc, SelectedView, LocChngFamille,
			  Famille, LocChngStyle, Style, LocChngCorps, Corps,
			LocChngSouligne, SoulStyle, LocChngEpais, SoulEpais);
		     if (ChngGraphiques)
			ModifGraphiques (pEl, SelDoc, SelectedView, LocChngStyleTrait,
			    StyleTrait, LocEpaisTraitUnit, EpaisTrait, TRUE,
					 LocChngTrame, Trame, FALSE, 0,
					 FALSE, 0);
		     if (ChngFormat)
			ModifLignes (pEl, SelDoc, SelectedView, LocChngCadr, Cadr,
				     LocChngJustif, Justif, LocChngIndent, ValIndent * SigneIndent,
				     LocChngInterL, oldinterligne, LocChngHyphen, Hyphenate);
		     if (ChngGeomStandard)
			SupprPresSpec (pEl, SelDoc, ReglesGeom, SelectedView);
		     /* si on est dans un element copie' par inclusion,   */
		     /* on met a jour les copies de cet element.          */
		     RedisplayCopies (pEl, SelDoc, TRUE);
		     /* cherche l'element a traiter ensuite */
		     pEl = NextInSelection (pEl, DerSel);
		  }
		/* fin de la boucle de parcours et traitement des */
		/* elements selectionnes */
	     }
	   /* tente de fusionner les elements voisins et reaffiche les paves */
	   /* modifie's et la selection */
	   MergeAndSelect (SelDoc, PremSel, DerSel, premcar, dercar);
	}
}

/* ---------------------------------------------------------------------- */
/* | ModPresentStandard traite le retour de l'entree Presentation       | */
/* |    Standard du menu Presenter.                                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcStandardPresentation (Document document, View view)
#else  /* __STDC__ */
void                TtcStandardPresentation (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i;
   char                chaine[200];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];
   ChngCarStandard = FALSE;
   ChngGraphStandard = FALSE;
   ChngFormStandard = FALSE;
   ChngCoulStandard = FALSE;
   ChngGeomStandard = FALSE;
   RuleSetClr (LesRegles);
   RuleSetClr (ReglesGeom);
   RazRetoursMenus ();

   /* formulaire presentation standard */
   TtaNewSheet (NumFormPresentStandard, TtaGetViewFrame (document, view), 0, 0,
		TtaGetMessage (LIB, STD_PRES),
		1, TtaGetMessage (LIB, APPLY), TRUE, 1, 'L', D_DONE);

   /* choix multiple presentation standard */
   i = 0;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, STD_CHAR));
   i += strlen (&chaine[i]) + 1;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, STD_GRAPHICS));
   i += strlen (&chaine[i]) + 1;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, STD_COLORS));
   i += strlen (&chaine[i]) + 1;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, STD_FORMAT));
   i += strlen (&chaine[i]) + 1;
   sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, STD_GEOMETRY));
   TtaNewToggleMenu (NumMenuPresentStandard, NumFormPresentStandard,
    TtaGetMessage (LIB, STD_PRES), 5, chaine, NULL, FALSE);
   /* annule toutes les options du choix multiple Presentation standard */
   TtaSetToggleMenu (NumMenuPresentStandard, -1, FALSE);
   /* active le formulaire "Presentation standard" */
   DocModPresent = pDoc;
   TtaShowDialogue (NumFormPresentStandard, TRUE);
}

/* ---------------------------------------------------------------------- */
/* | CallbackStdPresMenu traite le retour du popup Presentation      | */
/* |    Standard.                                                       | */
/* ---------------------------------------------------------------------- */
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
			   ChngCarStandard = TRUE;
			   RuleSetPut (LesRegles, PtSize);
			   RuleSetPut (LesRegles, PtStyle);
			   RuleSetPut (LesRegles, PtFont);
			   RuleSetPut (LesRegles, PtUnderline);
			   RuleSetPut (LesRegles, PtThickness);
			   break;
			case 1:
			   /* graphiques standard */
			   ChngGraphStandard = TRUE;
			   RuleSetPut (LesRegles, PtLineStyle);
			   RuleSetPut (LesRegles, PtLineWeight);
			   RuleSetPut (LesRegles, PtFillPattern);
			   break;
			case 2:
			   /* couleurs standard */
			   ChngCoulStandard = TRUE;
			   RuleSetPut (LesRegles, PtBackground);
			   RuleSetPut (LesRegles, PtForeground);
			   break;
			case 3:
			   /* format standard */
			   ChngFormStandard = TRUE;
			   RuleSetPut (LesRegles, PtIndent);
			   RuleSetPut (LesRegles, PtAdjust);
			   RuleSetPut (LesRegles, PtLineSpacing);
			   RuleSetPut (LesRegles, PtJustify);
			   RuleSetPut (LesRegles, PtHyphenate);
			   RuleSetPut (LesRegles, PtFunction);
			   break;
			case 4:
			   /* geometrie standard */
			   ChngGeomStandard = TRUE;
			   RuleSetPut (ReglesGeom, PtVertPos);
			   RuleSetPut (ReglesGeom, PtHorizPos);
			   RuleSetPut (ReglesGeom, PtHeight);
			   RuleSetPut (ReglesGeom, PtWidth);
			   break;
		     }
	       break;
	    case NumFormPresentStandard:
	       TtaDestroyDialogue (NumFormPresentStandard);
	       if (val == 1)
		  /* retour "Appliquer" du formulaire Presentation standard */
		  AppliqueModPresent ();
	       break;
	 }

}

/* ---------------------------------------------------------------------- */
/* | CallbackPresMenu traite les retours des formulaires de Presentation  | */
/* ---------------------------------------------------------------------- */
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
	    case NumMenuCharFamily:		/* famille de polices de caracteres */
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
		  ChngFamille = FALSE;
	       else
		 {
		    /* changement de famille de caracteres */
		    ChngFamille = TRUE;
		    Famille = c;
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
	    case NumMenuUnderlineType:	/* style du souligne */
	       /* l'entree 2 est supprimee dans cette version */
	       if (val == 3)	/* entree 3: Inchange' */
		  ChngSouligne = FALSE;
	       else if (val == 2)	/* entree 2->3 */
		 {
		    ChngSouligne = TRUE;
		    SoulStyle = val + 1;
		 }
	       else
		 {
		    ChngSouligne = TRUE;
		    SoulStyle = val;
		 }
	       break;
	    case NumMenuUnderlineWeight:		/* epaisseur du souligne */
	       if (val == 2)	/* entree 2: Inchange' */
		  ChngEpais = FALSE;
	       else
		 {
		    ChngEpais = TRUE;
		    SoulEpais = val;
		 }
	       break;
	    case NumMenuCharFontSize:	/* menu des corps en points typo */
	       if (val >= 0 && val < NumberOfFonts ())
		 {
		    ChngCorps = TRUE;
		    Corps = FontPointSize (val);
		 }
	       else
		  ChngCorps = FALSE;
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
	       ValIndent = val;
	       if (SigneIndent != 0 && ValIndent == 0)
		 {
		    SigneIndent = 0;
		    TtaSetMenuForm (NumMenuRecessSense, 1);
		 }
	       else if (SigneIndent == 0 && ValIndent != 0)
		 {
		    SigneIndent = 1;
		    TtaSetMenuForm (NumMenuRecessSense, 0);
		 }
	       break;
	    case NumMenuRecessSense:	/* sens du renfoncement de la premiere ligne */
	       if (val == 2)	/* entree 2: Inchange' */
		  ChngIndent = FALSE;
	       else
		 {
		    if (val == 0)
		       SigneIndent = 1;
		    else
		       SigneIndent = 0;
		    ChngIndent = TRUE;
		    if (SigneIndent != 0 && ValIndent == 0)
		      {
			 ValIndent = 15;
			 TtaSetNumberForm (NumZoneRecess, 15);
		      }
		    else if (SigneIndent == 0 && ValIndent != 0)
		      {
			 ValIndent = 0;
			 TtaSetNumberForm (NumZoneRecess, 0);
		      }
		 }
	       break;
	    case NumZoneLineSpacing:	/* interligne */
	       ChngInterL = TRUE;
	       if (oldinterligne != val)
		 {
		    oldinterligne = val;
		    if (val <= (INTERLGN_SIMPLE * 3) / 2)
		       i = 0;
		    else if (val >= INTERLGN_SIMPLE * 2)
		       i = 2;
		    else
		       i = 1;
		    TtaSetMenuForm (NumMenuLineSpacing, i);
		 }
	       break;
	    case NumMenuLineSpacing:	/* saisie de l'interligne par un menu */
	       if (val == 3)	/* entree 3: Inchange' */
		  ChngInterL = FALSE;
	       else
		 {
		    ChngInterL = TRUE;
		    /* l'utilisateur demande a changer l'interligne */
		    oldinterligne = ((val + 2) * INTERLGN_SIMPLE) / 2;
		    TtaSetNumberForm (NumZoneLineSpacing, oldinterligne);
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
		  ChngStyleTrait = FALSE;
	       else
		 {
		    /* changement effectif de style de trait */
		    ChngStyleTrait = TRUE;
		    StyleTrait = c;
		 }
	       break;
	    case NumZoneStrokeWeight:
	       ChngEpaisTrait = TRUE;
	       EpaisTrait = val;
	       break;
	    case NumToggleWidthUnchanged:
	       ChngEpaisTrait = FALSE;
	       break;
	    case NumSelectPattern:
	       i = PatternNumber (txt);
	       if (i >= 0)
		 {
		    ChngTrame = TRUE;
		    Trame = i;
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
		  AppliqueModPresent ();
	       else
		  TtaDestroyDialogue (ref);
	       break;
	    default:
	       break;
	 }
}


/* ---------------------------------------------------------------------- */
/* |    ModPresentCaracteres    L'utilisateur demande a modifier la     | */
/* |            presentation specifique des caracteres pour la vue      | */
/* |            Vue du document pDoc (si Assoc = FALSE) ou les elements | */
/* |            associes de numero Vue (si Assoc = Vrai)                | */
/* |            Initialise et active le formulaire correspondant.       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcChangeCharacters (Document document, View view)
#else  /* __STDC__ */
void                TtcChangeCharacters (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i, nbitem;
   int                 max, CorpsRelat, CorpsPt;
   PtrDocument         SelDoc;
   PtrElement          PremSel, DerSel;
   int                 premcar, dercar;
   boolean             selok;
   PtrAbstractBox             pAb;
   PtrElement          pEl;
   char                chaine[MAX_TXT_LEN];
   char               *s;
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];

   /* demande quelle est la selection courante */
   selok = GetCurrentSelection (&SelDoc, &PremSel, &DerSel, &premcar, &dercar);
   if (!selok)
     {
	/* par defaut la racine du document */
	SelDoc = pDoc;
	PremSel = pDoc->DocRootElement;
	DerSel = PremSel;
	selok = TRUE;
     }

   if (selok && SelDoc == pDoc)
      /* il y a une selection */
     {
	/* cherche si l'un des elements selectionne's est protege' */
	pEl = PremSel;
	selok = TRUE;
	while (selok && pEl != NULL)
	   if (ElementIsReadOnly (pEl))
	      selok = FALSE;
	   else
	      pEl = NextInSelection (pEl, DerSel);
     }

   if (selok)
      /* pas d'element protege', on peut modifier la presentation */
     {
	if (view > 100)
	   pAb = PaveDeElem (PremSel, 1);
	else
	   pAb = PaveDeElem (PremSel, view);

	if (pAb != NULL)
	  {
	     /* annule les etats memorises */
	     RazRetoursMenus ();

	     /* formulaire Presentation Caracteres */
	     TtaNewSheet (NumFormPresChar, TtaGetViewFrame (document, view), 0, 0,
			  TtaGetMessage (LIB, CHAR),
		  1, TtaGetMessage (LIB, APPLY), FALSE, 2, 'L', D_DONE);
	     /* sous-menu Famille de caracteres */
	     i = 0;
	     sprintf (&chaine[i], "%s", "BTimes");
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s", "BHelvetica");
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s", "BCourier");
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     TtaNewSubmenu (NumMenuCharFamily, NumFormPresChar, 0,
	      TtaGetMessage (LIB, FONT_FAMILY), 4, chaine, NULL, FALSE);

	     /* sous-menu style de caracteres */
	     i = 0;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, ROMAN));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, BOLD));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, ITALIC));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, OBLIQUE));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, BOLD_ITALIC));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, BOLD_OBLIQUE));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     TtaNewSubmenu (NumMenuStyleChar, NumFormPresChar, 0,
		    TtaGetMessage (LIB, STYLE), 7, chaine, NULL, FALSE);

	     /* sous-menu type de Souligne */
	     i = 0;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, NORMAL));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNDERLINE));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, CROSS_OUT));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     TtaNewSubmenu (NumMenuUnderlineType, NumFormPresChar, 0,
		     TtaGetMessage (LIB, LINE), 4, chaine, NULL, FALSE);
	     TtaNewLabel (NumMenuUnderlineWeight, NumFormPresChar, " ");
	     /* sous-menus des corps disponibles, en points typographiques */
	     nbitem = 0;
	     i = 0;
	     max = NumberOfFonts ();
	     s = TtaGetMessage (LIB, TYPOGRAPHIC_POINTS);
	     /* demande au mediateur la valeur en points typographiques de tous */
	     /* les corps relatifs successivement */
	     for (CorpsRelat = 0; CorpsRelat < max && i < MAX_TXT_LEN - 30; CorpsRelat++)
	       {
		  CorpsPt = FontPointSize (CorpsRelat);
		  /* ajoute ce nouveau corps dans le buffer du menu */
		  sprintf (&chaine[i], "%s%d %s", "B", CorpsPt, s);
		  i += strlen (&chaine[i]) + 1;
		  nbitem++;
	       }
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     nbitem++;
	     TtaNewSubmenu (NumMenuCharFontSize, NumFormPresChar, 0,
			    TtaGetMessage (LIB, BODY_SIZE_PTS), nbitem, chaine, NULL, FALSE);
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

/* ---------------------------------------------------------------------- */
/* |    ModPresentGraphiques    L'utilisateur demande a modifier la     | */
/* |            presentation specifique des graphiques pour la vue      | */
/* |            Vue du document pDoc (si Assoc = FALSE) ou les elements | */
/* |            associes de numero Vue (si Assoc = Vrai)                | */
/* |            Initialise et active le formulaire correspondant.       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcChangeGraphics (Document document, View view)
#else  /* __STDC__ */
void                TtcChangeGraphics (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i, nbitem;
   PtrDocument         SelDoc;
   PtrElement          PremSel, DerSel;
   int                 premcar, dercar;
   boolean             selok;
   PtrAbstractBox             pAb;
   int                 CorpsCourant;
   char                chaine[MAX_TXT_LEN];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];

   /* demande quelle est la selection courante */
   selok = GetCurrentSelection (&SelDoc, &PremSel, &DerSel, &premcar, &dercar);
   if (!selok)
     {
	/* par defaut la racine du document */
	SelDoc = pDoc;
	PremSel = pDoc->DocRootElement;
	selok = TRUE;
     }

   if (selok && SelDoc == pDoc)
      /* il y a une selection */
     {
	/* recherche le pave concerne */
	if (view > 100)
	   pAb = PaveDeElem (PremSel, 1);
	else
	   pAb = PaveDeElem (PremSel, view);

	if (pAb != NULL)
	  {
	     /* annule les etats memorises */
	     RazRetoursMenus ();

	     /* feuille de dialogue Presentation Graphiques */
	     TtaNewSheet (NumFormPresGraphics, TtaGetViewFrame (document, view), 0, 0,
			  TtaGetMessage (LIB, GRAPHICS_PRES),
		  1, TtaGetMessage (LIB, APPLY), FALSE, 3, 'L', D_DONE);

	     /* sous-menu style des traits */
	     i = 0;
	     sprintf (&chaine[i], "%s", "Bsssss");	/* Traits_continu */
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s", "Bttttt");	/* Traits_tirete */
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s", "Buuuuu");	/* Traits_pointilles */
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     TtaNewSubmenu (NumMenuStrokeStyle, NumFormPresGraphics, 0,
	       TtaGetMessage (LIB, LINE_STYLE), 4, chaine, NULL, FALSE);
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
			TtaGetMessage (LIB, LINE_WEIGHT), 0, 72, FALSE);
	     TtaSetNumberForm (NumZoneStrokeWeight, 1);
	     /* Toggle button Epaisseur des traits inchange'e */
	     sprintf (chaine, "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     TtaNewToggleMenu (NumToggleWidthUnchanged, NumFormPresGraphics,
			       NULL, 1, chaine, NULL, FALSE);
	     /* initialise la zone de saisie epaisseur des traits */
	     if (pAb->AbLineWeightUnit == UnPoint)
		i = pAb->AbLineWeight;
	     else
	       {
		  if (pAb->AbSizeUnit == UnPoint)
		     CorpsCourant = pAb->AbSize;
		  else
		     CorpsCourant = FontPointSize (pAb->AbSize);
		  i = (CorpsCourant * pAb->AbLineWeight) / 10;
		  if ((CorpsCourant * i) % 10 >= 5)
		     i++;
	       }
	     TtaSetNumberForm (NumZoneStrokeWeight, i);

	     /* selecteur motif de remplissage */
	     nbitem = MakeMenuPattern (chaine, MAX_TXT_LEN);
	     if (nbitem > 0)
		/* il y a des motifs de remplissage definis */
		/* on cree un selecteur */
	       {
		  if (nbitem >= 5)
		     i = 5;
		  else
		     i = nbitem;
		  TtaNewSelector (NumSelectPattern, NumFormPresGraphics,
				  TtaGetMessage (LIB, FILL_PATTERN),
				  nbitem, chaine, i, NULL, TRUE, FALSE);
		  /* initialise le selecteur sur sa premiere entree */
		  TtaSetSelector (NumSelectPattern, pAb->AbFillPattern, "");
	       }
	     /* Toggle button Motif de remplissage inchange' */
	     i = 0;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     TtaNewToggleMenu (NumTogglePatternUnchanged, NumFormPresGraphics,
			       NULL, 1, chaine, NULL, FALSE);
	     DocModPresent = pDoc;
	     TtaShowDialogue (NumFormPresGraphics, TRUE);
	  }
     }
}



/* ---------------------------------------------------------------------- */
/* |    ModPresentFormat        L'utilisateur demande a modifier la     | */
/* |            presentation specifique (le format) pour la vue         | */
/* |            Vue du document pDoc (si Assoc = FALSE) ou les elements | */
/* |            associes de numero Vue (si Assoc = Vrai)                | */
/* |            Initialise et active le formulaire correspondant.       | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcChangeFormat (Document document, View view)
#else  /* __STDC__ */
void                TtcChangeFormat (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i;
   PtrDocument         SelDoc;
   PtrElement          PremSel, DerSel;
   int                 premcar, dercar;
   boolean             selok;
   PtrAbstractBox             pAb;
   int                 CorpsCourant;
   char                chaine[MAX_TXT_LEN];
   PtrDocument         pDoc;

   pDoc = LoadedDocument[document - 1];

   /* demande quelle est la selection courante */
   selok = GetCurrentSelection (&SelDoc, &PremSel, &DerSel, &premcar, &dercar);
   if (!selok)
     {
	/* par defaut la racine du document */
	SelDoc = pDoc;
	PremSel = pDoc->DocRootElement;
	selok = TRUE;
     }

   if (selok && SelDoc == pDoc)
      /* il y a une selection */
     {
	/* recherche le pave concerne */
	if (view > 100)
	   pAb = PaveDeElem (PremSel, 1);
	else
	   pAb = PaveDeElem (PremSel, view);

	if (pAb != NULL)
	  {

	     RazRetoursMenus ();
	     /* formulaire Presentation Format */
	     TtaNewSheet (NumFormPresFormat, TtaGetViewFrame (document, view), 0, 0,
			  TtaGetMessage (LIB, FORMAT),
		   1, TtaGetMessage (LIB, APPLY), TRUE, 3, 'L', D_DONE);

	     /* sous-menu Alignement */
	     i = 0;
	     sprintf (&chaine[i], "%s", "Bmiidn");	/* gauche */
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s", "Bmeiin");	/* droite */
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s", "Bmfogn");	/* Centrer */
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));	/* Inchange */
	     TtaNewSubmenu (NumMenuAlignment, NumFormPresFormat, 0,
		    TtaGetMessage (LIB, ALIGN), 4, chaine, NULL, FALSE);
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
			 TtaGetMessage (LIB, INDENT_PTS), 0, 300, TRUE);
	     /* initialise la valeur du renfoncement */
	     if (pAb->AbSizeUnit == UnPoint)
		CorpsCourant = pAb->AbSize;
	     else
		CorpsCourant = FontPointSize (pAb->AbSize);
	     ValIndent = abs (pAb->AbIndent);
	     if (pAb->AbIndentUnit == UnRelative)
		/* convertit ValIndent en points typographiques */
	       {
		  ValIndent = (CorpsCourant * ValIndent) / 10;
		  if ((CorpsCourant * ValIndent) % 10 >= 5)
		     ValIndent++;
	       }
	     TtaSetNumberForm (NumZoneRecess, ValIndent);

	     /* sous-menu sens de renfoncement */
	     i = 0;
	     sprintf (&chaine[i], "%s", "Bm_`an");
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s", "Bmb`an");
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     TtaNewSubmenu (NumMenuRecessSense, NumFormPresFormat, 0,
		    TtaGetMessage (LIB, INDENT), 3, chaine, NULL, TRUE);
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
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, LIB_YES));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, LIB_NO));
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     TtaNewSubmenu (NumMenuJustification, NumFormPresFormat, 0,
	     TtaGetMessage (LIB, JUSTIFY), 3, chaine, NULL, FALSE);
	     /* menu de justification */
	     if (pAb->AbJustify)
		i = 1;		/* avec justification */
	     else
		i = 2;		/* sans justification */
	     TtaSetMenuForm (NumMenuJustification, i - 1);

	     /* zone de saisie de l'interligne en points typo */
	     TtaNewNumberForm (NumZoneLineSpacing, NumFormPresFormat,
		   TtaGetMessage (LIB, LINE_SPACING_PTS), 1, 200, TRUE);

	     /* sous-menu Interligne (Grandeur) */
	     i = 0;
	     sprintf (&chaine[i], "%s", "BmTTTn");	/* 'Normal%' */
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s", "BmWWWn");	/* 'Double%' */
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s", "BmZZZn");	/* 'Triple%' */
	     i += strlen (&chaine[i]) + 1;
	     sprintf (&chaine[i], "%s%s", "B", TtaGetMessage (LIB, UNCHANGED));
	     TtaNewSubmenu (NumMenuLineSpacing, NumFormPresFormat, 0,
	      TtaGetMessage (LIB, LINE_SPACING), 4, chaine, NULL, TRUE);
	     /* change la police des 3 premieres entrees */
	     for (i = 0; i < 3; i++)
		TtaRedrawMenuEntry (NumMenuLineSpacing, i, "icones", ThotColorNone, -1);
	     /* initialise l'interligne en points typographiques */
	     if (pAb->AbSizeUnit == UnPoint)
		CorpsCourant = pAb->AbSize;
	     else
		CorpsCourant = FontPointSize (pAb->AbSize);
	     i = pAb->AbLineSpacing;
	     if (pAb->AbLineSpacingUnit == UnRelative)
		/* convertit l'interligne en points typographiques */
	       {
		  i = (CorpsCourant * i) / 10;
		  if ((CorpsCourant * i) % 10 >= 5)
		     i++;
	       }
	     oldinterligne = i;
	     TtaSetNumberForm (NumZoneLineSpacing, i);

	     /* saisie de l'interligne par un menu */
	     if (oldinterligne <= (INTERLGN_SIMPLE * 3) / 2)
		i = 0;
	     else if (pAb->AbLineSpacing >= INTERLGN_SIMPLE * 2)
		i = 2;
	     else
		i = 1;
	     TtaSetMenuForm (NumMenuLineSpacing, i);
	     DocModPresent = pDoc;
	     TtaShowDialogue (NumFormPresFormat, TRUE);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
static void         RazRetoursMenus ()
{
   if (ThotLocalActions[T_presentstd] == NULL)
     {
	/* Connecte les actions liees au traitement de la TtcSplit */
	TteConnectAction (T_presentstd, (Proc) CallbackStdPresMenu);
	TteConnectAction (T_present, (Proc) CallbackPresMenu);
     }
   ChngCarStandard = FALSE;
   ChngCoulStandard = FALSE;
   ChngGraphStandard = FALSE;
   ChngGeomStandard = FALSE;
   ChngFormStandard = FALSE;
   ChngStyleTrait = FALSE;
   ChngEpaisTrait = FALSE;
   ChngTrame = FALSE;
   ChngFamille = FALSE;
   ChngSouligne = FALSE;
   ChngEpais = FALSE;
   ChngStyle = FALSE;
   ChngCorps = FALSE;
   ChngCadr = FALSE;
   ChngJustif = FALSE;
   ChngHyphen = FALSE;
   ChngIndent = FALSE;
   ChngInterL = FALSE;
}
/* End Of Module modpres */
