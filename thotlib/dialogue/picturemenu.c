/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   editmenu.c : fonctions d'edition -- menus fichier, vue, import export.
 */

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


/*  */
#include "picture.h"
#undef EXPORT
#define EXPORT extern
#define MyNumMenuCadrageImage		1
#define MyNumZoneDirImage		2
#define MyNumFormImage		        3
#define MyNumZoneFichierImage		4
#define MyNumSelImage		        5
#define MyNumMenuTypeImage		6
#define IMAGE_MENU_MAX			7
extern PathBuffer   DocumentPath;
extern PathBuffer   SchemaPath;
extern char        *SuffixImage[];

#undef EXPORT
#define EXPORT static

static int          IndexTypeImage, IndexPresImage, BaseDlgImage;
static boolean      redisplayPicture;
static char         NomImage[100] = "";
static char         DirectoryImage[MAX_PATH] = "";

#include "picture_f.h"

#ifdef __STDC__
extern void         DesactiveEntree (int, int);
extern boolean      TtaIsSuffixFileIn (char *, char *);
extern void         TtaListDirectory (char *, int, char *, int, char *, char *, int);
extern void         BuildFileName (Name, char *, PathBuffer, PathBuffer, int *);
extern void         EntreeMenu (int *, char *, char[]);

#else
extern void         DesactiveEntree ();
extern boolean      TtaIsSuffixFileIn ();
extern void         TtaListDirectory ();
extern void         BuildFileName ();
extern void         EntreeMenu ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    CheckPresImage valide ou invalide le type de presentation en    | */
/* |            fonction du type d'image.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         CheckPresImage (int indexType)
#else  /* __STDC__ */
static void         CheckPresImage (indexType)
int                 indexType;

#endif /* __STDC__ */

{
   if (indexType == 1)
     {
	/* Le strech est autorise pour les images EPSF */
	TtaRedrawMenuEntry (BaseDlgImage + MyNumMenuCadrageImage, 1, NULL, -1, 1);
	TtaRedrawMenuEntry (BaseDlgImage + MyNumMenuCadrageImage, 2, NULL, -1, 1);
     }
   else
     {
	/* Le strech n'est pas autorise pour les autres images */
	DesactiveEntree (BaseDlgImage + MyNumMenuCadrageImage, 1);
	DesactiveEntree (BaseDlgImage + MyNumMenuCadrageImage, 2);
	IndexPresImage = 0;
	TtaSetMenuForm (BaseDlgImage + MyNumMenuCadrageImage, IndexPresImage);
     }
}

/* ---------------------------------------------------------------------- */
/* |    InitPathImage   initialise les paths des images.                | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         InitPathImage ()

#else  /* __STDC__ */
static void         InitPathImage ()
#endif				/* __STDC__ */

{
   char                BufDir[MAX_PATH * 2];
   int                 i, j;
   int                 nb;
   int                 max;

   /* remplace ':' par '\0' pour le formulaire de saisie des images */
   nb = 1;
   max = MAX_PATH * 2;
   j = 0;
   /* paths des schemas */
   for (i = 0; i < MAX_PATH && SchemaPath[i] != '\0'; i++)
      if (SchemaPath[i] == PATH_SEP)
	{
	   if (BufDir[j - 1] != '\0')
	     {
		BufDir[j++] = '\0';
		nb++;
	     }
	}
      else
	 BufDir[j++] = SchemaPath[i];

   if (j < max)
      BufDir[j++] = '\0';
   else
      BufDir[j - 1] = '\0';

   /* paths des documents */
   nb++;
   for (i = 0; i < MAX_PATH && DocumentPath[i] != '\0'; i++)
      if (DocumentPath[i] == PATH_SEP)
	{
	   if (BufDir[j - 1] != '\0')
	     {
		BufDir[j++] = '\0';
		nb++;
	     }
	}
      else
	 BufDir[j++] = DocumentPath[i];

   if (j < max)
      BufDir[j] = '\0';
   else
      BufDir[j - 1] = '\0';
   TtaNewSelector (BaseDlgImage + MyNumZoneDirImage, BaseDlgImage + MyNumFormImage, "Dossiers documents",
		   nb, BufDir, 9, NULL, FALSE, TRUE);
}


/* ----------------------------------------------------------------------- */
/* |  RetMenuImage enregistre les retours du formulaire Picture            | */
/* ----------------------------------------------------------------------- */
#ifdef __STDC__
void                RetMenuImage (int ref, int typedata, char *txt)

#else  /* __STDC__ */
void                RetMenuImage (ref, typedata, txt)
int                 ref;
int                 typedata;
char               *txt;

