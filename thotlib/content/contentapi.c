
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "application.h"
#include "document.h"
#include "tree.h"
#include "view.h"
#include "selection.h"
#include "language.h"
#ifndef NODISPLAY
#include "frame.h"
#endif
#include "typecorr.h"

#include "arbabs.f"
#include "attribut.f"
#include "commun.f"
#include "font.f"
#include "imagedrvr.f"
#include "modpres.f"
#include "modimabs.f"
#include "memory.f"
#include "select.f"
#ifndef NODISPLAY
#include "sel.f"
#endif
#include "textelem.f"
#include "thotmsg.f"


#undef EXPORT
#define EXPORT
#include "edit.var"
#ifndef NODISPLAY
#include "frame.var"
#endif

extern int          UserErrorCode;


/* ----------------------------------------------------------------------
   TtaSetTextContent

   Changes the content of a Text basic element. The full content (if any) is
   deleted and replaced by the new one.
   This function can also be used for changing the content (the file name)
   of a Picture basic element.

   Parameters:
   element: the Text element to be modified.
   content: new content for that element.
   language: language of that Text element.
   document: the document containing that element.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetTextContent (Element element, char *content, Language language, Document document)

#else  /* __STDC__ */
void                TtaSetTextContent (element, content, language, document)
Element             element;
char               *content;
Language            language;
Document            document;

#endif /* __STDC__ */

{
   PtrTextBuffer      pBuf, pBufPrec, pBufSuiv;
   char               *ptr;
   int                 length, l, delta;
   PtrElement          pEl;

#ifndef NODISPLAY
   PtrDocument         selDoc;
   PtrElement          premSel, derSel;
   int                 premCar, derCar;
   boolean             selOk, changeSelection;

#endif

   UserErrorCode = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElLeafType != LtText &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	/* verifie le parametre document */
	if (document < 1 || document > MAX_DOCUMENTS)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else if (TabDocuments[document - 1] == NULL)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else
	   /* parametre document correct */
	  {
#ifndef NODISPLAY
	     /* modifie la selection si l'element en fait partie */
	     selOk = SelEditeur (&selDoc, &premSel, &derSel, &premCar, &derCar);
	     changeSelection = False;
	     if (selOk)
		if (selDoc == TabDocuments[document - 1])
		   if ((PtrElement) element == premSel ||
		       (PtrElement) element == derSel)
		      /* la selection commence et/ou se termine dans */
		      /* l'element. On annule la selection d'abord */
		     {
			TtaSelectElement (document, NULL);
			changeSelection = True;
			if (derCar > 1)
			   derCar -= 1;
			pEl = (PtrElement) element;
			if (pEl == premSel)
			   /* notre element est en tete de la selection */
			  {
			     premCar = 0;
			     if (pEl == derSel)
				/* la selection se reduit a cet element, on */
				/* selectionnera tout l'element */
				derCar = 0;
			  }
			else if (pEl == derSel)
			   /* notre element est en queue de la selection */
			   /* on selectionnera jusqu'a la fin de l'element */
			   derCar = 0;
		     }
#endif
	     ptr = content;
	     length = strlen (content);
	     delta = length - ((PtrElement) element)->ElTextLength;
	     ((PtrElement) element)->ElTextLength = length;
	     ((PtrElement) element)->ElVolume = length;
	     if (((PtrElement) element)->ElLeafType == LtText)
		((PtrElement) element)->ElLanguage = language;
	     pBufPrec = NULL;
	     pBuf = ((PtrElement) element)->ElText;

	     do
	       {
		  if (pBuf == NULL)
		     GetBufTexte (&pBuf);
		  if (length >= MAX_CHAR)
		     l = MAX_CHAR - 1;
		  else
		     l = length;
		  if (l > 0)
		    {
		       strncpy (pBuf->BuContent, ptr, l);
		       ptr += l;
		       length -= l;
		    }
		  pBuf->BuLength = l;
		  pBuf->BuContent[l] = '\0';
		  pBuf->BuPrevious = pBufPrec;
		  if (pBufPrec == NULL)
		     ((PtrElement) element)->ElText = pBuf;
		  else
		     pBufPrec->BuNext = pBuf;
		  pBufPrec = pBuf;
		  pBuf = pBuf->BuNext;
		  pBufPrec->BuNext = NULL;
	       }
	     while (length > 0);

	     while (pBuf != NULL)
		/* libere les buffers qui restent */
	       {
		  pBufSuiv = pBuf->BuNext;
#ifdef NODISPLAY
		  FreeBufTexte (pBuf);
#else
		  DestBuff (pBuf, ActifFen);
#endif
		  pBuf = pBufSuiv;
	       }
	     /* met a jour le volume des elements ascendants */
	     pEl = ((PtrElement) element)->ElParent;
	     while (pEl != NULL)
	       {
		  pEl->ElVolume += delta;
		  pEl = pEl->ElParent;
	       }
	     if (((PtrElement) element)->ElLeafType == LtPicture)
	       {
		  /* On libere la pixmap */
		  if (((PtrElement) element)->ElImageDescriptor != NULL)
		     FreeImage ((ImageDescriptor *) (((PtrElement) element)->ElImageDescriptor));
	       }

#ifndef NODISPLAY
	     RedispLeaf ((PtrElement) element, document, delta);
	     /* etablit une nouvelle selection si l'element en fait partie */
	     if (changeSelection)
	       {
		  if (premCar > 0)
		     TtaSelectString (document, (Element) premSel, premCar, 0);
		  else
		     TtaSelectElement (document, (Element) premSel);
		  if (derSel != premSel)
		     TtaExtendSelection (document, (Element) derSel, derCar);
	       }
#endif
	  }
     }
}

	/* InsertText   insere la chaine de caracteres "content" dans l'element
	   pointe' par pEl (qui doit etre de type Texte), a la position
	   "position". On travaille pour le document "document". Si document
	   est nul, on ne s'occupe ni de selection ni d'affichage. */

