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
 * User interface for checker commands
 *
 * Author: H. Richy (INRISA)
 *
 */

#include "thot_gui.h"
#include "thot_sys.h"

#include "corrmsg.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "constcorr.h"
#include "typecorr.h"
#include "language.h"
#include "dialog.h"
#include "corrmenu.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appdialogue_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "spell_tv.h"

/* les variables locales */
static int          OldNC;
static int          BaseCorrector;
static boolean      CorrPremiereFois;
static boolean      remplacer;
static char         MotCorrige[MAX_WORD_LEN];
static char         MotCourant[MAX_WORD_LEN];
static PtrDocument  docsel;
static char         CarSpeciaux[] =
{"@#$&+~"};

/* les variables importees de l'editeur */

/* procedures importees de l'editeur */
#include "spellchecker_f.h"
#include "dictionary_f.h"
#include "word_f.h"
#include "appli_f.h"
#include "memory_f.h"
#include "views_f.h"
#include "structselect_f.h"

#ifdef __STDC__
static void         Corr_affichermots (void);
extern void         UnsetEntryMenu (int, int);

#else  /* __STDC__ */
static void         Corr_affichermots ();
extern void         UnsetEntryMenu ();

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   EnleveFormCorriger fait disparaitre les formulaires correction    
   s'ils concernent le document pDoc et s'ils sont a l'ecran.      
  ----------------------------------------------------------------------*/
static void         EnleveFormCorriger ()
{
   if (ChkrRange != NULL)
     {
	FreeSearchContext (&ChkrRange);
	TtaDestroyDialogue (BaseCorrector + ChkrFormCorrect);
     }
}


/*----------------------------------------------------------------------
   ActionCorriger active le formulaire de correction                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSpellCheck (Document doc, View view)
#else  /* __STDC__ */
void                TtcSpellCheck (doc, view)
Document            doc;
view                view;

