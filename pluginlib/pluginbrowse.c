/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* author:
 * - Ramzi GUETARI (W3C/INRIA)
 *
 * Last modification: March 28 1997
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
#include "pluginhandler_f.h"

#ifdef _WINDOWS
#define PSuffix ".DLL"
#else  /* _WINDOWS */
#define PSuffix ".so"
#endif /* _WINDOWS */

#define NAME_LENGTH_	100
static char  ls_unixFiles[MAX_NAME * NAME_LENGTH_];
static int   ls_fileNbr;

PluginInfo* pluginTable [100];
int   pluginCounter = 0;

extern int              currentExtraHandler;
extern NPNetscapeFuncs* amayaFunctionsTable;
extern char*            amayaPluginDir;

/* -----------------------------------------------------------------------
   TtaBrowsePluginDirectory: Browses the directory containing the plug-ins
                       and initializes the river of each plugin encountred.
                       The plug-ins directory may be defined by the
                       environment variable AMAYA_PLUGIN_DIR or by default 
                       $(HOME)/.amaya/plugins is considered.
   ----------------------------------------------------------------------- */
#ifdef __STDC__
void TtaBrowsePluginDirectory (void)
#else  /* __STDC__ */
void TtaBrowsePluginDirectory ()
#endif /* __STDC__ */
{
   struct        stat;
   char          word[4 * NAME_LENGTH_];
   int           ls_currentfile;
   int           res ;
   ThotBool       stop;
   ThotDirBrowse thotDir;

   if ((amayaPluginDir == NULL) || (amayaPluginDir[0] == '\0')) {
       /* the envoronment variable AMAYA_PLUGIN_DIR is not positioned */
       /* One use the defauly directory $(HOME)/.amaya/plugins        */
      char* homeDir = TtaGetEnvString ("HOME") ;
      amayaPluginDir = (char*) malloc (strlen (homeDir) + 20) ;
      sprintf (amayaPluginDir, "%s/.amaya/plugins", homeDir) ;
   }

   if (TtaCheckDirectory (amayaPluginDir)) {
      Ap_InitializeAmayaTable ();
      ls_unixFiles[0] = '\0';
      ls_fileNbr = 0;
      ls_currentfile = 0;
      stop = FALSE;
      thotDir.buf = word;
      thotDir.bufLen = sizeof (word);
      thotDir.PicMask = ThotDirBrowse_FILES;
      if (ThotDirBrowse_first (&thotDir, amayaPluginDir, "*", PSuffix) == 1) {
	  /* Looking for all ".so" files for unix or ".DLL" for windows in the amayaPluginDir */
         do {
            pluginTable [pluginCounter] = (PluginInfo*) malloc (sizeof (PluginInfo)) ;
            pluginTable [pluginCounter]->pluginDL = (char*) malloc (strlen (thotDir.buf) + 1);
            pluginTable [pluginCounter]->pluginHandle = (void*) NULL;
            pluginTable [pluginCounter]->pluginFunctionsTable = (NPPluginFuncs*) NULL;
            strcpy (pluginTable [pluginCounter]->pluginDL, thotDir.buf);

            /* Initialize the plugin handler */
            res = InitPluginHandlers (FALSE, pluginCounter);
            if (res != -1)
               pluginCounter++;
         } while (ThotDirBrowse_next (&thotDir) == 1);
         ThotDirBrowse_close (&thotDir);
      }
      currentExtraHandler = pluginCounter - 1;
   }
}
