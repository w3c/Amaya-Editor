/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Content API routines
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA) - Polylines
 *
 */

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
#include "fileaccess.h"
#include "content.h"
#include "frame.h"
#include "typecorr.h"

#undef THOT_EXPORT
#define THOT_EXPORT
#include "edit_tv.h"
#ifndef NODISPLAY
#include "frame_tv.h"
#endif

#include "abspictures_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "font_f.h"
#include "geom_f.h"
#include "memory_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "units_f.h"
#include "viewapi_f.h"
#include "writepivot_f.h"


/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetTextContent (Element element, STRING content, Language language, Document document)

#else  /* __STDC__ */
void                TtaSetTextContent (element, content, language, document)
Element             element;
STRING              content;
Language            language;
Document            document;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBuf, pPreviousBuff, pNextBuff;
   STRING              ptr;
   int                 length, l, delta = 0;
   PtrElement          pEl;

#ifndef NODISPLAY
   PtrDocument         selDoc;
   PtrElement          firstSelection, lastSelection;
   int                 firstChar, lastChar;
   ThotBool            selOk, changeSelection;

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
     {
	/* verifies the parameter document */
	if (document < 1 || document > MAX_DOCUMENTS)
	     TtaError (ERR_invalid_document_parameter);
	else if (LoadedDocument[document - 1] == NULL)
	     TtaError (ERR_invalid_document_parameter);
	else
	   /* parameter document is correct */
	  {
#ifndef NODISPLAY
	     /* modifies the selection if the element is within it */
	     selOk = GetCurrentSelection (&selDoc, &firstSelection, &lastSelection, &firstChar, &lastChar);
	     changeSelection = FALSE;
	     if (selOk && content != NULL)
		if (selDoc == LoadedDocument[document - 1])
		   if ((PtrElement) element == firstSelection ||
		       (PtrElement) element == lastSelection)
		      /* The selection starts and/or stops in the element */
		      /* First, we abort the selection */
		     {
			TtaSelectElement (document, NULL);
			changeSelection = TRUE;
			if (lastChar > 1)
			   lastChar -= 1;
			pEl = (PtrElement) element;
			if (pEl == firstSelection)
			   /* The element is at the begenning of the selection */
			  {
			     firstChar = 0;
			     if (pEl == lastSelection)
				/* The selection consists only in the element it self, 
				 * one select the whole element */
				lastChar = 0;
			  }
			else if (pEl == lastSelection)
			   /* The element is at the end of the selection 
			    * one select until the end of the element */
			   lastChar = 0;
		     }
#endif
	     if (((PtrElement) element)->ElLeafType == LtText)
		((PtrElement) element)->ElLanguage = language;
	     if (content != NULL)
	       {
	       ptr = content;
	       length = ustrlen (content);
	       delta = length - ((PtrElement) element)->ElTextLength;
	       ((PtrElement) element)->ElTextLength = length;
	       ((PtrElement) element)->ElVolume = length;
	       pPreviousBuff = NULL;
	       pBuf = ((PtrElement) element)->ElText;

	       do
	         {
		  if (pBuf == NULL)
		     GetTextBuffer (&pBuf);
		  if (length >= THOT_MAX_CHAR)
		     l = THOT_MAX_CHAR - 1;
		  else
		     l = length;
		  if (l > 0)
		    {
		       ustrncpy (pBuf->BuContent, ptr, l);
		       ptr += l;
		       length -= l;
		    }
		  pBuf->BuLength = l;
		  pBuf->BuContent[l] = EOS;
		  pBuf->BuPrevious = pPreviousBuff;
		  if (pPreviousBuff == NULL)
		     ((PtrElement) element)->ElText = pBuf;
		  else
		     pPreviousBuff->BuNext = pBuf;
		  pPreviousBuff = pBuf;
		  pBuf = pBuf->BuNext;
		  pPreviousBuff->BuNext = NULL;
	         }
	       while (length > 0);

	       while (pBuf != NULL)
		  /* Release the remaining buffers */
	         {
		  pNextBuff = pBuf->BuNext;
#ifdef NODISPLAY
		  FreeTextBuffer (pBuf);
#else
		  DeleteBuffer (pBuf, ActiveFrame);
#endif
		  pBuf = pNextBuff;
	         }
	       /* Updates the volumes of the ancestors */
	       pEl = ((PtrElement) element)->ElParent;
	       while (pEl != NULL)
	         {
		  pEl->ElVolume += delta;
		  pEl = pEl->ElParent;
	         }
	       if (((PtrElement) element)->ElLeafType == LtPicture)
	         {
		  /* Releases the  pixmap */
		  if (((PtrElement) element)->ElPictInfo != NULL)
		     FreePictInfo ((PictInfo *) (((PtrElement) element)->ElPictInfo));
	         }
	      }
#ifndef NODISPLAY
	     RedisplayLeaf ((PtrElement) element, document, delta);
	     /* Sets up a new selection if the element is within it */
	     if (changeSelection)
	       {
		  if (firstChar > 0)
		     TtaSelectString (document, (Element) firstSelection, firstChar, 0);
		  else
		     TtaSelectElement (document, (Element) firstSelection);
		  if (lastSelection != firstSelection)
		     TtaExtendSelection (document, (Element) lastSelection, lastChar);
	       }
#endif
	  }
     }
}

