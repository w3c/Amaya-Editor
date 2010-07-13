/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
   nodialog : empty functions/globals variables for building the process print
 */
#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */


#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"
#include "application.h"
#include "memory_f.h"
#include "nodialog.h"
#include "style.h"


/*----------------------------------------------------------------------
  IsTemplateInstanceDocument: Test if a document is a template instance
  doc : Document to test
  return : TRUE if the document is a template instance
  ----------------------------------------------------------------------*/
ThotBool IsTemplateInstanceDocument(Document doc)
{
  return FALSE;
}

/*----------------------------------------------------------------------
  TtaGetPixelValue converts a logical value into a pixel value for
  a given element.
  ----------------------------------------------------------------------*/
int TtaGetPixelValue (int val, int unit, Element element, Document document)
{
  return 0;
}

/*----------------------------------------------------------------------
  TtaGetLogicalValue converts a pixel value into a logical value for
  a given element.
  ----------------------------------------------------------------------*/
int TtaGetLogicalValue (int val, int unit, Element element, Document document)
{
  return 0;
}

/*----------------------------------------------------------------------
  TtaExecuteMenuAction execute the corresponding menu action.
  ----------------------------------------------------------------------*/
void TtaExecuteMenuAction (const char *actionName, Document doc, View view,
			   ThotBool force)
{
}

/*----------------------------------------------------------------------
  FindMenuAction returns the entry that describes the menu action.
  ----------------------------------------------------------------------*/
int FindMenuAction ( const char *actionName )
{
  return 0;
}

/*----------------------------------------------------------------------
  TtaPostMenuAction execute the corresponding menu action when idle.
  ----------------------------------------------------------------------*/
void TtaPostMenuAction (const char *actionName, Document doc, View view,
                           ThotBool force)
{
}


/*----------------------------------------------------------------------
  TtaPostMenuActionById execute the corresponding menu action when idle.
  ----------------------------------------------------------------------*/
void TtaPostMenuActionById (int actionId, Document doc, View view,
                           ThotBool force)
{
}

/*----------------------------------------------------------------------
  TtaExecuteMenuActionFromActionId execute the corresponding menu action.
  When force is TRUE the action is called even if it's not active.
  ----------------------------------------------------------------------*/
void TtaExecuteMenuActionFromActionId (int action_id, Document doc,
                                       View view, ThotBool force)
{
}

/*----------------------------------------------------------------------
  DisplayCounterRule displays an item of a counter in the CSS format.
  ----------------------------------------------------------------------*/
void DisplayCounterRule (int counter, int item, FILE *fileDescriptor,
			 PtrElement pEl, PtrPSchema pSchP)
{
}

/*----------------------------------------------------------------------
  DisplayPRule displays the presentation rule pointed by RP.
  ----------------------------------------------------------------------*/
void DisplayPRule (PtrPRule RP, FILE *fileDescriptor,
		   PtrElement pEl, PtrPSchema pSchP, int indent)
{
}

/*----------------------------------------------------------------------
  DisplayStyleValue: update the property in Style dialog
  ----------------------------------------------------------------------*/
void  DisplayStyleValue (const char *property, const char *start_value, char *end_value)
{
}

/*----------------------------------------------------------------------
   NewInitialSequence
   Mark the current sequence as the initial sequence.
  ----------------------------------------------------------------------*/
void NewInitialSequence (PtrDocument pDoc)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ShowBox (int frame, PtrBox pBox, int position, int percent,
              ThotBool updateScroll)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ExtendSelection (PtrElement pEl, int rank, ThotBool fixed, ThotBool begin,
		      ThotBool drag)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ResetSelection (PtrDocument pDoc)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SelectString (PtrDocument pDoc, PtrElement pEl, int firstChar, int lastChar)
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
int GetObjectWWW (int docid, char *urlName, const char *formdata,
		  char *outputfile, int mode, void *incremental_cbf, 
		  void* context_icbf, void *terminate_cbf, 
		  void* context_tcbf, ThotBool error_html, const char *content_type)
{
  return 0;
}

/*----------------------------------------------------------------------
  IsW3Path                                           
  returns TRUE if path is in fact a URL.
  ----------------------------------------------------------------------*/
ThotBool IsW3Path (const char* path)
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
void NormalizeURL (char* orgName, Document doc, char* newName, char* docName,
		   const char* otherPath)
{
  TtaExtractName (orgName, newName, docName);
}

/*----------------------------------------------------------------------
   LoadRemoteStyleSheet loads a remote style sheet into a file.
   Return FALSE if it's a local file and TRUE otherwise.
   When returning, the parameter completeURL contains the normalized url
   and the parameter localfile the path of the local copy of the file.
  ----------------------------------------------------------------------*/
