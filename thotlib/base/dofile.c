/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/* 
   Construction des noms de fichiers. 
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "registry.h"
#include "thotfile.h"
#include "thotdir.h"

#define EXPORT extern
#include "platform_tv.h"

#include "dofile_f.h"
#include "platform_f.h"

/*----------------------------------------------------------------------
   MakeCompleteName compose un nom de fichier absolu en concatenant 
   un nom de directory, le nom de fichier (fname) et       
   l'extension (fext).                                     
   Retourne le nom compose' dans nomabs.                   
   Si fname se termine deja par fext, alors copie          
   simplement fname dans nomabs.                           
   La chaine directory_list peut contenir un path          
   hierarchique ou` les noms de repertoires sont classe's  
   par ordre d'importance, et separes par PATH_SEP         
   Si le fichier existe, on retourne dans directory_list   
   le nom du directory qui le contient, sinon              
   directory_list n'est pas change'.                       
   Si le fichier n'existe pas, on retourne nomabs vide et  
   dans directory_list le 1er nom du path fourni a` l'appel
   (MakeCompleteName est utilise pour la lecture)          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MakeCompleteName (Name fname, char *fext, PathBuffer directory_list, PathBuffer completeName, int *length)
#else  /* __STDC__ */
void                MakeCompleteName (fname, fext, directory_list, completeName, length)
Name                 fname;
char               *fext;
PathBuffer          directory_list;
PathBuffer          completeName;
int                *length;
#endif /* __STDC__ */
{
   int                 i, j;
   PathBuffer          single_directory;
   PathBuffer          first_directory;
   boolean             found;

   found = FALSE;
   i = 1;
   first_directory[0] = '\0';
   while (directory_list[i - 1] != '\0' && (!found))
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
	FindCompleteName (fname, fext, single_directory, completeName, length);
	if (ThotFile_exist (completeName))
	  {
	     found = TRUE;
	     strncpy (directory_list, single_directory, MAX_PATH);
	  }
	else
	   /* on essaie avec un autre directory en sautant le PATH_SEP */
	if (directory_list[i - 1] == PATH_SEP)
	   i++;
     }
   if (!found)
     {
	completeName[0] = '\0';
	if (first_directory[0] != '\0')
	   strncpy (directory_list, first_directory, MAX_PATH);
     }
}

/*----------------------------------------------------------------------
   GetPictureFileName construit dans fileName le nom absolu d'un   
   fichier image a` partir du nom contenu dans name et     
   des repertoires de documents ou de sche'mas.            
   Si le fichier n'existe pas retourne name.               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                GetPictureFileName (char *name, char *fileName)
#else  /* __STDC__ */
void                GetPictureFileName (name, fileName)
char               *name;
char               *fileName;
#endif /* __STDC__ */
{
   int                 length;
   PathBuffer          directory;

   /* Recherche le fichier dans les repertoires de documents */
   if (name[0] == DIR_SEP)
      strcpy (fileName, name);
   else
     {
	strcpy (directory, DocumentPath);
	MakeCompleteName (name, "", directory, fileName, &length);
	if (!ThotFile_exist (fileName))
	  {
	     /* Recherche le fichier dans les repertoires de schemas */
	     strcpy (directory, SchemaPath);
	     MakeCompleteName (name, "", directory, fileName, &length);
	  }
     }
}

/*----------------------------------------------------------------------
   IsExtended compare la fin de fileName avec extension. Si la fin 
   est identique, retourne Vrai.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      IsExtended (Name fileName, char *extension)
#else  /* __STDC__ */
static boolean      IsExtended (fileName, extension)
Name                fileName;
char               *extension;
#endif /* __STDC__ */
{
   int                 i, j;
   int                 nameLength, extLength;
   boolean             ok;

   nameLength = 0;
   extLength = 0;

   /* on mesure extension */
   extLength = strlen (extension);
   /* on mesure fileName */
   nameLength = strlen (fileName);
   if (nameLength >= MAX_CHAR)
      ok = FALSE;
   else if (nameLength > 0 && extLength > 0)
     {
	ok = TRUE;
	j = nameLength - 1;
	for (i = extLength - 1; i >= 0; i--)
	  {
	     ok = (extension[i] == fileName[j]) && ok;
	     j--;
	  }
	ok = ok && (fileName[j] == '.');
     }
   else
      ok = FALSE;
   return ok;
}


/*----------------------------------------------------------------------
   FindCompleteName compose un nom de fichier absolu en concatenant 
   le nom de directory, le nom de fichier (fileName) et    
   l'extension (extension).                                
   Retourne le nom compose dans completeName et la         
   longueur de ce nom dans length.                         
   Si fileName se termine deja par extension, alors copie  
   simplement fileName dans completeName.                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FindCompleteName (Name fileName, char *extension, PathBuffer directory, PathBuffer completeName, int *length)
#else  /* __STDC__ */
void                FindCompleteName (fileName, extension, directory, completeName, length)
Name                 fileName;
char               *extension;
PathBuffer          directory;
PathBuffer          completeName;
int                *length;
#endif /* __STDC__ */
{
   int                 i, j, k, h = 0;
   char               *home_dir = NULL;

   /* on recopie le repertoire */
   i = strlen (directory);
   j = strlen (fileName);

   /* check for tilde indicating the HOME directory */
   if (directory[0] == '~')
     {
	home_dir = TtaGetEnvString ("HOME");
     }
   if (home_dir != NULL)
     {
       /* tilde will not be copied */
	i--;
	h = strlen (home_dir);
     }
   if (i > 1)
     /* for the added DIR_STR */
      i++;

   /* si on cherche a ouvrir un fichier pivot et que le nom de fichier se
      termine par ".piv", on remplace ce suffixe par ".PIV" */
   if (strcmp (extension, "PIV") == 0)
     {
	if (j > 4)
	   if (fileName[j - 4] == '.')
	      if (fileName[j - 3] == 'p')
		 if (fileName[j - 2] == 'i')
		    if (fileName[j - 1] == 'v')
		      {
			 fileName[j - 3] = 'P';
			 fileName[j - 2] = 'I';
			 fileName[j - 1] = 'V';
		      }
     }
   if (!IsExtended (fileName, extension) && extension[0] != '\0')
      k = strlen (extension) + 1;	/* dont forget the '.' */
   else
      k = 0;
   if (i + j + k + h >= MAX_PATH)
      return;

   completeName[0] = '\0';
   if (home_dir)
     {
	strcat (completeName, home_dir);
	strcat (completeName, &directory[1]);
     }
   else
     {
	strcat (completeName, directory);
     }

   /* on ajoute un DIR_STR */
   if (i >= 1)
     {
	strcat (completeName, DIR_STR);
     }

   /* on recopie le nom */
   strcat (completeName, fileName);
   if (k != 0)
     {
	/* on ajoute l'extension */
	strcat (completeName, ".");
	strcat (completeName, extension);
     }
   /* on termine la chaine */
   *length = i + j + k + h;
}