/*----------------------------------------------------------------------
   InsertText   inserts the string "content" in the element 
   pEl (which must be of type text), at the position
   "position". If "document" is null, we have not to consider
   the selction nor the displaying
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InsertText (PtrElement pEl, int position, STRING content, Document document)
#else  /* __STDC__ */
void                InsertText (pEl, position, content, document)
PtrElement          pEl;
int                 position;
STRING              content;
Document            document;
#endif /* __STDC__ */

{
   PtrTextBuffer       pBuf, pPreviousBuff, newBuf;
   STRING              ptr;
   int                 stringLength, l, delta, lengthBefore;
   PtrElement          pElAsc;

#ifndef NODISPLAY
   PtrDocument         selDoc;
   PtrElement          firstSelection, lastSelection;
   int                 firstChar, lastChar;
   ThotBool            selOk, changeSelection = FALSE;
#endif

   stringLength = ustrlen (content);
   if (stringLength > 0)
     {
	/* corrects the parameter position */
	if (position > pEl->ElTextLength)
	   position = pEl->ElTextLength;
#ifndef NODISPLAY
	if (document > 0)
	  {
	     /* modifies the selection if the element belongs to it */
	     selOk = GetCurrentSelection (&selDoc, &firstSelection, &lastSelection, &firstChar, &lastChar);
	     changeSelection = FALSE;
	     if (selOk)
		if (selDoc == LoadedDocument[document - 1])
		   if (pEl == firstSelection || pEl == lastSelection)
		      /* The selection starts and/or stops in the element */
		      /* First, we abort the selection */
		     {
			TtaSelectElement (document, NULL);
			changeSelection = TRUE;
			if (lastChar > 1)
			   lastChar -= 1;
			if (pEl == firstSelection)
			   /* The element is at the begenning of the selection */
			  {
			     if (firstChar > position)
				firstChar += stringLength;
			  }
			if (pEl == lastSelection)
			   /* The element is at the end of the selection */
			  {
			     if (position < lastChar)
				lastChar += stringLength;
			  }
		     }
	  }
#endif
	delta = stringLength;
	pEl->ElTextLength += stringLength;
	pEl->ElVolume += stringLength;
	/* looks for the buffer pBuf where the insertion has to be done */
	pBuf = pEl->ElText;
	lengthBefore = 0;
	while (pBuf->BuNext && lengthBefore + pBuf->BuLength < position)
	  {
	     lengthBefore += pBuf->BuLength;
	     pBuf = pBuf->BuNext;
	  }
	/* Length of the remaining of the buffer */
	lengthBefore = position - lengthBefore;
	if (lengthBefore == 0)
	   /* one insert before the first character of the element */
	  {
	    if (pBuf->BuLength != 0)
	      {
		/* one add a buffer before the existing buffers */
		GetTextBuffer (&newBuf);
		if (position)
		  {
		    /* chain a new buffer between two buffers */
		    newBuf->BuNext = pBuf;
		    newBuf->BuPrevious = pBuf->BuPrevious;
		    pBuf->BuPrevious->BuNext = newBuf;
		    pBuf->BuPrevious = newBuf;
		  }
		else
		  {
		    /* chain a new buffer as the first buffer */
		    newBuf->BuNext = pEl->ElText;
		    newBuf->BuPrevious = NULL;
		    if (newBuf->BuNext)
		      newBuf->BuNext->BuPrevious = newBuf;
		    pEl->ElText = newBuf;
		  }
		pBuf = newBuf;
	      }
	  }
	else if (lengthBefore < pBuf->BuLength)
	  {
	   /* Creates a buffer for the second part of the text */
	     newBuf = NewTextBuffer (pBuf);
	     ustrcpy (newBuf->BuContent, pBuf->BuContent + lengthBefore);
	     newBuf->BuLength = pBuf->BuLength - lengthBefore;
	  }
	pBuf->BuContent[lengthBefore] = EOS;
	pBuf->BuLength = lengthBefore;
	/* If there is enough space in the buffer, one add a string at its end */
	ptr = content;
	while (stringLength > 0)
	  {
	    if (stringLength < THOT_MAX_CHAR - lengthBefore)
	      l = stringLength;
	    else
	      l = THOT_MAX_CHAR - lengthBefore - 1;
	    ustrncpy (pBuf->BuContent + lengthBefore, ptr, l);
	    pBuf->BuLength += l;
	    pBuf->BuContent[pBuf->BuLength] = EOS;
	    lengthBefore = 0;
	    stringLength -= l;
	    ptr = &ptr[l];
	    if (stringLength > 0)
	      {
		pPreviousBuff = pBuf;
		pBuf = NewTextBuffer (pPreviousBuff);
	      }
	  }
	/* Updates the volumes of ancestors */
	pElAsc = pEl->ElParent;
	while (pElAsc != NULL)
	  {
	     pElAsc->ElVolume += delta;
	     pElAsc = pElAsc->ElParent;
	  }
#ifndef NODISPLAY
	if (document > 0)
	  {
	     RedisplayLeaf (pEl, document, delta);
	     /* Sets up a new selection if the element is within it */
	     if (changeSelection)
	       {
		  if (firstChar > 0)
		     if (lastSelection == firstSelection)
			TtaSelectString (document, (Element) firstSelection, firstChar, lastChar);
		     else
			TtaSelectString (document, (Element) firstSelection, firstChar, 0);
		  else
		     TtaSelectElement (document, (Element) firstSelection);
		  if (lastSelection != firstSelection)
		     TtaExtendSelection (document, (Element) lastSelection, lastChar);
	       }
	  }
#endif
     }
}


