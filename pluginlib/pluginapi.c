/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/* authors:
 * - Ramzi GUETARI (W3C/INRIA)
 * - Nabil LAYAIDA
 * - Loay  SABRY-ISMAIL
 *
 * Last modification: Jan 30 1998
 */

/*----------------------------------------------------------------------
  This module implements Amaya plug-in API functions.
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

#define MAX_LENGTH 512
#define THOT_EXPORT extern

/* How are Network accesses provided ? */

#include "amaya.h"
#ifdef AMAYA_JAVA
#include "content.h"
#include "libjava.h"
#else
#include "libwww.h"
#endif

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
static NPStream*        progressStream;
static boolean          streamOpened = FALSE;

#ifdef _WINDOW
FARPROC ptr_NPP_GetMIMEDescription;
FARPROC ptr_NPP_Initialize;
FARPROC ptr_NP_Initialize;
#else  /* _WINDOWS */
static int (*ptr_NPP_GetMIMEDescription) ();
static int (*ptr_NPP_Initialize)         ();
static int (*ptr_NP_Initialize)          (NPNetscapeFuncs*, NPPluginFuncs*);
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
#include "AHTURLTools_f.h"
#include "init_f.h"
#include "query_f.h"
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
   int  index     = 0;
   int  suffixNdx = 0;
   int  ndx;
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
#              ifdef PLUGIN_DEBUG
	       printf ("Plugin name: %s\n", token);
#              endif
               pluginTable [indexHandler]->pluginID = (char*) malloc (strlen (token) + 1);
               strcpy (pluginTable [indexHandler]->pluginID, token);
            } else {
#                  ifdef PLUGIN_DEBUG
                   printf ("Plugin name: Unknown plugin\n");              
#                  endif
                   pluginTable [indexHandler]->pluginID = (char*) malloc (14);
                   strcpy (pluginTable [indexHandler]->pluginID, "Unknown Plugin");
            }
         } else {
#             ifdef PLUGIN_DEBUG
	      printf ("Plugin type: %s\n", token);
#             endif
              if (pluginMimeType [index] != ':')
		printf ("bad mime type\n");
              else 
                  index ++;
              ndx = 0;

              while (!endOfSuffixes) {
                    while (pluginMimeType [index] == SPACE || pluginMimeType [index] == TAB)
		          index++;
                    if (pluginMimeType [index] == ':' || pluginMimeType [index] == ';' || pluginMimeType [index] == EOS) {
                       token [ndx] = EOS;
                       endOfSuffixes = TRUE;
                       if (pluginMimeType [index] != EOS)
                          index++;
                    } else {
                            if (pluginMimeType [index] == '.') index++;
                            while (isalnum (pluginMimeType [index]))
                                  token [ndx++] = suffixes [suffixNdx++] = pluginMimeType [index++];
	            }
                    if (pluginMimeType [index] == ',')
                       token [ndx++] = suffixes [suffixNdx++] = pluginMimeType [index++];
              }
#             ifdef PLUGIN_DEBUG
              printf ("suffixes: %s\n", token);
#             endif
              if (pluginMimeType [index] !=EOS) {
                 ndx = 0;
                 while (pluginMimeType [index] != EOS && pluginMimeType [index] != ';' && pluginMimeType [index] != ':')
                       token [ndx++] = pluginMimeType [index++];
                 token [ndx] = EOS;
                 if (pluginMimeType [index] == ';') {
                    suffixes [suffixNdx++] = ',';
                    index++;
                 }
                 if (pluginMimeType [index] == ':')
                    index++;
                 pluginTable [indexHandler]->pluginID = (char*) malloc (strlen (token) + 1);
                 strcpy (pluginTable [indexHandler]->pluginID, token);
#                ifdef PLUGIN_DEBUG
                 printf ("comment: %s\n", token);
#                endif
              }
         }
   }
   suffixes [suffixNdx] = EOS;
#  ifdef PLUGIN_DEBUG
   printf ("Suffixes: %s\n", suffixes);
#  endif
   pluginTable [indexHandler]->fileExt = (char*) malloc (strlen (suffixes) + 1);
   strcpy (pluginTable [indexHandler]->fileExt, suffixes);
}

