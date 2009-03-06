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

static Document parser_doc;
static Element parser_el;
static Element parser_new_el;

static Element ParserNewMROW(Document doc)
{
  ElementType elType;
  Element mrow;

  elType.ElSSchema = GetMathMLSSchema (parser_doc);
  elType.ElTypeNum = MathML_EL_MROW;
  mrow = TtaNewTree(parser_doc, elType, "");
  TtaDeleteTree(TtaGetFirstChild(mrow), doc);
  return mrow;
}

static Element ParserNewMSUP(Document doc, Element base, Element supscript)
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

static Element ParserNewMSUB(Document doc, Element base, Element subscript)
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

static Element ParserNewMathElement(Document doc, const char *string,
				    int elTypeNum)
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

#include "chemistry.tab.c"

void InsertMathElementFromText(Element theElem, Element theText, Document doc)
{
  ElementType   elType;
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
  yy_delete_buffer( YY_CURRENT_BUFFER );
  yy_scan_string(iso);

  parser_doc = doc;
  parser_el = theElem;
  parser_new_el = NULL;

  elType.ElTypeNum = MathML_EL_MROW;

  switch(CurrentMathEditMode)
    {
    case CHEMISTRY_MODE:
      yyparse();
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
