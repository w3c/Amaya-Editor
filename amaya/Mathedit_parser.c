/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: F. Wang
 */         
#include <stdarg.h>

#undef THOT_EXPORT
#define THOT_EXPORT extern

#include "amaya.h"
#include "undo.h"

#include "fetchXMLname_f.h"

// TODO: share this with Mathedit
enum { DEFAULT_MODE, CHEMISTRY_MODE, UNITS_MODE, LATEX_MODE};

static Document parser_doc;
static Element parser_el;
static Element parser_new_el;

/*----------------------------------------------------------------------
  NewMathElement

  Create a new MathML element with a text content
  -----------------------------------------------------------------------*/
static Element NewMathElement(Document doc, int elTypeNum, const char *string)
{
  ElementType elType;
  Element newEl, textUnit;

  elType.ElSSchema = GetMathMLSSchema (doc);
  elType.ElTypeNum = elTypeNum;
  newEl = TtaNewElement(doc, elType);

  if(string != NULL)
    {
      elType.ElTypeNum = MathML_EL_TEXT_UNIT;
      textUnit = TtaNewElement(doc, elType);
      TtaInsertFirstChild (&textUnit, newEl, doc);
      TtaInsertTextContent (textUnit, 0, (unsigned char *)string, doc);
    }

  return newEl;
  }

/*----------------------------------------------------------------------
  NewSymbol

  Create a new MathML element with a CHAR_T symbol inside
  -----------------------------------------------------------------------*/
static Element NewSymbol(Document doc, int elTypeNum, CHAR_T symbol)
{
  ElementType elType;
  Element newEl, textUnit;
  CHAR_T text[2];

  elType.ElSSchema = GetMathMLSSchema (doc);
  elType.ElTypeNum = elTypeNum;
  newEl = TtaNewElement(doc, elType);

  elType.ElTypeNum = MathML_EL_TEXT_UNIT;
  textUnit = TtaNewElement(doc, elType);
  TtaInsertFirstChild (&textUnit, newEl, doc);
  text[0] = symbol;
  text[1] = EOS;
  TtaSetBufferContent (textUnit, text, TtaGetLanguageIdFromScript('L'), doc);

  return newEl;
}

/*----------------------------------------------------------------------
  NewMROW

  Create an empty mrow
  -----------------------------------------------------------------------*/
static Element NewMROW(Document doc)
{
  ElementType elType;
  Element mrow;

  elType.ElSSchema = GetMathMLSSchema (doc);
  elType.ElTypeNum = MathML_EL_MROW;
  mrow = TtaNewTree(doc, elType, "");
  TtaDeleteTree(TtaGetFirstChild(mrow), doc);
  return mrow;
}

/*----------------------------------------------------------------------
  NewFencedExpression

  Create an mrow with open and close MO's.
  -----------------------------------------------------------------------*/
static Element NewFencedExpression(Document doc, Element el, const char *open,
				   const char *close)
{
  Element fenced;
  Element open_, close_;
  fenced = NewMROW(doc);
  open_ = NewMathElement(doc, MathML_EL_MO, open);
  close_ = NewMathElement(doc, MathML_EL_MO, close);
  TtaInsertFirstChild(&open_, fenced, doc);
  TtaInsertSibling(el, open_, FALSE, doc);
  TtaInsertSibling(close_, el, FALSE, doc);
  return fenced;
}

/*----------------------------------------------------------------------
  NewMSUP

  Create a new msup with base and supscript children
  -----------------------------------------------------------------------*/
static Element NewMSUP(Document doc, Element base, Element supscript)
{
  ElementType elType;
  Element msup_, base_, supscript_;

  elType.ElSSchema = GetMathMLSSchema (doc);
  elType.ElTypeNum = MathML_EL_MSUP;
  msup_ = TtaNewTree(doc, elType, "");

  base_ = TtaGetFirstChild(msup_);
  supscript_ = base_;
  TtaNextSibling(&supscript_);

  TtaDeleteTree(TtaGetFirstChild(base_), doc);
  TtaInsertFirstChild(&base, base_, doc);
  TtaDeleteTree(TtaGetFirstChild(supscript_), doc);
  TtaInsertFirstChild(&supscript, supscript_, doc);

  return msup_;
}

/*----------------------------------------------------------------------
  NewMSUB

  Create a new msup with base and subscript children
  -----------------------------------------------------------------------*/
static Element NewMSUB(Document doc, Element base, Element subscript)
{
  ElementType elType;
  Element msub_, base_, subscript_;

  elType.ElSSchema = GetMathMLSSchema (doc);
  elType.ElTypeNum = MathML_EL_MSUB;
  msub_ = TtaNewTree(doc, elType, "");

  base_ = TtaGetFirstChild(msub_);
  subscript_ = base_;
  TtaNextSibling(&subscript_);

  TtaDeleteTree(TtaGetFirstChild(base_), doc);
  TtaInsertFirstChild(&base, base_, doc);
  TtaDeleteTree(TtaGetFirstChild(subscript_), doc);
  TtaInsertFirstChild(&subscript, subscript_, doc);

  return msub_;
}

#ifndef __CEXTRACT__
#include "mathedit/chemistry.bison.c"
#endif

/*----------------------------------------------------------------------
  InsertMathElementFromText
  -----------------------------------------------------------------------*/
void InsertMathElementFromText(Element theElem, Element theText, Document doc,
			       int mode)
{
  ElementType   elType;
#define TXTBUFLEN 100
  CHAR_T        text[TXTBUFLEN];
  int len;
  char               *iso;
  Language	lang;

  elType = TtaGetElementType(theText);
  if (elType.ElTypeNum != MathML_EL_TEXT_UNIT)
    return;

  len = TtaGetElementVolume (theText);
  if (len <= 0)
    return;

  len = TXTBUFLEN;
  TtaGiveBufferContent (theText, text, len, &lang);
  iso = (char *)TtaConvertCHARToByte (text, ISO_8859_1);
  if(iso == NULL)
    return;

  printf("*********\nstring=%s\n", iso);
  parser_doc = doc;
  parser_el = theElem;
  parser_new_el = NULL;

  elType.ElTypeNum = MathML_EL_MROW;

  switch(mode)
    {
    case CHEMISTRY_MODE:
      chemistry_delete_buffer( YY_CURRENT_BUFFER );
      chemistry_scan_string(iso);
      chemistryparse();
      break;

    case DEFAULT_MODE:
    case UNITS_MODE:
    case LATEX_MODE:
    default:
      break;
    }

  TtaFreeMemory (iso);

  if(parser_new_el != NULL)
    {
      TtaInsertSibling(parser_new_el, parser_el, FALSE, parser_doc);
      TtaRegisterElementCreate (parser_new_el, parser_doc);
      TtaRegisterElementDelete (parser_el, parser_doc);
      TtaDeleteTree(parser_el, parser_doc);
    }

  printf("*********\n");
  return;
}
