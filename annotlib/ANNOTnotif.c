#if 0
#ifdef ANNOTATIONS

/*
 * Module de gestion de la notification des annotations
 *
 */

#include "ANNOT.h"
#include "alliance_assistant.h"
#include "editorComs.h"

/*-----------------------------------------------------------------------
   Procedure ANNOT_NotifyLocalUsers (document, annotName)
  -----------------------------------------------------------------------
   Notification de la nouvelle annotation "annotName" creee dans le
   document courant dans un reseau local
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_NotifyLocalUsers (Document document, STRING annotName)
#else  /* __STDC__ */
void ANNOT_NotifyLocalUsers (document, annotName)
     Document document;
     STRING   annotName;
#endif /* __STDC__ */
{
  T_Editor  editors[MAX_ALLIANCE_EDITOR];
  char      coopFileName[DIRNAME_LENGTH];
  int       nbEditors, curDocIndex, length, i;
  struct    sockaddr_in dest;
  T_ThotMsg msg;

  printf ("(ANNOT_NotifyLocalUsers) DEBUT\n");

  curDocIndex = SearchByzDocument (document);
  sprintf (coopFileName,
	   "%s/%s/%s",
	   oDoc[curDocIndex].docDir,
	   oDoc[curDocIndex].docName,
	   DOCUMENT_COOP);
  nbEditors = EM_GetAllOtherEditors (coopFileName,
				     oDoc[curDocIndex].docEditorIndex,
				     editors);

  for (i = 0; i < nbEditors; i++)
  {
    msg.request = NEW_ANNOTATION_REQUEST;
    msg.documentRef = oDoc[curDocIndex].docRef;
    strcpy (msg.treeName, annotName);
    msg.fragmentId.g_FragId = 0;

    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = editors[i].node;
    dest.sin_port = editors[i].port;

    length = sizeof (T_ThotMsg);
    if ((length = sendto (ThotSocket, (char *) &msg, length,
                          0, (struct sockaddr *) & dest, sizeof (dest))) == -1)
    {
      perror ("<ANNOT_NotifyLocalUsers> Error on send call");
    }
  }

  printf ("(ANNOT_NotifyLocalUsers) FIN\n");

}

/*-----------------------------------------------------------------------
   Procedure ANNOT_NotifyToRemoteSites (document, annotName)
  -----------------------------------------------------------------------
   Notification de la nouvelle annotation "annotName" creee dans le
   document courant dans un reseau a grande distance
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void ANNOT_NotifyToRemoteSites (Document document, STRING annotName)
#else  /* __STDC__ */
void ANNOT_NotifyToRemoteSites (document, annotName)
     Document document;
     STRING   annotName;
#endif /* __STDC__ */
{
  T_msgMsqAnnot msg;
  int curDocIndex, messageSize;
  
  printf ("(ANNOT_NotifyToRemoteSites) DEBUT\n");

  /* Recuperation du numero "Alliance" du document cible de l'annotation */
  curDocIndex = SearchByzDocument (document);

  /* Preparation du message a envoyer */
  msg.mtext.msgType = MSG_NEW_ANNOTATION;
  msg.annot_DocRef = oDoc[curDocIndex].docRef;
  sprintf (msg.annot_Strings,
           "%s\1%s\1%s",
           oDoc[curDocIndex].userBase,
           oDoc[curDocIndex].docName,
           annotName);
  messageSize = sizeof (T_MsgAnnotBody) 
                - (sizeof (msg.annot_Strings) - strlen (msg.annot_Strings) + 1);

  /* Envoi du message */
  sendSpecificMsg ((T_msgMsq *) & msg, messageSize);

  printf ("(ANNOT_NotifyToRemoteSites) END\n");

}
 
#endif /* ANNOTATIONS */
#endif /* 0 */