ThotBool LoadRemoteStyleSheet (char *url, Document doc, Element el,
			       char *urlRef, char *completeURL, char *localfile)
{
  strcpy (completeURL, url);
  strcpy (localfile, url);
  return FALSE;
}

/*----------------------------------------------------------------------
   FetchImage loads an IMG from local file or from the web. The flags
   may indicate extra transfer parameters, for example bypassing the cache.		
  ----------------------------------------------------------------------*/
void FetchImage (Document doc, Element el, char *URL, int flags,
		 LoadedImageCallback callback, void *extra)
{
  if (callback && URL)
	(*callback) (
		doc,
		el,
		URL,
		extra,
		FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Prof_FreeTable ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotIcon TtaCreatePixmapLogo (char** data)
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
ThotBool ConfigGetPSchemaNature (PtrSSchema pSS, char* nomNature, char **presNature)
{
   return FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool ConfigDefaultPSchema (char* schema, char **schpres)
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
void RedisplayCopies ( PtrElement pEl,
                       PtrDocument pDoc,
                       ThotBool redisplay )
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
void FreeSavedElements ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CleanSearchContext ()
{
}
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool CallEventType (NotifyEvent * notifyEvent, ThotBool pre)
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
void TtaSetStatus (Document document, View view, const char *text, const char *name)
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
void SelectElement (PtrDocument pDoc, PtrElement pEl,  ThotBool begin,
                    ThotBool check, ThotBool withPath)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ComputeViewSelMarks (ViewSelection * marque, int frame)
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
void InsertViewSelMarks ( int frame,
                          PtrAbstractBox pAb,
                          int firstChar,
                          int lastChar,
                          ThotBool startSelection,
                          ThotBool endSelection,
                          ThotBool alone )
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SelectRange (PtrDocument SelDoc, PtrElement PremSel, PtrElement DerSel,
		  int premcar, int dercar)
{
}

/*----------------------------------------------------------------------
  DisplayPointSelection draw characteristics point of the box.
  ----------------------------------------------------------------------*/
void DisplayPointSelection (int frame, PtrBox pBox, int pointselect,
                            ThotBool could_resize)
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
  The parameter t gives the top extra margin of the box.
  ----------------------------------------------------------------------*/
void DisplayStringSelection (int frame, int leftX, int rightX, int t,
			     PtrBox pBox)
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
PtrElement CreateSibling (PtrDocument pDoc, PtrElement pEl, ThotBool before,
						  ThotBool createAbsBox, int typeNum, PtrSSchema pSS,
						  ThotBool inclusion)
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

/*----------------------------------------------------------------------
   TtaIsSelectionUnique

   Returns TRUE if there is a current selection and only one element is
   selected.
  ----------------------------------------------------------------------*/
ThotBool TtaIsSelectionUnique ()
{
  return FALSE;
}

int TtaGiveActiveFrame()
{
  return -1;
}

ThotBool GL_DrawAll()
{
  return FALSE;
}

#ifdef _WX
ThotBool TtaIsSpecialKey( int wx_keycode )
{
  return false;
}

ThotBool TtaHandleSpecialKey( wxKeyEvent& event )
{
  return false;  
}

ThotBool TtaHandleShortcutKey( wxKeyEvent& event )
{
  return false;  
}

ThotBool TtaHandleUnicodeKey( wxKeyEvent& event )
{
  return false;  
}

AmayaWindow * TtaGetActiveWindow()
{
  return NULL;
}

void TtaRedirectFocus()
{
}

void TtaSendStatsInfo()
{
}

#endif /* _WX */

ThotBool TtaDetachFrame( int frame_id )
{
  return TRUE;
}

/*----------------------------------------------------------------------
  TtaCloseAllHelpWindows Closes all opened help windows.
  ----------------------------------------------------------------------*/
void TtaCloseAllHelpWindows ()
{
}

/*----------------------------------------------------------------------
  ColApplyCSSRule
  apply a CSS rule attached to the COL or COLGROUP element el
  ----------------------------------------------------------------------*/
void ColApplyCSSRule (Element el, PresentationContext ctxt, char *cssRule, CSSInfoPtr css)
{
}

void SVGhandleFillUrl (Element el, Document doc, char *text)
{
}

char *SVGhandleStopColor (Element el, char *color)
{
  return NULL;
}

/*----------------------------------------------------------------------
  GenerateMarkers
  Apply a CSS rule marker* to element el in document doc.
  Parameter marker is the marker element to be used.
  Parameter position indicates where the marker has to be put on element pEl:
    0: all vertices
    1: start vertex
    2: mid vertices
    3: end vertex
  ----------------------------------------------------------------------*/
void GenerateMarkers (Element el, Document doc, Element marker, int position)
{
}

