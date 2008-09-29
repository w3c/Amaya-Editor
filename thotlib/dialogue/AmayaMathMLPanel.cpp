/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/utils.h"
#include "wx/dcmemory.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"
#include "panel.h"
#include "editcommands_f.h"
#include "units_f.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"
#include "registry_wx.h"

#include "AmayaMathMLPanel.h"
#include "AmayaNormalWindow.h"
#include "displayview_f.h"


/**
 * Defines entries for Trigo panel.
 */
#ifdef _MACOS
#define MAX_TOOL_PER_LINE 6
const char *TrigoTable[] = 
  {
    "sin", "cos", "tan", "sec", "csc", "exp",
    "sinh", "cosh", "tanh", "sech", "csch", "ln",
    "arcsin", "arccos", "arctan", "arcsec", "arccsc", "log",
    "arcsinh", "arccosh", "arctanh", "arcsech", "arccsch"
  };

const char * TrigoID[]={
  "wxID_PANEL_MATH_SIN", "wxID_PANEL_MATH_COS", "wxID_PANEL_MATH_TAN",
  "wxID_PANEL_MATH_SEC", "wxID_PANEL_MATH_CSC", "wxID_PANEL_MATH_EXP",
  "wxID_PANEL_MATH_SINH", "wxID_PANEL_MATH_COSH", "wxID_PANEL_MATH_TANH",
  "wxID_PANEL_MATH_SECH", "wxID_PANEL_MATH_CSCH", "wxID_PANEL_MATH_LN",
  "wxID_PANEL_MATH_ARCSIN", "wxID_PANEL_MATH_ARCCOS", "wxID_PANEL_MATH_ARCTAN",
  "wxID_PANEL_MATH_ARCSEC", "wxID_PANEL_MATH_ARCCSC", "wxID_PANEL_MATH_LOG",
  "wxID_PANEL_MATH_ARCSINH", "wxID_PANEL_MATH_ARCCOSH", "wxID_PANEL_MATH_ARCTANH",
  "wxID_PANEL_MATH_ARCSECH", "wxID_PANEL_ARCMATH_CSCH"
};
#else /* _MACOS */
#define MAX_TOOL_PER_LINE 4
const char *TrigoTable[] = 
  {
  "sin", "sinh", "arcsin", "arcsinh",
  "cos", "cosh", "arccos", "arccosh",
  "tan", "tanh", "arctan", "arctanh",
  "cot", "coth", "arccot", "arccoth",
  "sec", "sech", "arcsec", "arcsech",
  "csc", "csch", "arccsc", "arccsch",
  "exp", "ln", "log"
  };

const char * TrigoID[]={
  "wxID_PANEL_MATH_SIN", "wxID_PANEL_MATH_SINH", "wxID_PANEL_MATH_ARCSIN", "wxID_PANEL_MATH_ARCSINH",
  "wxID_PANEL_MATH_COS", "wxID_PANEL_MATH_COSH", "wxID_PANEL_MATH_ARCCOS", "wxID_PANEL_MATH_ARCCOSH",
  "wxID_PANEL_MATH_TAN", "wxID_PANEL_MATH_TANH", "wxID_PANEL_MATH_ARCTAN", "wxID_PANEL_MATH_ARCTANH",
  "wxID_PANEL_MATH_COT", "wxID_PANEL_MATH_COTH", "wxID_PANEL_MATH_ARCCOT", "wxID_PANEL_MATH_ARCCOTH",
  "wxID_PANEL_MATH_SEC", "wxID_PANEL_MATH_SECH", "wxID_PANEL_MATH_ARCSEC", "wxID_PANEL_MATH_ARCSECH",
  "wxID_PANEL_MATH_CSC", "wxID_PANEL_MATH_CSCH", "wxID_PANEL_MATH_ARCCSC", "wxID_PANEL_MATH_ARCCSCH",
  "wxID_PANEL_MATH_EXP", "wxID_PANEL_MATH_LN", "wxID_PANEL_MATH_LOG"
};
#endif /* _MACOS */

int MAX_Trigo = sizeof(TrigoTable) / sizeof (char *);


