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
 * browsing routines
 *
 * Author: I. Vatton (INRIA)
 *
 */
 
#include "thot_gui.h"
#include "thot_sys.h"
#include "thotdir.h"
#include "dialog.h"
#include "message.h"
#include "constint.h"
#include "application.h"

#define NAME_LENGTH	100
#define MAX_NAME	 80
#define SELECTOR_NB_ITEMS 5
static FILE        *ls_stream;
static int          ls_car;
static char         ls_unixFiles[MAX_NAME * NAME_LENGTH];
static int          ls_fileNbr;
static char         EmptyMsg[] = "";

/*----------------------------------------------------------------------
   ExtractFileName
   extracts a filename from the ls_stream.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ExtractFileName (char *word)
#else  /* __STDC__ */
void                ExtractFileName (word)
char               *word;

#endif /* __STDC__ */
{
   int                 i;
   boolean             notEof;

   i = 0;
   while (((char) ls_car == ' ') || ((char) ls_car == '\t') || ((char) ls_car == '\n'))
      ls_car = fgetc (ls_stream);

   notEof = TRUE;
   while (((char) ls_car != ' ') && ((char) ls_car != '\t') && ((char) ls_car != '\n') && (notEof))
     {
	if (ls_car == EOF)
	   notEof = FALSE;
	else
	  {
	     word[i] = (char) ls_car;
	     i++;
	     ls_car = fgetc (ls_stream);
	  }
     }

   word[i] = EOS;
   i++;
}

/*----------------------------------------------------------------------
   TtaIsSuffixFileIn
   returns TRUE if the directory contains any file with the requested
   suffix.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             TtaIsSuffixFileIn (char *aDirectory, char *suffix)
#else  /* __STDC__ */
boolean             TtaIsSuffixFileIn (aDirectory, suffix)
char               *aDirectory;
char               *suffix;

#endif /* __STDC__ */
{
   boolean             ret;
   char                command[200];
   ThotDirBrowse       thotDir;

   ret = FALSE;
   /* S'il s'agit d'un directory accessible */
   if (TtaCheckDirectory (aDirectory))
     {
	thotDir.buf = command;
	thotDir.bufLen = sizeof (command);
	thotDir.PicMask = (ThotDirBrowse_mask)
                          (ThotDirBrowse_FILES | ThotDirBrowse_DIRECTORIES);
	ret = ThotDirBrowse_first (&thotDir, aDirectory, "*", suffix);
	ThotDirBrowse_close (&thotDir);
     }
   return (ret);
#if 0
#ifdef WWW_WINDOWS
   WIN32_FIND_DATA     findData;
   HANDLE              findNextHandle;

   sprintf (command, "%s\\*.%s", aDirectory, suffix);	/* use the same space */
   if ((findNextHandle = FindFirstFile (command, &findData)) !=
       INVALID_HANDLE_VALUE)
     {
	ret = TRUE;
	FindClose (findNextHandle);
     }
#else  /* WWW_WINDOWS */
   /* Commande ls sur le directory */
   sprintf (command, "/bin/ls %s/*%s 2>/dev/null", aDirectory, suffix);
   ls_stream = popen (command, "r");
   if (ls_stream != NULL)
     {
	ls_car = fgetc (ls_stream);
	if (ls_car != EOF)
	   ret = TRUE;
	pclose (ls_stream);
     }
#endif /* !WWW_WINDOWS */
#endif
}