/*----------------------------------------------------------------------
   TtaAppendTextContent

   Appends a character string at the end of a Text basic element.

   Parameters:
   element: the Text element to be modified.
   content: the character string to be appended.
   document: the document containing that element.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaAppendTextContent (Element element, STRING content, Document document)

#else  /* __STDC__ */
void                TtaAppendTextContent (element, content, document)
Element             element;
STRING              content;
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
      /* verifies the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
      InsertText ((PtrElement) element,
		  ((PtrElement) element)->ElTextLength,
		  content, document);
}


/*----------------------------------------------------------------------
   TtaInsertTextContent

   Inserts a character string in a text basic element.

   Parameters:
   element: the Text element to be modified.
   position: rank of the character after which the new string must
   be inserted. 0 for inserting before the first character.
   content: the character string to be inserted.
   document: the document containing the text element.

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaInsertTextContent (Element element, int position, STRING content, Document document)

#else  /* __STDC__ */
void                TtaInsertTextContent (element, position, content, document)
Element             element;
int                 position;
STRING              content;
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
      /* verifies the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
      InsertText ((PtrElement) element, position, content, document);
}

/*----------------------------------------------------------------------
   TtaDeleteTextContent

   Deletes a character string in a text basic element.

   Parameters:
   element: the Text element to be modified.
   position: rank of the first character to be deleted.
   length: length of the character string to be deleted.
   document: the document containing the text element.

  ----------------------------------------------------------------------*/

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
   PtrTextBuffer       pBufFirst, pBufLast, pBufNext;
   STRING              dest, source;
   int                 delta, lengthBefore, firstDeleted, lastDeleted,
                       l;
   PtrElement          pElAsc;

#ifndef NODISPLAY
   PtrDocument         selDoc;
   PtrElement          firstSelection, lastSelection;
   int                 firstChar, lastChar;
   ThotBool            selOk, changeSelection;

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
      /* verifies the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
      if (length < 0 || position <= 0 ||
	  position > ((PtrElement) element)->ElTextLength)
      TtaError (ERR_invalid_parameter);
   else if (length > 0)
     {
	/* verifies that the parameter length is not too big */
	if (position + length > ((PtrElement) element)->ElTextLength + 1)
	   length = ((PtrElement) element)->ElTextLength - position + 1;
#ifndef NODISPLAY
	/* modifies the selection if the element belongs to it */
	selOk = GetCurrentSelection (&selDoc, &firstSelection, &lastSelection, &firstChar, &lastChar);
	changeSelection = FALSE;
	if (selOk)
	   if (selDoc == LoadedDocument[document - 1])
	      if ((PtrElement) element == firstSelection || (PtrElement) element == lastSelection)
		 /* The selection starts and/or stops in the element */
		 /* First, we abort the selection */
		{
		   TtaSelectElement (document, NULL);
		   changeSelection = TRUE;
		   if (lastChar > 1)
		      lastChar -= 1;
		   if ((PtrElement) element == firstSelection)
		      /* The element is at the begenning of the selection */
		     {
			if (firstChar > position)
			  {
			     firstChar -= length;
			     if (firstChar < position)
				firstChar = position;
			  }
		     }
		   if ((PtrElement) element == lastSelection)
		      /* The element is at the end of the selection */
		     {
			if (position < lastChar)
			  {
			     lastChar -= length;
			     if (lastChar < position)
				lastChar = position;
			  }
		     }
		}
