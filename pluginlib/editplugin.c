/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: Ramzi Guetari (W3C/INRIA)
 *
 */

#include "thot_sys.h"
#include "thot_gui.h"
#include "pluginbrowse.h"
#include "dialog.h"
#include "view.h"
#include "document.h"
#include "app.h"

#define PLUGIN_MAX_REF	   4

#define PLUGIN_FORM        1   /* For plugin's selection */
#define PLUGIN_SELECTOR    2   /* Plugin selector */
#define PLUGIN_INFO        3   /* Displaying plugin's information */

char*                 pluginPath;

static int            BasePlugin = 0;
static char           buffer [800]; /* A REVOIR $$$$$$$$$$$$$ */

extern PluginInfo*    pluginTable [100];
extern int            pluginCounter ;
extern int            currentExtraHandler;

#ifdef __STDC__
void CallbackFormPlugin (int ref, int dataType, char* data)
#else  /* __STDC__ */
void CallbackFormPlugin (ref, dataType, data)
int   ref; 
int   dataType; 
char* data;
#endif /* __STDC__ */
{
   int index ;
   switch (ref - BasePlugin) {
   case PLUGIN_SELECTOR: pluginPath = (char*) malloc (strlen (data) + 1) ;
                         strcpy (pluginPath, data) ;
                         for (index = 0; index < pluginCounter; index++) 
                             if (!strcmp (pluginPath, pluginTable [index]->pluginID))
                                currentExtraHandler = index ;
                         sprintf (buffer, "Mime type: %s", pluginTable [currentExtraHandler]->pluginMimeType);
                         TtaNewLabel (BasePlugin + PLUGIN_INFO, BasePlugin + PLUGIN_FORM, buffer);
                         break;
   case PLUGIN_FORM:     /*if ((int) data == 0)
                            currentExtraHandler = -1; */ /* Cancel */
                         /*else { */                /* Confirm */
			     /* InitPluginHandlers (FALSE); */
			     /* charger la page HTML du plugin */
                             /* sprintf (buffer, "Mime type: %s", pluginTable [currentExtraHandler]->pluginDL);
                              TtaNewLabel (BasePlugin + PLUGIN_INFO, BasePlugin + PLUGIN_FORM, buffer);
                         }*/
                         break;
   default:              break;                         
   }
}

#ifdef __STDC__ 
void TtaCreateFormPlugin (Document document, View view)
#else  /* __STDC__ */
void TtaCreateFormPlugin (document, view) 
Document document; 
View     view;
#endif /* __STDC__ */
{
   int i;
   char* pBuf;
   if (BasePlugin == 0) {
      BasePlugin = TtaSetCallback ((Proc) CallbackFormPlugin, PLUGIN_MAX_REF);
   }
   TtaNewForm (BasePlugin + PLUGIN_FORM, TtaGetViewFrame (document, view),  "Plugin Selector", TRUE, 1, 'L', D_CANCEL);
   pBuf = buffer;
   for (i = 0; i < pluginCounter; i++) {
       strcpy (pBuf, pluginTable [i]->pluginID);
       pBuf += strlen (pBuf) + 1;
   }

   TtaNewSelector (BasePlugin + PLUGIN_SELECTOR, BasePlugin + PLUGIN_FORM, "Plugin List", pluginCounter, buffer, 10, NULL, FALSE, TRUE);
   TtaSetSelector (BasePlugin + PLUGIN_SELECTOR, 0, ""); /* Select the first item */
   if (pluginCounter > 0)
      sprintf (buffer, "Mime type: %s", pluginTable[0]->pluginMimeType);
   else
       buffer[0] = '\0';
   /*    currentExtraHandler = 0;
   sprintf (buffer, "Mime type: %s", pluginTable [currentExtraHandler]->pluginDL);
   pluginPath = (char*) malloc (strlen (pluginTable [currentExtraHandler]->pluginDL) + 1);
   strcpy (pluginPath, pluginTable [currentExtraHandler]->pluginDL);*/
   TtaNewLabel (BasePlugin + PLUGIN_INFO, BasePlugin + PLUGIN_FORM, buffer);
   TtaShowDialogue (BasePlugin + PLUGIN_FORM, TRUE); 
}