#ifdef __STDC__
void                InsertText (PtrElement pEl, int position, char *content, Document document)

#else  /* __STDC__ */
void                InsertText (pEl, position, content, document)
PtrElement          pEl;
int                 position;
char               *content;
Document            document;

#endif /* __STDC__ */

{
   PtrTextBuffer      pBuf, pBufPrec, newBuf;
   char               *ptr;
   int                 stringLength, l, delta, lengthBefore;
   PtrElement          pElAsc;

#ifndef NODISPLAY
   PtrDocument         selDoc;
   PtrElement          premSel, derSel;
   int                 premCar, derCar;
   boolean             selOk, changeSelection;

#endif

   stringLength = strlen (content);
   if (stringLength > 0)
     {
	/* corrige eventuellement le parametre position */
	if (position > pEl->ElTextLength)
	   position = pEl->ElTextLength;
#ifndef NODISPLAY
	if (document > 0)
	  {
	     /* modifie la selection si l'element en fait partie */
	     selOk = SelEditeur (&selDoc, &premSel, &derSel, &premCar, &derCar);
	     changeSelection = False;
	     if (selOk)
		if (selDoc == TabDocuments[document - 1])
		   if (pEl == premSel || pEl == derSel)
		      /* la selection commence et/ou se termine dans */
		      /* l'element. On annule la selection d'abord */
		     {
			TtaSelectElement (document, NULL);
			changeSelection = True;
			if (derCar > 1)
			   derCar -= 1;
			if (pEl == premSel)
			   /* notre element est en tete de la selection */
			  {
			     if (premCar > position)
				premCar += stringLength;
			  }
			if (pEl == derSel)
			   /* notre element est en queue de la selection */
			  {
			     if (position < derCar)
				derCar += stringLength;
			  }
		     }
	  }
#endif
	delta = stringLength;
	pEl->ElTextLength += stringLength;
	pEl->ElVolume += stringLength;
	/* cherche le buffer pBuf ou` se place l'insertion */
	pBuf = pEl->ElText;
	lengthBefore = 0;
	while (pBuf->BuNext != NULL && lengthBefore + pBuf->BuLength < position)
	  {
	     lengthBefore += pBuf->BuLength;
	     pBuf = pBuf->BuNext;
	  }
	/* longueur qui doit rester dans le buffer coupe' */
	lengthBefore = position - lengthBefore;
	if (lengthBefore == 0)
	   /* on insere avant le 1er caractere de l'element */
	  {
	     /* on ajoute un buffer avant ceux qui existent deja */
	     GetBufTexte (&pBuf);
	     pBuf->BuNext = pEl->ElText;
	     pBuf->BuPrevious = NULL;
	     if (pBuf->BuNext != NULL)
		pBuf->BuNext->BuPrevious = pBuf;
	     pEl->ElText = pBuf;
	  }
	else
	   /* cree un buffer pour la 2eme partie de texte */
	if (lengthBefore < pBuf->BuLength)
	  {
	     newBuf = NewTextBuffer (pBuf);
	     strcpy (newBuf->BuContent, pBuf->BuContent + lengthBefore);
	     newBuf->BuLength = pBuf->BuLength - lengthBefore;
	  }
	pBuf->BuContent[lengthBefore] = '\0';
	pBuf->BuLength = lengthBefore;
	/* s'il y a assez de place dans le buffer,
	   on ajoute la chaine a la fin du buffer */
	if (stringLength < MAX_CHAR - lengthBefore)
	  {
	     strncpy (pBuf->BuContent + lengthBefore, content, stringLength);
	     pBuf->BuLength += stringLength;
	     pBuf->BuContent[pBuf->BuLength] = '\0';
	  }
	else
	   /* pas assez de place, on cree de nouveaux buffers */
	  {
	     pBufPrec = pBuf;
	     pBuf = NULL;
	     ptr = content;
	     while (stringLength > 0)
	       {
		  if (pBuf == NULL)
		     pBuf = NewTextBuffer (pBufPrec);
		  if (stringLength >= MAX_CHAR)
		     l = MAX_CHAR - 1;
		  else
		     l = stringLength;
		  strncpy (pBuf->BuContent, ptr, l);
		  ptr += l;
		  stringLength -= l;
		  pBuf->BuLength = l;
		  pBuf->BuContent[l] = '\0';
		  pBufPrec = pBuf;
		  pBuf = NULL;
	       }
	  }
	/* met a jour le volume des elements ascendants */
	pElAsc = pEl->ElParent;
	while (pElAsc != NULL)
	  {
	     pElAsc->ElVolume += delta;
	     pElAsc = pElAsc->ElParent;
	  }
#ifndef NODISPLAY
	if (document > 0)
	  {
	     RedispLeaf (pEl, document, delta);
	     /* etablit une nouvelle selection si l'element en fait partie */
	     if (changeSelection)
	       {
		  if (premCar > 0)
		     if (derSel == premSel)
			TtaSelectString (document, (Element) premSel, premCar, derCar);
		     else
			TtaSelectString (document, (Element) premSel, premCar, 0);
		  else
		     TtaSelectElement (document, (Element) premSel);
		  if (derSel != premSel)
		     TtaExtendSelection (document, (Element) derSel, derCar);
	       }
	  }
#endif
     }
}