#endif
	delta = length;
	((PtrElement) element)->ElTextLength -= delta;
	((PtrElement) element)->ElVolume -= delta;
	/* Looks for the buffer pBufFirst where starts the string to suppress */
	pBufFirst = ((PtrElement) element)->ElText;
	lengthBefore = 0;
	while (pBufFirst->BuNext != NULL &&
	       lengthBefore + pBufFirst->BuLength < position)
	  {
	     lengthBefore += pBufFirst->BuLength;
	     pBufFirst = pBufFirst->BuNext;
	  }
	/* Length of the buffer containing the begenning of the string to suppress */
	firstDeleted = position - lengthBefore;
	/* Looks for the buffer pBufLast containing the end of the string to suppress 
	   and releases the intermediate buffers. The buffers containing the begenning 
	   of the string to suppress and its end are not released */
	pBufLast = pBufFirst;
	lastDeleted = firstDeleted + length - 1;
	while (pBufLast->BuNext != NULL &&
	       lastDeleted > pBufLast->BuLength)
	  {
	     lastDeleted -= pBufLast->BuLength;
	     if (pBufLast != pBufFirst)
		/* This is not the buffer containing the begenning of the string. It is released */
	       {
		  pBufNext = pBufLast->BuNext;
		  pBufLast->BuPrevious->BuNext = pBufLast->BuNext;
		  if (pBufLast->BuNext != NULL)
		     pBufLast->BuNext->BuPrevious = pBufLast->BuPrevious;
#ifdef NODISPLAY
		  FreeTextBuffer (pBufLast);
#else
		  DeleteBuffer (pBufLast, ActiveFrame);
#endif
		  pBufLast = pBufNext;
	       }
	     else
		pBufLast = pBufLast->BuNext;
	  }
	/* The text following the string to suppress is moved on the right */
	if (pBufFirst == pBufLast)
	   /* The whole string to suppress is in the same buffer */
	   /* The text following the string to be suppressed is moved at 
	      the position of the begenning of the string to suppress */
	  {
	     dest = pBufFirst->BuContent + firstDeleted - 1;
	     l = length;
	  }
	else
	   /* The begenning and the and of the string to be suppressed are
	      in different buffers. The text of the first buffer is troncated
	      and the text remaining in the other buffer is moved at the
	      begenning of the buffer */
	  {
	     pBufFirst->BuContent[firstDeleted - 1] = EOS;
	     pBufFirst->BuLength = firstDeleted - 1;
	     dest = pBufLast->BuContent;
	     l = lastDeleted;
	  }
	/* The text following the part to be suppresses is moved */
	source = pBufLast->BuContent + lastDeleted - 1;
	do
	  {
	     source++;
	     *dest = *source;
	     dest++;
	  }
	while (*source != EOS);
	pBufLast->BuLength -= l;
	/* If the buffers of the begening and the end of the suppresses string
	   are empty, they are released. A buffer is kept for the element */
	if (pBufFirst->BuLength == 0 &&
	    (pBufFirst->BuPrevious != NULL || pBufFirst->BuNext != NULL))
	   /* If the buffer of the begenning is empty, it is released */
	  {
	     if (pBufFirst->BuPrevious != NULL)
		pBufFirst->BuPrevious->BuNext = pBufFirst->BuNext;
	     else
		((PtrElement) element)->ElText = pBufFirst->BuNext;
	     if (pBufFirst->BuNext != NULL)
		pBufFirst->BuNext->BuPrevious = pBufFirst->BuPrevious;
#ifdef NODISPLAY
	     FreeTextBuffer (pBufFirst);
#else
	     DeleteBuffer (pBufFirst, ActiveFrame);
#endif
	  }
	if (pBufFirst != pBufLast)
	   if (pBufLast->BuLength == 0 &&
	       (pBufLast->BuPrevious != NULL || pBufLast->BuNext != NULL))
	      /* The buffer of the end is empty. It is released */
	     {
		if (pBufLast->BuPrevious != NULL)
		   pBufLast->BuPrevious->BuNext = pBufLast->BuNext;
		else
		   ((PtrElement) element)->ElText = pBufLast->BuNext;
		if (pBufLast->BuNext != NULL)
		   pBufLast->BuNext->BuPrevious = pBufLast->BuPrevious;
#ifdef NODISPLAY
		FreeTextBuffer (pBufLast);
#else
		DeleteBuffer (pBufLast, ActiveFrame);
#endif
	     }
	/* Updates the volumes of the ancestors */
	pElAsc = ((PtrElement) element)->ElParent;
	while (pElAsc != NULL)
	  {
	     pElAsc->ElVolume -= delta;
	     pElAsc = pElAsc->ElParent;
	  }
