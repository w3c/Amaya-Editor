/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles Abstract Views
 *
 * Author: I. Vatton (INRIA)
 */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "libmsg.h"
#include "message.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "picture.h"
#include "appaction.h"
#include "dialog.h"
#include "application.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "modif_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "boxes_tv.h"
#include "platform_tv.h"
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "absboxes_f.h"
#include "abspictures_f.h"
#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attrmenu.h"
#include "attrmenu_f.h"
#include "attrpresent_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "config_f.h"
#include "content_f.h"
#include "creationmenu_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "glwindowdisplay.h"
#include "frame_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "presvariables_f.h"
#include "readpivot_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "views_f.h"

#ifdef _WX
#include "appdialogue_wx_f.h"
#endif /* _WX */

#ifdef _GL
#include "animbox_f.h"
#include "displaybox_f.h"
#include "content.h"
#endif /* _GL */
 

/*----------------------------------------------------------------------
  FrameToView retourne, sous la forme qui convient a l'API Thot, 
  les parametres identifiant le document et la vue        
  qui correspondent a une frame donnee.                   
  ----------------------------------------------------------------------*/
void FrameToView (int frame, int *doc, int *view)
{
  int                 i;
  PtrDocument         pDoc;

  *doc = FrameTable[frame].FrDoc;
  *view = 0;
  if (*doc == 0)
    return;
  else
    {
      pDoc = LoadedDocument[*doc - 1];
      *view = 0;
      if (pDoc != NULL)
        {
          GetViewFromFrame (frame, pDoc, &i);
          *view = i;
        }
    }
}
/*----------------------------------------------------------------------
  GetViewFromFrame retourne le pointeur sur le numero de vue (viewNum)      
  dans le document pDoc, correspondant a`                 	
  la fenetre de numero nframe.
  ----------------------------------------------------------------------*/
void GetViewFromFrame (int nframe, PtrDocument pDoc, int *viewNum)
{
  int                 view;

  *viewNum = 0;
  view = 0;
  /* cherche d'abord dans les vues de l'arbre principal */
  do
    {
      if (pDoc->DocView[view].DvPSchemaView > 0 &&
          pDoc->DocViewFrame[view] == nframe)
        *viewNum = view + 1;
      else
        view++;
    }
  while (view < MAX_VIEW_DOC && *viewNum == 0);
}
/*----------------------------------------------------------------------
  RedisplayDocViews demande le reaffichage de toutes les vues du	
  document pDoc.						
  ----------------------------------------------------------------------*/
void RedisplayDocViews (PtrDocument pDoc)
{
  DisplayMode       displayMode;
  int                 i, frame;

  displayMode = documentDisplayMode[IdentDocument (pDoc) - 1];
  if (displayMode == DisplayImmediately)
    {
      for (i = 0; i < MAX_VIEW_DOC; i++)
        if (pDoc->DocView[i].DvPSchemaView > 0)
          {
            /* open view */
            frame = pDoc->DocViewFrame[i];
#ifdef _GL
            GL_SwapEnable (frame);
#endif /* _GL */
            DisplayFrame (frame);
          }
    }
}


/*----------------------------------------------------------------------
  AbstractImageUpdated	signale les modifications de l'image	
  abstraite du document pDoc.				
  ----------------------------------------------------------------------*/
void AbstractImageUpdated (PtrDocument pDoc)
{
  DisplayMode       displayMode;
  PtrAbstractBox    pAb;
  int               i, h, frame;
  ThotBool          rootAbWillBeFree;

  displayMode = documentDisplayMode[IdentDocument (pDoc) - 1];
  if (displayMode == NoComputedDisplay)
    return;

  for (i = 0; i < MAX_VIEW_DOC; i++)
    if (pDoc->DocView[i].DvPSchemaView > 0
        && pDoc->DocViewModifiedAb[i] != NULL)
      {
        /* on ne s'occupe pas de la hauteur de page */
        h = 0;
        frame = pDoc->DocViewFrame[i];
        pAb = pDoc->DocViewModifiedAb[i];
        if (pDoc->DocViewRootAb[i] == NULL)
          pDoc->DocViewRootAb[i] = pAb;
        pDoc->DocViewModifiedAb[i] = NULL;
        ChangeConcreteImage (frame, &h, pAb);
        /* libere les paves morts */
        rootAbWillBeFree = pDoc->DocViewRootAb[i]->AbDead;
        if (rootAbWillBeFree)
          pDoc->DocViewRootAb[i] = NULL;
        FreeDeadAbstractBoxes (pAb, frame);
      }
}

/*----------------------------------------------------------------------
  EnclosingAbsBoxesBreakable
  Returns TRUE if all abstract boxes in view v corresponding to
  ancestors of element pEl are allowed to be incomplete.
  ----------------------------------------------------------------------*/
static ThotBool	EnclosingAbsBoxesBreakable (PtrElement pEl, int v,
                                            PtrDocument pDoc)
{
  PtrAbstractBox	pAb;

  pAb = NULL;
  while (pEl && !pAb)
    {
      pEl = pEl->ElParent;
      if (pEl)
        pAb = pEl->ElAbstractBox[v - 1];
    }
  return (IsBreakable (pAb, pDoc));
}

/*----------------------------------------------------------------------
  SupprFollowingAbsBoxes
  Delete from a given view all abstract boxes associated with elements
  that follow pEl (following siblings of pEl and its ancestors).
  ----------------------------------------------------------------------*/