/*----------------------------------------------------------------------
   Ap_Normal: This function is called by Ap_GetURL and Ap_CreatePluginInstance
              if the type of stream required by the plugin is NP_NORMAL.
              The data is delivered pregressively to the plug-in. 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void Ap_Normal (NPP pluginInstance, NPStream* stream, const char* url) 
#else  /* __STDC__ */
static void Ap_Normal (pluginInstance, stream, url) 
NPP         pluginInstance; 
NPStream*   stream; 
const char* url;
#endif /* __STDC__ */
{
    FILE*       fptr;
    char*       buffer;
    int         count = 0, ret = 0, ready_to_read;
    long        offset;

#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_Normal *****\n");
#   endif
    fptr = fopen (url, "rb");

    offset = 0;
    fseek (fptr, offset, SEEK_SET);
     
    while (!feof (fptr)) { 
	  /* What quantity of data the plug-in is ready to accept (ready_to_read)? */
	  ready_to_read = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->writeready)) (pluginInstance, stream);

	  buffer = (char*) malloc (ready_to_read);
          /* Reading data */
	  count = fread (buffer, sizeof (char), ready_to_read, fptr);   

          /* Data is delivered to the plug-in */             
	  ret = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->write)) (pluginInstance, stream, offset , count , buffer);
#         ifdef PLUGIN_DEBUG
	  printf ("%d WriteReady \n", ready_to_read);
	  printf ("\t%d bytes consumed by NPP_Write\n", ret);
#         endif
	  offset += count;
	  free (buffer);
    }
    fclose (fptr);
}

/*----------------------------------------------------------------------
   Ap_AsFile: This function is called by Ap_GetURL and Ap_CreatePluginInstance
              if the type of stream required by the plugin is NP_ASFILE.
              The data is delivered pregressively to the plug-in as it is saved. 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void Ap_AsFile (NPP pluginInstance, NPStream* stream, const char* url) 
#else  /* __STDC__ */
static void Ap_AsFile (pluginInstance, stream, url) 
NPP         pluginInstance; 
NPStream*   stream; 
const char* url;
#endif /* __STDC__ */
{
    FILE*       fptr;
    char        buffer [BUFSIZE];
    int         count = 0, ret = 0;
    long        offset;

#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_AsFile *****\n");
#   endif
    fptr = fopen (url, "rb");

    offset = 0;
    fseek (fptr, offset, SEEK_SET);
     
    while (!feof (fptr)) { 
	  count = fread (buffer, sizeof (char), BUFSIZE, fptr);
	  ret = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->write)) (pluginInstance, stream, offset , count , buffer);
#         ifdef PLUGIN_DEBUG
	  printf ("\t%d bytes consumed by NPP_Write\n", ret);
#         endif
	  offset += count;
    }
    fclose (fptr);
}

/*----------------------------------------------------------------------
  Ap_GetURLNotifyProgressCallback
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void   Ap_GetURLNotifyProgressCallback (int doc, int status, char *urlName, char *outputfile, char *content_type, char *pbuffer, int buffer_length,  void * context)
#else  /* __STDC__ */
static void  Ap_GetURLNotifyProgressCallback (doc, status, urlName, outpufile, content_type, pbuffer, buffer_length, context)

int doc;
int status;
char *urlName;
char *outputfile;
char *content_type;
char* pbuffer;
int   buffer_length;
void *context;

#endif /* __STDC__ */
{
#   ifndef AMAYA_JAVA
    struct stat    sbuf;
    static FILE*   fptr = NULL;
    static char*   file;
    static long    offset = 0;
    char*          buffer;
    NPP            instance;
    int            count;
    int            ready_to_read;
    uint16         stype;
    int            ret;

    /* manage status: checking errors */
    /* Test buffer_length             */

    instance = (NPP) context;

    if (!streamOpened) {    
       file                       = strdup (outputfile);
       fptr                       = fopen (file, "rb");
       progressStream             = (NPStream*) malloc (sizeof (NPStream));
       progressStream->url        = strdup (file);
       progressStream->pdata      = instance->pdata;
       progressStream->ndata      = NULL;
       progressStream->notifyData = NULL;
       ret = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->newstream))
	 (instance,
	  pluginTable [currentExtraHandler]->pluginMimeType,
	  progressStream, FALSE, &stype); 
       streamOpened = TRUE;    
    }

    stat (file, &sbuf);
    progressStream->end          = sbuf.st_size;
    progressStream->lastmodified = sbuf.st_mtime;
    
    if (fptr && offset < progressStream->end) {
       fseek (fptr, offset, SEEK_SET);
       
       ready_to_read = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->writeready)) (instance, progressStream);

       if (progressStream->end >= offset + ready_to_read) {       
	  buffer = (char*) malloc (ready_to_read);
	  /* Reading data */
	  count = fread (buffer, sizeof (char), ready_to_read, fptr);
	     
	  /* Data is delivered to the plug-in */             
	  ret = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->write)) (instance, progressStream, offset , ready_to_read, buffer);