static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaMathMLToolBarToolDef)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_BMATH",               "CreateMath", LIB, TMSG_MATHML) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PARENTHESIS",         "CreateMPARENTHESIS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MROW",                "CreateMROW", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SQRT",                "CreateMSQRT", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ROOT",                "CreateMROOT", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_FRAC",                "CreateMFRAC", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LFRAC",               "CreateMLFRAC", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MTABLE",              "CreateMTABLE", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_UNDER",               "CreateMUNDER", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVER",                "CreateMOVER", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERUNDER",           "CreateMUNDEROVER", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MSCRIPT",             "CreateMMULTISCRIPTS", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUB",                 "CreateMSUB", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUP",                 "CreateMSUP", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUBSUP",              "CreateMSUBSUP", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INTEGRAL",            "CreateMIntegral", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUM",                 "CreateMSum", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MATRIX",              "CreateMMATRIX", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERARROW",           "CreateMOVERARROW", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERBAR",             "CreateMOVERBAR", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERBREVE",           "CreateMOVERBREVE", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERCHECK",           "CreateMOVERCHECK", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERBRACE",           "CreateMOVERBRACE", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_UNDERBRACE",          "CreateMUNDERBRACE", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERDOT",             "CreateMOVERDOT", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERHAT",             "CreateMOVERHAT", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERTILDE",           "CreateMOVERTILDE", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ABS",                 "CreateMABS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ALEPHSUB",            "CreateMALEPHSUB", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_AND",                 "CreateMAND", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ANDBINARY",           "CreateMANDBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_APPROX",              "CreateMAPPROX", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARG",                 "CreateMARG", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARROW1",              "CreateMARROW1", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARROW2",              "CreateMARROW2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CARD",                "CreateMCARD", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CARD2",               "CreateMCARD2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CARTESIANPRODUCT",    "CreateMCARTESIANPRODUCT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CARTESIANPRODUCTBINARY", "CreateMCARTESIANPRODUCTBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CEILING",              "CreateMCEILING", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CODOMAIN",             "CreateMCODOMAIN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COMBINATION",          "CreateMCOMBINATION", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COMPLEMENT",           "CreateMCOMPLEMENT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COMPLEMENTSUB",        "CreateMCOMPLEMENTSUB", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COMPLEXCARTESIAN",     "CreateMCOMPLEXCARTESIAN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COMPLEXCARTESIAN2",    "CreateMCOMPLEXCARTESIAN2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COMPLEXES",            "CreateMCOMPLEXES", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COMPLEXPOLAR",         "CreateMCOMPLEXPOLAR", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COMPOSE",              "CreateMCOMPOSE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COMPOSEBINARY",        "CreateMCOMPOSEBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CONGRU",               "CreateMCONGRU", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CONJUGATE",            "CreateMCONJUGATE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COUPLE",               "CreateMCOUPLE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CURL",                 "CreateMCURL", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DETERMINANT",          "CreateMDETERMINANT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DETERMINANT2",         "CreateMDETERMINANT2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DIAGONALINTERSECTION", "CreateMDIAGONALINTERSECTION", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DIFF",                 "CreateMDIFF", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DIFF2",                "CreateMDIFF2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DIFF3",                "CreateMDIFF3", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DIRECTSUM",            "CreateMDIRECTSUM", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DIVERGENCE",           "CreateMDIVERGENCE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DIVIDE",               "CreateMDIVIDE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_DOMAIN",               "CreateMDOMAIN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EMPTYSET",             "CreateMEMPTYSET", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EQ",                   "CreateMEQ", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EQUIVALENT",           "CreateMEQUIVALENT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EQUIVALENT2",          "CreateMEQUIVALENT2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EQUIVALENT2BINARY",    "CreateMEQUIVALENT2BINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EQUIVALENTBINARY",     "CreateMEQUIVALENTBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EQUIVALENTUNDER",      "CreateMEQUIVALENTUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ENCLOSE",              "CreateMENCLOSE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ENCLOSE2",             "CreateMENCLOSE2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MPHANTOM",             "CreateMPHANTOM", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EULERGAMMA",           "CreateMEULERGAMMA", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EXISTS",               "CreateMEXISTS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EXISTS2",              "CreateMEXISTS2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EXPONENTIALE",         "CreateMEXPONENTIALE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_FACTORIAL",            "CreateMFACTORIAL", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_FACTOROF",             "CreateMFACTOROF", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_FALSE",                "CreateMFALSE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_FENCE",                "CreateMFENCE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_FLOOR",                "CreateMFLOOR", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_FORALL",               "CreateMFORALL", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_GCD",                  "CreateMGCD", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_GEQ",                  "CreateMGEQ", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_GEQBINARY",            "CreateMGEQBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_GRAD",                 "CreateMGRAD", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_GT",                   "CreateMGT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_IDENT",                "CreateMIDENT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_IMAGE",                "CreateMIMAGE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_IMAGINARY",            "CreateMIMAGINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_IMAGINARYI",           "CreateMIMAGINARYI", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_IMPLIES",              "CreateMIMPLIES", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_IN",                   "CreateMIN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INF",                  "CreateMINF", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INFINITY",             "CreateMINFINITY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INFUNDER",             "CreateMINFUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INT2",                 "CreateMINT2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INTEGERS",             "CreateMINTEGERS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INTERSECT",            "CreateMINTERSECT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INTERSECTBINARY",      "CreateMINTERSECTBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INTERSECTUNDER",       "CreateMINTERSECTUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INTERVAL",             "CreateMINTERVAL", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INTUNDER",             "CreateMINTUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_INVERSE",              "CreateMINVERSE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ISOMORPHIC",           "CreateMISOMORPHIC", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LAPLACIAN",            "CreateMLAPLACIAN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LCM",                  "CreateMLCM", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LEQ",                  "CreateMLEQ", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LEQBINARY",            "CreateMLEQBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LISTEXTENSION",        "CreateMLISTEXTENSION", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LISTSEPARATION",       "CreateMLISTSEPARATION", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LAMBDA",               "CreateMLAMBDA", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LIM",                  "CreateMLIM", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LIMTENDSTO",           "CreateMLIMTENDSTO", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LT",                   "CreateMLT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MAP",                  "CreateMMAP", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MAX",                  "CreateMMAX", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MAXUNDER",             "CreateMMAXUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MEAN",                 "CreateMMEAN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MEDIAN",               "CreateMMEDIAN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MIN",                  "CreateMMIN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MINUNDER",             "CreateMMINUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MINUSBINARY",          "CreateMMINUSBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MINUSUNARY",           "CreateMMINUSUNARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MODE",                 "CreateMMODE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_MOMENT",               "CreateMMOMENT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_NATURALS",             "CreateMNATURALS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_NEQ",                  "CreateMNEQ", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_NOT",                  "CreateMNOT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_NOTANUMBER",           "CreateMNOTANUMBER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_NOTIN",                "CreateMNOTIN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_NOTPRSUBSET",          "CreateMNOTPRSUBSET", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_NOTSUBSET",            "CreateMNOTSUBSET", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_NUPLET",               "CreateMNUPLET", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OMEGASUB",             "CreateMOMEGASUB", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OR",                   "CreateMOR", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ORBINARY",             "CreateMORBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ORTHOGONAL",           "CreateMORTHOGONAL", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ORTHOGONALCOMPLEMENT", "CreateMORTHOGONALCOMPLEMENT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OUTERPRODUCT",         "CreateMOUTERPRODUCT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PARTIALDIFF",          "CreateMPARTIALDIFF", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PARTIALDIFF2",         "CreateMPARTIALDIFF2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PI",                   "CreateMPI", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PIECEWISE",            "CreateMPIECEWISE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PLUS",                 "CreateMPLUS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_POWER",                "CreateMPOWER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_POWERSET",             "CreateMPOWERSET", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PRIMES",               "CreateMPRIMES", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PRODUNDER",            "CreateMPRODUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PRODUNDEROVER",        "CreateMPRODUNDEROVER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PRSUBSET",             "CreateMPRSUBSET", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PRSUBSETBINARY",       "CreateMPRSUBSETBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_QUOTIENT",             "CreateMQUOTIENT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_RATIONNALS",           "CreateMRATIONNALS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_REAL",                 "CreateMREAL", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_REALS",                "CreateMREALS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_REM",                  "CreateMREM", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SCALARPRODUCT",        "CreateMSCALARPRODUCT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SDEV",                 "CreateMSDEV", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SELECTOR",             "CreateMSELECTOR", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SETDIFF",              "CreateMSETDIFF", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SETEXTENSION",         "CreateMSETEXTENSION", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SETSEPARATION",        "CreateMSETSEPARATION", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SETSYMDIFF",           "CreateMSETSYMDIFF", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUBSET",               "CreateMSUBSET", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUBSETBINARY",         "CreateMSUBSETBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUMUNDER",             "CreateMSUMUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUP2",                 "CreateMSUP2", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUPMINUS",             "CreateMSUPMINUS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUPPLUS",              "CreateMSUPPLUS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SUPUNDER",             "CreateMSUPUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_TENDSTO",              "CreateMTENDSTO", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_TENDSTOTENDSTO",       "CreateMTENDSTOTENDSTO", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_TIMES",                "CreateMTIMES", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_TIMESBINARY",          "CreateMTIMESBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_TRANSPOSE",            "CreateMTRANSPOSE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_TRUE",                 "CreateMTRUE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_UNION",                "CreateMUNION", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_UNIONUNARY",           "CreateMUNIONUNARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_UNIONUNDER",           "CreateMUNIONUNDER", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_UNIONBINARY",          "CreateMUNIONBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_VARIANCE",             "CreateMVARIANCE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_VECTORPRODUCT",        "CreateMVECTORPRODUCT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_VECTORROW",            "CreateMVECTORROW", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_VECTORCOLUMN",         "CreateMVECTORCOLUMN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_VERTICALBRACE",        "CreateMVERTICALBRACE", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_XOR",                  "CreateMXOR", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_XORBINARY",            "CreateMXORBINARY", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LL",                   "CreateMLL", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_GG",                   "CreateMGG", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_NORM",                 "CreateMNORM", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SYMBOLOO",             "CreateMSYMBOLOO", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SYMBOLO",              "CreateMSYMBOLO", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_PARALLEL",             "CreateMPARALLEL", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_QUATERNIONS",          "CreateMQUATERNIONS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_OVERFROWN",            "CreateMOVERFROWN", wxID_ANY, wxID_ANY) 
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCCOS",   "CreateMARCCOS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCCOSH",  "CreateMARCCOSH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCCOT",   "CreateMARCCOT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCCOTH",  "CreateMARCCOTH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCCSC",   "CreateMARCCSC", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCCSCH",  "CreateMARCCSCH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCSEC",   "CreateMARCSEC", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCSECH",  "CreateMARCSECH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCSIN",   "CreateMARCSIN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCSINH",  "CreateMARCSINH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCTAN",   "CreateMARCTAN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_ARCTANH",  "CreateMARCTANH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COS",      "CreateMCOS", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COSH",     "CreateMCOSH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COT",      "CreateMCOT", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_COTH",     "CreateMCOTH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CSC",      "CreateMCSC", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_CSCH",     "CreateMCSCH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_EXP",      "CreateMEXP", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LN",       "CreateMLN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_LOG",      "CreateMLOG", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SEC",      "CreateMSEC", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SECH",     "CreateMSECH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SIN",      "CreateMSIN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_SINH",     "CreateMSINH", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_TAN",      "CreateMTAN", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_MATH_TANH",     "CreateMTANH", wxID_ANY, wxID_ANY)
AMAYA_END_TOOLBAR_DEF_TABLE()