#ifndef NODISPLAY
	/* Redisplays the element */
	RedisplayLeaf ((PtrElement) element, document, -delta);
	/* Sets up a new selection if the element belongs to it */
	if (changeSelection)
	  {
	     if (firstChar > 0)
		if (lastSelection == firstSelection)
		   TtaSelectString (document, (Element) firstSelection, firstChar, lastChar);
		else
		   TtaSelectString (document, (Element) firstSelection, firstChar, 0);
	     else
		TtaSelectElement (document, (Element) firstSelection);
	     if (lastSelection != firstSelection)
		TtaExtendSelection (document, (Element) lastSelection, lastChar);
	  }
#endif
     }
}

/*----------------------------------------------------------------------
   TtaSplitText

   Divides a text element into two elements.

   Parameters:
   element: the text element to be divided. A new text element containing
   the second part of the text is created as the next sibling.
   position: rank of the character after which the element must be cut.
   document: the document to which the element belongs.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSplitText (Element element, int position, Document document)
#else  /* __STDC__ */
void                TtaSplitText (element, position, document)
Element             element;
int                 position;
Document            document;
#endif /* __STDC__ */
{
   PtrElement	secondPart, pNextEl;

   UserErrorCode = 0;
   if (element == NULL)
	TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
	TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtText)
	TtaError (ERR_invalid_element_type);
   else if (document < 1 || document > MAX_DOCUMENTS)
	TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
	TtaError (ERR_invalid_document_parameter);
   else if (position < 1 || position >
	  ((PtrElement) element)->ElTextLength)
	TtaError (ERR_invalid_parameter);
   else
     {
	pNextEl = ((PtrElement) element)->ElNext;
	SplitTextElement ((PtrElement) element, position + 1,
			  LoadedDocument[document - 1], FALSE, &secondPart);
#ifndef NODISPLAY
	RedisplaySplittedText ((PtrElement) element, position, secondPart,
				pNextEl, document);
#endif
     }
}


/*----------------------------------------------------------------------
   TtaMergeText

   Merges two text elements.

   Parameters:
   element: the first text element. Merging occurs only if
   the next sibling is a text element with the same attributes.
   document: the document to which the text element belongs.

   Return value:
   TRUE if merging has been done.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaMergeText (Element element, Document document)

#else  /* __STDC__ */
ThotBool            TtaMergeText (element, document)
Element             element;
Document            document;

#endif /* __STDC__ */

{
   ThotBool            ok = FALSE;
#ifndef NODISPLAY
   PtrElement          FreeElement;
   PtrElement          pEl2;

   UserErrorCode = 0;
   if (element == NULL)
     TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
     TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtText)
     TtaError (ERR_invalid_element_type);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
     TtaError (ERR_invalid_document_parameter);
   else
     /* parameter document is correct */
     {
       pEl2 = ((PtrElement) element)->ElNext;
       if (((PtrElement) element)->ElStructSchema->SsRule[((PtrElement) element)->ElTypeNumber - 1].SrConstruct != CsConstant)
	 if (pEl2 != NULL)
	   if (pEl2->ElTerminal && pEl2->ElLeafType == LtText)
	     if (pEl2->ElLanguage == ((PtrElement) element)->ElLanguage)
	       if (pEl2->ElStructSchema->SsRule[pEl2->ElTypeNumber - 1].SrConstruct != CsConstant)
		 if (SameAttributes ((PtrElement) element, pEl2))
		   if (((PtrElement) element)->ElSource == NULL && pEl2->ElSource == NULL)
		     if (BothHaveNoSpecRules ((PtrElement) element, pEl2))
		       {
			 /* destroy the second element of the text */
			 DestroyAbsBoxes (pEl2, LoadedDocument[document - 1], FALSE);
			 MergeTextElements ((PtrElement) element, &FreeElement,
					    LoadedDocument[document - 1], FALSE, FALSE);
			 RedisplayMergedText ((PtrElement) element, document);
			 if (FreeElement != NULL)
			   DeleteElement (&FreeElement, LoadedDocument[document - 1]);
			 ok = TRUE;
		       }
     }
#endif
   return ok;
}


