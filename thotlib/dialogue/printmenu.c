/* -- Copyright (c) 1990 - 1996 Inria/CNRS  All rights reserved. -- */

/*
   printmenu.c : gestion des messages et menus de l'impression.
   Code issu des modules initcatal.c et editmenu.c      

   V. Quint     Mai 1992
   C. Roisin     Avril 1996
   I. Vatton     Juin 1996
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "language.h"
#include "constmenu.h"

#include "document.h"
#include "dialog.h"
#include "appdialogue.h"


#undef EXPORT
#define EXPORT extern
#include "environ.var"
#include "edit.var"
#include "appdialogue.var"

#include "appli_f.h"
#include "applicationapi_f.h"
#include "attrmenu_f.h"
#include "tree_f.h"
#include "browser_f.h"
#include "search_f.h"
#include "searchmenu_f.h"
#include "textcommands_f.h"
#include "config_f.h"
#include "structcreation_f.h"
#include "creationmenu_f.h"
#include "dialogapi_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "viewapi_f.h"
#include "dofile_f.h"
#include "viewcommands_f.h"
#include "exceptions_f.h"
#include "absboxlist_f.h"
#include "structmodif_f.h"
#include "presentmenu_f.h"
#include "printmenu_f.h"
#include "structselect_f.h"
#include "selectmenu_f.h"
#include "sysexec_f.h"


#define NBMAXENTREES 25
static PathBuffer   psdir;
static boolean      ImprimerPapier;

/* static AvailableView     LesVuesImprimables; */
static boolean      AlimentationManuelle;
static boolean      NewImprimerPapier;


/* ---------------------------------------------------------------------- */
/* |  ConnectPrint initialise les valeurs du formulaire d'impression.   | */
/* ---------------------------------------------------------------------- */
static void         ConnectPrint ()
{
   char               *ptr;

   if (ThotLocalActions[T_rprint] == NULL)
     {
	/* Connecte les actions liees au traitement du print */
	TteConnectAction (T_rprint, (Proc) RetMenuImprimer);
	/* read DEFAULTPRINTER shell variable */
	ptr = TtaGetEnvString ("THOTPRINT");
	if (ptr == NULL)
	   strcpy (ptImprimante, "");
	else
	   strcpy (ptImprimante, ptr);
	psdir[0] = '\0';
	ImprimerPapier = TRUE;
	AlimentationManuelle = FALSE;
	strcpy (page_size, "A4");
     }
}


/* ---------------------------------------------------------------------- */
/* |  TraiteMenuImprimer traite les retours du formulaire d'impression. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcPrint (Document document, View view)
#else  /* __STDC__ */
void                TtcPrint (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PathBuffer          dirname;
   Name                 docname;
   PathBuffer          vuesaimprimer;

   boolean             ok;

   Name                 savePres, newPres;



   pDocPrint = TabDocuments[document - 1];
   ConnectPrint ();
   /* prepare le lancement de l'impression */
   strcpy (savePres, pDocPrint->DocSSchema->SsDefaultPSchema);
   ConfigGetPSchemaForPageSize (pDocPrint->DocSSchema, page_size, newPres);
   if (newPres[0] != '\0')
      strcpy (pDocPrint->DocSSchema->SsDefaultPSchema, newPres);

   /* la repagination se fait dans le print */
   SavePath (pDocPrint, dirname, docname);
   GetTempNames (pDocPrint->DocDirectory, pDocPrint->DocDName);
   ok = SauveDocument (pDocPrint, 2);

   /* restaure le schema de presentation */
   strcpy (pDocPrint->DocSSchema->SsDefaultPSchema, savePres);

   /* on fait une sauvegarde automatique */
   if (ok)
     {
	strcpy (vuesaimprimer, TtaGetViewName (document, view));
	strcat (vuesaimprimer, " ");
	if (ImprimerPapier)
	   Imprimer (pDocPrint->DocDName,
		     pDocPrint->DocDirectory,
		     pDocPrint->DocSchemasPath,
		     DirectoryDoc,
		     pDocPrint->DocSSchema->SsDefaultPSchema,
		     docname, dirname, ptImprimante,
		     1, 999, 1, 0, 0, 0,
		     100, 1, TRUE,
		     (int) AlimentationManuelle, 0,
		     1,
		     vuesaimprimer);
	else if (psdir[0] != '\0')
	   SauverPS (pDocPrint->DocDName,
		     pDocPrint->DocDirectory,
		     pDocPrint->DocSchemasPath,
		     DirectoryDoc,
		     pDocPrint->DocSSchema->SsDefaultPSchema,
		     docname, dirname, psdir,
		     1, 999, 1, 0, 0, 0,
		     100, 1, TRUE,
		     (int) AlimentationManuelle, 0,
		     1,
		     vuesaimprimer);
     }
   RestorePath (pDocPrint, dirname, docname);
}


