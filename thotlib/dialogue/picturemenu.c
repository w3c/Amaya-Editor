/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
   fonctions d'edition -- menus fichier, vue, import export.
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
#define MY_NUM_MENU_IMAGE_FRAME		1
#define MY_NUM_ZONE_DIR_IMAGE		2
#define MY_NUM_IMAGE_FORM		3
#define MY_NUM_ZONE_IMAGE_FILE		4
#define MY_NUM_IMAGE_SEL		5
#define MY_NUM_MENU_IMAGE_TYPE		6
#define MAX_IMAGE_MENU			7
extern PathBuffer   DocumentPath;
extern PathBuffer   SchemaPath;
extern char        *FileExtension[];

#undef EXPORT
#define EXPORT static

static int          IndexTypeImage, IndexPresImage, BaseDlgImage;
static boolean      RedisplayPicture;
static char         ImageName[100] = "";
static char         DirectoryImage[MAX_PATH] = "";

#include "picture_f.h"

#ifdef __STDC__
extern void         UnsetEntryMenu (int, int);
extern boolean      TtaIsSuffixFileIn (char *, char *);
extern void         TtaListDirectory (char *, int, char *, int, char *, char *, int);
extern void         MakeCompleteName (Name, char *, PathBuffer, PathBuffer, int *);
extern void         EntreeMenu (int *, char *, char[]);

#else
extern void         UnsetEntryMenu ();
extern boolean      TtaIsSuffixFileIn ();
extern void         TtaListDirectory ();
extern void         MakeCompleteName ();
extern void         EntreeMenu ();

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   CheckPresImage valide ou invalide le type de presentation en    
   fonction du type d'image.                               
  ----------------------------------------------------------------------*/
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
	TtaRedrawMenuEntry (BaseDlgImage + MY_NUM_MENU_IMAGE_FRAME, 1, NULL, -1, 1);
	TtaRedrawMenuEntry (BaseDlgImage + MY_NUM_MENU_IMAGE_FRAME, 2, NULL, -1, 1);
     }
   else
     {
	/* Le strech n'est pas autorise pour les autres images */
	UnsetEntryMenu (BaseDlgImage + MY_NUM_MENU_IMAGE_FRAME, 1);
	UnsetEntryMenu (BaseDlgImage + MY_NUM_MENU_IMAGE_FRAME, 2);
	IndexPresImage = 0;
	TtaSetMenuForm (BaseDlgImage + MY_NUM_MENU_IMAGE_FRAME, IndexPresImage);
     }
}

/*----------------------------------------------------------------------
   InitPathImage   initialise les paths des images.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitPathImage ()

#else  /* __STDC__ */
static void         InitPathImage ()
#endif				/* __STDC__ */

{
   char                bufDir[MAX_PATH * 2];
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
	   if (bufDir[j - 1] != '\0')
	     {
		bufDir[j++] = '\0';
		nb++;
	     }
	}
      else
	 bufDir[j++] = SchemaPath[i];

   if (j < max)
      bufDir[j++] = '\0';
   else
      bufDir[j - 1] = '\0';

   /* paths des documents */
   nb++;
   for (i = 0; i < MAX_PATH && DocumentPath[i] != '\0'; i++)
      if (DocumentPath[i] == PATH_SEP)
	{
	   if (bufDir[j - 1] != '\0')
	     {
		bufDir[j++] = '\0';
		nb++;
	     }
	}
      else
	 bufDir[j++] = DocumentPath[i];

   if (j < max)
      bufDir[j] = '\0';
   else
      bufDir[j - 1] = '\0';
   TtaNewSelector (BaseDlgImage + MY_NUM_ZONE_DIR_IMAGE, BaseDlgImage + MY_NUM_IMAGE_FORM, "Dossiers documents",
		   nb, bufDir, 9, NULL, FALSE, TRUE);
}


/*----------------------------------------------------------------------
   CallbackPictureMenu enregistre les retours du formulaire Picture            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackPictureMenu (int ref, int typeData, char *txt)

#else  /* __STDC__ */
void                CallbackPictureMenu (ref, typeData, txt)
int                 ref;
int                 typeData;
char               *txt;

