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
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "fileaccess.h"

/*----------------------------------------------------------------------
   TtaDirExists returns TRUE if the dirpath points to a directory.
   FALSE if the path points to a filename or doesn't
   point to anything.
  ----------------------------------------------------------------------*/
int TtaDirExists (CONST char *dirpath)
{
  int         status = 0;
#ifdef _WINGUI
  DWORD       attribs;

  attribs = GetFileAttributes ((LPCTSTR)dirpath);
  if (attribs == 0xFFFFFFFF)
    status = 0;
  else if (attribs & FILE_ATTRIBUTE_DIRECTORY)
    status = 1;
  else
    status = 0;
#else /* _WINGUI */
  struct stat buf;
#ifdef _WINDOWS /* SG TODO : a valider */
  status = stat(dirpath, &buf) == 0 && (((buf.st_mode)&S_IFMT) == S_IFDIR);
#else /* #ifdef _WINDOWS */
  status = stat(dirpath, &buf) == 0 && S_ISDIR (buf.st_mode);
#endif /* #ifdef _WINDOWS */
#endif /* _WINGUI */
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
#ifdef _WINGUI
  DWORD       attribs;

  attribs = GetFileAttributes ((LPCTSTR)filename);
  if (attribs == 0xFFFFFFFF)
    status = 0;
  else if (attribs & FILE_ATTRIBUTE_DIRECTORY)
    status = 0;
  else
    status = 1;
#else /* _WINGUI */

  int         filedes;
  struct stat statinfo;

  filedes = open (filename, O_RDONLY);
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
  return status;
}

/*----------------------------------------------------------------------
   TtaFileUnlink : remove a file.                                     
  ----------------------------------------------------------------------*/
int TtaFileUnlink (CONST char *filename)
{
  if (filename)
    return (unlink (filename));
  else
    return 0;
}

/*----------------------------------------------------------------------
   TtaFileOpen returns: ThotFile_BADHANDLE: error handle:		
  ----------------------------------------------------------------------*/
