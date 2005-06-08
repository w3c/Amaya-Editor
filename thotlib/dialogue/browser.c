/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
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
#include "dialog.h"
#include "message.h"
#include "constmedia.h"
#include "typeint.h"
#include "application.h"
#include "dialogapi_f.h"

#define NAME_LENGTH	200
#define MAX_NAME	 80
#define SELECTOR_NB_ITEMS 5

#ifdef _GTK
  static char         ls_unixDirs[MAX_NAME * NAME_LENGTH];
  static int          ls_dirNbr;
  static char         ls_unixFiles[MAX_NAME * NAME_LENGTH];
  static int          ls_fileNbr;
#endif /* _GTK */

#include "browser_f.h"
#include "fileaccess_f.h"


/*----------------------------------------------------------------------
   TtaExtractSuffix extract suffix from a file nane.                
  ----------------------------------------------------------------------*/
void TtaExtractSuffix (char *aName, char *aSuffix)
{
  int               lg, i;
  char             *ptr, *oldptr;

  if (!aSuffix || !aName)
    /* bad suffix */
    return;

  aSuffix[0] = EOS;
  lg = strlen (aName);
  if (lg)
    {
      /* the name is not empty */
      oldptr = ptr = aName;
      do
	{
	  ptr = strrchr (oldptr, '.');
	  if (ptr)
	    oldptr = &ptr[1];
	}
      while (ptr);

      i = (long int) (oldptr) - (long int) (aName);	/* name length */
      if (i > 1)
	{
	  aName[i - 1] = EOS;
	  if (i != lg)
	    strcpy (aSuffix, oldptr);
	}
    }
}

/*----------------------------------------------------------------------
  TtaStrAddSorted
  Adds new entry in table. Table is a linear array with strings
  separated by EOS. The sort is done with a strcmp. This means
  that digits are sorted scriptically, rather than numerically.
  Parameters:
  new_entry : string to be added
  table : string linear table
  ptr_last : ptr to the last used byte in the table
  nb_entries : number of entries in the table
  ----------------------------------------------------------------------*/
void TtaStrAddSorted (char *new_entry, char *table, char *ptr_last, int nb_entries)
{
  int i;
  int len_new_entry;
  int index;
  char *ptr;
  
  index = -1;
  len_new_entry = strlen (new_entry);
  for (i = 0; i < nb_entries; i++)
    {
      index++;
      ptr = &table[index];
      if (strcmp (new_entry, ptr) < 0)
	{
	  /* new entry comes before ptr  */
	  memmove (ptr + len_new_entry + 1, ptr, ptr_last - ptr + 1);
	  break;
	}
      while (table[index] != EOS)
	index++;
    }

  if (i == nb_entries)
    index++;
  /* insert new entry at the end */
  strcpy (&table[index], new_entry);
}

/*----------------------------------------------------------------------
   TtaListDirectory
   reads the content of a directory.
   dirname specifies the directory to read  (!=/afs). 
   dirTitle gives the title of the directory selector.
   formRef is the reference to the form we are using.
   dirRef is the reference to the directory selector.
   suffix gives the sort suffix for the files
   fileTitle gives the title of the files selector
   fileRef is the reference of the files selector
   If dirRef or fileRef have a negative values, the corresponding selector
   won't be created.
   If dirname doesn't exist, the selectors will be empty.
  ----------------------------------------------------------------------*/
