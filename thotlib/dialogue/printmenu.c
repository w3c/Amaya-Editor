/*
   gestion des messages et menus de l'impression.

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
#include "frame.h"
#include "thotfile.h"

#undef EXPORT
#define EXPORT extern
#include "environ.var"
#include "edit.var"
#include "appdialogue.var"
#include "frame.var"

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

#define NBMAXENTREES 25
static PathBuffer   psdir;
static boolean      ImprimerPapier;
static boolean      AlimentationManuelle;
static boolean      NewImprimerPapier;

static ThotFileHandle msgfile_fid = ThotFile_BADHANDLE;		/* le fichier temporaire des messages */
static char         msgfile_name[40];	/* son nom */
static ThotFileOffset msgfile_curpos;	/* la position courante dans le fichier */
static char         msg_buffer[1024];	/* le message courant */


/* ---------------------------------------------------------------------- */

/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         initImpression (int user_orientation, char *thotdir, char *tempdir, char *dir, char *nom, char *realname,
				    char *imprimante, int pid, long thotWin, char *thotsch, char *thotdoc, char *traitement)
#else  /* __STDC__ */
static void         initImpression (user_orientation, thotdir, tempdir, dir, nom, realname, imprimante, pid, thotWin, thotsch, thotdoc, traitement)
int                 user_orientation;
char               *thotdir;
char               *tempdir;
char               *dir;
char               *nom;
char               *realname;
char               *imprimante;
int                 pid;
long                thotWin;
char               *thotsch;
char               *thotdoc;
char               *traitement;

#endif /* __STDC__ */
{
   char                cmd[800];
   char               *bak_name;
   char               *piv_name;

   if (user_orientation == 0)
      strcpy (orientation, "Portrait");
   else
      strcpy (orientation, "Paysage");

   sprintf (cmd, "/bin/mkdir %s\n", tempdir);
   system (cmd);
   sprintf (cmd, "chmod +rwx '%s'\n", tempdir);
   system (cmd);

   bak_name = (char *) TtaGetMemory (strlen (nom) + 5);
   sprintf (bak_name, "%s.BAK", nom);
   piv_name = (char *) TtaGetMemory (strlen (nom) + 5);
   sprintf (piv_name, "%s.PIV", nom);
   sprintf (cmd, "/bin/mv '%s'/'%s' '%s'/'%s'\n", dir, bak_name, tempdir, piv_name);
   system (cmd);
   sprintf (cmd, "traitement=%s\n", traitement);
   system (cmd);
   sprintf (cmd, "export traitement\n");
   system (cmd);
   sprintf (cmd, "realname=%s\n", realname);
   system (cmd);
   sprintf (cmd, "export realname\n");
   system (cmd);
   /*  sprintf (cmd, "printer_or_psname=%s\n", imprimante) ;
      system (cmd) ;
      sprintf (cmd, "export printer_or_psname\n") ;
      system (cmd) ; */
   sprintf (cmd, "thotpid=%d\n", pid);
   system (cmd);
   sprintf (cmd, "export pid\n");
   system (cmd);
   sprintf (cmd, "thotwindow=%ld\n", thotWin);
   system (cmd);
   sprintf (cmd, "export thotwindow\n");
   system (cmd);
   sprintf (cmd, "BIN=%s/bin\n", thotdir);
   system (cmd);
   sprintf (cmd, "export BIN\n");
   system (cmd);
   sprintf (cmd, "THOTDIR=%s\n", thotdir);
   system (cmd);
   sprintf (cmd, "export THOTDIR\n");
   system (cmd);
   sprintf (cmd, "THOTSCH=%s\n", thotsch);
   system (cmd);
   sprintf (cmd, "export THOTSCH\n");
   system (cmd);
   sprintf (cmd, "THOTDOC=%s:%s\n", tempdir, thotdoc);
   system (cmd);
   sprintf (cmd, "export THOTDOC\n");
   system (cmd);
}

/* ---------------------------------------------------------------------- */
/* |    Imprimer effectue le lancement du shell pour l'impression.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void          Imprimer (char *nom, char *dir, char *thotsch, char *thotdoc, char *thotpres, char *realname, char *realdir, char *imprimante, int pagedeb, int pagefin, int nbex, int decalage_h, int decalage_v, int user_orientation, int reduction, int nb_ppf, int suptrame, int alimmanuelle, int noiretblanc, int repaginer, char *vuesaimprimer)

#else  /* __STDC__ */
static void        Imprimer (nom, dir, thotsch, thotdoc, thotpres, realname, realdir, imprimante, pagedeb, pagefin, nbex, decalage_h, decalage_v, user_orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, repaginer, vuesaimprimer)
char               *nom;
char               *dir;
char               *thotsch;
char               *thotdoc;
char               *thotpres;
char               *realname;
char               *realdir;
char               *imprimante;
int                 pagedeb;
int                 pagefin;
int                 nbex;
int                 decalage_h;
int                 decalage_v;
int                 user_orientation;
int                 reduction;
int                 nb_ppf;
int                 suptrame;
int                 alimmanuelle;
int                 noiretblanc;
int                 repaginer;
char               *vuesaimprimer;

#endif /* __STDC__ */