/*----------------------------------------------------------------------
   TtaSetGraphicsShape

   Changes the shape of a Graphics or Symbol basic element.
   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Graphics or Symbol.
   shape: new shape for that element.
   document: the document containing that element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaSetGraphicsShape (Element element, CHAR_T shape, Document document)
#else  /* __STDC__ */
void                TtaSetGraphicsShape (element, shape, document)
Element             element;
CHAR_T                shape;
Document            document;
#endif /* __STDC__ */
{
   int                 delta;
   ThotBool            polyline;
   PtrElement          pElAsc;

   UserErrorCode = 0;
   if (element == NULL)
     TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
     TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtSymbol &&
	    ((PtrElement) element)->ElLeafType != LtGraphics &&
	    ((PtrElement) element)->ElLeafType != LtPolyLine)
     TtaError (ERR_invalid_element_type);
   else
     {
	/* verifies the parameter document */
	if (document < 1 || document > MAX_DOCUMENTS)
	  TtaError (ERR_invalid_document_parameter);
	else if (LoadedDocument[document - 1] == NULL)
	  TtaError (ERR_invalid_document_parameter);
	else
	   /* parameter document is correct */
	  {
	     delta = 0;
	     if (((PtrElement) element)->ElLeafType == LtSymbol)
	       {
		  if (((PtrElement) element)->ElGraph == EOS &&
		      shape != EOS)
		    delta = 1;
		  else if (((PtrElement) element)->ElGraph != EOS &&
			   shape == EOS)
		    delta = -1;
	       }
	     else
	       {
		  polyline = (shape == TEXT('S') || shape == TEXT('U') || shape == TEXT('N') ||
			      shape == TEXT('M') || shape == TEXT('B') || shape == TEXT('A') ||
			      shape == TEXT('F') || shape == TEXT('D') || shape == TEXT('p') ||
			      shape == TEXT('s') || shape == TEXT('w') || shape == TEXT('x') ||
			      shape == TEXT('y') || shape == TEXT('z'));
		  if (polyline && ((PtrElement) element)->ElLeafType == LtGraphics)
		     /* changing simple graphic --> polyline */
		    {
		       ((PtrElement) element)->ElLeafType = LtPolyLine;
		       GetTextBuffer (&((PtrElement) element)->ElPolyLineBuffer);
		       ((PtrElement) element)->ElNPoints = 1;
		       ((PtrElement) element)->ElText->BuLength = 1;
		       ((PtrElement) element)->ElText->BuPoints[0].XCoord = 0;
		       ((PtrElement) element)->ElText->BuPoints[0].YCoord = 0;
		    }
		  else if (!polyline && ((PtrElement) element)->ElLeafType == LtPolyLine)
		     /* changing polyline --> simple graphic */
		    {
		       delta = -((PtrElement) element)->ElNPoints;
		       if (shape != EOS)
			  delta++;
		       ClearText (((PtrElement) element)->ElPolyLineBuffer);
		       FreeTextBuffer (((PtrElement) element)->ElPolyLineBuffer);
		       ((PtrElement) element)->ElLeafType = LtGraphics;
		    }
		  else if (((PtrElement) element)->ElLeafType == LtGraphics)
		     if (((PtrElement) element)->ElGraph == EOS &&
			 shape != EOS)
			delta = 1;
		     else if (((PtrElement) element)->ElGraph != EOS &&
			      shape == EOS)
			delta = -1;
	       }
	     if (((PtrElement) element)->ElLeafType == LtPolyLine)
		((PtrElement) element)->ElPolyLineType = shape;
	     else
		((PtrElement) element)->ElGraph = shape;
	     /* Updates the volumes of ancestors */
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
	     RedisplayLeaf ((PtrElement) element, document, delta);
#endif
	  }
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     PolylineOK (Element element, Document document)
#else  /* __STDC__ */
static ThotBool     PolylineOK (element, document)
Element             element;
Document            document;
#endif /* __STDC__ */
{
   ThotBool            ok;

   ok = FALSE;
   UserErrorCode = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
      TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtPolyLine)
      TtaError (ERR_invalid_element_type);
   else
      /* verifies the parameter document */
   if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else
      /* parameter document is correct */
      ok = TRUE;
   return ok;
}


/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/
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
   PtrTextBuffer       firstBuffer;
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
		x = PixelToPoint (x);
		y = PixelToPoint (y);
	     }

	   /* adds the point to the polyline */
	   AddPointInPolyline (firstBuffer, rank, x, y);
	   ((PtrElement) element)->ElNPoints++;
	   /* Updates the volumes of ancestors */
	   pEl = ((PtrElement) element)->ElParent;
	   while (pEl != NULL)
	     {
		pEl->ElVolume++;
		pEl = pEl->ElParent;
	     }
#ifndef NODISPLAY
	   RedisplayLeaf ((PtrElement) element, document, 1);
#endif
	}
}

