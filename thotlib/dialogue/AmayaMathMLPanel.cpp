#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

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

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"

#include "AmayaMathMLPanel.h"
#include "AmayaNormalWindow.h"


//
//
// AmayaMathMLToolPanel
//
//

IMPLEMENT_DYNAMIC_CLASS(AmayaMathMLToolPanel, AmayaToolPanel)

AmayaMathMLToolPanel::AmayaMathMLToolPanel():
  AmayaToolPanel()
{
}

AmayaMathMLToolPanel::~AmayaMathMLToolPanel()
{
}

bool AmayaMathMLToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  return wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_MATHML"));
}

wxString AmayaMathMLToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_MATHML));
}


/*----------------------------------------------------------------------
 *       Class:  AmayaMathMLToolPanel
 *      Method:  OnButton
 * Description:  this method is called when the user click on a tool
 -----------------------------------------------------------------------*/
void AmayaMathMLToolPanel::OnButton( wxCommandEvent& event )
{
  Document doc;
  View view;
  TtaGetActiveView( &doc, &view );

  int id       = event.GetId();
  CloseTextInsertion ();
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_BMATH")) )
      TtaExecuteMenuAction ("CreateMath", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PARENTHESIS")) )
      TtaExecuteMenuAction ("CreateMPARENTHESIS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MROW")) )
      TtaExecuteMenuAction ("CreateMROW", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SQRT")) )
      TtaExecuteMenuAction ("CreateMSQRT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ROOT")) )
      TtaExecuteMenuAction ("CreateMROOT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FRAC")) )
      TtaExecuteMenuAction ("CreateMFRAC", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MTABLE")) )
      TtaExecuteMenuAction ("CreateMTABLE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNDER")) )
      TtaExecuteMenuAction ("CreateMUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVER")) )
      TtaExecuteMenuAction ("CreateMOVER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERARROW")) )
      TtaExecuteMenuAction ("CreateMOVERARROW", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERBAR")) )
      TtaExecuteMenuAction ("CreateMOVERBAR", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERBREVE")) )
      TtaExecuteMenuAction ("CreateMOVERBREVE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERCHECK")) )
      TtaExecuteMenuAction ("CreateMOVERCHECK", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERBRACE")) )
      TtaExecuteMenuAction ("CreateMOVERBRACE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNDERBRACE")) )
      TtaExecuteMenuAction ("CreateMUNDERBRACE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERDOT")) )
      TtaExecuteMenuAction ("CreateMOVERDOT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERHAT")) )
      TtaExecuteMenuAction ("CreateMOVERHAT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERTILDE")) )
      TtaExecuteMenuAction ("CreateMOVERTILDE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERUNDER")) )
      TtaExecuteMenuAction ("CreateMUNDEROVER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MSCRIPT")) )
      TtaExecuteMenuAction ("CreateMMULTISCRIPTS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUB")) )
      TtaExecuteMenuAction ("CreateMSUB", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUP")) )
      TtaExecuteMenuAction ("CreateMSUP", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUBSUP")) )
      TtaExecuteMenuAction ("CreateMSUBSUP", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTEGRAL")) )
      TtaExecuteMenuAction ("CreateMIntegral", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUM")) )
      TtaExecuteMenuAction ("CreateMSum", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MATRIX")) )
      TtaExecuteMenuAction ("CreateMMATRIX", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ABS")) )
      TtaExecuteMenuAction ("CreateMABS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ALEPHSUB")) )
      TtaExecuteMenuAction ("CreateMALEPHSUB", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_AND")) )
      TtaExecuteMenuAction ("CreateMAND", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ANDBINARY")) )
      TtaExecuteMenuAction ("CreateMANDBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_APPROX")) )
      TtaExecuteMenuAction ("CreateMAPPROX", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARG")) )
      TtaExecuteMenuAction ("CreateMARG", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARROW1")) )
      TtaExecuteMenuAction ("CreateMARROW1", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARROW2")) )
      TtaExecuteMenuAction ("CreateMARROW2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CARD")) )
      TtaExecuteMenuAction ("CreateMCARD", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CARD2")) )
      TtaExecuteMenuAction ("CreateMCARD2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CARTESIANPRODUCT")) )
      TtaExecuteMenuAction ("CreateMCARTESIANPRODUCT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CARTESIANPRODUCTBINARY")) )
      TtaExecuteMenuAction ("CreateMCARTESIANPRODUCTBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CEILING")) )
      TtaExecuteMenuAction ("CreateMCEILING", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CODOMAIN")) )
      TtaExecuteMenuAction ("CreateMCODOMAIN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMBINATION")) )
      TtaExecuteMenuAction ("CreateMCOMBINATION", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEMENT")) )
      TtaExecuteMenuAction ("CreateMCOMPLEMENT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEMENTSUB")) )
      TtaExecuteMenuAction ("CreateMCOMPLEMENTSUB", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEXCARTESIAN")) )
      TtaExecuteMenuAction ("CreateMCOMPLEXCARTESIAN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEXCARTESIAN2")) )
      TtaExecuteMenuAction ("CreateMCOMPLEXCARTESIAN2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEXES")) )
      TtaExecuteMenuAction ("CreateMCOMPLEXES", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEXPOLAR")) )
      TtaExecuteMenuAction ("CreateMCOMPLEXPOLAR", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPOSE")) )
      TtaExecuteMenuAction ("CreateMCOMPOSE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPOSEBINARY")) )
      TtaExecuteMenuAction ("CreateMCOMPOSEBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CONGRU")) )
      TtaExecuteMenuAction ("CreateMCONGRU", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CONJUGATE")) )
      TtaExecuteMenuAction ("CreateMCONJUGATE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COUPLE")) )
      TtaExecuteMenuAction ("CreateMCOUPLE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CURL")) )
      TtaExecuteMenuAction ("CreateMCURL", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DETERMINANT")) )
      TtaExecuteMenuAction ("CreateMDETERMINANT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DETERMINANT2")) )
      TtaExecuteMenuAction ("CreateMDETERMINANT2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIAGONALINTERSECTION")) )
      TtaExecuteMenuAction ("CreateMDIAGONALINTERSECTION", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIFF")) )
      TtaExecuteMenuAction ("CreateMDIFF", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIFF2")) )
      TtaExecuteMenuAction ("CreateMDIFF2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIFF3")) )
      TtaExecuteMenuAction ("CreateMDIFF3", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIRECTSUM")) )
      TtaExecuteMenuAction ("CreateMDIRECTSUM", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIVERGENCE")) )
      TtaExecuteMenuAction ("CreateMDIVERGENCE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIVIDE")) )
      TtaExecuteMenuAction ("CreateMDIVIDE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DOMAIN")) )
      TtaExecuteMenuAction ("CreateMDOMAIN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EMPTYSET")) )
      TtaExecuteMenuAction ("CreateMEMPTYSET", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQ")) )
      TtaExecuteMenuAction ("CreateMEQ", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENT")) )
      TtaExecuteMenuAction ("CreateMEQUIVALENT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENT2")) )
      TtaExecuteMenuAction ("CreateMEQUIVALENT2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENT2BINARY")) )
      TtaExecuteMenuAction ("CreateMEQUIVALENT2BINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENTBINARY")) )
      TtaExecuteMenuAction ("CreateMEQUIVALENTBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENTUNDER")) )
      TtaExecuteMenuAction ("CreateMEQUIVALENTUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ENCLOSE")) )
      TtaExecuteMenuAction ("CreateMENCLOSE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ENCLOSE2")) )
      TtaExecuteMenuAction ("CreateMENCLOSE2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MPHANTOM")) )
      TtaExecuteMenuAction ("CreateMPHANTOM", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EULERGAMMA")) )
      TtaExecuteMenuAction ("CreateMEULERGAMMA", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EXISTS")) )
      TtaExecuteMenuAction ("CreateMEXISTS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EXISTS2")) )
      TtaExecuteMenuAction ("CreateMEXISTS2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EXPONENTIALE")) )
      TtaExecuteMenuAction ("CreateMEXPONENTIALE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FACTORIAL")) )
      TtaExecuteMenuAction ("CreateMFACTORIAL", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FACTOROF")) )
      TtaExecuteMenuAction ("CreateMFACTOROF", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FALSE")) )
      TtaExecuteMenuAction ("CreateMFALSE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FENCE")) )
      TtaExecuteMenuAction ("CreateMFENCE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FLOOR")) )
      TtaExecuteMenuAction ("CreateMFLOOR", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FORALL")) )
      TtaExecuteMenuAction ("CreateMFORALL", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GCD")) )
      TtaExecuteMenuAction ("CreateMGCD", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GEQ")) )
      TtaExecuteMenuAction ("CreateMGEQ", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GEQBINARY")) )
      TtaExecuteMenuAction ("CreateMGEQBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GRAD")) )
      TtaExecuteMenuAction ("CreateMGRAD", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GT")) )
      TtaExecuteMenuAction ("CreateMGT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IDENT")) )
      TtaExecuteMenuAction ("CreateMIDENT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IMAGE")) )
      TtaExecuteMenuAction ("CreateMIMAGE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IMAGINARY")) )
      TtaExecuteMenuAction ("CreateMIMAGINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IMAGINARYI")) )
      TtaExecuteMenuAction ("CreateMIMAGINARYI", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IMPLIES")) )
      TtaExecuteMenuAction ("CreateMIMPLIES", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IN")) )
      TtaExecuteMenuAction ("CreateMIN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INF")) )
      TtaExecuteMenuAction ("CreateMINF", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INFINITY")) )
      TtaExecuteMenuAction ("CreateMINFINITY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INFUNDER")) )
      TtaExecuteMenuAction ("CreateMINFUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INT2")) )
      TtaExecuteMenuAction ("CreateMINT2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTEGERS")) )
      TtaExecuteMenuAction ("CreateMINTEGERS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTERSECT")) )
      TtaExecuteMenuAction ("CreateMINTERSECT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTERSECTBINARY")) )
      TtaExecuteMenuAction ("CreateMINTERSECTBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTERSECTUNDER")) )
      TtaExecuteMenuAction ("CreateMINTERSECTUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTERVAL")) )
      TtaExecuteMenuAction ("CreateMINTERVAL", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTUNDER")) )
      TtaExecuteMenuAction ("CreateMINTUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INVERSE")) )
      TtaExecuteMenuAction ("CreateMINVERSE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ISOMORPHIC")) )
      TtaExecuteMenuAction ("CreateMISOMORPHIC", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LAPLACIAN")) )
      TtaExecuteMenuAction ("CreateMLAPLACIAN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LCM")) )
      TtaExecuteMenuAction ("CreateMLCM", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LEQ")) )
      TtaExecuteMenuAction ("CreateMLEQ", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LEQBINARY")) )
      TtaExecuteMenuAction ("CreateMLEQBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LISTEXTENSION")) )
      TtaExecuteMenuAction ("CreateMLISTEXTENSION", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LISTSEPARATION")) )
      TtaExecuteMenuAction ("CreateMLISTSEPARATION", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LAMBDA")) )
      TtaExecuteMenuAction ("CreateMLAMBDA", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LIM")) )
      TtaExecuteMenuAction ("CreateMLIM", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LIMTENDSTO")) )
      TtaExecuteMenuAction ("CreateMLIMTENDSTO", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LT")) )
      TtaExecuteMenuAction ("CreateMLT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MAP")) )
      TtaExecuteMenuAction ("CreateMMAP", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MAX")) )
      TtaExecuteMenuAction ("CreateMMAX", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MAXUNDER")) )
      TtaExecuteMenuAction ("CreateMMAXUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MEAN")) )
      TtaExecuteMenuAction ("CreateMMEAN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MEDIAN")) )
      TtaExecuteMenuAction ("CreateMMEDIAN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MIN")) )
      TtaExecuteMenuAction ("CreateMMIN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MINUNDER")) )
      TtaExecuteMenuAction ("CreateMMINUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MINUSBINARY")) )
      TtaExecuteMenuAction ("CreateMMINUSBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MINUSUNARY")) )
      TtaExecuteMenuAction ("CreateMMINUSUNARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MODE")) )
      TtaExecuteMenuAction ("CreateMMODE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MOMENT")) )
      TtaExecuteMenuAction ("CreateMMOMENT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NATURALS")) )
      TtaExecuteMenuAction ("CreateMNATURALS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NEQ")) )
      TtaExecuteMenuAction ("CreateMNEQ", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOT")) )
      TtaExecuteMenuAction ("CreateMNOT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOTANUMBER")) )
      TtaExecuteMenuAction ("CreateMNOTANUMBER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOTIN")) )
      TtaExecuteMenuAction ("CreateMNOTIN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOTPRSUBSET")) )
      TtaExecuteMenuAction ("CreateMNOTPRSUBSET", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOTSUBSET")) )
      TtaExecuteMenuAction ("CreateMNOTSUBSET", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NUPLET")) )
      TtaExecuteMenuAction ("CreateMNUPLET", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OMEGASUB")) )
      TtaExecuteMenuAction ("CreateMOMEGASUB", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OR")) )
      TtaExecuteMenuAction ("CreateMOR", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ORBINARY")) )
      TtaExecuteMenuAction ("CreateMORBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ORTHOGONAL")) )
      TtaExecuteMenuAction ("CreateMORTHOGONAL", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ORTHOGONALCOMPLEMENT")) )
      TtaExecuteMenuAction ("CreateMORTHOGONALCOMPLEMENT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OUTERPRODUCT")) )
      TtaExecuteMenuAction ("CreateMOUTERPRODUCT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PARTIALDIFF")) )
      TtaExecuteMenuAction ("CreateMPARTIALDIFF", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PARTIALDIFF2")) )
      TtaExecuteMenuAction ("CreateMPARTIALDIFF2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PI")) )
      TtaExecuteMenuAction ("CreateMPI", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PIECEWISE")) )
      TtaExecuteMenuAction ("CreateMPIECEWISE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_VERTICALBRACE")) )
      TtaExecuteMenuAction ("CreateMVERTICALBRACE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PLUS")) )
      TtaExecuteMenuAction ("CreateMPLUS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_POWER")) )
      TtaExecuteMenuAction ("CreateMPOWER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_POWERSET")) )
      TtaExecuteMenuAction ("CreateMPOWERSET", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRIMES")) )
      TtaExecuteMenuAction ("CreateMPRIMES", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRODUNDER")) )
      TtaExecuteMenuAction ("CreateMPRODUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRODUNDEROVER")) )
      TtaExecuteMenuAction ("CreateMPRODUNDEROVER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRSUBSET")) )
      TtaExecuteMenuAction ("CreateMPRSUBSET", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRSUBSETBINARY")) )
      TtaExecuteMenuAction ("CreateMPRSUBSETBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_QUOTIENT")) )
      TtaExecuteMenuAction ("CreateMQUOTIENT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_RATIONNALS")) )
      TtaExecuteMenuAction ("CreateMRATIONNALS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_REAL")) )
      TtaExecuteMenuAction ("CreateMREAL", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_REALS")) )
      TtaExecuteMenuAction ("CreateMREALS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_REM")) )
      TtaExecuteMenuAction ("CreateMREM", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SCALARPRODUCT")) )
      TtaExecuteMenuAction ("CreateMSCALARPRODUCT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SDEV")) )
      TtaExecuteMenuAction ("CreateMSDEV", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SELECTOR")) )
      TtaExecuteMenuAction ("CreateMSELECTOR", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SETDIFF")) )
      TtaExecuteMenuAction ("CreateMSETDIFF", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SETEXTENSION")) )
      TtaExecuteMenuAction ("CreateMSETEXTENSION", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SETSEPARATION")) )
      TtaExecuteMenuAction ("CreateMSETSEPARATION", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SETSYMDIFF")) )
      TtaExecuteMenuAction ("CreateMSETSYMDIFF", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUBSET")) )
      TtaExecuteMenuAction ("CreateMSUBSET", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUBSETBINARY")) )
      TtaExecuteMenuAction ("CreateMSUBSETBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUMUNDER")) )
      TtaExecuteMenuAction ("CreateMSUMUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUP2")) )
      TtaExecuteMenuAction ("CreateMSUP2", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUPMINUS")) )
      TtaExecuteMenuAction ("CreateMSUPMINUS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUPPLUS")) )
      TtaExecuteMenuAction ("CreateMSUPPLUS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUPUNDER")) )
      TtaExecuteMenuAction ("CreateMSUPUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TENDSTO")) )
      TtaExecuteMenuAction ("CreateMTENDSTO", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TENDSTOTENDSTO")) )
      TtaExecuteMenuAction ("CreateMTENDSTOTENDSTO", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TIMES")) )
      TtaExecuteMenuAction ("CreateMTIMES", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TIMESBINARY")) )
      TtaExecuteMenuAction ("CreateMTIMESBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TRANSPOSE")) )
      TtaExecuteMenuAction ("CreateMTRANSPOSE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TRUE")) )
      TtaExecuteMenuAction ("CreateMTRUE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNION")) )
      TtaExecuteMenuAction ("CreateMUNION", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNIONUNARY")) )
      TtaExecuteMenuAction ("CreateMUNIONUNARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNIONUNDER")) )
      TtaExecuteMenuAction ("CreateMUNIONUNDER", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNIONBINARY")) )
      TtaExecuteMenuAction ("CreateMUNIONBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_VARIANCE")) )
      TtaExecuteMenuAction ("CreateMVARIANCE", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_VECTORPRODUCT")) )
      TtaExecuteMenuAction ("CreateMVECTORPRODUCT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_VECTORROW")) )
      TtaExecuteMenuAction ("CreateMVECTORROW", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_VECTORCOLUMN")) )
      TtaExecuteMenuAction ("CreateMVECTORCOLUMN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_XOR")) )
      TtaExecuteMenuAction ("CreateMXOR", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_XORBINARY")) )
      TtaExecuteMenuAction ("CreateMXORBINARY", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_QUATERNIONS")) )
      TtaExecuteMenuAction ("CreateMQUATERNIONS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERFROWN")) )
      TtaExecuteMenuAction ("CreateMOVERFROWN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PARALLEL")) )
      TtaExecuteMenuAction ("CreateMPARALLEL", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SYMBOLO")) )
      TtaExecuteMenuAction ("CreateMSYMBOLO", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SYMBOLOO")) )
      TtaExecuteMenuAction ("CreateMSYMBOLOO", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NORM")) )
      TtaExecuteMenuAction ("CreateMNORM", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GG")) )
      TtaExecuteMenuAction ("CreateMGG", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LL")) )
    TtaExecuteMenuAction ("CreateMLL", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCCOS")) )
    TtaExecuteMenuAction ("CreateMARCCOS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCCOSH")) )
    TtaExecuteMenuAction ("CreateMARCCOSH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCCOT")) )
    TtaExecuteMenuAction ("CreateMARCCOT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCCOTH")) )
    TtaExecuteMenuAction ("CreateMARCCOTH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCCSC")) )
    TtaExecuteMenuAction ("CreateMARCCSC", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCCSCH")) )
    TtaExecuteMenuAction ("CreateMARCCSCH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCSEC")) )
    TtaExecuteMenuAction ("CreateMARCSEC", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCSECH")) )
    TtaExecuteMenuAction ("CreateMARCSECH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCSIN")) )
    TtaExecuteMenuAction ("CreateMARCSIN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCSINH")) )
    TtaExecuteMenuAction ("CreateMARCSINH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCTAN")) )
    TtaExecuteMenuAction ("CreateMARCTAN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARCTANH")) )
    TtaExecuteMenuAction ("CreateMARCTANH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COS")) )
    TtaExecuteMenuAction ("CreateMCOS", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COSH")) )
    TtaExecuteMenuAction ("CreateMCOSH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COT")) )
    TtaExecuteMenuAction ("CreateMCOT", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COTH")) )
    TtaExecuteMenuAction ("CreateMCOTH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CSC")) )
    TtaExecuteMenuAction ("CreateMCSC", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CSCH")) )
    TtaExecuteMenuAction ("CreateMCSCH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EXP")) )
    TtaExecuteMenuAction ("CreateMEXP", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LN")) )
    TtaExecuteMenuAction ("CreateMLN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LOG")) )
    TtaExecuteMenuAction ("CreateMLOG", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SEC")) )
    TtaExecuteMenuAction ("CreateMSEC", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SECH")) )
    TtaExecuteMenuAction ("CreateMSECH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SIN")) )
    TtaExecuteMenuAction ("CreateMSIN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SINH")) )
    TtaExecuteMenuAction ("CreateMSINH", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TAN")) )
    TtaExecuteMenuAction ("CreateMTAN", doc, view, FALSE);
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TANH")) )
    TtaExecuteMenuAction ("CreateMTANH", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMathMLToolPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 -----------------------------------------------------------------------*/
void AmayaMathMLToolPanel::SendDataToPanel( AmayaParams& p )
{
  int action = p.param1;
  if (action == wxMATHML_ACTION_INIT)
    {
    }
  else if (action == wxMATHML_ACTION_REFRESH)
    {
    }
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaMathMLToolPanel, AmayaToolPanel)
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_BMATH"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PARENTHESIS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MROW"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SQRT"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ROOT"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_FRAC"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MTABLE"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNDER"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVER"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERUNDER"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MSCRIPT"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUB"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUP"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUBSUP"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTEGRAL"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUM"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MATRIX"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVER"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERARROW"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERBAR"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERBREVE"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERCHECK"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERBRACE"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNDERBRACE"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERDOT"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERHAT"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERTILDE"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ABS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ALEPHSUB"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_AND"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ANDBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_APPROX"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARG"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARROW1"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARROW2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CARD"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CARD2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CARTESIANPRODUCT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CARTESIANPRODUCTBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CEILING"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CODOMAIN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMBINATION"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEMENT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEMENTSUB"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEXCARTESIAN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEXCARTESIAN2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEXES"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEXPOLAR"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPOSE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPOSEBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CONGRU"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CONJUGATE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COUPLE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CURL"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DETERMINANT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DETERMINANT2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIAGONALINTERSECTION"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIFF"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIFF2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIFF3"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIRECTSUM"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIVERGENCE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIVIDE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_DOMAIN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EMPTYSET"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQ"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENT2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENT2BINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENTBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENTUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ENCLOSE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ENCLOSE2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MPHANTOM"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EULERGAMMA"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EXISTS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EXISTS2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EXPONENTIALE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_FACTORIAL"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_FACTOROF"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_FALSE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_FENCE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_FLOOR"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_FORALL"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_GCD"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_GEQ"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_GEQBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_GRAD"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_GT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_IDENT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_IMAGE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_IMAGINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_IMAGINARYI"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_IMPLIES"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_IN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INF"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INFINITY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INFUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INT2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTEGERS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTERSECT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTERSECTBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTERSECTUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTERVAL"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_INVERSE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ISOMORPHIC"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LAPLACIAN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LCM"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LEQ"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LEQBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LISTEXTENSION"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LISTSEPARATION"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LAMBDA"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LIM"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LIMTENDSTO"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MAP"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MAX"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MAXUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MEAN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MEDIAN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MIN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MINUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MINUSBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MINUSUNARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MODE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_MOMENT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_NATURALS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_NEQ"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOTANUMBER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOTIN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOTPRSUBSET"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOTSUBSET"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_NUPLET"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OMEGASUB"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OR"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ORBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ORTHOGONAL"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ORTHOGONALCOMPLEMENT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OUTERPRODUCT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PARTIALDIFF"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PARTIALDIFF2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PI"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PIECEWISE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PLUS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_POWER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_POWERSET"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRIMES"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRODUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRODUNDEROVER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRSUBSET"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRSUBSETBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_QUOTIENT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_RATIONNALS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_REAL"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_REALS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_REM"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SCALARPRODUCT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SDEV"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SELECTOR"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SETDIFF"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SETEXTENSION"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SETSEPARATION"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SETSYMDIFF"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUBSET"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUBSETBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUMUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUP2"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUPMINUS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUPPLUS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUPUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_TENDSTO"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_TENDSTOTENDSTO"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_TIMES"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_TIMESBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_TRANSPOSE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_TRUE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNION"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNIONUNARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNIONUNDER"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNIONBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_VARIANCE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_VECTORPRODUCT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_VECTORROW"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_VECTORCOLUMN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_VERTICALBRACE"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_XOR"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_XORBINARY"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LL"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_GG"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_NORM"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SYMBOLOO"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SYMBOLO"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_PARALLEL"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_QUATERNIONS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERFROWN"), AmayaMathMLToolPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCCOS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCCOSH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCCOT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCCOTH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCCSC"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCCSCH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCSEC"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCSECH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCSIN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCSINH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCTAN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARCTANH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COS"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COSH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COT"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_COTH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CSC"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_CSCH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_EXP"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_LOG"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SEC"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SECH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SIN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_SINH"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_TAN"), AmayaMathMLToolPanel::OnButton )
  EVT_BUTTON( XRCID("wxID_PANEL_MATH_TANH"), AmayaMathMLToolPanel::OnButton )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