#endif /* __STDC__ */
{
   PtrDocument         document;
   PtrElement          pEl1, pElN;
   int                 c1, cN;
   int                 i, indx;
   char                BufMenu[MAX_TXT_LEN];
   boolean             ok;


   /* Document concerne */
   document = LoadedDocument[doc - 1];
   if (document == NULL)
      return;
   SpellCheckLoadResources ();
   if (ChkrRange != NULL)
      EnleveFormCorriger ();
   TtaDestroyDialogue (BaseCorrector + ChkrFormCorrect);
   /* creer la feuille de dialogue de CORRECTION */
   /* attache'e au bouton Confirmer du formulaire (1) CORRIGER */
   indx = 0;			/* tous les boutons du dialogue de correction */
   strcpy (&BufMenu[indx], TtaGetMessage (CORR, Pass_Without));
  /*----------------------------------------------------------------------
   indx += strlen(&BufMenu[indx]) + 1;
   strcpy(&BufMenu[indx], TtaGetMessage(CORR, Pass_With));
  ----------------------------------------------------------------------*/
   indx += strlen (&BufMenu[indx]) + 1;
   strcpy (&BufMenu[indx], TtaGetMessage (CORR, Replace_Without));
  /*----------------------------------------------------------------------
   indx += strlen(&BufMenu[indx]) + 1;
   strcpy(&BufMenu[indx], TtaGetMessage(CORR, Replace_With));
  ----------------------------------------------------------------------*/
   /* ne pas afficher cette feuille maintenant */
   TtaNewSheet (BaseCorrector + ChkrFormCorrect, TtaGetViewFrame (doc, view), 
		TtaGetMessage (CORR, Correct), 2, BufMenu, FALSE, 4, 'L', D_DONE);

   /* initialise le champ langue de correction courante */
   TtaNewLabel (BaseCorrector + ChkrLabelLanguage, BaseCorrector + ChkrFormCorrect, " ");

   /* Afficher une liste de mots EMPTY */
   strcpy (ChkrCorrection[0], " ");
   for (i = 1; i <= NC; i++)
      strcpy (ChkrCorrection[i], "$");
   Corr_affichermots ();

   /* creer le sous-menu OU dans la feuille OPTIONS */
   indx = 0;			/* Ou commencer la correction ? */
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (LIB, TMSG_BEFORE_SEL));
   indx += strlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (LIB, TMSG_WITHIN_SEL));
   indx += strlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (LIB, TMSG_AFTER_SEL));
   indx += strlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (LIB, TMSG_IN_WHOLE_DOC));
   TtaNewSubmenu (BaseCorrector + ChkrMenuOR, BaseCorrector + ChkrFormCorrect, 0,
		  TtaGetMessage (CORR, What), 4, BufMenu, NULL, FALSE);
   TtaSetMenuForm (BaseCorrector + ChkrMenuOR, 2);	/* apres la selection */
   /* Initialiser le formulaire CORRIGER */
   /* Document selectionne */
   ok = GetCurrentSelection (&docsel, &pEl1, &pElN, &c1, &cN);
   if (!ok || docsel != document)
     {
	/* pas de selection dans le document d'ou vient la commande */
	UnsetEntryMenu (BaseCorrector + ChkrMenuOR, 0);
	UnsetEntryMenu (BaseCorrector + ChkrMenuOR, 1);
	UnsetEntryMenu (BaseCorrector + ChkrMenuOR, 2);
	TtaSetMenuForm (BaseCorrector + ChkrMenuOR, 3);
     }
   else
     {

	TtaRedrawMenuEntry (BaseCorrector + ChkrMenuOR, 0, NULL, -1, 1);
	TtaRedrawMenuEntry (BaseCorrector + ChkrMenuOR, 1, NULL, -1, 1);
	TtaRedrawMenuEntry (BaseCorrector + ChkrMenuOR, 2, NULL, -1, 1);
     }

   /* initialise le champ termine de correction courante */
   TtaNewLabel (BaseCorrector + ChkrLabelNotFound, BaseCorrector + ChkrFormCorrect, " ");

   /* creer la forme de modification d'un nombre NC 1-10 */
   TtaNewNumberForm (BaseCorrector + ChkrCaptureNC, BaseCorrector + ChkrFormCorrect,
     TtaGetMessage (CORR, Number_Propositions), 1, MAX_PROPOSAL_CHKR, TRUE);
   TtaSetNumberForm (BaseCorrector + ChkrCaptureNC, 3);

   /* sous-menu Mots a ignorer */
   indx = 0;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (CORR, Capitals));
   indx += strlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (CORR, Arabics));
   indx += strlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (CORR, Romans));
   indx += strlen (&BufMenu[indx]) + 1;
   sprintf (&BufMenu[indx], "B%s", TtaGetMessage (CORR, Specials));
   TtaNewToggleMenu (BaseCorrector + ChkrMenuIgnore,
		     BaseCorrector + ChkrFormCorrect,
		     TtaGetMessage (CORR, Ignore),
		     4, BufMenu, NULL, TRUE);

   /* liste des caracteres speciaux dans le formulaire OPTIONS */
   TtaNewTextForm (BaseCorrector + ChkrSpecial, BaseCorrector + ChkrFormCorrect, NULL, 20, 1, TRUE);
   TtaSetTextForm (BaseCorrector + ChkrSpecial, CarSpeciaux);

/* ne pas ignorer les mots en capitale, chiffres romains */
   /* ou contenant des chiffres arabes,  certains car. speciaux */
   strncpy (RejectedChar, CarSpeciaux, MAX_REJECTED_CHARS);	/* liste par defaut */
   IgnoreUppercase = FALSE;
   IgnoreArabic = FALSE;
   IgnoreRoman = FALSE;
   IgnoreSpecial = FALSE;
   /* Selectionne les types de mots a ne ignorer par defaut */
   TtaSetToggleMenu (BaseCorrector + ChkrMenuIgnore, 0, IgnoreUppercase);
   TtaSetToggleMenu (BaseCorrector + ChkrMenuIgnore, 1, IgnoreArabic);
   TtaSetToggleMenu (BaseCorrector + ChkrMenuIgnore, 2, IgnoreRoman);
   TtaSetToggleMenu (BaseCorrector + ChkrMenuIgnore, 3, IgnoreSpecial);

   /* affiche une liste de car. speciaux */
   TtaSetTextForm (BaseCorrector + ChkrSpecial, RejectedChar);

   /* Et enfin, afficher le formulaire de CORRECTION */
   TtaShowDialogue (BaseCorrector + ChkrFormCorrect, TRUE);

   /* Indique que c'est une nouvelle correction qui debute */
   CorrPremiereFois = TRUE;

   ChkrRange = NULL;		/* pas de contexte de correction alloue */
   /* On alloue le contexte */
   GetSearchContext (&ChkrRange);
   ChkrRange->SDocument = document;
   /* ne cree pas inutilement le dictionnaire fichier */
   TtaLoadDocumentDictionary (document, (int*) &ChkrFileDict, FALSE);

}				/*ActionCorriger */




