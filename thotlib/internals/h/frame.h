/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

#ifndef _FRAME_H
#define _FRAME_H

#ifndef NODISPLAY

#include "constmenu.h"

#define MAX_FONT    60
#define MAX_FONTNAME   10
#define MAX_BUTTON  30

#if defined(_MOTIF) || defined(_GTK) || defined(_WINGUI)
typedef struct _Frame_Ctl {
  int        FrTopMargin;               /* Window Top Margin                 */
  int        FrScrollOrg;               /* Scrolling origin                  */
  int        FrScrollWidth;             /* Scrolling width                   */
  int        FrWidth;                   /* Window Width                      */
  int        FrHeight;                  /* Window Height                     */
  int        FrDoc;                     /* Document ID                       */
  int        FrView;                    /* Presentation schema View          */
  ThotScrollBar WdScrollV;              /* Widget of Vertical Scroll         */
  ThotScrollBar WdScrollH;              /* Widget of Horizontal Scroll       */
  ThotFrame  	WdFrame;                /* Widget of the Document Frame      */
  ThotStatusBar WdStatus;               /* Widget of the Document status     */
  struct     _Menu_Ctl *FrMenus;        /* First menu context                */
  int        MenuAttr;                  /* Attributes menu ID or -1          */
  int        MenuSelect;                /* Selection menu ID or -1           */
  int        MenuPaste;                 /* Menu including the Paste command
					   or -1 */
  int        MenuContext;               /* Contextual menu ID or -1          */
  int        EntryPaste;                /* Entry number of the Paste command */
  int        MenuUndo;                  /* Menu including the Undo command
					   or -1  */
  int        EntryUndo;                 /* Entry number of the Undo command  */
  int        MenuRedo;                  /* Menu including the Redo command
					   or -1  */
  int        EntryRedo;                 /* Entry number of the Redo command  */
  ThotMenu   WdMenus[MAX_MENU];         /* List of menu Widgets              */
  ThotBool   EnabledMenus[MAX_MENU];    /* Enabled menus                     */

  Proc       Call_Button[MAX_BUTTON];   /* List of button Callbacks          */
  ThotButton Button[MAX_BUTTON];        /* List of button Widgets            */
  ThotBool   EnabledButton[MAX_BUTTON]; /* Enabled buttons                   */
  ThotBool   CheckedButton[MAX_BUTTON]; /* Checked buttons                   */
  
#ifdef _WINGUI
  int        ButtonId[MAX_BUTTON];
  char      *TbStrings[MAX_BUTTON];     /* Tooltips text                     */
  ThotWidget Label;                     /* Labels of text zones              */
#else /* _WINGUI */
  ThotWidget Row_Zone;                  /* Parent widget of Text_Zone        */
  ThotWidget Combo;                     /* List of combobox Widgets          */
#endif /* _WINGUI */
  ThotWidget Text_Zone;                 /* List of text-zone Widgets         */
  Proc       Call_Text;                 /* List of text-zone Callbacks       */
  
#ifdef _GL
  ThotBool   DblBuffNeedSwap;
  void       *Animated_Boxes;
  double     BeginTime;
  double     LastTime;
  ThotBool   Anim_play;  
  ThotBool   Scroll_enabled;  
#endif /*_GL*/

} Frame_Ctl;
#endif /* #if defined(_MOTIF) || defined(_GTK) || defined(_WINGUI) */

#if defined(_WX)
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
  int        FrDoc;                     /* Document ID                       */
  int        FrView;                    /* Presentation schema View          */
  
  ThotScrollBar WdScrollV;              /* Widget of Vertical Scroll         */
  ThotScrollBar WdScrollH;              /* Widget of Horizontal Scroll       */
  ThotFrame  	WdFrame;                /* Widget of the Document Frame      */

  struct     _Menu_Ctl *FrMenus;        /* First menu context                */
  int        MenuAttr;                  /* Attributes menu ID or -1          */
  int        MenuSelect;                /* Selection menu ID or -1           */
  int        MenuPaste;                 /* Menu including the Paste command or -1 */
  int        MenuContext;               /* Contextual menu ID or -1          */

  int        MenuShowPanelID;             /* parent menu id                         */
  int        MenuSplitViewID;             /* parent menu id                         */
  int        MenuFullScreenID;            /* parent menu id                         */
  int        MenuItemShowPanelID;         /* show/hide panel menu item ID or -1     */
  int        MenuItemSplitViewID;         /* split/unsplit menu item ID or -1       */
  int        MenuItemFullScreenID;        /* fullscreen on/off menu item ID or -1   */

  int        EntryPaste;                /* Entry number of the Paste command */
  int        MenuUndo;                  /* Menu including the Undo command
					   or -1  */
  int        EntryUndo;                 /* Entry number of the Undo command  */
  int        MenuRedo;                  /* Menu including the Redo command
					   or -1  */
  int        EntryRedo;                 /* Entry number of the Redo command  */
  ThotMenu   WdMenus[MAX_MENU];         /* List of menu Widgets              */
  ThotBool   EnabledMenus[MAX_MENU];    /* Enabled menus                     */

  ThotBool   EnabledButton[MAX_BUTTON]; /* Enabled toolbar buttons           */
  ThotBool   CheckedButton[MAX_BUTTON]; /* Checked toolbar buttons           */  

  bool       EnabledButton_Panel_XHTML[MAX_BUTTON]; /* Enabled XHTML panel buttons           */
  bool       CheckedButton_Panel_XHTML[MAX_BUTTON]; /* Checked XHTML panel buttons           */  

  Proc       Call_Text;                 /* This is the callback which is called when a url is activated */
  
#ifdef _GL
  ThotBool   DblBuffNeedSwap;
  void       *Animated_Boxes;
  double     BeginTime;
  double     LastTime;
  ThotBool   Anim_play;  
  ThotBool   Scroll_enabled;  
#endif /*_GL*/

} Frame_Ctl;

/*
 * wxWidgets Window_Ctl definition
 * this is the description of a Frame_Ctl container
 * It contains : toolbar, urlbar, pages
 * */
typedef struct _Window_Ctl {
  int        FrWidth;                   /* Window Width                      */
  int        FrHeight;                  /* Window Height                     */
  
  AmayaWindow * WdWindow;               /* Widget of the wxWidgets window    */  
  ThotStatusBar WdStatus;               /* Widget of the window status bar   */

  
  ThotBitmapButton 	Button[MAX_BUTTON];               /* List of toolbar button Widgets */
  Proc              Call_Button[MAX_BUTTON];          /* List of toolbar button Callbacks  */
} Window_Ctl;

#endif /* #if defined(_WX) */


#endif /* !NODISPLAY */

#endif /* _FRAME_H */
