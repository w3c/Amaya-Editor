/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* authors (alphabetical order):
 * - Ramzi GUETARI
 * - Nabil LAYAIDA
 * - Loay  SABRY-ISMAIL
 *
 * Last modification: Jan 09 1997
 */

/*----------------------------------------------------------------------
  This module have to implement Amaya plug-in functions.
  ----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>


#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "interface.h"
#include "view.h"

#include "npapi.h"
#include "npupp.h"
#include "pluginbrowse.h"

#define EXPORT extern
#include "picture_tv.h"

extern XtAppContext     app_cont;
extern PluginInfo*      pluginTable [100];
extern Document         currentDocument;
extern int              currentPlugin;
extern PictureHandler   PictureHandlerTable[MAX_PICT_FORMATS];

static void*            handle = NULL;
static NPP              pluginInstance;
static NPSavedData*     saved = NULL;
static NPNetscapeFuncs* amayaFunctionsTable;
/* static NPPluginFuncs*   pluginFunctionsTable; */
static NPMIMEType       pluginType;

static int (*ptr_NPP_GetMIMEDescription) () ;
static int (*ptr_NP_Initialize)          (NPNetscapeFuncs*, NPPluginFuncs*) ;

#ifndef EOS
#define EOS '\0'
#endif
#ifndef SPACE
#define SPACE ' '
#endif
#ifndef TAB
#define TAB '\t'
#endif

/*----------------------------------------------------------------------
   Functions Supplied by the Amaya side                                  
  ----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  ParseMIMEType: parses the Mime type of the plugin in order to identify
                 the files which may be needed by it.
	 ** pluginMimeType: input variable representing the mime type
            of the plugin
	 ** fileSuffix: output variable represnting all file suffixes
            supported by the plugin.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void ParseMIMEType (const int indexHandler, const char* pluginMimeType)
#else  /* __STDC__ */
static void ParseMIMEType (indexHandler, pluginMimeType)
const int   indexHandler;
const char* pluginMimeType;
#endif /* __STDC__ */
{
   int  index     = 0 ;
   int  suffixNdx = 0;
   int  ndx       ;
   int  endOfSuffixes;
   char token [800];
   char suffixes [800];
   
   if (!pluginMimeType || pluginMimeType[0] ==  EOS)
      pluginTable [indexHandler]->fileExt = (char*) 0;
   
   while (pluginMimeType [index] == SPACE || pluginMimeType [index] == TAB)
         index++;
   if (pluginMimeType [index] == EOS) 
      pluginTable [indexHandler]->fileExt = (char*) 0;
   ndx = 0;
   while (isalpha (pluginMimeType [index]))
         token [ndx++] = pluginMimeType [index++];
   token [ndx] = EOS;
   if (strcmp (token, "Mime"))
      pluginTable [indexHandler]->fileExt = (char*) 0;
   while (pluginMimeType [index] == SPACE || pluginMimeType [index] == TAB)
         index++;
   if (pluginMimeType [index] == EOS) 
      pluginTable [indexHandler]->fileExt = (char*) 0;
   if (pluginMimeType [index] != ':')
      pluginTable [indexHandler]->fileExt = (char*) 0;
   index++;
   while (pluginMimeType [index] != EOS) {
         endOfSuffixes = FALSE;
         ndx = 0;
         while (pluginMimeType [index] == SPACE || pluginMimeType [index] == TAB)
               index++;
         while (pluginMimeType [index] != EOS && pluginMimeType [index] != ':')
               token [ndx++] = pluginMimeType [index++];
         token [ndx] = EOS;
         if (pluginMimeType [index] == EOS) {
	    if (token) {
               printf ("Plugin name: %s\n", token) ;
               pluginTable [indexHandler]->pluginID = (char*) malloc (strlen (token) + 1);
               strcpy (pluginTable [indexHandler]->pluginID, token);
            } else {
                   printf ("Plugin name: Unknown plugin\n") ;              
                   pluginTable [indexHandler]->pluginID = (char*) malloc (14);
                   strcpy (pluginTable [indexHandler]->pluginID, "Unknown Plugin");
            }
         } else {
	      printf ("Plugin type: %s\n", token) ;
              if (pluginMimeType [index] != ':')
                 printf ("bad mime type\n") ;
              else 
                  index ++;
              ndx = 0;
              while (!endOfSuffixes) {
                    while (pluginMimeType [index] == SPACE || pluginMimeType [index] == TAB)
		          index++;
                    if (pluginMimeType [index] == ':' || pluginMimeType [index] == ';') {
                       token [ndx] = EOS;
                       endOfSuffixes = TRUE ;
                       index++;
                    } else {
                            if (pluginType [index] == '.') index++ ;
                            while (isalnum (pluginMimeType [index]))
                                  token [ndx++] = suffixes [suffixNdx++] = pluginMimeType [index++];
	            }
                    if (pluginMimeType [index] == ',')
                       token [ndx++] = suffixes [suffixNdx++] = pluginMimeType [index++];
              }
              printf ("suffixes: %s\n", token);
              ndx = 0;
              while (pluginMimeType [index] != EOS && pluginMimeType [index] != ';' && pluginMimeType [index] != ':')
                    token [ndx++] = pluginMimeType [index++];
              token [ndx] = EOS;
              if (pluginMimeType [index] == ';') {
                 suffixes [suffixNdx++] = ',' ;
                 index++;
              }
              if (pluginMimeType [index] == ':')
                 index++;
              printf ("comment: %s\n", token);
         }
   }
   suffixes [suffixNdx] = EOS;
   printf ("all suffixes: %s\n", suffixes) ;
   pluginTable [indexHandler]->fileExt = (char*) malloc (strlen (suffixes) + 1) ;
   strcpy (pluginTable [indexHandler]->fileExt, suffixes);
}

