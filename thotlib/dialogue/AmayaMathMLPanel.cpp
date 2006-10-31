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
#include "panel_tv.h"

#include "AmayaMathMLPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaMathMLPanel, AmayaSubPanel)

/*----------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  AmayaMathMLPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
  -----------------------------------------------------------------------*/
AmayaMathMLPanel::AmayaMathMLPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_MATHML") )
{
  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_MATHML)));

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  ~AmayaMathMLPanel
 * Description:  destructor
 *               TODO
  -----------------------------------------------------------------------*/
AmayaMathMLPanel::~AmayaMathMLPanel()
{
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  GetPanelType
 * Description:  
  -----------------------------------------------------------------------*/
int AmayaMathMLPanel::GetPanelType()
{
  return WXAMAYA_PANEL_MATHML;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
  -----------------------------------------------------------------------*/
void AmayaMathMLPanel::RefreshToolTips()
{  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButton
 * Description:  this method is called when the user click on a tool
  -----------------------------------------------------------------------*/
void AmayaMathMLPanel::OnButton( wxCommandEvent& event )
{
  Document doc;
  View view;
  TtaGetActiveView( &doc, &view );

  int id       = event.GetId();
  CloseTextInsertion ();
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_BMATH")) )
    TtaExecuteMenuAction ("CreateMath", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FENCE")) )
    TtaExecuteMenuAction ("CreateMROW", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SQRT")) )
    TtaExecuteMenuAction ("CreateMSQRT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ROOT")) )
    TtaExecuteMenuAction ("CreateMROOT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FRAC")) )
    TtaExecuteMenuAction ("CreateMFRAC", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MATRIX")) )
    TtaExecuteMenuAction ("CreateMTABLE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNDER")) )
    TtaExecuteMenuAction ("CreateMUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVER")) )
    TtaExecuteMenuAction ("CreateMOVER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERARROW")) )
    TtaExecuteMenuAction ("CreateMOVERARROW", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERBAR")) )
    TtaExecuteMenuAction ("CreateMOVERBAR", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERBREVE")) )
    TtaExecuteMenuAction ("CreateMOVERBREVE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERCHECK")) )
    TtaExecuteMenuAction ("CreateMOVERCHECK", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERDOT")) )
    TtaExecuteMenuAction ("CreateMOVERDOT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERHAT")) )
    TtaExecuteMenuAction ("CreateMOVERHAT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERTILDE")) )
    TtaExecuteMenuAction ("CreateMOVERTILDE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERUNDER")) )
    TtaExecuteMenuAction ("CreateMUNDEROVER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MSCRIPT")) )
    TtaExecuteMenuAction ("CreateMMULTISCRIPTS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUB")) )
    TtaExecuteMenuAction ("CreateMSUB", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUP")) )
    TtaExecuteMenuAction ("CreateMSUP", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUBSUP")) )
    TtaExecuteMenuAction ("CreateMSUBSUP", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTEGRAL")) )
    TtaExecuteMenuAction ("CreateMIntegral", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUM")) )
    TtaExecuteMenuAction ("CreateMSum", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MATRIX2")) )
    TtaExecuteMenuAction ("CreateMMATRIX2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ABS")) )
   TtaExecuteMenuAction ("CreateMABS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ALEPHSUB")) )
   TtaExecuteMenuAction ("CreateMALEPHSUB", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_AND")) )
   TtaExecuteMenuAction ("CreateMAND", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ANDBINARY")) )
   TtaExecuteMenuAction ("CreateMANDBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_APPROX")) )
   TtaExecuteMenuAction ("CreateMAPPROX", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARG")) )
   TtaExecuteMenuAction ("CreateMARG", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARROW1")) )
   TtaExecuteMenuAction ("CreateMARROW1", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ARROW2")) )
   TtaExecuteMenuAction ("CreateMARROW2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CARD")) )
   TtaExecuteMenuAction ("CreateMCARD", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CARD2")) )
   TtaExecuteMenuAction ("CreateMCARD2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CARTESIANPRODUCT")) )
   TtaExecuteMenuAction ("CreateMCARTESIANPRODUCT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CARTESIANPRODUCTBINARY")) )
   TtaExecuteMenuAction ("CreateMCARTESIANPRODUCTBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CEILING")) )
   TtaExecuteMenuAction ("CreateMCEILING", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CODOMAIN")) )
   TtaExecuteMenuAction ("CreateMCODOMAIN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMBINATION")) )
   TtaExecuteMenuAction ("CreateMCOMBINATION", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEMENT")) )
   TtaExecuteMenuAction ("CreateMCOMPLEMENT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEMENTSUB")) )
   TtaExecuteMenuAction ("CreateMCOMPLEMENTSUB", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEXCARTESIAN")) )
   TtaExecuteMenuAction ("CreateMCOMPLEXCARTESIAN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEXCARTESIAN2")) )
   TtaExecuteMenuAction ("CreateMCOMPLEXCARTESIAN2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEXES")) )
   TtaExecuteMenuAction ("CreateMCOMPLEXES", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPLEXPOLAR")) )
   TtaExecuteMenuAction ("CreateMCOMPLEXPOLAR", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPOSE")) )
   TtaExecuteMenuAction ("CreateMCOMPOSE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COMPOSEBINARY")) )
   TtaExecuteMenuAction ("CreateMCOMPOSEBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CONGRU")) )
   TtaExecuteMenuAction ("CreateMCONGRU", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CONJUGATE")) )
   TtaExecuteMenuAction ("CreateMCONJUGATE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_COUPLE")) )
   TtaExecuteMenuAction ("CreateMCOUPLE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CURL")) )
   TtaExecuteMenuAction ("CreateMCURL", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DETERMINANT")) )
   TtaExecuteMenuAction ("CreateMDETERMINANT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DETERMINANT2")) )
   TtaExecuteMenuAction ("CreateMDETERMINANT2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIAGONALINTERSECTION")) )
   TtaExecuteMenuAction ("CreateMDIAGONALINTERSECTION", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIFF")) )
   TtaExecuteMenuAction ("CreateMDIFF", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIRECTSUM")) )
   TtaExecuteMenuAction ("CreateMDIRECTSUM", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIVERGENCE")) )
   TtaExecuteMenuAction ("CreateMDIVERGENCE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DIVIDE")) )
   TtaExecuteMenuAction ("CreateMDIVIDE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_DOMAIN")) )
   TtaExecuteMenuAction ("CreateMDOMAIN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_CLASSICALFUNCTIONS")) )
   TtaExecuteMenuAction ("CreateMCLASSICALFUNCTIONS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EMPTYSET")) )
   TtaExecuteMenuAction ("CreateMEMPTYSET", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQ")) )
   TtaExecuteMenuAction ("CreateMEQ", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENT")) )
   TtaExecuteMenuAction ("CreateMEQUIVALENT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENT2")) )
   TtaExecuteMenuAction ("CreateMEQUIVALENT2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENT2BINARY")) )
   TtaExecuteMenuAction ("CreateMEQUIVALENT2BINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENTBINARY")) )
   TtaExecuteMenuAction ("CreateMEQUIVALENTBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EQUIVALENTUNDER")) )
   TtaExecuteMenuAction ("CreateMEQUIVALENTUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ENCLOSE")) )
   TtaExecuteMenuAction ("CreateMENCLOSE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EULERGAMMA")) )
   TtaExecuteMenuAction ("CreateMEULERGAMMA", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EXISTS")) )
   TtaExecuteMenuAction ("CreateMEXISTS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_EXPONENTIALE")) )
   TtaExecuteMenuAction ("CreateMEXPONENTIALE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FACTORIAL")) )
   TtaExecuteMenuAction ("CreateMFACTORIAL", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FACTOROF")) )
   TtaExecuteMenuAction ("CreateMFACTOROF", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FALSE")) )
   TtaExecuteMenuAction ("CreateMFALSE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FENCE2")) )
   TtaExecuteMenuAction ("CreateMFENCE2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FLOOR")) )
   TtaExecuteMenuAction ("CreateMFLOOR", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FORALL")) )
   TtaExecuteMenuAction ("CreateMFORALL", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GCD")) )
   TtaExecuteMenuAction ("CreateMGCD", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GEQ")) )
   TtaExecuteMenuAction ("CreateMGEQ", doc, view, FALSE);  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GEQBINARY")) )
   TtaExecuteMenuAction ("CreateMGEQBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GRAD")) )
   TtaExecuteMenuAction ("CreateMGRAD", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_GT")) )
   TtaExecuteMenuAction ("CreateMGT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IDENT")) )
   TtaExecuteMenuAction ("CreateMIDENT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IMAGE")) )
   TtaExecuteMenuAction ("CreateMIMAGE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IMAGINARY")) )
   TtaExecuteMenuAction ("CreateMIMAGINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IMAGINARYI")) )
   TtaExecuteMenuAction ("CreateMIMAGINARYI", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IMPLIES")) )
   TtaExecuteMenuAction ("CreateMIMPLIES", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_IN")) )
   TtaExecuteMenuAction ("CreateMIN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INF")) )
   TtaExecuteMenuAction ("CreateMINF", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INFINITY")) )
   TtaExecuteMenuAction ("CreateMINFINITY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INFUNDER")) )
   TtaExecuteMenuAction ("CreateMINFUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INT2")) )
   TtaExecuteMenuAction ("CreateMINT2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTEGERS")) )
   TtaExecuteMenuAction ("CreateMINTEGERS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTERSECT")) )
   TtaExecuteMenuAction ("CreateMINTERSECT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTERSECTBINARY")) )
   TtaExecuteMenuAction ("CreateMINTERSECTBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTERSECTUNDER")) )
   TtaExecuteMenuAction ("CreateMINTERSECTUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INTUNDER")) )
   TtaExecuteMenuAction ("CreateMINTUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_INVERSE")) )
   TtaExecuteMenuAction ("CreateMINVERSE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ISOMORPHIC")) )
   TtaExecuteMenuAction ("CreateMISOMORPHIC", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LAPLACIAN")) )
   TtaExecuteMenuAction ("CreateMLAPLACIAN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LEQ")) )
   TtaExecuteMenuAction ("CreateMLCM", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LEQ")) )
   TtaExecuteMenuAction ("CreateMLEQ", doc, view, FALSE);  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LEQBINARY")) )
   TtaExecuteMenuAction ("CreateMLEQBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LISTEXTENSION")) )
   TtaExecuteMenuAction ("CreateMLISTEXTENSION", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LISTSEPARATION")) )
   TtaExecuteMenuAction ("CreateMLISTSEPARATION", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LAMBDA")) )
   TtaExecuteMenuAction ("CreateMLAMBDA", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LIM")) )
   TtaExecuteMenuAction ("CreateMLIM", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LIMTENDSTO")) )
   TtaExecuteMenuAction ("CreateMLIMTENDSTO", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_LT")) )
   TtaExecuteMenuAction ("CreateMLT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MAP")) )
   TtaExecuteMenuAction ("CreateMMAP", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MATRIX2")) )
   TtaExecuteMenuAction ("CreateMMATRIX2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MAX")) )
   TtaExecuteMenuAction ("CreateMMAX", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MAXUNDER")) )
   TtaExecuteMenuAction ("CreateMMAXUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MEAN")) )
   TtaExecuteMenuAction ("CreateMMEAN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MEDIAN")) )
   TtaExecuteMenuAction ("CreateMMEDIAN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MIN")) )
   TtaExecuteMenuAction ("CreateMMIN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MINUNDER")) )
   TtaExecuteMenuAction ("CreateMMINUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MINUSBINARY")) )
   TtaExecuteMenuAction ("CreateMMINUSBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MINUSUNARY")) )
   TtaExecuteMenuAction ("CreateMMINUSUNARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MODE")) )
   TtaExecuteMenuAction ("CreateMMODE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MOMENT")) )
   TtaExecuteMenuAction ("CreateMMOMENT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NATURALS")) )
   TtaExecuteMenuAction ("CreateMNATURALS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NEQ")) )
   TtaExecuteMenuAction ("CreateMNEQ", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOT")) )
   TtaExecuteMenuAction ("CreateMNOT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOTANUMBER")) )
   TtaExecuteMenuAction ("CreateMNOTANUMBER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOTIN")) )
   TtaExecuteMenuAction ("CreateMNOTIN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOTPRSUBSET")) )
   TtaExecuteMenuAction ("CreateMNOTPRSUBSET", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NOTSUBSET")) )
   TtaExecuteMenuAction ("CreateMNOTSUBSET", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_NUPLET")) )
   TtaExecuteMenuAction ("CreateMNUPLET", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OMEGASUB")) )
   TtaExecuteMenuAction ("CreateMOMEGASUB", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OR")) )
   TtaExecuteMenuAction ("CreateMOR", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ORBINARY")) )
   TtaExecuteMenuAction ("CreateMORBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ORTHOGONAL")) )
   TtaExecuteMenuAction ("CreateMORTHOGONAL", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ORTHOGONALCOMPLEMENT")) )
   TtaExecuteMenuAction ("CreateMORTHOGONALCOMPLEMENT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OUTERPRODUCT")) )
   TtaExecuteMenuAction ("CreateMOUTERPRODUCT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PARTIALDIFF")) )
   TtaExecuteMenuAction ("CreateMPARTIALDIFF", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PARTIALDIFF2")) )
   TtaExecuteMenuAction ("CreateMPARTIALDIFF2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PI")) )
   TtaExecuteMenuAction ("CreateMPI", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PIECEWISE")) )
   TtaExecuteMenuAction ("CreateMPIECEWISE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PLUS")) )
   TtaExecuteMenuAction ("CreateMPLUS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_POWER")) )
   TtaExecuteMenuAction ("CreateMPOWER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_POWERSET")) )
   TtaExecuteMenuAction ("CreateMPOWERSET", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRIMES")) )
   TtaExecuteMenuAction ("CreateMPRIMES", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRODUNDER")) )
   TtaExecuteMenuAction ("CreateMPRODUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRODUNDEROVER")) )
   TtaExecuteMenuAction ("CreateMPRODUNDEROVER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRSUBSET")) )
   TtaExecuteMenuAction ("CreateMPRSUBSET", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_PRSUBSETBINARY")) )
   TtaExecuteMenuAction ("CreateMPRSUBSETBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_QUOTIENT")) )
   TtaExecuteMenuAction ("CreateMQUOTIENT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_RATIONNALS")) )
   TtaExecuteMenuAction ("CreateMRATIONNALS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_REAL")) )
   TtaExecuteMenuAction ("CreateMREAL", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_REALS")) )
   TtaExecuteMenuAction ("CreateMREALS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_REM")) )
   TtaExecuteMenuAction ("CreateMREM", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SCALARPRODUCT")) )
   TtaExecuteMenuAction ("CreateMSCALARPRODUCT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SDEV")) )
   TtaExecuteMenuAction ("CreateMSDEV", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SELECTOR")) )
   TtaExecuteMenuAction ("CreateMSELECTOR", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SETDIFF")) )
   TtaExecuteMenuAction ("CreateMSETDIFF", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SETEXTENSION")) )
   TtaExecuteMenuAction ("CreateMSETEXTENSION", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SETSEPARATION")) )
   TtaExecuteMenuAction ("CreateMSETSEPARATION", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SETSYMDIFF")) )
   TtaExecuteMenuAction ("CreateMSETSYMDIFF", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUBSET")) )
   TtaExecuteMenuAction ("CreateMSUBSET", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUBSETBINARY")) )
   TtaExecuteMenuAction ("CreateMSUBSETBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUMUNDER")) )
   TtaExecuteMenuAction ("CreateMSUMUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUP2")) )
   TtaExecuteMenuAction ("CreateMSUP2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUPMINUS")) )
   TtaExecuteMenuAction ("CreateMSUPMINUS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUPPLUS")) )
   TtaExecuteMenuAction ("CreateMSUPPLUS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUPUNDER")) )
   TtaExecuteMenuAction ("CreateMSUPUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TENDSTO")) )
   TtaExecuteMenuAction ("CreateMTENDSTO", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TENDSTOTENDSTO")) )
   TtaExecuteMenuAction ("CreateMTENDSTOTENDSTO", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TIMES")) )
   TtaExecuteMenuAction ("CreateMTIMES", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TIMESBINARY")) )
   TtaExecuteMenuAction ("CreateMTIMESBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TRANSPOSE")) )
   TtaExecuteMenuAction ("CreateMTRANSPOSE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_TRUE")) )
   TtaExecuteMenuAction ("CreateMTRUE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNION")) )
   TtaExecuteMenuAction ("CreateMUNION", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNIONUNARY")) )
   TtaExecuteMenuAction ("CreateMUNIONUNARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNIONUNDER")) )
   TtaExecuteMenuAction ("CreateMUNIONUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNIONBINARY")) )
   TtaExecuteMenuAction ("CreateMUNIONBINARY", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_VARIANCE")) )
   TtaExecuteMenuAction ("CreateMVARIANCE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_VECTORPRODUCT")) )
   TtaExecuteMenuAction ("CreateMVECTORPRODUCT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_VECTORROW")) )
   TtaExecuteMenuAction ("CreateMVECTORROW", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_VECTORCOLUMN")) )
   TtaExecuteMenuAction ("CreateMVECTORCOLUMN", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_XOR")) )
   TtaExecuteMenuAction ("CreateMXOR", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_XORBINARY")) )
   TtaExecuteMenuAction ("CreateMXORBINARY", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
  -----------------------------------------------------------------------*/
void AmayaMathMLPanel::SendDataToPanel( AmayaParams& p )
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
 *       Class:  AmayaMathMLPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
  -----------------------------------------------------------------------*/
void AmayaMathMLPanel::DoUpdate()
{
  AmayaSubPanel::DoUpdate();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  IsActive
 * Description:  
  -----------------------------------------------------------------------*/
bool AmayaMathMLPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaMathMLPanel, AmayaSubPanel)
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_BMATH"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FENCE"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SQRT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ROOT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FRAC"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MATRIX"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNDER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERUNDER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MSCRIPT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUB"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUP"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUBSUP"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTEGRAL"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUM"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MATRIX2"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERARROW"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERBAR"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERBREVE"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERCHECK"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERDOT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERHAT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERTILDE"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ABS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ALEPHSUB"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_AND"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ANDBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_APPROX"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARG"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARROW1"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ARROW2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CARD"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CARD2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CARTESIANPRODUCT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CARTESIANPRODUCTBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CEILING"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CODOMAIN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMBINATION"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEMENT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEMENTSUB"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEXCARTESIAN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEXCARTESIAN2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEXES"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPLEXPOLAR"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPOSE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COMPOSEBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CONGRU"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CONJUGATE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_COUPLE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CURL"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_DETERMINANT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_DETERMINANT2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIAGONALINTERSECTION"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIFF"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIRECTSUM"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIVERGENCE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_DIVIDE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_DOMAIN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_CLASSICALFUNCTIONS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EMPTYSET"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQ"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENT2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENT2BINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENTBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EQUIVALENTUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ENCLOSE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EULERGAMMA"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EXISTS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_EXPONENTIALE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FACTORIAL"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FACTOROF"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FALSE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FENCE2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FLOOR"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FORALL"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_GCD"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_GEQ"), AmayaMathMLPanel::OnButton )    EVT_BUTTON( XRCID("wxID_PANEL_MATH_GEQBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_GRAD"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_GT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_IDENT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_IMAGE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_IMAGINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_IMAGINARYI"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_IMPLIES"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_IN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INF"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INFINITY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INFUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INT2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTEGERS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTERSECT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTERSECTBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTERSECTUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INTUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_INVERSE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ISOMORPHIC"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LAPLACIAN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LCM"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LEQ"), AmayaMathMLPanel::OnButton )    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LEQBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LISTEXTENSION"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LISTSEPARATION"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LAMBDA"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LIM"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LIMTENDSTO"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_LT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MAP"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MATRIX2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MAX"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MAXUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MEAN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MEDIAN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MIN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MINUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MINUSBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MINUSUNARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MODE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MOMENT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_NATURALS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_NEQ"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOTANUMBER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOTIN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOTPRSUBSET"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_NOTSUBSET"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_NUPLET"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OMEGASUB"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OR"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ORBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ORTHOGONAL"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ORTHOGONALCOMPLEMENT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OUTERPRODUCT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PARTIALDIFF"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PARTIALDIFF2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PI"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PIECEWISE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PLUS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_POWER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_POWERSET"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRIMES"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRODUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRODUNDEROVER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRSUBSET"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_PRSUBSETBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_QUOTIENT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_RATIONNALS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_REAL"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_REALS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_REM"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SCALARPRODUCT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SDEV"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SELECTOR"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SETDIFF"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SETEXTENSION"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SETSEPARATION"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SETSYMDIFF"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUBSET"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUBSETBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUMUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUP2"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUPMINUS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUPPLUS"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUPUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_TENDSTO"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_TENDSTOTENDSTO"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_TIMES"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_TIMESBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_TRANSPOSE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_TRUE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNION"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNIONUNARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNIONUNDER"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNIONBINARY"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_VARIANCE"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_VECTORPRODUCT"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_VECTORROW"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_VECTORCOLUMN"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_XOR"), AmayaMathMLPanel::OnButton )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_XORBINARY"), AmayaMathMLPanel::OnButton )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
