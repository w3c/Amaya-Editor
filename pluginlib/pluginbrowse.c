/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* author:
 * - Ramzi GUETARI
 *
 * Last modification: Jan 23 1997
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "npapi.h"
#include "npupp.h"
#include "picture.h"

#include "pluginbrowse.h"
#include "pluginbrowse_f.h"
#include "pluginapi_f.h"

static char  ls_unixFiles[MAX_NAME * NAME_LENGTH];
static int   ls_fileNbr;

PluginInfo* pluginTable [100];

char* homeDir ;
char* amayaPluginDir ;

int   pluginCounter = 0;

extern int currentExtraHandler;
extern NPNetscapeFuncs* amayaFunctionsTable;

#ifdef __STDC__
void TtaBrowsePluginDirectory (void)
#else  /* __STDC__ */
void TtaBrowsePluginDirectory ()
#endif /* __STDC__ */
{
   struct        stat;
   char          word[4 * NAME_LENGTH];
   int           ls_currentfile;
   boolean       stop;
   ThotDirBrowse thotDir;

   homeDir = getenv ("HOME") ;
   amayaPluginDir = TtaGetMemory (strlen (homeDir) + 16) ;
   sprintf (amayaPluginDir, "%s/.amaya/plugins", homeDir);

   if (TtaCheckDirectory (amayaPluginDir)) {
      Ap_InitializeAmayaTable ();
      ls_unixFiles[0] = '\0';
      ls_fileNbr = 0;
      ls_currentfile = 0;
      stop = FALSE;
      thotDir.buf = word;
      thotDir.bufLen = sizeof (word);
      thotDir.PicMask = ThotDirBrowse_FILES;
      if (ThotDirBrowse_first (&thotDir, amayaPluginDir, "*", ".so") == 1) {
         do {
            pluginTable [pluginCounter] = (PluginInfo*) malloc (sizeof (PluginInfo)) ;
            pluginTable [pluginCounter]->pluginDL = (char*) malloc (strlen (thotDir.buf) + 1);
            pluginTable [pluginCounter]->pluginHandle = (void*) NULL;
            pluginTable [pluginCounter]->pluginFunctionsTable = (NPPluginFuncs*) NULL;
            strcpy (pluginTable [pluginCounter]->pluginDL, thotDir.buf);
            InitPluginHandlers (FALSE, pluginCounter);
            pluginCounter++;
         } while (ThotDirBrowse_next (&thotDir) == 1);
         ThotDirBrowse_close (&thotDir);
      }
      currentExtraHandler = pluginCounter - 1;
   }
}