/*----------------------------------------------------------------------
   TtaListDirectory
   reads the content of a directory.
   aDirectory specifies the directory to read  (!=/afs). 
   dirTitle gives the title of the directory selector.
   formRef is the reference to the form we are using.
   dirRef is the reference to the directory selector.
   suffix gives the sort suffix for the files
   fileTitle gives the title of the files selector
   fileRef is the reference of the files selector
   If dirRef or fileRef have a negative values, the corresponding selector
   won't be created.
   If aDirectory doesn't exist, the selectors will be empty.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaListDirectory (char *aDirectory, int formRef, char *dirTitle, int dirRef, char *suffix, char *fileTitle, int fileRef)
#else  /* __STDC__ */
void                TtaListDirectory (aDirectory, formRef, dirTitle, dirRef, suffix, fileTitle, fileRef)
char               *aDirectory;
int                 formRef;
char               *dirTitle;
int                 dirRef;
char               *suffix;
char               *fileTitle;
int                 fileRef;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   struct stat;
   char               *addr1;
   char                word[4 * NAME_LENGTH];
   int                 ls_currentfile;
   int                 length;
   boolean             stop;

   if (dirTitle == NULL)
      dirTitle = EmptyMsg;
   if (fileTitle == NULL)
      fileTitle = EmptyMsg;

   /* S'il s'agit d'un directory accessible */
   if (TtaCheckDirectory (aDirectory)
       && strcmp (aDirectory, "/afs") != 0
       && strcmp (aDirectory, "/afs/") != 0)
     {
	/* CsList les directories du directory */
	if (dirRef >= 0)
	  {
	     ThotDirBrowse       thotDir;

	     ls_unixFiles[0] = EOS;
	     ls_fileNbr = 0;
	     ls_currentfile = 0;
	     stop = FALSE;
	     thotDir.buf = word;
	     thotDir.bufLen = sizeof (word);
	     thotDir.PicMask = ThotDirBrowse_DIRECTORIES;
	     if (ThotDirBrowse_first (&thotDir, aDirectory, "*", "") == 1)
	       {
		  do
		    {
		       /* On ne garde que le nom du fichier en eliminant le path */
		       addr1 = strrchr (word, DIR_SEP);
		       stop = (addr1 == NULL);
		       if (!stop)
			 {
			    addr1++;
			    length = strlen (addr1) + 1;
			    stop = (ls_currentfile + length >= MAX_NAME * NAME_LENGTH);
			    if (!stop)
			      {
				 strcpy (&ls_unixFiles[ls_currentfile], addr1);
				 ls_currentfile += strlen (addr1) + 1;
				 ls_fileNbr++;
			      }
			 }
		    }
		  while (ThotDirBrowse_next (&thotDir) == 1);
		  ThotDirBrowse_close (&thotDir);
	       }
	     if (strlen (aDirectory) == 0)
		TtaNewSelector (dirRef, formRef,
				dirTitle, ls_fileNbr, ls_unixFiles, SELECTOR_NB_ITEMS, "", FALSE, TRUE);
	     else
		TtaNewSelector (dirRef, formRef,
				dirTitle, ls_fileNbr, ls_unixFiles, SELECTOR_NB_ITEMS, "..", FALSE, TRUE);
	     TtaSetSelector (dirRef, -1, "");
	  }
	/* CsList les fichiers du directory */
	if (fileRef >= 0)
	  {
	     ThotDirBrowse       thotDir;

	     thotDir.buf = word;
	     thotDir.bufLen = sizeof (word);
	     thotDir.PicMask = ThotDirBrowse_FILES;

	     ls_unixFiles[0] = EOS;
	     ls_fileNbr = 0;
	     ls_currentfile = 0;
	     stop = FALSE;
	     /* Commande ls sur le directory */
	     if (ThotDirBrowse_first (&thotDir, aDirectory, "*", suffix) == 1)
		do
		  {
		     /* c'est un fichier regulier -> compare le suffixe */
		     /* On ne garde que le nom du fichier en eliminant le path */
		     addr1 = strrchr (word, DIR_SEP);
		     stop = (addr1 == NULL);
		     if (!stop)
		       {
			  addr1++;
			  length = strlen (addr1) + 1;
			  stop = (ls_currentfile + length >= MAX_NAME * NAME_LENGTH);
			  if (!stop)
			    {
			       strcpy (&ls_unixFiles[ls_currentfile], addr1);
			       ls_currentfile += strlen (addr1) + 1;
			       ls_fileNbr++;
			    }
		       }
		  }
		while (ThotDirBrowse_next (&thotDir) == 1);
	     ThotDirBrowse_close (&thotDir);
	     /* initialisation des menus */
	     TtaNewSelector (fileRef, formRef,
			     fileTitle, ls_fileNbr, ls_unixFiles, SELECTOR_NB_ITEMS + 1, NULL, FALSE, TRUE);
	     TtaSetSelector (fileRef, -1, "");
	  }
     }
   else
     {
	/* Ce n'est pas un directory -> annule les selecteurs */
	if (dirRef >= 0)
	  {
	     TtaNewSelector (dirRef, formRef,
		     dirTitle, 0, NULL, SELECTOR_NB_ITEMS, "", FALSE, TRUE);
	     TtaSetSelector (dirRef, -1, "");
	  }
	if (fileRef >= 0)
	  {
	     TtaNewSelector (fileRef, formRef,
	      fileTitle, 0, NULL, SELECTOR_NB_ITEMS + 1, NULL, FALSE, TRUE);
	     TtaSetSelector (fileRef, -1, "");
	  }
     }
#  endif /* _WINDOWS */
}

