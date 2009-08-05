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
#include <fstream>    
#include <iostream> 
#include<sstream> 

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "undo.h"
#include "mathedit.h"
#include "fetchXMLname_f.h"

// #include "chemistry"

// extern int chemistryparse(quex::chemistry  *qlex);

Document parser_doc;
Element parser_el;
Element parser_new_el;
extern int CurrentMathEditMode;


/*----------------------------------------------------------------------
  AttachClassName
  -----------------------------------------------------------------------*/
static void AttachClassName(Document doc, Element el)
{
  AttributeType attrType;
  Attribute attr;

  if(CurrentMathEditMode != CHEMISTRY_MODE &&
     CurrentMathEditMode != UNITS_MODE)
    return;
    
  attrType.AttrSSchema = GetMathMLSSchema (doc);
  attrType.AttrTypeNum = MathML_ATTR_class;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (el, attr, doc);

  switch(CurrentMathEditMode)
    {
    case CHEMISTRY_MODE:
      TtaSetAttributeText (attr, "chem", el, doc);
      break;

    case UNITS_MODE:
      TtaSetAttributeText (attr, "MathML-Unit", el, doc);
      break;
    }
}

/*----------------------------------------------------------------------
  NewMathElement

  Create a new MathML element with a text content
  -----------------------------------------------------------------------*/
Element NewMathElement(Document doc, int elTypeNum, const char *string)
{
  ElementType elType;
  Element newEl, textUnit;

  elType.ElSSchema = GetMathMLSSchema (doc);
  elType.ElTypeNum = elTypeNum;
  newEl = TtaNewElement(doc, elType);

  if(string != NULL)
    {
      /* String is not empty: insert the corresponding text unit */
      elType.ElTypeNum = MathML_EL_TEXT_UNIT;
      textUnit = TtaNewElement(doc, elType);
      TtaInsertFirstChild (&textUnit, newEl, doc);
      // XXXfw should use TtaSetBufferContent here
      TtaInsertTextContent (textUnit, 0, (unsigned char *)string, doc);
    }

  if(elTypeNum == MathML_EL_MI)
      /* It's an <mi/> element add a class element according to the
	 current mode*/
      AttachClassName(doc, newEl);

  return newEl;
  }

/*----------------------------------------------------------------------
  NewSymbol

  Create a new MathML element with a CHAR_T symbol inside
  -----------------------------------------------------------------------*/
Element NewSymbol(Document doc, int elTypeNum, CHAR_T symbol)
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
  // XXXfw should we really use TtaGetLanguageIdFromScript('L')?
  TtaSetBufferContent (textUnit, text, TtaGetLanguageIdFromScript('L'), doc);

  return newEl;
}

/*----------------------------------------------------------------------
  NewMROW

  Create an empty mrow
  -----------------------------------------------------------------------*/
Element NewMROW(Document doc)
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
// XXXfw use CHAR_T for open and close
Element NewFencedExpression(Document doc, Element el, const char *open,
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
Element NewMSUP(Document doc, Element base, Element supscript)
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
Element NewMSUB(Document doc, Element base, Element subscript)
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

/*----------------------------------------------------------------------
  InsertMathElementFromText
  -----------------------------------------------------------------------*/
Element InsertMathElementFromText(Element theElem, Element theText,
				  Document doc)
{
  ElementType   elType;
  int len;
  Language	lang;

  // XXXfw: for the moment disable the experimental MathML parsing mode 
  return NULL;

  elType = TtaGetElementType(theText);
  if (elType.ElTypeNum != MathML_EL_TEXT_UNIT)
    return NULL;

  len = TtaGetElementVolume (theText);
  if (len <= 0)
    return NULL;

  CHAR_T *text = new CHAR_T[len+2];
  TtaGiveBufferContent (theText, text+1, len+1, &lang);

  parser_doc = doc;
  parser_el = theElem;
  parser_new_el = NULL;

  elType.ElTypeNum = MathML_EL_MROW;

  switch(CurrentMathEditMode)
    {
    case CHEMISTRY_MODE:
      {
	// quex::chemistry qlex(text, len+1);
	// qlex.buffer_fill_region_finish(len);
	// chemistryparse(&qlex);
      }
      break;

    case DEFAULT_MODE:
    case UNITS_MODE:
    default:
      break;
    }

  delete[] text;

  if(parser_new_el != NULL)
    {
      TtaInsertSibling(parser_new_el, parser_el, FALSE, parser_doc);
      TtaRegisterElementCreate (parser_new_el, parser_doc);
      TtaRegisterElementDelete (parser_el, parser_doc);
      TtaDeleteTree(parser_el, parser_doc);

      return parser_new_el;
    }

  return NULL;
}
