/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* This file defines the data structures that allow an application to */
/* update standard menus */

#ifndef _MENUACTION_H_
#define _MENUACTION_H_

/* description d'un nom (de schema I, de menu, d'item ou d'action) */
typedef struct _AppName *PtrAppName;
typedef struct _AppName
{
  char			*AppNameValue;
  boolean		AppStandardName;
  PtrAppName		AppNextName;
} AppName;

/* description d'un item de menu */
typedef struct _AppMenuItem *PtrAppMenuItem;
typedef struct _AppMenuItem
{
  char			*AppItemName;
  char			*AppItemActionName;
  PtrAppMenuItem	AppSubMenu;
  char			AppItemType;
  boolean		AppStandardAction;
  PtrAppMenuItem	AppNextItem;
} AppMenuItem;

/* description d'un menu */
typedef struct _AppMenu *PtrAppMenu;
typedef struct _AppMenu
{
  char			*AppMenuName;
  int			AppMenuView;
  PtrAppMenuItem	AppMenuItems;
  PtrAppMenu		AppNextMenu;
} AppMenu;

/* description des menus propres a un type de document */
typedef struct _AppDocType *PtrAppDocType;
typedef struct _AppDocType
{
  char			*AppDocTypeName;
  PtrAppMenu		AppDocTypeMenus;
  PtrAppDocType		AppNextDocType;
} AppDocType;

#endif
