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
#include "frame.h"

#include "memory_f.h"

/* Les fonctions vides pour l'edition de lien */


#ifdef __STDC__
Pixmap              TtaCreatePixmapLogo (char **data)
#else  /* __STDC__ */
Pixmap              TtaCreatePixmapLogo (data)
char              **data;

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

void                RedisplayCopies ()
{
}

void                DrawAddAttr ()
{
}
#ifdef __STDC__
void                DrawChar (unsigned char car, int frame, int x, int y, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                DrawChar (car, frame, x, y, font, RO, active, fg)
unsigned char       car;
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
boolean             IsASavedElement (PtrElement pEl)
#else  /* __STDC__ */
boolean             IsASavedElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   return FALSE;
}

boolean             LinkReference ()
{
   return FALSE;
}

#ifdef __STDC__
void                NewPosition (PtrAbstractBox pAb, int DeltaX, int DeltaY, int frame, boolean Disp)
#else  /* __STDC__ */
void                NewPosition (pAb, DeltaX, DeltaY, frame, Disp)
PtrAbstractBox      pAb;
int                 DeltaX;
int                 DeltaY;
int                 frame;
boolean             Disp;

#endif /* __STDC__ */
{
}


#ifdef __STDC__
boolean             BothHaveNoSpecRules (PtrElement pEl1, PtrElement pEl2)
#else  /* __STDC__ */
boolean             BothHaveNoSpecRules (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */
{
   return TRUE;
}

#ifdef __STDC__
void                ModPresent (boolean OnlyChars)
#else  /* __STDC__ */
void                ModPresent (OnlyChars)
boolean             OnlyChars;

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
boolean             CallEventType (int *notifyEvent, boolean pre)
#else  /* __STDC__ */
boolean             CallEventType (notifyEvent, pre)
int                *notifyEvent;
boolean             pre;

#endif /* __STDC__ */
{
   return FALSE;
}

#ifdef __STDC__
boolean             CallEventAttribute (NotifyAttribute * notifyAttr, boolean pre)
#else  /* __STDC__ */
boolean             CallEventAttribute (notifyAttr, pre)
NotifyAttribute    *notifyAttr;
boolean             pre;

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
int                 CopyXClipboard (unsigned char **buffer)
#else  /* __STDC__ */
int                 CopyXClipboard (buffer)
unsigned char     **buffer;

#endif /* __STDC__ */
{
   return 0;
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
void                ChangeFrameTitle (int frame, char *texte)
#else  /* __STDC__ */
void                ChangeFrameTitle (frame, texte)
int                 frame;
char               *texte;

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
void                ShowSelection (PtrAbstractBox PavRac, boolean Visible)
#else  /* __STDC__ */
void                ShowSelection (PavRac, Visible)
PtrAbstractBox      PavRac;
boolean             Visible;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                SwitchSelection (int frame, boolean Allume)
#else  /* __STDC__ */
void                SwitchSelection (frame, Allume)
int                 frame;
boolean             Allume;

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
void                AddInSelection (PtrElement pEl, boolean dernier)

#else  /* __STDC__ */
void                AddInSelection (pEl, dernier)
PtrElement          pEl;
boolean             dernier;

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
void                SelectElement (PtrDocument pDoc, PtrElement pEl, boolean Debut, boolean Controle)

#else  /* __STDC__ */
void                SelectElement (pDoc, pEl, Debut, Controle)
PtrDocument         pDoc;
PtrElement          pEl;
boolean             Debut;
boolean             Controle;

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
void                HighlightSelection (boolean DebVisible)
#else  /* __STDC__ */
void                HighlightSelection (DebVisible)
boolean             DebVisible;

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
void                MergeAndSelect (PtrDocument SelDoc, PtrElement PremSel, PtrElement DerSel, int premcar, int dercar)

#else  /* __STDC__ */
void                MergeAndSelect (SelDoc, PremSel, DerSel, premcar, dercar)
PtrDocument         SelDoc;
PtrElement          PremSel;
PtrElement          DerSel;
int                 premcar;
int                 dercar;

#endif /* __STDC__ */
{
}


#ifdef __STDC__
void                SetNewSelectionStatus (int frame, PtrAbstractBox pAb, boolean Etat)
#else  /* __STDC__ */
void                SetNewSelectionStatus (frame, pAb, Etat)
int                 frame;
PtrAbstractBox      pAb;
boolean             Etat;

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
boolean             IsAbstractBoxDisplayed (PtrAbstractBox pav, int frame)
#else  /* __STDC__ */
boolean             IsAbstractBoxDisplayed (pav, frame)
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
boolean             ConfigGetPSchemaNature (PtrSSchema pSS, char *nomNature, char *presNature)
#else  /* __STDC__ */
boolean             ConfigGetPSchemaNature (pSS, nomNature, presNature)
PtrSSchema          pSS;
char               *nomNature;
char               *presNature;

#endif /* __STDC__ */
{
   return FALSE;
}


#ifdef __STDC__
boolean             ConfigDefaultPSchema (char *schema, char *schpres)

#else  /* __STDC__ */
boolean             ConfigDefaultPSchema (schema, schpres)
char               *schema;
char               *schpres;

#endif /* __STDC__ */
{
   return FALSE;
}

/* Les procedures de geom.c */
#ifdef __STDC__
void                UserGeometryMove (int frame, int *x, int *y, int large, int haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym)
#else  /* __STDC__ */
void                UserGeometryMove (frame, x, y, large, haut, xr, yr, xmin, xmax, ymin, ymax, xm, ym)
int                 frame;
int                *x;
int                *y;
int                 large;
int                 haut;
int                 xr;
int                 yr;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 xm;
int                 ym;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                UserGeometryResize (int frame, int x, int y, int *large, int *haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym)
#else  /* __STDC__ */
void                UserGeometryResize (frame, x, y, large, haut, xr, yr, xmin, xmax, ymin, ymax, xm, ym)
int                 frame;
int                 x;
int                 y;
int                *large;
int                *haut;
int                 xr;
int                 yr;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 xm;
int                 ym;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                UserGeometryCreate (int frame, int *x, int *y, int xr, int yr, int *large, int *haut, int xmin, int xmax, int ymin, int ymax, int PosX, int PosY, int DimX, int DimY)
#else  /* __STDC__ */
void                UserGeometryCreate (frame, x, y, xr, yr, large, haut, xmin, xmax, ymin, ymax, PosX, PosY, DimX, DimY)
int                 frame;
int                *x;
int                *y;
int                 xr;
int                 yr;
int                *large;
int                *haut;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 PosX;
int                 PosY;
int                 DimX;
int                 DimY;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                PolyLineModification (int frame, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close)
#else  /* __STDC__ */
void                PolyLineModification (frame, x, y, Pbuffer, Bbuffer, nbpoints, point, close)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer       Pbuffer;
PtrTextBuffer       Bbuffer;
int                 nbpoints;
int                 point;
boolean             close;

#endif /* __STDC__ */
{
}
