/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handle callbacks for Thot events
 *
 * Author: I. Vatton (INRIA)
 *
 */
#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */ 

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appstruct.h"
#include "document.h"
#include "dialogapi.h"


#ifdef _GTK
#include <gdk/gdkx.h>
#include "gtk-functions.h" /* GTK prototype */
#endif /*_GTK */

#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "appevents_tv.h"
#include "dialogapi_tv.h"

#include "readstr_f.h"
#include "callbackinit_f.h"
#include "dialogapi_f.h"
#include "memory_f.h"

/*----------------------------------------------------------------------
  ElementHasAction
  It returns TRUE if element pEl has an action associated with event/pre
  else it returns FALSE.
  Only Users action are considered.
  ----------------------------------------------------------------------*/
ThotBool  ElementHasAction (PtrElement pEl, APPevent event, ThotBool pre)
{
  PtrActionEvent	pActEvent;
  ThotBool		hasAction;

  hasAction = FALSE;

  if (pEl->ElStructSchema != NULL)
    if (pEl->ElStructSchema->SsActionList != NULL)
      {
        /* take the concerned actions list */
        pActEvent = pEl->ElStructSchema->SsActionList->EvSList[event];
        while (pActEvent != NULL && !hasAction)
          if (pActEvent->AEvPre == pre &&
              pActEvent->AEvType == pEl->ElTypeNumber)
            hasAction = TRUE;
          else
            pActEvent = pActEvent->AEvNext;
      }
  return hasAction;
}

/*----------------------------------------------------------------------
  CallAction looks for the concerned action in event list.
  It returns TRUE if the event action takes place of the editor action
  else it returns FALSE.
  ----------------------------------------------------------------------*/
static ThotBool CallAction (NotifyEvent * notifyEvent, APPevent event,
                            ThotBool pre, int type, Element element,
                            PtrSSchema schStruct, ThotBool attr)
{
  PtrActionEvent      pActEvent;
  PtrEventsSet        eventsSet;
  ThotBool            status;
  Proc                procEvent;
  Func                funcEvent;

  procEvent = NULL;
  funcEvent = NULL;

  /* See all actions linked with this event in different event lists */
  while (schStruct != NULL && procEvent == NULL && funcEvent == NULL)
    {
      eventsSet = schStruct->SsActionList;
      if (eventsSet != NULL)
        {
          /* take the concerned actions list */
          pActEvent = eventsSet->EvSList[event];
          while (pActEvent != NULL)
            {
              if (pActEvent->AEvPre == pre && (pActEvent->AEvType == 0 || pActEvent->AEvType == type))
                {
                  if (pre)
                    funcEvent = (Func) pActEvent->AEvAction->ActAction;
                  else
                    procEvent = pActEvent->AEvAction->ActAction;
                  pActEvent = NULL;	/* end of research */
                }
              else
                pActEvent = pActEvent->AEvNext;	/* continue */
            }
        }

      /* See in the parent schema, except for attributes */
      if (attr)
        schStruct = NULL;
      else if (procEvent == NULL && funcEvent == NULL)
        {
          status = TRUE;	/* still in the same schema */
          /*if (element != 0)
            element = (Element) ((PtrElement) element)->ElParent;*/
          while (status && element != 0)
            {
              status = (schStruct == ((PtrElement) element)->ElStructSchema);
              if (!status)
                {
                  /* a new schema */
                  schStruct = ((PtrElement) element)->ElStructSchema;
                  /* do not consider specific types of the previous schema */
                  if (type > MAX_BASIC_TYPE)
                    type = 0;
                }
              else
                element = (Element) ((PtrElement) element)->ElParent;
            }
	   
          if (element == 0)
            schStruct = NULL;	/* no more schema */
        }
    }

  /* See all actions linked with this event in EDITOR application */
  if (procEvent == NULL && funcEvent == NULL)
    {
      eventsSet = EditorEvents;
      if (eventsSet != NULL)
        {
          /* take the concerned actions list */
          pActEvent = eventsSet->EvSList[event];
          while (pActEvent != NULL)
            {
              if (pActEvent->AEvPre == pre && (pActEvent->AEvType == 0 || pActEvent->AEvType == type))
                {
                  if (pre)
                    funcEvent = (Func) pActEvent->AEvAction->ActAction;
                  else
                    procEvent = pActEvent->AEvAction->ActAction;
                  pActEvent = NULL;	/* end of research */
                }
              else
                pActEvent = pActEvent->AEvNext;
            }
        }
    }

  status = FALSE;
  if (funcEvent != NULL || procEvent != NULL)
    {
      if (funcEvent != NULL)
        status = (*(Func1)funcEvent) ((void *)notifyEvent);
      else
        (*(Proc1)procEvent) ((void *)notifyEvent);
    }
  return status;
}

/*----------------------------------------------------------------------
  CallEventAttribute notifies the possible application that
  an attribute has been created, deleted, modified, read
  or saved.
  It returns TRUE if the event action takes place of the editor action
  else it returns FALSE.
  ----------------------------------------------------------------------*/
