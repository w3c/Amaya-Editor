/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* author:
 * - Ramzi GUETARI (W3C/INRIA)
 *
 * Last modification: Jan 23 1997
 */

#ifndef PLUGIN_BROWSE_H
#define PLUGIN_BROWSE_H

#include "thot_gui.h"
#include "thot_sys.h"
#include "thotdir.h"
#include "dialog.h"
#include "message.h"
#include "application.h"

#include "npapi.h"
#include "npupp.h"

#define MAX_NAME	 80
#define SELECTOR_NB_ITEMS 5

typedef struct _PluginInfo {
        char*            pluginDL ;       /* Location of the dynamic library of the plugin */
        char*            pluginMimeType ; /* Mime type of the plugin                       */
        char*            pluginID ;       /* Plugin identifier (if mentionned)             */
        char*            pluginURL ;      /* URL of the plugin                             */
        char*            fileExt ;        /* Suffix of files supported by the plugin       */
#ifdef _WINDOWS
        HANDLE           pluginHandle ;
#else  /* _WINDOWS */
        void*            pluginHandle ;
#endif /* _WINDOWS */
        NPPluginFuncs*   pluginFunctionsTable;
} PluginInfo ;

#endif /* PLUGIN_BROWSE_H */





