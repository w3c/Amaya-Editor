/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

#ifndef _FILEACCESS_H_
#define _FILEACCESS_H_

#include "thot_sys.h"
#include "typebase.h"

typedef FILE *BinFile;

typedef struct { /* fill in with more elements as they are needed */
#if defined(_WINDOWS) && !defined(__GNUC__)
    FILETIME atime; /* don't know yet what format will be used most */
    DWORD size;
#else /* _WINDOWS */
    time_t atime;
    off_t size;
#endif /* !_WINDOWS */
} ThotFileInfo;

#if defined(_WINDOWS) && !defined(__GNUC__)

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

#else /* ! _WINDOWS */

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

#endif /* !_WINDOWS */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern int TtaFileExist ( CONST CHAR_T* filename );
extern int TtaFileUnlink ( CONST CHAR_T* filename );
extern int TtaFileClose ( ThotFileHandle handle );
extern int TtaFileClose ( ThotFileHandle handle );
extern int TtaFileRead ( ThotFileHandle handle,
                         void *buffer,
                         unsigned int count );
extern int TtaFileWrite ( ThotFileHandle handle,
                          void *buffer,
                          unsigned int count );
extern ThotFileOffset TtaFileSeek ( ThotFileHandle handle,
                                    ThotFileOffset offset,
                                    ThotFileOrigin origin );
extern int TtaFileStat ( ThotFileHandle handle,
                         ThotFileInfo * pInfo );
extern void TtaFileCopy ( CONST CHAR_T* sourceFileName,
                          CONST CHAR_T* targetFileName );

extern ThotBool TtaReadByte ( BinFile file,
                             char* bval );
extern ThotBool TtaReadWideChar (BinFile file, CHAR_T* bval, CHARSET);

extern ThotBool TtaReadBool ( BinFile file,
                             ThotBool * bval );
extern ThotBool TtaReadShort ( BinFile file,
                              int *sval );
extern ThotBool TtaReadSignedShort ( BinFile file,
                                    int *sval );
extern ThotBool TtaReadInteger ( BinFile file,
                                int *sval );
extern ThotBool TtaReadName ( BinFile file,
                             CHAR_T* name );
extern BinFile TtaReadOpen (CONST CHAR_T* filename);
extern void TtaReadClose ( BinFile file );
extern void TtaWriteClose ( BinFile file );
extern ThotBool TtaWriteByte ( BinFile file,
                              char bval );
extern ThotBool TtaWriteWideChar (BinFile, CHAR_T, CHARSET);
extern ThotBool TtaWriteShort ( BinFile file,
                               int sval );
extern ThotBool TtaWriteInteger ( BinFile file,
                                 int lval );
extern ThotBool TtaCompareFiles ( CONST STRING file1,
                                 CONST STRING file2 );
extern ThotBool TtaMakeDirectory ( CHAR_T* directory );
extern ThotBool TtaCheckDirectory ( CHAR_T* directory );

#else /* __STDC__ */

extern int TtaFileExist (/* CONST CHAR_T* filename */);
extern int TtaFileUnlink (/* CONST CHAR_T *filename */);
extern ThotFileHandle TtaFileOpen (/* CONST char *name,
                                      ThotFileMode mode */);
extern int TtaFileClose (/* ThotFileHandle handle */);
extern int TtaFileRead (/* ThotFileHandle handle,
                           void *buffer,
                           unsigned int count */);
extern int TtaFileWrite (/* ThotFileHandle handle,
                            void *buffer,
                            unsigned int count */);
extern ThotFileOffset TtaFileSeek (/* ThotFileHandle handle,
                                      ThotFileOffset offset,
                                      ThotFileOrigin origin */);
extern int TtaFileStat (/* ThotFileHandle handle,
                           ThotFileInfo * pInfo */);
extern void TtaFileCopy (/* CONST CHAR_T* sourceFileName,
                            CONST CHAR_T* targetFileName */);

extern ThotBool TtaReadByte (/* BinFile file,
                               char *bval */);
extern ThotBool TtaReadWideChar (/* BinFile file, CHAR_T* bval */);
extern ThotBool TtaReadBool (/* BinFile file,
                               ThotBool * bval */);
extern ThotBool TtaReadShort (/* BinFile file,
                                int *sval */);
extern ThotBool TtaReadSignedShort (/* BinFile file,
                                      int *sval */);
extern ThotBool TtaReadInteger (/* BinFile file,
                                  int *sval */);
extern ThotBool TtaReadName (/* BinFile file,
                               CHAR_T *name */);
extern BinFile TtaReadOpen (/* CONST CHAR_T* filename */);
extern void TtaReadClose (/* BinFile file */);
extern void TtaWriteClose (/* BinFile file */);
extern ThotBool TtaWriteByte (/* BinFile file,
                                char bval */);
extern ThotBool TtaWriteWideChar (/* BinFile, CHAR_T, CHARSET */);
extern ThotBool TtaWriteShort (/* BinFile file,
                                 int sval */);
extern ThotBool TtaWriteInteger (/* BinFile file,
                                   int lval */);
extern ThotBool TtaCompareFiles (/* CONST char *file1,
                                   CONST char *file2 */);
extern ThotBool TtaMakeDirectory (/* CHAR_T* directory */);
extern ThotBool TtaCheckDirectory (/* CHAR_T* directory */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
#endif /* _FILEACCESS_H_ */