ThotBool CallEventAttribute (NotifyAttribute * notifyAttr, ThotBool pre)
{
  Element             element;
  PtrSSchema          schStruct;

  if (notifyAttr != NULL)
    {
      element = notifyAttr->element;
      schStruct = (PtrSSchema) ((notifyAttr->attributeType).AttrSSchema);
      return CallAction ((NotifyEvent *) notifyAttr, notifyAttr->event, pre,
                         notifyAttr->attributeType.AttrTypeNum, element, schStruct, TRUE);
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  CallEventType sends a pointer to a actionstruct               
  and a structure-element and executes the                
  corresponding action (if any). If an action was         
  executed the function returns 'TRUE' else it returns    
  'FALSE'.                                                
  ----------------------------------------------------------------------*/
ThotBool CallEventType (NotifyEvent * notifyEvent, ThotBool pre)
{
  Element             element;
  PtrSSchema          schStruct;
  int                 elType;
  int                 doc;

  if (notifyEvent == NULL)
    return FALSE;

  elType = 0;
  schStruct = NULL;
  element = NULL;
  switch (notifyEvent->event)
    {
    case TteElemNew:
    case TteElemRead:
    case TteElemInclude:
    case TteElemMenu:
      if (pre)
        {
          elType = ((NotifyElement *) notifyEvent)->elementType.ElTypeNum;
          element = ((NotifyElement *) notifyEvent)->element;
          schStruct = (PtrSSchema) (((NotifyElement *) notifyEvent)->elementType.ElSSchema);
        }
      else
        {
          element = ((NotifyElement *) notifyEvent)->element;
          elType = ((PtrElement) element)->ElTypeNumber;
          schStruct = ((PtrElement) element)->ElStructSchema;
        }
      break;
    case TteElemDelete:
      if (pre)
        {
          element = ((NotifyElement *) notifyEvent)->element;
          elType = ((PtrElement) element)->ElTypeNumber;
          schStruct = ((PtrElement) element)->ElStructSchema;
        }
      else
        {
          element = ((NotifyElement *) notifyEvent)->element;
          elType = ((NotifyElement *) notifyEvent)->elementType.ElTypeNum;
          schStruct = (PtrSSchema) (((NotifyElement *) notifyEvent)->elementType.ElSSchema);
        }
      break;
    case TteElemSave:
    case TteElemExport:
    case TteElemSelect:
    case TteElemClick:
    case TteElemLClick:
    case TteElemMClick:
    case TteElemRClick:
    case TteElemActivate:
    case TteElemExtendSelect:
    case TteElemCopy:
    case TteElemChange:
    case TteElemMove:
    case TteElemMouseOver:
    case TteElemMouseOut:
    case TteElemReturn:
    case TteElemTab:
      element = ((NotifyElement *) notifyEvent)->element;
      elType = ((PtrElement) element)->ElTypeNumber;
      schStruct = ((PtrElement) element)->ElStructSchema;
      break;
    case TteElemSetReference:
    case TteElemFetchInclude:
    case TteElemTextModify:
      element = ((NotifyOnTarget *) notifyEvent)->element;
      elType = ((PtrElement) element)->ElTypeNumber;
      schStruct = ((PtrElement) element)->ElStructSchema;
      break;
    case TteElemPaste:
      if (pre)
        {
          element = ((NotifyOnValue *) notifyEvent)->target;
          elType = ((PtrElement) element)->ElTypeNumber;
          schStruct = ((PtrElement) element)->ElStructSchema;
        }
      else
        {
          element = ((NotifyElement *) notifyEvent)->element;
          elType = ((PtrElement) element)->ElTypeNumber;
          schStruct = ((PtrElement) element)->ElStructSchema;
        }
      break;
    case TteElemGraphModify:
      element = ((NotifyOnValue *) notifyEvent)->element;
      elType = ((PtrElement) element)->ElTypeNumber;
      schStruct = ((PtrElement) element)->ElStructSchema;
      break;
    case TtePRuleCreate:
    case TtePRuleModify:
    case TtePRuleDelete:
      element = ((NotifyPresentation *) notifyEvent)->element;
      elType = ((PtrElement) element)->ElTypeNumber;
      schStruct = ((PtrElement) element)->ElStructSchema;
      break;
    case TteDocOpen:
    case TteDocTmpOpen:
    case TteDocCreate:
    case TteDocClose:
    case TteDocSave:
    case TteDocExport:
    case TteDocNatPresent:
    case TteViewOpen:
    case TteViewClose:
    case TteViewResize:
    case TteViewScroll:
      element = NULL;
      doc = ((NotifyDialog *) notifyEvent)->document;
      if (doc && LoadedDocument[doc - 1])
        {
          schStruct = LoadedDocument[doc - 1]->DocSSchema;
          if (schStruct)
            elType = schStruct->SsRootElem;
        }
      break;
    case TteInit:
    case TteExit:
      break;
    default:
      return FALSE;
      break;
    }
  return CallAction (notifyEvent, notifyEvent->event, pre, elType, element, schStruct, FALSE);
}




#ifdef _WINGUI
/*-----------------------------------------------------------------------
  ThotDlgProc
  ------------------------------------------------------------------------*/
LRESULT CALLBACK ThotDlgProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  int        ndx;
    
  switch (msg)
    {
    case WM_CREATE:
      {
        for (ndx = 0; ndx < bIndex; ndx ++)
          {
            SetParent (formulary.Buttons[ndx], hwnDlg);
            ShowWindow (formulary.Buttons[ndx], SW_SHOW);
          }
        return 0;
      }
    case WM_COMMAND:
      switch (LOWORD (wParam))
        {
        case IDCANCEL: DestroyWindow (hwnDlg);
          return 0;
        default:       WIN_ThotCallBack (GetParent (hwnDlg), wParam , lParam);
          return 0;
        }
    default: return (DefWindowProc (hwnDlg, msg, wParam, lParam));
    }
}

#endif /* _WINGUI */

/*----------------------------------------------------------------------
  Default callback function
  ----------------------------------------------------------------------*/
void CallbackError (int ref, int typedata, char *data)
{
  printf ("Toolkit error : No callback procedure ...\n");
}

/*----------------------------------------------------------------------
  Callback for a menu button
  ----------------------------------------------------------------------*/
ThotBool CallMenu (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
{
#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
  register int        i;
  register int        index;
  register int        entry;
  struct E_List      *adbloc;
  struct Cat_Context *icatal;
#ifdef _WINGUI
  int                 ndx;
  UINT                menuEntry;
  int                 nbMenuItem;
#endif /* _WINGUI */

  /* Une attende est peut etre debloquee */
  icatal = catalogue;
  while (icatal->Cat_PtParent)
    icatal = icatal->Cat_PtParent;
  if (icatal == ShowCat && ShowReturn == 1)
    ShowReturn = 0;
  /* A menu entry is selected */
  if (catalogue->Cat_Widget)
    {
      if ((long int) catalogue->Cat_Widget == -1)
        /*** back to a simple button ***/
        (*(Proc3)CallbackDialogueProc) (
                                        (void *)catalogue->Cat_Ref,
                                        (void *)INTEGER_DATA,
                                        (void *)0);
      else
        {
          adbloc = catalogue->Cat_Entries;
          entry = -1;
          index = 0;
          i = 2;		/* decalage de 2 pour le widget titre */
          while (entry == -1 && adbloc)
            {
              while (entry == -1 && i < C_NUMBER)
                {
#ifdef _WINGUI
                  if (IsMenu ((ThotMenu) (adbloc->E_ThotWidget[i])))
                    {
                      nbMenuItem = GetMenuItemCount ((ThotMenu) (adbloc->E_ThotWidget[i]));
                      for (ndx = 0; ndx < nbMenuItem; ndx++)
                        {
                          menuEntry = GetMenuItemID ((ThotMenu) (adbloc->E_ThotWidget[i]), ndx);
                          if (menuEntry == (catalogue->Cat_Ref + (UINT)w))
                            entry = ndx;
                        }
                    }
                  else
#endif /* _WINGUI */
                    if (adbloc->E_ThotWidget[i] == w)
                      entry = index;
                  i++;
                  index++;
                }
              /* Passe au bloc suivant */
              adbloc = adbloc->E_Next;
              i = 0;
            }
          /*** Retour de l'entree du menu choisie vers l'application ***/
          (*(Proc3)CallbackDialogueProc) (
                                          (void *)catalogue->Cat_Ref,
                                          (void *)INTEGER_DATA,
                                          (void *)entry);
        }
    }
  return TRUE;
#endif /* #if defined(_WINGUI) || defined(_GTK) || defined(_WX) */
}


/*----------------------------------------------------------------------
  Menu Callback interface for wxWindows
  ----------------------------------------------------------------------*/
ThotBool CallMenuWX (ThotWidget w, void * catalogue)
{
#ifdef _WX
  return CallMenu( w, (struct Cat_Context *)catalogue, 0 );
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  Menu Callback interface for GTK
  ----------------------------------------------------------------------*/
ThotBool CallMenuGTK (ThotWidget w, struct Cat_Context *catalogue)
{
#ifdef _GTK
  return CallMenu( w, (struct Cat_Context *)catalogue, 0 );
#else /* _GTK */
  return FALSE;
#endif /* _GTK */
}

#if defined(_WINGUI) || defined(_GTK) || defined(_WX)
/*----------------------------------------------------------------------
  Callback pour un bouton du toggle-menu
  ----------------------------------------------------------------------*/
#ifdef _WINGUI
ThotBool CallToggle (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
#endif /* _WINGUI */
#ifdef _GTK
     ThotBool CallToggleGTK (ThotWidget w, struct Cat_Context *catalogue)
#endif /* _GTK */
#if defined(_WX)
     ThotBool CallToggleWX (ThotWidget w, struct Cat_Context *catalogue)
#endif /* _WX */
{
  register int        i;
  int                 entry;
  int                 ent;
  int                 max;
  struct E_List      *adbloc;

  /* Enregistre la selection d'un toggle button */
  if (catalogue->Cat_Widget)
    {
      adbloc = catalogue->Cat_Entries;
      entry = -1;
      ent = 0;
      max = catalogue->Cat_Data;	/* nombre d'entrees definies */
      i = 2;			/* decalage de 2 pour le widget titre */
      while (entry == -1 && adbloc && ent <= max)
        {
          while (entry == -1 && i < C_NUMBER && ent <= max)
            {
              if (adbloc->E_ThotWidget[i] == w)
                {
                  entry = 0;
                  /* Bascule la valeur du bouton correspondant a l'entree */
                  adbloc->E_Free[i] = 'Y';
                  /* signale que l'entree est basculee si le menu est reactif */
                  if (catalogue->Cat_React)
                    (*(Proc3)CallbackDialogueProc) (
                                                    (void *)catalogue->Cat_Ref,
                                                    (void *)INTEGER_DATA,
                                                    (void *)ent);
                }
              i++;
              ent++;
            }
          /* Passe au bloc suivant */
          adbloc = adbloc->E_Next;
          i = 0;
        }
    }
  return TRUE;  
}

#endif /* #if defined(_WINGUI) || defined(_GTK) || defined(_WX) */

#ifdef _WINGUI
/*----------------------------------------------------------------------
  Callback for entry menus
  ----------------------------------------------------------------------*/
ThotBool CallRadio (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
{
  register int        i;
  register int        index;
  register int        entry;
  struct E_List      *adbloc;

  /* Enregistre la selection d'un toggle button */
  if (catalogue->Cat_Widget)
    {
      adbloc = catalogue->Cat_Entries;
      entry = -1;
      index = 0;
      i = 2;  /* shift of 2 positions for the title widget */
      while (entry == -1 && adbloc)
        {
          while (i < C_NUMBER && adbloc->E_ThotWidget[i])
            {
              if (adbloc->E_ThotWidget[i] == w)
                entry = index;
              i++;
              index++;
            }
          /* Passe au bloc suivant */
          adbloc = adbloc->E_Next;
          i = 0;
        }
      /*** Sauve la valeur de la derniere selection ***/
      catalogue->Cat_Data = entry;
      /* retourne la valeur si le menu est reactif */
      if (catalogue->Cat_React)
        (*(Proc3)CallbackDialogueProc) (
                                        (void *)catalogue->Cat_Ref,
                                        (void *)INTEGER_DATA,
                                        (void *)entry);
    }
  return TRUE;  
}
#endif /* _WINGUI */

#ifdef _GTK
/*----------------------------------------------------------------------
  Callback for radio buttons
  This callback is activated when a radio button is toogled :
  last radio button toogled is manualy untoogled because of a GTK bug
  NOTICE: this call back could be activated into the "style" menu when selecting
  font size, style ...
  ----------------------------------------------------------------------*/
ThotBool CallRadioGTK (ThotWidget w, struct Cat_Context *catalogue)
{
  register int        i;
  register intptr_t   index;
  register intptr_t   entry;
  struct E_List      *adbloc;

  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)) == FALSE)
    {
      /* Prevent to unselect an element directly...
         you must select another one to unselect others  */
      index = (intptr_t) gtk_object_get_data (GTK_OBJECT (w), "toggled");
      gtk_signal_handler_block (GTK_OBJECT(w), index);
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), TRUE);
      gtk_signal_handler_unblock (GTK_OBJECT(w), index); 
      return TRUE;
    }
  
  /* Enregistre la selection d'un toggle button */
  if (catalogue->Cat_Widget)
    {
      adbloc = catalogue->Cat_Entries;
      entry = -1;
      index = 0;
      i = 2;			/* decalage de 2 pour le widget titre */
      while (entry == -1 && adbloc)
        {
          while (entry == -1 && i < C_NUMBER)
            {
              if (adbloc->E_ThotWidget[i] == w)
                entry = index;
              i++;
              index++;
            }
          /* Passe au bloc suivant */
          adbloc = adbloc->E_Next;
          i = 0;
        }
      /*Deactivate All other Radio Button*/
      adbloc = catalogue->Cat_Entries;
      i = 2;
      while (adbloc)
        {
          while (i < C_NUMBER &&  adbloc->E_ThotWidget[i])
            {
              if (adbloc->E_ThotWidget[i] != w) 
                {
                  index = (intptr_t) gtk_object_get_data (GTK_OBJECT (adbloc->E_ThotWidget[i]), "toggled");
                  gtk_signal_handler_block (GTK_OBJECT(adbloc->E_ThotWidget[i]), index);
		  
                  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (adbloc->E_ThotWidget[i]), FALSE);
		  
                  gtk_signal_handler_unblock (GTK_OBJECT(adbloc->E_ThotWidget[i]), index); 
                }
              i++;	       
            }
          /* Go to next block */
          adbloc = adbloc->E_Next;
          i = 0;
        }
      
      /*** Sauve la valeur de la derniere selection ***/
      catalogue->Cat_Data = entry;
      /* retourne la valeur si le menu est reactif */
      if (catalogue->Cat_React)
        (*(Proc3)CallbackDialogueProc) (
                                        (void *)catalogue->Cat_Ref,
                                        (void *)INTEGER_DATA,
                                        (void *)entry);
    }
  return TRUE;  
}
#endif /* _GTK */

