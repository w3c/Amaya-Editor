/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */
/* I. Vatton    Mai 1994 */

#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
#include "typemedia.h"
#include "app.h"
#include "appdialogue.h"
#include "tree.h"
#include "libmsg.h"
#include "message.h"
#include "dialog.h"
#include "application.h"
#include "document.h"
#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "appdialogue.var"
#include "environ.var"
#include "img.var"
#include "frame.var"
#include "opendoc.f"
#include "createdoc.f"
extern int          CurrentDialog;
static PathBuffer   NomDirectorySauver;
static PathBuffer   NomFichierSauver;
static PathBuffer   NomSchemaTraduction;
static boolean      SauveDocAvecCopie;
static boolean      SauveDocAvecMove;

#ifdef __STDC__
extern void         NomFonte (char, char, int, int, TypeUnit, char[], char[]);
extern boolean      SauverDoc (PtrDocument, Name, PathBuffer, boolean, boolean);
extern int          IdentDocument (PtrDocument);
extern boolean      ThotSendMessage (NotifyEvent *, boolean);
extern void         DoFileName (Name, char *, PathBuffer, PathBuffer, int *);
extern void         ExportDocument (PtrDocument, char *, char *);
extern boolean      TtaIsSuffixFileIn (char *, char *);
extern void         ConfigGetNomExportSchema (int, char *);
extern int          FileExist (char *);
extern int          ConfigMakeMenuExport (char *, char *);
extern void         EntreeMenu (int *, char *, char[]);
extern int          GetWindowNumber (Document, View);
extern boolean      SauveDocument (PtrDocument, int);

#else  /* __STDC__ */
extern void         NomFonte ();
extern boolean      SauverDoc ();
extern int          IdentDocument ();
extern boolean      ThotSendMessage ();
extern void         DoFileName ();
extern void         ExportDocument ();
extern boolean      TtaIsSuffixFileIn ();
extern void         ConfigGetNomExportSchema ();
extern int          FileExist ();
extern int          ConfigMakeMenuExport ();
extern void         EntreeMenu ();
extern int          GetWindowNumber ();
extern boolean      SauveDocument ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* | DesactiveEntree visualise comme non active l'entree ent du menu    | */
/* | dont la re'fe'rence est passe'e en parame`tre.                     | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                DesactiveEntree (int ref, int ent)
#else  /* __STDC__ */
void                DesactiveEntree (ref, ent)
int                 ref;
int                 ent;

#endif /* __STDC__ */
{
   char                fontname[100];
   char                texte[20];

   if (Gdepth (0) > 1)
      TtaRedrawMenuEntry (ref, ent, NULL, InactiveB_Color, 0);
   else
     {
	NomFonte ('L', 'T', 2, 11, 1, texte, fontname);
	TtaRedrawMenuEntry (ref, ent, fontname, -1, 0);
     }
}				/*DesactiveEntree */

/* ----------------------------------------------------------------------- */
/* | TraiteSauverDoc traite la Sauvegarde du Fichier                     | */
/* ----------------------------------------------------------------------- */
void                TraiteSauverDoc ()

