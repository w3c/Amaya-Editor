typedef struct { /* fill in with more elements as they are needed */
#ifdef WWW_MSWINDOWS
    FILETIME atime; /* don't know yet what format will be used most */
    DWORD size;
#else /* WWW_MSWINDOWS */
    time_t atime;
    off_t size;
#endif /* !WWW_MSWINDOWS */
} ThotFileInfo;
#ifdef WWW_MSWINDOWS
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
#else /* WWW_MSWINDOWS */
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
#endif /* !WWW_MSWINDOWS */
#ifdef INCLUDE_TESTING_CODE
void ThotFile_test(char * name);
#endif /* INCLUDE_TESTING_CODE */

#ifdef __STDC__
ThotFileHandle ThotFile_open(char * name, ThotFileMode mode);
int ThotFile_close(ThotFileHandle handle);
int ThotFile_read(ThotFileHandle handle, void * buffer, unsigned int count);
int ThotFile_write(ThotFileHandle handle, void * buffer, unsigned int count);
ThotFileOffset ThotFile_seek(ThotFileHandle handle, ThotFileOffset offset, ThotFileOrigin origin);
int ThotFile_stat(ThotFileHandle handle, ThotFileInfo * pInfo);
#else /* __STDC__ */
ThotFileHandle ThotFile_open(/* char * name, ThotFileMode mode */);
int ThotFile_close(/* ThotFileHandle handle */);
int ThotFile_read(/* ThotFileHandle handle, void * buffer, unsigned int count */);
int ThotFile_write(/* ThotFileHandle handle, void * buffer, unsigned int count */);
ThotFileOffset ThotFile_seek(/* ThotFileHandle handle, ThotFileOffset offset, ThotFileOrigin origin */);
int ThotFile_stat(/* ThotFileHandle handle, ThotFileInfo * pInfo */);
#endif /* !__STDC__ */

