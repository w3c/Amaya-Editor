/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * platform.c : basic system functions
 *
 * Authors: I. Vatton, D. Veillard (INRIA)
 *          R. Guetari (W3C/INRIA) Windows version
 *
 */
#ifdef _WX
  #include "wx/utils.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"

#include "fileaccess_f.h"
#include "memory_f.h"


/*----------------------------------------------------------------------
   TtaDirExists returns TRUE if the dirpath points to a directory.
   FALSE if the path points to a filename or doesn't
   point to anything.
  ----------------------------------------------------------------------*/
int TtaDirExists (CONST char *dirpath)
{
  int         status = 0;
  char       *name;
#ifdef _WINGUI
  DWORD       attribs;
#endif /* _WINGUI */

  name = GetRealFileName (dirpath);
#ifdef _WINGUI
  attribs = GetFileAttributes ((LPCTSTR)name);
  if (attribs == 0xFFFFFFFF)
    status = 0;
  else if (attribs & FILE_ATTRIBUTE_DIRECTORY)
    status = 1;
  else
    status = 0;
#else /* _WINGUI */
#ifdef _WX
  if (wxDirExists(wxString(name, *wxConvCurrent)))
    status = 1;
#else /* #ifdef _WX */
  struct stat buf;
  status = stat (name, &buf) == 0 && S_ISDIR (buf.st_mode);
#endif /* #ifdef _WX */
#endif /* _WINGUI */

  TtaFreeMemory (name);
  return status;
}

/*----------------------------------------------------------------------
   TtaFileExist teste l'existence d'un fichier.                       
   Rend 1 si le fichier a e't'e trouve' et 0 sinon.        
   Si filename est un repertoire, on retourne 0.           
  ----------------------------------------------------------------------*/
int TtaFileExist (CONST char *filename)
{
  int         status = 0;
  char       *name;
#ifdef _WINGUI
  DWORD       attribs;
#else /* _WINGUI */
  int         filedes;
  struct stat statinfo;
#endif /* _WINGUI */

  name = GetRealFileName (filename);
#ifdef _WINGUI
  attribs = GetFileAttributes ((LPCTSTR)name);
  if (attribs == 0xFFFFFFFF)
    status = 0;
  else if (attribs & FILE_ATTRIBUTE_DIRECTORY)
    status = 0;
  else
    status = 1;
#else /* _WINGUI */
  filedes = open (name, O_RDONLY);
  if (filedes < 0)
    status = 0;
  else
    {
      /* on ne veut pas de directory */
      if (fstat (filedes, &statinfo) != -1)
	{
	  if (statinfo.st_mode & S_IFDIR)
	    status = 0;
	  else
	    status = 1;
        }
      close (filedes);
    }
#endif /* _WINGUI */
  TtaFreeMemory (name);
  return status;
}

/*----------------------------------------------------------------------
   TtaFileUnlink : remove a file.                                     
  ----------------------------------------------------------------------*/
int TtaFileUnlink (CONST char *filename)
{
  int         ret;
  char       *name;

  if (filename)
    {
      name = GetRealFileName (filename);
      ret = unlink (name);
      TtaFreeMemory (name);
      return ret;
    }
  else
    return 0;
}

/*----------------------------------------------------------------------
   TtaGetFileSize
  ----------------------------------------------------------------------*/
unsigned long TtaGetFileSize (char *filename)
{
  ThotFileOffset      ret;
  char               *name;
  unsigned long       file_size = 0L;
  ThotFileHandle      handle;

  name = GetRealFileName (filename);
  handle = open (name, O_RDONLY);
  if (handle)
    {
#ifdef _WINGUI
      BY_HANDLE_FILE_INFORMATION info;

      ret = GetFileInformationByHandle (handle, &info);
      if (ret)
	file_size = info.nFileSizeLow;
#else  /* _WINGUI */
      struct stat         buf;

      ret = fstat (handle, &buf) == 0;
      if (ret)
	file_size = buf.st_size;
#endif /* _WINGUI */
      close (handle);
    }
   TtaFreeMemory (name);
   return file_size;
}

/*----------------------------------------------------------------------
   TtaFileCopy copies a source file into a target file.
   Return TRUE if the copy is done.
  ----------------------------------------------------------------------*/
ThotBool TtaFileCopy (CONST char *sourceFileName, CONST char *targetFileName)
{
  FILE               *targetf, *sourcef;
  int                 size;
  char                buffer[8192];

  if (!sourceFileName || !targetFileName)
    return FALSE;
  if (strcmp (sourceFileName, targetFileName) != 0)
    {
      if ((targetf = TtaWriteOpen (targetFileName)) == NULL)
	/* cannot write into the target file */
	return FALSE;
      else
	{
	  if ((sourcef = TtaReadOpen (sourceFileName)) == NULL)
	    {
	      /* cannot read the source file */
	      TtaWriteClose (targetf);
	      TtaFileUnlink (targetFileName);
	      return FALSE;
	    }
	  else
	    {
	      /* copy the file contents */
	      while ((size = fread (buffer, 1, 8192, sourcef)) != 0)
		fwrite (buffer, 1, size, targetf);
	      TtaReadClose (sourcef);
	    }
	  TtaWriteClose (targetf);
	}
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  TtaCompareFiles
  Compare the content of two files.
  Returns FALSE if one of the files is not available for reading or
  if their content differs, TRUE if they are identical.
  ----------------------------------------------------------------------*/
ThotBool TtaCompareFiles(CONST char *file1, CONST char *file2)
{
  FILE   *f1;
  FILE   *f2;
  char    buffer1[512];
  char    buffer2[512];
  size_t  res1;
  size_t  res2;

  if (file1 == NULL)
    return (FALSE);
  if (file2 == NULL)
    return (FALSE);
  f1 = TtaReadOpen (file1);
  if (f1 == NULL)
    return (FALSE);

  f2 = TtaReadOpen (file2);
  if (f2 == NULL)
    {
      TtaReadClose (f1);
      return (FALSE);
    }
  while (1)
    {
      res1 = fread (&buffer1[0], 1, sizeof(buffer1), f1);
      res2 = fread (&buffer2[0], 1, sizeof(buffer2), f2);
      if (res1 != res2)
	{
	  TtaReadClose (f1);
	  TtaReadClose (f2);
	  return (FALSE);
	}
      if (memcmp(&buffer1[0], &buffer2[0], res2))
	{
	  TtaReadClose (f1);
	  TtaReadClose (f2);
	  return (FALSE);
	}
      res1 = feof(f1);
      res2 = feof(f2);
      if (res1 != res2)
	{
	  TtaReadClose (f1);
	  TtaReadClose (f2);
	  return (FALSE);
	}
      if (res1)
	break;
    }
  TtaReadClose (f1);
  TtaReadClose (f2);
  return (TRUE);
}

