/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
   nodialog : empty functions for building the process print
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "application.h"
#include "memory_f.h"

/*----------------------------------------------------------------------
  DisplayPRule displays the presentation rule pointed by RP.
  ----------------------------------------------------------------------*/
void DisplayPRule (PtrPRule RP, FILE *fileDescriptor,
		   PtrElement pEl, PtrPSchema pSchP)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ShowBox (int frame, PtrBox pBox, int position, int percent)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ExtendSelection (PtrElement pEl, int rank, ThotBool fixed, ThotBool begin,
		      ThotBool drag)
{
}

/*----------------------------------------------------------------------
   ConfigFree
   Frees the table entries of all the document types, natures and
   extensions.
  ----------------------------------------------------------------------*/
void ConfigFree (void)
{
}

/*----------------------------------------------------------------------
   TtaFreeAllCatalogs frees the memory associated with catalogs.                      
  ----------------------------------------------------------------------*/
void TtaFreeAllCatalogs (void)
{
}

/*----------------------------------------------------------------------
   InitDialogueFont initializes the dialogue font
  ----------------------------------------------------------------------*/
void InitDialogueFont ()
{
}

/*----------------------------------------------------------------------
   TteFreeAllEventsList frees all event lists and the event/action
   context pointers.			                        
  ----------------------------------------------------------------------*/
void TteFreeAllEventsList (void)
{
}

/*----------------------------------------------------------------------
   GetObjectWWW
  ----------------------------------------------------------------------*/
int GetObjectWWW (int docid, char *urlName, char *formdata,
		  char *outputfile, int mode, void *incremental_cbf, 
		  void* context_icbf, void *terminate_cbf, 
		  void* context_tcbf, ThotBool error_html, char *content_type)
{
  return 0;
}

/*----------------------------------------------------------------------
  IsW3Path                                           
  returns TRUE if path is in fact a URL.
  ----------------------------------------------------------------------*/
ThotBool             IsW3Path (const char* path)
{
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CloseTextInsertion ()
{
}

/*----------------------------------------------------------------------
   NormalizeURL
   The function returns the new complete and normalized URL 
   or file name path (newName) and the name of the document (docName).        
  ----------------------------------------------------------------------*/
void NormalizeURL (char* orgName, Document doc, char* newName, char* docName, char* otherPath)
{
  TtaExtractName (orgName, newName, docName);
}

/*----------------------------------------------------------------------
   LoadRemoteStyleSheet loads a remote style sheet into a file.
   Return FALSE if it's a local file and TRUE otherwise.
   When returning, the parameter completeURL contains the normalized url
   and the parameter localfile the path of the local copy of the file.
  ----------------------------------------------------------------------*/
ThotBool LoadRemoteStyleSheet (char *url, Document doc, Element el, void *css, char *completeURL, char *localfile)
{
  strcpy (completeURL, url);
  strcpy (localfile, url);
  return FALSE;
}

/*----------------------------------------------------------------------
   FetchImage loads an IMG from local file or from the web. The flags
   may indicate extra transfer parameters, for example bypassing the cache.		
  ----------------------------------------------------------------------*/
void FetchImage (Document doc, Element el, char *URL, int flags, Proc callback, void *extra)
{
  if (callback && URL)
    callback (doc, el, URL, extra);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Prof_FreeTable ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Pixmap TtaCreatePixmapLogo (char** data)
{
   return None;
}

/*----------------------------------------------------------------------
  TtaHandlePendingEvents
  Processes all pending events in an application.
  ----------------------------------------------------------------------*/
void TtaHandlePendingEvents ()
{
}


/*----------------------------------------------------------------------
   ConfigInit initializes the configuration module
  ----------------------------------------------------------------------*/
void ConfigInit ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ConfigTranslateSSchema (PtrSSchema pSS)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool ConfigGetPSchemaNature (PtrSSchema pSS, char* nomNature, char* presNature)
{
   return FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool ConfigDefaultPSchema (char* schema, char* schpres)
{
   return FALSE;
}
/*----------------------------------------------------------------------
   FreeTranslations remove all translation structures.
  ----------------------------------------------------------------------*/
void FreeTranslations ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void RedisplayCopies ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool IsASavedElement (PtrElement pEl)
{
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool CallEventType (int *notifyEvent, ThotBool pre)
{
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool CallEventAttribute (NotifyAttribute * notifyAttr, ThotBool pre)
{
   return FALSE;
}


/*----------------------------------------------------------------------
   TtaSetStatus affiche le status de la vue du document.                      
  ----------------------------------------------------------------------*/
void TtaSetStatus (Document document, View view, char *text, char *name)
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FreeMenus ()
{
}

/*----------------------------------------------------------------------
  SwitchPaste enables (on=TRUE) or disables (on=FALSE) the Paste
  entry in all frames.
  ----------------------------------------------------------------------*/
void SwitchPaste (PtrDocument pDoc, ThotBool on)
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void UpdateScrollbars (int frame)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ChangeFrameTitle (int frame, char *texte, CHARSET encoding)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SetCursorWatch (int thotWindowid)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ResetCursorWatch (int thotWindowid)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InsertOption (PtrElement pEl, PtrElement *p, PtrDocument pDoc)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ShowSelection (PtrAbstractBox PavRac, ThotBool Visible)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
PtrElement NextInSelection (PtrElement pEl, PtrElement PcLast)
{
   return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AddInSelection (PtrElement pEl, ThotBool dernier)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ClearViewSelection (int frame)
{
}

/* ----------------------------------------------------------------------
   ---------------------------------------------------------------------- */
int TtaGetElementLineNumber (Element element)
{
  return 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void TtaClearViewSelections ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SelectElement (PtrDocument pDoc, PtrElement pEl, ThotBool Debut, ThotBool Controle)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ComputeViewSelMarks (ViewSelection * marque)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void HighlightSelection (ThotBool DebVisible, ThotBool clearOldSel)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CancelSelection ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SelectRange (PtrDocument SelDoc, PtrElement PremSel, PtrElement DerSel, int premcar, int dercar)
{
}

/*----------------------------------------------------------------------
  DisplayPointSelection draw characteristics point of the box.
  ----------------------------------------------------------------------*/
void DisplayPointSelection (int frame, PtrBox pBox, int pointselect)
{
}

/*----------------------------------------------------------------------
  DisplayBgBoxSelection paints the box background with the selection
  color.
  ----------------------------------------------------------------------*/
void DisplayBgBoxSelection (int frame, PtrBox pBox)
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SetNewSelectionStatus (int frame, PtrAbstractBox pAb, ThotBool Etat)
{
}


/*----------------------------------------------------------------------
  DisplayStringSelection the selection on a substring of text
  between leftX and rightX.
  t, b, l, and r give top, bottom, left and right extra margins.
  ----------------------------------------------------------------------*/
void DisplayStringSelection (int frame, int leftX, int rightX, PtrBox pBox,
			     int t, int b, int l, int r)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void DirectCreation (PtrBox pBox, int frame)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool IsAbstractBoxDisplayed (PtrAbstractBox pav, int frame)
{
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SetMainWindowBackgroundColor (int frame, int color)
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Clear (int frame, int larg, int haut, int x, int y)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
PtrElement CreateSibling (PtrDocument pDoc, PtrElement pEl, ThotBool before, ThotBool createAbsBox, int typeNum, PtrSSchema pSS, ThotBool inclusion)
{
  return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CloseAttributeDialogues (PtrDocument pDoc)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool OpenParsingErrors (Document document)
{
  return TRUE;
}