/*----------------------------------------------------------------------
   Corr_affichermots prepare le selecteur                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Corr_affichermots (void)
#else
static void         Corr_affichermots ()
#endif
{
   int                 i, indx, longueur;
   char               *entree;
   char                BufMenu[MAX_TXT_LEN];

   /* recopie les propositions */
   indx = 0;
   /* commencer a 1 parce qu'en 0 il y a MotCourant */
   for (i = 1; (i <= NC && strcmp (ChkrCorrection[i], "$") != 0); i++)
     {
	entree = ChkrCorrection[i];
	longueur = strlen (entree) + 1;
	if (longueur + indx < MAX_TXT_LEN)
	  {
	     strcpy ((BufMenu) + indx, entree);
	     indx += longueur;
	  }
     }
   /* creer le selecteur dans la feuille de dialogue */
   entree = ChkrCorrection[0];
   /* remplir le formulaire CORRIGER */
   /* attention i = nbpropositions + 1 (le mot errone) */
   TtaNewSelector (BaseCorrector + ChkrSelectProp, BaseCorrector + ChkrFormCorrect,
		   TtaGetMessage (CORR, Correct), i - 1,
		   ((i < 2) ? "" : BufMenu), 3, entree, TRUE, FALSE);
   /* selectionner la proposition 0 dans le selecteur - si elle existe */
   if (ChkrCorrection[1] != "$")
      TtaSetSelector (BaseCorrector + ChkrSelectProp, -1, ChkrCorrection[1]);
   else
      TtaSetSelector (BaseCorrector + ChkrSelectProp, -1, "");

   /* le formulaire est maintenant pret a etre affiche' */
   OldNC = NC;
}				/*Corr_affichermots */

/*----------------------------------------------------------------------
   Corr_propositions                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Corr_propositions (Language langue)

#else  /* __STDC__ */
static void         Corr_propositions (langue)
Language            langue;

#endif /* __STDC__ */
{
   char                Lab[200];

   /* calculer les propositions de correction du mot courant */
   Corr_proposer (langue, ChkrFileDict);
   /* afficher les mots proposes contenus dans ChkrErrWord */
   Corr_affichermots ();
   /* recopier la premiere proposition dans MotCorrige */
   if (strcmp (ChkrCorrection[1], "$") != 0)
      strcpy (MotCorrige, ChkrCorrection[1]);
   else
      MotCorrige[0] = '\0';

   /* afficher la langue de correction courante */
   sprintf (Lab, "%s: %s", TtaGetMessage (LIB, TMSG_LANGUAGE), TtaGetLanguageName (ChkrLanguage));
   TtaNewLabel (BaseCorrector + ChkrLabelLanguage, BaseCorrector + ChkrFormCorrect, Lab);
}				/*Corr_propositions */


/*----------------------------------------------------------------------
   Corr_lancer retourne 0 si c'est OK et -1 en cas d'erreur          
  ----------------------------------------------------------------------*/
static boolean      Corr_lancer ()
{
   boolean             ok;

   /* preparer le lancement de la premiere correction */
   ok = TRUE;
   MotCorrige[0] = '\0';
   MotCourant[0] = '\0';
   remplacer = FALSE;
   /* Rechercher la premiere erreur */
   if (CorrPremiereFois)
     {
	CorrPremiereFois = FALSE;
	ChkrElement = NULL;
	ChkrIndChar = 0;
     }

   /* en tenant compte des options choisies (par defaut) */
   Corr_errsuivant (MotCourant, ChkrFileDict);
   if (MotCourant[0] != '\0')
      /* calculer la 1ere liste des propositions ds selecteur */
      Corr_propositions (ChkrLanguage);
   else
     {
	/* Correction TERMINEE */
	TtaDisplaySimpleMessage (INFO, CORR, END_CHECK);
	/* message 'Pas trouve' dans le formulaire */
	TtaNewLabel (BaseCorrector + ChkrLabelNotFound, BaseCorrector + ChkrFormCorrect,
		     TtaGetMessage (LIB, TMSG_NOT_FOUND));
	CorrPremiereFois = TRUE;
	ok = FALSE;
     }
   return (ok);
}				/*Corr_lancer */