{
   PathBuffer          NomFichierSortie;
   int                 i;
   NotifyDialog        notifyDoc;

   /* Name du fichier a sauver */
   strcpy (NomFichierSortie, NomDirectorySauver);
   strcat (NomFichierSortie, DIR_STR);
   strcat (NomFichierSortie, NomFichierSauver);
   if (NomSchemaTraduction[0] == '\0')
      /* sauver en format Thot */
     {
	strcat (NomFichierSortie, ".PIV");
	(void) SauverDoc (DocumentASauver,
			  NomFichierSauver, NomDirectorySauver,
			  SauveDocAvecCopie, SauveDocAvecMove);
     }
   else
      /* exporter le document */
     {
	/* envoie le message DocExport.Pre a l'application */
	notifyDoc.event = TteDocExport;
	notifyDoc.document = (Document) IdentDocument (DocumentASauver);
	notifyDoc.view = 0;
	if (!ThotSendMessage ((NotifyEvent *) & notifyDoc, True))
	   /* l'application accepte que Thot exporte le document */
	  {
	     TtaDisplaySimpleMessageString (LIB, INFO, LIB_EXPORTING, DocumentASauver->DocDName);
	     DoFileName (NomFichierSauver, "", NomDirectorySauver, NomFichierSortie, &i);
	     ExportDocument (DocumentASauver, NomFichierSortie, NomSchemaTraduction);
	     TtaDisplaySimpleMessageString (LIB, INFO, LIB_DOC_WRITTEN, NomFichierSortie);
	     /* envoie le message DocExport.Post a l'application */
	     notifyDoc.event = TteDocExport;
	     notifyDoc.document = (Document) IdentDocument (DocumentASauver);
	     notifyDoc.view = 0;
	     ThotSendMessage ((NotifyEvent *) & notifyDoc, False);
	  }
     }
}

/* ----------------------------------------------------------------------- */
/* | RetMenuSauverDoc traite les retours du menu Sauver Fichier          | */
/* ----------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuSauverDoc (int ref, int typedata, char *txt)

#else  /* __STDC__ */
void                RetMenuSauverDoc (ref, typedata, txt)
int                 ref;
int                 typedata;
char               *txt;

#endif /* __STDC__ */