static void SupprFollowingAbsBoxes (PtrElement pEl, PtrDocument pDoc, int view)
{
  PtrAbstractBox	pAb, pNextAb, pParentAb, pAbbRedisp, pAbbR;

  pAbbRedisp = NULL;
  /* look for the first abstract box (pAb) corresponding to a following
     sibling of pEl or to a following sibling of its ancestors */
  pAb = NULL;
  while (!pAb && pEl)
    {
      if (pEl->ElNext)
        {
          pAb = pEl->ElNext->ElAbstractBox[view - 1];
          if (!pAb)
            pEl = pEl->ElNext;
        }
      else
        pEl = pEl->ElParent;
    }
  /* now, delete the abstract box we have found and all following abstract
     boxes */
  if (pAb)
    {
      /* we will have to redisplay at least pAb */
      pAbbRedisp = pAb;
      do
        {
          pParentAb = pAb->AbEnclosing;
          /* kill pAb and all its following siblings */
          while (pAb)
            {
              pNextAb = pAb->AbNext;
              SetDeadAbsBox (pAb);
              ApplyRefAbsBoxSupp (pAb, &pAbbR, pDoc);
              pAbbRedisp = Enclosing (pAbbRedisp, pAbbR);
              pAb = pNextAb;
            }
          /* look for the first ancestor abstract box that has a next sibling*/
          pAb = pParentAb;
          if (pAb)
            {
              /* the parent abstract box is no longer complete */
              pAb->AbTruncatedTail = TRUE;
              if (pAb->AbNext)
                pAb = pAb->AbNext;
              else
                {
                  pNextAb = NULL;
                  while (pAb && !pNextAb)
                    {
                      pAb = pAb->AbEnclosing;
                      if (pAb)
                        {
                          pNextAb = pAb->AbNext;
                          if (pNextAb)
                            pAb = pNextAb;
                        }
                    }
                }
            }
        }
      while (pAb);
    }
  /* remember the abstract box that has to be redisplayed */
  if (pAbbRedisp)
    {
      pDoc->DocViewModifiedAb[view - 1] =
        Enclosing (pAbbRedisp, pDoc->DocViewModifiedAb[view - 1]);
    }
}

/*----------------------------------------------------------------------
  BuildAbstractBoxes
  Cree dans toutes les vues ouvertes du document pDoc les paves qui
  correspondent a l'element pEl, dans la limite de la capacite' des vues.
  ----------------------------------------------------------------------*/
void BuildAbstractBoxes (PtrElement pEl, PtrDocument pDoc)
{
  PtrAbstractBox      pAb;
  int                 view;

  /* on ne cree les paves que s'ils tombent dans la partie de l'image */
  /* du document deja construite */
  /* traite toutes les vues du document */
  for (view = 1; view <= MAX_VIEW_DOC; view++)
    {
      if (pDoc->DocView[view - 1].DvPSchemaView > 0)
        /* la vue est ouverte */
        if (ElemWithinImage (pEl, view, pDoc->DocViewRootAb[view-1], pDoc))
          /* l'element est a l'interieur de l'image deja construite */
          {
            if (!EnclosingAbsBoxesBreakable (pEl, view, pDoc) ||
                pEl->ElVolume < 10 ||
                pEl->ElVolume + pDoc->DocViewRootAb[view - 1]->AbVolume
                < 2 * pDoc->DocViewVolume[view - 1])
              /* force the creation of abstract boxes */
              /* CreateNewAbsBoxes will restore the right volume */
              pDoc->DocViewFreeVolume[view - 1] = THOT_MAXINT;
            else
              /* le volume du nouvel element ajoute' au volume existant
                 de la vue est tres superieur a ce que la fenetre peut
                 montrer d'un coup */
              {
                /* on detruit la partie de l'image abstraite qui suit le
                   nouvel element */
                SupprFollowingAbsBoxes (pEl, pDoc, view);
                /* on creera seulement une partie des paves du nouvel
                   element */
                pDoc->DocViewFreeVolume[view - 1] =
                  pDoc->DocViewVolume[view - 1] - pDoc->DocViewRootAb[view - 1]->AbVolume;
              }
            /* cree effectivement les paves du nouvel element dans la vue */
            if (pDoc->DocViewFreeVolume[view - 1] > 0)
              CreateNewAbsBoxes (pEl, pDoc, view);
            else
              {
                /* mark enclosing abstract boxes as truncated */
                pAb = pEl->ElParent->ElAbstractBox[view - 1];
                while (pAb && !pAb->AbTruncatedTail)
                  {
                    pAb->AbTruncatedTail = TRUE;
                    pAb = pAb->AbEnclosing;
                  }
              }
          }
    }
  /* applique les regles retardees concernant les paves cree's */
  ApplDelayedRule (pEl, pDoc);
  AbstractImageUpdated (pDoc);
}

/*----------------------------------------------------------------------
  TransmitAccessRight
  Updates access rights to element abstract boxes and all children.
  ----------------------------------------------------------------------*/