/*----------------------------------------------------------------------
   Corr_commande traite l'action de correction demandee.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Corr_commande (int val)

#else  /* __STDC__ */
static void         Corr_commande (val)
int                 val;

#endif /* __STDC__ */
{
   boolean             change;
   PtrDocument         document;

   if (ChkrRange == NULL)
      /* Le correcteur n'est pas lance */
      return;

   /* Teste l'etat de la nouvelle selection */
   document = ChkrRange->SDocument;
   change = CorrChangeSelection ();

   if (document != ChkrRange->SDocument || val == 0)
     {
	/* ABANDON de la corrrection */
	EnleveFormCorriger ();
     }
   else
     {
	change = TRUE;		/* On passe a priori au mot suivant */
	switch (val)
	      {
		 case 1:
		    /* Passer sans maj du dictionnaire */
		    break;
#ifdef IV
		 case 2:
		    /* Passer apres maj du dictionnaire */
		    /* mettre MotCourant dans le dictionnaire */
		    if (ChkrElement != NULL)	/* Sauf au 1er lancement */
		       if (MotCorrige[0] != '\0' && (strcmp (MotCorrige, ChkrCorrection[1]) != 0))
			 {
			    /* ajouter le mot corroge dans le dictionaire */
			    if (!Corr_verifiermot (MotCorrige, ChkrLanguage, ChkrFileDict))
			       Corr_ajoutermot (MotCorrige, &ChkrFileDict);
			 }	/* end of if */
		       else
			 {
			    /* ajouter le mot courant dans le dictionnaire */
			    if (!Corr_verifiermot (MotCourant, ChkrLanguage, ChkrFileDict))
			       Corr_ajoutermot (MotCourant, &ChkrFileDict);
			 }	/* end of else */
		    break;
		 case 3:
#endif
		 case 2:
		    /* Remplacer */
		    if (remplacer)	/* MotCorrige est rempli */
		      {		/* et ce n'est pas 1er lancement */
			 if (ChkrElement != NULL && MotCourant[0] != '\0')
			    Corr_remplacermot (MotCourant, MotCorrige);
			 remplacer = FALSE;
		      }
		    break;
#ifdef IV
		 case 4:
		    /* Remplacer et maj dictionnaire */
		    if (remplacer)	/* MotCorrige est rempli */
		      {		/* et ce n'est pas 1er lancement */
			 if (ChkrElement != NULL && MotCourant[0] != '\0')
			   {
			      Corr_remplacermot (MotCourant, MotCorrige);
			      /* mettre MotCorrige dans le dictionnaire */
			      if (!Corr_verifiermot (MotCorrige, ChkrLanguage, ChkrFileDict))
				 /* mettre ce nouveau mot dans le dictionnaire */
				 Corr_ajoutermot (MotCorrige, &ChkrFileDict);
			   }
			 remplacer = FALSE;
		      }
		    break;
#endif
	      }			/* end of switch */

	if (change)
	   /* Lancement de la recherche d'erreur suivante */
	   Corr_lancer ();
     }
}				/*Corr_commande */


/*----------------------------------------------------------------------
   CallbackCorrector                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackCorrector (int Ref, int TypeData, char *Data)
#else  /* __STDC__ */
void                CallbackCorrector (Ref, TypeData, Data)
int                 Ref;
int                 TypeData;
char               *Data;

#endif /* __STDC__ */

