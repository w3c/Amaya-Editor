/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
   platform.c : basic system functions
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
int                 TtaFileExist (char *filename)
#else  /* __STDC__ */
int                 TtaFileExist (filename)
char               *filename;

#endif /* __STDC__ */
{
   int                 status = 0;

#ifdef WWW_MSWINDOWS
   DWORD               attribs;

   attribs = GetFileAttributes (filename);
   if (attribs == 0xFFFFFFFF)
      status = 0;
   else if (attribs & FILE_ATTRIBUTE_DIRECTORY)
      status = 0;
   else
      status = 1;
#else  /* WWW_MSWINDOWS */
   int                 filedes;
   struct stat         statinfo;

   filedes = open (filename, 0);
   if (filedes < 0)
      status = 0;
   else
     {
	if (fstat (filedes, &statinfo) != -1)
	  {
	     if (statinfo.st_mode & S_IFDIR)
		/* on ne veut pas de directory */
		status = 0;
	     else
		status = 1;
	  }
	close (filedes);
     }
#endif /* !WWW_MSWINDOWS */
   return status;
}

/*----------------------------------------------------------------------
   TtaFileUnlink : remove a file.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaFileUnlink (char *filename)
#else  /* __STDC__ */
int                 TtaFileUnlink (filename)
char               *filename;

