/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _APPDIALOG_H
#define _APPDIALOG_H

#include "frame.h"

/* Structure of an item in the Actions Table */
typedef struct _Action_Ctl
{
  char         *ActionName;    /* External name of the action           */
  Proc 		Call_Action;   /* Address C procedure                   */
  char         *ActionEquiv;   /* Displayed text for shortcuts          */
  ThotBool	ActionActive[MAX_FRAME];
}Action_Ctl;

/* Structure to declare a Menu Item */
typedef struct _Item_Ctl
{
  int		ItemID;		/* ID of the menu item			*/
  char          ItemType;	/* 'B'=Button, 'T'=Toggle, 'D'=Dynamic	*/
  				/* 'S'=Separator, 'M'=Menu		*/
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
  int		ItemsNb;	/* Number of items in the menu menu	*/
  Item_Ctl     *ItemsList;	/* Pointer to the first item structure	*/
  struct _Menu_Ctl	*NextMenu;	/* Next menu		    	*/
}Menu_Ctl;

/* Structure to associate menus and specific structure schemas */
typedef struct _SchemaMenu_Ctl
{
  char      *SchemaName;                  /* Structure schema name     */
  Menu_Ctl  *SchemaMenu;	                 /* Pointer to the first menu */
  struct     _SchemaMenu_Ctl *NextSchema; /* Next association          */
}SchemaMenu_Ctl;

/*
  Predefined indexes in the internal default Thot actions table.
  This table is used by the Thotlib to accede optional actions. It tests
  the availability of the current action (code loaded by the application)
  before calling it.
  */
#define T_colors	0
#define T_keyboard	1
#define T_imagemenu	2
#define T_updateparagraph	3
#define T_stopinsert	4
#define T_pasteclipboard	5
#define T_chsplit	6
#define T_chselect	7
#define T_chattr	8
#define T_insertchar	9
#define T_editfunc	10
#define T_confirmclose	11
#define T_rconfirmclose	12
#define T_insertpaste	13
#define T_rcinsertpaste	14
#define T_clearhistory                  15
#define T_openhistory                   16
#define T_addhistory                    17
#define T_attraddhistory                18
#define T_cancelhistory                 19
#define T_closehistory                  20
#define T_lock                          21
#define T_unlock                        22
#define T_islock                        23
#define T_backuponfatal			24
#define T_switchsel                     25
#define T_checksel                      26
#define T_resetsel                      27
#define T_selstring                     28
#define T_selectsiblings 29
#define T_searchtext	30
#define T_locatesearch	31
#define T_freesearch	32
#define T_rchoice	33
#define T_present	34
#define T_presentstd	35
#define T_attrreq	36
#define T_rattrreq	37
#define T_rattrval	38
#define T_rattr		39
#define T_rattrlang	40
#define T_rselect	41
#define T_emptybox	42
#define T_freesavedel   43
#define T_docmodified   44
#define T_checkHiddenElement		45
#define T_checkReadOnlyElement		46
#define T_checkInsertNearElement	47
#define T_AIupdate			48
#define T_initevents			49
#define T_rprint	50
#define T_deletepage            51
#define T_deletepageab          52
#define T_redisplay		53
#define T_writedocument         54
#define T_enter	                55
#define T_cmdpaste	        56
#define T_deletenextchar        57
#define T_checktable            58
#define T_cleartable		59
#define T_resizetable           60
#define T_firstcolumn		61
#define T_checkcolumn		62
#define T_checktableheight      63
#define T_createtable           64
#define T_selecttable           65
#define T_singlecell            66
#define T_attrtable             67
#define T_abref                 68
#define T_vertspan              69
#define T_excepttable           70 
#define T_lastsaved             71
#define T_condlast              72
#define T_pastesiblingtable     73
#define T_refattr               74
#define T_ruleattr              75
#define T_createhairline        76
#define T_holotable             77
#define T_checkextens           78
#define T_entertable            82
#define T_insertpage            83
#define T_cutpage               84
#define T_autosave              85
#define MAX_LOCAL_ACTIONS       86

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
#define MAX_INTERNAL_CMD	37

#define CST_EquivLineUp "C Up"
#define CST_EquivScrollLeft "C Left"
#endif