#endif /* __STDC__ */
{
   PathBuffer          nomcomplet;
   int                 i, val;

   val = (int) txt;

   switch (ref - BaseDlgImage)
	 {
	    case MyNumZoneFichierImage:
	       if (TtaCheckDirectory (txt) && txt[strlen (txt) - 1] != DIR_SEP)
		 {
		    strcpy (DirectoryImage, txt);
		    NomImage[0] = '\0';
		 }
	       else
		 {
		    /* conserve le nom du document a ouvrir */
		    TtaExtractName (txt, DirectoryImage, NomImage);
		    if (NomImage[0] == '\0' && !TtaCheckDirectory (DirectoryImage))
		      {
			 /* Le texte correspond au nom de l'image sans directory */
			 strncpy (NomImage, DirectoryImage, 100);
			 DirectoryImage[0] = '\0';
		      }
		 }

	       if (TtaCheckDirectory (DirectoryImage))
		 {
		    /* Est-ce un nouveau directory qui contient des documents */
		    if (!TtaIsInDocumentPath (DirectoryImage))
		       if (TtaIsSuffixFileIn (DirectoryImage, SuffixImage[IndexTypeImage]))
			 {
			    /* il faut ajouter le directory au path */
			    i = strlen (DocumentPath);
			    if (i + strlen (DirectoryImage) + 2 < MAX_PATH)
			      {
				 strcat (DocumentPath, PATH_STR);
				 strcat (DocumentPath, DirectoryImage);
				 InitPathImage ();
				 TtaListDirectory (DirectoryImage, BaseDlgImage + MyNumFormImage, NULL, -1,
						   SuffixImage[IndexTypeImage], "Fichiers", BaseDlgImage + MyNumSelImage);
			      }
			 }
		 }
	       break;
	    case MyNumSelImage:
	       if (DirectoryImage[0] == '\0')
		 {
		    /* compose le path complet du fichier pivot */
		    strncpy (DirectoryImage, DocumentPath, MAX_PATH);
		    /* recheche indirectement le directory */
		    BuildFileName (txt, "", DirectoryImage, nomcomplet, &i);
		    /* separe directory et nom */
		    TtaExtractName (nomcomplet, DirectoryImage, NomImage);
		 }
	       else
		 {
		    strcpy (nomcomplet, DirectoryImage);
		    strcat (nomcomplet, "/");
		    strcat (nomcomplet, txt);
		    strcpy (NomImage, txt);
		 }
	       TtaSetTextForm (BaseDlgImage + MyNumZoneFichierImage, nomcomplet);
	       break;
	    case MyNumMenuTypeImage:
	       if (val != IndexTypeImage)
		 {
		    IndexTypeImage = val;
		    if (TtaCheckDirectory (DirectoryImage))
		      {
			 /* Est-ce un nouveau directory qui contient des documents */
			 if (!TtaIsInDocumentPath (DirectoryImage))
			    if (TtaIsSuffixFileIn (DirectoryImage, SuffixImage[IndexTypeImage]))
			      {
				 /* il faut ajouter le directory au path */
				 i = strlen (DocumentPath);
				 if (i + strlen (DirectoryImage) + 2 < MAX_PATH)
				   {
				      strcat (DocumentPath, ":");
				      strcat (DocumentPath, DirectoryImage);
				      InitPathImage ();
				   }
			      }
		      }
		    TtaListDirectory (DirectoryImage, BaseDlgImage + MyNumFormImage, NULL, -1,
				      SuffixImage[IndexTypeImage], "Fichiers", BaseDlgImage + MyNumSelImage);
		    CheckPresImage (val);
		 }
	       break;
	    case MyNumMenuCadrageImage:
	       if (val != IndexPresImage)
		 {
		    IndexPresImage = val;
		    /* Faut-il mettre a jour la liste des fichiers */
		    if (DirectoryImage[0] != '\0')
		       TtaListDirectory (DirectoryImage, BaseDlgImage + MyNumFormImage, NULL, -1,
					 SuffixImage[IndexTypeImage], "Fichiers", BaseDlgImage + MyNumSelImage);
		 }
	       break;
	    case MyNumZoneDirImage:
	       strcpy (DirectoryImage, txt);
	       TtaSetTextForm (BaseDlgImage + MyNumZoneFichierImage, DirectoryImage);
	       TtaListDirectory (DirectoryImage, BaseDlgImage + MyNumFormImage, NULL, -1,
				 SuffixImage[IndexTypeImage], "Fichiers", BaseDlgImage + MyNumSelImage);
	       break;
	    case MyNumFormImage:
	       if (val == 1)
		  /* Edition realisee */
		  redisplayPicture = TRUE;
	       TtaDestroyDialogue (BaseDlgImage + MyNumFormImage);
	       break;
	    default:
	       break;
	 }
}