#endif /* __STDC__ */
{
#ifdef WWW_MSWINDOWS
   return (remove (filename));
#else
   return (unlink (filename));
#endif
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
#ifdef WWW_MSWINDOWS
   DWORD               attr;

   if (strlen (me->data.cFileName) + me->dirLen > me->bufLen)
      return -2;
   strcpy (me->buf + me->dirLen, me->data.cFileName);
   if ((attr = GetFileAttributes (me->buf)) == 0xFFFFFFFF)
      return -1;
   if (attr & FILE_ATTRIBUTE_DIRECTORY && !(me->PicMask & ThotDirBrowse_DIRECTORIES))
      return 0;
   if (attr & FILE_ATTRIBUTE_NORMAL && !(me->PicMask & ThotDirBrowse_FILES))
      return 0;
   return 1;
#else  /* WWW_MSWINDOWS */
   int                 i;
   int                 ls_car;
   BOOLEAN             notEof;
   struct stat         fileStat;

   while (TRUE)
     {
	ls_car = fgetc (me->ls_stream);
	/* saute les caracteres de separation */
	while (((char) ls_car == ' ') || ((char) ls_car == '\t') ||
	       ((char) ls_car == '\n'))
	   ls_car = fgetc (me->ls_stream);
	notEof = TRUE;
	i = 0;
	while (((char) ls_car != ' ') && ((char) ls_car != '\t') &&
	       ((char) ls_car != '\n') && (notEof))
	  {
	     if (ls_car == EOF)
		notEof = FALSE;
	     else
	       {
		  me->buf[i] = (char) ls_car;
		  i++;
		  if (i == me->bufLen)
		    {
		       me->buf[i - 1] = 0;
		       return -2;
		    }
		  ls_car = fgetc (me->ls_stream);
	       }
	  }
	me->buf[i] = '\0';
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
#endif /* !WWW_MSWINDOWS */
}

/*----------------------------------------------------------------------
   ThotDirBrowse_first - get first dir/name.ext and setup            
   platform dependent ThotDirBrowse structure                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ThotDirBrowse_first (ThotDirBrowse * me, char *dir, char *name, char *ext)
#else  /* __STDC__ */
int                 ThotDirBrowse_first (me, dir, name, ext)
ThotDirBrowse      *me;
char               *dir;
char               *name;
char               *ext;

#endif /* __STDC__ */
{
   char                space[MAX_PATH];
   int                 ret;

   me->dirLen = strlen (dir);
   strcpy (me->buf, dir);
   strcpy (me->buf + (me->dirLen++), DIR_STR);
#ifdef WWW_MSWINDOWS
   sprintf (space, "%s\\%s%s", dir ? dir : "", name ? name : "",
	    ext ? ext : "");
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
#else  /* WWW_MSWINDOWS */
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
#endif /* !WWW_MSWINDOWS */
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
#ifdef WWW_MSWINDOWS
   int                 ret;

   do
     {
	if (FindNextFile (me->handle, &me->data) != TRUE)
	   return GetLastError () == ERROR_NO_MORE_FILES ? 0 : -1;
     }
   while ((ret = ThotDirBrowse_copyFile (me)) == 0);
   return ret;
#else  /* WWW_MSWINDOWS */
   return ThotDirBrowse_copyFile (me);
#endif /* !WWW_MSWINDOWS */
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

#ifdef WWW_MSWINDOWS
   if (me->handle == INVALID_HANDLE_VALUE)
      return 0;
   ret = (FindClose (me->handle) == 1 ? 1 : -1);
   me->handle = INVALID_HANDLE_VALUE;
#else  /* WWW_MSWINDOWS */
   if (me->ls_stream == NULL)
      return 0;
   ret = (pclose (me->ls_stream) == -1 ? -1 : 1);
   me->ls_stream = NULL;
#endif /* !WWW_MSWINDOWS */
   return ret;
}

/* uniform file access for unix, WIN32, and refrigerator
   see ThotFile_test for sample usage
 */

#ifdef INCLUDE_TESTING_CODE
/* ThotFile_test - use to test ThotFile on any platform
 */
void                ThotFile_test (char *name)
{
   ThotFileHandle      handle = ThotFile_BADHANDLE;
   ThotFileOffset      offset;
   ThotFileInfo        info;
   int                 i;
   char                space[16];
   const char         *format = "%15d\0";

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
   CoreHandler est un handler d'erreur fatale.                     
  ----------------------------------------------------------------------*/
static void         ErrorHandler ()
{
#ifndef _WINDOWS
   signal (SIGBUS, SIG_DFL);
   signal (SIGSEGV, SIG_DFL);
   signal (SIGPIPE, SIG_IGN);
#ifdef SIGABRT
   signal (SIGABRT, SIG_DFL);
#else
   signal (SIGIOT, SIG_DFL);
#endif
#endif /* _WINDOWS */
   exit (1);
}


/*----------------------------------------------------------------------
   ThotExit termine l'application Thot.                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ThotExit (int result)
#else  /* __STDC__ */
void                ThotExit (result)
int                 result;

#endif /* __STDC__ */
{
   fflush (stderr);
   fflush (stdout);
   if (result)
      ErrorHandler ();
   else
      exit (result);
}


/*----------------------------------------------------------------------
   QuitHandler est un handler pour Interrupt.                      
  ----------------------------------------------------------------------*/
static void         QuitHandler ()
{
#ifndef _WINDOWS
   signal (SIGINT, ErrorHandler);
   signal (SIGQUIT, SIG_DFL);
   signal (SIGTERM, ErrorHandler);
#endif /* _WINDOWS */
   exit (1);
#ifndef _WINDOWS
   signal (SIGINT, QuitHandler);
   signal (SIGQUIT, QuitHandler);
   signal (SIGTERM, QuitHandler);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   InitErrorHandler initialise le handler de core dump.             
  ----------------------------------------------------------------------*/
void                InitErrorHandler ()
{
#ifndef _WINDOWS
   signal (SIGBUS, ErrorHandler);
   signal (SIGSEGV, ErrorHandler);
#ifdef SIGABRT
   signal (SIGABRT, ErrorHandler);
#else
   signal (SIGIOT, ErrorHandler);
#endif

   signal (SIGHUP, ErrorHandler);
   signal (SIGINT, QuitHandler);
   signal (SIGQUIT, QuitHandler);
   signal (SIGTERM, QuitHandler);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   TtaFileOpen returns: ThotFile_BADHANDLE: error handle:		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotFileHandle      TtaFileOpen (char *name, ThotFileMode mode)
#else  /* __STDC__ */
ThotFileHandle      TtaFileOpen (name, mode)
char               *name;
ThotFileMode        mode;

#endif /* __STDC__ */
{
   ThotFileHandle      ret;

#ifdef WWW_MSWINDOWS
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
#else  /* WWW_MSWINDOWS */
   ret = open (name, mode, 0777);
#endif /* !WWW_MSWINDOWS */
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

#ifdef WWW_MSWINDOWS
   ret = CloseHandle (handle);
#else  /* WWW_MSWINDOWS */
   ret = close (handle) == 0;
#endif /* !WWW_MSWINDOWS */
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

#ifdef WWW_MSWINDOWS
   DWORD               red;

   ret = ReadFile (handle, buffer, count, &red, 0);	/* OK as long as we don't open for overlapped IO */
   if (ret == TRUE)
      ret = (int) red;
   else
      ret = -1;
#else  /* WWW_MSWINDOWS */
   ret = read (handle, buffer, count);
#endif /* !WWW_MSWINDOWS */
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

#ifdef WWW_MSWINDOWS
   DWORD               writ;

   /* OK as long as we don't open for overlapped IO */
   ret = WriteFile (handle, buffer, count, &writ, NULL);
   if (ret == TRUE)
      ret = (int) writ;
   else
      ret = -1;
#else  /* WWW_MSWINDOWS */
   ret = write (handle, buffer, count);
#endif /* !WWW_MSWINDOWS */
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

#ifdef WWW_MSWINDOWS
   ret = SetFilePointer (handle, offset, 0, origin);
#else  /* WWW_MSWINDOWS */
   ret = lseek (handle, offset, origin);
#endif /* !WWW_MSWINDOWS */
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

#ifdef WWW_MSWINDOWS
   BY_HANDLE_FILE_INFORMATION info;

   ret = GetFileInformationByHandle (handle, &info);
   if (ret)
     {
	pInfo->atime = info.ftLastAccessTime;
	pInfo->size = info.nFileSizeLow;
     }
#else  /* WWW_MSWINDOWS */
   struct stat         buf;

   ret = fstat (handle, &buf) == 0;
   if (ret)
     {
	pInfo->atime = buf.st_atime;
	pInfo->size = buf.st_size;
     }
#endif /* !WWW_MSWINDOWS */
   return ret;
}

/*----------------------------------------------------------------------
   TtaFileCopy copies a source file into a target file.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaFileCopy (char *sourceFileName, char *targetFileName)
#else
void                TtaFileCopy (sourceFileName, targetFileName)
char               *sourceFileName;
char               *targetFileName;

#endif
{
   FILE               *targetf;
   FILE               *sourcef;
   int                 size;
   char                buffer[8192];

   if (strcmp (sourceFileName, targetFileName) != 0)
     {
	if ((targetf = fopen (targetFileName, "w")) == NULL)
	   /* cannot write into the target file */
	   return;
	else
	  {
	     if ((sourcef = fopen (sourceFileName, "r")) == NULL)
	       {
		  /* cannot read the source file */
		  fclose (targetf);
		  unlink (targetFileName);
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