#ifdef _GTK
/*----------------------------------------------------------------------
  Callback for Icon Button
  This callback is activated when an icon button is clicked
  NOTICE : it is used into mathml palette and SVG palette (the buttons displayed)
  ----------------------------------------------------------------------*/
ThotBool CallIconButtonGTK (ThotWidget w, struct Cat_Context *catalogue)
{
  register int        i;
  register int        index;
  register int        entry;
  struct E_List      *adbloc;

  /* Enregistre la selection d'un toggle button */
  if (catalogue->Cat_Widget)
    {
      adbloc = catalogue->Cat_Entries;
      entry = -1;
      index = 0;
      i = 2;  /* shift of 2 positions for the title widget */
      while (entry == -1 && adbloc)
        {
          while (i < C_NUMBER && adbloc->E_ThotWidget[i])
            {
              if (adbloc->E_ThotWidget[i] == w)
                entry = index;
              i++;
              index++;
            }
          /* Passe au bloc suivant */
          adbloc = adbloc->E_Next;
          i = 0;
        }
      /*** Sauve la valeur de la derniere selection ***/
      catalogue->Cat_Data = entry;
      /* retourne la valeur si le menu est reactif */
      if (catalogue->Cat_React)
        (*(Proc3)CallbackDialogueProc) (
                                        (void *)catalogue->Cat_Ref,
                                        (void *)INTEGER_DATA,
                                        (void *)entry);
    }
  return TRUE;  
}
#endif /* _GTK */