#         ifdef PLUGIN_DEBUG
	  printf ("%d WriteReady \n", ready_to_read);
	  printf ("\t%d bytes consumed by NPP_Write\n", ret);
#         endif
	  offset += count;
	  free (buffer);
	  /*(*(pluginTable [currentExtraHandler]->pluginFunctionsTable->asfile)) ((NPP)(instance), progressStream, file); */
       }
    }
#   endif /* AMAYA_JAVA */
}

/*----------------------------------------------------------------------
  Ap_GetURLNotifyCallback
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void   Ap_GetURLNotifyCallback (int doc, int status, char *urlName, char *outputfile, char *content_type, void * context)
#else  /* __STDC__ */
static void  Ap_GetURLNotifyCallback (doc, status, urlName, outpufile, content_type, context)

int doc;
int status;
char *urlName;
char *outputfile;
char *content_type;
void *context;
#endif /* __STDC__ */
{
#   ifndef AMAYA_JAVA
    char*               file;
    struct stat         sbuf;
    NPStream*           stream;
    NPP                 instance;
    uint16              stype;
    int                 ret;

    if (status != HT_LOADED) {
       file = strdup (outputfile);
       instance = (NPP) context;

       stat (file, &sbuf);

       stream               = (NPStream*) malloc (sizeof (NPStream));
       stream->url          = strdup (file);
       stream->end          = 0;
       stream->pdata        = instance->pdata;
       stream->ndata        = NULL;
       stream->notifyData   = NULL;
       stream->end          = sbuf.st_size;
       stream->lastmodified = sbuf.st_mtime; 
       
       ret = (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->newstream))
	 (instance,
	  pluginTable [currentExtraHandler]->pluginMimeType,
	  stream, 
	  FALSE, 
	  &stype); 
       Ap_Normal ((NPP) (instance), stream, file); 
    }
#   endif /* AMAYA_JAVA */
}

/*----------------------------------------------------------------------
  Ap_MemAlloc
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void* Ap_MemAlloc (uint32 size)
#else  /* __STDC__ */
void* Ap_MemAlloc (size)
uint32 size;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_MemAlloc *****\n");
#   endif
    return malloc (size);
}

/*----------------------------------------------------------------------
  Ap_MemFlush
  ----------------------------------------------------------------------*/
#ifdef __STDC__
uint32  Ap_MemFlush (uint32 size)
#else  /* __STDC__ */
uint32  Ap_MemFlush (size)
uint32 size;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_MemFlush *****\n"); 
#   endif
    return ((uint32)malloc (size));
}

/*----------------------------------------------------------------------
  Ap_MemFree
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  Ap_MemFree (void* ptr)
#else  /* __STDC__ */
void  Ap_MemFree (ptr)
void* ptr;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_MemFree *****\n");
#   endif
    free (ptr);
}

/*----------------------------------------------------------------------
  Ap_FreePicture: Called when the instance of the plugin is destroyed
                  for example when leaving the document contaning the
                  plugin.
  ----------------------------------------------------------------------*/
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
NPP       instance;
NPStream* stream;
NPError   reason;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_DestroyStream *****\n");
#   endif

    if (instance == NULL)
       return NPERR_INVALID_INSTANCE_ERROR;

    if (stream->url)
       free ((char *)stream->url);

    if (stream->pdata)
       free (stream->pdata);

    if (stream->ndata)
       free (stream->ndata);

    if (stream->notifyData)
       free (stream->notifyData);

    free (stream);

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
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_GetJavaEnv *****\n"); 
#   endif
    return (NULL);
}

/*----------------------------------------------------------------------
  Ap_GetJavaEPeer
  ----------------------------------------------------------------------*/
#ifdef __STDC__
jref Ap_GetJavaPeer (NPP instance)
#else  /* __STDC__ */
jref Ap_GetJavaPeer (instance)
NPP instance;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_GetJavaPeer *****\n"); 
#   endif
    return (NULL);
}

