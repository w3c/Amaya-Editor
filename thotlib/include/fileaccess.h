/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2004
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
extern int TtaDirExists ( CONST char *dirpath );
extern int TtaFileExist ( CONST char *filename );
extern int TtaFileUnlink ( CONST char *filename );
extern ThotBool TtaFileRename( const char * oldname, const char * newname );
extern unsigned long TtaGetFileSize (char *filename);
extern ThotBool TtaFileCopy ( CONST char *sourceFileName,
			      CONST char *targetFileName );

/*----------------------------------------------------------------------
   TtaFileCopyUncompress copies a source file into a target file and
   uncompress if necessary
   Return TRUE if the copy is done.
  ----------------------------------------------------------------------*/
extern ThotBool TtaFileCopyUncompress (CONST char *sourceFile,
				       CONST char *targetFile);

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
extern gzFile TtaGZOpen (CONST char *filename);
extern void TtaGZClose (gzFile file);
#endif /* NODISPLAY */
extern BinFile TtaReadOpen (CONST char *filename);
extern void TtaReadClose ( BinFile file );
extern BinFile TtaWriteOpen (CONST char *filename);
extern BinFile TtaRWOpen (CONST char *filename);
extern void TtaWriteClose ( BinFile file );
extern ThotBool TtaWriteByte ( BinFile file,
                              char bval );
extern ThotBool TtaWriteWideChar (BinFile file, CHAR_T bval);
extern ThotBool TtaWriteShort ( BinFile file,
                               int sval );
extern ThotBool TtaWriteInteger ( BinFile file,
                                 int lval );
extern ThotBool TtaCompareFiles ( CONST char *file1,
                                 CONST char *file2 );
extern ThotBool TtaMakeDirectory ( char *directory );
extern ThotBool TtaCheckDirectory ( char *directory );

#endif /* __CEXTRACT__ */
#endif /* _FILEACCESS_H_ */

