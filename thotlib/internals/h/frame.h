/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
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

typedef struct _Frame_Ctl {
  int        FrTopMargin;               /* Window Top Margin                 */
  int        FrScrollOrg;               /* Scrolling origin                  */
  int        FrScrollWidth;             /* Scrolling width                   */
  int        FrWidth;                   /* Window Width                      */
  int        FrHeight;                  /* Window Height                     */
  int        FrDoc;                     /* Document ID                       */
  int        FrView;                    /* Presentation schema View          */
  ThotWidget WdScrollV;                 /* Widget of Vertical Scroll         */
  ThotWidget WdScrollH;                 /* Widget of Horizontal Scroll       */
  ThotWidget WdFrame;                   /* Widget of the Document Frame      */
  ThotWidget WdStatus;                  /* Widget of the Document status     */
  struct     _Menu_Ctl *FrMenus;        /* First menu context                */
  int        MenuAttr;                  /* Attributes menu ID or -1          */
  int        MenuSelect;                /* Selection menu ID or -1           */
  int        MenuPaste;                 /* Menu including the Paste command
					   or -1 */
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
#ifdef _WINDOWS
  int        ButtonId[MAX_BUTTON];
  char      *TbStrings[MAX_BUTTON];     /* Tooltips text                     */
  ThotWidget Label;                     /* Labels of text zones              */
#else /* _WINDOWS */
  ThotWidget Row_Zone;                  /* Parent widget of Text_Zone        */
  ThotWidget Combo;                     /* List of combobox Widgets          */
#endif /* _WINDOWS */
  ThotWidget Text_Zone;                 /* List of text-zone Widgets         */
  Proc       Call_Text;                 /* List of text-zone Callbacks       */

#ifdef _GL
  ThotBool   DblBuffNeedSwap;
  void       *Animated_Boxes;
  double     BeginTime;
  ThotBool   Anim_play;  
  int        Timer;  
#endif /*_GL*/

} Frame_Ctl;
#endif /* !NODISPLAY */

#endif
