/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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
   GetObjectWWW
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int GetObjectWWW (int docid, STRING urlName, STRING formdata,
		  STRING outputfile, int mode, void *incremental_cbf, 
		  void* context_icbf, void *terminate_cbf, 
		  void* context_tcbf, ThotBool error_html, STRING content_type)
#else
int GetObjectWWW (docid, urlName, formdata, outputfile, mode, 
		  incremental_cbf, context_icbf, 
		  terminate_cbf, context_tcbf, error_html, content_type)
int           docid;
STRING        urlName;
STRING        formdata;
STRING        outputfile;
int           mode;
void         *incremental_cbf;
void         *context_icbf;
void         *terminate_cbf;
void         *context_tcbf;
ThotBool      error_html;
STRING        content_type;
#endif
{
  return 0;
}

/*----------------------------------------------------------------------
  IsW3Path                                           
  returns TRUE if path is in fact a URL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool             IsW3Path (const CHAR_T* path)
#else  /* __STDC__ */
ThotBool             IsW3Path (path)
const CHAR_T*        path;
#endif /* __STDC__ */
{
  return FALSE;
}

/*----------------------------------------------------------------------
   NormalizeURL
   The function returns the new complete and normalized URL 
   or file name path (newName) and the name of the document (docName).        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                NormalizeURL (CHAR_T* orgName, Document doc, CHAR_T* newName, CHAR_T* docName, CHAR_T* otherPath)
#else  /* __STDC__ */
void                NormalizeURL (orgName, doc, newName, docName, otherPath)
STRING              orgName;
Document            doc;
STRING              newName;
STRING              docName;
STRING              otherPath;
#endif /* __STDC__ */
{
  TtaExtractName (orgName, newName, docName);
}

/*----------------------------------------------------------------------
   LoadRemoteStyleSheet loads a remote style sheet into a file.
   Return FALSE if it's a local file and TRUE otherwise.
   When returning, the parameter completeURL contains the normalized url
   and the parameter localfile the path of the local copy of the file.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool        LoadRemoteStyleSheet (STRING url, Document doc, Element el, void *css, STRING completeURL, STRING localfile)
#else
ThotBool        LoadRemoteStyleSheet (url, doc, el, css, completeURL, localfile)
STRING          url;
Document        doc;
Element         el;
void           *css;
#endif
{
  ustrcpy (completeURL, url);
  ustrcpy (localfile, url);
  return FALSE;
}

/*----------------------------------------------------------------------
   FetchImage loads an IMG from local file or from the web. The flags
   may indicate extra transfer parameters, for example bypassing the cache.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FetchImage (Document doc, Element el, STRING URL, int flags, Proc callback, void *extra)
#else  /* __STDC__ */
void                FetchImage (doc, el, URL, flags, callback, extra)
Document            doc;
Element             el;
STRING              URL;
int                 flags;
Proc                callback;
void               *extra;