/*----------------------------------------------------------------------
   TtaDeletePointInPolyline

   Deletes a point in a Polyline basic element.

   Parameters:
   element: the element to be changed. This element must
   be a basic element of type Polyline.
   rank: rank of the point to be deleted. If rank is greater
   than the actual number of points, the last point is deleted.
   rank must be strictly positive.
   document: the document containing the polyline element.

  ----------------------------------------------------------------------*/
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
	   /* Suppresses the point from the polyline */
	   DeletePointInPolyline (&(((PtrElement) element)->ElPolyLineBuffer), rank);
	   /* There is a point less in the element */
	   ((PtrElement) element)->ElNPoints--;
	   /* Updates the volumes of ancestors */
	   pEl = ((PtrElement) element)->ElParent;
	   while (pEl != NULL)
	     {
		pEl->ElVolume--;
		pEl = pEl->ElParent;
	     }
#ifndef NODISPLAY
	   RedisplayLeaf ((PtrElement) element, document, -1);
#endif
	}
}


/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/

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
		x = PixelToPoint (x);
		y = PixelToPoint (y);
	     }
	   ModifyPointInPolyline (((PtrElement) element)->ElPolyLineBuffer, rank, x, y);
#ifndef NODISPLAY
	   RedisplayLeaf ((PtrElement) element, document, 0);
#endif
	}
}


/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/
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
   PtrTextBuffer       firstBuffer;
   PtrTextBuffer       pBuff;
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
		x = PixelToPoint (x);
		y = PixelToPoint (y);
	     }
	   firstBuffer = ((PtrElement) element)->ElPolyLineBuffer;
	   /* verifies that the new point coordinates are greatest than all the coordinates of
	      the other points of the polyline */
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
	   /* Updates the coordinates of the new boundary point */
	   firstBuffer->BuPoints[0].XCoord = x;
	   firstBuffer->BuPoints[0].YCoord = y;
#ifndef NODISPLAY
	   RedisplayLeaf ((PtrElement) element, document, 0);
#endif
	}
}


/*----------------------------------------------------------------------
   TtaCopyPage

   Copies the page element source into the page element destination.
   Both page elements must be in an abstract tree.

   Parameters:
   destination: identifier of the page element to be modified.
   source : identifier of the source page element.

  ----------------------------------------------------------------------*/
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


/*----------------------------------------------------------------------
   TtaGetVolume

   Returns the current volume of the current element.

   Parameter:
   element: the element of interest.

   Return value:
   volume contained in the element.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetVolume (Element element)
#else  /* __STDC__ */
int                 TtaGetVolume (element)
Element             element;
#endif /* __STDC__ */
{
   int              volume;

   UserErrorCode = 0;
   volume = 0;
   if (element == NULL)
     TtaError (ERR_invalid_parameter);
   else
     volume = ((PtrElement) element)->ElVolume;
   return (volume);
}


/*----------------------------------------------------------------------
   TtaGetPictureType

   Returns the type of Picture element.

   Parameter:
   element: the element of interest. This element must be a Picture or have
   a bacground image.

   Return value:
   PicType: type of the element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PicType             TtaGetPictureType (Element element)
#else  /* __STDC__ */
PicType             TtaGetPictureType (element)
Element             element;
#endif /* __STDC__ */
{
   PtrAbstractBox   pAb;
   PicType          pictType;
   PictInfo        *imageDesc;
   int              typeImage;
   int              view;
   ThotBool         found;

   UserErrorCode = 0;
   pictType = unknown_type;
   if (element == NULL)
     TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
     {
       view = 0;
       pAb = NULL;
       found = FALSE;
       do
	 {
	   pAb = ((PtrElement) element)->ElAbstractBox[view];
	   if (pAb != NULL)
	     {
	       while (pAb->AbPresentationBox)
		 pAb = pAb->AbNext;
	       /* the background exist or will exist soon */
	       found = (pAb->AbPictBackground != NULL || pAb->AbAspectChange);
	     }
	   view++;
	 }
       while (!found && view < MAX_VIEW_DOC);

       if (!found)
	 TtaError (ERR_invalid_element_type);
       else
	 {
	   imageDesc = (PictInfo *) pAb->AbPictBackground;
	   if (imageDesc != NULL)
	     typeImage = imageDesc->PicType;
	   else
	     typeImage = UNKNOWN_FORMAT;
	 }
     }
   else if (((PtrElement) element)->ElTerminal &&
	    ((PtrElement) element)->ElLeafType != LtPicture)
     TtaError (ERR_invalid_element_type);
   else
     {
       imageDesc = (PictInfo *) (((PtrElement) element)->ElPictInfo);
       if (imageDesc != NULL)
	 {
	   typeImage = imageDesc->PicType;
	   if (typeImage != UNKNOWN_FORMAT)
	     pictType = (PicType) typeImage;
	 }
     }
   return pictType;
}