{
   PathBuffer          ptNomTrad;
   PathBuffer          BufDir;
   int                 i, nbitem;
   int                 val;

   val = (int) txt;
   switch (ref)
	 {
	    case NumZoneNomDocASauver:
	       /* zone de saisie du nom du document a creer */
	       if (TtaCheckDirectory (txt) && txt[strlen (txt) - 1] != DIR_SEP)
		 {
		    strcpy (NomDirectorySauver, txt);
		    NomFichierSauver[0] = '\0';
		 }
	       else
		 {
		    /* conserve le nom du document a ouvrir */
		    TtaExtractName (txt, NomDirectorySauver, BufDir);
		    /* RemoveElement le suffixe .PIV du nom de fichier */
		    i = strlen (BufDir) - 4;
		    if (!strcmp (&BufDir[i], ".PIV"))
		       BufDir[i] = '\0';
		    else
		       i += 4;
		    if (i >= MAX_NAME_LENGTH - 1)
		      {
			 i = MAX_NAME_LENGTH - 1;		/*Longueur du nom limitee */
			 BufDir[i] = '\0';
			 strcpy (ptNomTrad, NomDirectorySauver);
			 strcat (ptNomTrad, DIR_STR);
			 strcat (ptNomTrad, BufDir);
			 if (NomSchemaTraduction[0] == '\0')
			    strcat (ptNomTrad, ".PIV");
			 /* reinitialise la zone du nom de document */
			 TtaSetTextForm (NumZoneNomDocASauver, ptNomTrad);
		      }
		    strcpy (NomFichierSauver, BufDir);
		 }

	       if (TtaCheckDirectory (NomDirectorySauver))
		 {
		    /* Est-ce un nouveau directory qui contient des documents */
		    if (!TtaIsInDocumentPath (NomDirectorySauver))
		       if (TtaIsSuffixFileIn (NomDirectorySauver, ".PIV"))
			 {
			    /* il faut ajouter le directory au path */
			    i = strlen (DirectoryDoc);
			    if (i + strlen (NomDirectorySauver) + 2 < MAX_PATH)
			      {
				 strcat (DirectoryDoc, PATH_STR);
				 strcat (DirectoryDoc, NomDirectorySauver);
				 BuildPathDocBuffer (BufDir, '\0', &nbitem);
				 TtaNewSelector (NumZoneDirDocASauver, NumFormSauverComme,
						 TtaGetMessage (LIB, LIB_DOCUMENT_DIRECTORIES),
				      nbitem, BufDir, 6, NULL, False, True);
			      }
			 }
		 }

	       break;
	    case NumZoneDirDocASauver:
	       /* zone de saisie du directory ou le document doit etre cree */
	       strcpy (NomDirectorySauver, txt);
	       strcpy (ptNomTrad, NomDirectorySauver);
	       strcat (ptNomTrad, DIR_STR);
	       strcat (ptNomTrad, NomFichierSauver);
	       if (NomSchemaTraduction[0] == '\0')
		  strcat (ptNomTrad, ".PIV");
	       /* reinitialise la zone du nom de document */
	       TtaSetTextForm (NumZoneNomDocASauver, ptNomTrad);
	       break;
	    case NumMenuFormatDocASauver:
	       /* sous-menu pour le choix du format de sauvegarde */
	       strcpy (ptNomTrad, NomDirectorySauver);
	       strcat (ptNomTrad, DIR_STR);
	       strcat (ptNomTrad, NomFichierSauver);
	       if (val == 0)
		 {
		    /* premiere entree du menu format: format Thot */
		    NomSchemaTraduction[0] = '\0';
		    strcat (ptNomTrad, ".PIV");
		    TtaRedrawMenuEntry (NumMenuCopierOuRenommer, 0, NULL, -1, 1);
		    TtaRedrawMenuEntry (NumMenuCopierOuRenommer, 1, NULL, -1, 1);
		 }
	       else
		 {
		    ConfigGetNomExportSchema (val, NomSchemaTraduction);
		    DesactiveEntree (NumMenuCopierOuRenommer, 0);
		    DesactiveEntree (NumMenuCopierOuRenommer, 1);
		 }
	       /* reinitialise la zone du nom de document */
	       TtaSetTextForm (NumZoneNomDocASauver, ptNomTrad);
	       break;
	    case NumMenuCopierOuRenommer:
	       /* sous-menu copier/renommer un document */
	       if (val == 0)
		  /* c'est un copy */
		 {
		    SauveDocAvecCopie = True;
		    SauveDocAvecMove = False;
		 }
	       else
		  /* c'est un move */
		 {
		    SauveDocAvecCopie = False;
		    SauveDocAvecMove = True;
		 }
	       break;
	    case NumFormSauverComme:
	       /* le formulaire Sauver Comme */
	       /* fait disparaitre la feuille de dialogue */
	       if (val == 1)
		 {
		    /* c'est une confirmation */
		    TtaUnmapDialogue (NumFormSauverComme);
		    CurrentDialog = NumFormSauverComme;
		    if (DocumentASauver != NULL)
		       if (DocumentASauver->DocSSchema != NULL)
			  /* le document a sauver n'a pas ete ferme' entre temps */
			  if (!TtaCheckDirectory (NomDirectorySauver))
			    {	/* le repertoire est invalide : affiche un message et detruit les dialogues */
			       TtaDisplaySimpleMessageString (LIB, CONFIRM, LIB_DIRECTORY_NOT_FOUND, NomDirectorySauver);
			       if (ThotLocalActions[T_confirmcreate] != NULL)
				  (*ThotLocalActions[T_confirmcreate])
				     (0, 1, (char *) 1);
			    }
			  else if (!strcmp (NomDirectorySauver, DocumentASauver->DocDirectory)
				   && !strcmp (NomFichierSauver, DocumentASauver->DocDName)
				   && NomSchemaTraduction[0] == '\0')
			    {	/* traite la confirmation */
			       if (ThotLocalActions[T_confirmcreate] != NULL)
				  (*ThotLocalActions[T_confirmcreate]) (NumFormConfirm, 1, (char *) 1);
			    }
			  else
			    {
			       strcpy (ptNomTrad, NomDirectorySauver);
			       strcat (ptNomTrad, DIR_STR);
			       strcat (ptNomTrad, NomFichierSauver);
			       if (NomSchemaTraduction[0] == '\0')
				  strcat (ptNomTrad, ".PIV");
			       if (FileExist (ptNomTrad))
				 {
				    /* demande confirmation */
				    sprintf (BufDir, TtaGetMessage (LIB, LIB_FILE_EXIST), ptNomTrad);
				    TtaNewLabel (NumLabelConfirm, NumFormConfirm, BufDir);
				    TtaShowDialogue (NumFormConfirm, False);
				 }

			       else
				  /* traite la confirmation */
			       if (ThotLocalActions[T_confirmcreate] != NULL)
				  (*ThotLocalActions[T_confirmcreate]) (NumFormConfirm, 1, (char *) 1);
			    }
		 }
	       else if (ThotLocalActions[T_confirmcreate] != NULL)
		  (*ThotLocalActions[T_confirmcreate]) (0, 1, (char *) 1);
	       break;
	 }
}

