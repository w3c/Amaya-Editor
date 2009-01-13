/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _APPDIALOG_H
#define _APPDIALOG_H

#include "frame.h"
#include "typebase.h"

/* Structure of an item in the Actions Table */
typedef struct _Action_Ctl
{
  const char         *ActionName;    /* External name of the action           */
  Proc 		Call_Action;   /* Address C procedure                   */
  char         *ActionEquiv;   /* Displayed text for shortcuts          */
#ifdef _WX
  /* possible optim: these 2 field could be replaced by a unique one with multi flags support */
  ThotBool	ActionActive[MAX_DOCUMENTS]; /* used to know if the corresponding menu item is enabled or not */
  ThotBool	ActionToggle[MAX_DOCUMENTS]; /* used to know if the corresponding menu item is toggled or not */
#else /* _WX */
  ThotBool	ActionActive[MAX_FRAME];
#endif /* _WX */
}Action_Ctl;

/* Structure to declare a Menu Item */
typedef struct _Item_Ctl
{
  int		ItemID;		   /* ID of the menu item			*/
  char          ItemType;	   /* 'B'=Button, 'T'=Toggle, 'D'=Dynamic	*/
  				   /* 'S'=Separator, 'M'=Menu		*/
  char          ItemIconName[100]; /* a icon resource identifier to show in the menu near the item (on the left) */
  union
  {
    struct
    {
      int	_ItemAction;	/* ID of the linked action		*/
    } s0;
    struct
    {
      struct _Menu_Ctl	*_SubMenu; /* Pointer to the linked sub-menu	*/
    } s1;
  } u;
}Item_Ctl;
#define ItemAction u.s0._ItemAction
#define SubMenu u.s1._SubMenu

/* Structure to declare a Menu */
typedef struct _Menu_Ctl
{
  int		MenuID;		/* ID of the menu			*/
  int		MenuView;	/* Specific view number or Null		*/
  ThotBool	MenuAttr;	/* This menu is the attributes menu	*/
  ThotBool	MenuSelect;	/* This menu is the select menu		*/
  ThotBool	MenuHelp;	/* This menu is the help menu		*/
  ThotBool	MenuContext;	/* This menu is the contextual menu     */
  ThotBool  MenuDocContext;  /* This menu is the contextual menu inside documents */
  int		ItemsNb;	/* Number of items in the menu menu	*/
  Item_Ctl     *ItemsList;	/* Pointer to the first item structure	*/
  struct _Menu_Ctl	*NextMenu;	/* Next menu		    	*/
}Menu_Ctl;

/*
  Predefined indexes in the internal default Thot actions table.
  This table is used by the Thotlib to accede optional actions. It tests
  the availability of the current action (code loaded by the application)
  before calling it.
  */
#define T_backuponfatal	0
#define T_chsplit	      1
#define T_chattr        2
#define T_confirmclose	3
#define T_rconfirmclose	4
#define T_insertpaste	  5
#define T_rcinsertpaste	6
#define T_searchtext	  7
#define T_locatesearch	8
#define T_rchoice	      9
#define T_present	     10
#define T_presentstd	 11
#define T_attrreq	     12
#define T_rattrreq	   13
#define T_rattrval	   14
#define T_rattr		     15
#define T_rattrlang	   16
#define T_docmodified  17
#define T_checkHiddenElement		        18
#define T_checkReadOnlyElement		      19
#define T_checkInsertNearElement	      20
#define T_initevents			              21
#define T_autosave                      22
#define T_writedocument                 23
#define T_clearhistory                  24
#define T_openhistory                   25
#define T_addhistory                    26
#define T_attraddhistory                27
#define T_cancelhistory                 28
#define T_closehistory                  29
#define MAX_LOCAL_ACTIONS  30

/*
 Predefined idexes of Thot actions which could be linked to shortcuts even
 if they are not part of any menu.
 */
#define CMD_DeletePrevChar	1
#define CMD_DeleteSelection	2
#define CMD_PreviousChar	3
#define CMD_NextChar		4
#define CMD_PreviousLine	5
#define CMD_NextLine		6
#define CMD_BeginningOfLine	7
#define CMD_EndOfLine		8
#define CMD_ParentElement	9
#define CMD_PreviousElement	10
#define CMD_NextElement		11
#define CMD_ChildElement	12
#define CMD_PageUp		13
#define CMD_PageDown		14
#define CMD_PageTop		15
#define CMD_PageEnd		16
#define CMD_CreateElement	17
#define CMD_CopyToClipboard	18
#define CMD_PasteFromClipboard	19
#define CMD_PreviousSelChar	20
#define CMD_NextSelChar		21
#define CMD_PreviousSelLine	22
#define CMD_NextSelLine		23
#define CMD_LineUp		24
#define CMD_LineDown		25
#define CMD_ScrollLeft		26
#define CMD_ScrollRight		27
#define CMD_LineBreak           28
#define CMD_PreviousWord        29
#define CMD_NextWord            30
#define CMD_PreviousSelWord     31
#define CMD_NextSelWord         32
#define CMD_SelBeginningOfLine  33
#define CMD_SelEndOfLine	34
#define CMD_Copy                35
#define CMD_Paste               36
#define CMD_CutSelection        37
#define CMD_SelPageUp		38
#define CMD_SelPageDown		39
#define MAX_INTERNAL_CMD	40

#define CST_EquivLineUp "C Up"
#define CST_EquivScrollLeft "C Left"
#endif

