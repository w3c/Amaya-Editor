/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
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
  #include "wx/file.h"
#endif /* _WX */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "zlib.h"
#include "message_wx.h"

#include "fileaccess_f.h"
#include "memory_f.h"


/*----------------------------------------------------------------------
   TtaDirExists returns TRUE if the name points to a directory.
   FALSE if the path points to a filename or doesn't
   point to anything.
  ----------------------------------------------------------------------*/
int ThotDirExists (CONST char *name)
{
#ifdef _WX
  wxASSERT_MSG(FALSE, _T("unused funtion"));
  return FALSE;
#else /* _WX */
  int         status = 0;
#ifdef _WINGUI
  DWORD       attribs;
#endif /* _WINGUI */

#ifdef _WINGUI
  attribs = GetFileAttributes ((LPCTSTR)name);
  if (attribs == 0xFFFFFFFF)
    status = 0;
  else if (attribs & FILE_ATTRIBUTE_DIRECTORY)
    status = 1;
  else
    status = 0;
#else /* _WINGUI */
  struct stat buf;
  status = stat (name, &buf) == 0 && S_ISDIR (buf.st_mode);
#endif /* _WINGUI */
  return status;
#endif /* _WX */
}

/*----------------------------------------------------------------------
   TtaDirExists returns TRUE if the dirpath points to a directory.
   FALSE if the path points to a filename or doesn't
   point to anything.
  ----------------------------------------------------------------------*/
int TtaDirExists (CONST char *dirpath)
{
#ifdef _WX
  return wxDirExists(TtaConvMessageToWX(dirpath));
#else /* #ifdef _WX */
  int         status = 0;
  char       *name;
  name = GetRealFileName (dirpath);
  status = ThotDirExists (name);
  TtaFreeMemory (name);
  return status;
#endif /* _WX */
}

/*----------------------------------------------------------------------
   ThotFileExist checks if the file exists.
   Return 1 is TRUE else 0.
  ----------------------------------------------------------------------*/
int ThotFileExist (CONST char *name)
{
#ifdef _WX
  wxASSERT_MSG(FALSE, _T("unused function"));
  return FALSE;
#else /* _WX */
  int         status = 0;
#ifdef _WINGUI
  DWORD       attribs;
#else /* _WINGUI */
  int         filedes;
  struct stat statinfo;
#endif /* _WINGUI */

#ifdef _WINGUI
  attribs = GetFileAttributes ((LPCTSTR)name);
  if (attribs == 0xFFFFFFFF)
    status = 0;
  else if (attribs & FILE_ATTRIBUTE_DIRECTORY)
    status = 0;
  else
    status = 1;
#else /* _WINGUI */
#ifdef _WINDOWS
  filedes = open (name, O_RDONLY | _O_BINARY, 0777);
  if (filedes < 0)
    filedes = open (name, O_RDWR| _O_BINARY, 0777);
#else /* _WINDOWS */
  filedes = open (name, O_RDONLY);
#endif /* _WINDOWS */
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
#endif /* _WX */
}

/*----------------------------------------------------------------------
   TtaFileExist teste l'existence d'un fichier.                       
   Rend TRUE si le fichier a e't'e trouve' et FALSE sinon.        
   Si filename est un repertoire, on retourne FALSE.
  ----------------------------------------------------------------------*/
ThotBool TtaFileExist (CONST char *filename)
{
#ifdef _WX
  return wxFile::Exists(TtaConvMessageToWX(filename));
#else /* _WX */
  char       *name;
  int         status = 0;

  name = GetRealFileName (filename);
  status = ThotFileExist (name);
  TtaFreeMemory (name);

  return status;
#endif /* _WX */
}

/*----------------------------------------------------------------------
   TtaFileUnlink : remove a file.                             
   Return TRUE if succesfull.
  ----------------------------------------------------------------------*/
