/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef __LIBMANAG_H_
#define __LIBMANAG_H_

#define MAX_LENGTH     512

/* Structure de liste chaine pour la correspondance URI TITLE */
typedef struct _Link_Table
{
  char *URI_Title[MAX_LENGTH][2];
  struct _Link_Table *LTNext;
} Link_Table , *Link_TablePtr;

/* Prototypes des fonctions */
/* voir libmanag_f.h */

#endif /* __LIBMANAG_H_ */
