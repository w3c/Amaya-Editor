/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Set of function common to presentation drivers. It offers the
 * translators between the internal Presentation Rules coding of
 * presentation attributes and the PresentationValue or
 * PresentationSetting equivalents available at the driver API level.
 *
 * Author: D. Veillard (INRIA)
 *
 */

#include "thot_sys.h"
#include "message.h"
#include "conststr.h"
#include "typestr.h"
#include "constprs.h"
#include "typeprs.h"
#include "pschema.h"
#include "application.h"

#include "presentdriver.h"

/*----------------------------------------------------------------------
 PresentationValueToPRule : set up an internal Presentation Rule accordingly
 to a Presentation Value for a given type of presentation attribute.
 specific is an extra parameter needed when using a Function rule.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
void                PresentationValueToPRule (PresentationValue val, int type, PRule pRule,
					      int specific)
#else
void                PresentationValueToPRule (val, type, pRule, specific)
PresentationValue   val;
int                 type;
PRule               pRule;
int                 specific;
#endif
{
  int                 value;
  int                 unit;
  TypeUnit            int_unit;
  int                 real;
  PtrPRule            rule = (PtrPRule) pRule;

  value = val.typed_data.value;
  unit = val.typed_data.unit;
  /* The drivers affect only the main "WYSIWYG" view */
  rule->PrViewNum = 1;
  /*
   * normalize the unit to fit the Thot internal ones.
   * The driver interface accept floats with up to 3 digits
   * after the dot, e.g. 1.115 , coded as :
   *      unit = unit | DRIVERP_UNIT_FLOAT
   *      val = 1115
   */
  if (DRIVERP_UNIT_IS_FLOAT (unit))
    {
      DRIVERP_UNIT_UNSET_FLOAT (unit);
      real = TRUE;
    }
  else
    real = FALSE;

  switch (unit)
    {
    case DRIVERP_UNIT_REL:
      int_unit = UnRelative;
      if (type == PtBreak1 ||
	  type == PtBreak2 ||
	  type == PtIndent ||
	  type == PtLineSpacing ||
	  type == PtLineWeight)
	value *= 10;
      break;
    case DRIVERP_UNIT_EM:
      int_unit = UnRelative;
      value *= 10;
      break;
    case DRIVERP_UNIT_PT:
      int_unit = UnPoint;
      break;
    case DRIVERP_UNIT_PC:
      int_unit = UnPoint;
      value *= 12;
      break;
    case DRIVERP_UNIT_IN:
      int_unit = UnPoint;
      value *= 72;
      break;
    case DRIVERP_UNIT_CM:
      int_unit = UnPoint;
      value *= 28;
      break;
    case DRIVERP_UNIT_MM:
      int_unit = UnPoint;
      value *= 28;
      value /= 10;
      break;
    case DRIVERP_UNIT_PX:
      int_unit = UnPixel;
      break;
    case DRIVERP_UNIT_PERCENT:
      int_unit = UnPercent;
      break;
    case DRIVERP_UNIT_XHEIGHT:
      int_unit = UnXHeight;
      value *= 10;
      break;
    case DRIVERP_UNIT_BOX:
      int_unit = UnRelative;	/* unused */
      break;
    default:
      int_unit = UnRelative;
      break;
    }

  if (real)
    value /= 1000;

  /* now, set-up the value */
  switch (type)
    {
    case PtVisibility:
    case PtDepth:
    case PtFillPattern:
    case PtBackground:
    case PtForeground:
      break;
    case PtFont:
    case PtStyle:
    case PtUnderline:
    case PtThickness:
    case PtLineStyle:
      break;
    case PtBreak1:
    case PtBreak2:
    case PtIndent:
    case PtSize:
    case PtLineSpacing:
    case PtLineWeight:
      rule->PrMinUnit = int_unit;
      rule->PrMinValue = value;
      rule->PrMinAttr = FALSE;
      break;
    case PtVertRef:
    case PtHorizRef:
    case PtVertPos:
    case PtHorizPos:
      rule->PrPosRule.PoDistUnit = int_unit;
      rule->PrPosRule.PoDistance = value;
      break;
    case PtHeight:
    case PtWidth:
      rule->PrDimRule.DrUnit = int_unit;
      rule->PrDimRule.DrValue = value;
      break;
    case PtJustify:
      break;
    case PtAdjust:
      break;
    case PtPictInfo:
      break;
    }

  /*
   * Fill up all the other fields.
   * If this is not a value but a predefined constant, translate
   * between external and internal value.
   */
  rule->PrType = (PRuleType) type;
  switch (type)
    {
    case PtVertPos:
      rule->PrPresMode = PresImmediate;
      rule->PrPosRule.PoPosRef = Bottom;
      rule->PrPosRule.PoPosDef = Top;
      rule->PrPosRule.PoDistAttr = FALSE;
      rule->PrPosRule.PoRelation = RlPrevious;
      rule->PrPosRule.PoNotRel = FALSE;
      rule->PrPosRule.PoUserSpecified = FALSE;
      rule->PrPosRule.PoRefKind = RkPresBox;
      rule->PrPosRule.PoRefIdent = 0;
      break;
    case PtHeight:
    case PtWidth:
      rule->PrDimRule.DrPosition = FALSE;
      rule->PrDimRule.DrAbsolute = TRUE;
      break;
    case PtFont:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case DRIVERP_FONT_HELVETICA:
	  rule->PrChrValue = 'H';
	  break;
	case DRIVERP_FONT_TIMES:
	  rule->PrChrValue = 'T';
	  break;
	case DRIVERP_FONT_COURIER:
	  rule->PrChrValue = 'C';
	  break;
	}
      break;
    case PtStyle:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case StyleBold:
	  rule->PrChrValue = 'B';
	  break;
	case StyleRoman:
	  rule->PrChrValue = 'R';
	  break;
	case StyleItalics:
	  rule->PrChrValue = 'I';
	  break;
	case StyleBoldItalics:
	  rule->PrChrValue = 'G';
	  break;
	case StyleOblique:
	  rule->PrChrValue = 'O';
	  break;
	case StyleBoldOblique:
	  rule->PrChrValue = 'Q';
	  break;
	}
      break;
    case PtUnderline:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case Underline:
	  rule->PrChrValue = 'U';
	  break;
	case Overline:
	  rule->PrChrValue = 'O';
	  break;
	case CrossOut:
	  rule->PrChrValue = 'C';
	  break;
	}
      break;
    case PtAdjust:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case DRIVERP_ADJUSTLEFT:
	  rule->PrAdjust = AlignLeft;
	  break;
	case DRIVERP_ADJUSTRIGHT:
	  rule->PrAdjust = AlignRight;
	  break;
	case DRIVERP_ADJUSTCENTERED:
	  rule->PrAdjust = AlignCenter;
	  break;
	case DRIVERP_ADJUSTLEFTWITHDOTS:
	  rule->PrAdjust = AlignLeftDots;
	  break;
	default:
	  rule->PrAdjust = AlignLeft;
	  break;
	}
      break;
    case PtJustify:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case DRIVERP_JUSTIFIED:
	  rule->PrJustify = TRUE;
	  break;
	case DRIVERP_NOTJUSTIFIED:
	  rule->PrJustify = FALSE;
	  break;
	}
      break;
    case PtHyphenate:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case DRIVERP_HYPHENATE:
	  rule->PrJustify = TRUE;
	  break;
	case DRIVERP_NOHYPHENATE:
	  rule->PrJustify = FALSE;
	  break;
	}
      break;
    case PtIndent:
    case PtBreak1:
    case PtBreak2:
    case PtLineSpacing:
    case PtLineWeight:
      rule->PrPresMode = PresImmediate;
      rule->PrMinAttr = 0;
      rule->PrMinUnit = int_unit;
      rule->PrMinValue = value;
      break;
    case PtSize:
      if (real && int_unit == UnRelative)
	{
	  int_unit = UnPercent;
	  value *= 10;
	}
      rule->PrMinUnit = int_unit;
      rule->PrPresMode = PresImmediate;
      rule->PrMinAttr = 0;
      rule->PrMinValue = value;
      break;
    case PtVisibility:
    case PtDepth:
    case PtFillPattern:
    case PtBackground:
    case PtForeground:
      rule->PrPresMode = PresImmediate;
      rule->PrAttrValue = 0;
      rule->PrIntValue = value;
      break;
    case PtFunction:
      rule->PrPresMode = PresFunction;
      switch (specific)
	{
	case FnLine:
	  if (value == DRIVERP_NOTINLINE)
	    rule->PrPresFunction = FnNoLine;
	  else
	    rule->PrPresFunction = FnLine;
	  rule->PrNPresBoxes = 0;
	  break;
	case FnCreateBefore:
	case FnCreateWith:
	case FnCreateFirst:
	case FnCreateLast:
	case FnCreateAfter:
	case FnCreateEnclosing:
	  rule->PrPresFunction = (FunctionType) specific;
	  rule->PrNPresBoxes = 0;
	  rule->PrElement = TRUE;
	  break;
	case FnShowBox:
	  rule->PrPresFunction = (FunctionType) specific;
	  rule->PrNPresBoxes = value;
	  break;
	case FnBackgroundPicture:
	  rule->PrPresFunction = (FunctionType) specific;
	  rule->PrNPresBoxes = 1;
	  rule->PrPresBox[0] = value;
	  break;
	case FnPictureMode:
	  rule->PrPresFunction = (FunctionType) specific;
	  rule->PrNPresBoxes = 1;
	  switch (value)
	    {
	    case DRIVERP_REALSIZE:
	      rule->PrPresBox[0] = RealSize;
	      break;
	    case DRIVERP_SCALE:
	      rule->PrPresBox[0] = ReScale;
	      break;
	    case DRIVERP_REPEAT:
	      rule->PrPresBox[0] = FillFrame;
	      break;
	    case DRIVERP_HREPEAT:
	      rule->PrPresBox[0] = XRepeat; break;
	    case DRIVERP_VREPEAT:
	      rule->PrPresBox[0] = YRepeat; break;
	    default:
	      rule->PrPresBox[0] = RealSize;
	    }
	  break;
	default:
	  fprintf (stderr, "Presentation GenericDriver : unsupported PtFunction %d\n", specific);
	}
      break;
    default:
      fprintf (stderr, "PresentationValueToPRule : unsupported PrType %d\n", rule->PrType);
    }
}

