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
 * platform.c : basic system functions
 *
 * Authors: I. Vatton, D. Veillard (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "thotdir.h"
#include "fileaccess.h"

/*----------------------------------------------------------------------
   TtaFileExist teste l'existence d'un fichier.                       
   Rend 1 si le fichier a e't'e trouve' et 0 sinon.        
   Si filename est un repertoire, on retourne 0.           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaFileExist (CONST char* filename)
#else  /* __STDC__ */
int                 TtaFileExist (filename)
CONST char*    filename;

#endif /* __STDC__ */
{
   int                 status = 0;

#if defined(_WINDOWS) && !defined(__GNUC__)
   DWORD               attribs;

   attribs = GetFileAttributes ((LPCTSTR)filename);
   if (attribs == 0xFFFFFFFF)
      status = 0;
   else if (attribs & FILE_ATTRIBUTE_DIRECTORY)
      status = 0;
   else
      status = 1;
#else /* _WINDOWS && !__GNUC__ */
   int                 filedes;
   struct stat         statinfo;

#ifdef _WINDOWS
   filedes = open (filename, _O_RDONLY | O_BINARY);
#else /* _WINDOWS */
   filedes = open (filename, O_RDONLY);
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
#endif /* _WINDOWS && !__GNUC__ */
   return status;
}

/*----------------------------------------------------------------------
   TtaFileUnlink : remove a file.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaFileUnlink (CONST STRING filename)
#else  /* __STDC__ */
int                 TtaFileUnlink (filename)
CONST STRING        filename;

#endif /* __STDC__ */
{
  if (filename)
    return (uunlink (filename));
else
  return 0;
}

/*----------------------------------------------------------------------
   static ThotDirBrowseCopyFile - copy the filename from the          
   platform's directory structure     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          ThotDirBrowse_copyFile (ThotDirBrowse * me)
#else  /* __STDC__ */
static int          ThotDirBrowse_copyFile (me)
ThotDirBrowse      *me;

#endif /* __STDC__ */
{
#if defined(_WINDOWS) && !defined(__GNUC__)
   DWORD               attr;

   if (ustrlen (me->data.cFileName) + me->dirLen > (size_t)me->bufLen)
      return -2;
   ustrcpy (me->buf + me->dirLen, me->data.cFileName);
   if ((attr = GetFileAttributes (me->buf)) == 0xFFFFFFFF)
      return -1;
   if (attr & FILE_ATTRIBUTE_DIRECTORY && !(me->PicMask & ThotDirBrowse_DIRECTORIES))
      return 0;
   if (attr & FILE_ATTRIBUTE_NORMAL && !(me->PicMask & ThotDirBrowse_FILES))
      return 0;
   return 1;
#else  /* _WINDOWS && !__GNUC__ */
   unsigned int        i;
   int                 ls_car;
   ThotBool            notEof;
   struct stat         fileStat;

   while (TRUE)
     {
	ls_car = fgetc (me->ls_stream);
	/* saute les caracteres de separation */
	while (((CHAR_T) ls_car == SPACE) || ((CHAR_T) ls_car == TAB) ||
	       ((CHAR_T) ls_car == EOL))
	   ls_car = fgetc (me->ls_stream);
	notEof = TRUE;
	i = 0;
	while (((CHAR_T) ls_car != SPACE) && ((CHAR_T) ls_car != TAB) &&
	       ((CHAR_T) ls_car != EOL) && (notEof))
	  {
	     if (ls_car == EOF)
		notEof = FALSE;
	     else
	       {
		  me->buf[i] = (CHAR_T) ls_car;
		  i++;
		  if (i == me->bufLen)
		    {
		       me->buf[i - 1] = 0;
		       return -2;
		    }
		  ls_car = fgetc (me->ls_stream);
	       }
	  }
	me->buf[i] = EOS;
	if (notEof == FALSE && !i)
	   return 0;
	if (stat (me->buf, &fileStat) == -1)
	   return -1;
	/* next if fileStat is not included in our PicMask */
	if (S_ISDIR (fileStat.st_mode) && !(me->PicMask & ThotDirBrowse_DIRECTORIES))
	   continue;
	if (S_ISREG (fileStat.st_mode) && !(me->PicMask & ThotDirBrowse_FILES))
	   continue;
	return 1;
     }
#endif /* _WINDOWS && !__GNUC__ */
}

/*----------------------------------------------------------------------
   ThotDirBrowse_first - get first dir/name.ext and setup            
   platform dependent ThotDirBrowse structure                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ThotDirBrowse_first (ThotDirBrowse * me, char* dir, char* name, char* ext)
#else  /* __STDC__ */
int                 ThotDirBrowse_first (me, dir, name, ext)
ThotDirBrowse      *me;
PathBuffer          dir;
STRING              name;
STRING              ext;

#endif /* __STDC__ */
{
   CHAR_T              space[MAX_PATH];
   int                 ret;

   me->dirLen = ustrlen (dir);
   ustrcpy (me->buf, dir);
   ustrcpy (me->buf + (me->dirLen++), DIR_STR);
#if defined(_WINDOWS) && !defined(__GNUC__)
   usprintf (space, TEXT("%s\\%s%s"), dir ? dir : _EMPTYSTR_, name ? name : _EMPTYSTR_, ext ? ext : _EMPTYSTR_);
   me->handle = INVALID_HANDLE_VALUE;
   if ((me->handle = FindFirstFile (space, &me->data)) ==
       INVALID_HANDLE_VALUE)
      return -1;
   if ((ret = ThotDirBrowse_copyFile (me)) == 1)
      return 1;
   ret = ThotDirBrowse_next (me);
   if (ret == -1)
      FindClose (me->handle);
   return ret;
#else  /* _WINDOWS && !__GNUC__ */
   /* sprintf (space, "/bin/ls%s %s/%s%s 2>/dev/null", 
      ext && *ext ? "" : " -d", dir ? dir : "", 
      name ? name : "", ext ? ext : ""); - EGP */
   sprintf (space, "/bin/ls -d %s/%s%s 2>/dev/null", dir ? dir : "",
	    name ? name : "", ext ? ext : "");
   me->ls_stream = NULL;
   if ((me->ls_stream = popen (space, "r")) == NULL)
      return -1;
   if ((ret = ThotDirBrowse_copyFile (me)) == 1)
      return 1;
   pclose (me->ls_stream);
   me->ls_stream = NULL;
#endif /* _WINDOWS && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   ThotDirBrowse_next - get next file                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ThotDirBrowse_next (ThotDirBrowse * me)
#else  /* __STDC__ */
int                 ThotDirBrowse_next (me)
ThotDirBrowse      *me;

#endif /* __STDC__ */
{
#if defined(_WINDOWS) && !defined(__GNUC__)
   int                 ret;

   do
     {
	if (FindNextFile (me->handle, &me->data) != TRUE)
	   return GetLastError () == ERROR_NO_MORE_FILES ? 0 : -1;
     }
   while ((ret = ThotDirBrowse_copyFile (me)) == 0);
   return ret;
#else  /* _WINDOWS && !__GNUC__ */
   return ThotDirBrowse_copyFile (me);
#endif /* _WINDOWS && !__GNUC__ */
}

/*----------------------------------------------------------------------
   ThotDirBrowse_close - recover system resources                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ThotDirBrowse_close (ThotDirBrowse * me)
#else  /* __STDC__ */
int                 ThotDirBrowse_close (me)
ThotDirBrowse      *me;

#endif /* __STDC__ */
{
   int                 ret;

#if defined(_WINDOWS) && !defined(__GNUC__)
   if (me->handle == INVALID_HANDLE_VALUE)
      return 0;
   ret = (FindClose (me->handle) == 1 ? 1 : -1);
   me->handle = INVALID_HANDLE_VALUE;
#else  /* _WINDOWS && !__GNUC__ */
   if (me->ls_stream == NULL)
      return 0;
   ret = (pclose (me->ls_stream) == -1 ? -1 : 1);
   me->ls_stream = NULL;
#endif /* _WINDOWS && !__GNUC__ */
   return ret;
}

/* uniform file access for unix, WIN32, and refrigerator
   see ThotFile_test for sample usage
 */

#ifdef INCLUDE_TESTING_CODE
/* ThotFile_test - use to test ThotFile on any platform
 */
void                ThotFile_test (STRING name)
{
   ThotFileHandle      handle = ThotFile_BADHANDLE;
   ThotFileOffset      offset;
   ThotFileInfo        info;
   int                 i;
   CHAR_T              space[16];
   CONST STRING        format = "%15d\0";

   space[sizeof (space) - 1] = 0;
   printf ("ThotFile_test: opening %s for CREATE/READ/WRITE\n", name);
   handle = TtaFileOpen (name, ThotFile_CREATE | ThotFile_TRUNCATE | ThotFile_READWRITE);
   if (handle == ThotFile_BADHANDLE)
     {
	printf ("ThotFile_test: handle == ThotFile_BADHANDLE\n");
	return;
     }
   printf ("ThotFile_test: writing %s\n", name);
   for (i = 0; i < 100; i++)
     {
	sprintf (space, format, i);
	if (TtaFileWrite (handle, space, sizeof (space)) == -1)
	  {
	     printf ("ThotFile_test: bad write\n");
	     break;
	  }
     }
   printf ("ThotFile_test: closing %s\n", name);
   if (TtaFileClose (handle) == 0)
     {
	printf ("ThotFile_test: bad write\n");
	return;
     }
   printf ("ThotFile_test: reopening %s\n", name);
   handle = TtaFileOpen (name, ThotFile_READWRITE);
   if (handle == ThotFile_BADHANDLE)
     {
	printf ("ThotFile_test: handle == ThotFile_BADHANDLE\n");
	return;
     }
   printf ("ThotFile_test: stating %s\n", name);
   if (TtaFileStat (handle, &info) == 0)
     {
	printf ("TtaFileStat: bad stat\n");
	return;
     }
   printf ("ThotFile_test: file %s is %d bytes long\n", name, info.size);
   printf ("ThotFile_test: seeking end of %s\n", name);
   offset = TtaFileSeek (handle, 0, ThotFile_SEEKEND);
   if (offset == ThotFile_BADOFFSET)
     {
	printf ("ThotFile_test: offset == ThotFile_BADOFFSET\n");
	return;
     }
   printf ("ThotFile_test: end found at %d\n", offset);
   printf ("ThotFile_test: seeking to offset %d in %s\n", offset - 4 * sizeof (space), name);
   offset = TtaFileSeek (handle, offset - 4 * sizeof (space), ThotFile_SEEKSET);
   if (offset == ThotFile_BADOFFSET)
     {
	printf ("ThotFile_test: offset == ThotFile_BADOFFSET\n");
	return;
     }
   printf ("ThotFile_test: reading %s\n", name);
   for (i -= 4; 1; i++)
     {
	int                 red;

	red = TtaFileRead (handle, space, sizeof (space));
	if (red == -1)
	  {
	     printf ("ThotFile_test: bad write\n");
	     break;
	  }
	if (red == 0)
	  {
	     printf ("ThotFile_test: end of file %s\n", name);
	     break;
	  }
	printf ("ThotFile_test: %s should == %d\n", space, i);
     }
   printf ("ThotFile_test: closing %s\n", name);
   if (TtaFileClose (handle) == 0)
     {
	printf ("ThotFile_test: bad write\n");
	return;
     }
}
#endif /* INCLUDE_TESTING_CODE */


/*----------------------------------------------------------------------
   TtaFileOpen returns: ThotFile_BADHANDLE: error handle:		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotFileHandle      TtaFileOpen (CONST STRING name, ThotFileMode mode)
#else  /* __STDC__ */
ThotFileHandle      TtaFileOpen (name, mode)
CONST STRING        name;
ThotFileMode        mode;

#endif /* __STDC__ */
{
   ThotFileHandle      ret;

#if defined(_WINDOWS) && !defined(__GNUC__)
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
#else  /* _WINDOWS && !__GNUC__ */
#ifdef _WINDOWS_
   ret = open (name, mode | _O_BINARY, 0777);
#else
   ret = open (name, mode, 0777);
#endif
#endif /* _WINDOWS && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileClose returns, 0: error, 1: OK.				
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaFileClose (ThotFileHandle handle)
#else  /* __STDC__ */
int                 TtaFileClose (handle)
ThotFileHandle      handle;

#endif /* __STDC__ */
{
   int                 ret;

#if defined(_WINDOWS) && !defined(__GNUC__)
   ret = CloseHandle (handle);
#else  /* _WINDOWS && !__GNUC__ */
   ret = close (handle) == 0;
#endif /* _WINDOWS && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileRead returns +n: number of bytes read, 0: at EOF, -1: error 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaFileRead (ThotFileHandle handle, void *buffer, unsigned int count)
#else  /* __STDC__ */
int                 TtaFileRead (handle, buffer, count)
ThotFileHandle      handle;
void               *buffer;
unsigned int        count;

#endif /* __STDC__ */
{
   int                 ret;

#if defined(_WINDOWS) && !defined(__GNUC__)
   DWORD               red;

   ret = ReadFile (handle, buffer, count, &red, 0);	/* OK as long as we don't open for overlapped IO */
   if (ret == TRUE)
      ret = (int) red;
   else
      ret = -1;
#else  /* _WINDOWS && !__GNUC__ */
   ret = read (handle, buffer, count);
#endif /* _WINDOWS && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileWrite returns:  n: number of bytes written, -1: error	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaFileWrite (ThotFileHandle handle, void *buffer, unsigned int count)
#else  /* __STDC__ */
int                 TtaFileWrite (handle, buffer, count)
ThotFileHandle      handle;
void               *buffer;
unsigned int        count;

#endif /* __STDC__ */
{
   int                 ret;

#if defined(_WINDOWS) && !defined(__GNUC__)
   DWORD               writ;

   /* OK as long as we don't open for overlapped IO */
   ret = WriteFile (handle, buffer, count, &writ, NULL);
   if (ret == TRUE)
      ret = (int) writ;
   else
      ret = -1;
#else  /* _WINDOWS && !__GNUC__ */
   ret = write (handle, buffer, count);
#endif /* _WINDOWS && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileSeek returns: ThotFile_BADOFFSET: error, ThotFileOffset	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotFileOffset      TtaFileSeek (ThotFileHandle handle, ThotFileOffset offset, ThotFileOrigin origin)
#else  /* __STDC__ */
ThotFileOffset      TtaFileSeek (handle, offset, origin)
ThotFileHandle      handle;
ThotFileOffset      offset;
ThotFileOrigin      origin;

#endif /* __STDC__ */
{
   ThotFileOffset      ret;

#if defined(_WINDOWS) && !defined(__GNUC__)
   ret = SetFilePointer (handle, offset, 0, origin);
#else  /* _WINDOWS && !__GNUC__ */
   ret = lseek (handle, offset, origin);
#endif /* _WINDOWS && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileSeek returns: 1: your data is all there, sir, 0: error	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaFileStat (ThotFileHandle handle, ThotFileInfo * pInfo)
#else  /* __STDC__ */
int                 TtaFileStat (handle, pInfo)
ThotFileHandle      handle;
ThotFileInfo       *pInfo;

#endif /* __STDC__ */
{
   ThotFileOffset      ret;

#if defined(_WINDOWS) && !defined(__GNUC__)
   BY_HANDLE_FILE_INFORMATION info;

   ret = GetFileInformationByHandle (handle, &info);
   if (ret)
     {
	pInfo->atime = info.ftLastAccessTime;
	pInfo->size = info.nFileSizeLow;
     }
#else  /* _WINDOWS && !__GNUC__ */
   struct stat         buf;

   ret = fstat (handle, &buf) == 0;
   if (ret)
     {
	pInfo->atime = buf.st_atime;
	pInfo->size = buf.st_size;
     }
#endif /* _WINDOWS && !__GNUC__ */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileCopy copies a source file into a target file.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaFileCopy (CONST STRING sourceFileName, CONST STRING targetFileName)
#else
void                TtaFileCopy (sourceFileName, targetFileName)
CONST STRING        sourceFileName;
CONST STRING        targetFileName;

#endif
{
   FILE               *targetf;
   FILE               *sourcef;
   int                 size;
   CHAR_T                buffer[8192];

   if (ustrcmp (sourceFileName, targetFileName) != 0)
     {
#ifdef _WINDOWS
	if ((targetf = ufopen (targetFileName, _WBinaryMODE_)) == NULL)
#else
	if ((targetf = ufopen (targetFileName, "w")) == NULL)
#endif
	   /* cannot write into the target file */
	   return;
	else
	  {
#ifdef _WINDOWS
	     if ((sourcef = ufopen (sourceFileName, _RBinaryMODE_)) == NULL)
#else
	     if ((sourcef = ufopen (sourceFileName, "r")) == NULL)
#endif
	       {
		  /* cannot read the source file */
		  fclose (targetf);
		  uunlink (targetFileName);
		  return;
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
}

/*----------------------------------------------------------------------
  TtaCompareFiles
  Compare the content of two files.
  Returns FALSE if one of the files is not available for reading or
  if their content differs, TRUE if they are identical.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaCompareFiles(CONST STRING file1, CONST STRING file2)
#else
ThotBool            TtaCompareFiles(file1, file2)
CONST STRING        file1;
CONST STRING        file2;

#endif
{
    FILE *f1;
    FILE *f2;
    CHAR_T buffer1[512];
    CHAR_T buffer2[512];
    size_t res1;
    size_t res2;

    if (file1 == NULL) return(FALSE);
    if (file2 == NULL) return(FALSE);
#ifdef _WINDOWS
    f1 = ufopen(file1,_RBinaryMODE_);
#else
    f1 = ufopen(file1,"r");
#endif
    if (f1 == NULL) return(FALSE);
#ifdef _WINDOWS
    f2 = ufopen(file2, _RBinaryMODE_);
#else
    f2 = fopen(file2,"r");
#endif
    if (f2 == NULL) {
	fclose(f1);
        return(FALSE);
    }
    while (1) {
        res1 = fread(&buffer1[0], 1, sizeof(buffer1), f1);
        res2 = fread(&buffer2[0], 1, sizeof(buffer2), f2);
	if (res1 != res2) {
	    fclose(f1);
	    fclose(f2);
	    return(FALSE);
	}
        if (memcmp(&buffer1[0], &buffer2[0], res2)) {
	    fclose(f1);
	    fclose(f2);
	    return(FALSE);
	}
	res1 = feof(f1);
	res2 = feof(f2);
	if (res1 != res2) {
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

