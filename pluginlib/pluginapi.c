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

#define THOT_EXPORT extern
#include "picture_tv.h"
#include "frame_tv.h"

extern ThotAppContext   app_cont;
extern PluginInfo*      pluginTable [100];
extern Document         currentDocument;
extern int              pluginCounter;
extern int              InlineHandlers;
extern int              currentExtraHandler;
extern PictureHandler   PictureHandlerTable[MAX_PICT_FORMATS];

static NPMIMEType       pluginMimeType;

#ifdef _WINDOWS
FARPROC ptr_NPP_GetMIMEDescription ;
FARPROC ptr_NPP_Initialize;
FARPROC ptr_NP_Initialize;
#else  /* _WINDOWS */
static int (*ptr_NPP_GetMIMEDescription) () ;
static int (*ptr_NPP_Initialize)         () ;
static int (*ptr_NP_Initialize)          (NPNetscapeFuncs*, NPPluginFuncs*) ;
#endif /* _WINDOWS */

NPNetscapeFuncs* amayaFunctionsTable;

#ifndef EOS
#define EOS '\0'
#endif
#ifndef SPACE
#define SPACE ' '
#endif
#ifndef TAB
#define TAB '\t'
#endif
#ifndef BUFSIZE
#define BUFSIZE 4096
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
                    if (pluginMimeType [index] == ':' || pluginMimeType [index] == ';' || pluginMimeType [index] == EOS) {
                       token [ndx] = EOS;
                       endOfSuffixes = TRUE ;
                       if (pluginMimeType [index] != EOS)
                          index++;
                    } else {
                            if (pluginMimeType [index] == '.') index++ ;
                            while (isalnum (pluginMimeType [index]))
                                  token [ndx++] = suffixes [suffixNdx++] = pluginMimeType [index++];
	            }
                    if (pluginMimeType [index] == ',')
                       token [ndx++] = suffixes [suffixNdx++] = pluginMimeType [index++];
              }
              printf ("suffixes: %s\n", token);
              if (pluginMimeType [index] !=EOS) {
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
   }
   suffixes [suffixNdx] = EOS;
   printf ("Suffixes: %s\n", suffixes) ;
   pluginTable [indexHandler]->fileExt = (char*) malloc (strlen (suffixes) + 1) ;
   strcpy (pluginTable [indexHandler]->fileExt, suffixes);
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
    printf ("***** Ap_MemAlloc *****\n") ;
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
    printf ("***** Ap_MemFlush *****\n") ; 
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
    printf ("***** Ap_MemFree *****\n") ;
    free (ptr);
}

/*----------------------------------------------------------------------
   Ap_Normal                                                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void Ap_Normal (NPP pluginInstance, NPStream* stream, char* url) 
#else  /* __STDC__ */
static void Ap_Normal (pluginInstance, stream, url) 
NPP       pluginInstance; 
NPStream* stream; 
char*     url;
#endif /* __STDC__ */
{
    FILE*       fptr;
    char*       buffer;
    int         count = 0, ret = 0, ready_to_read;
    long        offset;

    printf ("***** Ap_Normal *****\n") ;

    fptr = fopen (url, "rb");

    offset = 0;
    fseek (fptr, offset, SEEK_SET);
     
    while (!feof (fptr)) { 
	  ready_to_read = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->writeready)) (pluginInstance, stream);
	  buffer = (char*) malloc (ready_to_read);
	  count = fread (buffer, sizeof (char), ready_to_read, fptr);                
	  ret = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->write)) (pluginInstance, stream, offset , count , buffer);
	  printf ("%d WriteReady \n", ready_to_read);
	  printf ("\t%d bytes consumed by NPP_Write\n", ret);
	  offset += count;
	  free (buffer);
    }
    fclose (fptr);
}

/*----------------------------------------------------------------------
   Ap_AsFile                                                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void Ap_AsFile (NPP pluginInstance, NPStream* stream, char* url) 
#else  /* __STDC__ */
static void Ap_AsFile (pluginInstance, stream, url) 
NPP       pluginInstance; 
NPStream* stream; 
char*     url;
#endif /* __STDC__ */
{
    FILE*       fptr;
    char        buffer [BUFSIZE];
    int         count = 0, ret = 0;
    long        offset;

    printf ("***** Ap_AsFile *****\n") ;

    fptr = fopen (url, "rb");

    offset = 0;
    fseek (fptr, offset, SEEK_SET);
     
    while (!feof (fptr)) { 
	  count = fread (buffer, sizeof (char), BUFSIZE, fptr);
	  ret = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->write)) (pluginInstance, stream, offset , count , buffer);
	  printf ("\t%d bytes consumed by NPP_Write\n", ret);
	  offset += count;
    }
    fclose (fptr);
}