/* ----------------------------------------------------------------------
   TtaAppendTextContent

   Appends a character string at the end of a Text basic element.

   Parameters:
   element: the Text element to be modified.
   content: the character string to be appended.
   document: the document containing that element.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaAppendTextContent (Element element, char *content, Document document)

#else  /* __STDC__ */
void                TtaAppendTextContent (element, content, document)
Element             element;
char               *content;
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
      TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtText &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
      TtaError (ERR_invalid_element_type);
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      InsertText ((PtrElement) element,
		  ((PtrElement) element)->ElTextLength,
		  content, document);
}


/* ----------------------------------------------------------------------
   TtaInsertTextContent

   Inserts a character string in a text basic element.

   Parameters:
   element: the Text element to be modified.
   position: rank of the character after which the new string must
   be inserted. 0 for inserting before the first character.
   content: the character string to be inserted.
   document: the document containing the text element.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaInsertTextContent (Element element, int position, char *content, Document document)

#else  /* __STDC__ */
void                TtaInsertTextContent (element, position, content, document)
Element             element;
int                 position;
char               *content;
Document            document;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
      TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtText &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
      TtaError (ERR_invalid_element_type);
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      InsertText ((PtrElement) element, position, content, document);
}

/* ----------------------------------------------------------------------
   TtaDeleteTextContent

   Deletes a character string in a text basic element.

   Parameters:
   element: the Text element to be modified.
   position: rank of the first character to be deleted.
   length: length of the character string to be deleted.
   document: the document containing the text element.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaDeleteTextContent (Element element, int position, int length, Document document)

#else  /* __STDC__ */
void                TtaDeleteTextContent (element, position, length, document)
Element             element;
int                 position;
int                 length;
Document            document;

#endif /* __STDC__ */

