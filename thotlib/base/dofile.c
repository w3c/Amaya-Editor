
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/* 
   dofile.c : traitement des noms de fichiers. 
   Recherche un fichier avec un path de repertoires.
   Avril 1989   
   Major changes 
   PMA : 4/4/91       remise en forme
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "registry.h"

#define EXPORT extern
#include "environ.var"

#include "dofile.f"
#include "filesystem.f"

/* ---------------------------------------------------------------------- */
/* |    IsExtended compare la fin de fname avec fext. Si la fin est     | */
/* |            identique, retourne Vrai.                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static boolean      IsExtended (Name fname, char *fext)

#else  /* __STDC__ */
static boolean      IsExtended (fname, fext)
Name                 fname;
char               *fext;

#endif /* __STDC__ */

{
   int                 i, j, namelong, extlong;
   boolean             res;

   namelong = 0;
   extlong = 0;

   /* on mesure fext */
   extlong = strlen (fext);
   /* on mesure fname */
   namelong = strlen (fname);
   if (namelong >= MAX_CHAR)
      res = False;
   else if (namelong > 0 && extlong > 0)
     {
	res = True;
	j = namelong - 1;
	for (i = extlong - 1; i >= 0; i--)
	  {
	     res = fext[i] == fname[j] && res;
	     j--;
	  }
	res = res && fname[j] == '.';
     }
   else
      res = False;
   return res;
}

/* ---------------------------------------------------------------------- */
/* |    BuildFileName compose un nom de fichier absolu en concatenant   | */
/* |            un nom de directory, le nom de fichier (fname) et       | */
/* |            l'extension (fext).                                     | */
/* |            Retourne le nom compose' dans nomabs.                   | */
/* |            Si fname se termine deja par fext, alors copie          | */
/* |            simplement fname dans nomabs.                           | */
/* |            La chaine directory_list peut contenir un path          | */
/* |            hierarchique ou` les noms de repertoires sont classe's  | */
/* |            par ordre d'importance, et separes par PATH_SEP         | */
/* |            Si le fichier existe, on retourne dans directory_list   | */
/* |            le nom du directory qui le contient, sinon              | */
/* |            directory_list n'est pas change'.                       | */
/* |            Si le fichier n'existe pas, on retourne nomabs vide et  | */
/* |            dans directory_list le 1er nom du path fourni a` l'appel| */
/* |            (BuildFileName est utilise pour la lecture)             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                BuildFileName (Name fname, char *fext, PathBuffer directory_list, PathBuffer nomabs, int *lg)

#else  /* __STDC__ */
void                BuildFileName (fname, fext, directory_list, nomabs, lg)
Name                 fname;
char               *fext;
PathBuffer          directory_list;
PathBuffer          nomabs;
int                *lg;

#endif /* __STDC__ */

{
   int                 i, j;
   PathBuffer          single_directory;
   PathBuffer          first_directory;
   boolean             trouve;

   trouve = False;
   i = 1;
   first_directory[0] = '\0';
   while (directory_list[i - 1] != '\0' && (!trouve))
     {
	j = 1;
	while (directory_list[i - 1] != PATH_SEP
	       && directory_list[i - 1] != '\0'
	       && j < MAX_PATH
	       && i < MAX_PATH)
	  {
	     /* on decoupe la liste en directory individuels */
	     single_directory[j - 1] = directory_list[i - 1];
	     i++;
	     j++;
	  }
	/* on ajoute une fin de chaine */
	single_directory[j - 1] = '\0';
	/* on sauve ce nom de directory si c'est le 1er */
	if (first_directory[0] == '\0')
	   strncpy (first_directory, single_directory, MAX_PATH);
	/* on construit le nom */
	DoFileName (fname, fext, single_directory, nomabs, lg);
	if (FileExist (nomabs))
	  {
	     trouve = True;
	     strncpy (directory_list, single_directory, MAX_PATH);
	  }
	else
	   /* on essaie avec un autre directory en sautant le PATH_SEP */
	if (directory_list[i - 1] == PATH_SEP)
	   i++;
     }
   if (!trouve)
     {
	nomabs[0] = '\0';
	if (first_directory[0] != '\0')
	   strncpy (directory_list, first_directory, MAX_PATH);
     }
}