/*----------------------------------------------------------------------
  Ap_URLNotify
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_URLNotify (NPP instance, const char* url, NPReason reason, void* notifyData)
#else  /* __STDC__ */
void Ap_URLNotify (instance, url, reason, notifyData)
NPP         instance; 
const char *url; 
NPReason    reason; 
void*       notifyData;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("*** Ap_URLNotify ***\n");
#   endif
}

/*----------------------------------------------------------------------
  Ap_GetURL
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_GetURL (NPP instance, const char* url, const char* target)
#else  /* __STDC__ */
NPError Ap_GetURL (instance, url, target)
NPP         instance;
const char *url;
const char *target;
#endif /* __STDC__ */
{
  int         result;
  char        tempfile [500];
  char        tempurl [500];

#   ifdef PLUGIN_DEBUG
  printf ("***** Ap_GetURL *****\n"); 
#   endif
  if (IsValidProtocol (url))
    {
      if (target)
	{
	  /* pass the stream to AMAYA */
#         ifdef PLUGIN_DEBUG
          printf ("AM_geturl: Passing the stream to AMAYA\n");
#         endif
          GetHTMLDocument (url, NULL, 0, 0, CE_FALSE, TRUE, NULL, NULL);
	}
      else
	{
	  /* pass the stream to the plug-in */
#         ifdef PLUGIN_DEBUG
	  printf ("AM_geturl: Passing stream to the plug-in\n");
#         endif
	  strcpy (tempurl, url);
	  result = GetObjectWWW (1, tempurl, NULL, tempfile, AMAYA_IASYNC, 
				 (void*) Ap_GetURLNotifyProgressCallback,
				 (void*) instance, 
				 (void*) Ap_GetURLNotifyProgressCallback,
				 (void*) instance, FALSE, NULL);
	}
    }
# ifdef PLUGIN_DEBUG
  else
    /* java? */
    printf ("AM_geturl: Passing the stream to Java Virtual Machine\n");
# endif
  return NPERR_NO_ERROR;
}

/*----------------------------------------------------------------------
  Ap_GetURLNotify
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_GetURLNotify (NPP instance, const char* url, const char *target, void* notifyData)
#else  /* !__STDC__ */
NPError Ap_GetURLNotify (instance, url, target, notifyData)
NPP         instance; 
const char *url; 
const char *target; 
void       *notifyData;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("*** Ap_GetURLNotify ***\n");
#   endif
    Ap_GetURL (instance, url, target);
    Ap_URLNotify (instance, url, NPRES_DONE, notifyData);
    return NPERR_NO_ERROR;
}

/*----------------------------------------------------------------------
  Ap_NewStream
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError    Ap_NewStream (NPP instance, NPMIMEType type, const char *window, NPStream **stream_ptr)
#else  /* __STDC__ */
NPError    Ap_NewStream (instance, type, window, stream_ptr)
NPP        instance;
NPMIMEType type;
const char *window;
NPStream  **stream_ptr;
#endif /* __STDC__ */
{
#ifdef PLUGIN_DEBUG
    printf ("***** Ap_NewStream *****\n");
#endif

    (*stream_ptr)               = (NPStream*) malloc (sizeof (NPStream));
    (*stream_ptr)->url          = NULL;
    (*stream_ptr)->end          = 0;
    (*stream_ptr)->pdata        = instance->pdata;
    (*stream_ptr)->ndata        = NULL;
    (*stream_ptr)->notifyData   = NULL;
    (*stream_ptr)->end          = 0;
    (*stream_ptr)->lastmodified = 0; 
    
    return NPERR_NO_ERROR;
}