/*----------------------------------------------------------------------
  PRuleToPresentationValue : return the PresentationValue corresponding to
  a given PRule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PresentationValue   PRuleToPresentationValue (PRule pRule)
#else
PresentationValue   PRuleToPresentationValue (pRule)
PRule               pRule;
#endif
{
  PresentationValue   val;
  int                 value = 0;
  int                 unit = -1;
  TypeUnit            int_unit = -1;
  PtrPRule            rule = (PtrPRule) pRule;

  /* read the value */
  switch (rule->PrType)
    {
    case PtVisibility:
    case PtDepth:
    case PtFillPattern:
    case PtBackground:
    case PtForeground:
      value = rule->PrIntValue;
      break;
    case PtFont:
    case PtStyle:
    case PtUnderline:
    case PtThickness:
    case PtLineStyle:
      break;
    case PtBreak1:
    case PtBreak2:
    case PtIndent:
    case PtSize:
    case PtLineSpacing:
    case PtLineWeight:
      int_unit = rule->PrMinUnit;
      value = rule->PrMinValue;
      break;
    case PtVertRef:
    case PtHorizRef:
    case PtVertPos:
    case PtHorizPos:
      int_unit = rule->PrPosRule.PoDistUnit;
      value = rule->PrPosRule.PoDistance;
      break;
    case PtHeight:
    case PtWidth:
      int_unit = rule->PrDimRule.DrUnit;
      if (int_unit == 0)
	int_unit = UnPixel;

      value = rule->PrDimRule.DrValue;
      break;
    case PtJustify:
    case PtAdjust:
    case PtPictInfo:
    case PtFunction:
    case PtHyphenate:
    case PtVertOverflow:
    case PtHorizOverflow:
      break;
    }

  /* translate to external units */
  switch (int_unit)
    {
    case UnRelative:
      switch (rule->PrType)
	{
	case PtBreak1:
	case PtBreak2:
	case PtIndent:
	case PtSize:
	case PtLineSpacing:
	case PtLineWeight:
	  unit = DRIVERP_UNIT_EM;
	  break;
	default:
	  unit = DRIVERP_UNIT_REL;
	  break;
	}

      if (value % 10)
	{
	  DRIVERP_UNIT_SET_FLOAT (unit);
	  value *= 100;
	}
      else
	value /= 10;
      break;
    case UnXHeight:
      unit = DRIVERP_UNIT_XHEIGHT;
      if (value % 10)
	{
	  DRIVERP_UNIT_SET_FLOAT (unit);
	  value *= 100;
	}
      else
	value /= 10;
      break;
    case UnPoint:
      unit = DRIVERP_UNIT_PT;
      break;
    case UnPixel:
      unit = DRIVERP_UNIT_PX;
      break;
    case UnPercent:
      unit = DRIVERP_UNIT_PERCENT;
      break;
    default:
      unit = DRIVERP_UNIT_INVALID;
      break;
    }

  /* Specific case for converting between internal and external value */
  switch (rule->PrType)
    {
    case PtFont:
      switch (rule->PrChrValue)
	{
	case 'H':
	  value = DRIVERP_FONT_HELVETICA;
	  break;
	case 'T':
	  value = DRIVERP_FONT_TIMES;
	  break;
	case 'C':
	  value = DRIVERP_FONT_COURIER;
	  break;
	}
      break;
    case PtStyle:
      switch (rule->PrChrValue)
	{
	case 'B':
	  value = DRIVERP_FONT_BOLD;
	  break;
	case 'R':
	  value = DRIVERP_FONT_ROMAN;
	  break;
	case 'I':
	  value = DRIVERP_FONT_ITALICS;
	  break;
	case 'G':
	  value = DRIVERP_FONT_BOLDITALICS;
	  break;
	case 'O':
	  value = DRIVERP_FONT_OBLIQUE;
	  break;
	case 'Q':
	  value = DRIVERP_FONT_BOLDOBLIQUE;
	  break;
	}
      break;
    case PtUnderline:
      switch (rule->PrChrValue)
	{
	case 'U':
	  value = DRIVERP_UNDERLINE;
	  break;
	case 'O':
	  value = DRIVERP_OVERLINE;
	  break;
	case 'C':
	  value = DRIVERP_CROSSOUT;
	  break;
	}
      break;
    case PtAdjust:
      switch (rule->PrAdjust)
	{
	case AlignLeft:
	  value = DRIVERP_ADJUSTLEFT;
	  break;
	case AlignRight:
	  value = DRIVERP_ADJUSTRIGHT;
	  break;
	case AlignCenter:
	  value = DRIVERP_ADJUSTCENTERED;
	  break;
	case AlignLeftDots:
	  value = DRIVERP_ADJUSTLEFTWITHDOTS;
	  break;
	default:
	  value = DRIVERP_ADJUSTLEFT;
	  break;
	}
      break;
    case PtJustify:
      if (rule->PrJustify)
	value = DRIVERP_JUSTIFIED;
      else
	value = DRIVERP_NOTJUSTIFIED;
      break;
    case PtHyphenate:
      if (rule->PrJustify)
	value = DRIVERP_HYPHENATE;
      else
	value = DRIVERP_NOHYPHENATE;
      break;
    case PtFunction:
      switch (rule->PrPresFunction)
	{
	case FnLine:
	  value = DRIVERP_INLINE;
	  break;
	case FnNoLine:
	  value = DRIVERP_NOTINLINE;
	  break;
	case FnCreateBefore:
	case FnCreateWith:
	case FnCreateFirst:
	case FnCreateLast:
	case FnCreateAfter:
	case FnCreateEnclosing:
	  value = rule->PrNPresBoxes;
	  unit = DRIVERP_UNIT_BOX;
	  break;
	case FnShowBox:
	  value = rule->PrNPresBoxes;
	  unit = DRIVERP_UNIT_REL;
	  break;
	case FnBackgroundPicture:
	  value = rule->PrPresBox[0];
	  unit = DRIVERP_UNIT_REL;
	  break;
	case FnPictureMode:
	  unit = DRIVERP_UNIT_REL;
	  value = DRIVERP_REALSIZE;
	  switch (rule->PrPresBox[0])
	    {
	    case RealSize:
	      value = DRIVERP_REALSIZE; break;
	    case ReScale:
	      value = DRIVERP_SCALE; break;
	    case FillFrame:
	      value = DRIVERP_REPEAT; break;
	    case XRepeat:
	      value = DRIVERP_HREPEAT; break;
	    case YRepeat:
	      value = DRIVERP_VREPEAT; break;
	    default:
	      unit = DRIVERP_UNIT_INVALID;
	      value = 0;
	    }
	  break;
	default:
	  fprintf (stderr, "Presentation GenericDriver : unsupported PtFunction %d\n", rule->PrPresFunction);
	}
      break;
    default:
      break;
    }
  val.typed_data.value = value;
  val.typed_data.unit = unit;
  return (val);
}

