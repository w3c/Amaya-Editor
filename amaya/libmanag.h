/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef __LIBMANAG_H_
#define __LIBMANAG_H_

/* Structure de liste chaine pour la correspondance URI TITLE */
typedef struct _Link_Table
{
  char *URI_Title[MAX_LENGTH][2];
  struct _Link_Table *LTNext;
} Link_Table , *Link_TablePtr;

/* Prototypes des fonctions */
/* voir libmanag_f.h */
int GetWindowNumber (Document document, View view);
unsigned char *GetScreenshot (int frame, char *pngurl);

#endif /* __LIBMANAG_H_ */