{
   ThotPid             pid = ThotPid_get ();

#ifndef NEW_WILLOWS
   char                cmd[800];
   int                 res;
   char               *thotdir;
   char               *tempdir;

   thotdir = (char *) TtaGetEnvString ("THOTDIR");
   if (!thotdir)
	thotdir = ThotDir ();
   tempdir = (char *) TtaGetMemory (40);
   sprintf (tempdir, "/tmp/Thot%d", pid);

   initImpression (user_orientation, thotdir, tempdir, dir, nom, realname, imprimante, pid, FrRef[0], thotsch, thotdoc, "PRINT");
   if (imprimante[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s \"%s\" %s %d %d %d %s %d %d %d %d %d %ld Imprimer &\n",
	       DirectoryBinaries, nom, tempdir, repaginer, pagedeb, pagefin, vuesaimprimer, realname, imprimante, page_size, nbex,
	       decalage_h, decalage_v, orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, FrRef[0]);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %s %d %d %d %s %d %d %d %d %d %ld Imprimer &\n",
	       DirectoryBinaries, nom, tempdir, repaginer, pagedeb, pagefin, vuesaimprimer, realname, "lp", page_size, nbex,
	       decalage_h, decalage_v, orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, FrRef[0]);

   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, ERROR_PS_TRANSLATION);
#endif /* NEW_WILLOWS */
}


/* ---------------------------------------------------------------------- */
/* |    SauverPS effectue le lancement du shell pour sauvegarde PS.     | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void          SauverPS (char *nom, char *dir, char *thotsch, char *thotdoc, char *thotpres, char *realname, char *realdir, char *nomps, int pagedeb, int pagefin, int nbex, int decalage_h, int decalage_v, int user_orientation, int reduction, int nb_ppf, int suptrame, int alimmanuelle, int noiretblanc, int repaginer, char *vuesaimprimer)

#else  /* __STDC__ */
static void         SauverPS (nom, dir, thotsch, thotdoc, thotpres, realname, realdir, nomps, pagedeb, pagefin, nbex, decalage_h, decalage_v, user_orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, repaginer, vuesaimprimer)
char               *nom;
char               *dir;
char               *thotsch;
char               *thotdoc;
char               *thotpres;
char               *realname;
char               *realdir;
char               *nomps;
int                 pagedeb;
int                 pagefin;
int                 nbex;
int                 decalage_h;
int                 decalage_v;
int                 user_orientation;
int                 reduction;
int                 nb_ppf;
int                 suptrame;
int                 alimmanuelle;
int                 noiretblanc;
int                 repaginer;
char               *vuesaimprimer;

#endif /* __STDC__ */

{
   char                cmd[800];
   int                 res;
   char               *thotdir;

   char               *tempdir;
   ThotPid             pid = ThotPid_get ();

   thotdir = TtaGetEnvString ("THOTDIR");
   if (!thotdir)
     {
	thotdir = ThotDir ();
     }
   tempdir = (char *) TtaGetMemory (40);
   sprintf (tempdir, "/tmp/Thot%d", pid);
   initImpression (user_orientation, thotdir, tempdir, dir, nom, realname, nomps, pid, FrRef[0], thotsch, thotdoc, "SAVEPS");

   if (nomps[0] != '\0')
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %s %d %d %d %s %d %d %d %d %d %ld Sauver &\n",
	       DirectoryBinaries, nom, tempdir, repaginer, pagedeb, pagefin, vuesaimprimer, realname, nomps, page_size, nbex,
	       decalage_h, decalage_v, orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, FrRef[0]);
   else
      sprintf (cmd, "%s/print %s %s %d %d %d 0 %s %s %s %s %d %d %d %s %d %d %d %d %d %ld Sauver &\n",
	       DirectoryBinaries, nom, tempdir, repaginer, pagedeb, pagefin, vuesaimprimer, realname, "out.ps", page_size, nbex,
	       decalage_h, decalage_v, orientation, reduction, nb_ppf, suptrame, alimmanuelle, noiretblanc, FrRef[0]);

   res = system (cmd);
   if (res == -1)
      TtaDisplaySimpleMessage (CONFIRM, LIB, ERROR_PS_TRANSLATION);
}


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

   strcpy (pDocPrint->DocDirectory, "/tmp");
   strcpy (pDocPrint->DocDName, "ThotXXXXXX");
#ifdef WWW_MSWINDOWS
   _mktemp (pDocPrint->DocDName);
#else  /* WWW_MSWINDOWS */
   mktemp (pDocPrint->DocDName);
#endif /* !WWW_MSWINDOWS */

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
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, MANUAL_FEED));
   TtaNewToggleMenu (NumMenuOptions, NumFormImprimer,
		 TtaGetMessage (LIB, OPTIONS), 1, BufMenu, NULL, FALSE);
   if (AlimentationManuelle)
      TtaSetToggleMenu (NumMenuOptions, 0, TRUE);

   /* sous-menu format papier */
   i = 0;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, A4));
   i += strlen (&BufMenu[i]) + 1;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, US));
   TtaNewSubmenu (NumMenuFormatPapier, NumFormImprimer, 0,
	    TtaGetMessage (LIB, PAPER_SIZE), 2, BufMenu, NULL, FALSE);
   if (!strcmp (page_size, "US"))
      TtaSetMenuForm (NumMenuFormatPapier, 1);
   else
      TtaSetMenuForm (NumMenuFormatPapier, 0);

   /* sous-menu imprimer papier / sauver PostScript */
   i = 0;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, PRINTER));
   i += strlen (&BufMenu[i]) + 1;
   sprintf (&BufMenu[i], "%s%s", "B", TtaGetMessage (LIB, PS_FILE));
   TtaNewSubmenu (NumMenuSupport, NumFormImprimer, 0,
		  TtaGetMessage (LIB, OUTPUT), 2, BufMenu, NULL, TRUE);
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