{
   PtrTextBuffer      pBufFirst, pBufLast, pBufNext;
   char               *dest, *source;
   int                 delta, lengthBefore, firstDeleted, lastDeleted,
                       l;
   PtrElement          pElAsc;

#ifndef NODISPLAY
   PtrDocument         selDoc;
   PtrElement          premSel, derSel;
   int                 premCar, derCar;
   boolean             selOk, changeSelection;

#endif

   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
      TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtText &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
      TtaError (ERR_invalid_element_type);
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      if (length < 0 || position <= 0 ||
	  position > ((PtrElement) element)->ElTextLength)
      TtaError (ERR_invalid_parameter);
   else if (length > 0)
     {
	/* verifie que le parametre length n'est pas trop grand */
	if (position + length > ((PtrElement) element)->ElTextLength + 1)
	   length = ((PtrElement) element)->ElTextLength - position + 1;
#ifndef NODISPLAY
	/* modifie la selection si l'element en fait partie */
	selOk = SelEditeur (&selDoc, &premSel, &derSel, &premCar, &derCar);
	changeSelection = False;
	if (selOk)
	   if (selDoc == TabDocuments[document - 1])
	      if ((PtrElement) element == premSel || (PtrElement) element == derSel)
		 /* la selection commence et/ou se termine dans */
		 /* l'element. On annule la selection d'abord */
		{
		   TtaSelectElement (document, NULL);
		   changeSelection = True;
		   if (derCar > 1)
		      derCar -= 1;
		   if ((PtrElement) element == premSel)
		      /* notre element est en tete de la selection */
		     {
			if (premCar > position)
			  {
			     premCar -= length;
			     if (premCar < position)
				premCar = position;
			  }
		     }
		   if ((PtrElement) element == derSel)
		      /* notre element est en queue de la selection */
		     {
			if (position < derCar)
			  {
			     derCar -= length;
			     if (derCar < position)
				derCar = position;
			  }
		     }
		}
#endif
	delta = length;
	((PtrElement) element)->ElTextLength -= delta;
	((PtrElement) element)->ElVolume -= delta;
	/* cherche le buffer pBufFirst ou commence la chaine a */
	/* retirer */
	pBufFirst = ((PtrElement) element)->ElText;
	lengthBefore = 0;
	while (pBufFirst->BuNext != NULL &&
	       lengthBefore + pBufFirst->BuLength < position)
	  {
	     lengthBefore += pBufFirst->BuLength;
	     pBufFirst = pBufFirst->BuNext;
	  }
	/* longueur qui doit rester dans le buffer contenant le */
	/* debut de la chaine a retirer */
	firstDeleted = position - lengthBefore;
	/* cherche le buffer pBufLast contenant la fin de la chaine  */
	/* a retirer et libere les buffers intermediaires (on ne     */
	/* libere pas le buffer qui contient le debut de la chaine a */
	/* supprimer, ni celui qui contient la fin de cette chaine)/ */
	pBufLast = pBufFirst;
	lastDeleted = firstDeleted + length - 1;
	while (pBufLast->BuNext != NULL &&
	       lastDeleted > pBufLast->BuLength)
	  {
	     lastDeleted -= pBufLast->BuLength;
	     if (pBufLast != pBufFirst)
		/* ce n'est pas le buffer contenant le debut de chaine, */
		/* on libere ce buffer. */
	       {
		  pBufNext = pBufLast->BuNext;
		  pBufLast->BuPrevious->BuNext = pBufLast->BuNext;
		  if (pBufLast->BuNext != NULL)
		     pBufLast->BuNext->BuPrevious = pBufLast->BuPrevious;
#ifdef NODISPLAY
		  FreeBufTexte (pBufLast);
#else
		  DestBuff (pBufLast, ActifFen);
#endif
		  pBufLast = pBufNext;
	       }
	     else
		pBufLast = pBufLast->BuNext;
	  }
	/* on decale vers la droite le texte qui suit la chaine a */
	/* supprimer */
	if (pBufFirst == pBufLast)
	   /* toute la chaine a supprimer est dans le meme buffer, */
	   /* on va decaler le texte qui suit la cahine a supprimer */
	   /* pour l'amener a la position du debut de la chaine a */
	   /* supprimer */
	  {
	     dest = pBufFirst->BuContent + firstDeleted - 1;
	     l = length;
	  }
	else
	   /* le debut et la fin de la chaine a supprimer sont dans */
	   /* buffers differents. On tronque simplement le texte du */
	   /* premier buffer et on va decaler le texte qui doit */
	   /* rester dans le buffer de fin pour l'amener en tete de */
	   /* ce buffer. */
	  {
	     pBufFirst->BuContent[firstDeleted - 1] = '\0';
	     pBufFirst->BuLength = firstDeleted - 1;
	     dest = pBufLast->BuContent;
	     l = lastDeleted;
	  }
	/* on decale effectivement le texte qui suit la partie */
	/* supprimee */
	source = pBufLast->BuContent + lastDeleted - 1;
	do
	  {
	     source++;
	     *dest = *source;
	     dest++;
	  }
	while (*source != '\0');
	pBufLast->BuLength -= l;
	/* on verifie si les buffers de debut et de fin sont devenus */
	/* vides, et si oui, on les libere. On garde quand meme au   */
	/* moins un buffer pour l'element. */
	if (pBufFirst->BuLength == 0 &&
	    (pBufFirst->BuPrevious != NULL || pBufFirst->BuNext != NULL))
	   /* le buffer de debut est vide et il reste au moins un */
	   /* autre buffer. On libere le buffer de debut */
	  {
	     if (pBufFirst->BuPrevious != NULL)
		pBufFirst->BuPrevious->BuNext = pBufFirst->BuNext;
	     else
		((PtrElement) element)->ElText = pBufFirst->BuNext;
	     if (pBufFirst->BuNext != NULL)
		pBufFirst->BuNext->BuPrevious = pBufFirst->BuPrevious;
#ifdef NODISPLAY
	     FreeBufTexte (pBufFirst);
#else
	     DestBuff (pBufFirst, ActifFen);
#endif
	  }
	if (pBufFirst != pBufLast)
	   if (pBufLast->BuLength == 0 &&
	       (pBufLast->BuPrevious != NULL || pBufLast->BuNext != NULL))
	      /* le buffer de fin est vide et il reste au moins un */
	      /* autre buffer. On libere le buffer de fin. */
	     {
		if (pBufLast->BuPrevious != NULL)
		   pBufLast->BuPrevious->BuNext = pBufLast->BuNext;
		else
		   ((PtrElement) element)->ElText = pBufLast->BuNext;
		if (pBufLast->BuNext != NULL)
		   pBufLast->BuNext->BuPrevious = pBufLast->BuPrevious;
#ifdef NODISPLAY
		FreeBufTexte (pBufLast);
#else
		DestBuff (pBufLast, ActifFen);
#endif
	     }
	/* met a jour le volume des elements ascendants */
	pElAsc = ((PtrElement) element)->ElParent;
	while (pElAsc != NULL)
	  {
	     pElAsc->ElVolume -= delta;
	     pElAsc = pElAsc->ElParent;
	  }
#ifndef NODISPLAY
	/* reaffiche l'element */
	RedispLeaf ((PtrElement) element, document, -delta);
	/* etablit une nouvelle selection si l'element en fait partie */
	if (changeSelection)
	  {
	     if (premCar > 0)
		if (derSel == premSel)
		   TtaSelectString (document, (Element) premSel, premCar, derCar);
		else
		   TtaSelectString (document, (Element) premSel, premCar, 0);
	     else
		TtaSelectElement (document, (Element) premSel);
	     if (derSel != premSel)
		TtaExtendSelection (document, (Element) derSel, derCar);
	  }
#endif
     }
}

/* ----------------------------------------------------------------------
   TtaSplitText

   Divides a text element into two elements.

   Parameters:
   element: the text element to be divided. A new text element containing
   the second part of the text is created as the next sibling.
   position: rank of the character after which the element must be cut.
   document: the document to which the element belongs.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaSplitText (Element element, int position, Document document)

#else  /* __STDC__ */
void                TtaSplitText (element, position, document)
Element             element;
int                 position;
Document            document;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElLeafType != LtText)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
      if (position < 1 || position >
	  ((PtrElement) element)->ElTextLength)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	SplitTextElement ((PtrElement) element, position + 1,
		    TabDocuments[document - 1], False);
#ifndef NODISPLAY
	RedispSplittedText ((PtrElement) element, position, document);
#endif
     }
}


/* ----------------------------------------------------------------------
   TtaMergeText

   Merges two text elements.

   Parameters:
   element: the first text element. Merging occurs only if
   the next sibling is a text element with the same attributes.
   document: the document to which the text element belongs.

   Return value:
   True if merging has been done.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             TtaMergeText (Element element, Document document)

#else  /* __STDC__ */
boolean             TtaMergeText (element, document)
Element             element;
Document            document;

#endif /* __STDC__ */

