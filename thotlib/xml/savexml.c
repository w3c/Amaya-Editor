/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */

/*
 * Handle Xml Document Writing 
 *
 * Author: S. Bonhomme (INRIA)
 *         
 *
 */

#include "ustring.h"
#include "libmsg.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "fileaccess.h"
#include "message.h"
#include "appdialogue.h"

#include "applicationapi_f.h"
#include "callback_f.h"
#include "documentapi_f.h"
#include "fileaccess_f.h"
#include "parsexml_f.h"
#include "views_f.h"
#include "writedoc_f.h"
#include "writexml_f.h"

#define THOT_EXPORT extern
#include "appdialogue_tv.h"

static PathBuffer   SaveDirectoryName;
static PathBuffer   SaveFileName;
static ThotBool     SaveDocWithCopy;
static ThotBool     SaveDocWithMove;
static PtrDocument  DocumentToSave;
extern CHAR_T       DefaultFileSuffix[5];

/*----------------------------------------------------------------------
  XmlSetWriteDirectory sets the directory, filename, if the next write 
  is with copy and with move
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void              XmlSetWriteDirectory (PtrDocument pDoc, PathBuffer fileName, PathBuffer directoryName, ThotBool withCopy, ThotBool withMove)
#else  /* __STDC__ */
void              XmlSetWriteDirectory (pDoc, fileName, directoryName, withCopy, withMove)
PtrDocument    pDoc;
PathBuffer     fileName;
PathBuffer     directoryName;
ThotBool       withCopy;
ThotBool       withMove;
#endif /* __STDC__ */
{
  ustrcpy (SaveFileName, fileName);
  ustrcpy (SaveDirectoryName, directoryName);
  SaveDocWithCopy = withCopy;
  SaveDocWithMove = withMove;
  DocumentToSave = pDoc;
}


/*----------------------------------------------------------------------
   XmlSimpleSave: Saves a document in Xml format
                  Document's name is given by name
		  Returns FALSE if problem               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     XmlSimpleSave (Document doc, STRING name, ThotBool withEvent)
#else  /* __STDC__ */
static ThotBool     XmlSimpleSave (doc, name, withEvent)
Document            doc;
STRING              name;
ThotBool            withEvent;
#endif /* __STDC__ */
{
   BinFile             xmlFile;
   ThotBool            ok;

   xmlFile = TtaWriteOpen (name);
   if (xmlFile == 0)
     ok = FALSE;
   else
     {
       ok = SauveXmlDoc (xmlFile, doc, withEvent);
       TtaWriteClose (xmlFile);
     }
   return ok;
}


/*----------------------------------------------------------------------
   saveWithExtension saves the document without event nor user 
   notification in a file docname.extension
  return FALSE is the document could not be saved
                                                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     saveWithExtension (PtrDocument pDoc, STRING extension)
#else  /* __STDC__ */
static ThotBool     saveWithExtension (pDoc, extension)
PtrDocument         pDoc;
STRING              extension;

#endif /* __STDC__ */
{
   CHAR_T                buf[MAX_TXT_LEN];
   int                 i;

   if (pDoc == NULL)
      return FALSE;
   FindCompleteName (pDoc->DocDName, extension, pDoc->DocDirectory, buf, &i);
   if (XmlSimpleSave ((Document)IdentDocument (pDoc), buf, FALSE))
     {
/* 	UpdateAllInclusions (pDoc); */
	return TRUE;
     }
   else
     {
	TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, TMSG_WRITING_IMP), buf);
	return FALSE;
     }
}
/*----------------------------------------------------------------------
  interactiveSave saves the document with message and user notification
  return FALSE is the document could not be saved
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     interactiveSave (PtrDocument pDoc)

#else  /* __STDC__ */
ThotBool            interactiveSave (pDoc)
PtrDocument         pDoc;
#endif /* __STDC__ */

