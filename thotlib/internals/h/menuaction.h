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
 
/* This file defines the data structures that allow an application to */
/* update standard menus */

#ifndef _MENUACTION_H_
#define _MENUACTION_H_

/* description d'un nom (de schema I, de menu, d'item ou d'action) */
typedef struct _AppName *PtrAppName;
typedef struct _AppName
{
  char      *AppNameValue;
  PtrAppName AppNextName;
  ThotBool   AppStandardName;
  /* TRUE when that action is declared in the section FUNCTION */
  ThotBool   AppFunction;
} AppName;

/* description d'un item de menu */
typedef struct _AppMenuItem *PtrAppMenuItem;
typedef struct _AppMenuItem
{
  char          *AppItemName;
  char          *AppItemActionName;
  char          *AppItemIconName;
  PtrAppMenuItem AppSubMenu;
  char           AppItemType;
  ThotBool       AppStandardAction;
  PtrAppMenuItem AppNextItem;
} AppMenuItem;

/* description d'un menu */
typedef struct _AppMenu *PtrAppMenu;
typedef struct _AppMenu
{
  char          *AppMenuName;
  int            AppMenuView;
  PtrAppMenuItem AppMenuItems;
  PtrAppMenu     AppNextMenu;
} AppMenu;

/* description des menus propres a un type de document */
typedef struct _AppDocType *PtrAppDocType;
typedef struct _AppDocType
{
  char         *AppDocTypeName;
  PtrAppMenu    AppDocTypeMenus;
  PtrAppDocType AppNextDocType;
} AppDocType;

#endif
