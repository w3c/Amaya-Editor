/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

#ifndef _FILEACCESS_H_
#define _FILEACCESS_H_

#include "thot_sys.h"
#include "typebase.h"
#ifndef NODISPLAY
#include "zlib.h"
#endif /* NODISPLAY */

typedef FILE *BinFile;

typedef struct
{ /* fill in with more elements as they are needed */
#if defined(_WINGUI) && !defined(__GNUC__)
    FILETIME atime; /* don't know yet what format will be used most */
    DWORD size;
#else /* _WINGUI */
    time_t atime;
    off_t size;
#endif /* !_WINGUI */
} ThotFileInfo;

#if defined(_WINGUI) && !defined(__GNUC__)

typedef HANDLE ThotFileHandle;
#define ThotFile_BADHANDLE INVALID_HANDLE_VALUE

typedef enum {ThotFile_CREATE = 0x1, ThotFile_TRUNCATE = 0x2, 
	      ThotFile_EXCLUSIVE = 0x4, ThotFile_READ = 0x10, 
	      ThotFile_WRITE = 0x20, ThotFile_READWRITE = 0x30} ThotFileMode;

typedef DWORD ThotFileOrigin;
#define ThotFile_SEEKSET SEEK_SET
#define ThotFile_SEEKCUR SEEK_CUR
#define ThotFile_SEEKEND SEEK_END

typedef long ThotFileOffset;
#define ThotFile_BADOFFSET 0xFFFFFFFF

#else /* ! _WINGUI */

typedef int ThotFileHandle;
#define ThotFile_BADHANDLE -1

typedef int ThotFileMode;
#define ThotFile_CREATE O_CREAT
#define ThotFile_READWRITE O_RDWR
#define ThotFile_TRUNCATE O_TRUNC
#define ThotFile_EXCLUSIVE O_EXCL

typedef int ThotFileOrigin;
#define ThotFile_SEEKSET SEEK_SET
#define ThotFile_SEEKCUR SEEK_CUR
#define ThotFile_SEEKEND SEEK_END

typedef long ThotFileOffset;
#define ThotFile_BADOFFSET -1L
#endif /* !_WINGUI */

#ifndef __CEXTRACT__
extern int TtaDirExists ( const char *dirpath );
extern ThotBool TtaFileExist ( const char *filename );
extern ThotBool TtaFileUnlink ( const char *filename );
extern ThotBool TtaDirectoryUnlink ( const char *filename );
extern ThotBool TtaFileRename( const char * oldname, const char * newname );
extern unsigned long TtaGetFileSize (const char *filename);
extern ThotBool TtaFileCopy ( const char *sourceFileName,
    const char *targetFileName );

/*----------------------------------------------------------------------
   TtaFileCopyUncompress copies a source file into a target file and
   uncompress if necessary
   Return TRUE if the copy is done.
  ----------------------------------------------------------------------*/
extern ThotBool TtaFileCopyUncompress (const char *sourceFile,
    const char *targetFile);

extern ThotBool TtaReadByte ( BinFile file,
                             unsigned char *bval );
extern ThotBool TtaReadWideChar (BinFile file, CHAR_T *bval);

extern ThotBool TtaReadBool ( BinFile file,
                             ThotBool * bval );
extern ThotBool TtaReadShort ( BinFile file,
                              int *sval );
extern ThotBool TtaReadSignedShort ( BinFile file,
                                    int *sval );
extern ThotBool TtaReadInteger ( BinFile file,
                                int *sval );
extern ThotBool TtaReadName ( BinFile file,
                             unsigned char *name );
extern char * TtaGetRealFileName(CONST char *name);

#ifndef NODISPLAY
extern gzFile TtaGZOpen (const char *filename);
extern void TtaGZClose (gzFile file);
#endif /* NODISPLAY */
extern BinFile TtaReadOpen (const char *filename);
extern void TtaReadClose ( BinFile file );
extern BinFile TtaWriteOpen (const char *filename);
extern BinFile TtaAddOpen (const char *filename);
extern BinFile TtaRWOpen (const char *filename);
extern void TtaWriteClose ( BinFile file );
extern ThotBool TtaWriteByte ( BinFile file,
                              char bval );
extern ThotBool TtaWriteWideChar (BinFile file, CHAR_T bval);
extern ThotBool TtaWriteShort ( BinFile file,
                               int sval );
extern ThotBool TtaWriteInteger ( BinFile file,
                                 int lval );
extern ThotBool TtaCompareFiles ( const char *file1,
    const char *file2 );

/*----------------------------------------------------------------------
   TtaMakeDirectory
   Platform independent call to the local mkdir function
   Return value:
   TRUE if the directory could be created or if it existed already,
   FALSE otherwise.
  ----------------------------------------------------------------------*/
extern ThotBool TtaMakeDirectory ( const char *directory );

/*----------------------------------------------------------------------
   TtaCheckDirectory
   Ckecks that a directory exists and can be accessed.
   Return value:
   TRUE if the directory is OK, FALSE if not.	
  ----------------------------------------------------------------------*/
extern ThotBool TtaCheckDirectory ( const char *directory );

/*----------------------------------------------------------------------
  TtaCheckMakeDirectory
  Checks that a directory name exists. If No, it tries to create it.
  If recusive == TRUE, it tries to create all the intermediary directories.
  Return value:
  TRUE if the operation succeeds, FALSE otherwise.
  ----------------------------------------------------------------------*/
extern ThotBool TtaCheckMakeDirectory (const char *name, ThotBool recursive);

/*----------------------------------------------------------------------
  TtaIsW3Path returns TRUE if path is in fact a URL.       
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsW3Path (const char *path);


#endif /* __CEXTRACT__ */
#endif /* _FILEACCESS_H_ */