/* ---------------------------------------------------------------------- */
/* |    RetMenuImprimer analyse les retours du formulaire d'impression. | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuImprimer (int ref, int val, char *txt)
#else  /* __STDC__ */
void                RetMenuImprimer (ref, val, txt)
int                 ref;
int                 val;
char               *txt;

#endif /* __STDC__ */
{
   if (pDocPrint != NULL)
      if (pDocPrint->DocSSchema != NULL)
	 /* le document a imprimer existe toujours */
	 switch (ref)
	       {
		  case NumMenuSupport:
		     /* sous-menu imprimer papier / sauver PostScript */
		     switch (val)
			   {
			      case 0:
				 if (!NewImprimerPapier)
				   {
				      NewImprimerPapier = TRUE;
				      TtaSetTextForm (NumZoneNomImprimante, ptImprimante);
				   }
				 break;
			      case 1:
				 if (NewImprimerPapier)
				   {
				      NewImprimerPapier = FALSE;
				      TtaSetTextForm (NumZoneNomImprimante, psdir);
				   }
				 break;
			   }
		     break;
		  case NumMenuFormatPapier:
		     /* sous-menu format papier */
		     switch (val)
			   {
			      case 0:
				 strcpy (page_size, "A4");
				 break;
			      case 1:
				 strcpy (page_size, "US");
				 break;
			   }
		     break;
		  case NumMenuOptions:
		     /* choix multiple Options */
		     AlimentationManuelle = !AlimentationManuelle;
		     break;
		  case NumZoneNomImprimante:
		     if (txt[0] != '\0')
			if (NewImprimerPapier)
			   /* zone de saisie du nom de l'imprimante */
			   strncpy (ptImprimante, txt, MAX_NAME_LENGTH);
			else
			   /* zone de saisie du nom du fichier PostScript */
			   strncpy (psdir, txt, MAX_PATH);
		     break;
		  case NumFormImprimer:
		     /* formulaire Imprimer */
		     TtaDestroyDialogue (NumFormImprimer);
		     switch (val)
			   {
			      case 1:
				 /* confirme l'option Imprimer papier */
				 /* les autres options ne sont prises en compte que sur confirmation */
				 ImprimerPapier = NewImprimerPapier;
				 break;
			      default:
				 break;
			   }
		     break;
		  default:
		     break;
	       }
}

/* ---------------------------------------------------------------------- */
/* |    TtcPrintSetup construit les catalogues qui seront utilises      | */
/* |            par l'editeur pour le formulaire d'impression.          | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtcPrintSetup (Document document, View view)
#else  /* __STDC__ */
void                TtcPrintSetup (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i;
   char                BufMenu[MAX_TXT_LEN];

   pDocPrint = TabDocuments[document - 1];

   /* formulaire Imprimer */
   ConnectPrint ();
   TtaNewSheet (NumFormImprimer, TtaGetViewFrame (document, view), 0, 0,
		TtaGetMessage (LIB, LIB_PRINT),
		1, TtaGetMessage (LIB, LIB_CONFIRM), FALSE, 2, 'L', D_DONE);
   i = 0;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, LIB_MANUAL_FEED));
   TtaNewToggleMenu (NumMenuOptions, NumFormImprimer,
		 TtaGetMessage (LIB, LIB_OPTIONS), 1, BufMenu, NULL, FALSE);
   if (AlimentationManuelle)
      TtaSetToggleMenu (NumMenuOptions, 0, TRUE);

   /* sous-menu format papier */
   i = 0;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, LIB_A4));
   i += strlen (&BufMenu[i]) + 1;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, LIB_US));
   TtaNewSubmenu (NumMenuFormatPapier, NumFormImprimer, 0,
	    TtaGetMessage (LIB, LIB_PAPER_FORMAT), 2, BufMenu, NULL, FALSE);
   if (!strcmp (page_size, "US"))
      TtaSetMenuForm (NumMenuFormatPapier, 1);
   else
      TtaSetMenuForm (NumMenuFormatPapier, 0);

   /* sous-menu imprimer papier / sauver PostScript */
   i = 0;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, LIB_PRINTER));
   i += strlen (&BufMenu[i]) + 1;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, LIB_POSTSCRIPT_FILE));
   TtaNewSubmenu (NumMenuSupport, NumFormImprimer, 0,
		  TtaGetMessage (LIB, LIB_OUTPUT), 2, BufMenu, NULL, TRUE);
   /* zone de saisie du nom de l'imprimante */
   TtaNewTextForm (NumZoneNomImprimante, NumFormImprimer, NULL, 30, 1, FALSE);

   /* initialisation du selecteur ImprimerPapier */
   NewImprimerPapier = ImprimerPapier;
   if (ImprimerPapier)
     {
	TtaSetMenuForm (NumMenuSupport, 0);
	TtaSetTextForm (NumZoneNomImprimante, ptImprimante);
     }
   else
     {
	TtaSetMenuForm (NumMenuSupport, 1);
	TtaSetTextForm (NumZoneNomImprimante, psdir);
     }

   /* active le formulaire "Imprimer" */
   TtaShowDialogue (NumFormImprimer, FALSE);

}