//
//
// AmayaMathMLToolBar
//
//

IMPLEMENT_DYNAMIC_CLASS(AmayaMathMLToolBar, AmayaBaseToolBar)

BEGIN_EVENT_TABLE(AmayaMathMLToolBar, AmayaBaseToolBar)
  EVT_UPDATE_UI(wxID_ANY, AmayaMathMLToolBar::OnUpdate)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
-----------------------------------------------------------------------*/
AmayaMathMLToolBar::AmayaMathMLToolBar():
  AmayaBaseToolBar()
{
  // Overload std map, use a static math for all MathMl toolbars
  m_map = &s_mymap;
  
  ShowAllTools(true);
  
  if(!s_isinit)
    {
      Add(AmayaMathMLToolBarToolDef);
      s_isinit = true;
    }
}

/*----------------------------------------------------------------------
-----------------------------------------------------------------------*/
void AmayaMathMLToolBar::OnUpdate(wxUpdateUIEvent& event)
{
   event.Enable(true);
}




/*----------------------------------------------------------------------
-----------------------------------------------------------------------*/
AmayaToolBarToolDefHashMap AmayaMathMLToolBar::s_mymap;
bool AmayaMathMLToolBar::s_isinit = false;



//
//
// AmayaMathMLPanel
//
//

/*----------------------------------------------------------------------
-----------------------------------------------------------------------*/
AmayaMathMLPanel::AmayaMathMLPanel():
  wxPanel(),
  m_pBook(NULL),
  m_imagelist(16,16)
{
}


