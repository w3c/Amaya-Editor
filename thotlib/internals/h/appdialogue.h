/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 

#ifndef _APPDIALOG_H
#define _APPDIALOG_H

#include "frame.h"

/* Structure of an item in the Actions Table */
typedef struct _Action_Ctl
{
  char*     ActionName;	/* External name of the action		*/
  UserProc 	User_Action;	/* Address of the User procedure (Java)	*/
  void 		*User_Arg;	/* Arguments of the User procedure	*/
  Proc 		Call_Action;	/* Address C procedure            	*/
  STRING    ActionEquiv;	/* Displayed text for shortcuts		*/
  ThotBool	ActionActive[MAX_FRAME];
}Action_Ctl;

/* Structure to declare a Menu Item */
typedef struct _Item_Ctl
{
  int		ItemID;		/* ID of the menu item			*/
  char      ItemType;	/* 'B'=Button, 'T'=Toggle, 'D'=Dynamic	*/
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
  Item_Ctl	*ItemsList;	/* Pointer to the first item structure	*/
  struct _Menu_Ctl	*NextMenu;	/* Next menu		    	*/
}Menu_Ctl;

/* Structure to associate menus and specific structure schemas */
typedef struct _SchemaMenu_Ctl
{
  STRING     SchemaName;	/* Structure schema name		*/
  Menu_Ctl	*SchemaMenu;	/* Pointer to the first menu		*/
  struct _SchemaMenu_Ctl *NextSchema;	/* Next association	       	*/
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
#define T_chvisibility	8
#define T_rsvisibility	9
#define T_chzoom	10
#define T_rszoom	11
#define T_chselect	12
#define T_chattr	13
#define T_insertchar	14
#define T_editfunc	15
#define T_presentation	16
#define T_presentchoice	17
#define T_rsavedoc	18
#define T_savedoc	19
#define T_openview	20

#define T_createdoc	21
#define T_opendoc	22
#define T_import	23
#define T_confirmcreate	24
#define T_confirmclose	25
#define T_rconfirmclose	26
#define T_buildpathdocbuffer	27
#define T_insertpaste	28
#define T_rcinsertpaste	29
#define T_raskfornew	30
#define T_searchtext	31
#define T_locatesearch	32
#define T_searchpage	33
#define T_rchoice	34
#define T_present	35
#define T_presentstd	36
#define T_attrreq	37
#define T_rattrreq	38
#define T_rattrval	39
#define T_rattr		40
#define T_rattrlang	41
#define T_rselect	42
#define T_emptybox	43

#define T_corrector	44
#define T_rscorrector	45
#define T_indexcopy	46
#define T_indexnew	47
#define T_indexverif	48
#define T_indexschema	49
#define T_rsindex	50
#define T_rprint	51
#define T_rextprint	52

#define T_cancopyorcut	53
#define T_selectsiblings 54
#define T_selecbox	55
#define T_pastesibling	56
#define T_pastewithin	57
#define T_enter	        58
#define T_cmdpaste	59
#define T_deletenextchar 60

#define T_strsearchconstmenu	61
#define T_strsearchgetparams	62
#define T_strsearchonly		63
#define T_strsearcheletattr	64
#define T_strsearchshowvalattr	65
#define T_strsearchretmenu	66
#define T_createtable           67
#define T_selecttable           68
#define T_singlecell            69
#define T_attrtable             70
#define T_lastsaved             71
#define T_condlast              72
#define T_pastesiblingtable     73
#define T_refattr               74
#define T_ruleattr              75
#define T_createhairline        76
#define T_holotable             77
#define T_checkextens           78
#define T_abref                 79
#define T_vertspan              80
#define T_excepttable           81 
#define T_entertable            82
#define T_insertpage            83
#define T_cutpage               84
#define T_deletepage            85
#define T_deletepageab          86
#define T_searchemptyelt        87
#define T_searchemptyref        88
#define T_searchrefto           89

#define T_checkHiddenElement		90
#define T_checkReadOnlyElement		91
#define T_checkInsertNearElement	92
#define T_rchangetype			93
#define T_rsurround			94
#define T_rchangepres			95
#define T_checktable			96
#define T_checkcolumn			97
#define T_cleartable			98
#define T_resizetable                   99
#define T_firstcolumn			100
#define T_colupdates			101
#define T_backuponfatal			102
#define T_AIupdate			103
#define T_redisplay			104
#define T_writedocument                 105
#define T_setwritedirectory             106
#define T_xmlparsedoc			107
#define T_transformintotype		108
#define T_docmodified                   109
#define T_switchsel                     110
#define T_checksel                      111
#define T_resetsel                      112
#define T_selstring                     113
#define T_extendsel                     114
#define T_showbox                       115
#define T_freesavedel                   116
#define T_clearhistory                  117
#define T_openhistory                   118
#define T_addhistory                    119
#define T_attraddhistory                120
#define T_cancelhistory                 121
#define T_closehistory                  122

#define T_initevents                    123
#define MAX_LOCAL_ACTIONS               124

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
#define MAX_INTERNAL_CMD	28

#define CST_EquivDel TEXT("Delete")
#define CST_EquivBS TEXT("BackSpace")
#define CST_EquivPrior TEXT("Prior")
#define CST_EquivNext TEXT("Next")
#define CST_EquivHome TEXT("Home")
#define CST_EquivEnd TEXT("End")
#define CST_EquivLineUp TEXT("C Up")
#define CST_EquivLineDown TEXT("C Down")
#define CST_EquivScrollLeft TEXT("C Left")
#define CST_EquivScrollRight TEXT("C Right")
#endif