{
   PtrElement          FreeElement;
   PtrElement          pEl2;
   boolean             ok;

   UserErrorCode = 0;
   ok = False;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElLeafType != LtText)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
     {
	pEl2 = ((PtrElement) element)->ElNext;
	if (((PtrElement) element)->ElSructSchema->SsRule[((PtrElement) element)->ElTypeNumber - 1].SrConstruct != CsConstant)
	   if (pEl2 != NULL)
	      if (pEl2->ElTerminal && pEl2->ElLeafType == LtText)
		 if (pEl2->ElLanguage == ((PtrElement) element)->ElLanguage)
		    if (pEl2->ElSructSchema->SsRule[pEl2->ElTypeNumber - 1].SrConstruct != CsConstant)
		       if (MemesAttributs ((PtrElement) element, pEl2))
			  if (((PtrElement) element)->ElSource == NULL && pEl2->ElSource == NULL)
			     if (MemesRegleSpecif ((PtrElement) element, pEl2))
			       {
#ifndef NODISPLAY
				  /* detruit les paves du 2eme element de texte */
				  DetrPaves (pEl2, TabDocuments[document - 1], False);
#endif
				  MergeTextElements ((PtrElement) element, &FreeElement,
					 TabDocuments[document - 1], False);
#ifndef NODISPLAY
				  RedispMergedText ((PtrElement) element, document);
#endif
				  if (FreeElement != NULL)
				     DeleteElement (&FreeElement);
				  ok = True;
			       }
     }
   return ok;
}


/* ----------------------------------------------------------------------
   TtaSetGraphicsShape

   Changes the shape of a Graphics or Symbol basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Graphics or Symbol.
   shape: new shape for that element.
   document: the document containing that element.

   ---------------------------------------------------------------------- */


#ifdef __STDC__
void                TtaSetGraphicsShape (Element element, char shape, Document document)

#else  /* __STDC__ */
void                TtaSetGraphicsShape (element, shape, document)
Element             element;
char                shape;
Document            document;

#endif /* __STDC__ */

{
   int                 delta;
   boolean             polyline;
   PtrElement          pElAsc;

   UserErrorCode = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElLeafType != LtSymbol &&
	    ((PtrElement) element)->ElLeafType != LtGraphics &&
	    ((PtrElement) element)->ElLeafType != LtPlyLine)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	/* verifie le parametre document */
	if (document < 1 || document > MAX_DOCUMENTS)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else if (TabDocuments[document - 1] == NULL)
	  {
	     TtaError (ERR_invalid_document_parameter);
	  }
	else
	   /* parametre document correct */
	  {
	     delta = 0;
	     if (((PtrElement) element)->ElLeafType == LtSymbol)
	       {
		  if (((PtrElement) element)->ElGraph == '\0' &&
		      shape != '\0')
		     delta = 1;
		  else if (((PtrElement) element)->ElGraph != '\0' &&
			   shape == '\0')
		     delta = -1;
	       }
	     else
	       {
		  polyline = (shape == 'S' || shape == 'U' || shape == 'N' ||
			      shape == 'M' || shape == 'B' || shape == 'A' ||
			      shape == 'F' || shape == 'D' || shape == 'p' ||
			      shape == 's');
		  if (polyline && ((PtrElement) element)->ElLeafType == LtGraphics)
		     /* changement graphique simple --> polyline */
		    {
		       ((PtrElement) element)->ElLeafType = LtPlyLine;
		       GetBufTexte (&((PtrElement) element)->ElPolyLineBuffer);
		       ((PtrElement) element)->ElNPoints = 1;
		       ((PtrElement) element)->ElText->BuLength = 1;
		       ((PtrElement) element)->ElText->BuPoints[0].XCoord = 0;
		       ((PtrElement) element)->ElText->BuPoints[0].YCoord = 0;
		    }
		  else if (!polyline && ((PtrElement) element)->ElLeafType == LtPlyLine)
		     /* changement polyline --> graphique simple */
		    {
		       delta = -((PtrElement) element)->ElNPoints;
		       if (shape != '\0')
			  delta++;
		       ClearText (((PtrElement) element)->ElPolyLineBuffer);
		       FreeBufTexte (((PtrElement) element)->ElPolyLineBuffer);
		       ((PtrElement) element)->ElLeafType = LtGraphics;
		    }
		  else if (((PtrElement) element)->ElLeafType == LtGraphics)
		     if (((PtrElement) element)->ElGraph == '\0' &&
			 shape != '\0')
			delta = 1;
		     else if (((PtrElement) element)->ElGraph != '\0' &&
			      shape == '\0')
			delta = -1;
	       }
	     if (((PtrElement) element)->ElLeafType == LtPlyLine)
		((PtrElement) element)->ElPolyLineType = shape;
	     else
		((PtrElement) element)->ElGraph = shape;
	     /* met a jour le volume des elements ascendants */
	     if (delta > 0)
	       {
		  pElAsc = (PtrElement) element;
		  while (pElAsc != NULL)
		    {
		       pElAsc->ElVolume += delta;
		       pElAsc = pElAsc->ElParent;
		    }
	       }
#ifndef NODISPLAY
	     RedispLeaf ((PtrElement) element, document, delta);
#endif
	  }
     }
}


#ifdef __STDC__
static boolean      PolylineOK (Element element, Document document)

#else  /* __STDC__ */
static boolean      PolylineOK (element, document)
Element             element;
Document            document;

#endif /* __STDC__ */

