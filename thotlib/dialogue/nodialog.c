/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
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
   ConfigFree
   Frees the table entries of all the document types, natures and
   extensions.
  ----------------------------------------------------------------------*/
void                ConfigFree (void)
{
}

/*----------------------------------------------------------------------
   TtaFreeAllCatalogs frees the memory associated with catalogs.                      
  ----------------------------------------------------------------------*/
void                TtaFreeAllCatalogs (void)
{
}

/*----------------------------------------------------------------------
   TteFreeAllEventsList frees all event lists and the event/action
   context pointers.			                        
  ----------------------------------------------------------------------*/
void                TteFreeAllEventsList (void)
{
}

/*----------------------------------------------------------------------
   GetObjectWWW
  ----------------------------------------------------------------------*/
int GetObjectWWW (int docid, STRING urlName, STRING formdata,
		  STRING outputfile, int mode, void *incremental_cbf, 
		  void* context_icbf, void *terminate_cbf, 
		  void* context_tcbf, ThotBool error_html, STRING content_type)
{
  return 0;
}

/*----------------------------------------------------------------------
  IsW3Path                                           
  returns TRUE if path is in fact a URL.
  ----------------------------------------------------------------------*/
ThotBool             IsW3Path (const CHAR_T* path)
{
  return FALSE;
}

/*----------------------------------------------------------------------
   NormalizeURL
   The function returns the new complete and normalized URL 
   or file name path (newName) and the name of the document (docName).        
  ----------------------------------------------------------------------*/
void                NormalizeURL (CHAR_T* orgName, Document doc, CHAR_T* newName, CHAR_T* docName, CHAR_T* otherPath)
{
  TtaExtractName (orgName, newName, docName);
}

/*----------------------------------------------------------------------
   LoadRemoteStyleSheet loads a remote style sheet into a file.
   Return FALSE if it's a local file and TRUE otherwise.
   When returning, the parameter completeURL contains the normalized url
   and the parameter localfile the path of the local copy of the file.
  ----------------------------------------------------------------------*/
ThotBool        LoadRemoteStyleSheet (STRING url, Document doc, Element el, void *css, STRING completeURL, STRING localfile)
{
  ustrcpy (completeURL, url);
  ustrcpy (localfile, url);
  return FALSE;
}

/*----------------------------------------------------------------------
   FetchImage loads an IMG from local file or from the web. The flags
   may indicate extra transfer parameters, for example bypassing the cache.		
  ----------------------------------------------------------------------*/
void                FetchImage (Document doc, Element el, STRING URL, int flags, Proc callback, void *extra)
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
Pixmap              TtaCreatePixmapLogo (char** data)
{
   return None;
}

/*----------------------------------------------------------------------
  TtaHandlePendingEvents
  Processes all pending events in an application.
  ----------------------------------------------------------------------*/
void                TtaHandlePendingEvents ()
{
}


/*----------------------------------------------------------------------
   ConfigInit initializes the configuration module
  ----------------------------------------------------------------------*/
void                ConfigInit ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                ConfigTranslateSSchema (PtrSSchema pSS)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool            ConfigGetPSchemaNature (PtrSSchema pSS, CHAR_T* nomNature, CHAR_T* presNature)
{
   return FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool            ConfigDefaultPSchema (CHAR_T* schema, CHAR_T* schpres)
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
void                DrawAddAttr (PtrAttribute * pAttr, PtrElement pEl)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                RedisplayCopies ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DrawSupprAttr (PtrAttribute pAttr, PtrElement pEl)
#else  /* __STDC__ */
void                DrawSupprAttr (pAttr, pEl)
PtrAttribute        pAttr;
PtrElement          pEl;

#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool            IsASavedElement (PtrElement pEl)
{
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool            CallEventType (int *notifyEvent, ThotBool pre)
{
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool            CallEventAttribute (NotifyAttribute * notifyAttr, ThotBool pre)
{
   return FALSE;
}


/*----------------------------------------------------------------------
   TtaSetStatus affiche le status de la vue du document.                      
  ----------------------------------------------------------------------*/
void                TtaSetStatus (Document document, View view, CONST STRING text, CONST STRING name)
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void    FreeMenus ()
{
}

/*----------------------------------------------------------------------
  SwitchPaste enables (on=TRUE) or disables (on=FALSE) the Paste
  entry in all frames.
  ----------------------------------------------------------------------*/
void         SwitchPaste (PtrDocument pDoc, ThotBool on)
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                UpdateScrollbars (int frame)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                ChangeFrameTitle (int frame, STRING texte)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                SetCursorWatch (int thotWindowid)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                ResetCursorWatch (int thotWindowid)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                InsertOption (PtrElement pEl, PtrElement * p, PtrDocument pDoc)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                ShowSelection (PtrAbstractBox PavRac, ThotBool Visible)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
PtrElement          NextInSelection (PtrElement pEl, PtrElement PcLast)
{
   return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                AddInSelection (PtrElement pEl, ThotBool dernier)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                ClearViewSelection (int frame)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                TtaClearViewSelections ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                SelectElement (PtrDocument pDoc, PtrElement pEl, ThotBool Debut, ThotBool Controle)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                ComputeViewSelMarks (ViewSelection * marque)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                HighlightSelection (ThotBool DebVisible, ThotBool clearOldSel)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                CancelSelection ()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                SelectRange (PtrDocument SelDoc, PtrElement PremSel, PtrElement DerSel, int premcar, int dercar)
{
}

/*----------------------------------------------------------------------
  DisplayPointSelection draw characteristics point of the box.
  ----------------------------------------------------------------------*/
void         DisplayPointSelection (int frame, PtrBox pBox, int pointselect)
{
}

/*----------------------------------------------------------------------
  DisplayBgBoxSelection paints the box background with the selection
  color.
  ----------------------------------------------------------------------*/
void         DisplayBgBoxSelection (int frame, PtrBox pBox)
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                SetNewSelectionStatus (int frame, PtrAbstractBox pAb, ThotBool Etat)
{
}


/*----------------------------------------------------------------------
  DisplayStringSelection the selection on a substring of text
  between leftX and rightX.
  ----------------------------------------------------------------------*/
void         DisplayStringSelection (int frame, int leftX, int rightX, PtrBox pBox)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                DirectCreation (PtrBox pBox, int frame)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool            IsAbstractBoxDisplayed (PtrAbstractBox pav, int frame)
{
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void         SetMainWindowBackgroundColor (int frame, int color)
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                Clear (int frame, int larg, int haut, int x, int y)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
PtrElement          CreateSibling (PtrDocument pDoc, PtrElement pEl, ThotBool before, ThotBool createAbsBox, int typeNum, PtrSSchema pSS, ThotBool inclusion)
{
  return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void                CloseAttributeDialogues (PtrDocument pDoc)
{
}