/*----------------------------------------------------------------------
  Ap_PostURL
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_PostURL (NPP instance, const char* url, const char* target, uint32 len, const char* buf, NPBool file)
#else  /* __STDC__ */
NPError Ap_PostURL (instance, url, target, len, buf, file)
NPP         instance;
const char* url;
const char* target;
uint32      len;
const char* buf;
NPBool      file;
#endif /* __STDC__ */
{
  int         count;
  char*       bufToPost = NULL;
  char*       fileToPost;
  char        tempurl[500];
  char        tempbuf[500];
  FILE*       fPtr;
  struct stat sBuff;

#   ifdef PLUGIN_DEBUG
  printf ("***** Ap_PostURL *****\n");
#   endif
  strcpy (tempurl, url);
  strcpy (tempbuf, buf);
  if (file)
    {
      /* Posting data from a file */
      if (!strncasecmp (tempbuf, "file://", 7))
	fileToPost = &tempbuf[7];
      else
	fileToPost = tempbuf;
      
      stat (fileToPost, &sBuff);      
      if ((fPtr = fopen (fileToPost, "rb")) == NULL)
	{
	  printf ("error: file %s does not exist\n", fileToPost);
	  return (NPERR_FILE_NOT_FOUND);
	}
      else
	{
	  count = fread (bufToPost, sizeof (char), sBuff.st_size, fPtr);
	  if (count != sBuff.st_size)
	    {
	      printf ("error occured while reading file: %s\n", fileToPost);
	      return (NPERR_GENERIC_ERROR);
	    }
	  GetObjectWWW (1, tempurl, bufToPost, NULL, AMAYA_ASYNC, NULL, NULL, NULL, NULL, 0, NULL);
	}
    }
  else
    /* Posting data from memory */
    GetObjectWWW (1, tempurl, tempbuf, NULL, AMAYA_ASYNC, NULL, NULL, NULL, NULL, 0, NULL);
  
   return (NPERR_NO_ERROR);
}

/*----------------------------------------------------------------------
  Ap_PostURLNotify
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_PostURLNotify (NPP instance, const char* url, const char* target, uint32 len, const char* buf, NPBool file, void* notifyData)
#else  /* !__STDC__ */
NPError Ap_PostURLNotify (instance, url, target, len, buf, file, void* notifyData)
NPP         instance; 
const char* url; 
const char* target; 
uint32      len;
const char* buf;
NPBool      file;
void*       notifyData;
#endif /* __STDC__ */
{
    printf ("*** Ap_PostURLNotify ***\n");

    Ap_PostURL (instance, url, target, len, buf, file); 
    Ap_URLNotify (instance, url, NPRES_DONE, notifyData);
    return (NPERR_NO_ERROR);
}

/*----------------------------------------------------------------------
  Ap_RequestRead
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_RequestRead (NPStream* stream, NPByteRange* rangeList)
#else  /* __STDC__ */
NPError Ap_RequestRead (stream, rangeList)
NPStream*    stream;
NPByteRange* rangeList;
#endif /* __STDC__ */
{
    FILE*        fptr;
    char*        buffer = NULL;
    int          count;
    long         offset;
    NPByteRange* currentRangeList = rangeList;

#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_RequestRead *****\n");
#   endif
    fptr = fopen (stream->url, "rb");
   
    while (currentRangeList)
      {
	if (currentRangeList->offset < 0)
	  offset = stream->end - currentRangeList->length;
	else
	  offset = currentRangeList->offset;
	
	fseek (fptr, offset, SEEK_SET);
	buffer = (char*) malloc (currentRangeList->length);
	count = fread (buffer, sizeof (char), currentRangeList->length, fptr);                
      }
    if (buffer)
      free (buffer);
    fclose (fptr);
    return (NPERR_NO_ERROR);
}

/*----------------------------------------------------------------------
  Ap_Status
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_Status (NPP instance, const char* message)
#else  /* __STDC__ */
void Ap_Status (instance, message)
NPP         instance; 
const char* message;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_Status *****\n");
    printf ("Status %s\n", message);
#   endif
}

/*----------------------------------------------------------------------
  Ap_UserAgent
  ----------------------------------------------------------------------*/
#ifdef __STDC__
const char* Ap_UserAgent (NPP instance)
#else  /* __STDC__ */
const char* Ap_UserAgent (instance)
NPP instance;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_UserAgent *****\n");
#   endif
    return ("Amaya");
}

/*----------------------------------------------------------------------
  Ap_Write
  ----------------------------------------------------------------------*/
#ifdef __STDC__ 
int32 Ap_Write (NPP instance, NPStream* stream, int32 len, void* buffer)
#else  /* __STDC__ */
int32 Ap_Write (instance, stream, len, buffer)
NPP       instance;
NPStream* stream;
int32     len;
void*     buffer;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_Write *****\n");
#   endif
    return 4;
}

/*----------------------------------------------------------------------
  Ap_Version
  ----------------------------------------------------------------------*/
