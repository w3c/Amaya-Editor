/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
   nodialog : fonctions vides pour l'edition de lien
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "frame.h"

#include "memory_f.h"

/* Les fonctions vides pour l'edition de lien */
#ifdef __STDC__
Pixmap              TtaCreatePixmapLogo (STRING *data)
#else  /* __STDC__ */
Pixmap              TtaCreatePixmapLogo (data)
STRING             *data;
#endif /* __STDC__ */
{
   return None;
}


#ifdef __STDC__
void                ConfigTranslateSSchema (PtrSSchema pSS)
#else  /* __STDC__ */
void                ConfigTranslateSSchema (pSS)
PtrSSchema          pSS;
#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                DrawAddAttr (PtrAttribute * pAttr, PtrElement pEl)
#else  /* __STDC__ */
void                DrawAddAttr (pAttr, pEl)
PtrAttribute       *pAttr;
PtrElement          pEl;
#endif /* __STDC__ */
{
}

void                RedisplayCopies ()
{
}

#ifdef __STDC__
void                DrawChar (UCHAR_T car, int frame, int x, int y, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawChar (car, frame, x, y, font, RO, active, fg)
UCHAR_T             car;
int                 frame;
int                 x;
int                 y;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;
#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                DrawSupprAttr (PtrAttribute pAttr, PtrElement pEl)
#else  /* __STDC__ */
void                DrawSupprAttr (pAttr, pEl)
PtrAttribute        pAttr;
PtrElement          pEl;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                AttachAttrWithValue (PtrElement pEl, PtrDocument pDoc, PtrAttribute pNewAttr)
#else  /* __STDC__ */
void                AttachAttrWithValue (pEl, pDoc, pNewAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pNewAttr;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
ThotBool            IsASavedElement (PtrElement pEl)
#else  /* __STDC__ */
ThotBool            IsASavedElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   return FALSE;
}

ThotBool            LinkReference ()
{
   return FALSE;
}

#ifdef __STDC__
void                NewPosition (PtrAbstractBox pAb, int X, int Y, int frame, ThotBool Disp)
#else  /* __STDC__ */
void                NewPosition (pAb, X, Y, frame, Disp)
PtrAbstractBox      pAb;
int                 X;
int                 Y;
int                 frame;
ThotBool            Disp;

#endif /* __STDC__ */
{
}


#ifdef __STDC__
ThotBool            BothHaveNoSpecRules (PtrElement pEl1, PtrElement pEl2)
#else  /* __STDC__ */
ThotBool            BothHaveNoSpecRules (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */
{
   return TRUE;
}

#ifdef __STDC__
void                ModPresent (ThotBool OnlyChars)
#else  /* __STDC__ */
void                ModPresent (OnlyChars)
ThotBool            OnlyChars;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ShowBox (int frame, PtrBox boite, int position, int pourcent)
#else  /* __STDC__ */
void                ShowBox (frame, boite, position, pourcent)
int                 frame;
PtrBox              boite;
int                 position;
int                 pourcent;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
int                 IdentDocument (PtrDocument pDoc)
#else  /* __STDC__ */
int                 IdentDocument (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   return 1;
}

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

#ifdef __STDC__
void                CloseParagraphInsertion (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
void                CloseParagraphInsertion (pAb, frame)
PtrAbstractBox      pAb;
int                 frame;

#endif /* __STDC__ */
{
}


#ifdef __STDC__
void                UpdateScrollbars (int frame)
#else  /* __STDC__ */
void                UpdateScrollbars (frame)
int                 frame;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ChangeFrameTitle (int frame, STRING texte)
#else  /* __STDC__ */
void                ChangeFrameTitle (frame, texte)
int                 frame;
STRING              texte;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                SetCursorWatch (int thotWindowid)
#else  /* __STDC__ */
void                SetCursorWatch (thotWindowid)
int                 thotWindowid;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ResetCursorWatch (int thotWindowid)
#else  /* __STDC__ */
void                ResetCursorWatch (thotWindowid)
int                 thotWindowid;

#endif /* __STDC__ */
{
}

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

#ifdef __STDC__
void                ShowSelection (PtrAbstractBox PavRac, ThotBool Visible)
#else  /* __STDC__ */
void                ShowSelection (PavRac, Visible)
PtrAbstractBox      PavRac;
ThotBool            Visible;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                SwitchSelection (int frame, ThotBool Allume)
#else  /* __STDC__ */
void                SwitchSelection (frame, Allume)
int                 frame;
ThotBool            Allume;

#endif /* __STDC__ */
{
}

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

#ifdef __STDC__
void                AddInSelection (PtrElement pEl, ThotBool dernier)

#else  /* __STDC__ */
void                AddInSelection (pEl, dernier)
PtrElement          pEl;
ThotBool            dernier;

#endif /* __STDC__ */

{
}

#ifdef __STDC__
void                ClearViewSelection (int frame)
#else  /* __STDC__ */
void                ClearViewSelection (frame)
int                 frame;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                TtaClearViewSelections ()
#else  /* __STDC__ */
void                TtaClearViewSelections ()

#endif /* __STDC__ */
{
}

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

#ifdef __STDC__
void                ComputeViewSelMarks (ViewSelection * marque)
#else  /* __STDC__ */
void                ComputeViewSelMarks (marque)
ViewSelection      *marque;

#endif /* __STDC__ */
{
}


#ifdef __STDC__
void                HighlightSelection (ThotBool DebVisible, ThotBool clearOldSel)
#else  /* __STDC__ */
void                HighlightSelection (DebVisible, clearOldSel)
ThotBool            DebVisible;
ThotBool            clearOldSel;
#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                CancelSelection ()

#else  /* __STDC__ */
void                CancelSelection ()
#endif				/* __STDC__ */
{
}


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

#ifdef __STDC__
void                DirectCreation (PtrBox pBox, int frame)
#else  /* __STDC__ */
void                DirectCreation (pBox, frame)
PtrBox              pBox;
int                 frame;

#endif /* __STDC__ */
{
}

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

#ifdef __STDC__
void         SetMainWindowBackgroundColor (int frame, int color)
#else  /* __STDC__ */
void         SetMainWindowBackgroundColor (frame, color)
int          frame;
int          color;
#endif /* __STDC__ */
{
}

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

#ifdef __STDC__
ThotBool            ConfigGetPSchemaNature (PtrSSchema pSS, STRING nomNature, STRING presNature)
#else  /* __STDC__ */
ThotBool            ConfigGetPSchemaNature (pSS, nomNature, presNature)
PtrSSchema          pSS;
STRING              nomNature;
STRING              presNature;

#endif /* __STDC__ */
{
   return FALSE;
}


#ifdef __STDC__
ThotBool            ConfigDefaultPSchema (STRING schema, STRING schpres)

#else  /* __STDC__ */
ThotBool            ConfigDefaultPSchema (schema, schpres)
STRING              schema;
STRING              schpres;

#endif /* __STDC__ */
{
   return FALSE;
}