/* ---------------------------------------------------------------------- */
/* |    SauveDocComme   cree, initialise et active le formulaire "Sauver comme"| */
/* |            pour le document pDoc.                                   | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         SauveDocComme (PtrDocument pDoc)
#else  /* __STDC__ */
static void         SauveDocComme (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   int                 nbitem;
   char                BufMenu[MAX_TXT_LEN];
   char                BufMenuB[MAX_TXT_LEN];
   char                BufDir[MAX_PATH];
   char               *src;
   char               *dest;
   int                 i, k, l, Indx;

   if (pDoc != NULL)
      if (!pDoc->DocReadOnly)
	 if (pDoc->DocSSchema != NULL)
	   {
	      DocumentASauver = pDoc;
	      /* cree le formaulaire Sauver comme */
	      TtaNewSheet (NumFormSauverComme, 0, 0, 0,
			   TtaGetMessage (LIB, LIB_SAVE_AS),
		  1, TtaGetMessage (LIB, LIB_SAVE), True, 3, 'L', D_CANCEL);

	      /* cree et */
	      /* initialise le selecteur sur aucune entree */
	      BuildPathDocBuffer (BufDir, '\0', &nbitem);
	      TtaNewSelector (NumZoneDirDocASauver, NumFormSauverComme,
			      TtaGetMessage (LIB, LIB_DOCUMENT_DIRECTORIES),
			      nbitem, BufDir, 6, NULL, False, True);

	      TtaSetSelector (NumZoneDirDocASauver, -1, "");
	      /* initialise le titre du formulaire Sauver Comme */
	      strcpy (NomFichierSauver, pDoc->DocDName);
	      strcpy (NomDirectorySauver, pDoc->DocDirectory);
	      /* compose le menu des formats de sauvegarde applicables */
	      /* a ce document, d'apres sa classe */
	      nbitem = ConfigMakeMenuExport (pDoc->DocSSchema->SsName, BufMenu);
	      /* met le format Thot en tete */
	      BufMenuB[0] = 'B';
	      strcpy (&BufMenuB[1], TtaGetMessage (LIB, LIB_THOT));
	      l = strlen (TtaGetMessage (LIB, LIB_THOT)) + 2;
	      /* ajoute 'B' au debut de chaque entree */
	      dest = &BufMenuB[l];
	      src = &BufMenu[0];
	      for (k = 1; k <= nbitem; k++)
		{
		   strcpy (dest, "B");
		   dest++;
		   l = strlen (src);
		   strcpy (dest, src);
		   dest += l + 1;
		   src += l + 1;
		}
	      nbitem++;
	      TtaNewSubmenu (NumMenuFormatDocASauver, NumFormSauverComme, 0,
			     TtaGetMessage (LIB, LIB_DOC_FORMAT), nbitem, BufMenuB, NULL, True);
	      TtaSetMenuForm (NumMenuFormatDocASauver, 0);
	      /* sous-menu copier/renommer un document */
	      Indx = 0;
	      sprintf (&BufMenu[Indx], "%s%s", "B", TtaGetMessage (LIB, LIB_COPY));
	      Indx += strlen (&BufMenu[Indx]) + 1;
	      sprintf (&BufMenu[Indx], "%s%s", "B", TtaGetMessage (LIB, LIB_RENAME));
	      TtaNewSubmenu (NumMenuCopierOuRenommer, NumFormSauverComme, 0,
		    TtaGetMessage (LIB, LIB_SAVE), 2, BufMenu, NULL, False);
	      TtaSetMenuForm (NumMenuCopierOuRenommer, 0);
	      /* initialise le  nom de document propose */
	      strcpy (BufMenu, TtaGetMessage (LIB, LIB_SAVE));
	      strcat (BufMenu, " ");
	      strcat (BufMenu, pDoc->DocDName);
	      TtaChangeFormTitle (NumFormSauverComme, BufMenu);
	      strcpy (BufMenu, NomDirectorySauver);
	      strcat (BufMenu, DIR_STR);
	      strcat (BufMenu, NomFichierSauver);
	      strcat (BufMenu, ".PIV");
	      /* nom de document propose' */
	      TtaNewTextForm (NumZoneNomDocASauver, NumFormSauverComme,
		       TtaGetMessage (LIB, LIB_DOCUMENT_NAME), 50, 1, True);
	      TtaSetTextForm (NumZoneNomDocASauver, BufMenu);

/*        ActiveEntree(NumMenuCopierOuRenommer, 0); */
/*        ActiveEntree(NumMenuCopierOuRenommer, 1); */
/*        TtaSetMenuForm(NumMenuCopierOuRenommer, 0); */
	      /* premiere entree du menu format: format Thot */
	      NomSchemaTraduction[0] = '\0';
	      SauveDocAvecCopie = True;
	      SauveDocAvecMove = False;
	      /* Formulaire Confirmation creation */
	      /* ++++++++++++++++++++++++++++++++ */
	      strcpy (BufMenu, TtaGetMessage (LIB, LIB_SAVE_AS));
	      i = strlen (BufMenu) + 1;
	      strcpy (&BufMenu[i], TtaGetMessage (LIB, LIB_CONFIRM));
	      TtaNewDialogSheet (NumFormConfirm, 0, 0, 0, NULL, 2, BufMenu, False, 1, 'L', D_CANCEL);

	      /* affiche le formulaire */
	      TtaShowDialogue (NumFormSauverComme, False);
	   }
}

