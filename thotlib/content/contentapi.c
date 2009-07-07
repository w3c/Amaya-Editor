/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Content API routines
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA) - Polylines
 *          F. Wang - More operations on transform matrix and paths/polyline,
 *                    shape recognition.
 */
#include "thot_gui.h"
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
#include "ustring.h"

#undef THOT_EXPORT
/*#define THOT_EXPORT*/
#define THOT_EXPORT extern	/* to avoid redefinitions */
#include "edit_tv.h"
#include "select_tv.h"
#ifndef NODISPLAY
#include "frame_tv.h"
#endif

#include "abspictures_f.h"
#include "attributes_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "geom_f.h"
#include "memory_f.h"
#include "structselect_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "units_f.h"
#include "viewapi_f.h"
#include "writepivot_f.h"

#ifdef _GL
#include "animbox_f.h"
#include "glgradient_f.h"
#define ALLOC_POINTS 100
#endif /* _GL */


/*----------------------------------------------------------------------
  InsertText   inserts the string "content" in the element 
  pEl (which must be of type text), at the position
  "position". If "document" is null, we have not to consider
  the selection nor the redisplay
  ----------------------------------------------------------------------*/
static void InsertText (PtrElement pEl, int position, unsigned char *content,
                        Document document)
{
  PtrTextBuffer       pBuf, newBuf;
#ifndef NODISPLAY
  PtrDocument         selDoc;
  PtrElement          firstSelection, lastSelection;
  int                 firstChar, lastChar;
  ThotBool            selOk, changeSelection = FALSE;
#endif /* NODISPLAY */
  int                 length, delta, lengthBefore;
  PtrElement          pElAsc;

  length = strlen ((char *)content);
  if (length > 0)
    {
      /* corrects the parameter position */
      if (position > pEl->ElTextLength)
        position = pEl->ElTextLength;

      /* looks for the buffer pBuf where the insertion has to be done */
      pBuf = pEl->ElText;
      lengthBefore = 0;
      while (pBuf->BuNext && lengthBefore + pBuf->BuLength < position)
        {
          lengthBefore += pBuf->BuLength;
          pBuf = pBuf->BuNext;
        }
      /* Length of the remaining of the buffer */
      lengthBefore = - lengthBefore + position;
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
          /* creates a buffer for the second part of the text */
          newBuf = NewTextBuffer (pBuf);
          ustrcpy (newBuf->BuContent, pBuf->BuContent + lengthBefore);
          newBuf->BuLength = pBuf->BuLength - lengthBefore;
        }
      pBuf->BuContent[lengthBefore] = EOS;
      pBuf->BuLength = lengthBefore;
      length = CopyMBs2Buffer (content, pBuf, lengthBefore, length);
      delta = length;
      pEl->ElTextLength += length;
      pEl->ElVolume += length;

#ifndef NODISPLAY
      if (document > 0)
        {
          /* modifies the selection if the element belongs to it */
          selOk = GetCurrentSelection (&selDoc, &firstSelection, &lastSelection, &firstChar, &lastChar);
          changeSelection = FALSE;
          if (selOk && selDoc == LoadedDocument[document - 1] &&
              (pEl == firstSelection || pEl == lastSelection))
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
                    firstChar += length;
                }
              if (pEl == lastSelection &&
                  /* The element is at the end of the selection */
                  position < lastChar)
                lastChar += length;
            }
          RedisplayLeaf (pEl, document, delta);
          /* Sets up a new selection if the element is within it */
          if (changeSelection)
            {
              if (firstChar > 0)
                if (lastSelection == firstSelection)
                  TtaSelectString (document, (Element) firstSelection,
                                   firstChar, lastChar);
                else
                  TtaSelectString (document, (Element) firstSelection,
                                   firstChar, 0);
              else
                TtaSelectElement (document, (Element) firstSelection);
              if (lastSelection != firstSelection)
                TtaExtendSelection (document, (Element) lastSelection,
                                    lastChar);
            }
        }
#endif /* NODISPLAY */
      /* Updates the volumes of ancestors */
      pElAsc = pEl->ElParent;
      while (pElAsc != NULL)
        {
          pElAsc->ElVolume += delta;
          pElAsc = pElAsc->ElParent;
        }
    }
}

/*----------------------------------------------------------------------
  SetContent
  ----------------------------------------------------------------------*/
static void SetContent (Element element, const unsigned char *content,
                        Language language, Document document)
{
  PtrTextBuffer       pBuf, pNextBuff;
  PtrElement          pEl;
  ThotPictInfo       *image;
  int                 length, delta = 0, i;
  int                 max;
#ifndef NODISPLAY
  PtrDocument         selDoc;
  PtrElement          firstSelection, lastSelection;
  int                 firstChar, lastChar;
  ThotBool            selOk, changeSelection;
#endif /* NODISPLAY */

  UserErrorCode = 0;
  pEl = NULL;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElLeafType != LtText &&
           ((PtrElement) element)->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    {
      /* parameter document is correct */
      pEl = (PtrElement) element;
      if (pEl->ElLeafType == LtText)
        pEl->ElLanguage = language;
      
      /* store the contents of the element */
      if (content)
        {
          max = strlen ((char *)content);
          pBuf = pEl->ElText;
          if (pBuf == NULL)
            {
              GetTextBuffer (&pEl->ElText);
              pBuf = pEl->ElText;
            }
          if (pBuf && max > 0)
            length = CopyMBs2Buffer (content, pBuf, 0, max);
          else
            length = 0;
        }
      else
        length = 0;

      delta = length - pEl->ElTextLength;
      pEl->ElTextLength = length;
      pEl->ElVolume = length;

      /* Point to the first buffer to be released */
      pBuf = pEl->ElText;
      if (length == 0)
        /* keep the first buffer, even if it's empty */
        {
          pBuf = pEl->ElText;
          if (pBuf)
            pBuf = pBuf->BuNext;	  
        }
      else
        {
          i = 0;
          while (pBuf && i < length)
            {
              i += pBuf->BuLength;
              pBuf = pBuf->BuNext;
            }
        }
      /* Release extra buffers */
      while (pBuf)
        {
          pNextBuff = pBuf->BuNext;
#ifdef NODISPLAY
          FreeTextBuffer (pBuf);
#else /* NODISPLAY */
          DeleteBuffer (pBuf, ActiveFrame);
#endif /* NODISPLAY */
          pBuf = pNextBuff;
        }

      if (pEl->ElLeafType == LtPicture && pEl->ElPictInfo)
        {
          /* Releases the  pixmap */
          image = (ThotPictInfo *)pEl->ElPictInfo;
          CleanPictInfo (image);
          /* the new image may be in a different format */
          image->PicType = -1;
          /* change the filename of the image */
          TtaFreeMemory (image->PicFileName);
          image->PicFileName = (char *)TtaStrdup ((char *)content);
        }

#ifndef NODISPLAY
      /* modifies the selection if the element is within it */
      selOk = GetCurrentSelection (&selDoc, &firstSelection,
                                   &lastSelection, &firstChar,
                                   &lastChar);
      changeSelection = FALSE;
      if (selOk && selDoc == LoadedDocument[document - 1] &&
          AbsBoxSelectedAttr == NULL)
        {
          if (pEl == firstSelection)
            /* The selection starts in the element */
            {
              TtaSelectElement (document, NULL);
              changeSelection = TRUE;
              if (firstChar > length + 1)
                /* Selection begins beyond the new length */
                firstChar = length + 1;
            }
          if (pEl == lastSelection)
            /* The selection ends in the element */
            {
              if (!changeSelection)
                TtaSelectElement (document, NULL);
              changeSelection = TRUE;
              if (lastChar > length + 1)
                /* Selection ends beyond the new length */
                lastChar = length + 1;
            }
        }
      RedisplayLeaf (pEl, document, delta);
      /* Sets up a new selection if the element is within it */
      if (changeSelection)
        {
          if (firstChar == 0)
            TtaSelectElement (document, (Element) firstSelection);
          else
            {
              if (lastSelection == firstSelection)
                TtaSelectString (document, (Element) firstSelection,
                                 firstChar, lastChar);
              else
                TtaSelectString (document, (Element) firstSelection,
                                 firstChar, 0);
            }
          if (lastSelection != firstSelection)
            TtaExtendSelection (document, (Element) lastSelection,
                                lastChar);
        }
#endif /* NODISPLAY */
      /* Updates the volumes of the ancestors */
      pEl = pEl->ElParent;
      while (pEl != NULL)
        {
          pEl->ElVolume += delta;
          pEl = pEl->ElParent;
        }
    }
}

/*----------------------------------------------------------------------
  TtaSetTextContent

  Changes the content of a Text basic element. The full content (if any) is
  deleted and replaced by the new one.
  This function can also be used for changing the content (the file name)
  of a Picture basic element.
  Parameters:
  element: the Text element to be modified.
  content: new content for that element coded in ISO-Latin or UTF-8.
  language: language of that Text element.
  document: the document containing that element.
  ----------------------------------------------------------------------*/
void TtaSetTextContent (Element element, const unsigned char *content,
                        Language language, Document document)
{
  SetContent (element, content, language, document);
}

/*----------------------------------------------------------------------
  TtaSetPictureContent

  N.B. For the moment, this function is identical to TtaSetTextContent,
  except that we also give a MIME type. It should evolve with time.
  Changes the content of a Text basic element. The full content (if any) is
  deleted and replaced by the new one.
  This function can also be used for changing the content (the file name)
  of a Picture basic element.
  Parameters:
  element: the Text element to be modified.
  content: new content for that element coded in ISO-Latin or UTF-8..
  language: language of that Text element.
  document: the document containing that element.
  mime_type: MIME tpye of the picture
  ----------------------------------------------------------------------*/
void TtaSetPictureContent (Element element, const unsigned char *content,
                           Language language, Document document, const char *mime_type)
{
  SetContent (element, content, language, document);
#ifndef NODISPLAY
  /* @@ Probably create the pictinfo el here and add the mime type. 
     Redisplayleaf is doing so further down */
#endif /* NODISPLAY */
}


/*----------------------------------------------------------------------
  TtaAppendTextContent

  Appends a character string at the end of a Text basic element.
  Parameters:
  element: the Text element to be modified.
  content: the character string to be appended.
  document: the document containing that element.
  ----------------------------------------------------------------------*/
void TtaAppendTextContent (Element element, unsigned char *content,
                           Document document)
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
      InsertText ((PtrElement) element, ((PtrElement) element)->ElTextLength,
                  content, document);
}


/*----------------------------------------------------------------------
  TtaHasFinalSpace

  Returns TRUE if the text element ends with a space or a NEWLINE
  Parameters:
  element: the Text element to be modified.
  document: the document containing that element.
  ----------------------------------------------------------------------*/
