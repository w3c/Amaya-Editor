#ifndef AHTEVNTRG_H
#define AHTEVNTRG_H

#include "sysdep.h"

/*

  Windows Specific Handles
  
 */

extern LRESULT CALLBACK AsyncWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam
);

bool AHTEventInit (void);
bool AHTEventTerminate (void);

#endif /* AHTEVENTRG_H */