/*----------------------------------------------------------------------
-----------------------------------------------------------------------*/
AmayaMathMLPanel::AmayaMathMLPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style, const wxString& name, wxObject* extra):
      wxPanel(),
  m_pBook(NULL),
  m_imagelist(16,16)
{
  Create(parent, id, pos, size, style, name, extra);
}

/*----------------------------------------------------------------------
-----------------------------------------------------------------------*/
AmayaMathMLPanel::~AmayaMathMLPanel()
{
}

/*----------------------------------------------------------------------
-----------------------------------------------------------------------*/
bool AmayaMathMLPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_MATHML")))
    return false;
#ifdef _WINDOWS
  SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */
  m_pBook = XRCCTRL(*this,"wxID_MATHS_CHOICEBOOK", wxChoicebook);
  m_pBook->SetPageText(0, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_MATH_PANEL_1)));
  m_pBook->SetPageText(1, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_MATH_PANEL_2)));
  m_pBook->SetPageText(2, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_MATH_PANEL_3)));
  m_pBook->SetPageText(3, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_MATH_PANEL_4)));
  m_pBook->SetPageText(4, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_MATH_PANEL_5)));
  m_pBook->ChangeSelection(0);
  // add the trigonometry page
  Initialize();
  return true;
}

/*----------------------------------------------------------------------
-----------------------------------------------------------------------*/
void AmayaMathMLPanel::Initialize()
{
  wxSizer* sz = new wxBoxSizer(wxVERTICAL);
  sz->Add(m_pBook, 1, wxEXPAND);
  SetSizer(sz);
  m_pBook->SetImageList(&m_imagelist);
  wxBitmap bmp;
  bmp.LoadFile(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "dummy.png"), wxBITMAP_TYPE_ANY);
  int img = m_imagelist.Add(bmp);
  wxPanel*   panel = new wxPanel(m_pBook, wxID_ANY);
  m_pBook->AddPage(panel, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_MATH_PANEL_6)), false, img);
  
  wxToolBar* tb = NULL;
  sz = new wxBoxSizer(wxVERTICAL);
  int i, line = 0;
  for (i = 0; i < MAX_Trigo; i++)
    {
      if( ++line >= MAX_TOOL_PER_LINE || !tb)
        {
          if (tb)
            tb->Realize();
          tb = new AmayaMathMLToolBar();
          tb->Create(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                     wxTB_HORIZONTAL|wxNO_BORDER|wxTB_NODIVIDER);
#ifdef _MACOS
          tb->SetToolBitmapSize(wxSize(24,24));
#else /* _MACOS */
          tb->SetToolBitmapSize(wxSize(36,16));
#endif /* _MACOS */
          tb->SetToolPacking(4);
          sz->Add(tb, 0);
          line = 0;
        }
      int toolid = wxXmlResource::GetXRCID( TtaConvMessageToWX(TrigoID[i]));
      wxString str = TtaConvMessageToWX(TrigoTable[i]);
#ifdef _MACOS
      tb->AddTool(toolid, str, wxCharToIcon<24,24>(str), str);
#else /* _MACOS */
      tb->AddTool(toolid, str, wxCharToIcon<36, 16>(str), str);
#endif /* _MACOS */
    }
  panel->SetSizer(sz);
}
  
#endif /* #ifdef _WX */