/* ---------------------------------------------------------------------- */
/* |    GetImageFileName construit dans fn le nom absolu d'un fichier   | */
/* |            image a` partir du nom du fichier contenu dans name     | */
/* |            partir du nom du fichier contenu dans name et des'      | */
/* |            repertoires de documents ou de sche'mas.                | */
/* |            Si le fichier n'existe pas on retourne name.            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetImageFileName (char *name, char *fn)

#else  /* __STDC__ */
void                GetImageFileName (name, fn)
char               *name;
char               *fn;

#endif /* __STDC__ */

{
   int                 lg;
   PathBuffer          Directory;

   /* Recherche le fichier dans les repertoires de documents */
   if (name[0] == DIR_SEP)
      strcpy (fn, name);
   else
     {
	strcpy (Directory, DirectoryDoc);
	BuildFileName (name, "", Directory, fn, &lg);
	if (!FileExist (fn))
	  {
	     /* Recherche le fichier dans les repertoires de schemas */
	     strcpy (Directory, DirectorySchemas);
	     BuildFileName (name, "", Directory, fn, &lg);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    DoFileName compose un nom de fichier absolu en concatenant le   | */
/* |            nom de directory, le nom de fichier (fname) et          | */
/* |            l'extension (fext).                                     | */
/* |            Retourne le nom compose' dans nomabs et la longueur     | */
/* |            de ce nom dans lg.                                      | */
/* |            Si fname se termine deja par fext, alors copie          | */
/* |            simplement fname dans nomabs.                           | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                DoFileName (Name fname, char *fext, PathBuffer directory, PathBuffer nomabs, int *lg)

#else  /* __STDC__ */
void                DoFileName (fname, fext, directory, nomabs, lg)
Name                 fname;
char               *fext;
PathBuffer          directory;
PathBuffer          nomabs;
int                *lg;

#endif /* __STDC__ */

{
   int                 i, j, k, h = 0;
   char               *home_dir = NULL;

   /* on recopie le repertoire */
   i = strlen (directory);
   j = strlen (fname);

   /* check for tilde indicating the HOME directory */
   if (directory[0] == '~')
     {
	home_dir = TtaGetEnvString ("HOME");
     }
   if (home_dir != NULL)
     {
	i--;			/* tilde will not be copied */
	h = strlen (home_dir);
     }
   if (i > 1)
      i++;			/* for the added DIR_STR */

   /* si on cherche a ouvrir un fichier pivot et que le nom de fichier se
      termine par ".piv", on remplace ce suffixe par ".PIV" */
   if (strcmp (fext, "PIV") == 0)
     {
	if (j > 4)
	   if (fname[j - 4] == '.')
	      if (fname[j - 3] == 'p')
		 if (fname[j - 2] == 'i')
		    if (fname[j - 1] == 'v')
		      {
			 fname[j - 3] = 'P';
			 fname[j - 2] = 'I';
			 fname[j - 1] = 'V';
		      }
     }
   if (!IsExtended (fname, fext) && fext[0] != '\0')
      k = strlen (fext) + 1;	/* dont forget the '.' */
   else
      k = 0;
   if (i + j + k + h >= MAX_PATH)
      return;

   nomabs[0] = '\0';
   if (home_dir)
     {
	strcat (nomabs, home_dir);
	strcat (nomabs, &directory[1]);
     }
   else
     {
	strcat (nomabs, directory);
     }

   /* on ajoute un DIR_STR */
   if (i >= 1)
     {
	strcat (nomabs, DIR_STR);
     }

   /* on recopie le nom */
   strcat (nomabs, fname);
   if (k != 0)
     {
	/* on ajoute l'extension */
	strcat (nomabs, ".");
	strcat (nomabs, fext);
     }
   /* on termine la chaine */
   *lg = i + j + k + h;
}