/*----------------------------------------------------------------------
  Ap_DestroyStream
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_DestroyStream (NPP instance, NPStream* stream, NPError reason)
#else  /* __STDC__ */
NPError Ap_DestroyStream (instance, stream, reason)
NPP       instance ;
NPStream* stream ;
NPError   reason ;
#endif /* __STDC__ */
{
    if (instance == NULL)
       return NPERR_INVALID_INSTANCE_ERROR;

    printf ("Destroy Stream\n");
    return NPERR_NO_ERROR;
}

/*----------------------------------------------------------------------
  Ap_GetJavaEnv
  ----------------------------------------------------------------------*/
#ifdef __STDC__
JRIEnv*  Ap_GetJavaEnv (void)
#else  /* __STDC__ */
JRIEnv*  Ap_GetJavaEnv ()
#endif /* __STDC__ */
{
    printf ("Get Java Environment \n");
    return (NULL);
}

/*----------------------------------------------------------------------
  Ap_GetJavaEPeer
  ----------------------------------------------------------------------*/
#ifdef __STDC__
jref Ap_GetJavaPeer (NPP instance)
#else  /* __STDC__ */
jref Ap_GetJavaPeer (instance)
NPP instance ;
#endif /* __STDC__ */
{
    printf ("Get Java Peer \n");
    return (NULL);
}

/*----------------------------------------------------------------------
  Ap_GetURL
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_GetURL (NPP instance, const char* url, const char* window)
#else  /* __STDC__ */
NPError Ap_GetURL (instance, url, window)
NPP         instance ;
const char* url ;
const char* window ;
#endif /* __STDC__ */
{
   /*NPStream * stream;*/

   printf ("GetURL  %s \n", url);
    
   if (window) {
      /* pass the stream to AMAYA */
      printf ("AM_geturl: Passing the stream to AMAYA\n");
   } else { /* window = NULL */
	  /* pass the stream to the plug-in */
	  printf ("AM_geturl: Passing stream to the plug-in\n");

	  /*NPP_NewStream();*/
	  /* NPP_WriteReady(); */
	  /* NPP_Write(); */  /* Until the stream ends */
	  /* NPP_DestroyStream(); */
   }
   return NPERR_NO_ERROR;
}

/*----------------------------------------------------------------------
  Ap_MemAlloc
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void* Ap_MemAlloc (uint32 size)
#else  /* __STDC__ */
void* Ap_MemAlloc (size)
uint32 size ;
#endif /* __STDC__ */
{
    printf ("MemAlloc %d\n", (int) size);
    return malloc (size);
}