#ifdef __STDC__ 
void Ap_Version (int* plugin_major, int* plugin_minor, int* amaya_major, int* amaya_minor)
#else  /* __STDC__ */
void Ap_Version (plugin_major, plugin_minor, amaya_major, amaya_minor)
int* plugin_major; 
int* plugin_minor; 
int* amaya_major; 
int* amaya_minor;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_Version *****\n");
#   endif
    *plugin_major = NP_VERSION_MAJOR;
    *plugin_minor = NP_VERSION_MINOR;
    *amaya_major  = 1;
    *amaya_minor  = 0;
}

/*----------------------------------------------------------------------
  Ap_ReloadPlugins
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_ReloadPlugins (NPBool reloadPages)
#else  /* __STDC__ */
void Ap_ReloadPlugins (reloadPages)
NPBool reloadPages;
#endif /* __STDC__ */
{
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_ReloadPlugins *****\n");
#   endif
}

#ifdef XP_UNIX
/*----------------------------------------------------------------------
  AM_getvalue
  ----------------------------------------------------------------------*/
#ifdef __STDC__
NPError Ap_GetValue (NPP instance, NPNVariable variable, void* r_value)
#else  /* __STDC__ */
NPError Ap_GetValue (instance, variable, r_value)
NPP         instance; 
NPNVariable variable; 
void*       r_value;
#endif /* __STDC__ */
{
    NPError error = NPERR_NO_ERROR;
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_GetValue *****\nVariable: %d\n", variable);
#   endif

    if (!instance) error = NPERR_INVALID_INSTANCE_ERROR;
    else {
         switch (variable) {
         case NPNVxDisplay:     *((char**) r_value) = (char*) TtaGetCurrentDisplay ();
                                break;
         case NPNVxtAppContext: *((char**) r_value) = (char*) app_cont;
                                break;
         default:               error = NPERR_GENERIC_ERROR;
                                break;
         }
    }
    return error;
}
#endif /* XP_UNIX */


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
#   ifdef PLUGIN_DEBUG
    printf ("Size of NPAmayaFuncs = %d\n", (int) sizeof (NPNetscapeFuncs));
#   endif

    amayaFunctionsTable->size          = ((uint16) (sizeof       (NPNetscapeFuncs)));
    amayaFunctionsTable->version       = ((uint16) (0));
    amayaFunctionsTable->geturl        = ((NPN_GetURLUPP)        (Ap_GetURL));
    amayaFunctionsTable->posturl       = ((NPN_PostURLUPP)       (Ap_PostURL));
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
    amayaFunctionsTable->geturlnotify  = ((NPN_GetURLNotifyUPP)  (Ap_GetURLNotify));
    amayaFunctionsTable->posturlnotify = ((NPN_PostURLNotifyUPP) (Ap_PostURLNotify));
#ifdef XP_UNIX
    amayaFunctionsTable->getvalue      = ((NPN_GetValueUPP)      (Ap_GetValue));
#endif /* XP_UNIX */
}