/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TtaGiveSubString (Element element, STRING buffer, int position, int length)

#else  /* __STDC__ */
void                TtaGiveSubString (element, buffer, position, length)
Element             element;
STRING              buffer;
int                 position;
int                 length;

#endif /* __STDC__ */

{
   PtrTextBuffer       pBuf;
   STRING              ptr;
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
	/* The begenning buffer is not considered */
	while (pBuf != NULL && len <= position)
	  {
	     position -= len;
	     pBuf = pBuf->BuNext;
	     len = pBuf->BuLength;
	  }
	/* pying into the buffer */
	while (pBuf != NULL && length > 0)
	  {
	     if (length + position < pBuf->BuLength)
		l = length;
	     else
		l = pBuf->BuLength - position;
	     ustrncpy (ptr, pBuf->BuContent + position, l);
	     ptr = ptr + l;
	     length = length - l;
	     pBuf = pBuf->BuNext;
	     position = 0;
	  }
	*ptr = EOS;
     }
}

/*----------------------------------------------------------------------
   TtaGetGraphicsShape

   Returns the content of a Graphics or Symbol basic element.

   Parameter:
   element: the element of interest. This element must be a basic
   element of type Graphics or Symbol.

   Return value:
   a single character representing the shape of the graphics element or
   symbol contained in the element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
CHAR_T                TtaGetGraphicsShape (Element element)
#else  /* __STDC__ */
CHAR_T                TtaGetGraphicsShape (element)
Element             element;
#endif /* __STDC__ */
{
   CHAR_T                content;

   UserErrorCode = 0;
   content = EOS;
   if (element == NULL)
     TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
     TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtSymbol &&
	    ((PtrElement) element)->ElLeafType != LtGraphics &&
	    ((PtrElement) element)->ElLeafType != LtPolyLine)
     TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType == LtPolyLine)
     content = ((PtrElement) element)->ElPolyLineType;
   else
     content = ((PtrElement) element)->ElGraph;
   return content;
}

/*----------------------------------------------------------------------
   TtaGetPolylineLength

   Returns the number of points in a Polyline basic element.

   Parameter:
   element: the Polyline element. This element must
   be a basic element of type Polyline.
  ----------------------------------------------------------------------*/
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
   else if (((PtrElement) element)->ElLeafType != LtPolyLine)
      TtaError (ERR_invalid_element_type);
   else
      /* one ignore the boundary point limite, considered in ElNPoints */
      return ((PtrElement) element)->ElNPoints - 1;
   return (0);
}

/*----------------------------------------------------------------------
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

  ----------------------------------------------------------------------*/
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
   PtrTextBuffer       pBuff;

   UserErrorCode = 0;
   *x = 0;
   *y = 0;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) element)->ElTerminal)
      TtaError (ERR_invalid_element_type);
   else if (((PtrElement) element)->ElLeafType != LtPolyLine)
      TtaError (ERR_invalid_element_type);
   else if (unit != UnPoint && unit != UnPixel)
      TtaError (ERR_invalid_parameter);
   else
     {
	/* Looking for the buffer containing the point which rank is: rank */
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
	     *x = pBuff->BuPoints[rank].XCoord;
	     *y = pBuff->BuPoints[rank].YCoord;
	     /* Convert values to millipoints */
	     if (unit == UnPixel)
	       {
		  *x = PointToPixel (*x);
		  *y = PointToPixel (*y);
	       }
	     *x = *x / 1000;
	     *y = *y / 1000;
	  }
     }
}


/*----------------------------------------------------------------------
   TtaGetPageNumber

   Returns the page number of a Page basic element.

   Parameter:
   pageElement: the page element.

   Return value:
   page number of that page element.
  ----------------------------------------------------------------------*/
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
     TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) pageElement)->ElTerminal)
     TtaError (ERR_invalid_element_type);
   else if (((PtrElement) pageElement)->ElLeafType != LtPageColBreak)
     TtaError (ERR_invalid_element_type);
   else
     pageNumber = ((PtrElement) pageElement)->ElPageNumber;
   return pageNumber;
}

/*----------------------------------------------------------------------
   TtaGetPageView

   Returns the view corresponding to a Page basic element.

   Parameter:
   pageElement: the page element.

   Return value:
   view of that page.

  ----------------------------------------------------------------------*/
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
	TtaError (ERR_invalid_parameter);
   else if (!((PtrElement) pageElement)->ElTerminal)
	TtaError (ERR_invalid_element_type);
   else if (((PtrElement) pageElement)->ElLeafType != LtPageColBreak)
	TtaError (ERR_invalid_element_type);
   else
      pageView = ((PtrElement) pageElement)->ElViewPSchema;
   return pageView;
}

/* End of the module */