{
   PtrElement          pEl1, pElN;
   int                 c1, cN;

   if (Ref == -1)
     {				/* detruit le dialogue du correcteur */
	EnleveFormCorriger ();
     }
   else
      switch (Ref - BaseCorrector)
	    {
	       case ChkrMenuIgnore:
		  switch ((int) Data)
			{
			   case 0:
			      /* ignorer les mots ecrits en majuscule */
			      IgnoreUppercase = !IgnoreUppercase;
			      break;
			   case 1:
			      /* ignorer les mots contenant un chiffre arabe */
			      IgnoreArabic = !IgnoreArabic;
			      break;
			   case 2:
			      /* ignorer les chiffres romains */
			      IgnoreRoman = !IgnoreRoman;
			      break;
			   case 3:
			      /* ignorer les mots contenant certains car speciaux */
			      IgnoreSpecial = !IgnoreSpecial;
			      break;
			}	/* end of switch */
		  break;
	       case ChkrSpecial:
		  /* recopier la liste des car. speciaux dans RejectedChar[] */
		  strncpy (RejectedChar, Data, MAX_REJECTED_CHARS);
		  /* bascule automatiquement l'indicateur IgnoreSpecial */
		  if (!IgnoreSpecial)
		    {
		       IgnoreSpecial = TRUE;
		       TtaSetToggleMenu (BaseCorrector + ChkrMenuIgnore, 3, IgnoreSpecial);
		    }
		  break;
	       case ChkrCaptureNC:
		  /* modification de Nb de corrections a proposer */
		  NC = (int) Data;
		  if (NC > OldNC && ChkrElement != NULL && MotCourant[0] != '\0')
		     Corr_propositions (ChkrLanguage);
		  else
		     Corr_affichermots ();
		  break;
	       case ChkrMenuOR:
		  /* definition du sens de correction OU? */
		  switch ((int) Data)
			{
			   case 0:	/* ElemIsBefore la selection */
			      ChkrRange->SStartToEnd = FALSE;
			      break;
			   case 1:	/* Dans la selection */
			      CorrPremiereFois = TRUE;
			      break;
			   case 2:	/* Apres la selection */
			      ChkrRange->SStartToEnd = TRUE;
			      break;
			   case 3:	/* Dans tout le document */
			      CorrPremiereFois = TRUE;
			      break;
			}
		  /* La premiere fois on initialise le domaine de recherche */
		  if (CorrPremiereFois)
		    {
		       InitSearchDomain ((int) Data, ChkrRange);
		       /* On prepare la recheche suivante */
		       if (ChkrRange->SStartToEnd)
			  TtaSetMenuForm (BaseCorrector + ChkrMenuOR, 2);
		       else
			  TtaSetMenuForm (BaseCorrector + ChkrMenuOR, 0);
		    }
		  else if (ChkrRange != NULL)
		     /* Est-ce que le document vient de recevoir la selection */
		     if (docsel != ChkrRange->SDocument)
		       {
			  /* Est-ce encore vrai */
			  GetCurrentSelection (&docsel, &pEl1, &pElN, &c1, &cN);
			  if (docsel == ChkrRange->SDocument)
			    {
			       /* Il faut reactiver les entree */
			       TtaRedrawMenuEntry (BaseCorrector + ChkrMenuOR, 0, NULL, -1, 1);
			       TtaRedrawMenuEntry (BaseCorrector + ChkrMenuOR, 1, NULL, -1, 1);
			       TtaRedrawMenuEntry (BaseCorrector + ChkrMenuOR, 2, NULL, -1, 1);
/*          ActiveEntree(ChkrMenuOR, 0); */
/*             ActiveEntree(ChkrMenuOR, 1); */
/*             ActiveEntree(ChkrMenuOR, 2); */
			    }
		       }
		  break;
	       case ChkrSelectProp:
		  /* retour du selecteur de propositions */
		  /* recopier le choix dans MotCorrige */
		  strcpy (MotCorrige, Data);
		  if (MotCorrige[0] != '\0'
		      && MotCourant != '\0'
		      && strcmp (MotCorrige, MotCourant) != 0)
		     remplacer = TRUE;
		  break;
	       case ChkrFormCorrect:
		  /* retour de la feuille de dialogue CORRECTION */
		  /* effectuer l'action demandee */
		  Corr_commande ((int) Data);
		  break;
	    }			/* end of switch (Ref) */
}				/* end of CallbackCorrector */


/*----------------------------------------------------------------------
   SpellCheckLoadResources charge les ressouces du correcteur       
   orthographique                                         
  ----------------------------------------------------------------------*/
void                SpellCheckLoadResources ()
{
   if (ThotLocalActions[T_corrector] == NULL)
     {
	TteConnectAction (T_rscorrector, (Proc) CallbackCorrector);
	TteConnectAction (T_corrector, (Proc) TtcSpellCheck);
	CORR = TtaGetMessageTable ("corrdialogue", MSG_MAX_CHECK);
	BaseCorrector = TtaSetCallback (CallbackCorrector, ChkrMaxDialogue);
	/* Initialisation des variables globales */
	Corr_Param ();
	ChkrFileDict = NULL;
	ChkrRange = NULL;
     }
}
