/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _THOT_DIR_H__
#define _THOT_DIR_H__

typedef enum {
  ThotDirBrowse_FILES = 1, 
  ThotDirBrowse_DIRECTORIES = 2
} ThotDirBrowse_mask;

typedef struct {
#   if defined(_WINDOWS) && !defined(__GNUC__)
    WIN32_FIND_DATA    data;
    HANDLE	           handle;
#   else /* WWW_MSWINDOWS */
    FILE*              ls_stream;
#endif /* !WWW_MSWINDOWS */
    ThotDirBrowse_mask PicMask;
    CHAR_T*            buf;
    size_t             bufLen;
    int                dirLen;
} ThotDirBrowse;


int ThotDirBrowse_close(ThotDirBrowse * me);
int ThotDirBrowse_next(ThotDirBrowse * me);
int ThotDirBrowse_first(ThotDirBrowse * me, CHAR_T* dir, CHAR_T* name, CHAR_T* ext);

#endif /* _THOT_DIR_H__ */