/*----------------------------------------------------------------------
  Ap_MemFlush
  ----------------------------------------------------------------------*/
#ifdef __STDC__
uint32  Ap_MemFlush (uint32 size)
#else  /* __STDC__ */
uint32  Ap_MemFlush (size)
uint32 size ;
#endif /* __STDC__ */
{
    printf ("MemFlush \n");
    return (0);
}

/*----------------------------------------------------------------------
  Ap_MemFree
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  Ap_MemFree (void* ptr)
#else  /* __STDC__ */
void  Ap_MemFree (ptr)
void* ptr ;
#endif /* __STDC__ */
{
    printf ("MemFree \n");
    free (ptr);
}

/*----------------------------------------------------------------------
  Ap_NewStream
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_NewStream (NPP instance, NPMIMEType type, const char* window, NPStream** stream_ptr)
#else  /* __STDC__ */
NPError Ap_NewStream (NPP instance, NPMIMEType type, const char* window, NPStream** stream_ptr)
#endif /* __STDC__ */
{
    printf ("NewStream \n");

    (*stream_ptr) = (NPStream*) malloc (sizeof (NPStream));
    
    return NPERR_NO_ERROR;
}

/*----------------------------------------------------------------------
  Ap_PostURL
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_PostURL (NPP instance, const char* url, const char* window, uint32 len, const char* buf, NPBool file)
#else  /* __STDC__ */
NPError Ap_PostURL (instance, url, window, len, buf, file)
NPP         instance ;
const char* url ;
const char* window ;
uint32      len ;
const char* buf ;
NPBool      file ;
#endif /* __STDC__ */
{
   printf ("PostURL  \n");
   return NPERR_NO_ERROR;
}

/*----------------------------------------------------------------------
  Ap_RequestRead
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_RequestRead (NPStream* stream, NPByteRange* rangeList)
#else  /* __STDC__ */
NPError Ap_RequestRead (stream, rangeList)
NPStream*    stream ;
NPByteRange* rangeList ;
#endif /* __STDC__ */
{
    FILE        *fptr;
    /*
    char        *buffer;
    int         count = 0, ret = 0, ready_to_read, size;
    long        offset;

    printf ("RequestRead  \n");
    
    fptr = fopen ("/tahiti/guetari/example/nasa.cgm","rb");

    offset = 0;
    fseek (fptr, offset, SEEK_SET);
     
    while (!feof (fptr)){
	ready_to_read = (*(pluginFunctionsTable->writeready)) (instance, stream);
	size = ready_to_read > rangeList->length ? rangeList->length : ready_to_read;
	buffer = (char*) malloc (size);
	count = fread (buffer, sizeof (char), size, fptr);

	ret = (*(pluginFunctionsTable->write)) (instance, stream, offset , count, buffer);
	
	printf ("%d WriteReady \n", ready_to_read);
	printf ("\t%d bytes consumed by NPP_Write\n", ret);
	offset += count;
	free (buffer);
    }
    */
    fclose (fptr);
    return NPERR_NO_ERROR;
}

/*----------------------------------------------------------------------
  Ap_Status
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_Status (NPP instance, const char* message)
#else  /* __STDC__ */
void Ap_Status (instance, message)
NPP         instance ; 
const char* message ;
#endif /* __STDC__ */
{
    printf ("Status %s\n", message);
}

/*----------------------------------------------------------------------
  Ap_UserAgent
  ----------------------------------------------------------------------*/
#ifdef __STDC__
const char* Ap_UserAgent (NPP instance)
#else  /* __STDC__ */
const char* Ap_UserAgent (instance)
NPP instance ;
#endif /* __STDC__ */
{
    printf ("User Agent \n");
    return ("Amaya");
}

/*----------------------------------------------------------------------
  Ap_Write
  ----------------------------------------------------------------------*/
#ifdef __STDC__ 
int32 Ap_Write (NPP instance, NPStream* stream, int32 len, void* buffer)
#else  /* __STDC__ */
int32 Ap_Write (instance, stream, len, buffer)
NPP       instance ;
NPStream* stream ;
int32     len ;
void*     buffer ;
#endif /* __STDC__ */
{
    printf ("Write \n");
    return 4;
}