static void TransmitAccessRight (PtrAbstractBox pAb, ThotBool ro)
{
  if (pAb == NULL)
    return;
  pAb->AbReadOnly = ro;
  pAb = pAb->AbFirstEnclosed;
  while (pAb)
    {
      if (pAb && !pAb->AbPresentationBox && pAb->AbElement)
        {
          if (pAb->AbElement->ElAccess == Inherited)
            TransmitAccessRight (pAb, ro);
          else
            TransmitAccessRight (pAb, pAb->AbElement->ElAccess == ReadOnly);
        }
      pAb = pAb->AbNext;
    }
}

/*----------------------------------------------------------------------
  RedisplayNewElement affiche un element qui vient d'etre ajoute'    
  dans un arbre abstrait.                                         
  ----------------------------------------------------------------------*/
void RedisplayNewElement (Document document,
                          PtrElement newElement,
                          PtrElement sibling, ThotBool first,
                          ThotBool creation)
{
  PtrDocument         pDoc;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* If the document doesn't have any presentation schema, do nothing */
  if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
    return;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  if (sibling != NULL)
    /* l'element sibling n'est plus le dernier (ou premier) fils de son pere */
    ChangeFirstLast (sibling, pDoc, first, TRUE);
  /* cree les paves de l'element dans la limite de la capacite' des vues
     ouvertes */
  BuildAbstractBoxes (newElement, pDoc);
  /* reaffiche */
  RedisplayCommand (document);
  if (creation)
    {
      /* reaffiche les paves qui copient le nouvel element */
      RedisplayCopies (newElement, pDoc, (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
      /* reaffiche les numeros suivants qui changent */
      UpdateNumbers (NextElement(newElement), newElement, pDoc,
                     (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
    }
}

/*----------------------------------------------------------------------
  TCloseDocument ferme toutes les vue d'un document et decharge ce	
  document. Si pDoc est NULL, demande a` l'utilisateur de 
  designer le document a` fermer et lui demande           
  confirmation, sinon pDoc designe le contexte du document
  a` fermer.                                              
  Detruit egalement le fichier .BAK du document.          
  ----------------------------------------------------------------------*/
void TCloseDocument (PtrDocument pDoc)
{
  NotifyDialog      notifyDoc;
  Document          document;

  if (pDoc != NULL)
    {
      document = (Document) IdentDocument (pDoc);
      /* the document is still open */
      notifyDoc.event = TteDocClose;
      notifyDoc.document = document;
      notifyDoc.view = 0;
      if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
        {
		  pDoc = LoadedDocument[document-1];
		  if (pDoc)
		  {
            /* if there is a "Spell checker" menu entry, close the spell checker
               dialog box */
            if (ThotLocalActions[T_clearhistory] != NULL)
              (*(Proc1)ThotLocalActions[T_clearhistory]) ((void *)pDoc);
            /* if some dialog boxes for attribute input are displayed
               for that document, close them */
            CloseAttributeDialogues (pDoc);
            TtaHandlePendingEvents ();
            CloseDocumentAndViews(pDoc, TRUE);
		  }
        }
    }
}

/*----------------------------------------------------------------------
  NumberOfOpenViews retourne le nombre de vues qui existent pour	
  le document pDoc					
  ----------------------------------------------------------------------*/
int NumberOfOpenViews (PtrDocument pDoc)
{
  int                 view, result;

  result = 0;
  for (view = 0; view < MAX_VIEW_DOC; view++)
    if (pDoc->DocView[view].DvPSchemaView > 0)
      result++;
  return result;
}

/*----------------------------------------------------------------------
  FreeView libere les paves et le contexte de la vue view du	
  document pDoc.						
  ----------------------------------------------------------------------*/
void FreeView (PtrDocument pDoc, DocViewNumber view)
{
  PtrGuestViewDescr  guestView, next;

  view--;
  if (pDoc->DocViewRootAb[view] != NULL)
    FreeAbView (pDoc->DocViewRootAb[view], pDoc->DocViewFrame[view]);
  pDoc->DocViewRootAb[view] = NULL;
  pDoc->DocView[view].DvSSchema = NULL;
  pDoc->DocView[view].DvPSchemaView = 0;
  pDoc->DocView[view].DvSync = FALSE;
  if (pDoc->DocView[view].DvFirstGuestView)
    {
      guestView = pDoc->DocView[view].DvFirstGuestView;
      pDoc->DocView[view].DvFirstGuestView = NULL;
      while (guestView)
        {
          next = guestView->GvNextGuestView;
          TtaFreeMemory (guestView);
          guestView = next;
        }
    }
  pDoc->DocViewFrame[view] = 0;
  pDoc->DocViewVolume[view] = 0;
  pDoc->DocViewFreeVolume[view] = 0;
  pDoc->DocViewSubTree[view] = NULL;
}

static PtrDocument Closing_doc = NULL;
static int         Closing_view = 0;
/*----------------------------------------------------------------------
  CloseDocumentView detruit la vue de numero view
  pour le document pDoc. S'il s'agit de la derniere vue, libere le
  document dans le cas seulement ou closeDoc est vrai.
  ----------------------------------------------------------------------*/
void  CloseDocumentView (PtrDocument pDoc, int view, ThotBool closeDoc)
{
  /* avoid to close the same view twice */
  if (Closing_doc == pDoc && Closing_view == view)
    return;
  if (pDoc)
    {
      /* check if the document already exist */
      FreeView (pDoc, view);
      if (closeDoc && NumberOfOpenViews (pDoc) < 1 && Closing_doc != pDoc)
        {
          /* the last view of the document is closed */
          Closing_doc = pDoc;
          Closing_view = view;
          TCloseDocument (pDoc);
        }
    }
  if (Closing_doc == pDoc && Closing_view == view)
    {
      Closing_doc = NULL;
      Closing_view = view;
    }
}

/*----------------------------------------------------------------------
  CloseAllViewsDoc ferme toutes les vues ouvertes du document pDoc 
  ----------------------------------------------------------------------*/
void CloseAllViewsDoc (PtrDocument pDoc)
{
  int                 view;
  if (pDoc != NULL)
    {
      /* detruit les vues de l'arbre principal */
      for (view = 0; view < MAX_VIEW_DOC; view++)
        if (pDoc->DocView[view].DvPSchemaView != 0)
          {
            DestroyFrame (pDoc->DocViewFrame[view]);
            CloseDocumentView (pDoc, view + 1, FALSE);
          }
    }
}

/*----------------------------------------------------------------------
  CleanImageView cleans the abstract image of View corresponding to pDoc. 
  View = view number
  complete = TRUE if the window is completely cleaned.           
  ----------------------------------------------------------------------*/
void CleanImageView (int View, PtrDocument pDoc, ThotBool complete)
{
  PtrAbstractBox      pAb;
  int                 h;
  int                 frame;
  PtrAbstractBox      pAbbRoot;

  frame = 1;			/* initialization (for the compiler !) */
  pAbbRoot = NULL;		/* initialization (for the compiler !) */
  pAbbRoot = pDoc->DocViewRootAb[View - 1];
  frame = pDoc->DocViewFrame[View - 1];

#ifdef _GL
  FrameTable[frame].Anim_play = FALSE;
  if (FrameTable[frame].Animated_Boxes)
    {	    
      FreeAnimatedBoxes ((Animated_Cell*)FrameTable[frame].Animated_Boxes);
      FrameTable[frame].Animated_Boxes = NULL;
    }	
#endif /* _GL */

  /* All abstract boxes included into the root abs. box are marked dead */
  if (pAbbRoot == NULL)
    return;

  if (complete)
    {
      SetDeadAbsBox (pAbbRoot);
      ChangeConcreteImage (frame, &h, pAbbRoot);
      CloseDocumentView (pDoc, View, TRUE);
      FrameTable[frame].FrDoc = 0;
      /* selection is no more displayed */
      ViewFrameTable[frame - 1].FrSelectShown = FALSE;
    }
  else
    {
      pAb = pAbbRoot->AbFirstEnclosed;
      while (pAb != NULL)
        {
          SetDeadAbsBox (pAb);
          pAb = pAb->AbNext;
        }
      h = 0;
      ChangeConcreteImage (frame, &h, pAbbRoot);
      /* Releases all dead abstract boxes of the view */
      FreeDeadAbstractBoxes (pAbbRoot, frame);

      /* Shows that one must apply presentation rules of the root abstract
         box, for example to rebuild presentaion boxes, created by the
         root and destroyed */
      pAbbRoot->AbSize = -1;
      /* The complete root abstract box is marked. This allows AbsBoxesCreate
         to generate presentation abstract boxes created at the begenning */
      if (pAbbRoot->AbLeafType == LtCompound)
        pAbbRoot->AbTruncatedHead = FALSE;
    }
}

/*----------------------------------------------------------------------
  DestroyImage detruit l'image abstraite de toutes les vues          
  ouvertes dudocument pDoc                                     
  ----------------------------------------------------------------------*/
static void         DestroyImage (PtrDocument pDoc)
{
  int                 view, frame;

  for (view = 0; view < MAX_VIEW_DOC; view++)
    {
      if (pDoc->DocView[view].DvPSchemaView > 0)
        {
          CleanImageView (view + 1, pDoc, FALSE);
          /* selection is no more displayed */
          frame = pDoc->DocViewFrame[view];
          ViewFrameTable[frame - 1].FrSelectShown = FALSE;
        }
    }
}

/*----------------------------------------------------------------------
  RebuildImage recree l'image abstraite de toutes les vues            
  ouvertes du document pDoc                                      
  ----------------------------------------------------------------------*/
static void RebuildImage (PtrDocument pDoc)
{
  int                 view;
  PtrElement          pElRoot;
  PtrAbstractBox      pAbbRoot;
  ViewFrame          *pFrame;
  int                 frame, h, w;
  DisplayMode         dispMode;
  int                 doc;
  ThotBool            complete;

  for (view = 1; view <= MAX_VIEW_DOC; view++)
    if (pDoc->DocView[view - 1].DvPSchemaView > 0)
      {
        pElRoot = pDoc->DocDocElement;
        pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1];
        pAbbRoot = pDoc->DocViewRootAb[view - 1];
        frame = pDoc->DocViewFrame[view - 1];
        doc = IdentDocument (pDoc);
        dispMode = documentDisplayMode[doc - 1];
        documentDisplayMode[doc - 1] = NoComputedDisplay;
        AbsBoxesCreate (pElRoot, pDoc, view, TRUE, TRUE, &complete);
        documentDisplayMode[doc - 1] = dispMode;
        if (pAbbRoot == NULL)
          pAbbRoot = pDoc->DocViewRootAb[view - 1] = pElRoot->ElAbstractBox[view - 1];
        h = 0;
        ChangeConcreteImage (frame, &h, pAbbRoot);
        /* force to redraw all the frame */
        pFrame = &ViewFrameTable[frame - 1];
        /*CheckScrollingWidth (frame);*/
        if (FrameTable[frame].FrScrollOrg < 0)
          /* by default show the left side */
          pFrame->FrXOrg = FrameTable[frame].FrScrollOrg;
        GetSizesFrame (frame, &w, &h);
        DefClip (frame, pFrame->FrXOrg, pFrame->FrYOrg, w, h);
      }
}


/*----------------------------------------------------------------------
  RedisplayDefaultPresentation                                              
  ----------------------------------------------------------------------*/
void  RedisplayDefaultPresentation (Document document, PtrElement pEl,
                                    PRuleType typeRuleP, FunctionType funcType,
                                    int view)
{

  if (LoadedDocument[document - 1] == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (LoadedDocument[document - 1]->DocSSchema,
                          LoadedDocument[document - 1]) == NULL)
    return;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  /* if it's a function rule, nothing to do: there is no default presentation
     for functions */
  if (typeRuleP == PtFunction)
    return;
  ApplyStandardRule (pEl, LoadedDocument[document - 1], typeRuleP, funcType, view);
  AbstractImageUpdated (LoadedDocument[document - 1]);
  RedisplayCommand (document);
  /* le retrait de la regle de presentation doit etre pris en compte */
  /* dans les copies-inclusions de l'element */
  RedisplayCopies (pEl, LoadedDocument[document - 1], (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}


/*----------------------------------------------------------------------
  HideElement "desaffiche" un element qui devient invisible       
  mais n'est pas detruit.                                         
  ----------------------------------------------------------------------*/
void HideElement (PtrElement pEl, Document document)
{
  PtrDocument         pDoc;
  PtrElement          pChild;

  pDoc = LoadedDocument[document - 1];
  if (pDoc == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (!PresentationSchema (pDoc->DocSSchema, LoadedDocument [document - 1]))
    return;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  if (pEl->ElParent == NULL)
    /* c'est la racine d'un arbre, on detruit les paves des fils */
    /* pour garder au moins le pave racine */
    {
      if (!pEl->ElTerminal)
        {
          pChild = pEl->ElFirstChild;
          while (pChild != NULL)
            {
              DestroyAbsBoxes (pChild, pDoc, TRUE);
              pChild = pChild->ElNext;
            }
        }
    }
  else
    /* ce n'est pas une racine, on detruit les paves de l'element */
    DestroyAbsBoxes (pEl, pDoc, TRUE);
  /* reevalue l'image de toutes les vues */
  AbstractImageUpdated (pDoc);
  /* pas d'operation de reaffichage secondaires */
  RedisplayCommand (document);
}


/*----------------------------------------------------------------------
  RedisplayNewPRule                                               
  ----------------------------------------------------------------------*/
void RedisplayNewPRule (Document document, PtrElement pEl, PtrPRule pRule)
{
  if (LoadedDocument[document - 1] == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (LoadedDocument[document - 1]->DocSSchema,
                          LoadedDocument[document - 1]) == NULL)
    return;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  ApplyNewRule (LoadedDocument[document - 1], pRule, pEl);
  AbstractImageUpdated (LoadedDocument[document - 1]);
  RedisplayCommand (document);
  /* la nouvelle regle de presentation doit etre prise en compte dans */
  /* les copies-inclusions de l'element */
  RedisplayCopies (pEl, LoadedDocument[document - 1], (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}


/*----------------------------------------------------------------------
  UndisplayAttribute                                              
  ----------------------------------------------------------------------*/
void UndisplayAttribute (PtrElement pEl, PtrAttribute pAttr, Document document)
{
  ThotBool            inheritance, comparaison;
  PtrAttribute        pAttrAsc;
  PtrElement          pElAttr;
  PtrPSchema          pPS;

  if (LoadedDocument[document - 1] == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (LoadedDocument[document - 1]->DocSSchema,
                          LoadedDocument[document - 1]) == NULL)
    return;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  pPS = PresentationSchema (pAttr->AeAttrSSchema,
                            LoadedDocument[document - 1]);
  /* si l'attribut n'a pas de schema de presentation pour ce document, on
     ne fait rien */
  if (pPS == NULL)
    return;
  /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
  inheritance = (pPS->PsNHeirElems->Num[pAttr->AeAttrNum - 1] > 0);
  comparaison = (pPS->PsNComparAttrs->Num[pAttr->AeAttrNum - 1] > 0);
  if (inheritance || comparaison)
    /* cherche le premier attribut de meme type pose' sur un ascendant */
    /* de pEl */
    pAttrAsc = GetTypedAttrAncestor (pEl, pAttr->AeAttrNum,
                                     pAttr->AeAttrSSchema, &pElAttr);
  else
    pAttrAsc = NULL;
  if (pAttrAsc != NULL)
    {
      /* les regles viennent maintenant de l'heritage ou */
      /* de la comparaison a un ascendant */
      /* on applique sur les elements du sous arbre pEl  */
      /* les regles de presentation liees a l'heritage de cet attribut */
      /* par le sous-arbre s'il existe des elements heritants de celui-ci */
      ApplyAttrPRulesToSubtree (pEl, LoadedDocument[document - 1], pAttrAsc,
                                pElAttr);
      /* puis on applique sur les elements du sous-arbre pEl */
      /* les regles de presentation liees a la comparaison d'un attribut */
      /* du sous-arbre avec ce type d'attribut */
      ApplyAttrPRules (pEl, LoadedDocument[document - 1], pAttrAsc);
    }
  AbstractImageUpdated (LoadedDocument[document - 1]);
  RedisplayCommand (document);
  /* le nouvel attribut doit etre pris en compte dans */
  /* les copies-inclusions de l'element */
  RedisplayCopies (pEl, LoadedDocument[document - 1],
                   (ThotBool)(documentDisplayMode[document - 1] == DisplayImmediately));
}


/*----------------------------------------------------------------------
  DisplayAttribute                                                
  ----------------------------------------------------------------------*/
void DisplayAttribute (PtrElement pEl, PtrAttribute pAttr, Document document)
{
  PtrElement          pElChild;
  PtrPSchema          pPS;
  PtrHandlePSchema    pHd;   
  ThotBool            inheritance, comparison, reDisp;

  if (LoadedDocument[document - 1] == NULL || DocumentOfElement (pEl) == NULL)
    return;
  /* si le document n'a pas de schema de presentation, on ne fait rien */
  if (PresentationSchema (LoadedDocument[document - 1]->DocSSchema,
                          LoadedDocument[document - 1]) == NULL)
    return;
  /* si le document est en mode de non calcul de l'image, on ne fait rien */
  if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  pPS = PresentationSchema (pAttr->AeAttrSSchema,
                            LoadedDocument[document - 1]);
  /* si l'attribut n'a pas de schema de presentation pour ce document, on
     ne fait rien */
  if (pPS == NULL)
    return;
  /* doit-on se preoccuper des heritages et comparaisons d'attributs? */
  inheritance = FALSE;
  comparison = FALSE;
  pHd = NULL;
  while (pPS)
    {
      inheritance = inheritance ||
        (pPS->PsNHeirElems->Num[pAttr->AeAttrNum - 1] > 0);
      comparison = comparison ||
        (pPS->PsNComparAttrs->Num[pAttr->AeAttrNum - 1] > 0);
      /* next P schema */
      if (pHd == NULL)
        /* extension schemas have not been checked yet */
        /* get the first extension schema */
        pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema,
                                     LoadedDocument[document - 1], pEl);
      else
        /* get the next extension schema */
        pHd = pHd->HdNextPSchema;
      if (pHd == NULL)
        /* no more extension schemas. Stop */
        pPS = NULL;
      else
        pPS = pHd->HdPSchema;
    }
  reDisp = (documentDisplayMode[document - 1] == DisplayImmediately);
  /* d'abord on applique les regles de presentation liees */
  /* a l'attribut sur l'element lui-meme */
  ApplyAttrPRulesToElem (pEl, LoadedDocument[document - 1], pAttr, pEl, FALSE);
  /* puis on applique sur pEl et les elements du sous-arbre pEl */
  /* les regles de presentation liees a l'heritage de cet attribut */
  /* par le sous arbre s'il existe des elements heritants de celui-ci */
  if (inheritance)
    ApplyAttrPRulesToSubtree (pEl, LoadedDocument[document - 1], pAttr, pEl);
  /* puis on applique sur les elements du sous arbre pEl */
  /* les regles de presentation liees a la comparaison d'un attribut */
  /* du sous-arbre avec cetype d'attribut */
  if (!pEl->ElTerminal && comparison)
    for (pElChild = pEl->ElFirstChild; pElChild != NULL; pElChild = pElChild->ElNext)
      ApplyAttrPRules (pElChild, LoadedDocument[document - 1], pAttr);
  if (pAttr->AeAttrType == AtNumAttr)
    /* s'il s'agit d'un attribut initialisant un compteur, il */
    /* faut mettre a jour les boites utilisant ce compteur */
    UpdateCountersByAttr (pEl, pAttr, LoadedDocument[document - 1]);
  /* on applique les regles retardee */
  ApplDelayedRule (pEl, LoadedDocument[document - 1]);
  AbstractImageUpdated (LoadedDocument[document - 1]);
  RedisplayCommand (document);
  /* le nouvel attribut doit etre pris en compte dans */
  /* les copies-inclusions de l'element */
  RedisplayCopies (pEl, LoadedDocument[document - 1], reDisp);
}

/*----------------------------------------------------------------------
  RedisplayCommand        Selon le mode d'affichage, execute ou   
  met en attente une commande de reaffichage secondaire.          
  ----------------------------------------------------------------------*/
void RedisplayCommand (Document doc)
{
  if (documentDisplayMode[doc - 1] == DisplayImmediately)
    {
      /* reaffiche ce qui a deja ete prepare' */
      RedisplayDocViews (LoadedDocument[doc - 1]);
    }
}


/*----------------------------------------------------------------------
  NewSelection
  ----------------------------------------------------------------------*/
void NewSelection (Document doc, Element element, Attribute attr,
                   int firstCharacter, int lastCharacter)
{
  /* annule l'extension precedente */
  NewDocSelection[doc - 1].SDElemExt = NULL;
  NewDocSelection[doc - 1].SDCharExt = 0;
  /* enregistre cette nouvelle selection */
  NewDocSelection[doc - 1].SDSelActive = TRUE;
  NewDocSelection[doc - 1].SDAttribute = attr;
  NewDocSelection[doc - 1].SDElemSel = element;
  NewDocSelection[doc - 1].SDFirstChar = firstCharacter;
  NewDocSelection[doc - 1].SDLastChar = lastCharacter;
}

/*----------------------------------------------------------------------
  GetDeferredSelection
  Return TRUE if there is a differed selection
  ----------------------------------------------------------------------*/
ThotBool GetDeferredSelection (Document doc, PtrElement *firstSel, PtrElement *lastSel,
                               int *firstCharacter, int *lastCharacter)
{
  if (documentDisplayMode[doc - 1] == DeferredDisplay && NewDocSelection[doc - 1].SDSelActive)
    {
      *firstSel = (PtrElement) NewDocSelection[doc - 1].SDElemSel;
      *firstCharacter = NewDocSelection[doc - 1].SDFirstChar;
      if (NewDocSelection[doc - 1].SDElemExt)
        {
          // there is an extension
          *lastCharacter = NewDocSelection[doc - 1].SDCharExt;
          *lastSel = (PtrElement)NewDocSelection[doc - 1].SDElemExt;
        }
      else
        {
          *lastCharacter = NewDocSelection[doc - 1].SDLastChar;
          *lastSel = (PtrElement) NewDocSelection[doc - 1].SDElemSel;
        }
      return TRUE;
    }
  else
    {
      *firstSel = *lastSel = NULL;
      *firstCharacter = *lastCharacter = 0;
      return FALSE;
    }
}

/*----------------------------------------------------------------------
  NewSelectionExtension
  ----------------------------------------------------------------------*/
void  NewSelectionExtension (Document doc, Element element, int lastCharacter)
{
  /* enregistre cette nouvelle extension de selection */
  if (NewDocSelection[doc - 1].SDAttribute == NULL)
    NewDocSelection[doc - 1].SDElemExt = element;
  NewDocSelection[doc - 1].SDCharExt = lastCharacter;
}


/*----------------------------------------------------------------------
  TtaFreeView
  frees the view of the document. The window continues to exist but the
  document is no longer displayed in this window.
  Parameters:
  document: the document for which a view must be closed.
  view: the view to be closed.
  ----------------------------------------------------------------------*/
void TtaFreeView (Document document, View view)
{
  PtrDocument         pDoc;

  UserErrorCode = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else	if (view < 1 || view > MAX_VIEW_DOC)
    TtaError (ERR_invalid_parameter);
  else
    /* parameters look OK */
    {
      pDoc = LoadedDocument[document - 1];
      CleanImageView (view, pDoc, TRUE);
    }
}

/*----------------------------------------------------------------------
  IsSelectionRegistered
  ----------------------------------------------------------------------*/
ThotBool IsSelectionRegistered (Document doc, ThotBool *abort)
{
  ThotBool            ret;

  ret = NewDocSelection[doc - 1].SDSelActive;
  if (ret)
    *abort = (NewDocSelection[doc - 1].SDElemSel == NULL);
  return ret;
}

/*----------------------------------------------------------------------
  TtaSetDisplayMode
  Changes display mode for a document. Three display modes are available.
  In the immediate mode, each modification made in the abstract tree of a
  document is immediately reflected in all opened views where the modification
  can be seen.
  In the deferred mode, the programmer can decide when the modifications are
  made visible to the user; this avoids the image of the document to blink when
  several elementary changes are made successively. Modifications are displayed
  when mode is changed to DisplayImmediately.
  In the NoComputedDisplay mode, the modifications are not displayed and they 
  are not computed inside the editor; the execution is more rapid but the current
  image is lost. When mode is changed to DisplayImmediately or DeferredMode,
  the image is completely redrawn by the editor.
  In the SuspendDisplay mode, the modifications are not displayed but stored 
  inside the editor; the execution is more and the current image is not lost.
  When mode is changed to DisplayImmediately or DeferredMode, the modifications
  are computed by the editor.
  An application that handles several documents at the same time can choose
  different modes for different documents. When a document is open or created,
  it is initially in the immediate mode.
  Parameters:
  doc: the document.
  NewDisplayMode: new display mode for that document.
  ----------------------------------------------------------------------*/
void TtaSetDisplayMode (Document doc, DisplayMode newDisplayMode)
{
  DisplayMode       oldDisplayMode;
  PtrDocument       pDoc;
  PtrElement        pEl;

  UserErrorCode = 0;
  /* Checks the parameter document */
  if (doc < 1 || doc > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[doc - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is ok */
    {
      pDoc = LoadedDocument[doc - 1];
      /* si le document n'a pas de schema de presentation, on ne fait rien */
      if (PresentationSchema (pDoc->DocSSchema, pDoc) == NULL)
        return;

      oldDisplayMode = documentDisplayMode[doc - 1];
      if (oldDisplayMode != newDisplayMode)
        /* il y a effectivement changement de mode */
        {
          if (oldDisplayMode == DisplayImmediately)
            /* le document passe en mode affichage differe ou sans calcul
               d'image */
            {
              /* si on passe au mode sans calcul d'image il faut detruire
                 l'image */
              if (newDisplayMode == NoComputedDisplay)
                DestroyImage (pDoc);
              else if (newDisplayMode == SuspendDisplay)
                TtaClearViewSelections ();
              else
                // By default the selection doesn't need to be reset
                NewDocSelection[doc - 1].SDSelActive = FALSE;

              /* on met a jour le mode d'affichage */
              documentDisplayMode[doc - 1] = newDisplayMode;
            }
          else if (newDisplayMode == DisplayImmediately)
            /* le document passe du mode affichage differe' ou sans calcul  */
            /* d'image au mode  d'affichage immediat */
            {
              /* on met a jour le mode d'affichage */
              documentDisplayMode[doc - 1] = newDisplayMode;     
              if (oldDisplayMode == NoComputedDisplay)
                /* il faut recalculer l'image */
                RebuildImage (pDoc);
              else if (oldDisplayMode == SuspendDisplay)
                AbstractImageUpdated (pDoc);
	      
              if (!NewDocSelection[doc - 1].SDSelActive)
                {
                  /* la selection n'a pas change', on la rallume */
                  if (oldDisplayMode == SuspendDisplay)
                    HighlightSelection (TRUE, FALSE);
                }
              else
                /* restore the registered selection */
                {
                  pEl = (PtrElement)(NewDocSelection[doc - 1].SDElemSel);
                  if (pEl == NULL)
                    /* cancel the selection */
                    ResetSelection (pDoc);
                  else
                    {
                      if (NewDocSelection[doc - 1].SDElemExt == NULL &&
                          pEl->ElTerminal &&
                          pEl->ElLeafType == LtPicture)
                        /* partial selection */
                        SelectString (pDoc, pEl,
                                      NewDocSelection[doc - 1].SDFirstChar,
                                      NewDocSelection[doc - 1].SDLastChar);
                      else if (NewDocSelection[doc - 1].SDFirstChar == 0 &&
                               NewDocSelection[doc - 1].SDLastChar == 0)
                        /* whole element selected */
                        SelectElement (pDoc, pEl, TRUE, TRUE, TRUE);
                      else
                        /* partial selection */
                        SelectString (pDoc, pEl,
                                      NewDocSelection[doc - 1].SDFirstChar,
                                      NewDocSelection[doc - 1].SDLastChar);
                      /* the selection is done */
                      NewDocSelection[doc - 1].SDElemSel = NULL;
                    }

                  /* is there an extended selection */
                  if (NewDocSelection[doc - 1].SDElemExt)
                    /* il y a une extension de selection a etablir */
                    {
                      pEl = (PtrElement)(NewDocSelection[doc - 1].SDElemExt);
                      ExtendSelection (pEl,
                                       NewDocSelection[doc - 1].SDCharExt,
                                       TRUE, FALSE, FALSE);
                      /* il n'y a plus d'extension de selection a etablir */
                      NewDocSelection[doc - 1].SDElemExt = NULL;
                    }
                  /* plus de selection a faire pour ce document */
                  NewDocSelection[doc - 1].SDSelActive = FALSE;
                }
	      
              /* reaffiche ce qui a deja ete prepare' */
              RedisplayDocViews (pDoc);
            }
          else if (newDisplayMode == NoComputedDisplay)
            {
              /* le document passe du mode affichage differe'  */
              /* au mode d'affichage sans calcul d'image  */
              DestroyImage (pDoc);
              /* on met a jour le mode d'affichage */
              documentDisplayMode[doc - 1] = newDisplayMode;
            }
          else if (oldDisplayMode == NoComputedDisplay)
            {
              /* on met a jour le mode d'affichage */
              documentDisplayMode[doc - 1] = newDisplayMode;
              /* le document passe du mode affichage sans calcul d'image   */
              /* au mode d'affichage differe'  */
              RebuildImage (pDoc);
            }
          else if (oldDisplayMode == SuspendDisplay &&
                   newDisplayMode == DeferredDisplay)
            {
              /* on met a jour le mode d'affichage */
              documentDisplayMode[doc - 1] = newDisplayMode;
              AbstractImageUpdated (pDoc);
            }
          else
            /* on met a jour le mode d'affichage */
            documentDisplayMode[doc - 1] = newDisplayMode;

        }
    }
}

/*----------------------------------------------------------------------
  TtaGetDisplayMode

  Returns the current display mode for a document.
  Parameter:
  document: the document.

  Return value:
  current display mode for that document.
  ----------------------------------------------------------------------*/
DisplayMode TtaGetDisplayMode (Document document)
{
  DisplayMode         result;

  UserErrorCode = 0;
  result = DisplayImmediately;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is ok */
    result = documentDisplayMode[document - 1];
  return result;
}

/*----------------------------------------------------------------------
  TtaUpdateAccessRightInViews

  Update ReadOnly status of the element and its children in all views
  Parameter:
  document: the document.
  element: the root element of the updated tree
  ----------------------------------------------------------------------*/
void TtaUpdateAccessRightInViews (Document document, Element element)
{
  PtrElement          pEl = (PtrElement) element;
  int                 view;

  UserErrorCode = 0;
  /* Checks the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (documentDisplayMode[document - 1] == NoComputedDisplay)
    return;
  else
    {
      for (view = 0; view < MAX_VIEW_DOC; view++)
        {
          if (LoadedDocument[document - 1]->DocView[view].DvPSchemaView > 0 &&
              pEl->ElAbstractBox[view])
            /* transmit access rigth */
            TransmitAccessRight (pEl->ElAbstractBox[view],
                                 pEl->ElAccess == ReadOnly);
        }
    }
}