#endif /* __STDC__ */
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
#ifdef __STDC__
Pixmap              TtaCreatePixmapLogo (char** data)
#else  /* __STDC__ */
Pixmap              TtaCreatePixmapLogo (data)
char**              data;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                ConfigTranslateSSchema (PtrSSchema pSS)
#else  /* __STDC__ */
void                ConfigTranslateSSchema (pSS)
PtrSSchema          pSS;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ConfigGetPSchemaNature (PtrSSchema pSS, CHAR_T* nomNature, CHAR_T* presNature)
#else  /* __STDC__ */
ThotBool            ConfigGetPSchemaNature (pSS, nomNature, presNature)
PtrSSchema          pSS;
CHAR_T*             nomNature;
CHAR_T*             presNature;
#endif /* __STDC__ */
{
   return FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            ConfigDefaultPSchema (CHAR_T* schema, CHAR_T* schpres)
#else  /* __STDC__ */
ThotBool            ConfigDefaultPSchema (schema, schpres)
CHAR_T*             schema;
CHAR_T*             schpres;
#endif /* __STDC__ */
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
#ifdef __STDC__
void                DrawAddAttr (PtrAttribute * pAttr, PtrElement pEl)
#else  /* __STDC__ */
void                DrawAddAttr (pAttr, pEl)
PtrAttribute       *pAttr;
PtrElement          pEl;
#endif /* __STDC__ */
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
#ifdef __STDC__
ThotBool            IsASavedElement (PtrElement pEl)
#else  /* __STDC__ */
ThotBool            IsASavedElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            CallEventType (int *notifyEvent, ThotBool pre)
#else  /* __STDC__ */
ThotBool            CallEventType (notifyEvent, pre)
int                *notifyEvent;
ThotBool            pre;

#endif /* __STDC__ */
{
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            CallEventAttribute (NotifyAttribute * notifyAttr, ThotBool pre)
#else  /* __STDC__ */
ThotBool            CallEventAttribute (notifyAttr, pre)
NotifyAttribute    *notifyAttr;
ThotBool            pre;

#endif /* __STDC__ */
{
   return FALSE;
}


/*----------------------------------------------------------------------
   TtaSetStatus affiche le status de la vue du document.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetStatus (Document document, View view, CONST STRING text, CONST STRING name)
#else  /* __STDC__ */
void                TtaSetStatus (document, view, text, name)
Document            document;
View                view;
CONST STRING        text;
CONST STRING        name;
#endif /* __STDC__ */
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
#ifdef __STDC__
void         SwitchPaste (PtrDocument pDoc, ThotBool on)
#else  /* __STDC__ */
void         SwitchPaste (pDoc, on)
PtrDocument  pDoc;
ThotBool     on;
#endif /* __STDC__ */
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateScrollbars (int frame)
#else  /* __STDC__ */
void                UpdateScrollbars (frame)
int                 frame;

#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeFrameTitle (int frame, STRING texte)
#else  /* __STDC__ */
void                ChangeFrameTitle (frame, texte)
int                 frame;
STRING              texte;

#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetCursorWatch (int thotWindowid)
#else  /* __STDC__ */
void                SetCursorWatch (thotWindowid)
int                 thotWindowid;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ResetCursorWatch (int thotWindowid)
#else  /* __STDC__ */
void                ResetCursorWatch (thotWindowid)
int                 thotWindowid;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InsertOption (PtrElement pEl, PtrElement * p, PtrDocument pDoc)

#else  /* __STDC__ */
void                InsertOption (pEl, p, pDoc)
PtrElement          pEl;
PtrElement         *p;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ShowSelection (PtrAbstractBox PavRac, ThotBool Visible)
#else  /* __STDC__ */
void                ShowSelection (PavRac, Visible)
PtrAbstractBox      PavRac;
ThotBool            Visible;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          NextInSelection (PtrElement pEl, PtrElement PcLast)
#else  /* __STDC__ */
PtrElement          NextInSelection (pEl, PcLast)
PtrElement          pEl;
PtrElement          PcLast;
#endif /* __STDC__ */
{
   return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AddInSelection (PtrElement pEl, ThotBool dernier)
#else  /* __STDC__ */
void                AddInSelection (pEl, dernier)
PtrElement          pEl;
ThotBool            dernier;
#endif /* __STDC__ */

{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ClearViewSelection (int frame)
#else  /* __STDC__ */
void                ClearViewSelection (frame)
int                 frame;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaClearViewSelections ()
#else  /* __STDC__ */
void                TtaClearViewSelections ()
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectElement (PtrDocument pDoc, PtrElement pEl, ThotBool Debut, ThotBool Controle)
#else  /* __STDC__ */
void                SelectElement (pDoc, pEl, Debut, Controle)
PtrDocument         pDoc;
PtrElement          pEl;
ThotBool            Debut;
ThotBool            Controle;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ComputeViewSelMarks (ViewSelection * marque)
#else  /* __STDC__ */
void                ComputeViewSelMarks (marque)
ViewSelection      *marque;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HighlightSelection (ThotBool DebVisible, ThotBool clearOldSel)
#else  /* __STDC__ */
void                HighlightSelection (DebVisible, clearOldSel)
ThotBool            DebVisible;
ThotBool            clearOldSel;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CancelSelection ()
#else  /* __STDC__ */
void                CancelSelection ()
#endif				/* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SelectRange (PtrDocument SelDoc, PtrElement PremSel, PtrElement DerSel, int premcar, int dercar)
#else  /* __STDC__ */
void                SelectRange (SelDoc, PremSel, DerSel, premcar, dercar)
PtrDocument         SelDoc;
PtrElement          PremSel;
PtrElement          DerSel;
int                 premcar;
int                 dercar;
#endif /* __STDC__ */
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetNewSelectionStatus (int frame, PtrAbstractBox pAb, ThotBool Etat)
#else  /* __STDC__ */
void                SetNewSelectionStatus (frame, pAb, Etat)
int                 frame;
PtrAbstractBox      pAb;
ThotBool            Etat;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DirectCreation (PtrBox pBox, int frame)
#else  /* __STDC__ */
void                DirectCreation (pBox, frame)
PtrBox              pBox;
int                 frame;

#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            IsAbstractBoxDisplayed (PtrAbstractBox pav, int frame)
#else  /* __STDC__ */
ThotBool            IsAbstractBoxDisplayed (pav, frame)
PtrAbstractBox      pav;
int                 frame;
#endif /* __STDC__ */
{
   return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         SetMainWindowBackgroundColor (int frame, int color)
#else  /* __STDC__ */
void         SetMainWindowBackgroundColor (frame, color)
int          frame;
int          color;
#endif /* __STDC__ */
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                Clear (int frame, int larg, int haut, int x, int y)
#else  /* __STDC__ */
void                Clear (frame, larg, haut, x, y)
int                 frame;
int                 larg;
int                 haut;
int                 x;
int                 y;
#endif /* __STDC__ */
{
}