#ifdef _WINGUI

/*-----------------------------------------------------------------------
  Win_ScrPopupProc The callback handler for the Scroll popup widget
  ------------------------------------------------------------------------*/
LRESULT CALLBACK WIN_ScrPopupProc (HWND hwnDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static HWND scrPopupWin;
  HWND   listBox;
  struct Cat_Context *catalogue;
  int    itemIndex, ref;

  switch (msg)
    {
      /* initialize the widget */
    case WM_CREATE:
      {
        HWND   listBox;
        DWORD  dwStyle;
        RECT   rect;
        HDC    display;
        TEXTMETRIC  textMetric;
        int    width;
        int    height;

        /* get the rectangle size according to the font metrics (hack) */
        WIN_SetDialogfont (hwnDlg);
        GetWindowRect (hwnDlg, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;

        display = GetDC (hwnDlg);
        if (GetTextMetrics (display, &textMetric))
          {
            height = height * textMetric.tmHeight;
            width = width * (textMetric.tmAveCharWidth);
          }
        else 
          {
            /* try to give it some value */
            height = height * 14;
            width = width * 14;
          }
        ReleaseDC (hwnDlg, display);
        /* we change the settings of the current window too */
        SetWindowPos (hwnDlg, NULL, rect.left, rect.top, width, height, SWP_NOZORDER);

        /* create a list box inside the container window */
        scrPopupWin = hwnDlg;
        dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL 
          | LBS_HASSTRINGS | LBS_NOTIFY | LBS_WANTKEYBOARDINPUT;
        /* give it the same size as that of the container */

        listBox = CreateWindowEx (WS_EX_CLIENTEDGE, "LISTBOX", NULL, dwStyle, 0, 0,
                                  width, height,
                                  hwnDlg, (HMENU) 1, hInstance, NULL);
        /* set the font of the window */
        WIN_SetDialogfont (listBox);
        SetFocus (listBox);
        return 0;
      }
      break;
      /* destroy the widget */
    case WM_CLOSE:
    case WM_DESTROY:
      RemoveProp (hwnDlg, "ref");
      scrPopupWin = NULL;
      if (msg == WM_DESTROY)
        PostQuitMessage (0);
      return 0;
      break;
    case WM_VKEYTOITEM:
      ref = (int) wParam;
      switch ((TCHAR) wParam)
        {
        case VK_RETURN:  /* activate an entry */
          SendMessage (hwnDlg, WM_COMMAND, MAKEWPARAM (1, LBN_DBLCLK), MAKELPARAM(FALSE, 0));
          return -2;
          break;
        case VK_ESCAPE:  /* cancel */
          DestroyWindow (hwnDlg);
          return -2;
          break;
        }
      break;
    case WM_COMMAND:
      if (LOWORD (wParam) == 1)
        {
          switch (HIWORD (wParam))
            {
            case LBN_DBLCLK:  /* activate an entry */
              {
                listBox = GetDlgItem (hwnDlg, 1);
                itemIndex = SendMessage (listBox, LB_GETCURSEL, 0, 0);
                ref = (int) GetProp (hwnDlg, "ref");
                catalogue = CatEntry (ref);
                CallMenu ((ThotWidget) itemIndex, catalogue, NULL);
                scrPopupWin = NULL;
                DestroyWindow (hwnDlg);
                return 0;
              }
              break;
            case LBN_KILLFOCUS:   /* destroy the window if we click elsewhere */
              if (scrPopupWin)
                {
                  HWND win;
                  win = GetFocus ();
		  
                  if (win != scrPopupWin && GetParent (win) != scrPopupWin)
                    {
                      scrPopupWin = NULL;
                      DestroyWindow (hwnDlg);
                      return 0;
                    }
                }
              break;
            }
        }
      break;
    }
  return (DefWindowProc (hwnDlg, msg, wParam, lParam));
}

/*----------------------------------------------------------------------
  Callback for menu buttons (Windows)
  ----------------------------------------------------------------------*/
void WIN_ThotCallBack (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
  struct Cat_Context *catalogue;
  struct Cat_Context *nearest;
  int                 i;
  int                 frame;
  int                 ref;
  ThotBool            found;

#ifdef AMAYA_DEBUG
  fprintf (stderr, "Got WIN_ThotCallBack(%X, %X(%d:%d), %X(%d))\n",
           hWnd, wParam, HIWORD (wParam), LOWORD (wParam), lParam, lParam);
#endif /* AMAYA_DEBUG */
  frame = GetMainFrameNumber (hWnd);
  if (frame > 0 && frame <= MAX_FRAME)
    {
      nearest = NULL;
      ref = LOWORD (wParam);
      if (ref == 0)
        return;
      i = 0;
      found = FALSE;
      while (!found && i < MAX_FRAMECAT && FrameCatList[frame].Cat_Table[i])
        {
          catalogue = FrameCatList[frame].Cat_Table[i];
          if (catalogue)
            {
              if (catalogue->Cat_Ref == ref)
                found = TRUE;
              else if (nearest == NULL)
                nearest = catalogue;
              else if (ref >= catalogue->Cat_Ref &&
                       ref - catalogue->Cat_Ref < ref - nearest->Cat_Ref)
                nearest = catalogue;
            }
          i++;
        }
      if (!found)
        catalogue = nearest;
      if (catalogue == NULL)
        return;
      ref = ref - catalogue->Cat_Ref;
      switch (catalogue->Cat_Type)
        {
        case CAT_PULL:
        case CAT_MENU:
        case CAT_POPUP:
        case CAT_SCRPOPUP:
          CallMenu ((ThotWidget)ref, catalogue, NULL);
          break;
        case CAT_TMENU:
          CallToggle ((ThotWidget)ref, catalogue, NULL);
          break;
        case CAT_SHEET:
        case CAT_FMENU:
          CallRadio ((ThotWidget)ref, catalogue, NULL);
          break;
        default:
          break;
        }
    }
}
#endif /* _WINGUI */

#ifdef _GTK
/*----------------------------------------------------------------------
  Delete a form
  ----------------------------------------------------------------------*/
ThotBool formKillGTK (GtkWidget *w, GdkEvent *ev, struct Cat_Context *catalogue)
{
  /* Le widget est detruit */
  if (catalogue && w &&
      (catalogue->Cat_Type == CAT_FORM || catalogue->Cat_Type == CAT_SHEET ||
       catalogue->Cat_Type == CAT_DIALOG || catalogue->Cat_Type == CAT_POPUP ||
       catalogue->Cat_Type == CAT_SCRPOPUP))
    {
      TtaDestroyDialogue (catalogue->Cat_Ref);
      return TRUE;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  Callback for a scrolled window (click) @JK
  ----------------------------------------------------------------------*/
ThotBool CallPopGTK (GtkWidget *w, struct Cat_Context *catalogue)
{
  GtkWidget          *window;

  window = (GtkWidget *) gtk_object_get_data (GTK_OBJECT (w), "window");
  if (window && GTK_WIDGET_HAS_GRAB (window))
    {
      gtk_grab_remove (window);
      gdk_pointer_ungrab (GDK_CURRENT_TIME);
      gdk_keyboard_ungrab (GDK_CURRENT_TIME);
    }
  CallMenuGTK ((ThotWidget) w, catalogue);
  return FALSE;
}

/*----------------------------------------------------------------------
  Callback for a scrolled window (button press)
  ----------------------------------------------------------------------*/
ThotBool scr_popup_button_press (GtkWidget *w, GdkEventButton *ev, 
                                 struct Cat_Context *catalogue)
{ 
  return CallPopGTK (w, catalogue);
}

/*----------------------------------------------------------------------
  Callback for a scrolled window (keypress)
  ----------------------------------------------------------------------*/
ThotBool scr_popup_key_press (GtkWidget *w, GdkEventKey *ev,
                              struct Cat_Context *catalogue)
{
  if (ev->keyval == GDK_Escape) 
    {
      formKillGTK (w, NULL, catalogue);
      return TRUE;
    }
  else if (ev->keyval == GDK_Return || ev->keyval == GDK_space)
    {
      return (CallPopGTK (w, catalogue));
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool ListEventGTK (GtkWidget *w, GdkEventButton *ev, 
                       struct Cat_Context *catalogue)
{
  int x,y;

  switch (ev->type)
    {
    case GDK_BUTTON_RELEASE:
      {
        x = (int) ev->x;
        y = (int) ev->y;
        if (x < w->allocation.x || 
            x > w->allocation.x + w->allocation.width ||
            y > w->allocation.y + w->allocation.height ||
            y < w->allocation.y)
          formKillGTK (w, NULL, catalogue);
      }
      return TRUE;
    case GDK_LEAVE_NOTIFY:
      /*We could hide the widget there ?*/
      return FALSE;
    default:
      break;
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  New input value.
  ----------------------------------------------------------------------*/
void CallValueSet (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
{
  intptr_t          val, val1;
  char              text[11];
  ThotWidget        wtext;

  /* Indication de valeur */
  if (catalogue->Cat_Widget && catalogue->Cat_Type == CAT_INT)
    {
      catalogue->Cat_Data = 0;
      wtext = catalogue->Cat_Entries->E_ThotWidget[1];
      strncpy (text, gtk_entry_get_text (GTK_ENTRY (wtext)), 10);
      text[10] = EOS;
      if (text[0] != EOS)
        {
          if (text[0] == '-' && text[1] == EOS)
            /* cas ou le caractere - a ete tape, on met val a 0 */
            val = 0;
          else
            sscanf (text, "%ld", &val);
          /* Est-ce une valeur valide ? */
          if (val < (intptr_t) catalogue->Cat_Entries->E_ThotWidget[2])
            val1 = (intptr_t) catalogue->Cat_Entries->E_ThotWidget[2];
          else if (val > (intptr_t) catalogue->Cat_Entries->E_ThotWidget[3])
            val1 = (intptr_t) catalogue->Cat_Entries->E_ThotWidget[3];
          else
            val1 = val;	/* valeur inchangee */
	  
          /* Est-ce qu'il faut changer le contenu du widget ? */
          if (val != val1)
            {
              sprintf (text, "%ld", val1);
              /* Desactive la procedure de Callback */
              if (catalogue->Cat_React)
                RemoveSignalGTK (GTK_OBJECT(wtext), "changed"); 
              gtk_entry_set_text (GTK_ENTRY (wtext), text);
              val = strlen (text);
              /* Reactive la procedure de Callback */
              if (catalogue->Cat_React)
                ConnectSignalGTK (GTK_OBJECT(wtext), "changed",
                                  GTK_SIGNAL_FUNC(CallValueSet), (gpointer)catalogue);
            }
          /* retourne la valeur saisie si la feuille de saisie est reactive */
          if (catalogue->Cat_React)
            (*(Proc3)CallbackDialogueProc) (
                                            (void *)catalogue->Cat_Ref,
                                            (void *)INTEGER_DATA,
                                            (void *)val);
        }
    }
}

/*----------------------------------------------------------------------
  ReturnTogglevalues returns switched entries.
  ----------------------------------------------------------------------*/
void ReturnTogglevalues (struct Cat_Context *catalogue)
{
  register int        i;
  register int        index;
  int                 ent;
  int                 max;
  struct E_List      *adbloc;

  /* Enregistre la selection d'un toggle button */
  if (catalogue->Cat_Widget)
    {
      adbloc = catalogue->Cat_Entries;
      index = 0;
      ent = 0;
      max = catalogue->Cat_Data;	/* nombre d'entrees definies */
      i = 2;			/* decalage de 2 pour le widget titre */
      while (adbloc)
        {
          while (i < C_NUMBER && ent < max)
            {
              /*** Retour les entrees selectionnees vers l'application ***/
              if (adbloc->E_Free[i] == 'Y')
                {
                  (*(Proc3)CallbackDialogueProc) (
                                                  (void *)catalogue->Cat_Ref,
                                                  (void *)INTEGER_DATA,
                                                  (void *)index);
                  adbloc->E_Free[i] = 'N';
                }
              i++;
              ent++;
              index++;
            }
          /* next block */
          adbloc = adbloc->E_Next;
          i = 0;
        }
    }
}

/*----------------------------------------------------------------------
  ReturnSheet handles a sheet callback.          
  ----------------------------------------------------------------------*/
void ReturnSheet (struct Cat_Context *parentCatalogue, int entry,
                  struct E_List *adbloc)
{
  ThotWidget          tmpw;
  gchar              *wtext;
  struct Cat_Context *catalogue;
  char                text[100];
  char               *ptr;
  int                 i;
  int                 ent;

  /*** Retour vers l'application ***/
  /*** Eteins les sous-widgets du feuillet si on quitte ***/
  /*** Recupere les retours des sous-catalogues         ***/
  adbloc = adbloc->E_Next;
  ent = 1;
  while (adbloc->E_ThotWidget[ent])
    {
      /* Il faut sauter les widgets des RowColumns */
      if (adbloc->E_Free[ent] == 'N')
        {
          catalogue = (struct Cat_Context *) adbloc->E_ThotWidget[ent];
          if (catalogue->Cat_Widget)
            {
              if (entry == 0)
                gtk_widget_hide (GTK_WIDGET(catalogue->Cat_Widget));
              /* Sinon il faut retourner la valeur du sous-catalogue */
              else
                {
                  if (catalogue->Cat_React)
                    ; /* value already returned */
                  else if (catalogue->Cat_Type == CAT_FMENU)
                    {
                      i = catalogue->Cat_Data; /* a sub-menu */
                      (*(Proc3)CallbackDialogueProc) (
                                                      (void *)catalogue->Cat_Ref,
                                                      (void *)INTEGER_DATA,
                                                      (void *)i);
                    }
                  else if (catalogue->Cat_Type == CAT_TMENU)
                    ReturnTogglevalues (catalogue); /* a toggle */
                  else if (catalogue->Cat_Type == CAT_INT) /* a number */
                    {
                      CallValueSet (catalogue->Cat_Entries->E_ThotWidget[1], catalogue, NULL);
                      strncpy (text, gtk_entry_get_text(GTK_ENTRY(catalogue->Cat_Entries->E_ThotWidget[1])), 10);
                      text[10] = EOS;
                      if (text[0] != EOS)
                        sscanf (text, "%d", &i);
                      else
                        i = 0;
                      (*(Proc3)CallbackDialogueProc) (
                                                      (void *)catalogue->Cat_Ref,
                                                      (void *)INTEGER_DATA,
                                                      (void *)i);
                    }
                  else if (catalogue->Cat_Type == CAT_TEXT)
                    {
                      (*(Proc3)CallbackDialogueProc) (
                                                      (void *)catalogue->Cat_Ref,
                                                      (void *)STRING_DATA,
                                                      (void *)gtk_entry_get_text(GTK_ENTRY(catalogue->Cat_Entries)));
                    }
                  else if (catalogue->Cat_Type == CAT_SELECT)
                    {
                      if (catalogue->Cat_SelectList)
                        {
                          tmpw = GTK_WIDGET(catalogue->Cat_Entries);
                          if(GTK_LIST(tmpw)->selection)
                            {
                              gtk_label_get(GTK_LABEL(gtk_object_get_data(GTK_OBJECT(GTK_LIST(tmpw)->selection->data), "ListElementLabel")),&ptr);
                              (*(Proc3)CallbackDialogueProc) (
                                                              (void *)catalogue->Cat_Ref,
                                                              (void *)STRING_DATA,
                                                              (void *)ptr);
                            }
                        }
                      else
                        {
                          tmpw = GTK_WIDGET(catalogue->Cat_Entries);
                          tmpw = GTK_WIDGET (gtk_object_get_data (GTK_OBJECT (tmpw), "EntryZone"));
                          wtext = gtk_entry_get_text (GTK_ENTRY (tmpw));
                          (*(Proc3)CallbackDialogueProc) (
                                                          (void *)catalogue->Cat_Ref,
                                                          (void *)STRING_DATA,
                                                          (void *)wtext);
                        }
                    }
                }
            }
        }
      ent++;
      if (ent >= C_NUMBER)
        {
          /* next block */
          ent = 0;
          if (adbloc->E_Next == NULL)
            break;
          else
            adbloc = adbloc->E_Next;
        }
    }
  /*** On fait disparaitre le formulaire ***/
  if (entry == 0 || parentCatalogue->Cat_Type == CAT_DIALOG || parentCatalogue->Cat_Type == CAT_FORM)
    {
      gtk_widget_hide (parentCatalogue->Cat_Widget);
      /* Si on en a fini avec la feuille de dialogue */
      catalogue = parentCatalogue;
      while (catalogue->Cat_PtParent)
        catalogue = catalogue->Cat_PtParent;
      if (catalogue == ShowCat && ShowReturn == 1)
        ShowReturn = 0;
    }
  (*(Proc3)CallbackDialogueProc) (
                                  (void *)parentCatalogue->Cat_Ref,
                                  (void *)INTEGER_DATA,
                                  (void *)entry);
}

/*----------------------------------------------------------------------
  CallbackSheet: a button was clicked.                                              
  ----------------------------------------------------------------------*/
gboolean CallSheetGTK (ThotWidget w, struct Cat_Context *catalogue)
{
  struct E_List      *adbloc;
  int                 i;
  int                 entry;

  /* Activate an entry */
  if (catalogue && catalogue->Cat_Widget)
    {
      adbloc = catalogue->Cat_Entries;
      entry = -1;
      i = 0;
      while (entry == -1 && i < C_NUMBER)
        {
          if (adbloc->E_ThotWidget[i] == w)
            entry = i;
          i++;
        }
      /* Si la feuille de dialogue est detruite cela force l'abandon */
      if (entry == -1)
        entry = catalogue->Cat_Default;
      if (entry != -1)
        {
          ReturnSheet (catalogue, entry, adbloc);
          return TRUE;
        }
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  Callback for a list selection
  ----------------------------------------------------------------------*/
ThotBool CallListGTK (ThotWidget w, struct Cat_Context *catalogue)
{
  gchar              *text = NULL;
  ThotWidget         tmpw;

  if (catalogue->Cat_Widget && catalogue->Cat_Type == CAT_SELECT)
    {
      /* when you select an element in a selector box */
      tmpw = GTK_WIDGET (catalogue->Cat_Entries);
      if (catalogue->Cat_SelectList)
        {
          /* just a simple list */
          if (GTK_LIST(tmpw)->selection)
            {
              gtk_label_get(GTK_LABEL(gtk_object_get_data (GTK_OBJECT (GTK_LIST(tmpw)->selection->data), "ListElementLabel")),&text);
              (*(Proc3)CallbackDialogueProc) (
                                              (void *)catalogue->Cat_Ref,
                                              (void *)STRING_DATA,
                                              (void *)text);
            }
        }
      else
        {
          /* a list and a text zone */
          /* just get the selected text and assigne it to the text zone data */
          if (GTK_LIST(tmpw)->selection)
            {
              gtk_label_get(GTK_LABEL(gtk_object_get_data(GTK_OBJECT(GTK_LIST(tmpw)->selection->data), "ListElementLabel")), &text);
              tmpw = GTK_WIDGET(gtk_object_get_data (GTK_OBJECT (tmpw), "EntryZone"));
              gtk_entry_set_text (GTK_ENTRY (tmpw), text);
            }	      
        }
    }
  return TRUE;   
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
gboolean DeletePopShell (ThotWidget w, GdkEventButton *bu, gpointer data)
{
  PopShell = 0;
  return FALSE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
gboolean CallTextEnterGTK (ThotWidget w, GdkEventButton *bu, gpointer data)
{  
  struct Cat_Context *catalogue;
  int                 i;
  
  catalogue = (struct Cat_Context *) data;
  if (bu->button == 1) 
    {
      if (bu->type == GDK_2BUTTON_PRESS) 
        { 
          while (catalogue->Cat_PtParent)
            catalogue = catalogue->Cat_PtParent;
          i = catalogue->Cat_Default;
          (*(Proc3)CallbackDialogueProc) (
                                          (void *)catalogue->Cat_Ref,
                                          (void *)INTEGER_DATA,
                                          (void *)((char *)i));
          return TRUE;
        }
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  Callback de saisie de texte.
  ----------------------------------------------------------------------*/
ThotBool CallTextChangeGTK (ThotWidget w, struct Cat_Context *catalogue)
{
  char              *text = NULL;
    
  if (catalogue->Cat_Widget)
    {
      if (catalogue->Cat_Type == CAT_TEXT)
        {
          if (GTK_IS_ENTRY (catalogue->Cat_Entries))
            text = gtk_entry_get_text (GTK_ENTRY (catalogue->Cat_Entries));
          else
            text = gtk_editable_get_chars (GTK_EDITABLE(catalogue->Cat_Entries), 0, -1);
          (*(Proc3)CallbackDialogueProc) (
                                          (void *)catalogue->Cat_Ref,
                                          (void *)STRING_DATA,
                                          (void *)text);
        }
      else if (catalogue->Cat_Type == CAT_SELECT)
        {
          text = gtk_entry_get_text (GTK_ENTRY (w));
          (*(Proc3)CallbackDialogueProc) (
                                          (void *)catalogue->Cat_Ref,
                                          (void *)STRING_DATA,
                                          (void *)text);

        }
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  Callback pour un bouton du label de selecteur
  ----------------------------------------------------------------------*/
ThotBool CallLabel (ThotWidget w, struct Cat_Context *catalogue, caddr_t call_d)
{
  gchar *str;

  if (catalogue->Cat_Widget)
    {
      gtk_label_get(GTK_LABEL(gtk_object_get_data (GTK_OBJECT(w),"ButtonLabel")),&str);
      (*(Proc3)CallbackDialogueProc) (
                                      (void *)catalogue->Cat_Ref,
                                      (void *)STRING_DATA,
                                      (void *)str);
    }
  return TRUE;
}

/*----------------------------------------------------------------------
  ComboBoxGTK
  Callback function for the combo box
  ----------------------------------------------------------------------*/
ThotBool ComboBoxGTK (ThotWidget w, struct Cat_Context *catalogue, 
                      caddr_t call_d)
{
  char *val;

  if (!GTK_IS_ENTRY (w))
    return FALSE;

  val = gtk_entry_get_text (GTK_ENTRY (w));
  
  if (catalogue->Cat_React)
    (*(Proc3)CallbackDialogueProc) (
                                    (void *)catalogue->Cat_Ref,
                                    (void *)STRING_DATA,
                                    (void *)val);
  return FALSE;
}
#endif /* _GTK */

/*----------------------------------------------------------------------
  DefineCallbackDialog de'finit la proce'dure de traitement des      
  retoursde catalogues dans l'application.                           
  ----------------------------------------------------------------------*/
void TtaDefineDialogueCallback ( Proc procedure )
{
  CallbackDialogueProc = procedure;
}