void TtaListDirectory (char *dirname, int formRef, char *dirTitle,
		       int dirRef, char *suffix, char *fileTitle,
		       int fileRef)
{
#ifdef _GTK
  DIR *dp;
  struct stat         st;
#ifdef HAVE_DIRENT_H
  struct dirent      *d;
#else /* HAVE_DIRENT_H */
  struct direct      *d;
#endif /* HAVE_DIRENT_H */
  char               *ptr;
  char                filename[4 * NAME_LENGTH];
  char                s[NAME_LENGTH], ref[NAME_LENGTH];
  int                 ldir, lfile, length;
  int                 i, diff;
  ThotBool            stop, start, end, isSuffix;
#define SELECTOR_WIDTH 150

  if (dirTitle == NULL)
    dirTitle = "";
  if (fileTitle == NULL)
    fileTitle = "";
  isSuffix = FALSE;
  s[0] = EOS;
  /* S'il s'agit d'un directory accessible */
  if (TtaCheckDirectory (dirname)
      && strcmp (dirname, "/afs") != 0
      && strcmp (dirname, "/afs/") != 0)
    {
      /* CsList les directories du directory */
      if ((dp = opendir (dirname)) == NULL) 
	{
	  /* @@@ we couldn't open the directory ... we need some msg */
	  perror (dirname);
	  return;
	}
      /* handle the suffix */
      start = end = TRUE;
      if (suffix)
	{
	  i = 0;
	  while (suffix[i] == '*')
	    {
	      i++;
	      start = FALSE;
	    }
	  isSuffix = (suffix[i] == '.' && !start);
	  if (isSuffix)
	    {
	      /* check only the suffix */
	      i++;
	      start = TRUE;
	      while (suffix[i] == '*')
		{
		  i++;
		  start = FALSE;
		}
	    }
	  strncpy (ref, &suffix[i], NAME_LENGTH);
	  i = strlen (ref) - 1;
	  while (ref[i] == '*')
	    {
	      ref[i--] = EOS;
	      end = FALSE;
	    }
	  /* check if The current ref includes a '*' */
	  ptr = strstr (s, "*");
	  if (ptr)
	    /* if yes, display all entries */
	    ref[0] = EOS;
	}
      else
	ref[0] = EOS;
      ls_unixFiles[0] = EOS;
      ls_fileNbr = 0;
      lfile = 0;
      ls_unixDirs[0] = EOS;
      ls_dirNbr = 0;
      ldir = 0;
      stop = FALSE;
      while ((d = readdir (dp)) != NULL)
	{
	  /* skip the UNIX . and .. links */
	  if (!strcmp (d->d_name, "..") || !strcmp (d->d_name, "."))
	    continue;
	  sprintf (filename, "%s%c%s", dirname, DIR_SEP, d->d_name);
	  length = strlen (d->d_name) + 1;
	  if  (lstat (filename, &st) < 0 ) 
	    {
	      /* @@2 need some error message */
	      perror (filename);
	      continue;
	    }
	  if ((st.st_mode & S_IFMT) == S_IFLNK && TtaCheckDirectory (filename))
	    /* handle the entry as a directory */
	    st.st_mode = S_IFDIR;
	  switch (st.st_mode & S_IFMT)
	    {
	    case S_IFDIR:
	      /* it's a directory */
	      if (dirRef >= 0)
		{
		  stop = (ldir + length >= MAX_NAME * NAME_LENGTH);
		  if (!stop)
		    {
		      /* add this file in the list */
		      TtaStrAddSorted (d->d_name, ls_unixDirs, &ls_unixDirs[ldir], ls_dirNbr);
		      ldir += length;
		      ls_dirNbr++;
		    }
		}
	      break;
	    default:
	      /* it's a filename */
	      if (fileRef >= 0)
		{
		  stop = (lfile + length >= MAX_NAME * NAME_LENGTH);
		  if (!stop)
		    {
		      /* by default it's not okay */
		      diff = 1;
		      /* check if the suffix is valid */
		      if (ref[0] != EOS)
			{
			  strncpy (s, d->d_name, NAME_LENGTH);
			  if (isSuffix)
			    {
			      ptr = strstr (s, ".");
			      if (ptr)
				strcpy (s, &ptr[1]);
			    }
			  /* compare the name or the suffix with the requested string */
			  ptr = strstr (s, ref);
			  if (ptr)
			    {
			      /* the substring is found */
			      if ((start && ptr == s) || !start)
				{
				  diff = strcmp (ref, ptr);
				  if (!end && diff < 0)
				    diff = 0;
				}
			    }
			}
		      else
			diff = 0;
		      if (diff == 0)
			{
			  /* add this file in the list */
			  TtaStrAddSorted (d->d_name, ls_unixFiles, &ls_unixFiles[lfile], ls_fileNbr);
			  lfile += length;
			  ls_fileNbr++;
			}
		    }
		}
	      break;
	    }
	}
      closedir (dp);
      if (dirRef >= 0)
	{
	  TtaNewSizedSelector (dirRef, formRef, dirTitle,
			       ls_dirNbr, ls_unixDirs, SELECTOR_WIDTH,
			       SELECTOR_NB_ITEMS, "..", FALSE, TRUE);
	  TtaSetSelector (dirRef, -1, "");
	}
      if (fileRef >= 0)
	{
	  TtaNewSizedSelector (fileRef, formRef, fileTitle,
			       ls_fileNbr, ls_unixFiles, SELECTOR_WIDTH,
			       SELECTOR_NB_ITEMS + 1, NULL, FALSE, TRUE);
	  TtaSetSelector (fileRef, -1, "");
	}
    }
  else
    {
      /* Ce n'est pas un directory -> annule les selecteurs */
      if (dirRef >= 0)
	{
	  TtaNewSizedSelector (dirRef, formRef, dirTitle, 0, NULL,
			       SELECTOR_WIDTH, SELECTOR_NB_ITEMS,
			       "", FALSE, TRUE);
	  TtaSetSelector (dirRef, -1, "");
	}
      if (fileRef >= 0)
	{
	  TtaNewSizedSelector (fileRef, formRef, fileTitle, 0, NULL,
			       SELECTOR_WIDTH, SELECTOR_NB_ITEMS + 1,
			       NULL, FALSE, TRUE);
	  TtaSetSelector (fileRef, -1, "");
	}
    }
#endif /* _GTK */
}