{
   boolean             ok;

   ok = False;
   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
      TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtPlyLine)
      TtaError (ERR_invalid_element_type);
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (TabDocuments[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parametre document correct */
      ok = True;
   return ok;
}


/* ----------------------------------------------------------------------
   TtaAddPointInPolyline

   Adds a new point in a Polyline basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Polyline.
   document: the document containing the polyline element.
   rank: rank of the new point to be inserted. If rank is greater
   than the actual number of points, the new point is appended.
   rank must be strictly positive.
   x, y: coordinate of the new point, in millipoints, relatively to
   the upper left corner of the box. x and y
   must be positive or null.
   unit: UnPixel or UnPoint.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaAddPointInPolyline (Element element, int rank, TypeUnit unit, int x, int y, Document document)
#else  /* __STDC__ */
void                TtaAddPointInPolyline (element, rank, unit, x, y, document)
Element             element;
int                 rank;
TypeUnit            unit;
int                 x;
int                 y;
Document            document;

#endif /* __STDC__ */
{
   PtrTextBuffer      firstBuffer;
   PtrElement          pEl;

   if (PolylineOK (element, document))
      if (rank <= 0 || x < 0 || y < 0)
	 TtaError (ERR_invalid_parameter);
      else if (unit != UnPoint && unit != UnPixel)
	 TtaError (ERR_invalid_parameter);
      else
	{
	   firstBuffer = ((PtrElement) element)->ElPolyLineBuffer;
	   /* Convert values to millipoints */
	   x = x * 1000;
	   y = y * 1000;
	   if (unit == UnPixel)
	     {
		x = PixelEnPt (x, 0);
		y = PixelEnPt (y, 0);
	     }

	   /* ajoute le point a la polyline */
	   AddPointInPolyline (firstBuffer, rank, x, y);
	   ((PtrElement) element)->ElNPoints++;
	   /* met a jour le volume des elements ascendants */
	   pEl = ((PtrElement) element)->ElParent;
	   while (pEl != NULL)
	     {
		pEl->ElVolume++;
		pEl = pEl->ElParent;
	     }
#ifndef NODISPLAY
	   RedispLeaf ((PtrElement) element, document, 1);
#endif
	}
}

/* ----------------------------------------------------------------------
   TtaDeletePointInPolyline

   Deletes a point in a Polyline basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Polyline.
   rank: rank of the point to be deleted. If rank is greater
   than the actual number of points, the last point is deleted.
   rank must be strictly positive.
   document: the document containing the polyline element.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaDeletePointInPolyline (Element element, int rank, Document document)
#else  /* __STDC__ */
void                TtaDeletePointInPolyline (element, rank, document)
Element             element;
int                 rank;
Document            document;

#endif /* __STDC__ */
{
   PtrElement          pEl;

   if (PolylineOK (element, document))
      if (rank <= 0)
	 TtaError (ERR_invalid_parameter);
      else
	{
	   /* retire le point de la polyline */
	   DeletePointInPolyline (&(((PtrElement) element)->ElPolyLineBuffer), rank);
	   /* il y a un point de moins dans l'element */
	   ((PtrElement) element)->ElNPoints--;
	   /* met a jour le volume des elements ascendants */
	   pEl = ((PtrElement) element)->ElParent;
	   while (pEl != NULL)
	     {
		pEl->ElVolume--;
		pEl = pEl->ElParent;
	     }
#ifndef NODISPLAY
	   RedispLeaf ((PtrElement) element, document, -1);
#endif
	}
}


/* ----------------------------------------------------------------------
   TtaModifyPointInPolyline

   Changes the coordinates of a point in a Polyline basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Polyline.
   rank: rank of the point to be modified. If rank is greater
   than the actual number of points, the last point is changed.
   rank must be strictly positive.
   x, y: new coordinates of the point, in millipoints, relatively to
   the upper left corner of the enclosing rectangle. x and y
   must be positive or null.
   document: the document containing the polyline element.
   unit: UnPixel or UnPoint.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaModifyPointInPolyline (Element element, int rank, TypeUnit unit, int x, int y, Document document)

#else  /* __STDC__ */
void                TtaModifyPointInPolyline (element, rank, unit, x, y, document)
Element             element;
int                 rank;
TypeUnit            unit;
int                 x;
int                 y;
Document            document;

#endif /* __STDC__ */

{
   if (PolylineOK (element, document))
      if (rank <= 0 || x < 0 || y < 0)
	 TtaError (ERR_invalid_parameter);
      else if (unit != UnPoint && unit != UnPixel)
	 TtaError (ERR_invalid_parameter);
      else
	{
	   /* Convert values to millipoints */
	   x = x * 1000;
	   y = y * 1000;
	   if (unit == UnPixel)
	     {
		x = PixelEnPt (x, 0);
		y = PixelEnPt (y, 0);
	     }
	   ModifyPointInPolyline (((PtrElement) element)->ElPolyLineBuffer, rank, x, y);
#ifndef NODISPLAY
	   RedispLeaf ((PtrElement) element, document, 0);
#endif
	}
}


/* ----------------------------------------------------------------------
   TtaChangeLimitOfPolyline

   Changes the coordinates of the lower left corner of the box containing
   a Polyline basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Polyline.
   unit: UnPixel or UnPoint.
   x, y: new coordinates of the lower left corner, in millipoints,
   relatively to the upper left corner of the box. x and y
   must be positive or null and the box must contain all
   points of the polyline.
   document: the document containing the polyline element.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaChangeLimitOfPolyline (Element element, TypeUnit unit, int x, int y, Document document)
#else  /* __STDC__ */
void                TtaChangeLimitOfPolyline (element, unit, x, y, document)
Element             element;
TypeUnit            unit;
int                 x;
int                 y;
Document            document;

#endif /* __STDC__ */
{
   PtrTextBuffer      firstBuffer;
   PtrTextBuffer      pBuff;
   int                 rank;

   if (PolylineOK (element, document))
      if (x < 0 || y < 0)
	 TtaError (ERR_invalid_parameter);
      else if (unit != UnPoint && unit != UnPixel)
	 TtaError (ERR_invalid_parameter);
      else
	{
	   /* Convert values to millipoints */
	   x = x * 1000;
	   y = y * 1000;
	   if (unit == UnPixel)
	     {
		x = PixelEnPt (x, 0);
		y = PixelEnPt (y, 0);
	     }
	   firstBuffer = ((PtrElement) element)->ElPolyLineBuffer;
	   /* verifie que les coordonnees du nouveau point limite sont */
	   /* superieures a celles de tous les points de la polyline */
	   rank = 1;
	   pBuff = firstBuffer;
	   while (pBuff != NULL)
	     {
		while (rank < pBuff->BuLength)
		  {
		     if (x < pBuff->BuPoints[rank].XCoord)
			x = pBuff->BuPoints[rank].XCoord;
		     if (y < pBuff->BuPoints[rank].YCoord)
			y = pBuff->BuPoints[rank].YCoord;
		     rank++;
		  }
		rank = 0;
		pBuff = pBuff->BuNext;
	     }
	   /* met les coordonnees du nouveau point limite */
	   firstBuffer->BuPoints[0].XCoord = x;
	   firstBuffer->BuPoints[0].YCoord = y;
#ifndef NODISPLAY
	   RedispLeaf ((PtrElement) element, document, 0);
#endif
	}
}


/* ----------------------------------------------------------------------
   TtaCopyPage

   Copies the page element source into the page element destination.
   Both page elements must be in an abstract tree.

   Parameters:
   destination: identifier of the page element to be modified.
   source : identifier of the source page element.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaCopyPage (Element destination, Element source)

#else  /* __STDC__ */
void                TtaCopyPage (destination, source)
Element             destination;
Element             source;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (destination == NULL || source == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) destination)->ElTerminal ||
	    ((PtrElement) destination)->ElLeafType != LtPageColBreak)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) source)->ElTerminal ||
	    ((PtrElement) source)->ElLeafType != LtPageColBreak)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	((PtrElement) destination)->ElPageType =
	   ((PtrElement) source)->ElPageType;
	((PtrElement) destination)->ElPageNumber =
	   ((PtrElement) source)->ElPageNumber;
	((PtrElement) destination)->ElViewPSchema =
	   ((PtrElement) source)->ElViewPSchema;
     }
}