#endif /* __STDC__ */
{
   PathBuffer          completeName;
   int                 i, val;

   val = (int) txt;

   switch (ref - BaseDlgImage)
	 {
	    case MY_NUM_ZONE_IMAGE_FILE:
	       if (TtaCheckDirectory (txt) && txt[strlen (txt) - 1] != DIR_SEP)
		 {
		    strcpy (DirectoryImage, txt);
		    ImageName[0] = '\0';
		 }
	       else
		 {
		    /* conserve le nom du document a ouvrir */
		    TtaExtractName (txt, DirectoryImage, ImageName);
		    if (ImageName[0] == '\0' && !TtaCheckDirectory (DirectoryImage))
		      {
			 /* Le texte correspond au nom de l'image sans directory */
			 strncpy (ImageName, DirectoryImage, 100);
			 DirectoryImage[0] = '\0';
		      }
		 }

	       if (TtaCheckDirectory (DirectoryImage))
		 {
		    /* Est-ce un nouveau directory qui contient des documents */
		    if (!TtaIsInDocumentPath (DirectoryImage))
		       if (TtaIsSuffixFileIn (DirectoryImage, FileExtension[IndexTypeImage]))
			 {
			    /* il faut ajouter le directory au path */
			    i = strlen (DocumentPath);
			    if (i + strlen (DirectoryImage) + 2 < MAX_PATH)
			      {
				 strcat (DocumentPath, PATH_STR);
				 strcat (DocumentPath, DirectoryImage);
				 InitPathImage ();
				 TtaListDirectory (DirectoryImage, BaseDlgImage + MY_NUM_IMAGE_FORM, NULL, -1,
						   FileExtension[IndexTypeImage], "Files", BaseDlgImage + MY_NUM_IMAGE_SEL);
			      }
			 }
		 }
	       break;
	    case MY_NUM_IMAGE_SEL:
	       if (DirectoryImage[0] == '\0')
		 {
		    /* compose le path complet du fichier pivot */
		    strncpy (DirectoryImage, DocumentPath, MAX_PATH);
		    /* recheche indirectement le directory */
		    MakeCompleteName (txt, "", DirectoryImage, completeName, &i);
		    /* separe directory et nom */
		    TtaExtractName (completeName, DirectoryImage, ImageName);
		 }
	       else
		 {
		    strcpy (completeName, DirectoryImage);
		    strcat (completeName, "/");
		    strcat (completeName, txt);
		    strcpy (ImageName, txt);
		 }
	       TtaSetTextForm (BaseDlgImage + MY_NUM_ZONE_IMAGE_FILE, completeName);
	       break;
	    case MY_NUM_MENU_IMAGE_TYPE:
	       if (val != IndexTypeImage)
		 {
		    IndexTypeImage = val;
		    if (TtaCheckDirectory (DirectoryImage))
		      {
			 /* Est-ce un nouveau directory qui contient des documents */
			 if (!TtaIsInDocumentPath (DirectoryImage))
			    if (TtaIsSuffixFileIn (DirectoryImage, FileExtension[IndexTypeImage]))
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
		    TtaListDirectory (DirectoryImage, BaseDlgImage + MY_NUM_IMAGE_FORM, NULL, -1,
				      FileExtension[IndexTypeImage], "Files", BaseDlgImage + MY_NUM_IMAGE_SEL);
		    CheckPresImage (val);
		 }
	       break;
	    case MY_NUM_MENU_IMAGE_FRAME:
	       if (val != IndexPresImage)
		 {
		    IndexPresImage = val;
		    /* Faut-il mettre a jour la liste des fichiers */
		    if (DirectoryImage[0] != '\0')
		       TtaListDirectory (DirectoryImage, BaseDlgImage + MY_NUM_IMAGE_FORM, NULL, -1,
					 FileExtension[IndexTypeImage], "Files", BaseDlgImage + MY_NUM_IMAGE_SEL);
		 }
	       break;
	    case MY_NUM_ZONE_DIR_IMAGE:
	       strcpy (DirectoryImage, txt);
	       TtaSetTextForm (BaseDlgImage + MY_NUM_ZONE_IMAGE_FILE, DirectoryImage);
	       TtaListDirectory (DirectoryImage, BaseDlgImage + MY_NUM_IMAGE_FORM, NULL, -1,
				 FileExtension[IndexTypeImage], "Files", BaseDlgImage + MY_NUM_IMAGE_SEL);
	       break;
	    case MY_NUM_IMAGE_FORM:
	       if (val == 1)
		  /* Edition realisee */
		  RedisplayPicture = TRUE;
	       TtaDestroyDialogue (BaseDlgImage + MY_NUM_IMAGE_FORM);
	       break;
	    default:
	       break;
	 }
}


/*----------------------------------------------------------------------
   Menu appele lors de la modification d'une image                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void BuildPictureMenu (char *name, boolean *result, int *typim, int *pres, PtrBox pBox)

#else  /* __STDC__ */
void BuildPictureMenu (name, result, typim, pres, pBox)
char               *name;
boolean            *result;
int                *typim;
int                *pres;
int                *pres;
PtrBox            pBox;

#endif /* __STDC__ */
{
   int                 i, indx;
   char                bufTypeImage[MAX_TXT_LEN];
   char               *source;
   int                 imageTypeCount, length;
   char                bufMenu[MAX_TXT_LEN];
   PictInfo    *image;


   IndexTypeImage = GetPictTypeIndex (*typim);
   IndexPresImage = GetPictPresIndex (*pres);
   strcpy (ImageName, name);
   DirectoryImage[0] = '\0';
   RedisplayPicture = FALSE;

   TtaNewForm (BaseDlgImage + MY_NUM_IMAGE_FORM, 0, 0, 0, "Picture", TRUE, 2, 'L', D_DONE);
   InitPathImage ();

   /* liste des fichiers existants */
   TtaListDirectory ("", BaseDlgImage + MY_NUM_IMAGE_FORM, NULL, -1, "", "Files", BaseDlgImage + MY_NUM_IMAGE_SEL);

   /* sous-menu des types d'image du formulaire Picture */
   indx = 0;
   GetPictHandlersList (&imageTypeCount, bufTypeImage);
   /* ajoute un 'B' au debut de chaque entree du menu construit */
   source = &bufTypeImage[0];
   for (i = 1; i <= imageTypeCount; i++)
     {
	bufMenu[indx] = 'B';
	indx++;
	length = strlen (source) + 1;
	if (indx + length < MAX_TXT_LEN)
	  {
	     strcpy ((bufMenu) + indx, source);
	     indx += length;
	  }
	source += length;
     }
   TtaNewSubmenu (BaseDlgImage + MY_NUM_MENU_IMAGE_TYPE, BaseDlgImage + MY_NUM_IMAGE_FORM, 0, "Type image",




		  imageTypeCount, bufMenu, NULL, TRUE);

   /* sous-menu cadrage du formulaire Picture */
   indx = 0;
   sprintf (&bufMenu[indx], "%s%s", "B", "Sans Modification");
   indx += strlen (&bufMenu[indx]) + 1;
   sprintf (&bufMenu[indx], "%s%s", "B", "ReScale");
   indx += strlen (&bufMenu[indx]) + 1;
   sprintf (&bufMenu[indx], "%s%s", "B", "Plein Cadre");
   indx += strlen (&bufMenu[indx]) + 1;

   TtaNewSubmenu (BaseDlgImage + MY_NUM_MENU_IMAGE_FRAME, BaseDlgImage + MY_NUM_IMAGE_FORM, 0,
		  "Affichage", 3, bufMenu, NULL, FALSE);

   /* zone de saisie du nom du fichier image */
   TtaNewTextForm (BaseDlgImage + MY_NUM_ZONE_IMAGE_FILE, BaseDlgImage + MY_NUM_IMAGE_FORM,
		   "Name du fichier", 50, 1, TRUE);

   TtaSetSelector (BaseDlgImage + MY_NUM_ZONE_DIR_IMAGE, -1, "");
   TtaSetTextForm (BaseDlgImage + MY_NUM_ZONE_IMAGE_FILE, name);
   TtaSetMenuForm (BaseDlgImage + MY_NUM_MENU_IMAGE_TYPE, IndexTypeImage);
   TtaSetMenuForm (BaseDlgImage + MY_NUM_MENU_IMAGE_FRAME, IndexPresImage);
   CheckPresImage (IndexTypeImage);
   /* active le formulaire */
   TtaShowDialogue (BaseDlgImage + MY_NUM_IMAGE_FORM, FALSE);
   /* attend le retour du formulaire */
   TtaWaitShowDialogue ();
   if (ImageName[0] == '\0')
      RedisplayPicture = FALSE;

   if (RedisplayPicture)
     {
	strcpy (name, ImageName);
	*typim = GetPictureType (IndexTypeImage);
	*pres = (PictureScaling) (IndexPresImage);
	image = (PictInfo *) pBox->BxPictInfo;
	strcpy (image->PicFileName, name);
	image->PicPresent = *pres;
	image->PicType = *typim;
     }
   *result = RedisplayPicture;
}
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                ImageMenuLoadResources ()
{
   BaseDlgImage = TtaSetCallback (CallbackPictureMenu, MAX_IMAGE_MENU);
   if (BaseDlgImage != 0)
      TteConnectAction (T_imagemenu, (Proc) BuildPictureMenu);
}
