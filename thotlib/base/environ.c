
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   environ.c : traitement de l'environnement de l'editeur.
   Traite le formulaire environnement et les paths de schemas.
   Traite les var<iables de configuration.

   V. Quint     Mai 1988
 */

#include "thot_sys.h"
#include "functions.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"

#define EXPORT extern
#include "environ.var"

#include "filesystem.f"
#include "environ.f"

#ifdef __STDC__
extern char *ThotPath ( char *domain );
#else /* __STDC__ */
extern char *ThotPath (/* char *domain */);
#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    TtaCheckDirectory verifie que c'est un directory accessible.    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             TtaCheckDirectory (char *aDirectory)

#else  /* __STDC__ */
boolean             TtaCheckDirectory (aDirectory)
char               *aDirectory;

#endif /* __STDC__ */

{
#ifdef WWW_MSWINDOWS
   DWORD               attribs;

   /* NEW_WINDOWS - mark for furthur security stuff - EGP  
      SECURITY_INFORMATION secInfo;
      SECURITY_DESCRIPTOR secDesc; */
   attribs = GetFileAttributes (aDirectory);
   if (!(attribs & FILE_ATTRIBUTE_DIRECTORY))
      return False;
   return True;
#else  /* WWW_MSWINDOWS */
   struct stat         fileStat;

   /* Teste si le directory existe */
   if (strlen (aDirectory) < 1)
      return (False);		/* le directory n'est pas accessible */
   else if (access (aDirectory, 0) != 0)
      return (False);		/* le directory n'est pas accessible */
   else if (stat (aDirectory, &fileStat) != 0)
      return (False);		/* le directory n'est pas accessible */
   else if (S_ISDIR (fileStat.st_mode))
      return (True);
   else
      return (False);		/* ce n'est pas un directory */
#endif /* !WWW_MSWINDOWS */
}

/* ---------------------------------------------------------------------- */
/* |    TtaCheckPath verifie l'accessibilite de chacun des repertoires  | */
/* |            du path passe en parametre.                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             TtaCheckPath (PathBuffer path)

#else  /* __STDC__ */
boolean             TtaCheckPath (path)
PathBuffer          path;

#endif /* __STDC__ */

{
   int                 i, j;
   PathBuffer          single_directory;
   boolean             OK;

   i = 0;
   OK = True;
   while (OK && path[i] != '\0')
     {
	j = 0;
	while (path[i] != PATH_SEP && path[i] != '\0' && i <= MAX_PATH)
	  {
	     /* on decoupe la liste en directory individuels */
	     single_directory[j] = path[i];
	     i++;
	     j++;
	  }
	/* on ajoute une fin de chaine */
	single_directory[j] = '\0';

	OK = TtaCheckDirectory (single_directory);
	if (!OK)
	   /* affiche un message d'erreur avec le nom du directory */
	   TtaDisplayMessage (INFO, TtaGetMessage(LIB, LIB_DIRECTORY_NOT_FOUND), single_directory);
	/* on essaie avec un autre directory en sautant le PATH_SEP */
	if (path[i] == PATH_SEP)
	   i++;
     }
   return (OK);
}

/* ---------------------------------------------------------------------- */
/* |    SearchFile recherche un fichier en suivant les indications      | */
/* |            donnees par recherche.                                  | */
/* |            Retourne 1 avec le nom absolu dans nomcomplet si on     | */
/* |            le trouve et 0 sinon.                                   | */
/* |            RlNext la valeur de recherche, on cherche dans:         | */
/* |            - 0 : /                                                 | */
/* |            - 1 : ThotDir                                           | */
/* |            - 2 : ThotDir/bin                                       | */
/* |            - 3 : ThotDir/compil                                    | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 SearchFile (char *nomfichier, int recherche, char *nomcomplet)

#else  /* __STDC__ */
int                 SearchFile (nomfichier, recherche, nomcomplet)
char               *nomfichier;
int                 recherche;
char               *nomcomplet;

#endif /* __STDC__ */

{
   char                tmpbuf[200];
   char               *imagepath;
   int                 i, j;
   int                 retour;


   if (ThotDir () != NULL)
      strcpy (nomcomplet, ThotDir ());
   else
      *nomcomplet = '\0';
   switch (recherche)
	 {
	    case 1:		/* ================> Recherche dans les schemas et les documents */
	       strcat (nomcomplet, nomfichier);
	       retour = FileExist (nomcomplet);
	       /* Recherche le fichier dans les directories de schemas */
	       i = 0;
	       j = 0;
	       imagepath = DirectorySchemas;
	       while (retour == 0 && imagepath[i] != '\0')
		 {
		    while (imagepath[i] != '\0' && imagepath[i] != PATH_SEP && i < 200)
		       tmpbuf[j++] = imagepath[i++];

		    tmpbuf[j] = '\0';
		    i++;
		    j = 0;
		    sprintf (nomcomplet, "%s%s%s", tmpbuf, DIR_STR, nomfichier);
		    retour = FileExist (nomcomplet);
		 }		/*while */

	       /* continue la recheche dans les repertoires de documents */
	       i = 0;
	       j = 0;
	       imagepath = DirectorySchemas;
	       while (retour == 0 && imagepath[i] != '\0')
		 {
		    while (imagepath[i] != '\0' && imagepath[i] != PATH_SEP && i < 200)
		       tmpbuf[j++] = imagepath[i++];

		    tmpbuf[j] = '\0';
		    i++;
		    j = 0;
		    sprintf (nomcomplet, "%s%s%s", tmpbuf, DIR_STR, nomfichier);
		    retour = FileExist (nomcomplet);
		 }		/*while */
	       break;

	    case 2:		/* =========================================> Recherche dans config */
	       strcat (nomcomplet, DIR_STR);
	       strcat (nomcomplet, "config");
	       strcat (nomcomplet, DIR_STR);
	       strcat (nomcomplet, nomfichier);
	       break;

	    case 3:		/* ======================================> Recherche dans batch */
	       strcat (nomcomplet, DIR_STR);
	       strcat (nomcomplet, "batch");
	       strcat (nomcomplet, DIR_STR);
	       strcat (nomcomplet, nomfichier);
	       break;

	    default:
	       strcat (nomcomplet, DIR_STR);
	       strcat (nomcomplet, nomfichier);
	 }

   /* on cherche le fichier */
   retour = FileExist (nomcomplet);
   if (retour == 0)
     {
	strcpy (nomcomplet, nomfichier);
	retour = FileExist (nomcomplet);
     }
   return retour;
}
/* End Of Module environ */