{
   ThotBool            ok;
   CHAR_T                docname[MAX_TXT_LEN];
   CHAR_T                buf[MAX_TXT_LEN];
   CHAR_T                buf2[MAX_TXT_LEN];
   int                 i;
   NotifyDialog         notifyDoc;


   ok = FALSE;
   if (pDoc != NULL)
     {
       if (pDoc->DocReadOnly)
	 /* on ne sauve pas les documents qui sont en lecture seule */
	 TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
       else if (pDoc->DocSSchema == NULL)
	 TtaDisplaySimpleMessage (INFO, LIB, TMSG_EMPTY_DOC_NOT_WRITTEN);
       else
	 {
	   CloseInsertion ();
	   notifyDoc.event = TteDocSave;
	   notifyDoc.document = (Document) IdentDocument (pDoc);
	   notifyDoc.view = 0;
	   if (CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	     /* l'application a pris la sauvegarde en charge */
	     ok = TRUE;
	   else
	     {
	       FindCompleteName (SaveFileName, Tmp_EXT2,
				 SaveDirectoryName, buf, &i);
	       ok = (FileWriteAccess (buf) == 0);
	       if (ok)
		 {
		   TtaDisplaySimpleMessage (INFO, LIB, TMSG_WRITING);
		   ok = XmlSimpleSave ((Document)IdentDocument (pDoc), buf, TRUE);
		   /* if ok, should update all inclusions */
		 }
	       FindCompleteName (SaveFileName, _XMLElement_, SaveDirectoryName,
				 docname, &i);
	       if (!ok)
		 {
		   /* on indique un nom connu de l'utilisateur... */
		   TtaDisplayMessage (CONFIRM, 
				      TtaGetMessage (LIB, TMSG_WRITING_IMP),
				      docname);
		 }
	       else
		 {
		   /* 1- faire mv .xml sur .xml.old */
		   FindCompleteName (SaveFileName, TEXT("xml.old"), 
				     SaveDirectoryName, buf2, &i);
		   i = urename (docname, buf2);
		   /* 2- faire mv du .Tmp sur le .xml */
		   i = urename (buf, docname);
		   if (i >= 0)
		     /* >> tout s'est bien passe' << */
		     /* detruit l'ancienne sauvegarde */
		     {
		       FindCompleteName (pDoc->DocDName, TEXT("xml~"), 
					 pDoc->DocDirectory, buf, &i);
		       TtaFileUnlink (buf);
		       TtaDisplayMessage (INFO, 
					  TtaGetMessage (LIB, TMSG_LIB_DOC_WRITTEN),
					  docname);
		       SetDocumentModified (pDoc, FALSE, 0);

		       /* should update external references files */
		       if ((ustrcmp (SaveFileName, pDoc->DocDName) != 0 ||
			    ustrcmp (SaveDirectoryName, pDoc->DocDirectory) != 0) &&
			   SaveDocWithMove)
			 {
			   FindCompleteName (pDoc->DocDName, _XMLElement_, 
					     pDoc->DocDirectory, buf, &i);
			   TtaFileUnlink (buf);
			 }
			   
			 			     
		       ustrncpy (pDoc->DocDName, SaveFileName, MAX_NAME_LENGTH);
		       ustrncpy (pDoc->DocIdent, SaveFileName, MAX_DOC_IDENT_LEN);
		       ustrncpy (pDoc->DocDirectory, SaveDirectoryName, MAX_PATH);
		       ChangeDocumentName (pDoc, SaveFileName);
		       notifyDoc.event = TteDocSave;
		       notifyDoc.document = (Document) IdentDocument (pDoc);
		       notifyDoc.view = 0;
		       CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		     }
		 }
	     }
	 }
     }
   return ok;
}		       
		       


/*----------------------------------------------------------------------
   XmlWriteDocument saves the document pDoc in Xml form    
   pDoc. Returne True if the document have been saved,     
   False if the save failed.                                          
   - mode = 0 : asks the filename to user
   - mode = 1 : autosave 
   - mode = 2 : scratch file (no message)           
   - mode = 3 : emergency save (.SAV)       
   - mode = 4 : saves without asking filename
   - mode = 5 : saves without asking filename and without message
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            XmlWriteDocument (PtrDocument pDoc, int mode)

#else  /* __STDC__ */
ThotBool            XmlWriteDocument (pDoc, mode)
PtrDocument         pDoc;
int                 mode;

#endif /* __STDC__ */

{
  ThotBool ok = FALSE;
  
  /* save old documents in piv format */
  if (pDoc != NULL && 
      pDoc->DocPivotVersion < 6 && 
      pDoc->DocPivotVersion != -1 && 
      mode != 0)
    ok = WriteDocument (pDoc, mode);
 
  else if (mode >= 0 && mode <= 5)
    switch (mode)
      {
      case 0:
	if (DocumentToSave == pDoc)
	  ok = interactiveSave (pDoc);
	break;
      case 1:
	ok = saveWithExtension (pDoc, TEXT("xml~"));
	if (ok)
	  TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_DOC_WRITTEN), pDoc->DocDName);
	break;
      case 2:
	ok = saveWithExtension (pDoc, TEXT("xml~"));
	break;
      case 3:
	ok = saveWithExtension (pDoc, TEXT("xml.sav"));
	break;
      case 4:
	XmlSetWriteDirectory (pDoc, pDoc->DocDName, pDoc->DocDirectory, FALSE, FALSE);
	ok = interactiveSave (pDoc);
	break;
      case 5:
	ok = saveWithExtension (pDoc, _XMLElement_);
	break;
      }
  return ok;
}


/*----------------------------------------------------------------------
  XmlLoadResources
  intialises the pivot format save resource
  ----------------------------------------------------------------------*/
void XmlLoadResources()
{
  if (ThotLocalActions[T_writedocument] == NULL)
    {
      TteConnectAction (T_writedocument, (Proc) XmlWriteDocument);
      TteConnectAction (T_setwritedirectory, (Proc) XmlSetWriteDirectory);
    }
  XmlParserLoadResources ();
  ustrcpy (DefaultFileSuffix, XML_EXT);

}