/*----------------------------------------------------------------------
  PRuleToPresentationSetting : Translate the internal values stored
  in a PRule to a valid PresentationSetting.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void   PRuleToPresentationSetting (PRule pRule,
         PresentationSetting setting, int extra)
#else
void   PRuleToPresentationSetting (pRule, setting, extra)
PRule               pRule;
PresentationSetting setting;
int                 extra;
#endif
{
  PtrPRule          rule = (PtrPRule) pRule;

  /* first decoding step : analyze the type of the rule */
  switch (rule->PrType)
    {
    case PtVisibility:
      setting->type = DRIVERP_SHOW;
      break;
    case PtFunction:
      switch (extra)
	{
	case FnLine:
	  setting->type = DRIVERP_IN_LINE;
	  break;
	case FnShowBox:
	  setting->type = DRIVERP_SHOWBOX;
	  break;
	case FnBackgroundPicture:
	  setting->type = DRIVERP_BGIMAGE;
	  break;
	case FnPictureMode:
	  setting->type = DRIVERP_PICTUREMODE;
	  break;
	default:
	  /* not yet supported by the driver */
	  setting->type = DRIVERP_NONE;
	  return;
	}
      break;
    case PtSize:
      setting->type = DRIVERP_FONT_SIZE;
      break;
    case PtStyle:
      setting->type = DRIVERP_FONT_STYLE;
      break;
    case PtFont:
      setting->type = DRIVERP_FONT_FAMILY;
      break;
    case PtUnderline:
      setting->type = DRIVERP_TEXT_UNDERLINING;
      break;
    case PtIndent:
      setting->type = DRIVERP_INDENT;
      break;
    case PtLineSpacing:
      setting->type = DRIVERP_LINE_SPACING;
      break;
    case PtAdjust:
      setting->type = DRIVERP_ALIGNMENT;
      break;
    case PtJustify:
      setting->type = DRIVERP_JUSTIFICATION;
      break;
    case PtFillPattern:
      setting->type = DRIVERP_FILL_PATTERN;
      break;
    case PtBackground:
      setting->type = DRIVERP_BACKGROUND_COLOR;
      break;
    case PtForeground:
      setting->type = DRIVERP_FOREGROUND_COLOR;
      break;
    case PtHyphenate:
      setting->type = DRIVERP_HYPHENATION;
      break;
    case PtVertPos:
      setting->type = DRIVERP_VERTICAL_POSITION;
      break;
    case PtHorizPos:
      setting->type = DRIVERP_HORIZONTAL_POSITION;
      break;
    case PtHeight:
      setting->type = DRIVERP_HEIGHT;
      break;
    case PtWidth:
      setting->type = DRIVERP_WIDTH;
      break;
    default:
      /* not yet supported by the driver */
      setting->type = DRIVERP_NONE;
      return;
    }
  
  /* second decoding step : read the value contained */
  setting->value = PRuleToPresentationValue ((PRule) rule);
}