ThotBool TtaHasFinalSpace (Element element, Document document)
{
  PtrElement          pEl;
  PtrTextBuffer       pBuf;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  /* check parameters */
  if (document < 1 || document > MAX_DOCUMENTS || element == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (pEl->ElTerminal && pEl->ElLeafType == LtText)
    {
      /* go to the last buffer */
      pBuf = pEl->ElText;
      while (pBuf->BuNext)
        pBuf = pBuf->BuNext;
      if (pBuf->BuContent[pBuf->BuLength - 1] == SPACE ||
          pBuf->BuContent[pBuf->BuLength - 1] == EOL)
        /* there is almost one space at the end of the text element */
        return TRUE;
    }
  return FALSE;
}


/*----------------------------------------------------------------------
  TtaRemoveInitialSpaces

  Removes spaces and NEWLINE at the beginning of the text element
  Parameters:
  element: the Text element to be modified.
  document: the document containing that element.
  ----------------------------------------------------------------------*/
void TtaRemoveInitialSpaces (Element element, Document document)
{
  PtrElement          pEl, pElAsc;
  PtrTextBuffer       pBuf, pNext;
  int                 i, l, delta;
  ThotBool            still;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  /* check parameters */
  if (document < 1 || document > MAX_DOCUMENTS || element == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (pEl->ElTerminal && pEl->ElLeafType == LtText)
    {
      pBuf = pEl->ElText;
      delta = 0;
      still = (pBuf != NULL);
      while (still)
        {
          /* there is almost one space at the end of the text element */
          i = 0;
          while (i <= pBuf->BuLength - 1 &&
                 (pBuf->BuContent[i] == SPACE || pBuf->BuContent[i] == EOL))
            {
              i++;
              delta++;
            }
          still = (i == pBuf->BuLength);
          if (still)
            {
              /* remove the last buffer */
              pNext = pBuf->BuNext;
              if (pNext)
                {
                  pNext->BuPrevious = NULL;
#ifdef NODISPLAY
                  FreeTextBuffer (pBuf);
#else /* NODISPLAY */
                  DeleteBuffer (pBuf, ActiveFrame);
#endif /* NODISPLAY */
                  pBuf = pNext;
                }
              else
                /* stop if there is no next buffer */ 
                still = FALSE;
            }
          else if ( i > 0)
            {
              /* erase initial spaces */
              l = pBuf->BuLength - delta;
              for (i = 0; i < l; i++)
                pBuf->BuContent[i] = pBuf->BuContent[delta + i];
              pBuf->BuContent[i] = EOS;
              pBuf->BuLength = l;
            }
        }

      if (delta)
        {
          if (delta == pEl->ElVolume)
            /* empty TEXT element */
            TtaDeleteTree (element, document);
          else
            {
              pEl->ElTextLength -= delta;
              /* Updates the volume of the element and its ancestors */
              pElAsc = pEl;
              while (pElAsc)
                {
                  pElAsc->ElVolume -= delta;
                  pElAsc = pElAsc->ElParent;
                }
#ifndef NODISPLAY
              /* Redisplays the element */
              RedisplayLeaf (pEl, document, -delta);
#endif /* NODISPLAY */
            }
        }
    }
}


/*----------------------------------------------------------------------
  TtaRemoveFinalSpaces

  Removes spaces and NEWLINE at the end of the text element if the
  parameter all is TRUE, only the last CR/EOL if the parameter is FALSE.
  Parameters:
  element: the Text element to be modified.
  document: the document containing that element.
  all: when TRUE removes all spaces, when FALSE only the last CR/EOL.
  ----------------------------------------------------------------------*/
void TtaRemoveFinalSpaces (Element element, Document document,
                           ThotBool all)
{
  PtrElement          pEl, pElAsc;
  PtrTextBuffer       pBuf, pPrev;
  int                 i, delta, last;
  ThotBool            still;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  /* check parameters */
  if (document < 1 || document > MAX_DOCUMENTS || element == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (pEl->ElTerminal && pEl->ElLeafType == LtText)
    {
      pBuf = pEl->ElText;
      pPrev = NULL;
      delta = 0;
      /* go to the last buffer */
      while (pBuf && pBuf->BuNext)
        pBuf = pBuf->BuNext;
      still = (pBuf != NULL);
      if (!all)
        {
          i = pBuf->BuLength - 1;
          if (pBuf->BuContent[i] == __CR__ || pBuf->BuContent[i] == EOL)
            {
              delta++;
              pBuf->BuContent[i] = EOS;
              pBuf->BuLength = i;
            }
        }
      else
        {
          while (still)
            {
              pPrev = pBuf->BuPrevious;
              if (pPrev)
                last = pPrev->BuLength - 1;
              else
                last = 0;
              i = pBuf->BuLength - 1;
              /* is there at least one space at the end of the text element ? */
	      if (!strcmp (pEl->ElStructSchema->SsName, "MathML") ||
		  !strcmp (pEl->ElStructSchema->SsName, "SVG"))
		/* for MathML and SVG remove all trailing spaces */
		while (i >= 0 &&
		       (pBuf->BuContent[i] == SPACE ||
			pBuf->BuContent[i] == __CR__ ||
			pBuf->BuContent[i] == EOL))
		  {
		    i--;
		    delta++;
		  }
	      else
		/* for all other XML dialects, keep the last trailing space */
		while (i >= 0 &&
		       (pBuf->BuContent[i] == SPACE ||
			pBuf->BuContent[i] == __CR__ ||
			pBuf->BuContent[i] == EOL) &&
		       ((i > 0 &&
			 (pBuf->BuContent[i-1] == SPACE ||
			  pBuf->BuContent[i-1] == __CR__ ||
			  pBuf->BuContent[i-1] == EOL)) ||
			(i == 0 && pPrev &&
			 (pPrev->BuContent[last] == SPACE ||
			  pPrev->BuContent[last] == __CR__ ||
			  pPrev->BuContent[last] == EOL))))
		  {
		    i--;
		    delta++;
		  }
              still = (i < 0);
              if (still)
                {
                  /* remove the last buffer */
                  if (pPrev)
                    {
                      pPrev->BuNext = NULL;
#ifdef NODISPLAY
                      FreeTextBuffer (pBuf);
#else /* NODISPLAY */
                      DeleteBuffer (pBuf, ActiveFrame);
#endif /* NODISPLAY */
                      pBuf = pPrev;
                    }
                  else
                    /* stop if there is no previous buffer */ 
                    still = FALSE;
                }
              else if ( pBuf->BuLength != i + 1)
                {
                  pBuf->BuContent[i + 1] = EOS;
                  pBuf->BuLength = i + 1;
                }
            }
        }
      
      if (delta)
        {
          if (delta == pEl->ElVolume)
            /* empty TEXT element */
            TtaDeleteTree (element, document);
          else
            {
              pEl->ElTextLength -= delta;
              /* Updates the volume of the element and its ancestors */
              pElAsc = pEl;
              while (pElAsc)
                {
                  pElAsc->ElVolume -= delta;
                  pElAsc = pElAsc->ElParent;
                }
#ifndef NODISPLAY
              /* Redisplays the element */
              RedisplayLeaf (pEl, document, -delta);
#endif /* NODISPLAY */
            }
        }
    }
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
void TtaInsertTextContent (Element element, int position, unsigned char *content,
                           Document document)
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
void TtaDeleteTextContent (Element element, int position, int length,
                           Document document)
{
  PtrTextBuffer       pBufFirst, pBufLast, pBufNext;
  STRING              dest, source;
  PtrElement          pElAsc, pEl;
#ifndef NODISPLAY
  PtrDocument         selDoc;
  PtrElement          firstSelection, lastSelection;
  int                 firstChar, lastChar;
  ThotBool            selOk, changeSelection;
#endif /* NODISPLAY */
  int                 delta, lengthBefore, firstDeleted;
  int                 lastDeleted, l, i;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pEl->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (pEl->ElLeafType != LtText &&
           pEl->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (length < 0 || position <= 0 || position > pEl->ElTextLength)
    TtaError (ERR_invalid_parameter);
  else if (length > 0)
    {
      /* verifies that the parameter length is not too big */
      if (position + length > pEl->ElTextLength + 1)
        length = pEl->ElTextLength - position + 1;
#ifndef NODISPLAY
      /* modifies the selection if the element belongs to it */
      selOk = GetCurrentSelection (&selDoc, &firstSelection, &lastSelection,
                                   &firstChar, &lastChar);
      changeSelection = FALSE;
      if (selOk && selDoc == LoadedDocument[document - 1] &&
          (pEl == firstSelection || pEl == lastSelection))
        /* The selection starts and/or stops in the element */
        /* First, we abort the selection */
        {
          TtaSelectElement (document, NULL);
          changeSelection = TRUE;
          if (lastChar > 1)
            lastChar -= 1;
          if (pEl == firstSelection)
            /* The element is at the beginning of the selection */
            {
              if (firstChar > position)
                {
                  firstChar -= length;
                  if (firstChar < position)
                    firstChar = position;
                }
            }
          if (pEl == lastSelection)
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
#endif /* NODISPLAY */
      delta = length;
      pEl->ElTextLength -= delta;
      pEl->ElVolume -= delta;
      /* Looks for the buffer where the string to be suppressed starts*/
      pBufFirst = pEl->ElText;
      lengthBefore = 0;
      while (pBufFirst->BuNext != NULL &&
             lengthBefore + pBufFirst->BuLength < position)
        {
          lengthBefore += pBufFirst->BuLength;
          pBufFirst = pBufFirst->BuNext;
        }
      /* Length of the buffer containing the beginning of the string to be
         suppress */
      firstDeleted = position - lengthBefore;
      /* Looks for the buffer pBufLast containing the end of the string to
         be suppressed and releases the intermediate buffers. The buffers
         containing the begenning of the string to be suppressed and its
         end are not released */
      pBufLast = pBufFirst;
      lastDeleted = firstDeleted + length - 1;
      while (pBufLast->BuNext != NULL &&
             lastDeleted > pBufLast->BuLength)
        {
          lastDeleted -= pBufLast->BuLength;
          if (pBufLast != pBufFirst)
            /* This is not the buffer containing the begenning of the 
               tring. It is released */
            {
              pBufNext = pBufLast->BuNext;
              pBufLast->BuPrevious->BuNext = pBufLast->BuNext;
              if (pBufLast->BuNext != NULL)
                pBufLast->BuNext->BuPrevious = pBufLast->BuPrevious;
#ifdef NODISPLAY
              FreeTextBuffer (pBufLast);
#else /* NODISPLAY */
              DeleteBuffer (pBufLast, ActiveFrame);
#endif /* NODISPLAY */
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
          dest = &pBufFirst->BuContent[firstDeleted - 1];
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
      source = &pBufLast->BuContent[lastDeleted];
      i = 0;
      while (source[i] != EOS)
        {
          dest[i] = source[i];
          i++;
        }
      dest[i] = EOS;
      pBufLast->BuLength -= l;
      /* If the buffers of the begening and the end of the suppresses string
         are empty, they are released. A buffer is kept for the element */
      if (pBufFirst->BuLength == 0 &&
          (pBufFirst->BuPrevious || pBufFirst->BuNext))
        /* If the buffer of the begenning is empty, it is released */
        {
          if (pBufFirst->BuPrevious)
            pBufFirst->BuPrevious->BuNext = pBufFirst->BuNext;
          else
            pEl->ElText = pBufFirst->BuNext;
          if (pBufFirst->BuNext)
            pBufFirst->BuNext->BuPrevious = pBufFirst->BuPrevious;
#ifdef NODISPLAY
          FreeTextBuffer (pBufFirst);
#else /* NODISPLAY */
          DeleteBuffer (pBufFirst, ActiveFrame);
#endif /* NODISPLAY */
        }
      if (pBufFirst != pBufLast && pBufLast->BuLength == 0 &&
          (pBufLast->BuPrevious || pBufLast->BuNext))
        /* The buffer of the end is empty. It is released */
        {
          if (pBufLast->BuPrevious)
            pBufLast->BuPrevious->BuNext = pBufLast->BuNext;
          else
            pEl->ElText = pBufLast->BuNext;
          if (pBufLast->BuNext)
            pBufLast->BuNext->BuPrevious = pBufLast->BuPrevious;
#ifdef NODISPLAY
          FreeTextBuffer (pBufLast);
#else /* NODISPLAY */
          DeleteBuffer (pBufLast, ActiveFrame);
#endif /* NODISPLAY */
        }
      /* Updates the volumes of the ancestors */
      pElAsc = pEl->ElParent;
      while (pElAsc)
        {
          pElAsc->ElVolume -= delta;
          pElAsc = pElAsc->ElParent;
        }
#ifndef NODISPLAY
      /* Redisplays the element */
      RedisplayLeaf (pEl, document, -delta);
      /* Sets up a new selection if the element belongs to it */
      if (changeSelection)
        {
          if (firstChar > 0)
            {
              if (lastSelection == firstSelection)
                TtaSelectString (document, (Element) firstSelection,
                                 firstChar, lastChar);
              else
                TtaSelectString (document, (Element) firstSelection,
                                 firstChar, 0);
            }
          else
            TtaSelectElement (document, (Element) firstSelection);
          if (lastSelection != firstSelection)
            TtaExtendSelection (document, (Element) lastSelection,
                                lastChar);
        }
#endif /* NODISPLAY */
    }
}

/*----------------------------------------------------------------------
  TtaSplitText

  Divides a text element into two elements.
  Parameters:
  element: the text element to be divided. A new text element containing
  the second part of the text is created as the next sibling.
  position: rank of the character before which the element must be cut.
  document: the document to which the element belongs.
  ----------------------------------------------------------------------*/
void TtaSplitText (Element element, int position, Document document)
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
      SplitTextElement ((PtrElement) element, position,
                        LoadedDocument[document - 1], FALSE, &secondPart,
                        FALSE);
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
ThotBool TtaMergeText (Element element, Document document)
{
  ThotBool            ok = FALSE;
#ifndef NODISPLAY
  PtrElement          FreeElement;
  PtrElement          pEl2, pEl;

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
      pEl = (PtrElement) element;
      pEl2 = pEl->ElNext;
      if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct != CsConstant &&
          pEl2 && pEl2->ElTerminal && pEl2->ElLeafType == LtText &&
          pEl2->ElLanguage == pEl->ElLanguage &&
          pEl2->ElStructSchema->SsRule->SrElem[pEl2->ElTypeNumber - 1]->SrConstruct != CsConstant)
        if (SameAttributes (pEl, pEl2) &&
            pEl->ElSource == NULL && pEl2->ElSource == NULL &&
            BothHaveNoSpecRules (pEl, pEl2))
          {
            /* destroy the second element of the text */
            DestroyAbsBoxes (pEl2, LoadedDocument[document - 1], FALSE);
            ok = MergeTextElements (pEl,
                                    &FreeElement, LoadedDocument[document - 1],
                                    FALSE, FALSE);
            RedisplayMergedText (pEl, document);
            if (FreeElement != NULL)
              DeleteElement (&FreeElement, LoadedDocument[document - 1]);
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
void TtaSetGraphicsShape (Element element, char shape, Document document)
{
  int                 delta;
  ThotBool            polyline;
  PtrElement          pElAsc, pEl;
  PtrPathSeg          pPa, pPaNext;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElLeafType != LtSymbol &&
           ((PtrElement) element)->ElLeafType != LtGraphics &&
           ((PtrElement) element)->ElLeafType != LtPolyLine &&
           ((PtrElement) element)->ElLeafType != LtPath)
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
          pEl = (PtrElement) element;
          if(pEl->ElLeafType == LtPath)
            {
              if(shape == EOS)
                return;

              /* changing path --> simple graphic */
              delta = -pEl->ElVolume;
              if (shape != EOS)
                delta++;

              pPa = pEl->ElFirstPathSeg;
              pEl->ElFirstPathSeg = NULL;
              do
                {
                  pPaNext = pPa->PaNext;
                  FreePathSeg (pPa);
                  pPa = pPaNext;
                }
              while (pPa);

              pEl->ElLeafType = LtGraphics;
            }
          else if (pEl->ElLeafType == LtSymbol)
            {
              if (pEl->ElGraph == EOS && shape != EOS)
                delta = 1;
              else if (pEl->ElGraph != EOS && shape == EOS)
                delta = -1;
              pEl->ElWideChar = 0;
            }
          else
            {
              polyline = (shape == 'S' || shape == 'U' || shape == 'N' ||
                          shape == 'M' || shape == 'B' || shape == 'A' ||
                          shape == 'F' || shape == 'D' || shape == 'p' ||
                          shape == 's' || shape == 'w' || shape == 'x' ||
                          shape == 'y' || shape == 'z' || shape == 'g');
              if (polyline && pEl->ElLeafType == LtGraphics)
                /* changing simple graphic --> polyline */
                {
                  pEl->ElLeafType = LtPolyLine;
                  GetTextBuffer (&pEl->ElPolyLineBuffer);
                  pEl->ElNPoints = 1;
                  pEl->ElText->BuLength = 1;
                  pEl->ElText->BuPoints[0].XCoord = 0;
                  pEl->ElText->BuPoints[0].YCoord = 0;
                }
              else if (!polyline && pEl->ElLeafType == LtPolyLine)
                /* changing polyline --> simple graphic */
                {
                  delta = -pEl->ElNPoints;
                  if (shape != EOS)
                    delta++;
                  ClearText (pEl->ElPolyLineBuffer);
                  FreeTextBuffer (pEl->ElPolyLineBuffer);
                  pEl->ElLeafType = LtGraphics;
                }
              else if (pEl->ElLeafType == LtGraphics)
                {
                  if (pEl->ElGraph == EOS && shape != EOS)
                    delta = 1;
                  else if (pEl->ElGraph != EOS && shape == EOS)
                    delta = -1;
                }
            }
          if (pEl->ElLeafType == LtPolyLine)
            pEl->ElPolyLineType = shape;
          else
            pEl->ElGraph = shape;

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
  TtaSetSymbolCode

  Changes the wide char code associated with a Symbol basic element.
  Parameters:
  element: the element to be changed. This element must
  be a basic element of type Symbol whose shape is '?'
  code: wide char code
  document: the document containing that element.
  ----------------------------------------------------------------------*/
void TtaSetSymbolCode (Element element, wchar_t code, Document document)
{
  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElLeafType != LtSymbol)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElGraph != '?')
    TtaError (ERR_invalid_element_type);
  else if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else
    /* parameter document is correct */
    ((PtrElement) element)->ElWideChar = code;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static ThotBool PolylineOK (Element element, Document document)
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
  else if (document < 1 || document > MAX_DOCUMENTS)
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
int TtaAddPointInPolyline (Element element, int rank, TypeUnit unit,
                           int x, int y, Document document,
                           ThotBool IsBarycenter)
{
  PtrTextBuffer       firstBuffer;
  PtrElement          pEl;
  ThotBool IsClosed;
  int newpos;

  if (PolylineOK (element, document))
    {
      /* Cannot found why this two test
         were here, as it works for me */
      if (rank <= 0)/* || x < 0 || y < 0)*/
        TtaError (ERR_invalid_parameter);
      else if (unit != UnPoint && unit != UnPixel)
        TtaError (ERR_invalid_parameter);
      else
        {
          pEl = (PtrElement) element;
          IsClosed = (pEl->ElPolyLineType == 'p');
          firstBuffer = pEl->ElPolyLineBuffer;
          /* adds the point to the polyline */
          newpos = AddPointInPolyline (firstBuffer, rank, x, y, IsBarycenter, IsClosed);

          pEl->ElNPoints++;

          /* The new pos is at the end */
          if(newpos == -1)
            newpos = pEl->ElNPoints - 1;

          /* Updates the volumes of ancestors */
          pEl = pEl->ElParent;
          while (pEl != NULL)
            {
              pEl->ElVolume++;
              pEl = pEl->ElParent;
            }
#ifndef NODISPLAY
          RedisplayLeaf ((PtrElement) element, document, 1);
#endif
          return newpos;
        }
    }
  return 0;
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
void TtaDeletePointInPolyline (Element element, int rank, Document document)
{
  PtrElement          pEl;

  if (PolylineOK (element, document))
    {
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
void TtaModifyPointInPolyline (Element element, int rank, TypeUnit unit,
                               int x, int y, Document document)
{
  if (PolylineOK (element, document))
    {
      if (rank <= 0 || x < 0 || y < 0)
        TtaError (ERR_invalid_parameter);
      else if (unit != UnPoint && unit != UnPixel)
        TtaError (ERR_invalid_parameter);
      else
        {
          ModifyPointInPolyline (((PtrElement) element)->ElPolyLineBuffer,
                                 rank, x, y);
#ifndef NODISPLAY
          RedisplayLeaf ((PtrElement) element, document, 0);
#endif
        }
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
void TtaChangeLimitOfPolyline (Element element, TypeUnit unit, int x, int y,
                               Document document)
{
  PtrTextBuffer       firstBuffer;
  PtrTextBuffer       pBuff;
  int                 rank;

  if (PolylineOK (element, document))
    {
      if (x < 0 || y < 0) 
        TtaError (ERR_invalid_parameter); 
      else if (unit != UnPoint && unit != UnPixel)
        TtaError (ERR_invalid_parameter);
      else
        {
          firstBuffer = ((PtrElement) element)->ElPolyLineBuffer;
          /* verifies that the new point coordinates are greatest than all
             the coordinates of the other points of the polyline */
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
          if (x != firstBuffer->BuPoints[0].XCoord ||
              y != firstBuffer->BuPoints[0].YCoord)
            {
              /* Updates the coordinates of the new boundary point */
              firstBuffer->BuPoints[0].XCoord = x;
              firstBuffer->BuPoints[0].YCoord = y;
#ifndef NODISPLAY
              RedisplayLeaf ((PtrElement) element, document, 0);
#endif
            }
        }
    }
}

/*----------------------------------------------------------------------
  TtaRemovePathData
  Remove the path data attached to an element
  ----------------------------------------------------------------------*/
void TtaRemovePathData (Document document, Element element)
{
  PtrPathSeg          pPa, pPaNext;
  PtrElement pEl = ((PtrElement) (TtaGetFirstLeaf(element)));
  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* verifies the parameter document */
    if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
    else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
    else
      {
        /* parameter document is correct */
        if(pEl->ElLeafType == LtPath && pEl->ElFirstPathSeg)
          {
            pPa = pEl->ElFirstPathSeg;
            pEl->ElFirstPathSeg = NULL;
            do
              {
                pPaNext = pPa->PaNext;
                FreePathSeg (pPa);
                pPa = pPaNext;
              }
            while (pPa);
          }
      }
}

/*----------------------------------------------------------------------
  TtaNumberOfPointsInPath
  ----------------------------------------------------------------------*/
int TtaNumberOfPointsInPath(Element el)
{
  PtrElement pEl = ((PtrElement)el);
  PtrPathSeg          pPa;
  int n = 0;

  if(pEl)
    {
      if(pEl->ElLeafType == LtPath)
	{
	  for(pPa = pEl->ElFirstPathSeg; pPa; pPa=pPa->PaNext)
	    {
	      if (!pPa->PaPrevious || pPa->PaNewSubpath)
		n++;

	      n++;
	    }
	}
    }  

  return n;
}

/*----------------------------------------------------------------------
  TtaGetPathAttributeValue returns the path attribute value corresponding to
  the current set of path segments
  ---------------------------------------------------------------------- */
char *TtaGetPathAttributeValue (Element el, int width, int height)
{
  PtrElement pEl;
  PtrPathSeg          b;
  int                 length, l, add;
  char               *path;
  int nbSegments;
#define SIZE_OF_ONE_SEGMENT 50

  double w = width, h = height;

  pEl = ((PtrElement)el);

  if(pEl)
    {
      if(pEl->ElLeafType == LtPath)
        {
          nbSegments = pEl->ElVolume;
          length = nbSegments * SIZE_OF_ONE_SEGMENT;
          path = (char *)TtaGetMemory (length);
          path[0] = EOS;
          b = pEl->ElFirstPathSeg;
          l = 0;
          while (b && l + SIZE_OF_ONE_SEGMENT <= length)
            {
              if (!b->PaPrevious || b->PaNewSubpath)
                {
                  // new position
                  sprintf (&path[l], "M %d,%d ",
                           b->XStart, b->YStart);
                  add = strlen (&path[l]);
                  l += add;          
                }
              switch (b->PaShape)
                {
                case PtLine:
                  sprintf (&path[l], "L %d,%d",
                           b->XEnd, b->YEnd);
                  break;
                case PtCubicBezier:
                  sprintf (&path[l], "C %d,%d %d,%d %d,%d",
                           b->XCtrlStart, b->YCtrlStart,
                           b->XCtrlEnd, b->YCtrlEnd,
                           b->XEnd, b->YEnd);
                  break;
                case PtQuadraticBezier:
                  sprintf (&path[l], "Q %d,%d %d,%d",
                           b->XCtrlStart, b->YCtrlStart,
                           b->XEnd, b->YEnd);
                  break;
                case PtEllipticalArc:
                  sprintf (&path[l], "A %d %d %d %d %d %d,%d",
                           b->XRadius,
                           b->YRadius,
                           b->XAxisRotation,
                           b->LargeArc ? 1 : 0,
                           b->Sweep ? 1 : 0,
                           b->XEnd, b->YEnd);
                  break;
                }
              if (b)
                {
                  strcat (&path[l], " ");
                  add = strlen (&path[l]);
                  l += add;
                }
              /* next path element */
              b = b->PaNext;
            }
        }
      else if(pEl->ElLeafType == LtGraphics)
        {
          /* It's a special graphics */

          path = (char *)TtaGetMemory (4*SIZE_OF_ONE_SEGMENT);

          switch(pEl->ElGraph)
            {
            case 'L': /* diamond */
              sprintf(path, "M %g,%g L %g,%g %g,%g %g,%g z",
                      w/2, 0.,
                      w, h/2,
                      w/2, h,
                      0., h/2);
              break;

            case 2: /* Parallelogram */
              break;

            case 3: /* Trapezium */
              break;

            case 4: /* Equilateral triangle */
            case 5: /* Isosceles triangle */
              sprintf(path, "M %g,%g L %g,%g %g,%g z",
                      w/2,0.,
                      0.,h,
                      w,h
                      );
              break;

            case 6: /* Rectangled triangle */
              sprintf(path, "M %g,%g L %g,%g %g,%g z",
                      0.,0.,
                      0.,h,
                      w,0.);
              break;

            case 7: /* square */
            case 8: /* rectangle */
              sprintf(path, "M %g,%g L %g,%g %g,%g %g,%g z",
                      0., 0.,
                      w, 0.,
                      w, h,
                      0., h);
              break;
	      
            default:
              break;
            }
        }
    }
  return path;
}

/*----------------------------------------------------------------------
  TtaGetPointsAttributeValue returns the points attribute value corresponding to
  the current set of points
  ---------------------------------------------------------------------- */
char *TtaGetPointsAttributeValue (Element el, int width, int height)
{
  PtrElement pEl;
  PtrTextBuffer       pBuffer;
  char *points = NULL;
  int i, length, l, add, nbPoints;
#define SIZE_OF_ONE_POINT 20

  double w = width, h = height;

  pEl = ((PtrElement)el);
  if(pEl)
    {
      if(pEl->ElLeafType == LtPolyLine)
        {
          /* It's a polyline, generate the list of coordinates */
          pBuffer = pEl->ElPolyLineBuffer;
	  
          nbPoints = pEl->ElNPoints;
          length = nbPoints * SIZE_OF_ONE_POINT;
          points = (char *)TtaGetMemory (length);
          points[0] = EOS;
          i = 1;
          l = 0;
	  
          while(pBuffer && l + SIZE_OF_ONE_POINT <= length)
            {
              sprintf (&points[l], "%d,%d ",
                       pBuffer->BuPoints[i].XCoord,
                       pBuffer->BuPoints[i].YCoord);
              add = strlen (&points[l]);
              l += add;
	      
              i++;
              if (i == pBuffer->BuLength)
                {
                  pBuffer = pBuffer->BuNext;
                  i = 0;
                }
            }
        }
      else if(pEl->ElLeafType == LtGraphics)
        {
          /* It's a special graphics */

          points = (char *)TtaGetMemory (4*SIZE_OF_ONE_POINT);

          switch(pEl->ElGraph)
            {
            case 'L': /* diamond */
              sprintf(points, "%g,%g %g,%g %g,%g %g,%g",
                      w/2, 0.,
                      w, h/2,
                      w/2, h,
                      0., h/2);
              break;

            case 2: /* Parallelogram */
              break;

            case 3: /* Trapezium */
              break;

            case 4: /* Equilateral triangle */
            case 5: /* Isosceles triangle */
              sprintf(points, "%g,%g %g,%g %g,%g",
                      w/2,0.,
                      0.,h,
                      w,h
                      );
              break;

            case 6: /* Rectangled triangle */
              sprintf(points, "%g,%g %g,%g %g,%g",
                      0.,0.,
                      0.,h,
                      w,0.);
              break;

            case 7: /* square */
            case 8: /* rectangle */
              sprintf(points, "%g,%g %g,%g %g,%g %g,%g",
                      0., 0.,
                      w, 0.,
                      w, h,
                      0., h);
              break;
	      
            default:
              break;
            }

        }
    }

  return points;
}

/*----------------------------------------------------------------------
  TtaTransformCurveIntoPath

  Transform a polyline element into a path and return the SVG
  representation of that path.
  Parameters:
  el: the element to be transformed
  Return value:
  the SVG path expression
  ---------------------------------------------------------------------- */
char *TtaTransformCurveIntoPath (Element el)
{
  char           *path = NULL;
#ifndef NODISPLAY
  C_points       *ctrlPoints;
  PtrBox          pBox;
  PtrAbstractBox  pAb;
  PtrElement      pEl;
  PtrTextBuffer   adBuff, adNextBuff;
  int             nbPoints, i, j, len;
  ThotBool        closed;

  pEl = (PtrElement) el;
  if (pEl->ElLeafType != LtPolyLine)
    return (NULL);
  if (pEl->ElPolyLineType != 'B' &&        /* open Bezier */
      pEl->ElPolyLineType != 's')          /* closed Bezier */
    return (NULL);
  adBuff = pEl->ElPolyLineBuffer;
  nbPoints = pEl->ElNPoints;
  closed = (pEl->ElPolyLineType == 's');
  pAb = pEl->ElAbstractBox[0];
  pBox = pAb->AbBox;

  if (pBox->BxPictInfo)
    {
      ctrlPoints = (C_points *) pBox->BxPictInfo;
      path = (char *)TtaGetMemory (nbPoints * 40);
      sprintf (path, "M %d,%d", adBuff->BuPoints[1].XCoord,
               adBuff->BuPoints[1].YCoord);
      len = strlen (path);
      j = 1;
      for (i = 1; i < nbPoints - 1; i++)
        {
          j++;
          if (j >= adBuff->BuLength &&  adBuff->BuNext != NULL)
            {
              adBuff = adBuff->BuNext;
              j = 0;
            }
          sprintf (&path[len], " C %d,%d %d,%d %d,%d",
                   (int) ctrlPoints[i].rx, (int) ctrlPoints[i].ry,
                   (int) ctrlPoints[i+1].lx, (int) ctrlPoints[i+1].ly,
                   adBuff->BuPoints[j].XCoord, adBuff->BuPoints[j].YCoord);
          len = strlen (path);
        }
      if (closed)
        {
          adBuff = pEl->ElPolyLineBuffer;
          sprintf (&path[len], " C %d,%d %d,%d %d,%d",
                   (int) ctrlPoints[i].rx, (int) ctrlPoints[i].ry,
                   (int) ctrlPoints[1].lx, (int) ctrlPoints[1].ly,
                   adBuff->BuPoints[1].XCoord, adBuff->BuPoints[1].YCoord);
          len = strlen (path);
          sprintf (&path[len], " Z");
        }
    }

  adBuff = pEl->ElPolyLineBuffer;
  while (adBuff)
    {
      adNextBuff = adBuff->BuNext;
      free (adBuff);
      adBuff = adNextBuff;
    }

  if (pBox->BxPictInfo != NULL)
    free (pBox->BxPictInfo);

  pEl->ElLeafType = LtPath;
  pEl->ElFirstPathSeg = NULL;

  pAb->AbLeafType = LtPath;
  pAb->AbFirstPathSeg = NULL;

  pBox->BxFirstPathSeg = NULL;

#endif
  return (path);
}

/*----------------------------------------------------------------------
  TtaNewPathSegLine

  Creates a new path segment of type line.
  Parameters:
  xstart: absolute X coordinate for the start point of the path segment
  ystart: absolute X coordinate for the start point of the path segment
  xend:   absolute Y coordinate for the end point of the path segment
  yend:   absolute Y coordinate for the end point of the path segment
  newSubpath: this segment starts a new subpath
  Return value:
  the created path segment.
  ---------------------------------------------------------------------- */
PathSegment  TtaNewPathSegLine (int xstart, int ystart, int xend, int yend,
                                ThotBool newSubpath)
{
  PtrPathSeg       pPa;

  UserErrorCode = 0;
  GetPathSeg (&pPa);
  pPa->PaShape = PtLine;
  pPa->PaNewSubpath = newSubpath;
  pPa->XStart = xstart;
  pPa->YStart = ystart;
  pPa->XEnd = xend;
  pPa->YEnd = yend;
  return ((PathSegment) pPa);
}

/*----------------------------------------------------------------------
  TtaNewPathSegCubic

  Creates a new path segment of type cubic Bezier curve.
  Parameters:
  xstart: absolute X coordinate for the start point of the path segment
  ystart: absolute X coordinate for the start point of the path segment
  xend:   absolute Y coordinate for the end point of the path segment
  yend:   absolute Y coordinate for the end point of the path segment
  xctrl1: absolute X coordinate for the first control point
  yctrl1: absolute Y coordinate for the first control point
  xctrl2: absolute X coordinate for the second control point
  yctrl2: absolute Y coordinate for the second control point
  newSubpath: this segment starts a new subpath
  Return value:
  the created path segment.
  ---------------------------------------------------------------------- */
PathSegment TtaNewPathSegCubic (int xstart, int ystart, int xend, int yend,
                                int xctrl1, int yctrl1, int xctrl2, int yctrl2,
                                ThotBool newSubpath)
{
  PtrPathSeg       pPa;

  UserErrorCode = 0;
  GetPathSeg (&pPa);
  pPa->PaShape = PtCubicBezier;
  pPa->PaNewSubpath = newSubpath;
  pPa->XStart = xstart;
  pPa->YStart = ystart;
  pPa->XEnd = xend;
  pPa->YEnd = yend;
  pPa->XCtrlStart = xctrl1;
  pPa->YCtrlStart = yctrl1;
  pPa->XCtrlEnd = xctrl2;
  pPa->YCtrlEnd = yctrl2;
  return ((PathSegment) pPa);
}

/*----------------------------------------------------------------------
  TtaNewPathSegQuadratic

  Creates a new path segment of type quadratic Bezier curve.
  Parameters:
  xstart: absolute X coordinate for the start point of the path segment
  ystart: absolute X coordinate for the start point of the path segment
  xend:   absolute Y coordinate for the end point of the path segment
  yend:   absolute Y coordinate for the end point of the path segment
  xctrl:  absolute X coordinate for the control point
  yctrl:  absolute Y coordinate for the control point
  newSubpath: this segment starts a new subpath
  Return value:
  the created path segment.
  ---------------------------------------------------------------------- */
PathSegment TtaNewPathSegQuadratic (int xstart, int ystart, int xend, int yend,
                                    int xctrl, int yctrl, ThotBool newSubpath)
{
  PtrPathSeg       pPa;

  UserErrorCode = 0;
  GetPathSeg (&pPa);
  pPa->PaShape = PtQuadraticBezier;
  pPa->PaNewSubpath = newSubpath;
  pPa->XStart = xstart;
  pPa->YStart = ystart;
  pPa->XEnd = xend;
  pPa->YEnd = yend;
  pPa->XCtrlStart = xctrl;
  pPa->YCtrlStart = yctrl;
  pPa->XCtrlEnd = xctrl;
  pPa->YCtrlEnd = yctrl;
  return ((PathSegment) pPa);
}

/*----------------------------------------------------------------------
  TtaNewPathSegArc

  Creates a new path segment of type elliptical arc.
  Parameters:
  xstart:  absolute X coordinate for the start point of the path segment
  ystart:  absolute X coordinate for the start point of the path segment
  xend:    absolute Y coordinate for the end point of the path segment
  yend:    absolute Y coordinate for the end point of the path segment
  xradius: x-axis radius for the ellipse
  yradius: y-axis radius for the ellipse
  angle:   rotation angle in degrees for the ellipse's x-axis relative to
  the x-axis
  largearc:value for the large-arc-flag parameter
  sweep:   value for the sweep-flag parameter
  newSubpath: this segment starts a new subpath
  Return value:
  the created path segment.
  ---------------------------------------------------------------------- */
PathSegment TtaNewPathSegArc (int xstart, int ystart, int xend, int yend,
                              int xradius, int yradius, int angle,
                              ThotBool largearc, ThotBool sweep,
                              ThotBool newSubpath)
{
  PtrPathSeg       pPa;

  UserErrorCode = 0;
  GetPathSeg (&pPa);
  pPa->PaShape = PtEllipticalArc;
  pPa->PaNewSubpath = newSubpath;
  pPa->XStart = xstart;
  pPa->YStart = ystart;
  pPa->XEnd = xend;
  pPa->YEnd = yend;
  pPa->XRadius = xradius;
  pPa->YRadius = yradius;
  pPa->XAxisRotation = angle;
  pPa->LargeArc = largearc;
  pPa->Sweep = sweep;
  return ((PathSegment) pPa);
}

/*----------------------------------------------------------------------
  TtaQuadraticToCubicPathSeg
  ---------------------------------------------------------------------- */
void TtaQuadraticToCubicPathSeg (void *quadratic_segment)
{
  PtrPathSeg       pPa = (PtrPathSeg)quadratic_segment;

  int x0, y0, x1, y1, x2, y2;
  x0 = pPa->XStart;
  y0 = pPa->YStart;
  x1 = pPa->XCtrlStart;
  y1 = pPa->YCtrlStart;
  x2 = pPa->XEnd;
  y2 = pPa->YEnd;

  pPa->PaShape = PtCubicBezier;
  pPa->XCtrlStart = (x0 + 2*x1)/3;
  pPa->YCtrlStart = (y0 + 2*y1)/3;
  pPa->XCtrlEnd = (2*x1 + 1*x2)/3;
  pPa->YCtrlEnd = (2*y1 + 1*y2)/3;
}

/*----------------------------------------------------------------------
  TtaEndPointToCenterParam
  ---------------------------------------------------------------------- */
ThotBool TtaEndPointToCenterParam(int x1, int y1, int x2, int y2,
				  double *rx, double *ry,
				  double *phi,
				  ThotBool largeArc, ThotBool sweep,
				  double *cx, double *cy,
				  double *theta1, double *dtheta
				  )
{
  double g;
  double sinf, cosf, k1, k2, k3, k4, k5;
  double x1_, y1_, cx_, cy_;

  /****************** See SVG spec: Appendix F.6 ***********/
  /* Note: sometimes, argument of sqrt are negative value near zero
     (error of approximation) so take the absolute value.
  */
	  
  /* Degree to radian */
  *phi = (*phi)*M_PI/180.;

  /* Nothing or a line */
  if((x1 == x2 && y1 == y2) || (*rx == 0 || *ry == 0))
    return FALSE;

  /* Take the absolute values */
  if(*rx < 0)*rx = -*rx;
  if(*ry < 0)*ry = -*ry;

  sinf = sin(*phi);
  cosf = cos(*phi);
  k1 = ((double)(x1-x2))/2;
  k2 = ((double)(y1-y2))/2;
  x1_ = cosf*k1 + sinf*k2;
  y1_ = -sinf*k1 + cosf*k2;

  /* Check whether the arc is large enough */
  g = (x1_*x1_)/((*rx)*(*rx)) + (y1_*y1_)/((*ry)*(*ry));
  
  if(g > 1)
    {
      g = sqrt(g);
      (*rx) *= g;
      (*ry) *= g;
    }
  
  k5 = (*rx)*(*rx)*y1_*y1_+(*ry)*(*ry)*x1_*x1_;
  if(k5 == 0)return FALSE;
  k1 = sqrt(fabs(((*rx)*(*rx)*(*ry)*(*ry)
		  - (*rx)*(*rx)*y1_*y1_ - (*ry)*(*ry)*x1_*x1_)
		 /k5));
  
  if(largeArc == sweep)
    k1 = -k1;
  
  cx_ = k1*(*rx)*y1_/(*ry);
  cy_ = -k1*(*ry)*x1_/(*rx);
  
  *cx = cosf*cx_ - sinf*cx_ + ((double)(x1+x2))/2;
  *cy = sinf*cx_ + cosf*cy_ + ((double)(y1+y2))/2;
  
  k1 = (x1_ - cx_)/(*rx);
  k2 = (y1_ - cy_)/(*ry);
  k3 = (-x1_ - cx_)/(*rx);
  k4 = (-y1_ - cy_)/(*ry);
  
  k5 = sqrt(fabs(k1*k1+k2*k2));
  if(k5 == 0)return FALSE;
  k5 = k1/k5;
  if(k5 < -1)k5 = -1;
  else if(k5 > 1)k5 = 1;
  
  *theta1 = acos(k5);
  if(k2 < 0)*theta1 = - *theta1;
  
  k5 = sqrt(fabs((k1*k1+k2*k2)*(k3*k3+k4*k4)));
  if(k5 == 0)return FALSE;
  
  k5 = (k1*k3+k2*k4)/k5;
  if(k5 < -1)k5 = -1;
  else if(k5 > 1)k5 = 1;
  
  *dtheta = acos(k5);
  if(k1*k4-k3*k2 < 0)*dtheta = -*dtheta;
  
  if(!sweep && (*dtheta > 0))
    *dtheta -= 2*M_PI;
  else if(sweep && (*dtheta < 0))
    *dtheta += 2*M_PI;
  
  return TRUE;
}

/*----------------------------------------------------------------------
  TtaSplitPathSeg
  ---------------------------------------------------------------------- */
void TtaSplitPathSeg (void *segment, Document doc, Element el)
{
  PtrPathSeg       pPa = (PtrPathSeg)segment;
  PtrPathSeg       newSeg;
  PtrElement pElAsc;
  int x0, y0, x1, y1, x2, y2, x3, y3;
  double k1, k2, rx, ry, cx, cy, theta1, dtheta, phi;

  if(pPa == NULL)
    return;

  switch(pPa->PaShape)
    {
    case PtLine:
      x0 = pPa->XStart;
      y0 = pPa->YStart;
      x1 = pPa->XEnd;
      y1 = pPa->YEnd;

      newSeg = (PtrPathSeg)TtaNewPathSegLine ((x0+x1)/2,
                                              (y0+y1)/2,
                                              x1,y1,
                                              FALSE);
      break;

    case PtQuadraticBezier:
      x0 = pPa->XStart;
      y0 = pPa->YStart;
      x1 = pPa->XCtrlStart;
      y1 = pPa->YCtrlStart;
      x2 = pPa->XEnd;
      y2 = pPa->YEnd;

      newSeg = (PtrPathSeg)TtaNewPathSegQuadratic((x0+2*x1+x2)/4,
                                                  (y0+2*y1+y2)/4,
                                                  x2,y2,
                                                  (x1+x2)/2,
                                                  (y1+y2)/2,
                                                  FALSE);

      pPa->XCtrlStart = (x0+x1)/2;
      pPa->YCtrlStart = (y0+y1)/2;
      pPa->XCtrlEnd = pPa->XCtrlStart;
      pPa->YCtrlEnd = pPa->YCtrlStart;
      break;

    case PtCubicBezier:
      x0 = pPa->XStart;
      y0 = pPa->YStart;
      x1 = pPa->XCtrlStart;
      y1 = pPa->YCtrlStart;
      x2 = pPa->XCtrlEnd;
      y2 = pPa->YCtrlEnd;
      x3 = pPa->XEnd;
      y3 = pPa->YEnd;

      newSeg = (PtrPathSeg)TtaNewPathSegCubic((x0+3*x1+3*x2+x3)/8,
                                              (y0+3*y1+3*y2+y3)/8,
                                              x3,y3,
                                              (x1+2*x2+x3)/4,
                                              (y1+2*y2+y3)/4,
                                              (x2+x3)/2,
                                              (y2+y3)/2,
                                              FALSE);
      pPa->XCtrlStart = (x0+x1)/2;
      pPa->YCtrlStart = (y0+y1)/2;
      pPa->XCtrlEnd = (x0+2*x1+x2)/4;
      pPa->YCtrlEnd = (y0+2*y1+y2)/4;
      break;

      
    case PtEllipticalArc:
      x1 = pPa->XStart;
      y1 = pPa->YStart;
      x2 = pPa->XEnd;
      y2 = pPa->YEnd;
      rx = pPa->XRadius;
      ry = pPa->YRadius;
      phi = pPa->XAxisRotation;

      if(x1 == x2 && y1 == y2)
        return;

      if(rx == 0 || ry == 0)
        {
          newSeg = (PtrPathSeg)TtaNewPathSegLine ((x1+x2)/2,
                                                  (y1+y2)/2,
                                                  x2,y2,
                                                  FALSE);
        }
      else
        {
	  if(!TtaEndPointToCenterParam(x1, y1, x2, y2,
				       &rx, &ry,
				       &phi,
				       pPa->LargeArc, pPa->Sweep,
				       &cx, &cy,
				       &theta1, &dtheta
				       ))
	    return;

          /* Now we choose a new point (x3, y3) at theta = dtheta/2 */
          k1 = rx*cos(theta1+dtheta/2);
          k2 = ry*sin(theta1+dtheta/2);
          x3 = (int)((cos(phi) * k1 - sin(phi) * k2) + cx);
          y3 = (int)((sin(phi) * k1 + cos(phi) * k2) + cy);

          pPa->LargeArc = (fabs(dtheta/2) > M_PI);
          pPa->Sweep = (dtheta > 0);

          newSeg = (PtrPathSeg)TtaNewPathSegArc (x3, y3, x2, y2,
                                                 (int)rx,
                                                 (int)ry,
                                                 (int)(phi*180./M_PI),
                                                 pPa->LargeArc,
                                                 pPa->Sweep,
                                                 FALSE);
          pPa->XRadius = (int)rx;
          pPa->YRadius = (int)ry;

        }

      break;
      
    default:
      break;
    }

  /* Insert the new segment */
  pPa->XEnd = newSeg->XStart;
  pPa->YEnd = newSeg->YStart;
  newSeg->PaPrevious = pPa;
  newSeg->PaNext = pPa->PaNext;
  pPa->PaNext = newSeg;

  if(newSeg->PaNext)
    {
      /* Update the information of the successor, if it exists */
      newSeg->PaNext->PaPrevious = newSeg;
      if(!(newSeg->PaNext->PaNewSubpath))
        {
          newSeg->PaNext->XStart = newSeg->XEnd;
          newSeg->PaNext->YStart = newSeg->YEnd;
        }
    }

  /* Updates the volumes of ancestors */
  pElAsc = (PtrElement) el;
  while (pElAsc != NULL)
    {
      pElAsc->ElVolume++;
      pElAsc = pElAsc->ElParent;
    }
#ifndef NODISPLAY
  RedisplayLeaf ((PtrElement) el, doc, 1);
#endif
}


/*----------------------------------------------------------------------
  TtaAppendPathSeg

  Appends a path segment at the end of a Graphics basic element

  Parameters:
  element: the Graphics element to be modified.
  segment: the path segment to be appended.
  document: the document containing the element.
  ----------------------------------------------------------------------*/
void TtaAppendPathSeg (Element element, PathSegment segment, Document document)
{
  PtrPathSeg       pPa, pPrevPa;
  PtrElement       pElAsc;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElLeafType != LtPath &&
           ((PtrElement) element)->ElLeafType != LtGraphics)
    TtaError (ERR_invalid_element_type);
  else
    /* verifies the parameter document */
    if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
    else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
    else
      /* parameter document is correct */
      {
        if (((PtrElement) element)->ElLeafType == LtGraphics)
          {
            if (((PtrElement) element)->ElGraph == EOS)
              {
                ((PtrElement) element)->ElLeafType = LtPath;
                ((PtrElement) element)->ElFirstPathSeg = NULL;
              }
            else
              {
                TtaError (ERR_invalid_element_type);
                element = NULL;
              }
          }
        if (element && segment)
          {
            pPa = ((PtrElement) element)->ElFirstPathSeg;
            pPrevPa = NULL;
            while (pPa)
              {
                pPrevPa = pPa;
                pPa = pPa->PaNext;
              }
            if (pPrevPa == NULL)
              ((PtrElement) element)->ElFirstPathSeg = (PtrPathSeg) segment;
            else
              {
                pPrevPa->PaNext = (PtrPathSeg) segment;
                ((PtrPathSeg) segment)->PaPrevious = pPrevPa;
              }
            /* Updates the volumes of ancestors */
            pElAsc = (PtrElement) element;
            while (pElAsc != NULL)
              {
                pElAsc->ElVolume++;
                pElAsc = pElAsc->ElParent;
              }
#ifndef NODISPLAY
            RedisplayLeaf ((PtrElement) element, document, 1);
#endif
          }
      }
}

#ifndef NODISPLAY

/*----------------------------------------------------------------------
  TtaNewGradient
  ----------------------------------------------------------------------*/
void TtaNewGradient (ThotBool linear, Element el)
{
#ifdef _GL
  Gradient *grad;

  grad = (Gradient *)TtaGetMemory (sizeof (Gradient));
  grad->userSpace = FALSE;
  grad->gradTransform = NULL;
  grad->spreadMethod = 1;  /* spreadMethod = pad by default */
  grad->el = (PtrElement)el;
  grad->el->ElGradient = grad;
  grad->el->ElGradientDef = TRUE;
  grad->firstStop = NULL;
  if (linear)
    {
      grad->gradType = Linear;
      grad->gradX1 = 0;
      grad->gradX2 = 1;   /* default value = horizontal axis */
      grad->gradY1 = 0;
      grad->gradY2 = 0;
    }
  else
    {
      grad->gradType = Radial;
      grad->gradCx = .5;   /* default value */
      grad->gradCy = .5;   /* default value */
      grad->gradFx = .5;
      grad->gradFy = .5;
      grad->gradR = .5;    /* default value */
    }
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaNewGradientStop
  ----------------------------------------------------------------------*/
void TtaNewGradientStop (Element stop, Element gradient)
{
#ifdef _GL
  GradientStop *previous, *gstop, *newStop;
  Gradient *grad;

  if (!stop || !gradient)
    /* error */
    return;
  grad = ((PtrElement)gradient)->ElGradient;
  if (!grad)
    /* error */
    return;
  newStop = (GradientStop *)TtaGetMemory (sizeof (GradientStop));
  newStop->el = (PtrElement)stop;
  newStop->next = NULL;
  newStop->r = 0;
  newStop->g = 0;
  newStop->b = 0;
  newStop->a = 255;

  previous = NULL;
  gstop = grad->firstStop;
  while (gstop)
    {
      previous = gstop;
      gstop = gstop->next;
    }
  if (previous)
    previous->next = newStop;
  else
    grad->firstStop = newStop;
#endif /* _GL */
}

#ifdef _GL
/*----------------------------------------------------------------------
  GetGradientStop
  ----------------------------------------------------------------------*/
static GradientStop* GetGradientStop (PtrElement stop, PtrElement gradient)
{
  GradientStop *gstop;
  Gradient     *grad;

  if (stop == NULL || gradient == NULL || gradient->ElGradient == NULL)
    /* error */
    return NULL;
  grad = gradient->ElGradient;
  gstop = grad->firstStop;
  while (gstop)
    {
      if (gstop->el == stop)
        return gstop;
      else
	gstop = gstop->next;
    }
  return NULL;
}
#endif /* _GL */

/*----------------------------------------------------------------------
  TtaSetGradientUnits
  ----------------------------------------------------------------------*/
void TtaSetGradientUnits (ThotBool value, Element el)
{
#ifdef _GL
  Gradient *grad;

  if (el == NULL)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad)
    grad->userSpace = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaAppendGradientTransform
  ----------------------------------------------------------------------*/
void TtaAppendGradientTransform (Element el, void *transform)
{
#ifdef _GL
  Gradient *grad;
  PtrTransform       pPa, pPrevPa;

  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && transform)
    {
      pPa = grad->gradTransform;
      pPrevPa = NULL;
      while (pPa)
	{
	  pPrevPa = pPa;
	  pPa = pPa->Next;
	}
      if (pPrevPa == NULL)
	grad->gradTransform = (PtrTransform) transform;
      else
	pPrevPa->Next = (PtrTransform) transform;
    }
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetGradientSpreadMethod
  value = 1: pad
  value = 2: reflect
  value = 3: repeat
  ----------------------------------------------------------------------*/
void TtaSetGradientSpreadMethod (int value, Element el)
{
#ifdef _GL
  Gradient *grad;
  
  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad)
    grad->spreadMethod = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetLinearGradientx1
  ----------------------------------------------------------------------*/
void TtaSetLinearGradientx1 (float value, Element el)
{
#ifdef _GL
  Gradient *grad;

  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && grad->gradType == Linear)
    grad->gradX1 = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetLinearGradienty1
  ----------------------------------------------------------------------*/
void TtaSetLinearGradienty1 (float value, Element el)
{
#ifdef _GL
  Gradient *grad;
  
  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && grad->gradType == Linear)
    grad->gradY1 = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetLinearGradientx2
  ----------------------------------------------------------------------*/
void TtaSetLinearGradientx2 (float value, Element el)
{
#ifdef _GL
  Gradient *grad;

  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && grad->gradType == Linear)
    grad->gradX2 = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetLinearGradienty2
  ----------------------------------------------------------------------*/
void TtaSetLinearGradienty2 (float value, Element el)
{
#ifdef _GL
  Gradient *grad;
  
  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && grad->gradType == Linear)
    grad->gradY2 = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetRadialGradientRadius
  ----------------------------------------------------------------------*/
void TtaSetRadialGradientRadius (float value, Element el)
{
#ifdef _GL
  Gradient *grad;

  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && grad->gradType == Radial)
    grad->gradR = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetRadialGradientcx
  ----------------------------------------------------------------------*/
void TtaSetRadialGradientcx (float value, Element el)
{
#ifdef _GL
  Gradient *grad;

  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && grad->gradType == Radial)
    grad->gradCx = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetRadialGradientcy
  ----------------------------------------------------------------------*/
void TtaSetRadialGradientcy (float value, Element el)
{
#ifdef _GL
  Gradient *grad;

  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && grad->gradType == Radial)
    grad->gradCy = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetRadialGradientfx
  ----------------------------------------------------------------------*/
void TtaSetRadialGradientfx (float value, Element el)
{
#ifdef _GL
  Gradient *grad;

  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && grad->gradType == Radial)
    grad->gradFx = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetRadialGradientfy
  ----------------------------------------------------------------------*/
void TtaSetRadialGradientfy (float value, Element el)
{
#ifdef _GL
  Gradient *grad;
  
  if (!el)
    return;
  grad = ((PtrElement)el)->ElGradient;
  if (grad && grad->gradType == Radial)
    grad->gradFy = value;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetGradientStopOffset
  ----------------------------------------------------------------------*/
void TtaSetGradientStopOffset (float offset, Element el)
{
#ifdef _GL
  GradientStop *gstop;
  
  if (!el)
    return;
  gstop = GetGradientStop ((PtrElement)el, (PtrElement)(TtaGetParent (el)));
  if (gstop)
    gstop->offset = offset;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetGradientStopColor
  ----------------------------------------------------------------------*/
void TtaSetGradientStopColor (unsigned short red, unsigned short green,
                              unsigned short blue, Element el)
{
#ifdef _GL
  GradientStop *gstop;
  
  if (!el)
    return;
  gstop = GetGradientStop ((PtrElement)el, (PtrElement) (TtaGetParent(el)));
  if (gstop)
    {
      gstop->r = red;
      gstop->g = green;
      gstop->b = blue;
    }
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaSetGradientStopOpacity
  ----------------------------------------------------------------------*/
void TtaSetGradientStopOpacity (float opacity, Element el)
{
#ifdef _GL
  GradientStop *gstop;
  
  if (!el)
    return;
  gstop = GetGradientStop ((PtrElement)el, (PtrElement) (TtaGetParent(el)));
  if (gstop)
    gstop->a = (unsigned short) (opacity * 255);
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaCopyGradientUse
  If the parent of element el refers to a gradient, move this reference
  to element el itself.
  ----------------------------------------------------------------------*/
void TtaCopyGradientUse (Element el)
{
#ifdef _GL
  if (el == NULL || ((PtrElement)el)->ElParent == NULL)
    return;
  if (((PtrElement)el)->ElParent->ElGradient &&
      !((PtrElement)el)->ElParent->ElGradientDef)
    {
      ((PtrElement)el)->ElGradient = ((PtrElement)el)->ElParent->ElGradient;
      ((PtrElement)el)->ElGradientDef = FALSE;
      ((PtrElement)el)->ElParent->ElGradient = NULL;
    }
#endif /* _GL */
}

/* ----------------------------------------------------------------------
   getPathSegment
   ---------------------------------------------------------------------- */
static int getPathSegment (PtrPathSeg *pPa_, int pointselect,
                           ThotBool before)
{
  PtrPathSeg  pPa, pPaStart;
  int i = 1, i_start;

  pPa = *pPa_;
  *pPa_ = NULL;
  if(pointselect == 0)
    return 0;

  while (pPa)
    {
      if ((pPa->PaNewSubpath || !pPa->PaPrevious))
        {
          /* this path segment starts a new subpath */
          i_start = i;
          pPaStart = pPa;
	  
          if(pointselect == i || /* Current point selected */
	     
             (pointselect == i+1 && /* Next control point selected*/
              (pPa->PaShape == PtCubicBezier ||
               pPa->PaShape == PtQuadraticBezier))
             )
            {
              /* draw the start point of this path segment */
              if(before)
                {
                  /* check whether the subpath is closed */
                  i++;

                  while(pPa->PaNext && !(pPa->PaNext->PaNewSubpath))
                    {
                      if(pPa->PaShape == PtCubicBezier ||
                         pPa->PaShape == PtQuadraticBezier)
                        {
                          /* Skip Bezier handles */
                          i+=2;
                        }

                      pPa = pPa->PaNext;
                      i++;
                    }

                  if(pPa->PaShape == PtCubicBezier ||
                     pPa->PaShape == PtQuadraticBezier)
                    {
                      /* Skip Bezier handles */
                      i+=2;
                    }
		  
                  if(pPaStart->XStart == pPa->XEnd &&
                     pPaStart->YStart == pPa->YEnd)
                    {
                      *pPa_ = pPa;
                      return i;
                    }
                  else
                    {
                      *pPa_ = NULL;
                      return 0;
                    }
                }
              else
                {
                  *pPa_ = pPa;
                  return i;
                }
            }
          i++;
        }

      if(pPa->PaShape == PtCubicBezier ||
         pPa->PaShape == PtQuadraticBezier)
        {
          /* Skip Bezier handles */
          i+=2;
        }

      if(pointselect == i || /* Current point selected */

         (pointselect == i-1 && /* Previous control point selected*/
          (pPa->PaShape == PtCubicBezier ||
           pPa->PaShape == PtQuadraticBezier)) ||

         (pointselect == i+1 && /* Next control point selected */
          pPa->PaNext && !(pPa->PaNext->PaNewSubpath)
          && (pPa->PaNext->PaShape == PtCubicBezier ||
              pPa->PaNext->PaShape == PtQuadraticBezier)  )
         )
        {
          /* Draw the end point of the path segment */
          if(before)
            {
              *pPa_ = pPa;
              return i;
            }
          else 
            {
              if(pPa->PaNext && !(pPa->PaNext->PaNewSubpath))
                {
                  *pPa_ = pPa->PaNext;
                  return (i+1);
                }
              else
                {
                  /* check whether the subpath is closed */
                  if(pPaStart->XStart == pPa->XEnd &&
                     pPaStart->YStart == pPa->YEnd)
                    {
                      *pPa_ = pPaStart;
                      return i_start;
                    }
                  else
                    {
                      *pPa_ = NULL;
                      return 0;
                    }
                }
	    
            }

        }
	
      pPa = pPa->PaNext;
      i++;
    }
  return 0;
}


/* ----------------------------------------------------------------------
   TtaDeletePointInCurve
   ---------------------------------------------------------------------- */
ThotBool TtaDeletePointInCurve (Document doc, Element el,
                                int point_number)
{
  PtrPathSeg pPa, pPaPrevious = NULL, pPaNext = NULL;
  PtrElement pElAsc, pEl = (PtrElement) el;

  if(pEl == NULL)
    return FALSE;

  if(pEl->ElLeafType == LtPolyLine)
    {
      TtaDeletePointInPolyline (el, point_number, doc);
      return TRUE;
    }
  else if(pEl->ElLeafType == LtPath)
    {
      pPa = pEl->ElFirstPathSeg;
      getPathSegment(&pPa, point_number, TRUE);
      if(pPa)
        {
          /* we want to delete the first segment */
          if(pPa == pEl->ElFirstPathSeg)
            pEl->ElFirstPathSeg = pPa->PaNext;

          /* Remove the references to pPa */
          if(pPa->PaPrevious)
            pPa->PaPrevious->PaNext = pPa->PaNext;

          if(pPa->PaNext)
            pPa->PaNext->PaPrevious = pPa->PaPrevious;

          /* Check if the previous/next segment are in the same subpath */
          if(pPa->PaPrevious && !(pPa->PaNewSubpath))
            pPaPrevious = pPa->PaPrevious;

          if(pPa->PaNext && !(pPa->PaNext->PaNewSubpath))
            pPaNext = pPa->PaNext;

          /* Update points and handles */
          if(pPaNext)
            {
              if(pPaPrevious)
                {
                  pPaNext->XStart = pPaPrevious->XEnd;
                  pPaNext->YStart = pPaPrevious->YEnd;

                  if((pPaNext->PaShape == PtCubicBezier ||
                      pPaNext->PaShape == PtQuadraticBezier) &&
                     (pPa->PaShape == PtCubicBezier || 
                      pPa->PaShape == PtQuadraticBezier))
                    {
                      pPaNext->XCtrlStart = pPaNext->XStart +
                        (pPa->XCtrlStart - pPa->XStart);
                      pPaNext->YCtrlStart = pPaNext->YStart +
                        (pPa->YCtrlStart - pPa->YStart);

                      if(pPaNext->PaShape == PtQuadraticBezier)
                        {
                          pPaNext->XCtrlEnd = pPaNext->XCtrlStart;
                          pPaNext->YCtrlEnd = pPaNext->YCtrlStart;
                        }
                    }
	    
                }
              else
                pPaNext->PaNewSubpath = TRUE;
            }

          /* Remove the segment containing the point */
          FreePathSeg (pPa);

          /* Updates the volumes of ancestors */
          pElAsc = (PtrElement) el;
          while (pElAsc != NULL)
            {
              pElAsc->ElVolume--;
              pElAsc = pElAsc->ElParent;
            }
#ifndef NODISPLAY
          RedisplayLeaf ((PtrElement) el, doc, 1);
#endif
	  
          return TRUE;
        }

    }
  return FALSE;
}

/* ----------------------------------------------------------------------
   TtaInsertPointInCurve
   ---------------------------------------------------------------------- */
ThotBool TtaInsertPointInCurve (Document doc, Element el,
                                ThotBool before, int *point_number)
{
  PtrPathSeg pPa;
  int        p = *point_number;
  
  if (((PtrElement) el)->ElLeafType == LtPolyLine)
    {
      if (((PtrElement) el)->ElPolyLineType == 'g')
        // never add a point to a line;
        return FALSE;
      if (!before)
        p++;
      *point_number = TtaAddPointInPolyline (el, p, UnPixel, 1, 1, doc, TRUE);
      return TRUE;
    }
  else if (((PtrElement) el)->ElLeafType == LtPath)
    {
      pPa = ((PtrElement)el)->ElFirstPathSeg;
      p = getPathSegment(&pPa, p, before);
      if(pPa)
        {
          /* Add a new path segment */
          TtaSplitPathSeg ((void *)pPa, doc, el);

          /* Update the selected point */
          if(!before)
            {
              if(pPa->PaShape == PtQuadraticBezier ||
                 pPa->PaShape == PtCubicBezier)
                /* Skip Bezier handles */
                p+=2;
            }
	  
          *point_number = p;
	  
          return TRUE;
        }
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  TtaAppendTransform

  Appends a Transform at the end of a Graphics basic element
  Parameters:
  element: the Graphics element to be modified.
  transform: the transformation info to be appended.
  document: the document containing the element.
  ----------------------------------------------------------------------*/
void TtaAppendTransform (Element element, void *transform)
{
  PtrTransform       pPa, pPrevPa;
     
  UserErrorCode = 0;
  if (element == NULL || transform == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      pPa = ((PtrElement) element)->ElTransform;
      pPrevPa = NULL;
      while (pPa)
	{
	  pPrevPa = pPa;
	  pPa = pPa->Next;
	}
      if (pPrevPa == NULL)
	((PtrElement) element)->ElTransform = (PtrTransform) transform;
      else
	pPrevPa->Next = (PtrTransform) transform;
    }
}

/*----------------------------------------------------------------------
  TransformAddition
  Add A transformation to one already existing
  Parameters:
  Trans1: the Graphics element to be modified.
  Trans2: the new transform .
  ----------------------------------------------------------------------*/
static void TransformAddition (PtrTransform Trans1, PtrTransform Trans2)
{
  switch (Trans2->TransType)
    {
    case PtElBoxTranslate:
    case PtElScale:
    case PtElTranslate:
      Trans1->XScale += Trans2->XScale;
      Trans1->YScale += Trans2->YScale;
      break;
    case PtElRotate:
      Trans1->TrAngle += Trans2->TrAngle;	  
      Trans1->XRotate += Trans2->XRotate;	  
      Trans1->YRotate += Trans2->YRotate;	  
      break;
    case PtElMatrix:
      Trans1->AMatrix += Trans2->AMatrix;
      Trans1->BMatrix += Trans2->BMatrix;
      Trans1->CMatrix += Trans2->CMatrix;
      Trans1->DMatrix += Trans2->DMatrix;
      Trans1->EMatrix += Trans2->EMatrix;
      Trans1->FMatrix += Trans2->FMatrix;
      break;
    case PtElSkewX:
    case PtElSkewY: 
      Trans1->TrFactor = Trans2->TrFactor;	  
      break;
    case PtElViewBox:
      Trans1->VbXTranslate += Trans2->VbXTranslate;
      Trans1->VbYTranslate += Trans2->VbYTranslate;
      Trans1->VbWidth += Trans2->VbWidth;
      Trans1->VbHeight += Trans2->VbHeight;
      break;
    default:
      break;
    }
}
/*----------------------------------------------------------------------
  TtaAddTransform

  Insert or Add a Transform at the beginning of a Graphics basic element
  if a transformation of the same type exists in the list, it is replaced
  with old_value + delta

  Parameters:
  element: the Graphics element to be modified.
  transform: the transformation to be inserted.
  document: the document containing the element.
  ----------------------------------------------------------------------*/
void TtaAddTransform (Element element, void *transform, 
                      Document document)
{
  PtrTransform       pPa, pPrevPa;
     
  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* verifies the parameter document */
    if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
    else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
    else
      /* parameter document is correct */
      {
        if (element && transform)
          {
            pPa = ((PtrElement) element)->ElTransform;
            pPrevPa = NULL;
            while (pPa)
              {
                if (pPa->TransType == ((PtrTransform)transform)->TransType)
                  {
                    TransformAddition (pPa, (Transform*)transform);
                    TtaFreeMemory (transform);
                    return;
                  }	       
                pPrevPa = pPa;
                pPa = pPa->Next;
              }

            if (pPrevPa == NULL)
              ((PtrElement) element)->ElTransform = (PtrTransform) transform;
            else
              pPrevPa->Next = (PtrTransform) transform;

          }
      }
}

/*----------------------------------------------------------------------
  TtaInsertTransform

  Insert a Transform at the beginning of a Graphics basic element
  Parameters:
  element: the Graphics element to be modified.
  transform: the Transformation to be inserted.
  document: the document containing the element.
  ----------------------------------------------------------------------*/
void TtaInsertTransform (Element element, void *transform, 
                         Document document)
{
  PtrTransform       pPa;
     
  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* verifies the parameter document */
    if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
    else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
    else
      /* parameter document is correct */
      {
        if (element && transform)
          {
            pPa = ((PtrElement) element)->ElTransform;
            ((PtrTransform) transform)->Next = pPa;
            ((PtrElement) element)->ElTransform = (PtrTransform) transform;
          }
      }
}

/*----------------------------------------------------------------------
  TtaRemoveTransform

  Remove the Transform attached to a Graphics element
  ----------------------------------------------------------------------*/
void TtaRemoveTransform (Document document, Element element)
{
  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* verifies the parameter document */
    if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
    else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
    else
      {
        /* parameter document is correct */
        TtaFreeTransform(((PtrElement)element)->ElTransform);
        ((PtrElement)element)->ElTransform = NULL;
      }
}

/*----------------------------------------------------------------------
  SimplifyTransformMatrix
  Return a new transform of type PtElMatrix which represents the
  composition of all the elements of the list "transform".
  ---------------------------------------------------------------------- */
static PtrTransform SimplifyTransformMatrix (Element el, PtrTransform pPa)
{
  PtrTransform result;
  PtrElement   pEl = (PtrElement) el;
  float        xtrans,  ytrans;
  double       xscale, yscale;
  double       T, cosT,sinT, tanT, a, b, c, d, e, f;
  ThotBool     istranslated, isscaled;

  if ((pEl == NULL || pEl->ElTransform == NULL) && pPa == NULL)
    /* Nothing to do */
    return NULL;

  if (pPa == NULL)
    pPa = pEl->ElTransform;
  if (pPa->TransType == PtElMatrix && pPa->Next == NULL)
    /* The matrix is already simplified, return a copy */
    return (PtrTransform) TtaCopyTransform (pPa);

  /* result = Identity */
  result = (PtrTransform) TtaNewTransformMatrix (1, 0, 0, 1, 0, 0);
  result->Next = NULL;
  while (pPa)
    {
      switch (pPa->TransType)
        {
        case PtElTranslate:
        case PtElAnimTranslate:
          result->EMatrix += result->AMatrix*pPa->XScale +
            result->CMatrix*pPa->YScale;
          result->FMatrix += result->BMatrix*pPa->XScale +
            result->DMatrix*pPa->YScale;
          break;
        case PtElScale:
          result->AMatrix *= pPa->XScale;
          result->CMatrix *= pPa->YScale;
          result->BMatrix *= pPa->XScale;
          result->DMatrix *= pPa->YScale;
          break;
        case PtElViewBox:
          /* TODO */
          GetViewBoxTransformation (pEl->ElTransform,
                                    pEl->ElAbstractBox[0]->AbBox->BxW,
                                    pEl->ElAbstractBox[0]->AbBox->BxH,
                                    &xtrans, &ytrans,
                                    &xscale, &yscale,
                                    &istranslated, &isscaled);
          if (istranslated || isscaled)
            {
              //printf ("xtrans=%f ytrans=%f xscale=%f yscale=%f\n",xtrans,  ytrans, xscale, yscale);
              result->EMatrix = result->AMatrix * xtrans +
              result->CMatrix * ytrans;
              result->FMatrix = result->BMatrix * xtrans +
                result->DMatrix * ytrans;
              result->AMatrix *= xscale;
              result->CMatrix *= yscale;
              result->BMatrix *= xscale;
              result->DMatrix *= yscale;
            }
          break;
        case PtElRotate:
        case PtElAnimRotate:
          /* tranlate(XRotate,YRotate) */
          result->EMatrix += result->AMatrix*pPa->XRotate +
            result->CMatrix*pPa->YRotate;
          result->FMatrix += result->BMatrix*pPa->XRotate +
            result->DMatrix*pPa->YRotate;

          /* rotate(TrAngle,0,0) */
          T = (pPa->TrAngle * M_PI / 180);
          cosT = cos(T);
          sinT = sin(T);

          a = result->AMatrix;
          b = result->BMatrix;
          c = result->CMatrix;
          d = result->DMatrix;
          e = result->EMatrix;
          f = result->FMatrix;
          result->AMatrix = (float)(a*cosT + c*sinT);
          result->CMatrix = (float)(-a*sinT + c*cosT);
          result->BMatrix = (float)(b*cosT + d*sinT);
          result->DMatrix = (float)(-b*sinT + d*cosT);

          /* tranlate(-XRotate,-YRotate) */
          result->EMatrix -= result->AMatrix*pPa->XRotate +
            result->CMatrix*pPa->YRotate;
          result->FMatrix -= result->BMatrix*pPa->XRotate +
            result->DMatrix*pPa->YRotate;
          break;  
        case PtElMatrix:
          a = result->AMatrix;
          b = result->BMatrix;
          c = result->CMatrix;
          d = result->DMatrix;
          e = result->EMatrix;
          f = result->FMatrix;
          result->AMatrix = (float)(a*pPa->AMatrix + c*pPa->BMatrix);
          result->CMatrix = (float)(a*pPa->CMatrix + c*pPa->DMatrix);
          result->BMatrix = (float)(b*pPa->AMatrix + d*pPa->BMatrix);
          result->DMatrix = (float)(b*pPa->CMatrix + d*pPa->DMatrix);
          result->EMatrix += (float)(a*pPa->EMatrix + c*pPa->FMatrix);
          result->FMatrix += (float)(b*pPa->EMatrix + d*pPa->FMatrix);
          break;	  
        case PtElSkewX:
          T = pPa->TrAngle * M_PI / 180;
          tanT = tan(T);
          result->CMatrix += (float)(tanT*result->AMatrix);
          result->DMatrix += (float)(tanT*result->BMatrix);
          break;
        case PtElSkewY:
          T = pPa->TrAngle * M_PI / 180;
          tanT = tan(T);
          result->AMatrix += (float)(tanT*result->CMatrix);
          result->BMatrix += (float)(tanT*result->DMatrix);
          break;	  
        default:
          break;
        }
      pPa = pPa->Next;
    }

  return result;
}

/*----------------------------------------------------------------------
  TtaCoordinatesInParentSpace

  Convert the coordinates (x,y) of a point inside the space of the element
  el into coordinates in its parent space.
  ---------------------------------------------------------------------- */
void TtaCoordinatesInParentSpace(Element el, float *x, float *y)
{
  PtrTransform transform;
  float        newx, newy;

  transform = SimplifyTransformMatrix (el, NULL);
  if (transform)
    {
      newx = transform->AMatrix * *x +
        transform->CMatrix * *y +
        transform->EMatrix;

      newy = transform->BMatrix * *x +
        transform->DMatrix * *y +
        transform->FMatrix;
      *x = newx;
      *y = newy;
      TtaFreeTransform (transform);
    }
}

/*----------------------------------------------------------------------
  TtaGetCurrentTransformMatrix

  Get the CTM that allows to get coordinates of a point in the ancestor
  space from its coordinates in the element space.

  ( x_in_ancestor_space )         ( x_in_el_space )
  ( y_in_ancestor_space ) = CTM * ( y_in_el_space )
  (          1          )         (        1      )

  ---------------------------------------------------------------------- */
void *TtaGetCurrentTransformMatrix(Element el, Element ancestor)
{
  PtrTransform CTM = NULL, transform;
  PtrElement   pEl, pAncestor;
  float        x_trans, y_trans;
  double       x_scale, y_scale; 
  ThotBool     is_translated, is_scaled;

  pEl = (PtrElement) el;
  pAncestor = (PtrElement) ancestor;
  /* Concatenate all simplified transform matrix */
  while (pEl && pEl != pAncestor)
    {
      transform = SimplifyTransformMatrix(el, NULL);
      if (transform)
        {
          transform->Next = CTM;
          CTM = transform;
        }
      pEl = pEl->ElParent;
    }
  if (pEl == pAncestor && pEl->ElTransform &&
      pEl->ElTransform->TransType == PtElViewBox && pEl->ElAbstractBox[0] &&
      pEl->ElAbstractBox[0]->AbBox)
    {
      GetViewBoxTransformation (pEl->ElTransform,
                                pEl->ElAbstractBox[0]->AbBox->BxW,
                                pEl->ElAbstractBox[0]->AbBox->BxH,
                                &x_trans, &y_trans,
                                &x_scale, &y_scale,
                                &is_translated, &is_scaled);
      if (is_translated || is_scaled)
        {
          transform = ((PtrTransform)(TtaNewTransformMatrix(1, 0, 0, 1, 0, 0)));
          transform->Next = CTM;
          transform->EMatrix = transform->AMatrix * x_trans +
            transform->CMatrix * y_trans;
          transform->FMatrix = transform->BMatrix * x_trans +
            transform->DMatrix * y_trans;
          transform->AMatrix *= x_scale;
          transform->CMatrix *= y_scale;
          transform->BMatrix *= x_scale;
          transform->DMatrix *= y_scale;
          CTM = transform;
        }
    }
  /* Simplify the product */
  if (CTM)
    {
      transform = (PtrTransform) SimplifyTransformMatrix (NULL, CTM);
      TtaFreeTransform (CTM);
      CTM = transform;
    }
  return CTM;
}

/*----------------------------------------------------------------------
  TtaInverseTransform

  Return the matrix representing the inverse of a transform. If the
  transform is not inversible, then return NULL.
  ----------------------------------------------------------------------*/
void *TtaInverseTransform (void *transform)
{
  PtrTransform result, pPa;
  double a,b,c,d,e,f,a2,b2,c2,d2,e2,f2, cosA, sinA, tanA, det;
  
  result = (Transform*)TtaNewTransformMatrix (1, 0, 0, 1, 0, 0);
  pPa = (Transform*)transform;

  while (pPa)
    {      
      switch (pPa->TransType)
        {
        case PtElScale:
        case PtElTranslate:
        case PtElAnimTranslate:
          /* Check whether the scale matrix is inversible */
          if(pPa->XScale == 0 || pPa->YScale == 0)
            {
              TtaFreeTransform (result);
              return NULL;
            }

          /* Multiply by the inverse of scale(XScale, YScale) */
          result->AMatrix /= pPa->XScale;
          result->CMatrix /= pPa->XScale;
          result->EMatrix /= pPa->XScale;
          result->BMatrix /= pPa->YScale;
          result->DMatrix /= pPa->YScale;
          result->FMatrix /= pPa->YScale;
          break;
        case PtElViewBox:
          /* TODO: inverse matrix of a ViewBox ??? */
          break;
        case PtElRotate:
        case PtElAnimRotate:
          /* Multiply result by the inverse of translate(-XRotate,-YRotate) */
          result->EMatrix -= pPa->XRotate;
          result->FMatrix -= pPa->YRotate;

          /* Multiply result by the inverse of rotate(TrAngle,0,0) */
          cosA = cos(pPa->TrAngle);
          sinA = sin(pPa->TrAngle);

          a = result->AMatrix * cosA - result->BMatrix * sinA;
          c = result->CMatrix * cosA - result->DMatrix * sinA;
          e = result->EMatrix * cosA - result->FMatrix * sinA;
          b = result->AMatrix * sinA + result->BMatrix * cosA;
          d = result->CMatrix * sinA + result->DMatrix * cosA;
          f = result->EMatrix * sinA + result->FMatrix * cosA;

          result->AMatrix = (float)a;
          result->BMatrix = (float)b;
          result->CMatrix = (float)c;
          result->DMatrix = (float)d;
          result->EMatrix = (float)e;
          result->FMatrix = (float)f;

          /* Multiply result by the inverse of translate(+XRotate,+YRotate) */
          result->EMatrix += pPa->XRotate;
          result->FMatrix += pPa->YRotate;
          break;  
        case PtElMatrix:
          /* Check whether the matrix is inversible */
          det = pPa->AMatrix*pPa->DMatrix - pPa->CMatrix*pPa->BMatrix;

          if(det == 0)
            {
              TtaFreeTransform (result);
              return NULL;
            }
 
          /* Compute the inverse of matrix */
          a = pPa->DMatrix / det;
          c = -pPa->CMatrix / det;
          e = (pPa->CMatrix*pPa->FMatrix - pPa->DMatrix*pPa->EMatrix)/det;
          b = -pPa->BMatrix / det;
          d = pPa->AMatrix / det;
          f = -(pPa->AMatrix*pPa->FMatrix - pPa->BMatrix*pPa->EMatrix)/det;

          /* Multiply result by the inverse */
          a2 = a*result->AMatrix + c*result->BMatrix;
          c2 = a*result->CMatrix + c*result->DMatrix;
          e2 = a*result->EMatrix + c*result->FMatrix + e;
          b2 = b*result->AMatrix + d*result->BMatrix;
          d2 = b*result->CMatrix + d*result->DMatrix;
          f2 = b*result->EMatrix + d*result->FMatrix + f;

          result->AMatrix = (float)a2;
          result->BMatrix = (float)b2;
          result->CMatrix = (float)c2;
          result->DMatrix = (float)d2;
          result->EMatrix = (float)e2;
          result->FMatrix = (float)f2;

          break;	  
        case PtElSkewX:
          /* Multiply result by the inverse of skewX(TrFactor) */
          tanA = tan(pPa->TrFactor);
          result->AMatrix -= (float)(result->BMatrix * tanA);
          result->CMatrix -= (float)(result->DMatrix * tanA);
          result->EMatrix -= (float)(result->FMatrix * tanA);
        case PtElSkewY:
          /* Multiply result by the inverse of skewX(TrFactor) */
          tanA = tan(pPa->TrFactor);
          result->BMatrix -= (float)(result->AMatrix * tanA);
          result->DMatrix -= (float)(result->CMatrix * tanA);
          result->FMatrix -= (float)(result->EMatrix * tanA);
          break;	  
        default:
          break;
        }	       
      pPa = pPa->Next;
    }
  return (void *) result;
}

/*----------------------------------------------------------------------
  TtaApplyMatrixTransform

  Apply a transform matrix to an element and simplify its transformation
  matrix.
  ---------------------------------------------------------------------- */
void TtaApplyMatrixTransform (Document document, Element element,
                              float a, float b, float c, float d, float e,
                              float f)
{
  PtrTransform       transform;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* verifies the parameter document */
    if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
    else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
    else
      /* parameter document is correct */
      {
        /* Add a new transform */
        TtaInsertTransform (element,
                            TtaNewTransformMatrix (a, b, c, d, e, f),
                            document);

        /* Simplify the transform matrix */
        transform = SimplifyTransformMatrix (element, NULL);
        if(transform)
          {
            TtaFreeTransform (((PtrElement) element)->ElTransform);
            ((PtrElement) element)->ElTransform = transform;
          }
      }
}

/*----------------------------------------------------------------------
  TtaAppendMatrixTransform

  Apply a transform matrix to an element and simplify its transformation
  matrix.
  ---------------------------------------------------------------------- */
void TtaAppendMatrixTransform (Document document, Element element,
                               float a, float b, float c, float d, float e,
                               float f)
{
  PtrTransform       transform;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else
    /* verifies the parameter document */
    if (document < 1 || document > MAX_DOCUMENTS)
      TtaError (ERR_invalid_document_parameter);
    else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
    else
      /* parameter document is correct */
      {
        /* Add a new transform */
        TtaAppendTransform (element, TtaNewTransformMatrix (a, b, c, d, e, f));
        /* Simplify the transform matrix */
        transform = SimplifyTransformMatrix (element, NULL);

        if(transform)
          {
            TtaFreeTransform (((PtrElement) element)->ElTransform);
            ((PtrElement) element)->ElTransform = transform;
          }
      }
}

/*----------------------------------------------------------------------
  TtaDecomposeTransform

  Decompose the transform as a product of simple transforms:

  - translate(tx, ty)
  - rotate(theta, cx, cy)
  - scale(sx, sy)
  - skewX(theta)
  - skewY(theta)

  Each kind of transform is used at most once.
  ---------------------------------------------------------------------- */
void *TtaDecomposeTransform (void *transform)
{
  PtrTransform result = NULL, cursor;
  PtrTransform matrix = SimplifyTransformMatrix (NULL, (PtrTransform) transform);
  double       coeff, coeff1, coeff2, theta1, theta2;
  double       a, b, c, d, e, f, k;
  ThotBool     RemoveTranslate = FALSE, decompose;
  
  if (matrix == NULL)
    return NULL;
  if (!TtaGetEnvBoolean ("ENABLE_DECOMPOSE_TRANSFORM", &decompose))
    decompose = TRUE;
  /* Environnement variable set to false, simply return one matrix */
  if (!decompose)
    return matrix;

  a = matrix->AMatrix;
  b = matrix->BMatrix;
  c = matrix->CMatrix;
  d = matrix->DMatrix;
  e = matrix->EMatrix;
  f = matrix->FMatrix;
  TtaFreeTransform (matrix);

  /* Recall that the transform matrix is
   * 
   * (a c e)   (1 0 e)(a c 0)
   * (b d f) = (0 1 f)(b d 0)
   * (0 0 1)   (0 0 1)(0 0 1)
   *           \__ __/\__ __/
   *              v      v
   *  
   *     translate(e,f) (a c)
   * = change of origin (b d) = lineary part
   */

  /* Add the non-lineary part of the transform
     Translate(e, f)
  */
  result = (PtrTransform)TtaNewTransformTranslate((float)e, (float)f);
  cursor = result;

  if(b == 0 && c == 0)
    {
      /* I/ Diagonal matrix
       *
       * (a c)   (a 0)
       * (b d) = (0 d) = Scale(a, d)
       *
       */

      /* Add the scale if it is not the identity */
      if(!(a == 1 && d == 1))
        cursor->Next = (PtrTransform)TtaNewTransformScale((float)a, (float)d);
    }
  else if(a == 0 && d == 0)
    {
      /* II/ Anti-diagonal matrix
       *
       * (0 c)   (0 -1)(b   0)
       * (b 0) = (1  0)(0  -c) = Rotate(90°)Scale(b, -c)
       *
       * (0 c e)
       * (b 0 f)
       * (0 0 1) = Rotate(90°, [e-f]/2, [e+f]/2)Scale(b, -c)
       */

      /* Group the translate and the rotate in one rotate. */
      RemoveTranslate = TRUE;
      e/=2;
      f/=2;
      cursor->Next = (PtrTransform)TtaNewTransformRotate(90,(float)(e-f),(float)(e+f));
      cursor = cursor->Next;

      /* Add the scale if it is not the identity */
      if(!(b == 1 && -c == 1))
        cursor->Next = (PtrTransform)TtaNewTransformScale((float)b,(float)(-c));
    }
  else if(a == d && b == -c)
    {
      /* III/ A first case of product of scale and rotation
       *
       * (a -b)   (a'  -b')(coeff       )       {coeff = sqrt(a^2 + b^2)
       * (b  a) = (b'   a')(       coeff)  with {a',b' between -1 and 1
       *
       * Hence the second matrix is the one of a rotation of angle
       * theta1 = sgn(b')*acos(a').
       *
       * (a -b e)
       * (b  a f)
       * (0  0 1) = Translate(e,f)Rotate(theta)Scale(coeff, coeff)
       *          = Rotate(theta1,cx,cy)Scale(coeff, coeff)
       * 
       * k = sin(theta1)/(1-cos(theta1))
       *
       * (cx)	     (e - k*f)	  
       * (cy) = (1/2)(k*e + f)
       */
      coeff = sqrt(a*a + b*b); /* coeff > 0 since b != 0 */
      theta1 = acos(a/coeff);
      if(b < 0)theta1 = -theta1;

      /* Group the rotate with the translate if it is possible */
      if(theta1 != 0)
        {
          RemoveTranslate = TRUE;
          k = sin(theta1)/(1 - cos(theta1));
          theta1 *= (180 / M_PI);
          e/=2;
          f/=2;
          cursor->Next = (PtrTransform)TtaNewTransformRotate((float)theta1,
                                                               (float)(e - k*f),
                                                               (float)(k*e + f));
          cursor = cursor->Next;
        }

      /* Add the scale if it is not the identity */
      if(coeff != 1)
        cursor->Next = (PtrTransform)TtaNewTransformScale((float)coeff, (float)coeff);
    }
  else if(a*b + c*d == 0 && ((b!=0 && c!=0) || (a!=0 && d!=0)))
    {
      /* IV/ A second case of product of scale and rotation
       *
       * (a  c)   (-1/b     )(-ab  -bc)
       * (b  d) = (      1/c)( bc   cd)
       *
       * or
       *
       * (a  c)   (1/d     )(ad   cd) 
       * (b  d) = (     1/a)(ab   ad)
       *
       * Since cd = -ab The second matrix can be reduced as in III/
       */

      if(b != 0 && c != 0)
        {
          a = -a*b;
          d = b*c;
          coeff = sqrt(a*a + d*d);
          coeff1 = -coeff/b;
          coeff2 = coeff/c;
          theta1 = acos(a/coeff);
          if(d < 0)theta1=-theta1;
        }
      else /* a!= 0 && d != 0 */
        {
          b = a*d;
          c = c*d;
          coeff = sqrt(b*b + c*c);
          coeff1 = coeff/d;
          coeff2 = coeff/a;
          theta1 = acos(b/coeff);
          if(c < 0)theta1=-theta1;
        }

      /* case coeff1 == coeff2 == 1
         has already been treated in II/
         so the following scale is never useless */
      cursor->Next = (PtrTransform)TtaNewTransformScale((float)coeff1, (float)coeff2);
      cursor = cursor->Next;

      /* Group the rotate with the translate if it is possible */
      if(theta1 != 0)
        {
          /* Change the coefficient of the translation, because 
             translate(e,f)scale(coeff1,coeff2)
             = scale(coeff1,coeff2)translate(e/coeff1,f/coeff2)
          */
          e /= coeff1;
          f /= coeff2;

          RemoveTranslate = TRUE;
          k = sin(theta1)/(1 - cos(theta1));
          theta1 *= (180 / M_PI);
          e/=2;
          f/=2;
          cursor->Next = (PtrTransform)TtaNewTransformRotate((float)theta1,
                                                               (float)(e - k*f),
                                                               (float)(k*e + f));
        }
    }
  else if(a*c + b*d == 0 && ((b!=0 && c!=0) || (a!=0 && d!=0)))
    {
      /* V/ A third case of product of scale and rotation
       *
       * (a  c)   (ac  -bc)(1/c     ) 
       * (b  d) = (bc  -bd)(    -1/b)
       *
       * or 
       *
       * (a  c)   (ad ac)(1/d    ) 
       * (b  d) = (bd ad)(    1/a)
       *
       * Since ac = -bd, the first matrix can be reduced as in III/
       */

      if(b != 0 && c != 0)
        {
          a = a*c;
          d = b*c;
          coeff = sqrt(a*a + d*d);
          coeff1 = coeff/c;
          coeff2 = -coeff/b;
          theta1 = acos(a/coeff);
          if(d < 0)theta1=-theta1;
        }
      else /* a!= 0 && d != 0 */
        {
          b = a*d;
          c = a*c;
          coeff = sqrt(b*b + c*c);
          coeff1 = coeff/d;
          coeff2 = coeff/a;
          theta1 = acos(b/coeff);
          if(c < 0)theta1=-theta1;
        }

      /* Group the rotate with the translate if it is possible */
      if(theta1 != 0)
        {
          RemoveTranslate = TRUE;
          k = sin(theta1)/(1 - cos(theta1));
          theta1 *= (180 / M_PI);
          e/=2;
          f/=2;
          cursor->Next = (PtrTransform)TtaNewTransformRotate((float)theta1,
                                                               (float)(e - k*f),
                                                               (float)(k*e + f));
          cursor = cursor->Next;
        }


      /* case coeff1 == coeff2 == 1
         has already been treated in III/
         so the following scale is never useless */
      cursor->Next = (PtrTransform)TtaNewTransformScale((float)coeff1,(float)coeff2);
    }
  else if(a != 0)
    {
      /* VI/ A first case of product of scale and skews
       *	 
       * (a c)   (1        0)(a           )(1  tan(T2))
       * (b d) = (tan(T1)  1)(    d - bc/a)(0        1)
       *	 
       */
      theta1 = atan(b/a) * 180 / M_PI;
      theta2 = atan(c/a) * 180 / M_PI;
      d -= b*c/a;
      
      /* Add the skew if it is not the identity */
      if(theta1 != 0)
        {
          cursor->Next = (PtrTransform)TtaNewTransformSkewY((float)theta1);
          cursor = cursor->Next;
        }
      
      /* Add the scale if it is not the identity */
      if(!(a == 1 && d == 1))
        {
          cursor->Next = (PtrTransform)TtaNewTransformScale((float)a, (float)d);
          cursor = cursor->Next;
        }
      
      /* Add the skew if it is not the identity */
      if(theta2 != 0)
        cursor->Next = (PtrTransform)TtaNewTransformSkewX((float)theta2);
    }
  else if(d != 0)
    {
      /* VII/ A second case of product of scale and skews
       *
       * (a c)   (1  tan(T2))(a - bc/d    )(1        0)
       * (b d) = (0        1)(           d)(tan(T2)  1)
       * 
       */
      theta1 = atan(c/d) * 180 / M_PI;
      theta2 = atan(b/d) * 180 / M_PI;
      a -= b*c/d;
      
      /* Add the skew if it is not the identity */
      if(theta1 != 0)
        {
          cursor->Next = (PtrTransform)TtaNewTransformSkewX((float)theta1);
          cursor = cursor->Next;
        }

      /* Add the scale if it is not the identity */
      if(!(a == 1 && d == 1))
        {
          cursor->Next = (PtrTransform)TtaNewTransformScale((float)a, (float)d);
          cursor = cursor->Next;
        }

      /* Add the skew if it is not the identity */
      if(theta2 != 0)
        cursor->Next = (PtrTransform)TtaNewTransformSkewY((float)theta2);
    }
    
  if(RemoveTranslate || (e == 0 && f == 0))
    {
      /* Remove the initial translation */
      cursor = result->Next;
      result->Next = NULL;
      TtaFreeTransform(result);
      result = cursor;
    }

  return result;
}

/*----------------------------------------------------------------------
  TtaGetTransformAttributeValue

  Get the value of the transform attribute attached to the element el,
  using a reduced form (i.e. using only rotate, scale, translate and skew)

  ---------------------------------------------------------------------- */
char *TtaGetTransformAttributeValue (Document document, Element el)
{
  char buffer[500], buffer2[100], *result;

  ThotBool add;
  PtrTransform pPa;
  PtrTransform transform;
  float a,b,c,d,e,f;

  if(!el)return NULL;
  transform = (PtrTransform)TtaDecomposeTransform(((PtrElement) el)->ElTransform);

  *buffer = '\0';

  if(transform == NULL)
    /* No transformation */
    return NULL;

  pPa = transform;

  while(pPa)
    {
      add = FALSE;

      switch(pPa->TransType)
        {
        case PtElTranslate:
          if(!(pPa->XScale == 0 && pPa->YScale == 0))
            {
              if(pPa->YScale == 0)
                sprintf(buffer2, "translate(%g) ", pPa->XScale);
              else
                sprintf(buffer2, "translate(%g,%g) ", pPa->XScale, pPa->YScale);

              add = TRUE;
            }
          break;

        case PtElScale:
          if(!(pPa->XScale == 1 && pPa->YScale == 1))
            {
              if(pPa->XScale == pPa->YScale)
                sprintf(buffer2, "scale(%g) ", pPa->XScale);
              else
                sprintf(buffer2, "scale(%g,%g) ", pPa->XScale, pPa->YScale);

              add = TRUE;
            }
          break;

        case PtElRotate:
          if(pPa->TrAngle != 0)
            {
              if(pPa->XRotate == 0 && pPa->YRotate == 0)
                sprintf(buffer2, "rotate(%g) ", pPa->TrAngle);
              else
                sprintf(buffer2, "rotate(%g,%g,%g) ",
                        pPa->TrAngle,
                        pPa->XRotate,
                        pPa->YRotate
                        );

              add = TRUE;
            }
          break;  

        case PtElSkewX:
          if(pPa->TrFactor != 0)
            {
              sprintf(buffer2, "skewX(%g) ", pPa->TrFactor);
              add = TRUE;
            }
          break;

        case PtElSkewY:
          if(pPa->TrFactor != 0)
            {
              sprintf(buffer2, "skewY(%g) ", pPa->TrFactor);
              add = TRUE;
            }          
          break;	  

        case PtElMatrix:
          a = pPa->AMatrix;b = pPa->BMatrix;c = pPa->CMatrix;
          d = pPa->DMatrix;e = pPa->EMatrix;f = pPa->FMatrix;
          if(!(a == 1 && b == 0 && c == 0 && d == 1 && e == 0 && f == 0))
            {
              sprintf(buffer2, "matrix(%g,%g,%g,%g,%g,%g) ", a, b, c, d, e, f);  
              add = TRUE;
            }
          break;

        default:
          break;
        }
	   
      if(add)strcat(buffer, buffer2);
      pPa = pPa->Next;
    }
  
  TtaFreeTransform(transform);

  result = (char *)TtaGetMemory(strlen(buffer) + 1);

  if(result)
    strcpy(result, buffer);

  return result;
}

/*----------------------------------------------------------------------
  TtaGetMatrixTransform

  Get the coefficients of the matrix representing the transform attached to
  the element el.

  ( a  c  e )
  ( b  d  f )
  ( 0  0  1 )

  ---------------------------------------------------------------------- */
void TtaGetMatrixTransform (Document document, Element el,
                            float *a, float *b, float *c,
                            float *d, float *e, float *f)
{
  PtrTransform transform;

  if (!el)
    return;
  transform = SimplifyTransformMatrix (el, NULL);
  *a = transform->AMatrix;
  *b = transform->BMatrix;
  *c = transform->CMatrix;
  *d = transform->DMatrix;
  *e = transform->EMatrix;
  *f = transform->FMatrix;
  TtaFreeTransform (transform);
}

/*----------------------------------------------------------------------
  TtaSetElCoordinateSystem : make this element the start of a new
  coordinate system
  ----------------------------------------------------------------------*/
void TtaSetElCoordinateSystem (Element element)
{
  ((PtrElement)element)->ElSystemOrigin = TRUE;
}

/*----------------------------------------------------------------------
  TtaAppendAnim
  ----------------------------------------------------------------------*/
void TtaAppendAnim (Element element, void *anim)
{
  Animated_Element *a_list;
  
  if (((PtrElement)element)->ElParent)
    { 
      if (((PtrElement)element)->ElParent->ElAnimation)
        {
          a_list = (Animated_Element *)((PtrElement)element)->ElParent->ElAnimation;
          while (a_list->next)
            a_list = a_list->next;
          a_list->next = (Animated_Element *)anim;
        }
      else
        ((PtrElement)element)->ElParent->ElAnimation = anim; 
    }
}

/*----------------------------------------------------------------------
  TtaNewAnimPath
  create a new path segment defining the animation position

  Parameters:
  anim_seg: the animation path to be modified.
  doc: the path segment to be appended.
  ----------------------------------------------------------------------*/
void *TtaNewAnimPath (Document doc)
{
#ifdef _GL
  AnimPath *anim_seg;
  
  anim_seg = (AnimPath *)TtaGetMemory (sizeof (AnimPath));
  memset (anim_seg, 0, sizeof (AnimPath));
  return ((void *) anim_seg);
#else /* _GL */
  return NULL;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  TtaAppendPathSegToAnim

  Appends a path segment at the end of an animation

  Parameters:
  animation: the animation to be modified.
  segment: the path segment to be appended.
  ----------------------------------------------------------------------*/
void TtaAppendPathSegToAnim (void *anim, PathSegment segment, Document doc)
{
#ifdef _GL
  PtrPathSeg       pPa, pPrevPa;

  if (anim)
    {
      pPa = ((AnimPath *) anim)->FirstPathSeg;
      pPrevPa = NULL;
      while (pPa)
        {
          pPrevPa = pPa;
          pPa = pPa->PaNext;
        }
      if (pPrevPa == NULL)
        ((AnimPath *) anim)->FirstPathSeg = (PtrPathSeg) segment;
      else
        {
          pPrevPa->PaNext = (PtrPathSeg) segment;
          ((PtrPathSeg) segment)->PaPrevious = pPrevPa;
        }
    }
  else
#endif /* _GL */
    FreePathSeg ((PtrPathSeg)segment);
}
/*----------------------------------------------------------------------
  TtaAnimPathAddPoint
  ----------------------------------------------------------------------*/
void TtaAnimPathAddPoint (void *anim, float x, float y)
{
#ifdef _GL
  ThotPoint  *points, *tmp;
  AnimPath *path;
  int size;

  if (anim)
    {
      path = (AnimPath *) anim;
      if (path->Path == NULL ||
          path->npoints == 0 ||
          path->npoints >= path->maxpoints)
        {
          /*realloc*/
          size = path->maxpoints + ALLOC_POINTS;
          if ((tmp = (ThotPoint*)realloc(path->Path, size * sizeof(ThotPoint))) ==0)
            return;
          else
            {
              /* la reallocation a reussi */
              path->Path = tmp;
              path->maxpoints = size;
            }
        }
      points = path->Path;
      points[path->npoints].x = x;
      points[path->npoints].y = y;
      path->npoints++;
    }
#endif /* _GL */
}
/*----------------------------------------------------------------------
  TtaAddAnimMotionPath
  ----------------------------------------------------------------------*/
void TtaAddAnimMotionPath (void *info, void *anim)
{    
#ifdef _GL
  ((Animated_Element *) anim)->from = info;
  populate_path_proportion ((Animated_Element *) anim);
#endif /* _GL */
}
/*----------------------------------------------------------------------
  TtaAddAnimMotionFromTo
  ----------------------------------------------------------------------*/
void TtaAddAnimMotionFromTo (void *info, void *anim)
{    
#ifdef _GL
  ((Animated_Element *) anim)->from = info;
  populate_fromto_proportion (anim);  
#endif /* _GL */ 
}
/*----------------------------------------------------------------------
  TtaAddAnimMotionValues
  ----------------------------------------------------------------------*/
void TtaAddAnimMotionValues (void *info, void *anim)
{    
#ifdef _GL
  ((Animated_Element *) anim)->from = info;
  populate_values_proportion (anim);  
#endif /* _GL */  
}
/*----------------------------------------------------------------------
  TtaSetAnimTypetoTransform
  ----------------------------------------------------------------------*/
void TtaSetAnimTypetoMotion (void *anim)
{
  ((Animated_Element *) anim)->AnimType = Motion;
}
/*----------------------------------------------------------------------
  TtaSetAnimTypetoTransform
  ----------------------------------------------------------------------*/
void TtaSetAnimTypetoTransform (void *anim)
{
  ((Animated_Element *) anim)->AnimType = Transformation;
}

/*----------------------------------------------------------------------
  TtaSetAnimTypetoAnimate
  ----------------------------------------------------------------------*/
void TtaSetAnimTypetoAnimate (void *anim)
{
  ((Animated_Element *) anim)->AnimType = Animate;
}

/*----------------------------------------------------------------------
  TtaSetAnimTypetoColor
  ----------------------------------------------------------------------*/
void TtaSetAnimTypetoColor (void *anim)
{
  ((Animated_Element *) anim)->AnimType = Color;
}

/*----------------------------------------------------------------------
  TtaSetAnimTypetoSet
  ----------------------------------------------------------------------*/
void TtaSetAnimTypetoSet (void *anim)
{
  ((Animated_Element *) anim)->AnimType = Set;
}

/*----------------------------------------------------------------------
  TtaSetAnimReplace : If anim replace the precedent transformation
  concering this attribute
  ----------------------------------------------------------------------*/
void TtaSetAnimReplace (void *anim, ThotBool is_replace)
{    
  ((Animated_Element *) anim)->replace = is_replace;
}
/*----------------------------------------------------------------------
  TtaAddAnimFrom
  ----------------------------------------------------------------------*/
void TtaAddAnimFrom (void *info, void *anim)
{    
  ((Animated_Element *) anim)->from = info;
}

/*----------------------------------------------------------------------
  TtaAddAnimFreeze
  ----------------------------------------------------------------------*/
void TtaAddAnimFreeze (void *anim)
{
  ((Animated_Element *) anim)->Fill = Freeze;
}

/*----------------------------------------------------------------------
  TtaAddAnimFillRemove
  ----------------------------------------------------------------------*/
void TtaAddAnimRemove (void *anim)
{      
  ((Animated_Element *) anim)->Fill = Otherfill;
}
/*----------------------------------------------------------------------
  TtaAddAnimRepeatCount
  ----------------------------------------------------------------------*/
void TtaAddAnimRepeatCount (int repeat, void *anim)
{
  ((Animated_Element *) anim)->repeatCount = repeat;
}

/*----------------------------------------------------------------------
  TtaAddAnimTo
  ----------------------------------------------------------------------*/
void TtaAddAnimTo (void *info, void *anim)
{    
  ((Animated_Element *) anim)->to = info;
}

/*----------------------------------------------------------------------
  TtaAddAnimTo
  ----------------------------------------------------------------------*/
void TtaAddAnimAttrName (void *info, void *anim)
{    
  ((Animated_Element *) anim)->AttrName = (char *)info;
}

/*----------------------------------------------------------------------
  TtaSetAnimationTime
  ----------------------------------------------------------------------*/
void TtaSetAnimationTime (void *anim_info, double start, double duration)
{    
  ((Animated_Element *) anim_info)->duration = duration;
  ((Animated_Element *) anim_info)->start = start;
}
#endif /* NODISPLAY */
/*----------------------------------------------------------------------
  TtaCopyPage

  Copies the page element source into the page element destination.
  Both page elements must be in an abstract tree.
  Parameters:
  destination: identifier of the page element to be modified.
  source : identifier of the source page element.
  ----------------------------------------------------------------------*/
void TtaCopyPage (Element destination, Element source)
{
  PtrElement          pS, pD;

  UserErrorCode = 0;
  pS = (PtrElement) source;
  pD = (PtrElement) destination;
  if (destination == NULL || source == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pD->ElTerminal || pD->ElLeafType != LtPageColBreak)
    TtaError (ERR_invalid_parameter);
  else if (!pS->ElTerminal ||
           pS->ElLeafType != LtPageColBreak)
    TtaError (ERR_invalid_parameter);
  else
    {
      pD->ElPageType = pS->ElPageType;
      pD->ElPageNumber = pS->ElPageNumber;
      pD->ElViewPSchema = pS->ElViewPSchema;
    }
}

/*----------------------------------------------------------------------
  GetImageDesc

  Returns a pointer to the  PictInfo structure that's associated with
  element. Returns NULL if element doesn't have such structure.
  ----------------------------------------------------------------------*/
static ThotPictInfo *GetImageDesc (Element element)
{
  PtrAbstractBox   pAb;
  PtrElement       pEl = (PtrElement) element;
  ThotPictInfo    *imageDesc = NULL;
  int              view;
  ThotBool         found;

  UserErrorCode = 0;
  imageDesc = NULL;

  if (pEl == NULL)
    TtaError (ERR_invalid_parameter);
  else if (TypeHasException (ExcIsImg, pEl->ElTypeNumber, pEl->ElStructSchema))
    pEl = pEl->ElFirstChild;

  if (pEl == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pEl->ElTerminal)
    {
      view = 0;
      pAb = NULL;
      found = FALSE;
      do
        {
          pAb = pEl->ElAbstractBox[view];
          if (pAb != NULL)
            {
              while (pAb->AbPresentationBox)
                pAb = pAb->AbNext;
              /* the background exists or will exist soon */
              found = (pAb->AbPictBackground != NULL || pAb->AbAspectChange);
              if (found)
                imageDesc = (ThotPictInfo *) pAb->AbPictBackground;
              else
                {
                  found = (pAb->AbPictListStyle != NULL || pAb->AbAspectChange);
                  if (found)
                    imageDesc = (ThotPictInfo *) pAb->AbPictListStyle;
                }
            }
          view++;
        }
      while (!found && view < MAX_VIEW_DOC);

      if (!found)
        TtaError (ERR_invalid_element_type);
    }
  else if (pEl->ElTerminal && pEl->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  else
    imageDesc = (ThotPictInfo *) (pEl->ElPictInfo);
  return imageDesc;
}

/*----------------------------------------------------------------------
  TtaGivePictureSize
  Returns the original width and height of the picture.
  Parameter:
  element: the element of interest. This element must be a picture
  Return value:
  width and height of the image
  ----------------------------------------------------------------------*/
void TtaGivePictureSize (Element element, int *width, int *height)
{
  ThotPictInfo    *imageDesc;

  *width = *height = 0;
  imageDesc = GetImageDesc (element);
  if (imageDesc)
    {
      *width = imageDesc->PicWidth;
      *height = imageDesc->PicHeight;
    }
}

/*----------------------------------------------------------------------
  TtaGetPictureType
  Returns the type of Picture element.
  Parameter:
  element: the element of interest. This element must be a Picture or have
  a background image.
  Return value:
  PicType: type of the element.
  ----------------------------------------------------------------------*/
PicType TtaGetPictureType (Element element)
{
  PicType          pictType;
  ThotPictInfo    *imageDesc;
  int              typeImage;

  pictType = unknown_type;
  imageDesc = GetImageDesc (element);
  if (imageDesc)
    {
      typeImage = imageDesc->PicType;
      if (typeImage != unknown_type && typeImage != -1)
        pictType = (PicType) typeImage;
    }
  return pictType;
}

/*----------------------------------------------------------------------
  TtaSetPictureType
  Sets the type of a Picture element.
  Parameter:
  mime_type: mime type of an image.
  ----------------------------------------------------------------------*/
void TtaSetPictureType (Element element, const char *mime_type)
{
  PicType          typeImage;
  ThotPictInfo    *imageDesc;

  if (!element || !mime_type || *mime_type == EOS)
    return;

  imageDesc = GetImageDesc (element);
  if (imageDesc)
    {
      if (!strcmp (mime_type, "image/x-bitmap"))
        typeImage = xbm_type;
      else if (!strcmp (mime_type, "application/postscript"))
        typeImage = eps_type;
      else if (!strcmp (mime_type, "image/x-xpixmap"))
        typeImage = eps_type;
      else if (!strcmp (mime_type, "image/gif"))
        typeImage = gif_type;
      else if (!strcmp (mime_type, "image/jpeg"))
        typeImage = jpeg_type;
      else if (!strcmp (mime_type, "image/png"))
        typeImage = png_type;
      else if (!strcmp (mime_type, "image/svg") ||
               !strcmp (mime_type, "image/svg+xml") ||
               !strcmp (mime_type, "application/svg+xml"))
        typeImage = svg_type;
      else if (!strcmp (mime_type, "text/html") ||
               !strcmp (mime_type, "application/xhtml+xml"))
        typeImage = html_type;
      else if (!strcmp (mime_type, "text/mml") ||
               !strcmp (mime_type, "application/mathml+xml"))
        typeImage = mathml_type;
      else 
        typeImage = unknown_type;
      imageDesc->PicType = typeImage;
    }
}


/*----------------------------------------------------------------------
  TtaGiveBufferContent

  Returns the content of a Text basic element as a string of CHAR_T
  characters.
  Parameters:
  element: the element of interest. This element must be a basic
  element of type Text.
  buffer: the buffer that will contain the returned string. This buffer
  must be at least of size length.
  The length corresponds to the buffer length.
  length: the maximum length of the string to be returned. Must be strictly
  positive.
  Return parameter:
  buffer: (the buffer contains the substring).
  language: language of the text.
  ----------------------------------------------------------------------*/
void TtaGiveBufferContent (Element element, CHAR_T *buffer, int length,
                           Language *language)
{
  PtrTextBuffer       pBuf;
  PtrElement          pEl;
  int                 len, l;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  if (pEl)
    *language = pEl->ElLanguage;
  else
    *language = TtaGetDefaultLanguage ();

  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pEl->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (pEl->ElLeafType != LtText && pEl->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  else
    {
      len = 0;
      pBuf = pEl->ElText;
      while (pBuf != NULL && len < length - 1)
        {
          l = 0;
          if (length < len + pBuf->BuLength + 1)
            l = length - len;
          else
            l = pBuf->BuLength + 1;
          ustrncpy (&buffer[len], pBuf->BuContent, l);
          len = len + (l - 1);
          pBuf = pBuf->BuNext;
        }
    }
}

/*----------------------------------------------------------------------
  TtaSetBufferContent

  Changes the content of a Text basic element. The full content (if any) is
  deleted and replaced by the new one.
  Parameters:
  element: the Text element to be modified.
  content: new content for that element coded as a string of CHAR_T
  characters.
  language: language of that Text element.
  document: the document containing that element.
  ----------------------------------------------------------------------*/
void TtaSetBufferContent (Element element, CHAR_T *content,
                          Language language, Document document)
{
  PtrElement    pEl;
  PtrTextBuffer pBuf, prevBuf;
  int           remaining, length, delta, bulen, i;

  UserErrorCode = 0;
  pEl = NULL;
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
    {
      /* parameter document is correct */
      pEl = (PtrElement) element;
      pEl->ElLanguage = language;
      
      /* store the contents of the element */
      pBuf = pEl->ElText;
      if (content)
        length = ustrlen (content);
      else
        length = 0;
      if (length == 0)
        /* the new content of the element is null */
        {
          if (pBuf)
            {
              pBuf->BuContent[0] = EOS;
              pBuf->BuLength = 0;
              prevBuf = pBuf;
              pBuf = pBuf->BuNext;
            }
        }
      else
        {
          prevBuf = NULL;
          remaining = length;
          i = 0;
          while (remaining > 0)
            {
              if (!pBuf)
                /* create and initialize a new buffer */
                {
                  GetTextBuffer (&pBuf);
                  pBuf->BuNext = NULL;
                  if (prevBuf)
                    prevBuf->BuNext = pBuf;
                  else
                    pEl->ElText = pBuf;
                }
              if (remaining >= THOT_MAX_CHAR)
                /* the remaining length is longer than a single buffer */
                bulen = THOT_MAX_CHAR - 1;
              else
                bulen = remaining;
              ustrncpy (pBuf->BuContent, &content[i], bulen);
              i += bulen;
              pBuf->BuContent[bulen] = EOS;
              pBuf->BuLength = bulen;
              remaining -= bulen;
              prevBuf = pBuf;
              pBuf = pBuf->BuNext;
            }
        }

      /* free the remaining buffers, if any */
      if (pBuf && prevBuf)
        {
          prevBuf->BuNext = NULL;
          while (pBuf)
            {
              prevBuf = pBuf;
              pBuf = pBuf->BuNext;
              FreeTextBuffer (prevBuf);
            }
        }

      delta = length - pEl->ElTextLength;
      pEl->ElTextLength = length;
      pEl->ElVolume = length;

#ifndef NODISPLAY
      RedisplayLeaf (pEl, document, delta);
#endif /* NODISPLAY */

      /* Updates the volume of ancestors */
      if (delta != 0)
        {
          pEl = pEl->ElParent;
          while (pEl != NULL)
            {
              pEl->ElVolume += delta;
              pEl = pEl->ElParent;
            }
        }
    }
}


/*----------------------------------------------------------------------
  TtaGetFirstBufferContent

  Returns the first CHAR_T character of the string.
  Parameters:
  element: the element of interest. This element must be a basic
  element of type Text.
  Return parameter:
  buffer: the first character.
  ----------------------------------------------------------------------*/
CHAR_T TtaGetFirstBufferContent (Element element)
{
  PtrTextBuffer       pBuf;
  PtrElement          pEl;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  if (pEl == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pEl->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (pEl->ElLeafType != LtText && pEl->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  else
    {
      pBuf = pEl->ElText;
      return pBuf->BuContent[0];
    }
  return EOS;
}


/*----------------------------------------------------------------------
  TtaGetLastBufferContent

  Returns the last CHAR_T character of the string.
  Parameters:
  element: the element of interest. This element must be a basic
  element of type Text.
  Return parameter:
  buffer: the last character.
  ----------------------------------------------------------------------*/
CHAR_T TtaGetLastBufferContent (Element element)
{
  PtrTextBuffer       pBuf;
  PtrElement          pEl;
  int                 length;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  if (pEl == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pEl->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (pEl->ElLeafType != LtText && pEl->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  else
    {
      length = pEl->ElVolume - 1;
      pBuf = pEl->ElText;
      while (pBuf && pBuf->BuLength < length)
        {
          length -= pBuf->BuLength;
          pBuf = pBuf->BuNext;
        }
      if (pBuf)
        return pBuf->BuContent[length];
    }
  return EOS;
}


/*----------------------------------------------------------------------
  TtaGiveSubString

  Returns a substring from a Text basic element.
  Parameters:
  element: the element of interest. This element must be a basic
  element of type Text.
  buffer: the buffer that will contain the substring. This buffer
  must be at least of size length.
  In _I18N_ mode the length corresponds to the UTF-8 string.
  position: the rank of the first character of the substring.
  length: the length of the substring. Must be strictly positive.
  Return parameter:
  buffer: (the buffer contains the substring).
  In _I18N_ mode returns a UTF-8 string.
  ----------------------------------------------------------------------*/
void TtaGiveSubString (Element element, unsigned char *buffer, int position,
                       int length)
{
  PtrTextBuffer       pBuf;
  PtrElement          pEl;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (position < 1 || length < 1)
    TtaError (ERR_invalid_parameter);
  else if (!pEl->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (pEl->ElLeafType != LtText &&
           pEl->ElLeafType != LtPicture)
    TtaError (ERR_invalid_element_type);
  else
    {
      pBuf = pEl->ElText;
      position--;
      /* skip previous buffers */
      while (pBuf && pBuf->BuLength <= position)
        {
          position -= pBuf->BuLength;
          pBuf = pBuf->BuNext;
        }
      /* copying into the buffer */
      length = CopyBuffer2MBs (pBuf, position, buffer, length);
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
char TtaGetGraphicsShape (Element element)
{
  PtrElement          pEl;
  char                content;

  UserErrorCode = 0;
  pEl = (PtrElement) element;
  content = EOS;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!pEl->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (pEl->ElLeafType != LtSymbol &&
           pEl->ElLeafType != LtGraphics &&
           pEl->ElLeafType != LtPolyLine)
    TtaError (ERR_invalid_element_type);
  else if (pEl->ElLeafType == LtPolyLine)
    content = pEl->ElPolyLineType;
  else
    content = pEl->ElGraph;
  return content;
}

/*----------------------------------------------------------------------
  TtaGetPolylineLength

  Returns the number of points in a Polyline basic element.
  Parameter:
  element: the Polyline element. This element must
  be a basic element of type Polyline.
  ----------------------------------------------------------------------*/
int TtaGetPolylineLength (Element element)
{
  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElLeafType != LtPolyLine)
    TtaError (ERR_invalid_element_type);
  else
    /* one ignore the boundary point limit, considered in ElNPoints */
    return ((PtrElement) element)->ElNPoints - 1;
  return (0);
}

/*----------------------------------------------------------------------
  TtaGivePolylinePoint

  Returns the coordinates of a point in a Polyline basic element.

  Parameters:
  element: the Polyline element. This element must
  be a basic element of type Polyline.
  rank: rank of the point in the PolyLine. If rank is greater
  than the actual number of points, an error is raised.
  rank must be strictly positive.
  unit: UnPixel or UnPoint.

  Return values:
  x, y: coordinates of the point, in unit, relatively to
  the upper left corner of the enclosing rectangle.

  ----------------------------------------------------------------------*/
void TtaGivePolylinePoint (Element element, int rank, TypeUnit unit,
                           int *x, int *y)
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
      while (rank >= pBuff->BuLength && pBuff->BuNext != NULL)
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
        }
    }
}

/*----------------------------------------------------------------------
  Norm
  ----------------------------------------------------------------------*/
static double Norm(double dx1, double dy1)
{
  return sqrt(dx1*dx1+dy1*dy1);
}

/*----------------------------------------------------------------------
  UnsignedAngle
  ----------------------------------------------------------------------*/
static double UnsignedAngle(double dx1, double dy1,
			    double dx2, double dy2)
{
  /*       (dx1) (dx2)
       s = (dy1).(dy2) = r1*r2*cosA

   */

  double s, r1, r2, cosA;
  s = dx1*dx2 + dy1*dy2;
  r1 = Norm(dx1, dy1);
  r2 = Norm(dx2, dy2);

  if(r1 == 0 || r2 == 0)
    return 0;

  cosA = s/(r1*r2);

  /* Avoid errors when cosA is slightly greater/lower that 1/-1
    because of float approximations */
  if(cosA > 1.)cosA = 1.;
  if(cosA < -1.)cosA = -1.;

  return acos(cosA);
}

/*----------------------------------------------------------------------
  ThetaAngle
  ----------------------------------------------------------------------*/
static double ThetaAngle(double dx, double dy)
{
  /*       (1)(dx)
       s = (0)(dy) = Norm(dx, dy)*cosA

       det = dy;
   */

  double norm, cosA, A;

  norm = Norm(dx, dy);
  if(norm == 0)
    return 0;

  cosA = dx/norm;

  /* Avoid errors when cosA is slightly greater/lower that 1/-1
    because of float approximations */
  if(cosA > 1.)cosA = 1.;
  if(cosA < -1.)cosA = -1.;

  A = acos(cosA);
  return (dy < 0 ? -A : A);
}

/*----------------------------------------------------------------------
  BisectorAngle
  ----------------------------------------------------------------------*/
static double BisectorAngle(double dxPrev, double dyPrev,
			    double dxNext, double dyNext)
{
  double norm1, norm2, dx1, dy1, dx2, dy2;
  double dx, dy;

  norm1= Norm(dxPrev, dyPrev);
  norm2= Norm(dxNext, dyNext);
  if(norm1 == 0 || norm2 == 0)
    return 0;

  dx1 = dxPrev/norm1;
  dy1 = dyPrev/norm1;
  dx2 = dxNext/norm2;
  dy2 = dyNext/norm2;

  dx = dx1 + dx2;
  dy = dy1 + dy2;

  return ThetaAngle(dx, dy);
}

/*----------------------------------------------------------------------
  TtaGivePolylineAngle

  Returns the bisector angle of a vertex in a Polyline basic element.

  Parameters:
  element: the Polyline element. This element must
           be a basic element of type Polyline.
  rank: rank of the point in the PolyLine. If rank is greater
        than the actual number of points, an error is raised.
        rank must be strictly positive.

  Return value:
  angle: the angle of the bisector.

  ----------------------------------------------------------------------*/
void TtaGivePolylineAngle (Element element, int rank, double *angle)
{
  PtrTextBuffer       buff, prevBuff;
  int                 x, y, xPrev, yPrev, xNext, yNext, xStart, yStart;
  ThotBool            first, last;

  /* TODO:

     - zero-length path segments F.5 'path' element implementation notes.
     - when it's a <polygon/>, its last point is not the last element of the
       polyline buffer.
     - directionality of the first & last point in a closed polyline.
 */

  UserErrorCode = 0;
  *angle = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElLeafType != LtPolyLine)
    TtaError (ERR_invalid_element_type);
  else
    {
      /* Looking for the buffer containing the point which rank is: rank */
      first = (rank == 1);

      last = (rank == ((PtrElement) element)->ElNPoints - 1);
      buff = ((PtrElement) element)->ElPolyLineBuffer;
      xStart = buff->BuPoints[1].XCoord;
      yStart = buff->BuPoints[1].YCoord;
      xPrev = yPrev = xNext = yNext = 0;
      prevBuff = NULL;
      while (rank >= buff->BuLength && buff->BuNext != NULL)
        {
          rank -= buff->BuLength;
          prevBuff = buff;
          buff = buff->BuNext;
        }
      if (rank > buff->BuLength)
        TtaError (ERR_invalid_parameter);
      else
        {
          x = buff->BuPoints[rank].XCoord;
          y = buff->BuPoints[rank].YCoord;
          if (rank > 1)
            {
              xPrev = buff->BuPoints[rank-1].XCoord;
              yPrev = buff->BuPoints[rank-1].YCoord;
            }
          else if (prevBuff && !first)
            {
              xPrev = prevBuff->BuPoints[prevBuff->BuLength].XCoord;
              yPrev = prevBuff->BuPoints[prevBuff->BuLength].YCoord;
            }

          if (rank < buff->BuLength)
            {
              xNext = buff->BuPoints[rank+1].XCoord;
              yNext = buff->BuPoints[rank+1].YCoord;
            }
          else if (buff->BuNext && !last)
            {
              xNext = buff->BuNext->BuPoints[1].XCoord;
              yNext = buff->BuNext->BuPoints[1].YCoord;
            }

          if (first)
            /* first point in the polyline */
            *angle = ThetaAngle(xNext-x, yNext-y);
          else if (last)
            /* last point in the polyline */
            *angle = ThetaAngle(x-xPrev, y-yPrev);
          else
            /* any other point in the polyline : bisector of the angle */
            *angle = BisectorAngle(x-xPrev, y-yPrev, xNext-x, yNext-y);
          
          (*angle)*=(180/M_PI);
        }
    }
}

/*----------------------------------------------------------------------
  TtaGivePathPoint

  Returns the coordinates of a point in a Path basic element.

  Parameters:
  element: the Path element. This element must
           be a basic element of type Path.
  rank: rank of the point in the Path. If rank is greater
        than the actual number of points, an error is raised.
        rank must be strictly positive.
  unit: UnPixel or UnPoint.

  Return values:
  x, y: coordinates of the point, in unit
  ----------------------------------------------------------------------*/
void TtaGivePathPoint (Element element, int rank, TypeUnit unit, int *x, int *y)
{
  PtrPathSeg       pPa;
  int              i;
  ThotBool firstPoint = FALSE;

  UserErrorCode = 0;
  *x = 0;
  *y = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElLeafType != LtPath)
    TtaError (ERR_invalid_element_type);
  else if (unit != UnPoint && unit != UnPixel)
    TtaError (ERR_invalid_parameter);
  else if (rank > TtaNumberOfPointsInPath(element) + 1)
    TtaError (ERR_invalid_parameter);
  else
    {
      i = 1;
      for(pPa = ((PtrElement) element)->ElFirstPathSeg;
	  pPa;
	  pPa = pPa->PaNext)
	{
	  if(!pPa->PaPrevious || pPa->PaNewSubpath)
	    {
	      /* First point of a subpath */
	      if(i == rank)
		{
		  firstPoint = TRUE;
		  break;
		}
	      i++;
	    }

	  if(i == rank)
	    break;
	  i++;
	}
	  
      if (pPa)
        {
	  if(firstPoint)
	    {
	      *x = pPa->XStart;
	      *y = pPa->YStart;
	    }
	  else 
	    {
	      *x = pPa->XEnd;
	      *y = pPa->YEnd;
	    }
        }
    }
}

/*----------------------------------------------------------------------
  GivePathSegmentAngle

  before = the segment is before the point
  ----------------------------------------------------------------------*/
static ThotBool GivePathSegmentAngle(PtrPathSeg pPa,
				     ThotBool before,
				     double *dx,
				     double *dy)
{
  double rx, ry, cx, cy, phi, theta, dtheta;
  int x1, y1, x2, y2;

  if(pPa == NULL)
    return FALSE;

  *dx = 1.;
  *dy = 0.;

  switch (pPa->PaShape)
    {
    case PtLine:
      *dx = pPa->XEnd - pPa->XStart;
      *dy = pPa->YEnd - pPa->YStart;
      break;
    case PtQuadraticBezier:
    case PtCubicBezier:
      /* Directionnality is given by the start/end handles */
      if(before)
	{
	  *dx = pPa->XEnd - pPa->XCtrlEnd;
	  *dy = pPa->YEnd - pPa->YCtrlEnd;
	}
      else
	{
	  *dx = pPa->XCtrlStart - pPa->XStart;
	  *dy = pPa->YCtrlStart - pPa->YStart;
	}
      break;
    case PtEllipticalArc:
      x1 = pPa->XStart;
      y1 = pPa->YStart;
      x2 = pPa->XEnd;
      y2 = pPa->YEnd;
      rx = pPa->XRadius;
      ry = pPa->YRadius;
      phi = pPa->XAxisRotation;

      if(!TtaEndPointToCenterParam(x1, y1, x2, y2,
				   &rx, &ry,
				   &phi,
				   pPa->LargeArc, pPa->Sweep,
				   &cx, &cy,
				   &theta, &dtheta
				   ))
	return FALSE;

      /*
       * For T going from theta1 to theta1 + dtheta,
       *(x(T))   (cosf -sinf)(rx*cos(T))   (cx)
       *(y(T)) = (sinf  cosf)(ry*cos(T)) + (cy)
       *
       *(x'(T))   *dx
       *(y'(T)) = *dy
       *
       */
      if(before)
	theta += dtheta;

      *dx = -cos(phi)*rx*sin(theta) - sin(phi)*ry*cos(theta);
      *dy = -sin(phi)*rx*sin(theta) + cos(phi)*ry*cos(theta);

      /* Parameter T is decreasing */
      if(dtheta < 0)
	{
	  *dx = -*dx;
	  *dy = -*dy;
	}
      break;
    }

  return TRUE;
}

/*----------------------------------------------------------------------
  TtaGivePathAngle

  Returns the bisector angle of a vertex in a Path basic element.

  Parameters:
  element: the Path element. This element must be a basic element of type Path.
  rank: rank of the point in the Path. If rank is greater
        than the actual number of points, an error is raised.
        rank must be strictly positive.

  Return value:
  angle: the angle of the bisector.

  ----------------------------------------------------------------------*/
void TtaGivePathAngle (Element element, int rank, double *angle)
{
  PtrPathSeg       pPa, pPaStart;
  PtrPathSeg       pPaPrevious = NULL, pPaNext = NULL;
  int              i;
  double           dxPrevious, dyPrevious, dxNext, dyNext;

  UserErrorCode = 0;
  if (element == NULL)
    TtaError (ERR_invalid_parameter);
  else if (!((PtrElement) element)->ElTerminal)
    TtaError (ERR_invalid_element_type);
  else if (((PtrElement) element)->ElLeafType != LtPath)
    TtaError (ERR_invalid_element_type);
  else if (rank > TtaNumberOfPointsInPath(element) + 1)
    TtaError (ERR_invalid_parameter);
  else
    {
      i = 1;
      for(pPa = ((PtrElement) element)->ElFirstPathSeg;
	  pPa;
	  pPa = pPa->PaNext)
	{
	  if(!pPa->PaPrevious || pPa->PaNewSubpath)
	    {
	      pPaStart = pPa;

	      /* First point of a subpath */
	      if(i == rank)
		{
		  while(pPa->PaNext && !(pPa->PaNext->PaNewSubpath))
		    pPa = pPa->PaNext;

		  if(pPa->XEnd == pPaStart->XStart &&
		     pPa->YEnd == pPaStart->YStart)
		    {
		      /* A closed subpath */
		      pPaPrevious = pPa;
		    }
		  else
		    pPaPrevious = NULL;

		  pPaNext = pPaStart;
		  break;
		}
	      i++;
	    }

	  if(i == rank)
	    {
	      if(!pPa->PaNext || pPa->PaNext->PaNewSubpath)
		{
		  /* Last point of a subpath */
		  pPaPrevious = pPa;

		  if(pPa->XEnd == pPaStart->XStart &&
		     pPa->YEnd == pPaStart->YStart)
		    {
		      /* A closed subpath */
		      pPaNext = pPaStart;
		    }
		  else
		    pPaNext = NULL;
		}
	      else
		{
		  /* Point between two segments */
		  pPaPrevious = pPa;
		  pPaNext = pPa->PaNext;
		}
	      break;
	    }
	  i++;
	}

      /* TODO: zero-length path segments
	 F.5 'path' element implementation notes */

      *angle = 0;
      GivePathSegmentAngle(pPaPrevious, TRUE, &dxPrevious, &dyPrevious);
      GivePathSegmentAngle(pPaNext, FALSE, &dxNext, &dyNext);

      if(pPaPrevious && pPaNext)
	*angle = BisectorAngle(dxPrevious, dyPrevious, dxNext, dyNext);
      else if(pPaPrevious)
	*angle = ThetaAngle(dxPrevious, dyPrevious);
      else if(pPaNext)
	*angle = ThetaAngle(dxNext, dyNext);

      (*angle)*=(180/M_PI);
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
int TtaGetPageNumber (Element pageElement)
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
int TtaGetPageView (Element pageElement)
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


/*----------------------------------------------------------------------
  Almost*

  Used in CheckGeometricProperties to check the approximative equalities
  of mathematical properties.
  ----------------------------------------------------------------------*/

/* Error on angles are less than 1° */
#define EPSILON_MAX M_PI/180

/*----------------------------------------------------------------------
  IsNull
  ----------------------------------------------------------------------*/
static ThotBool IsNull(double dx1, double dy1)
{
  return (dx1 == 0 && dy1 == 0);
}

/*----------------------------------------------------------------------
  AlmostOrthogonalVectors
  ----------------------------------------------------------------------*/
static ThotBool AlmostOrthogonalVectors(double dx1, double dy1,
                                        double dx2, double dy2)
{
  double epsilon;

  if(IsNull(dx1, dy1) || IsNull(dx2, dy2))return TRUE;
  
  epsilon = fabs(UnsignedAngle(dx1, dy1, dx2, dy2) - M_PI/2);
  return (epsilon < EPSILON_MAX);
}

/*----------------------------------------------------------------------
  AlmostColinearVectors
  same = vectors are indentically oriented
  ----------------------------------------------------------------------*/
static ThotBool AlmostColinearVectors(double dx1, double dy1,
                                      double dx2, double dy2,
                                      ThotBool same)
{
  double angle;

  if(IsNull(dx1, dy1) || IsNull(dx2, dy2))return TRUE;

  angle = UnsignedAngle(dx1, dy1, dx2, dy2);
  return (angle < EPSILON_MAX ||
          (!same && fabs(angle - M_PI) < EPSILON_MAX));
}

/*----------------------------------------------------------------------
  AlmostEqualAngle
  ----------------------------------------------------------------------*/
static ThotBool AlmostEqualAngle(double ax, double ay,
                                 double bx, double by,
                                 double cx, double cy,
                                 double dx, double dy,
                                 double ex, double ey,
                                 double fx, double fy)
{
  double angle1, angle2;

  if(IsNull(ax - bx, ay - by) || IsNull(cx - bx, cy - by) ||
     IsNull(dx - ex, dy - ey) || IsNull(fx - ex, fy - ey))
    return FALSE;

  angle1 = UnsignedAngle(ax - bx, ay - by, cx - bx, cy - by);
  angle2 = UnsignedAngle(dx - ex, dy - ey, fx - ex, fy - ey);

  return (fabs(angle1 - angle2) < EPSILON_MAX);
}

/*----------------------------------------------------------------------
  CircularPermutationOnTriangle
  ----------------------------------------------------------------------*/
static void CircularPermutationOnTriangle(int *x1, int *y1,
                                          int *x2, int *y2,
                                          int *x3, int *y3,
                                          int direction)
{
  int   tmpx = *x1,tmpy = *y1;

  if(direction > 0)
    {
      *x1 = *x2; *y1 = *y2;
      *x2 = *x3; *y2 = *y3;
      *x3 = tmpx; *y3 = tmpy;
    }
  else
    {
      *x1 = *x3; *y1 = *y3;
      *x3 = *x2; *y3 = *y2;
      *x2 = tmpx; *y2 = tmpy;
    }
}

/*----------------------------------------------------------------------
  CircularPermutationOnQuadrilateral
  ----------------------------------------------------------------------*/
static void CircularPermutationOnQuadrilateral(int *x1, int *y1,
                                               int *x2, int *y2,
                                               int *x3, int *y3,
                                               int *x4, int *y4)
{
  int   tmpx = *x1,tmpy = *y1;

  *x1 = *x2; *y1 = *y2;
  *x2 = *x3; *y2 = *y3;
  *x3 = *x4; *y3 = *y4;
  *x4 = tmpx; *y4 = tmpy;
  
}

/*----------------------------------------------------------------------
  IsAcuteAngle
  ----------------------------------------------------------------------*/
static ThotBool IsAcuteAngle(double x1, double y1, double x2, double y2,
                             double x3, double y3)
{
  return (UnsignedAngle(x1 - x2, y1 - y2, x3 - x2, y3 - y2) <= M_PI/2);
}

/*----------------------------------------------------------------------
  GiveIntersectionPoint
  ----------------------------------------------------------------------*/
static void GiveIntersectionPoint(double x1, double y1, double dx1, double dy1,
                                  double x2, double y2, double dx2, double dy2,
                                  double *x0, double *y0)
{
  /*                   2
   *                    \
   *                     \    (dx1,dy1)         (x1)    (dx1)   (x0)
   *               1------0------>              (y1) + t(dy1) = (y0)
   *                       \
   *                        \ (dx2,dy2)         (x2)    (dx2)   (x0)
   *                         v                  (y2) + u(dy2) = (y0)
   *
   */
  double t;
  double a, b, c, d, e, f, det;
  
  a = -dx1;
  b = dx2;
  c = -dy1;
  d = dy2;
  e = x1 - x2;
  f = y1 - y2;
  det = a*d - b*c;
  /*
   *   (a  b)(t)   (e)
   *   (c  d)(u) = (f)
   *
   */

  if(det == 0)
    {
      *x0 = 0;
      *y0 = 0;
      return;
    }

  t = (d*e - b*f)/det;
  *x0 = x1 + t*dx1;
  *y0 = y1 + t*dy1;
}

enum shapes
  {
    EQUILATERAL_TRIANGLE,
    ISOSCELES_TRIANGLE,
    RECTANGLED_TRIANGLE,
    TRAPEZIUM,
    PARALLELOGRAM,
    DIAMOND,
    RECTANGLE,
    SQUARE    
  };


/*----------------------------------------------------------------------
  PathIsPolygon
  ----------------------------------------------------------------------*/
static ThotBool PathIsPolygon(PtrPathSeg pPa, int nbPoints)
{
  int nb = 0;
  int x0, y0;
  
  if(pPa == NULL || pPa->PaShape != PtLine)return FALSE;
  x0 = pPa->XStart;
  y0 = pPa->YStart;
  nb++;

  while(pPa && pPa->PaShape == PtLine)
    {
      nb++;
      if(nb == nbPoints + 1)
        return (pPa->PaNext == NULL && pPa->XEnd == x0 && pPa->YEnd == y0);
	
      pPa=pPa->PaNext;
      if(pPa->PaNewSubpath)
        return FALSE;
    }
  return FALSE;
}


/*----------------------------------------------------------------------
  CheckGeometricProperties
  ----------------------------------------------------------------------*/
ThotBool CheckGeometricProperties(Document doc, Element leaf,
                                  int *width, int *height,
                                  int *rx, int *ry)
			      
{
  PtrPathSeg pPa;
  PtrElement pLeaf = (PtrElement)leaf;
  PtrTextBuffer       pBuffer;
  int x1,y1,x2,y2,x3,y3,x4,y4;
  int nbPoints;
  int shape = -1;
  double w = 0, h = 0;
  double a = 1, b = 0, c = 0, d = 1, e = 0, f = 0;
  double x1_,y1_,x2_,y2_,x3_,y3_,x4_,y4_;
  ThotBool doAnalyse = FALSE;

  if(pLeaf->ElLeafType == LtPolyLine && pLeaf->ElPolyLineType == 'p')
    {
      /* A polygon: look if it's a triangle/quadrilateral */

      pBuffer = pLeaf->ElPolyLineBuffer;
      nbPoints = pLeaf->ElNPoints - 1;

      if(nbPoints == 3 || nbPoints == 4)
        {
          doAnalyse = TRUE;
          x1 = pBuffer->BuPoints[1].XCoord;
          y1 = pBuffer->BuPoints[1].YCoord;
          x2 = pBuffer->BuPoints[2].XCoord;
          y2 = pBuffer->BuPoints[2].YCoord;
          x3 = pBuffer->BuPoints[3].XCoord;
          y3 = pBuffer->BuPoints[3].YCoord;
          if(nbPoints == 4)
            {
              x4 = pBuffer->BuPoints[4].XCoord;
              y4 = pBuffer->BuPoints[4].YCoord;
            }
        } 
    }
  else if(pLeaf->ElLeafType == LtPath)
    {
      /* A path: look if it's a triangle/quadrilateral */

      nbPoints = pLeaf->ElVolume;
      if(nbPoints == 3 || nbPoints == 4)
        {
          pPa = pLeaf->ElFirstPathSeg;
          if(PathIsPolygon(pPa, nbPoints))
            {
              doAnalyse = TRUE;
              x1 = pPa->XStart;
              y1 = pPa->YStart;
              x2 = pPa->XEnd;
              y2 = pPa->YEnd;
              pPa = pPa->PaNext;
              x3 = pPa->XEnd;
              y3 = pPa->YEnd;
              if(nbPoints == 4)
                {
                  pPa = pPa->PaNext;
                  x4 = pPa->XEnd;
                  y4 = pPa->YEnd;
                }
            }
        }
    }

  if(doAnalyse)
    {
      if(nbPoints == 3)
        {
          /* A triangle
           *                2
           *             .  /
           *         .     /
           *      .       /  
           *     1_______3
           */

          /* Is 1 a right angle? */
          if(AlmostOrthogonalVectors(x2 - x1, y2 - y1, x3 - x1, y3 - y1))
            shape = RECTANGLED_TRIANGLE;
          /* Is 2 a right angle? */
          else if(AlmostOrthogonalVectors(x3 - x2, y3 - y2,
                                          x1 - x2, y1 - y2))
            {
              CircularPermutationOnTriangle(&x1, &y1, &x2, &y2,
                                            &x3, &y3, +1);
              shape = RECTANGLED_TRIANGLE;
            }
          /* Is 3 a right angle? */
          else if(AlmostOrthogonalVectors(x2 - x3, y2 - y3,
                                          x1 - x3, y1 - y3))
            {
              CircularPermutationOnTriangle(&x1, &y1, &x2, &y2,
                                            &x3, &y3, -1);
              shape = RECTANGLED_TRIANGLE;
            }
          else
            {
              if(AlmostEqualAngle(x1, y1, x2, y2, x3, y3,
                                  x2, y2, x3, y3, x1, y1))
		
                shape = ISOSCELES_TRIANGLE;
              /* Is Angle(3,1,2) == Angle(1,2,3)? */
              else if(AlmostEqualAngle(x3, y3, x1, y1, x2, y2,
                                       x1, y1, x2, y2, x3, y3))
                {
                  CircularPermutationOnTriangle(&x1, &y1, &x2, &y2,
                                                &x3, &y3, -1);
                  shape = ISOSCELES_TRIANGLE;
                }
              /* Is Angle(3,1,2) == Angle(2,3,1)? */
              else if(AlmostEqualAngle(x3, y3, x1, y1, x2, y2,
                                       x2, y2, x3, y3, x1, y1))
                {
                  CircularPermutationOnTriangle(&x1, &y1, &x2, &y2,
                                                &x3, &y3, +1);
                  shape = ISOSCELES_TRIANGLE;
                }
	      
              if(shape == ISOSCELES_TRIANGLE)
                {
                  /*       /\
                   *      /  \
                   *     /    \
                   *    /      \
                   *   /________\
                   */

                  /* Is Angle(3,1,2) == Angle(1,2,3)? */
                  if(AlmostEqualAngle(x3, y3, x1, y1, x2, y2,
                                      x1, y1, x2, y2, x3, y3))
                    shape = EQUILATERAL_TRIANGLE;
                }	  
            }
        }
      else if(nbPoints == 4)
        {
          /* A quadrilateral
           *      4  
           *     /   \
           *    /      \
           *   1         \    
           *    \          \  
           *     \           \
           *      2----------3
           */

          /* Are edges (1-2) and (3-4) parallel?*/
          if(AlmostColinearVectors(x2 - x1, y2 - y1,
                                   x3 - x4, y3 - y4, TRUE))
            shape = TRAPEZIUM;
          /* Are edges (2-3) and (1-4) parallel?*/
          else if(AlmostColinearVectors(x3 - x2, y3 - y2,
                                        x4 - x1, y4 - y1, TRUE))
            {
              CircularPermutationOnQuadrilateral(&x1, &y1, &x2, &y2,
                                                 &x3, &y3, &x4, &y4);
              shape = TRAPEZIUM;
            }

          if(shape == TRAPEZIUM)
            {
              /*
               *
               *   1---------2
               *  /           \
               * /              \
               * 4---------------3
               */

              /* Is Angle(1,2,3) == Angle(3,4,1)? */
              if(AlmostEqualAngle(x1, y1, x2, y2, x3, y3,
                                  x3, y3, x4, y4, x1, y1))
                {
                  /*
                   *     1----------2 
                   *      \___    \__\
                   *       \  \       \
                   *        4----------3
                   */

                  shape = PARALLELOGRAM;

                  /* Is Angle(2,4,1) == Angle(1,2,4)? */
                  if(AlmostEqualAngle(x2, y2, x4, y4, x1, y1,
                                      x1, y1, x2, y2, x4, y4))
                    {
                      /*
                       *           1
                       *          /\  
                       *         /  \
                       *        /\  /\
                       *       / |  | \
                       *      /__|__|__\
                       *     4\        /2
                       *       \      /
                       *        \    /
                       *         \  /
                       *          \/
                       *          3
                       */
                      shape = DIAMOND;
                    }

                  if(AlmostOrthogonalVectors(x2 - x1, y2 - y1,
                                             x3 - x2, y3 - y2))
                    { /*
                       *    1-------2
                       *    |       |
                       *    |       |
                       *    4-------3
                       */
                      if(shape == DIAMOND)
                        shape = SQUARE;
                      else
                        shape = RECTANGLE;
                    }

                  if(shape == PARALLELOGRAM)
                    {
                      /* A parallelogram which is not a rectangle/diamond:
                         make (4,1,2) a obtuse */
                      if(IsAcuteAngle(x4, y4, x1,
                                      y1, x2, y2))
                        CircularPermutationOnQuadrilateral(&x1, &y1, &x2, &y2,
                                                           &x3, &y3, &x4, &y4);
                    }

                }
              /* A trapezium which is not a parallelogram:
                 make (1-2) smaller than (3-4) */
              else if(Norm(x4 - x3, y4 - y3) < Norm(x2 - x1, y2 - y1))
                {
                  CircularPermutationOnQuadrilateral(&x1, &y1, &x2, &y2,
                                                     &x3, &y3, &x4, &y4);
                  CircularPermutationOnQuadrilateral(&x1, &y1, &x2, &y2,
                                                     &x3, &y3, &x4, &y4);
                }
            }


        }
      
      /* Now we want to find a transform matrix from the local box to
         the current system of coordinates:

         (a  c  e)
         (b  d  f)
         (0  0  1)
	     
      */
      switch(shape)
        {
        case EQUILATERAL_TRIANGLE:
        case ISOSCELES_TRIANGLE:
          w = Norm(x2 - x3, y2 - y3);
          h = Norm(((double) x2+x3)/2 - x1, ((double) y2+y3)/2 - y1);
          if(w == 0 || h == 0)
            return FALSE;

          e = ((double) 2*x1 + x3 - x2)/2;
          f = ((double) 2*y1 + y3 - y2)/2;

          a = (x2 - x3)/w;
          b = (y2 - y3)/w;
          c = (x3 - e)/h;
          d = (y3 - f)/h;
          if(shape == EQUILATERAL_TRIANGLE)
            TtaSetGraphicsShape (leaf, 4, doc);
          else
            TtaSetGraphicsShape (leaf, 5, doc);
          break;

        case RECTANGLED_TRIANGLE:
          w = Norm(x2 - x1, y2 - y1);
          h = Norm(x3 - x1, y3 - y1);
          if(w == 0 || h == 0)
            return FALSE;

          a = (x2 - x1)/w;
          b = (y2 - y1)/w;
          c = (x3 - x1)/h;
          d = (y3 - y1)/h;
          e = x1;
          f = y1;

          TtaSetGraphicsShape (leaf, 6, doc);
          break;

        case TRAPEZIUM:
          /*  We want to determine the bounding box:
           *   
           *   (1_)--------(2_)
           *   |             |
           *   |             |
           *   (4_)--------(3_)
           */
          return FALSE;

          if(IsAcuteAngle(x4, y4, x1, y1, x2, y2))
            {
              /*   1-------2.
               *   @\         ..
               *   @ \           .
               *   @@@4-----------3
               */
              x1_ = x1;
              y1_ = y1;

              GiveIntersectionPoint(x1, y1, -(y2 - y1), x2 - x1, 
                                    x4, y4, x3 - x4, y3 - y4,
                                    &x4_, &y4_);
            }
          else
            {
              /*      @@@1--------2
               *      @ /         |
               *      @/          |
               *      4-----------3
               */
              x4_ = x4;
              y4_ = y4;

              GiveIntersectionPoint(x1, y1, x2 - x1, y2 - y1,
                                    x4, y4, -(y3 - y4), x3 - x4,
                                    &x1_, &y1_);
            }

          if(IsAcuteAngle(x1, y1, x2, y2, x3, y3))
            {
              /*   1---------------2 
               *    \             /@
               *     \           / @
               *      4---------3@@@
               */
              x2_ = x2;
              y2_ = y2;

              GiveIntersectionPoint(x2, y2, -(y2 - y1), x2 - x1,
                                    x3, y3, x3 - x4, y3 - y4,
                                    &x3_, &y3_);
            }
          else
            {
              /*         1--------2@@@
               *        /          \ @
               *       /            \@
               *      4--------------3
               */
              x3_ = x3;
              y3_ = y3;

              GiveIntersectionPoint(x2, y2, x2 - x1, y2 - y1,
                                    x3, y3, -(y3 - y4), x3 - x4,
                                    &x2_, &y2_);
            }

          w = Norm(x2_ - x1_, y2_ - y1_);
          h = Norm(x4_ - x1_, y4_ - y1_);
          if(w == 0 || h == 0)
            return FALSE;

          a = (x2_ - x1_)/w;
          b = (y2_ - y1_)/w;
          c = (x4_ - x1_)/h;
          d = (y4_ - y1_)/h;
          e = x1_;
          f = y1_;
          break;

        case PARALLELOGRAM:
          return FALSE;
          x4_ = x4;
          y4_ = y4;

          GiveIntersectionPoint(x1, y1, x2 - x1, y2 - y1,
                                x4, y4, -(y3 - y4), x3 - x4,
                                &x1_, &y1_);

          x2_ = x2;
          y2_ = y2;

          GiveIntersectionPoint(x2, y2, -(y2 - y1), x2 - x1,
                                x3, y3, x3 - x4, y3 - y4,
                                &x3_, &y3_);

          w = Norm(x2_ - x1_, y2_ - y1_);
          h = Norm(x4_ - x1_, y4_ - y1_);
          if(w == 0 || h == 0)
            return FALSE;

          a = (x2_ - x1_)/w;
          b = (y2_ - y1_)/w;
          c = (x4_ - x1_)/h;
          d = (y4_ - y1_)/h;
          e = x1_;
          f = y1_;
          TtaSetGraphicsShape (leaf, 2, doc);
          *rx = (int)(x1 - x1_);
          break;

        case DIAMOND:
          w = Norm(x4 - x2, y4 - y2);
          h = Norm(x3 - x1, y3 - y1);
          if(w == 0 || h == 0)
            return FALSE;

          a = (x2 - x4)/w;
          b = (y2 - y4)/w;
          c = (x3 - x1)/h;
          d = (y3 - y1)/h;
          e = x1 - a*w/2;
          f = y1 - b*w/2;

          TtaSetGraphicsShape (leaf, 'L', doc);
          break;
        case RECTANGLE:
        case SQUARE:
          w = Norm(x2 - x1, y2 - y1);
          h = Norm(x4 - x1, y4 - y1);
          if(w == 0 || h == 0)
            return FALSE;

          a = (x2 - x1)/w;
          b = (y2 - y1)/w;
          c = (x4 - x1)/h;
          d = (y4 - y1)/h;
          e = x1;
          f = y1;
          if(shape == SQUARE)
            TtaSetGraphicsShape (leaf, 7, doc);
          else
            TtaSetGraphicsShape (leaf, 8, doc);
          break;
        }

      if(shape != -1)
        {
          TtaAppendTransform (TtaGetParent(leaf),
                              TtaNewTransformMatrix((float)a, (float)b, (float)c, (float)d, (float)e, (float)f));
          *width = (int)(w);
          *height = (int)(h);
          return TRUE;
        }
    }

  return FALSE;
}
