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
#include "view.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "attrmenu_f.h"
#include "frame_tv.h"

#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"


#include "AmayaPanel.h"
#include "AmayaXHTMLPanel.h"
#include "AmayaAttributePanel.h"
#include "AmayaNormalWindow.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaPanel, wxPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  AmayaPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaPanel::AmayaPanel( wxWindow *      p_parent_window
			,AmayaNormalWindow * p_parent_nwindow
			,wxWindowID     id
			,const wxPoint& pos
			,const wxSize&  size
			,long style
			)
  :  wxPanel( wxDynamicCast( p_parent_window, wxWindow ),
	      id, pos, size, style )
     ,m_pPanel_xhtml(NULL)
     ,m_pParentNWindow(p_parent_nwindow)
{
  wxLogDebug( _T("AmayaPanel::AmayaPanel") );

  // load title area
  m_pTitlePanel = wxXmlResource::Get()->LoadPanel(this, _T("wxID_TITLEPANEL"));
 
  // load static sub-panels  
  m_pPanel_xhtml     = new AmayaXHTMLPanel( this, p_parent_nwindow );
  m_pPanel_attribute = new AmayaAttributePanel( this, p_parent_nwindow );

  // attach subpanels & title to the panel
  wxBoxSizer * p_TopSizer = new wxBoxSizer ( wxVERTICAL );
  SetSizer(p_TopSizer);
  p_TopSizer->Add( m_pTitlePanel, 0, wxALL | wxEXPAND , 5 );
  p_TopSizer->Add( m_pPanel_xhtml, 0, wxALL | wxEXPAND , 5 );
  p_TopSizer->Add( m_pPanel_attribute, 0, wxALL | wxEXPAND , 5 );
  
  // setup labels
  XRCCTRL(*this, "wxID_LABEL_TOOLS", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_TOOLS)));
  XRCCTRL(*this, "wxID_BUTTON_CLOSE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DONE)));

  Layout();
  SetAutoLayout(TRUE);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  ~AmayaPanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaPanel::~AmayaPanel()
{
  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  HideWhenUnsplit
 * Description:  must hide the panel & his childs
 *--------------------------------------------------------------------------------------
 */
void AmayaPanel::ShowWhenUnsplit( bool show )
{
  wxLogDebug( _T("AmayaPanel::ShowWhenUnsplit") );
  if (!show)
    {
      Hide();
      m_pPanel_xhtml->Hide();
   }
  else
    {
      Show();
      m_pPanel_xhtml->Show();
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnClose
 * Description:  this method is called when the button for quick split is pushed
 *--------------------------------------------------------------------------------------
 */
void AmayaPanel::OnClose( wxCommandEvent& event )
{
  if ( event.GetId() != XRCCTRL(*this, "wxID_BUTTON_CLOSE", wxBitmapButton)->GetId() )
    {
      event.Skip();
      return;
    }
  m_pParentNWindow->ClosePanel();  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  GetXHTMLPanel
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaXHTMLPanel * AmayaPanel::GetXHTMLPanel() const
{
  return m_pPanel_xhtml;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  GetAttributePanel
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaAttributePanel * AmayaPanel::GetAttributePanel() const
{
  return m_pPanel_attribute;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaPanel::RefreshToolTips()
{
  m_pPanel_xhtml->RefreshToolTips();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPanel, wxPanel)
  EVT_BUTTON( -1, AmayaPanel::OnClose)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