#ifdef __STDC__
void Ap_FreePicture (PictInfo* imageDesc) 
#else  /* __STDC__ */
void Ap_FreePicture (imageDesc)
PictInfo* imageDesc;
#endif /* __STDC__ */
{
   pluginTable [imageDesc->PicType - InlineHandlers]->pluginFunctionsTable->destroy ((NPP)(imageDesc->pluginInstance), NULL);
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
    printf ("***** Ap_DestroyStream *****\n") ;

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
    printf ("***** Ap_GetJavaEnv *****\n") ; 
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
    printf ("***** Ap_GetJavaPeer *****\n") ; 
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

    printf ("***** Ap_GetURL *****\n") ; 
    
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
  Ap_NewStream
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_NewStream (NPP instance, NPMIMEType type, const char* window, NPStream** stream_ptr)
#else  /* __STDC__ */
NPError Ap_NewStream (NPP instance, NPMIMEType type, const char* window, NPStream** stream_ptr)
#endif /* __STDC__ */
{
    printf ("***** Ap_NewStream *****\n") ;

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
    printf ("***** Ap_PostURL *****\n") ;
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
    /*FILE        *fptr;*/

    printf ("***** Ap_RequestRead *****\n") ;

    /*
    char        *buffer;
    int         count = 0, ret = 0, ready_to_read, size;
    long        offset;*/

    printf ("RequestRead  \n");
    /*
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
    fclose (fptr);
    */
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
    printf ("***** Ap_Status *****\n") ;
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
    printf ("***** Ap_UserAgent *****\n") ;
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
    printf ("***** Ap_Write *****\n") ;
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
    printf ("***** Ap_Version *****\n") ;
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
    printf ("***** Ap_ReloadPlugins *****\n") ;
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
    printf ("***** Ap_GetValue *****\nVariable: %d\n", variable) ;
    if (!instance) error = NPERR_INVALID_INSTANCE_ERROR;
    else {
         switch (variable) {
         case NPNVxDisplay:     *((char**) r_value) = (char*) TtaGetCurrentDisplay () ;
                                break;
         case NPNVxtAppContext: *((char**) r_value) = (char*) app_cont;
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
void Ap_InitializePluginTable (int indexHandler)
#else  /* __STDC__ */
void Ap_InitializePluginTable (indexHandler)
int indexHandler;
#endif /* __STDC__ */
{
    char* message ;
    int   ret ;

    printf ("***** Ap_InitializePluginTable *****\n") ;

    pluginTable [indexHandler]->pluginFunctionsTable = (NPPluginFuncs*) malloc (sizeof (NPPluginFuncs));
    /* printf ("Size of NPPluginFuncs = %d\n", (int) sizeof (NPPluginFuncs)); */
    pluginTable [indexHandler]->pluginFunctionsTable->size         = ((uint16) (sizeof (NPPluginFuncs)));

#ifdef _WINDOWS
    ptr_NP_Initialize = GetProcAdress (pluginTable [indexHandler]->pluginHandle, "NP_Initialize");
    if (ptr_NP_Initialize == NULL) {
       message (char*) malloc (65 + strlen (pluginTable [indexHandler]->pluginDL));
       sprintf (message, "relocation error: symbol not found: NP_Initialize referenced in %s", pluginTable [indexHandler]->pluginDL);
    } else
          ret = (*ptr_NP_Initialize) (amayaFunctionsTable, pluginTable [indexHandler]->pluginFunctionsTable);
#else  /* _WINDOWS */
    ptr_NP_Initialize = (int (*) (NPNetscapeFuncs*, NPPluginFuncs*)) dlsym (pluginTable [indexHandler]->pluginHandle, "NP_Initialize");
    message = (char*) dlerror ();    
    if (message) 
	printf ("ERROR at Initialization: %s\n", message);

    ret = ptr_NP_Initialize (amayaFunctionsTable, pluginTable [indexHandler]->pluginFunctionsTable);
#endif /* _WINDOWS */
    /* printf ("result = %d\n", ret); */
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
    char* message = (char*) NULL;
    char GUI_Name [20];
    int  index1 = 0 ;
    int  index2 = 0;
    int  ret ;
    printf ("***** Ap_InitializePlugin *****\n") ;

#ifdef _WINDOWS
    pluginTable [indexHandler]->pluginHandle = LoadLibrary (path);
    if (pluginTable [indexHandler]->pluginHandle == NULL) {
	message = (char*) malloc (12 + strlen (path));
        sprintf (message, "Cannot open library %s", path);
    }
#else  /* _WINDOWS */
    pluginTable [indexHandler]->pluginHandle = dlopen (path, RTLD_NOW);
    message = (char*) dlerror ();
#endif /* _WINDOWS */
    
    if (message) {
	printf ("dlerror message: %s\n", message);
	exit (0);
    }

    /* get the symbols from the dynamic library */
#ifdef _WINDOWS
    ptr_NPP_Initialize = GetProcAdress (pluginTable [indexHandler]->pluginHandle, "NPP_Initialize");
    if (ptr_NPP_Initialize == NULL) {
       message (char*) malloc (65 + strlen (pluginTable [indexHandler]->pluginDL));
       sprintf (message, "relocation error: symbol not found: NPP_Initialize referenced in %s", pluginTable [indexPlug]->pluginDL);
    } else
          ret = (*ptr_NPP_Initialize) ();
    ptr_NPP_GetMIMEDescription = GetProcAdress (pluginTable [indexHandler]->pluginHandle, "NPP_GetMIMEDescription");
#else  /* _WINDOWS */
    ptr_NPP_Initialize = (int (*) ()) dlsym (pluginTable [indexHandler]->pluginHandle, "NPP_Initialize");
    message = (char*) dlerror ();    
    if (message) 
       printf ("ERROR at Initialization: %s\n", message);

    ret = (*ptr_NPP_Initialize) ();
    ptr_NPP_GetMIMEDescription = (int(*) ()) dlsym (pluginTable [indexHandler]->pluginHandle, "NPP_GetMIMEDescription");
#endif /* _WINDOWS */

    pluginMimeType = (NPMIMEType) (*ptr_NPP_GetMIMEDescription) ();
    pluginTable [indexHandler]->pluginMimeType = (char*) malloc (strlen (pluginMimeType) + 1) ;
    strcpy (pluginTable [indexHandler]->pluginMimeType, pluginMimeType);
    /* printf ("Mime: %s\n", pluginMimeType); */
    ParseMIMEType (indexHandler, pluginMimeType);
    while (pluginTable [indexHandler]->fileExt [index1] != ',' && pluginTable [indexHandler]->fileExt [index1] != '\0')
          GUI_Name [index1] = pluginTable [indexHandler]->fileExt [index1++] ;
    GUI_Name [index1++] = ' ';
    GUI_Name [index1++] = '(';
    GUI_Name [index1++] = '.';
    while (pluginTable [indexHandler]->fileExt [index2] != ',' && pluginTable [indexHandler]->fileExt [index2] != '\0')
          GUI_Name [index1++] = pluginTable [indexHandler]->fileExt [index2++] ;
    GUI_Name [index1++] = ')' ;
    GUI_Name [index1] = '\0';

    /* printf ("GUI_Name : %s\n", GUI_Name); */
    
    strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, GUI_Name, MAX_FORMAT_NAMELENGHT);
    /* Initializing the pointers to the netscape functions */
    Ap_InitializePluginTable (indexHandler);

}

/*----------------------------------------------------------------------
  CreateInstance
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_CreatePluginInstance (PictInfo *imageDesc, Display *display) 
#else  /* __STDC__ */
void Ap_CreatePluginInstance (imageDesc, display)
PictInfo *imageDesc;
Display* display; 
#endif /* __STDC__ */
{
    NPStream*   stream;
    NPByteRange range;
    NPWindow*   pwindow;
    char        widthText[10], heightText[10];
    char*       argn[6], *argv[6];
    char*       url;
    char*       message = (char*) NULL;
    uint16      stype;
    int         ret;
    int16       argc  = 6; /* to parametrize */
    /* int16       argc  = 3; */ /* to parametrize */
    struct stat sbuf;
     
    printf ("***** Ap_CreatePluginInstance *****\n") ;

    argn[0] = "SRC";
    argn[1] = "WIDTH";
    argn[2] = "HEIGHT";
    
    argn[3] = "CONTROLS";
    argn[4] = "AUTOSTART";
    argn[5] = "STATUSBAR";
    
    sprintf (widthText, "%d", imageDesc->PicWArea);
    sprintf (heightText, "%d", imageDesc->PicHArea);
    argv[0] = imageDesc->PicFileName;
    argv[1] = widthText;
    argv[2] = heightText;
    
    argv[3] = "TRUE";
    argv[4] = "TRUE";
    argv[5] = "TRUE";
    
    currentExtraHandler  = imageDesc->PicType - InlineHandlers;

    /* Prepare window information and "instance" structure */
    pwindow                  = (NPWindow*) malloc (sizeof (NPWindow));
    pwindow->x               = 0;
    pwindow->y               = 0;
    pwindow->width           = imageDesc->PicWArea;
    pwindow->height          = imageDesc->PicHArea;
    pwindow->window          = (Window*) XtWindow ((Widget) (imageDesc->wid));
    
    pwindow->clipRect.top    = 0;
    pwindow->clipRect.left   = 0;
    pwindow->clipRect.bottom = imageDesc->PicWArea;
    pwindow->clipRect.right  = imageDesc->PicHArea;

    pwindow->ws_info = (NPSetWindowCallbackStruct*) malloc (sizeof (NPSetWindowCallbackStruct));
    
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->display  = display;
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->visual   = DefaultVisual(display, DefaultScreen (display));
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->colormap = DefaultColormap(display, DefaultScreen (display));
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->depth    = DefaultDepth(display, DefaultScreen (display));
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->type     = 0;

    url = (char*) malloc (strlen (imageDesc->PicFileName) + 1);
    strcpy (url, imageDesc->PicFileName);
    
    (NPP) (imageDesc->pluginInstance) = (NPP) malloc (sizeof (NPP_t)); 
    (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->newp)) (pluginTable [currentExtraHandler]->pluginMimeType, 
                                                             (NPP)(imageDesc->pluginInstance), 
                                                             NP_EMBED, 
                                                             argc, 
                                                             argn, 
                                                             argv,  
                                                             NULL);

    stat (url, &sbuf);

    stream               = (NPStream*) malloc (sizeof (NPStream));
    stream->url          = strdup (url);
    stream->end          = 0;
    stream->pdata        = ((NPP) (imageDesc->pluginInstance))->pdata;
    stream->ndata        = NULL;
    stream->notifyData   = NULL;
    stream->end          = sbuf.st_size;
    stream->lastmodified = sbuf.st_mtime;


    (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->setwindow)) ((NPP)(imageDesc->pluginInstance), pwindow); 
    
    ret = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->newstream)) ((NPP)(imageDesc->pluginInstance), 
                                                                         pluginTable [currentExtraHandler]->pluginMimeType,
                                                                         stream, 
                                                                         FALSE, 
                                                                         &stype); 

    printf ("Stype : %d\n", stype);

    switch (stype) {
           case NP_NORMAL:     Ap_Normal ((NPP) (imageDesc->pluginInstance), stream, url); 
                               break;
           case NP_ASFILEONLY: (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->asfile)) ((NPP)(imageDesc->pluginInstance), stream, url);
                               break;
	   case NP_ASFILE:     Ap_AsFile ((NPP) (imageDesc->pluginInstance), stream, url);
	       /*case NP_ASFILE:     Ap_Normal (indexPlug, (NPP) (imageDesc->pluginInstance), stream, url);  */
                               break;
           case NP_SEEK:       /* ?????????????????????????????
                                  CALL Ap_RequestRead
                                  ????????????????????????????? */
                               break;
           default:            printf ("Error unknown mode %d\n", stype);
                               break;
    }

    (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->asfile)) ((NPP)(imageDesc->pluginInstance), stream, url);
    /* (*(pluginTable [indexPlug]->pluginFunctionsTable->destroystream)) ((NPP)(imageDesc->pluginInstance), stream, NPRES_DONE);*/
    range.offset = 0; /*10; */
    range.length = 2000; /*20;*/
    range.next   = NULL;

    /*AM_requestread(stream, &range);*/
 
    /* printf ("Retrun from new stream = %d\n", ret); */
    /*dlclose(fighandle);*/       
}

