/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
#include "profiles_f.h"
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

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
AmayaElementToolPanel::AmayaElementToolPanel():
  AmayaToolPanel(),
  m_xml(NULL)
{
  for (int i = 0; i < WXAMAYA_DOCTYPE_NUMBER; i++)
    m_panelIndexes[i] = wxNOT_FOUND;
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
AmayaElementToolPanel::~AmayaElementToolPanel()
{
	// Windows : delete notebook pages now to prevent a crash
	if (m_notebook)
		while (m_notebook->GetPageCount() > 0)
		{
			m_notebook->GetPage(0)->Hide();
			m_notebook->DeletePage(0);
		}
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
bool AmayaElementToolPanel::Create(wxWindow* parent, wxWindowID id,
                                   const wxPoint& pos, 
                                   const wxSize& size, long style,
                                   const wxString& name, wxObject* extra)
{
  if (! wxPanel::Create(parent, id, pos, size, style, name))
    return false;
#ifdef _WINDOWS
  SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */
  m_notebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP);
  wxSizer* sz = new wxBoxSizer(wxVERTICAL);
  sz->Add(m_notebook, 1, wxEXPAND);
  SetSizer(sz);

  wxWindow* win;
  
  if (Prof_ShowGUI("AmayaXHTMLPanel"))
    {
      m_notebook->AddPage(win = new AmayaXHTMLPanel(m_notebook, wxID_ANY), wxT(""), false, wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_html.png"), wxBITMAP_TYPE_PNG));
      m_panelIndexes[WXAMAYA_DOCTYPE_XHTML] = m_notebook->GetPageIndex(win);
    }
  if (Prof_ShowGUI("AmayaMathMLPanel"))
    {
      m_notebook->AddPage(win = new AmayaMathMLPanel(m_notebook, wxID_ANY), wxT(""), false, wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_math.png"), wxBITMAP_TYPE_PNG));
      m_panelIndexes[WXAMAYA_DOCTYPE_MATHML] = m_notebook->GetPageIndex(win);      
    }
  if (Prof_ShowGUI("AmayaSVGPanel"))
    {
      m_notebook->AddPage(win = new AmayaSVGPanel(m_notebook, wxID_ANY), wxT(""), false, wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_svg.png"), wxBITMAP_TYPE_PNG));
      m_panelIndexes[WXAMAYA_DOCTYPE_SVG] = m_notebook->GetPageIndex(win);
    }
  if (Prof_ShowGUI("AmayaTemplatePanel"))
    {
      m_notebook->AddPage(win = new AmayaTemplatePanel(m_notebook, wxID_ANY), wxT(""), false, wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_template.png"), wxBITMAP_TYPE_PNG));
      m_panelIndexes[WXAMAYA_DOCTYPE_XTIGER] = m_notebook->GetPageIndex(win);
    }
  if (Prof_ShowGUI("AmayaXMLPanel"))
    {
      m_notebook->AddPage(m_xml = new AmayaXMLPanel(m_notebook, wxID_ANY), wxT(""), false, wxBitmap(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, "document_xml.png"), wxBITMAP_TYPE_PNG));
      m_panelIndexes[WXAMAYA_DOCTYPE_XML] = m_notebook->GetPageIndex(m_xml);
    }
 
  return true;
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
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
 *      Method:  PanelTypeToDocType
 * Description:  Get the doctype index for specified panel type
  -----------------------------------------------------------------------*/
int AmayaElementToolPanel::PanelTypeToDocType(int panel_type)const
{
  switch(panel_type)
  {
    case WXAMAYA_PANEL_XHTML:
      return WXAMAYA_DOCTYPE_XHTML;
    case WXAMAYA_PANEL_MATHML:
      return WXAMAYA_DOCTYPE_MATHML;
    case WXAMAYA_PANEL_SVG:
      return WXAMAYA_DOCTYPE_SVG;
    case WXAMAYA_PANEL_XML:
      return WXAMAYA_DOCTYPE_XML;
    default:
      return wxNOT_FOUND;
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaElementToolPanel
 *      Method:  PanelTypeToIndex
 * Description:  Get the panel index for specified type
  -----------------------------------------------------------------------*/
int AmayaElementToolPanel::PanelTypeToIndex(int panel_type)const
{
  return DocTypeToIndex(PanelTypeToDocType(panel_type));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaElementToolPanel
 *      Method:  PanelTypeToIndex
 * Description:  Get the panel index for specified type
  -----------------------------------------------------------------------*/
int AmayaElementToolPanel::DocTypeToIndex(int doctype)const
{
  if (doctype >= 0 && doctype < WXAMAYA_DOCTYPE_NUMBER)
    return m_panelIndexes[doctype];
  else
    return wxNOT_FOUND;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaElementToolPanel
 *      Method:  RaisePanel
 * Description:  Raise the specified panel
  -----------------------------------------------------------------------*/
void AmayaElementToolPanel::RaisePanel(int panel_type)
{
  int index = PanelTypeToIndex(panel_type);
  if(index != wxNOT_FOUND)
    m_notebook->SetSelection(index);
}


/*----------------------------------------------------------------------
 *       Class:  AmayaElementToolPanel
 *      Method:  RaiseDoctypePanels
 * Description:  Raiser the specified panels
 -----------------------------------------------------------------------*/
void AmayaElementToolPanel::RaiseDoctypePanels(int doctype)
{
  int index = DocTypeToIndex(doctype);
  if (index != wxNOT_FOUND)
    m_notebook->SetSelection(index);

}




/*----------------------------------------------------------------------
 *       Class:  AmayaElementToolPanel
 *      Method:  wxSize DoGetBestSize() const;
 * Description:  Compute the best size of the panel
  -----------------------------------------------------------------------*/
wxSize AmayaElementToolPanel::DoGetBestSize() const
{
  wxSize sz(0,0);
  for(int i=0; i<(int)m_notebook->GetPageCount(); i++)
    {
      wxWindow* win = m_notebook->GetPage(i);
      if(sz.x<win->GetBestSize().x)
         sz.x = win->GetBestSize().x;
      if(sz.y<win->GetBestSize().y)
         sz.y = win->GetBestSize().y;
    }
  sz += wxSize(8, 32);
  return sz;
}

#endif /* #ifdef _WX */