/* ----------------------------------------------------------------------- */
/* |  Menu appele lors de la modification d'une image                    | */
/* ----------------------------------------------------------------------- */
#ifdef __STDC__
void MenuImage (char *nom, boolean *result, int *typim, int *pres, PtrBox pBox)

#else  /* __STDC__ */
void MenuImage (nom, result, typim, pres, pBox)
char               *nom;
boolean            *result;
int                *typim;
int                *pres;
int                *pres;
PtrBox            pBox;

#endif /* __STDC__ */
{
   int                 i, Indx;
   char                BufTypeImage[MAX_TXT_LEN];
   char               *Source;
   int                 imageTypeCount, longueur;
   char                BufMenu[MAX_TXT_LEN];
   PictInfo    *image;


   IndexTypeImage = GetImTypeIndex (*typim);
   IndexPresImage = GetImPresIndex (*pres);
   strcpy (NomImage, nom);
   DirectoryImage[0] = '\0';
   redisplayPicture = FALSE;

   TtaNewForm (BaseDlgImage + MyNumFormImage, 0, 0, 0, "Picture", TRUE, 2, 'L', D_DONE);
   InitPathImage ();

   /* liste des fichiers existants */
   TtaListDirectory ("", BaseDlgImage + MyNumFormImage, NULL, -1, "", "Fichiers", BaseDlgImage + MyNumSelImage);

   /* sous-menu des types d'image du formulaire Picture */
   Indx = 0;
   GetPictHandlersList (&imageTypeCount, BufTypeImage);
   /* ajoute un 'B' au debut de chaque entree du menu construit */
   Source = &BufTypeImage[0];
   for (i = 1; i <= imageTypeCount; i++)
     {
	BufMenu[Indx] = 'B';
	Indx++;
	longueur = strlen (Source) + 1;
	if (Indx + longueur < MAX_TXT_LEN)
	  {
	     strcpy ((BufMenu) + Indx, Source);
	     Indx += longueur;
	  }
	Source += longueur;
     }
   TtaNewSubmenu (BaseDlgImage + MyNumMenuTypeImage, BaseDlgImage + MyNumFormImage, 0, "Type image",




		  imageTypeCount, BufMenu, NULL, TRUE);

   /* sous-menu cadrage du formulaire Picture */
   Indx = 0;
   sprintf (&BufMenu[Indx], "%s%s", "B", "Sans Modification");
   Indx += strlen (&BufMenu[Indx]) + 1;
   sprintf (&BufMenu[Indx], "%s%s", "B", "ReScale");
   Indx += strlen (&BufMenu[Indx]) + 1;
   sprintf (&BufMenu[Indx], "%s%s", "B", "Plein Cadre");
   Indx += strlen (&BufMenu[Indx]) + 1;

   TtaNewSubmenu (BaseDlgImage + MyNumMenuCadrageImage, BaseDlgImage + MyNumFormImage, 0,
		  "Affichage", 3, BufMenu, NULL, FALSE);

   /* zone de saisie du nom du fichier image */
   TtaNewTextForm (BaseDlgImage + MyNumZoneFichierImage, BaseDlgImage + MyNumFormImage,
		   "Name du fichier", 50, 1, TRUE);

   TtaSetSelector (BaseDlgImage + MyNumZoneDirImage, -1, "");
   TtaSetTextForm (BaseDlgImage + MyNumZoneFichierImage, nom);
   TtaSetMenuForm (BaseDlgImage + MyNumMenuTypeImage, IndexTypeImage);
   TtaSetMenuForm (BaseDlgImage + MyNumMenuCadrageImage, IndexPresImage);
   CheckPresImage (IndexTypeImage);
   /* active le formulaire */
   TtaShowDialogue (BaseDlgImage + MyNumFormImage, FALSE);
   /* attend le retour du formulaire */
   TtaWaitShowDialogue ();
   if (NomImage[0] == '\0')
      redisplayPicture = FALSE;

   if (redisplayPicture)
     {
	strcpy (nom, NomImage);
	*typim = GetPictureType (IndexTypeImage);
	*pres = (PictureScaling) (IndexPresImage);
	image = (PictInfo *) pBox->BxPictInfo;
	strcpy (image->PicFileName, nom);
	image->PicPresent = *pres;
	image->PicType = *typim;
     }
   *result = redisplayPicture;
}
/* ----------------------------------------------------------------------- */
void                ImageMenuLoadResources ()
{
   BaseDlgImage = TtaSetCallback (RetMenuImage, IMAGE_MENU_MAX);
   if (BaseDlgImage != 0)
      TteConnectAction (T_imagemenu, (Proc) MenuImage);
}