/* -------------------------------------------------------------- */
#ifdef __STDC__
void                TtcSaveDocumentAs (Document document, View view)
#else  /* __STDC__ */
void                TtcSaveDocumentAs (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 frame;

   if (document != 0)
     {
	pDoc = TabDocuments[document - 1];
	if (pDoc != NULL)
	   /* il y a un document pour cette entree de la table */
	  {
	     if (ThotLocalActions[T_savedoc] == NULL)
	       {
		  TteConnectAction (T_savedoc, (Proc) TraiteSauverDoc);
		  TteConnectAction (T_confirmcreate, (Proc) retconfirm);
		  TteConnectAction (T_rsavedoc, (Proc) RetMenuSauverDoc);
		  TteConnectAction (T_buildpathdocbuffer, (Proc) BuildPathDocBuffer);
	       }
	     frame = GetWindowNumber (document, view);
	     if (ThotLocalActions[T_update_paragraph] != NULL)
		(*ThotLocalActions[T_update_paragraph]) (FntrTable[frame - 1].FrAbstractBox, frame);
	     SauveDocComme (pDoc);
	  }
     }
}


/* -------------------------------------------------------------- */
#ifdef __STDC__
void                TtcSaveDocument (Document document, View view)
#else  /* __STDC__ */
void                TtcSaveDocument (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrDocument         pDoc;
   int                 frame;

   if (document != 0)
     {
	pDoc = TabDocuments[document - 1];
	if (pDoc != NULL)
	   /* il y a un document pour cette entree de la table */
	  {
	     frame = GetWindowNumber (document, view);
	     if (ThotLocalActions[T_update_paragraph] != NULL)
		(*ThotLocalActions[T_update_paragraph]) (FntrTable[frame - 1].FrAbstractBox, frame);
	     pDoc->DocModified = !SauveDocument (pDoc, 4);
	  }
     }
}