/*----------------------------------------------------------------------
  Ap_Version
  ----------------------------------------------------------------------*/
#ifdef __STDC__ 
void Ap_Version (int* plugin_major, int* plugin_minor, int* amaya_major, int* amaya_minor)
#else  /* __STDC__ */
void Ap_Version (plugin_major, plugin_minor, amaya_major, amaya_minor)
int* plugin_major ; 
int* plugin_minor ; 
int* amaya_major  ; 
int* amaya_minor  ;
#endif /* __STDC__ */
{
    printf ("Version \n") ;
    *plugin_major = NP_VERSION_MAJOR;
    *plugin_minor = NP_VERSION_MINOR;
}

/*----------------------------------------------------------------------
  Ap_ReloadPlugins
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_ReloadPlugins (NPBool reloadPages)
#else  /* __STDC__ */
void Ap_ReloadPlugins (reloadPages)
NPBool reloadPages ;
#endif /* __STDC__ */
{
    printf ("Reload Plugins \n");
}

/*----------------------------------------------------------------------
  AM_getvalue
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_GetValue (NPP instance, NPNVariable variable, void* r_value)
#else  /* __STDC__ */
NPError Ap_GetValue (instance, variable, r_value)
NPP         instance ; 
NPNVariable variable ; 
void*       r_value ;
#endif /* __STDC__ */
{
    NPError error = NPERR_NO_ERROR;
    printf ("Get Value \n");
    if (!instance) error = NPERR_INVALID_INSTANCE_ERROR;
    else {
         switch (variable) {
         case NPNVxtAppContext: *((char**) r_value) = (char*) app_cont;
                                break;
         case NPNVxDisplay:     /* ASSIGN r_value TO A VALUE HERE */
                                break;
         default:               error = NPERR_GENERIC_ERROR;
                                break;
         }
    }
    return error;
}


/*----------------------------------------------------------------------
  InitializeTable
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void Ap_InitializeAmayaTable (void)
#else  /* __STDC__ */
void Ap_InitializeAmayaTable ()
#endif /* __STDC__ */
{
    amayaFunctionsTable  = (NPNetscapeFuncs*) malloc (sizeof (NPNetscapeFuncs));
    printf ("Size of NPAmayaFuncs = %d\n", (int) sizeof (NPNetscapeFuncs));

    amayaFunctionsTable->version       = ((uint16) (0));
    amayaFunctionsTable->size          = ((uint16) (sizeof       (NPNetscapeFuncs)));
    amayaFunctionsTable->posturl       = ((NPN_PostURLUPP)       (Ap_PostURL));
    amayaFunctionsTable->geturl        = ((NPN_GetURLUPP)        (Ap_GetURL));
    amayaFunctionsTable->requestread   = ((NPN_RequestReadUPP)   (Ap_RequestRead));
    amayaFunctionsTable->newstream     = ((NPN_NewStreamUPP)     (Ap_NewStream));
    amayaFunctionsTable->write         = ((NPN_WriteUPP)         (Ap_Write));
    amayaFunctionsTable->destroystream = ((NPN_DestroyStreamUPP) (Ap_DestroyStream));
    amayaFunctionsTable->status        = ((NPN_StatusUPP)        (Ap_Status));
    amayaFunctionsTable->uagent        = ((NPN_UserAgentUPP)     (Ap_UserAgent));
    amayaFunctionsTable->memalloc      = ((NPN_MemAllocUPP)      (Ap_MemAlloc));
    amayaFunctionsTable->memfree       = ((NPN_MemFreeUPP)       (Ap_MemFree));
    amayaFunctionsTable->memflush      = ((NPN_MemFlushUPP)      (Ap_MemFlush));
    amayaFunctionsTable->reloadplugins = ((NPN_ReloadPluginsUPP) (Ap_ReloadPlugins));
    amayaFunctionsTable->getJavaEnv    = ((NPN_GetJavaEnvUPP)    (Ap_GetJavaEnv));
    amayaFunctionsTable->getJavaPeer   = ((NPN_GetJavaPeerUPP)   (Ap_GetJavaPeer));
    amayaFunctionsTable->getvalue      = ((NPN_GetValueUPP)      (Ap_GetValue));
}