/*----------------------------------------------------------------------
  PresConstInsert : add a constant to the constant array of a
  Presentation Schema and returns the associated index.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int PresConstInsert (PSchema tcsh, char *value)
#else  /* __STDC__ */
int PresConstInsert (doc, value)
PSchema             tcsh;
char               *value;
#endif /* !__STDC__ */
{
  PtrPSchema pSchemaPrs = (PtrPSchema) tcsh;
  int i;

  if (pSchemaPrs == NULL || value == NULL)
    return (-1);

  /* lookup the existing constants, searching for a corresponding entry */
  for (i = 0; i < pSchemaPrs->PsNConstants; i++)
    {
      if (pSchemaPrs->PsConstant[i].PdType == CharString &&
	  !strncmp (value, pSchemaPrs->PsConstant[i].PdString, MAX_PRES_CONST_LEN))
	return (i+1);
    }

  /* if not found, try to add it at the end */
  if (pSchemaPrs->PsNConstants >= MAX_PRES_CONST)
    return (-1);
  i = pSchemaPrs->PsNConstants;
  pSchemaPrs->PsConstant[i].PdType = CharString;
  pSchemaPrs->PsConstant[i].PdAlphabet = 'L';
  strncpy (&pSchemaPrs->PsConstant[i].PdString[0], value, MAX_PRES_CONST_LEN);
  pSchemaPrs->PsNConstants++;
  return(i+1);
}