ThotBool TtaFileUnlink (CONST char *filename)
{
#ifdef _WX
  return wxRemoveFile(TtaConvMessageToWX(filename));
#else /* _WX */
  int         ret;
  char       *name;

  if (filename)
    {
      name = GetRealFileName (filename);
      ret = unlink (name);
      TtaFreeMemory (name);
      return (ret == -1);
    }
  else
    return TRUE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
   TtaDirectoryUnlink : remove a directory.                             
   Return TRUE if succesfull.
  ----------------------------------------------------------------------*/
ThotBool TtaDirectoryUnlink (CONST char *dirname)
{
#ifdef _WX
  return wxRmdir (TtaConvMessageToWX(dirname));
#else /* _WX */
  int         ret;
  char       *name;

  if (dirname)
    {
      name = GetRealFileName (dirname);
      ret = rmdir (name);
      TtaFreeMemory (name);
      return (ret == -1);
    }
  else
    return TRUE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
   TtaFileRename rename oldname to newname, returns true if succesfull
  ----------------------------------------------------------------------*/
ThotBool TtaFileRename( const char * oldname, const char * newname )
{
#ifdef _WX
  return wxRenameFile(TtaConvMessageToWX(oldname), TtaConvMessageToWX(newname));
#else /* _WX */
  return ( rename( oldname, newname ) == 0 );
#endif /* _WX */
}


#ifndef _WX
/*----------------------------------------------------------------------
   TtaFileOpen returns: ThotFile_BADHANDLE: error handle:		
  ----------------------------------------------------------------------*/
static ThotFileHandle TtaFileOpen (CONST char *name, ThotFileMode mode)
{
   ThotFileHandle      ret;
#ifdef _WINGUI
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
#else  /* _WINGUI */
   ret = open (name, mode, 0777);
#endif /* _WINGUI */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileClose returns, 0: error, 1: OK.				
  ----------------------------------------------------------------------*/
static int TtaFileClose (ThotFileHandle handle)
{
  int                 ret;

#ifdef _WINGUI
  ret = CloseHandle (handle);
#else  /* _WINGUI */
  ret = close (handle) == 0;
#endif /* _WINGUI */
  return ret;
}
#endif /* _WX */

/*----------------------------------------------------------------------
   TtaGetFileSize
  ----------------------------------------------------------------------*/
unsigned long TtaGetFileSize (const char *filename)
{
#ifdef _WX
  wxFile file(TtaConvMessageToWX(filename));
  return file.Length();
#else /* _WX */
  ThotFileOffset      ret;
  char               *name;
  unsigned long       file_size = 0L;
  ThotFileHandle      handle;

  name = GetRealFileName (filename);
  handle = TtaFileOpen (name, ThotFile_READWRITE);
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
      TtaFileClose (handle);
    }
   TtaFreeMemory (name);
   return file_size;
#endif /* _WX */
}

/*----------------------------------------------------------------------
   TtaFileCopy copies a source file into a target file.
   Return TRUE if the copy is done.
  ----------------------------------------------------------------------*/
ThotBool TtaFileCopy (CONST char *sourceFileName, CONST char *targetFileName)
{
#ifdef _WX
  char     *tmp;
  ThotBool result;
  wxString targetFile = TtaConvMessageToWX(targetFileName);
  wxString sourceFile = TtaConvMessageToWX(sourceFileName);
  wxString tmpFile;

  if (!wxFile::Exists(sourceFile))
	return FALSE;
  else if (wxFile::Exists(targetFile) &&
           !wxFile::Access(targetFile, wxFile::write))
    return FALSE;
  else if (wxFile::Access(sourceFile, wxFile::write))
    return wxCopyFile (sourceFile,targetFile, TRUE);
  else
    {
      tmp = (char *)TtaGetMemory (strlen(targetFileName)+10);
      sprintf (tmp, "%s.tmp", targetFileName);
      wxFile (targetFile, wxFile::write);
      tmpFile = TtaConvMessageToWX(tmp);
      wxFile (tmpFile, wxFile::write);
      result = wxFile::Exists(targetFile);
      if (result)
        result = wxConcatFiles (tmpFile, sourceFile, targetFile);
      wxRemoveFile(tmpFile);
      TtaFreeMemory (tmp);
      return result;
    }
#else /* _WX */
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
#endif /* _WX */
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