#ifdef __STDC__
void Ap_InitializePluginTable (void)
#else  /* __STDC__ */
void Ap_InitializePluginTable ()
#endif /* __STDC__ */
{
    char* message ;
    int   ret ;


    /*    amayaFunctionsTable  = (NPNetscapeFuncs*) malloc (sizeof (NPNetscapeFuncs)); */
    pluginTable [currentPlugin]->pluginFunctionsTable = (NPPluginFuncs*) malloc (sizeof (NPPluginFuncs));
    /* printf ("Size of NPNetscapeFuncs = %d\n", (int) sizeof (NPNetscapeFuncs)); */
    printf ("Size of NPPluginFuncs = %d\n", (int) sizeof (NPPluginFuncs));
    /*
    amayaFunctionsTable->version       = ((uint16) (0));
    amayaFunctionsTable->size          = ((uint16) (sizeof       (NPNetscapeFuncs)));
    amayaFunctionsTable->posturl       = ((NPN_PostURLUPP)       (Ap_PostURL));
    amayaFunctionsTable->geturl        = ((NPN_GetURLUPP)        (Ap_GetURL));
    amayaFunctionsTable->requestread   = ((NPN_RequestReadUPP)   (Ap_RequestRead));
    amayaFunctionsTable->newstream     = ((NPN_NewStreamUPP)     (Ap_NewStream));
    amayaFunctionsTable->write         = ((NPN_WriteUPP)         (Ap_Write));
    amayaFunctionsTable->destroystream = ((NPN_DestroyStreamUPP) (Ap_DestroyStream));
    amayaFunctionsTable->status        = ((NPN_StatusUPP)        (Ap_Status));
    amayaFunctionsTable->uagent        = ((NPN_UserAgentUPP)     (Ap_UserAgent));
    amayaFunctionsTable->memalloc      = ((NPN_MemAllocUPP)      (Ap_MemAlloc));
    amayaFunctionsTable->memfree       = ((NPN_MemFreeUPP)       (Ap_MemFree));
    amayaFunctionsTable->memflush      = ((NPN_MemFlushUPP)      (Ap_MemFlush));
    amayaFunctionsTable->reloadplugins = ((NPN_ReloadPluginsUPP) (Ap_ReloadPlugins));
    amayaFunctionsTable->getJavaEnv    = ((NPN_GetJavaEnvUPP)    (Ap_GetJavaEnv));
    amayaFunctionsTable->getJavaPeer   = ((NPN_GetJavaPeerUPP)   (Ap_GetJavaPeer));
    amayaFunctionsTable->getvalue      = ((NPN_GetValueUPP)      (Ap_GetValue));
    */
    pluginTable [currentPlugin]->pluginFunctionsTable->size         = ((uint16) (sizeof (NPPluginFuncs)));

    ptr_NP_Initialize = (int (*) (NPNetscapeFuncs*, NPPluginFuncs*)) dlsym (handle, "NP_Initialize");
    message = (char*) dlerror ();    
    if (message) 
	printf ("ERROR at Initialization: %s\n", message);

    ret = ptr_NP_Initialize (amayaFunctionsTable, pluginTable [currentPlugin]->pluginFunctionsTable);
    printf ("result = %d\n", ret); 
}