/* ----------------------------------------------------------------------
   TtaGetTextLength

   Returns the length of a Text basic element.

   Parameter:
   element: the element of interest. This element must be a basic
   element of type Text.

   Return value:
   textLength (number of characters) of the character string
   contained in the element.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetTextLength (Element element)

#else  /* __STDC__ */
int                 TtaGetTextLength (element)
Element             element;

#endif /* __STDC__ */

{
   int                 textLength;

   UserErrorCode = 0;
   textLength = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElLeafType != LtText &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
      textLength = ((PtrElement) element)->ElTextLength;
   return textLength;
}

/* ----------------------------------------------------------------------
   TtaGiveTextContent

   Returns the content of a Text basic element.

   Parameters:
   element: the element of interest. This element must be a basic
   element of type Text.
   buffer: the buffer that will contain the text.
   length: maximum length of that buffer.

   Return parameters:
   buffer: (the buffer contains the text).
   length: actual length of the text in the buffer.
   language: language of the text.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaGiveTextContent (Element element, char *buffer, int *length, Language * language)
#else  /* __STDC__ */
void                TtaGiveTextContent (element, buffer, length, language)
Element             element;
char               *buffer;
int                *length;
Language           *language;

#endif /* __STDC__ */
{
   PtrTextBuffer      pBuf;
   char               *ptr;
   int                 len, l;

   UserErrorCode = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElLeafType != LtText &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	if (*length <= ((PtrElement) element)->ElTextLength)
	  {
	     TtaError (ERR_buffer_too_small);
	  }
	len = 0;
	pBuf = ((PtrElement) element)->ElText;
	ptr = buffer;
	while (pBuf != NULL && len < (*length) - 1)
	  {
	     if ((*length) < len + pBuf->BuLength + 1)
		l = (*length) - len;
	     else
		l = pBuf->BuLength + 1;
	     strncpy (ptr, pBuf->BuContent, l);
	     ptr = ptr + (l - 1);
	     len = len + (l - 1);
	     pBuf = pBuf->BuNext;
	  }
	*length = len;
	*ptr = '\0';
	*language = ((PtrElement) element)->ElLanguage;
     }
}

/* ----------------------------------------------------------------------
   TtaGiveSubString

   Returns a substring from a Text basic element.

   Parameters:
   element: the element of interest. This element must be a basic
   element of type Text.
   buffer: the buffer that will contain the substring. This buffer
   must be at least of size length.
   position: the rank of the first character of the substring.
   rank must be strictly positive.
   length: the length of the substring. Must be strictly positive.

   Return parameter:
   buffer: (the buffer contains the substring).

   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGiveSubString (Element element, char *buffer, int position, int length)

#else  /* __STDC__ */
void                TtaGiveSubString (element, buffer, position, length)
Element             element;
char               *buffer;
int                 position;
int                 length;

#endif /* __STDC__ */

