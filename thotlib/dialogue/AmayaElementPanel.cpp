#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/notebook.h"

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
#include "displayview_f.h"
#include "registry_wx.h"
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"

#include "AmayaElementPanel.h"
#include "AmayaNormalWindow.h"


#include "AmayaXHTMLPanel.h"
#include "AmayaMathMLPanel.h"
#include "AmayaSVGPanel.h"
#include "AmayaXMLPanel.h"
#include "AmayaTemplatePanel.h"



//
//
// AmayaElementToolPanel
//
//


IMPLEMENT_DYNAMIC_CLASS(AmayaElementToolPanel, AmayaToolPanel)

AmayaElementToolPanel::AmayaElementToolPanel():
  AmayaToolPanel(),
  m_imageList(16, 16),
  m_xml(NULL)
{
}

AmayaElementToolPanel::~AmayaElementToolPanel()
{
}

bool AmayaElementToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(! wxPanel::Create(parent, id, pos, size, style, name))
    return false;

  m_imageList.Add(wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_html.png"), wxBITMAP_TYPE_PNG));
  m_imageList.Add(wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_math.png"), wxBITMAP_TYPE_PNG));
  m_imageList.Add(wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_xml.png"), wxBITMAP_TYPE_PNG));
  m_imageList.Add(wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_svg.png"), wxBITMAP_TYPE_PNG));
  m_imageList.Add(wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_template.png"), wxBITMAP_TYPE_PNG));
  
  m_notebook = new wxNotebook(this, wxID_ANY);
  m_notebook->SetImageList(&m_imageList);
  wxSizer* sz = new wxBoxSizer(wxVERTICAL);
  sz->Add(m_notebook, 1, wxEXPAND);
  SetSizer(sz);
  
  m_notebook->AddPage(new AmayaXHTMLPanel(m_notebook, wxID_ANY), wxT(""), false, 0);
  m_notebook->AddPage(new AmayaMathMLPanel(m_notebook, wxID_ANY), wxT(""), false, 1);
  m_notebook->AddPage(m_xml = new AmayaXMLPanel(m_notebook, wxID_ANY), wxT(""), false, 2);
  m_notebook->AddPage(new AmayaSVGPanel(m_notebook, wxID_ANY), wxT(""), false, 3);
  m_notebook->AddPage(new AmayaTemplatePanel(m_notebook, wxID_ANY), wxT(""), false, 4);
  return true;
}

wxString AmayaElementToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ELEMENTS));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaElementToolPanel
 *      Method:  GetDefaultAUIConfig
 * Description:  Return a default AUI config for the panel.
 -----------------------------------------------------------------------*/
wxString AmayaElementToolPanel::GetDefaultAUIConfig()
{
  return wxT("dir=2;layer=0;row=0;pos=0");
}

/*----------------------------------------------------------------------
 *       Class:  AmayaElementToolPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
  -----------------------------------------------------------------------*/
void AmayaElementToolPanel::SendDataToPanel(int panel_type, AmayaParams& p )
{
  switch(panel_type)
  {
    case WXAMAYA_PANEL_XML:
      if(m_xml)
        m_xml->SendDataToPanel(p);
      break;
    default:
      break;
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaElementToolPanel
 *      Method:  PanelTypeToIndex
 * Description:  Get the panel index for specified type
  -----------------------------------------------------------------------*/
int AmayaElementToolPanel::PanelTypeToIndex(int panel_type)const
{
  switch(panel_type)
  {
    case WXAMAYA_PANEL_XHTML:
      return 0;
    case WXAMAYA_PANEL_MATHML:
      return 1;
    case WXAMAYA_PANEL_XML:
      return 2;
    case WXAMAYA_PANEL_SVG:
      return 3;
    default:
      return wxNOT_FOUND;
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaElementToolPanel
 *      Method:  RaisePanel
 * Description:  Raise the specified panel
  -----------------------------------------------------------------------*/
void AmayaElementToolPanel::RaisePanel(int panel_type)
{
  int index = PanelTypeToIndex(panel_type);
  if(index!=wxNOT_FOUND)
    m_notebook->ChangeSelection(index);
}

#endif /* #ifdef _WX */
