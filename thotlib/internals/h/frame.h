/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

#ifndef _FRAME_H
#define _FRAME_H

#ifndef NODISPLAY

#include "constmenu.h"
#include "constbutton.h"

#define MAX_FONT    80
#define MAX_FONTNAME   10

/*
 * wxWidgets Frame_Ctl definition
 * this is the description of a document's view widget.
 * It contains : horizontal/vertical scrollbars, and a OpenGL area.
 * */
typedef struct _Frame_Ctl {
  int        FrTopMargin;               /* Frame Top Margin                  */
  int        FrScrollOrg;               /* Scrolling origin                  */
  int        FrScrollWidth;             /* Scrolling width                   */

  int        FrWidth;                   /* Frame Width                       */
  int        FrHeight;                  /* Frame Height                      */

  int        FrWindowId;                /* Parent window id                  */
  int        FrPageId;                  /* Frame page id                     */
  int        FrPagePos;                 /* Frame page position 1/2           */
  int        FrDoc;                     /* Document ID                       */
  int        FrView;                    /* Presentation schema View          */
  char       FrViewName[50];            /* The corresponding view name (Structure, Toc, Log ...) */
  
  ThotScrollBar WdScrollV;              /* Widget of Vertical Scroll         */
  ThotScrollBar WdScrollH;              /* Widget of Horizontal Scroll       */
  ThotFrame  	WdFrame;                /* Widget of the Document Frame      */

  ThotBool   EnabledButton_Panel_XHTML[MAX_BUTTON]; /* Enabled XHTML panel buttons           */
  ThotBool   CheckedButton_Panel_XHTML[MAX_BUTTON]; /* Checked XHTML panel buttons           */  
  
#ifdef _GL
  void       *Animated_Boxes;
  double     BeginTime;
  double     LastTime;
  ThotBool   DblBuffNeedSwap;
  ThotBool   Anim_play;
  ThotBool   Scroll_enabled;  /* Control when scroll applies */
  ThotBool   SwapOK;          /* Control when swapping applies */
#endif /*_GL*/

} Frame_Ctl;


enum
{
  ToolBarBrowsing,
  ToolBarEditing,
  ToolBarNumber
};

/*
 * wxWidgets Window_Ctl definition
 * this is the description of a Frame_Ctl container
 * It contains : toolbar, urlbar, pages
 * */
typedef struct _Window_Ctl {
  int        FrWidth;                   /* Window Width                      */
  int        FrHeight;                  /* Window Height                     */
#ifdef _WX  
  AmayaWindow * WdWindow;               /* Widget of the wxWidgets window    */
#endif /* _WX */
  ThotMenu      WdMenus[MAX_MENU];      /* List of menu Widgets in the menubar
					   these widgets are build with FrMenus model */
  struct     _Menu_Ctl *FrMenus;        /* First menu context                */
  int        MenuAttr;                  /* Attributes top menu ID or -1      */
  int        MenuSelect;                /* Selection top menu ID or -1       */
  int        MenuHelp;                  /* Help top menu or -1               */
  int        MenuContext;               /* Contextual top menu ID or -1      */
  int        MenuDocContext;            /* Contextual doc menu ID or -1      */
  int        MenuItemPaste;             /* Menu including the Paste command or -1 */
  int        MenuItemUndo;              /* Menu including the Undo command or -1  */
  int        MenuItemRedo;              /* Menu including the Redo command or -1  */
  int        MenuItemShowPanelID;       /* show/hide panel menu item ID or -1     */
  int        MenuItemShowToolBar[ToolBarNumber];    /* show/hide toolbar menu item ID or -1   */
} Window_Ctl;

#endif /* !NODISPLAY */

#endif /* _FRAME_H */