{
   PtrTextBuffer      pBuf;
   char               *ptr;
   int                 len, l;

   UserErrorCode = 0;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (position < 1 || length < 1)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElLeafType != LtText &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
     {
	len = 0;
	pBuf = ((PtrElement) element)->ElText;
	if (pBuf != NULL)
	   len = pBuf->BuLength;
	ptr = buffer;
	position--;
	/* on saute les buffers du debut */
	while (pBuf != NULL && len <= position)
	  {
	     position -= len;
	     pBuf = pBuf->BuNext;
	     len = pBuf->BuLength;
	  }
	/* on copie dans le buffer */
	while (pBuf != NULL && length > 0)
	  {
	     if (length + position < pBuf->BuLength)
		l = length;
	     else
		l = pBuf->BuLength - position;
	     strncpy (ptr, pBuf->BuContent + position, l);
	     ptr = ptr + l;
	     length = length - l;
	     pBuf = pBuf->BuNext;
	     position = 0;
	  }
	*ptr = '\0';
     }
}

/* ----------------------------------------------------------------------
   TtaGetGraphicsShape

   Returns the content of a Graphics or Symbol basic element.

   Parameter:
   element: the element of interest. This element must be a basic
   element of type Graphics or Symbol.

   Return value:
   a single character representing the shape of the graphics element or
   symbol contained in the element.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
char                TtaGetGraphicsShape (Element element)

#else  /* __STDC__ */
char                TtaGetGraphicsShape (element)
Element             element;

#endif /* __STDC__ */
{
   char                content;

   UserErrorCode = 0;
   content = '\0';
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) element)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElLeafType != LtSymbol &&
	    ((PtrElement) element)->ElLeafType != LtGraphics &&
	    ((PtrElement) element)->ElLeafType != LtPlyLine)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) element)->ElLeafType == LtPlyLine)
      content = ((PtrElement) element)->ElPolyLineType;
   else
      content = ((PtrElement) element)->ElGraph;
   return content;
}

/* ----------------------------------------------------------------------
   TtaGetPolylineLength

   Returns the number of points in a Polyline basic element.

   Parameter:
   element: the Polyline element. This element must
   be a basic element of type Polyline.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetPolylineLength (Element element)

#else  /* __STDC__ */
int                 TtaGetPolylineLength (element)
Element             element;

#endif /* __STDC__ */

{
   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
      TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtPlyLine)
      TtaError (ERR_invalid_element_type);
   else
      /* on ignore le point limite, compte' dans ElNPoints */
      return ((PtrElement) element)->ElNPoints - 1;
   return (0);
}

/* ----------------------------------------------------------------------
   TtaGivePolylinePoint

   Returns the coordinates of a point in a Polyline basic element.

   Parameters:
   element: the Polyline element. This element must
   be a basic element of type Polyline.
   rank: rank of the point in the PloyLine. If rank is greater
   than the actual number of points, an error is raised.
   rank must be strictly positive.
   unit: UnPixel or UnPoint.

   Return values:
   x, y: coordinates of the point, in unit, relatively to
   the upper left corner of the enclosing rectangle.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaGivePolylinePoint (Element element, int rank, TypeUnit unit, int *x, int *y)
#else  /* __STDC__ */
void                TtaGivePolylinePoint (element, rank, unit, x, y)
Element             element;
int                 rank;
TypeUnit            unit;
int                *x;
int                *y;

#endif /* __STDC__ */
{
   PtrTextBuffer      pBuff;

   UserErrorCode = 0;
   *x = 0;
   *y = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
      TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtPlyLine)
      TtaError (ERR_invalid_element_type);
   else if (unit != UnPoint && unit != UnPixel)
      TtaError (ERR_invalid_parameter);
   else
     {
	/* cherche le buffer contenant le point de rang rank */
	pBuff = ((PtrElement) element)->ElPolyLineBuffer;
	while (rank > pBuff->BuLength && pBuff->BuNext != NULL)
	  {
	     rank -= pBuff->BuLength;
	     pBuff = pBuff->BuNext;
	  }
	if (rank > pBuff->BuLength)
	   TtaError (ERR_invalid_parameter);
	else
	  {
	     *x = pBuff->BuPoints[rank - 1].XCoord;
	     *y = pBuff->BuPoints[rank - 1].YCoord;
	     /* Convert values to millipoints */
	     if (unit == UnPixel)
	       {
		  *x = PixelEnPt (*x, 0);
		  *y = PixelEnPt (*y, 0);
	       }
	     *x = *x / 1000;
	     *y = *y / 1000;
	  }
     }
}


/* ----------------------------------------------------------------------
   TtaGetPageNumber

   Returns the page number of a Page basic element.

   Parameter:
   pageElement: the page element.

   Return value:
   page number of that page element.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetPageNumber (Element pageElement)

#else  /* __STDC__ */
int                 TtaGetPageNumber (pageElement)
Element             pageElement;

#endif /* __STDC__ */

{
   int                 pageNumber;

   UserErrorCode = 0;
   pageNumber = 0;
   if (pageElement == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) pageElement)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) pageElement)->ElLeafType != LtPageColBreak)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
      pageNumber = ((PtrElement) pageElement)->ElPageNumber;
   return pageNumber;
}

/* ----------------------------------------------------------------------
   TtaGetPageView

   Returns the view corresponding to a Page basic element.

   Parameter:
   pageElement: the page element.

   Return value:
   view of that page.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaGetPageView (Element pageElement)

#else  /* __STDC__ */
int                 TtaGetPageView (pageElement)
Element             pageElement;

#endif /* __STDC__ */

{
   int                 pageView;

   UserErrorCode = 0;
   pageView = 0;
   if (pageElement == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (!((PtrElement) pageElement)->ElTerminal)
     {
	TtaError (ERR_invalid_element_type);
     }
   else if (((PtrElement) pageElement)->ElLeafType != LtPageColBreak)
     {
	TtaError (ERR_invalid_element_type);
     }
   else
      pageView = ((PtrElement) pageElement)->ElViewPSchema;
   return pageView;
}

/* fin du module */