/*----------------------------------------------------------------------
  Ap_InitializePluginTable
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_InitializePluginTable (int indexHandler)
#else  /* __STDC__ */
void Ap_InitializePluginTable (indexHandler)
int indexHandler;
#endif /* __STDC__ */
{
    char* message;
    int   ret;

#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_InitializePluginTable *****\n");
#   endif

    pluginTable[indexHandler]->pluginFunctionsTable = (NPPluginFuncs*) malloc (sizeof (NPPluginFuncs));
#   ifdef PLUGIN_DEBUG
    printf ("Size of NPPluginFuncs = %d\n", (int) sizeof (NPPluginFuncs));
#   endif
    pluginTable[indexHandler]->pluginFunctionsTable->size = ((uint16) (sizeof (NPPluginFuncs)));
  
#   ifdef _WINDOWS
    ptr_NP_Initialize = GetProcAdress (pluginTable [indexHandler]->pluginHandle, "NP_Initialize");
    if (ptr_NP_Initialize == NULL) {
       message (char*) malloc (65 + strlen (pluginTable [indexHandler]->pluginDL));
       sprintf (message, "relocation error: symbol not found: NP_Initialize referenced in %s", pluginTable [indexHandler]->pluginDL);
    } else
         ret = (*ptr_NP_Initialize) (amayaFunctionsTable, pluginTable [indexHandler]->pluginFunctionsTable);
#   else  /* _WINDOWS */
    ptr_NP_Initialize = (int (*) (NPNetscapeFuncs*, NPPluginFuncs*)) dlsym (pluginTable [indexHandler]->pluginHandle, "NP_Initialize");
    message = (char*) dlerror ();    
    if (message) 
       printf ("ERROR at Initialization: %s\n", message);
  
    ret = ptr_NP_Initialize (amayaFunctionsTable, pluginTable [indexHandler]->pluginFunctionsTable);
#   endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  InitializePlugin
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int Ap_InitializePlugin (char* path, int indexHandler)
#else  /* __STDC__ */
int Ap_InitializePlugin (path, indexHandler)
char* path;
int   indexHandler;
#endif /* __STDC__ */
{
    /* Open the library and get the symbols addresses */
    char* message = (char*) NULL;
    char GUI_Name [20];
    int  index1 = 0;
    int  index2 = 0;
    int  ret;
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_InitializePlugin *****\n");
#   endif
#   ifdef _WINDOWS
    pluginTable [indexHandler]->pluginHandle = LoadLibrary (path);
    if (pluginTable [indexHandler]->pluginHandle == NULL) {
	message = (char*) malloc (12 + strlen (path));
        sprintf (message, "Cannot open library %s", path);
    }
#   else  /* _WINDOWS */
    pluginTable [indexHandler]->pluginHandle = dlopen (path, RTLD_LAZY);
    message = (char*) dlerror ();
#   endif /* _WINDOWS */
    
    if (message) {
	printf ("ERROR: %s\n", message);
        return -1;
    } 

    /* get the symbols from the dynamic library */
#   ifdef _WINDOWS
    ptr_NPP_Initialize = GetProcAdress (pluginTable [indexHandler]->pluginHandle, "NPP_Initialize");
    if (ptr_NPP_Initialize == NULL) {
       message (char*) malloc (65 + strlen (pluginTable [indexHandler]->pluginDL));
       sprintf (message, "relocation error: symbol not found: NPP_Initialize referenced in %s", pluginTable [indexPlug]->pluginDL);
    } else
          ret = (*ptr_NPP_Initialize) ();
    ptr_NPP_GetMIMEDescription = GetProcAdress (pluginTable [indexHandler]->pluginHandle, "NPP_GetMIMEDescription");
#   else  /* _WINDOWS */
    ptr_NPP_Initialize = (int (*) ()) dlsym (pluginTable [indexHandler]->pluginHandle, "NPP_Initialize");
    message = (char*) dlerror ();    
    if (message) 
       printf ("ERROR at Initialization: %s\n", message);

    ret = (*ptr_NPP_Initialize) ();
    ptr_NPP_GetMIMEDescription = (int(*) ()) dlsym (pluginTable [indexHandler]->pluginHandle, "NPP_GetMIMEDescription");
#   endif /* _WINDOWS */

    pluginMimeType = (NPMIMEType) (*ptr_NPP_GetMIMEDescription) ();
    pluginTable [indexHandler]->pluginMimeType = (char*) malloc (strlen (pluginMimeType) + 1);
    strcpy (pluginTable [indexHandler]->pluginMimeType, pluginMimeType);
    /* printf ("Mime: %s\n", pluginMimeType); */
    ParseMIMEType (indexHandler, pluginMimeType);
    while (pluginTable [indexHandler]->fileExt [index1] != ',' && pluginTable [indexHandler]->fileExt [index1] != '\0')
          GUI_Name [index1] = pluginTable [indexHandler]->fileExt [index1++];
    GUI_Name [index1++] = ' ';
    GUI_Name [index1++] = '(';
    GUI_Name [index1++] = '.';
    while (pluginTable [indexHandler]->fileExt [index2] != ',' && pluginTable [indexHandler]->fileExt [index2] != '\0')
          GUI_Name [index1++] = pluginTable [indexHandler]->fileExt [index2++];
    GUI_Name [index1++] = ')';
    GUI_Name [index1] = '\0';

    /* printf ("GUI_Name : %s\n", GUI_Name); */
    
    strncpy (PictureHandlerTable[HandlersCounter].GUI_Name, GUI_Name, MAX_FORMAT_NAMELENGHT);
    /* Initializing the pointers to the netscape functions */
    Ap_InitializePluginTable (indexHandler);
    
    return 0;
}

/*----------------------------------------------------------------------
  CreateInstance
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void Ap_CreatePluginInstance (PtrBox box, PictInfo *imageDesc, Display *display, int type) 
#else  /* __STDC__ */
void Ap_CreatePluginInstance (imageDesc, display, type)
PictInfo* imageDesc;
Display*  display; 
int       type;
#endif /* __STDC__ */
{

    ElementType   elType;
    PtrElement    elem;
    Element       object;
    Element       param;
    AttributeType attrTypeN, attrTypeV;
    Attribute     attrN, attrV;
    int           length;
    NPStream*     stream;
    NPWindow*     pwindow;
    char          widthText[10], heightText[10];
    char*         argn[20], *argv[20];
    char*         url;
    uint16        stype;
    int           ret;
    int16         argc; 
    struct stat   sbuf;
     
#   ifdef PLUGIN_DEBUG
    printf ("***** Ap_CreatePluginInstance *****\n");
#   endif

    argn[0] = "SRC";
    argn[1] = "WIDTH";
    argn[2] = "HEIGHT";

    sprintf (widthText, "%d", imageDesc->PicWArea);
    sprintf (heightText, "%d", imageDesc->PicHArea);
    argv[0] = imageDesc->PicFileName;
    argv[1] = widthText;
    argv[2] = heightText;

    argc    = 3; 

    elem    = (PtrElement) box->BxAbstractBox->AbElement;
    object  = TtaGetParent ((Element) elem);
    elType  = TtaGetElementType (object);

    if (elType.ElTypeNum == HTML_EL_Object) {
       elType.ElTypeNum = HTML_EL_Parameter;
       param = TtaSearchTypedElement (elType, SearchInTree, object);
       if (param) {
	  attrTypeN.AttrSSchema = elType.ElSSchema;
	  attrTypeN.AttrTypeNum = HTML_ATTR_Param_name;
	  attrTypeV.AttrSSchema = elType.ElSSchema;
	  attrTypeV.AttrTypeNum = HTML_ATTR_Param_value;
	  while (param && TtaIsAncestor (param, object)) {	        
	        attrN = TtaGetAttribute (param, attrTypeN);
		length = TtaGetTextAttributeLength (attrN);
		argn[argc] = (char*) TtaGetMemory (length + 1);
		TtaGiveTextAttributeValue (attrN, argn[argc], &length);
		
		attrV = TtaGetAttribute (param, attrTypeV);
		length = TtaGetTextAttributeLength (attrV);
		argv[argc] = (char*) TtaGetMemory (length + 1);
		TtaGiveTextAttributeValue (attrV, argv[argc], &length);
		argc++;
	        param = TtaSearchTypedElement (elType, SearchForward, param);
	  }
       }
    }

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
#   ifdef XP_UNIX
    pwindow->ws_info = (NPSetWindowCallbackStruct*) malloc (sizeof (NPSetWindowCallbackStruct));
#   endif /* XP_UNIX */
    
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->display  = display;
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->visual   = DefaultVisual(display, DefaultScreen (display));
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->colormap = DefaultColormap(display, DefaultScreen (display));
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->depth    = DefaultDepth(display, DefaultScreen (display));
    ((NPSetWindowCallbackStruct*) (pwindow->ws_info))->type     = 0;

    url = (char*) malloc (strlen (imageDesc->PicFileName) + 1);
    strcpy (url, imageDesc->PicFileName);
    
    (NPP) (imageDesc->pluginInstance) = (NPP) malloc (sizeof (NPP_t)); 
    (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->newp)) 
                   (pluginTable [currentExtraHandler]->pluginMimeType, 
                    (NPP)(imageDesc->pluginInstance), type,  argc,  argn, argv, NULL);

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

#   ifdef PLUGIN_DEBUG
    printf ("Stype : %d\n", stype);
#   endif

    switch (stype) {
           case NP_NORMAL:     
                Ap_Normal ((NPP) (imageDesc->pluginInstance), stream, url); 
                break;
           case NP_ASFILEONLY: 
                (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->asfile)) ((NPP)(imageDesc->pluginInstance), stream, url);
                break;
	   case NP_ASFILE:     
                Ap_AsFile ((NPP) (imageDesc->pluginInstance), stream, url);
                break;
           case NP_SEEK:
                break;
           default:            
                break;
    }

    (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->asfile)) ((NPP)(imageDesc->pluginInstance), stream, url);
    (*(pluginTable [currentExtraHandler]->pluginFunctionsTable->destroystream)) ((NPP)(imageDesc->pluginInstance), stream, NPRES_DONE);
 }