ThotFileHandle TtaFileOpen (CONST char *name, ThotFileMode mode)
{
   ThotFileHandle      ret;

#if defined(_WINGUI) && !defined(__GNUC__)
   DWORD               access = 0;	/* access (read-write) mode  */

   SECURITY_ATTRIBUTES secAttribs;
   DWORD               creation;	/* how to create  */

      secAttribs.nLength = sizeof (secAttribs);
   secAttribs.lpSecurityDescriptor = NULL;
   secAttribs.bInheritHandle = TRUE;
   if (mode & ThotFile_READ)
      access |= GENERIC_READ;
   if (mode & ThotFile_WRITE)
      access |= GENERIC_WRITE;
   if (mode & ThotFile_CREATE && mode & ThotFile_TRUNCATE)
      creation = CREATE_ALWAYS;
   else if (mode & ThotFile_CREATE && mode & ThotFile_EXCLUSIVE)
      creation = CREATE_NEW;
   else if (mode & ThotFile_TRUNCATE && !(mode & ThotFile_CREATE))
      creation = TRUNCATE_EXISTING;
   else if (mode & ThotFile_CREATE)
      creation = OPEN_ALWAYS;
   else
      creation = OPEN_EXISTING;
   ret = CreateFile (name, access, FILE_SHARE_READ, &secAttribs, creation, FILE_ATTRIBUTE_NORMAL, NULL);
#else  /* _WINGUI && !__GNUC__ */
#ifdef _WINDOWS
   ret = open (name, mode | _O_BINARY, 0777);
#else
   ret = open (name, mode, 0777);
#endif
#endif /* _WINGUI && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileClose returns, 0: error, 1: OK.				
  ----------------------------------------------------------------------*/
int TtaFileClose (ThotFileHandle handle)
{
   int                 ret;

#if defined(_WINGUI) && !defined(__GNUC__)
   ret = CloseHandle (handle);
#else  /* _WINGUI && !__GNUC__ */
   ret = close (handle) == 0;
#endif /* _WINGUI && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileRead returns +n: number of bytes read, 0: at EOF, -1: error 
  ----------------------------------------------------------------------*/
int TtaFileRead (ThotFileHandle handle, void *buffer, unsigned int count)
{
   int                 ret;

#if defined(_WINGUI) && !defined(__GNUC__)
   DWORD               red;

   /* OK as long as we don't open for overlapped IO */
   ret = ReadFile (handle, buffer, count, &red, 0);
   if (ret == TRUE)
      ret = (int) red;
   else
      ret = -1;
#else  /* _WINGUI && !__GNUC__ */
   ret = read (handle, buffer, count);
#endif /* _WINGUI && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileWrite returns:  n: number of bytes written, -1: error	
  ----------------------------------------------------------------------*/
int TtaFileWrite (ThotFileHandle handle, void *buffer, unsigned int count)
{
   int                 ret;

#if defined(_WINGUI) && !defined(__GNUC__)
   DWORD               writ;

   /* OK as long as we don't open for overlapped IO */
   ret = WriteFile (handle, buffer, count, &writ, NULL);
   if (ret == TRUE)
      ret = (int) writ;
   else
      ret = -1;
#else  /* _WINGUI && !__GNUC__ */
   ret = write (handle, buffer, count);
#endif /* _WINGUI && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileSeek returns: ThotFile_BADOFFSET: error, ThotFileOffset	
  ----------------------------------------------------------------------*/
ThotFileOffset TtaFileSeek (ThotFileHandle handle, ThotFileOffset offset,
			    ThotFileOrigin origin)
{
   ThotFileOffset      ret;

#if defined(_WINGUI) && !defined(__GNUC__)
   ret = SetFilePointer (handle, offset, 0, origin);
#else  /* _WINGUI && !__GNUC__ */
   ret = lseek (handle, offset, origin);
#endif /* _WINGUI && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileSeek returns: 1: your data is all there, sir, 0: error	
  ----------------------------------------------------------------------*/
int TtaFileStat (ThotFileHandle handle, ThotFileInfo *pInfo)
{
   ThotFileOffset      ret;

#if defined(_WINGUI) && !defined(__GNUC__)
   BY_HANDLE_FILE_INFORMATION info;

   ret = GetFileInformationByHandle (handle, &info);
   if (ret)
     {
	pInfo->atime = info.ftLastAccessTime;
	pInfo->size = info.nFileSizeLow;
     }
#else  /* _WINGUI && !__GNUC__ */
   struct stat         buf;

   ret = fstat (handle, &buf) == 0;
   if (ret)
     {
	pInfo->atime = buf.st_atime;
	pInfo->size = buf.st_size;
     }
#endif /* _WINGUI && !__GNUC__ */
   return ret;
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
#ifdef _WINDOWS
      if ((targetf = fopen (targetFileName, "wb")) == NULL)
#else
      if ((targetf = fopen (targetFileName, "w")) == NULL)
#endif
	/* cannot write into the target file */
	return FALSE;
      else
	{
#ifdef _WINDOWS
	  if ((sourcef = fopen (sourceFileName, "rb")) == NULL)
#else
          if ((sourcef = fopen (sourceFileName, "r")) == NULL)
#endif
	    {
	      /* cannot read the source file */
	      fclose (targetf);
	      unlink (targetFileName);
	      return FALSE;
	    }
	  else
	    {
	      /* copy the file contents */
	      while ((size = fread (buffer, 1, 8192, sourcef)) != 0)
		fwrite (buffer, 1, size, targetf);
	      fclose (sourcef);
	    }
	  fclose (targetf);
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
    return(FALSE);
  if (file2 == NULL)
    return(FALSE);
#ifdef _WINDOWS
  f1 = fopen(file1,"rb");
#else
  f1 = fopen(file1, "r");
#endif
  if (f1 == NULL) return(FALSE);
#ifdef _WINDOWS
  f2 = fopen(file2, "rb");
#else
  f2 = fopen(file2, "r");
#endif
  if (f2 == NULL)
    {
      fclose(f1);
      return(FALSE);
    }
  while (1)
    {
      res1 = fread(&buffer1[0], 1, sizeof(buffer1), f1);
      res2 = fread(&buffer2[0], 1, sizeof(buffer2), f2);
      if (res1 != res2)
	{
	  fclose(f1);
	  fclose(f2);
	  return(FALSE);
	}
      if (memcmp(&buffer1[0], &buffer2[0], res2))
	{
	  fclose(f1);
	  fclose(f2);
	  return(FALSE);
	}
      res1 = feof(f1);
      res2 = feof(f2);
      if (res1 != res2)
	{
	  fclose(f1);
	  fclose(f2);
	  return(FALSE);
	}
      if (res1) break;
    }
  fclose(f1);
  fclose(f2);
  return(TRUE);
}