/*----------------------------------------------------------------------
  InitializePlugin
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_InitializePlugin (char* path, int indexHandler)
#else  /* __STDC__ */
void Ap_InitializePlugin (path, indexHandler)
char* path ;
int   indexHandler;
#endif /* __STDC__ */
{
    /* Open the library and get the symbols addresses */
    char* message ;
    char GUI_Name [20];
    int index1 = 0 ;
    int index2 = 0;
    handle = dlopen (path, RTLD_NOW);
    
    /* strcpy(message, dlerror());*/
 
    message = (char*) dlerror ();
    
    if (message) {
	printf ("dlerror message: %s\n", message);
	exit (0);
    }

    /* get the symbols from the dynamic library */
    ptr_NPP_GetMIMEDescription = (int(*) ()) dlsym (handle, "NPP_GetMIMEDescription");
    pluginType = (NPMIMEType) (*ptr_NPP_GetMIMEDescription) ();
    pluginTable [indexHandler]->pluginType = (char*) malloc (strlen (pluginType) + 1) ;
    strcpy (pluginTable [indexHandler]->pluginType, pluginType);
    printf ("Mime: %s\n", pluginType);
    ParseMIMEType (indexHandler, pluginType);
    while (pluginTable [indexHandler]->fileExt [index1] != ',' && pluginTable [indexHandler]->fileExt [index1] != '\0')
          GUI_Name [index1] = pluginTable [indexHandler]->fileExt [index1++] ;
    GUI_Name [index1++] = ' ';
    GUI_Name [index1++] = '(';
    GUI_Name [index1++] = '.';
    while (pluginTable [indexHandler]->fileExt [index2] != ',' && pluginTable [indexHandler]->fileExt [index2] != '\0')
          GUI_Name [index1++] = pluginTable [indexHandler]->fileExt [index2++] ;
    GUI_Name [index1++] = ')' ;
    GUI_Name [index1] = '\0';

    printf (GUI_Name);
    
    strncpy (PictureHandlerTable[indexHandler].GUI_Name, GUI_Name, MAX_FORMAT_NAMELENGHT);
    /* Initializing the pointers to the netscape functions */
    Ap_InitializePluginTable ();

}

/*----------------------------------------------------------------------
  CreateInstance
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_CreatePluginInstance (Window window, Display* display, char* filename) 
#else  /* __STDC__ */
void Ap_CreatePluginInstance (window, display, filename) 
Window   window;
Display* display; 
char*    filename;
#endif /* __STDC__ */
{
    NPStream*   stream;
    NPByteRange range;
    NPWindow*   pwindow;
    uint16      stype;
    int         ret;
    
    char* argn[3] = {"SRC", "WIDTH", "HEIGHT"}; 
    char* argv[3] = { filename, "400", "400"};
    int16 argc    = 3;
    
    /* Prepare window information and "instance" structure */

    pwindow                  = (NPWindow*) malloc (sizeof (NPWindow));
    pwindow->x               = 0;
    pwindow->y               = 0;
    pwindow->width           = atoi (argv [1]);
    pwindow->height          = atoi (argv [2]);
    pwindow->window          = (Window*) window;
    
    pwindow->clipRect.top    = 0;
    pwindow->clipRect.left   = 0;
    pwindow->clipRect.bottom = atoi (argv [1]);
    pwindow->clipRect.right  = atoi (argv [2]);

    (NPSetWindowCallbackStruct*) (pwindow->ws_info) = (NPSetWindowCallbackStruct*) malloc (sizeof (NPSetWindowCallbackStruct));
    
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->display  = display;
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->visual   = DefaultVisual(display, DefaultScreen (display));
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->colormap = DefaultColormap(display, DefaultScreen (display));
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->depth    = DefaultDepth(display, DefaultScreen (display));
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->type     = 0;
    
    pluginInstance = (NPP) malloc (sizeof (NPP_t));

    (*(pluginTable [currentPlugin]->pluginFunctionsTable->newp)) (pluginType, pluginInstance, NP_EMBED, argc, argn, argv, /* saved */ NULL);

    stream      = (NPStream*) malloc (sizeof (NPStream));
    stream->url = filename;
    stream->end = 0;
       
    (*(pluginTable [currentPlugin]->pluginFunctionsTable->setwindow)) (pluginInstance, pwindow); 
    
    ret = (*(pluginTable [currentPlugin]->pluginFunctionsTable->newstream)) (pluginInstance, pluginType, stream, TRUE, &stype); 

    range.offset = 0; /*10; */
    range.length = 2000; /*20;*/
    range.next   = NULL;

    /*printf ("WriteReady = %d\n", (int) (*(pluginFunctionsTable->writeready)) (pluginInstance, stream));*/
    /*AM_requestread(stream, &range);*/
 
    printf ("Retrun from new stream = %d\n", ret);
    (*(pluginTable [currentPlugin]->pluginFunctionsTable->asfile)) (pluginInstance, stream, filename);
    /*dlclose(fighandle);*/       
}

