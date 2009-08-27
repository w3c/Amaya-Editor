/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"

#include "registry_wx.h"
#include "message_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"


#include "AmayaPanel.h"
#include "AmayaToolPanel.h"

// Set of registered AmayaToolPanel class infos.
ClassInfoSet g_AmayaToolPanelClassInfoSet; 


//
//
// AmayaToolPanelBar
//
//

IMPLEMENT_DYNAMIC_CLASS(AmayaToolPanelBar, wxPanel)

BEGIN_EVENT_TABLE(AmayaToolPanelBar, wxPanel)
  EVT_BUTTON(XRCID("wxID_BUTTON_CLOSE"), AmayaToolPanelBar::OnClose)
END_EVENT_TABLE()


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanelBar::AmayaToolPanelBar():
wxPanel()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanelBar::AmayaToolPanelBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style, const wxString& name):
wxPanel()
{
  Create(parent, id, pos, size, style, name);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanelBar::~AmayaToolPanelBar()
{
  // Show or hide panel using env
  unsigned int n, max = m_items.GetCount();
  for (n = 0; n < max; n++)
    {
      delete m_items[n];
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style, const wxString& name)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_PANEL")))
    return false;
  
  XRCCTRL(*this, "wxID_LABEL_TOOLS", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_TOOLS)));
  XRCCTRL(*this, "wxID_BUTTON_CLOSE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DONE)));

  m_scwin = XRCCTRL(*this, "wxID_PANEL_SWIN", wxScrolledWindow);
  
  // Hide bar title.
  HideHeader();
  
  return true;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::OnClose( wxCommandEvent& event )
{
  Hide();
  if(GetParent())
    GetParent()->Layout();
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::AddPanel(AmayaToolPanel* panel)
{

  AmayaToolPanelItem* item = new AmayaToolPanelItem(panel, m_scwin);
  m_scwin->GetSizer()->Add(item, 0, wxEXPAND);
  panel->SetWindow((AmayaNormalWindow*)this->GetParent());
  m_items.Add(item);
  return true;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::LoadConfig()
{
  // Show or hide panel using env
  unsigned int n, max = m_items.GetCount();

  for (n = 0; n < max; n++)
    {
      AmayaToolPanelItem* item = m_items[n];
      if (item)
        {
          ThotBool value;
          wxString str = wxT("OPEN_") + item->GetPanel()->GetToolPanelConfigKeyName();
          TtaGetEnvBoolean((char*)(const char*)str.mb_str(wxConvUTF8), &value);
          MinimizePanel(item->GetPanel(), !value);
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::SaveConfig()
{
  unsigned int n, max = m_items.GetCount();
  for (n = 0; n < max; n++)
    {
      AmayaToolPanelItem* item = m_items[n];
      if(item)
        {
          ThotBool value = IsExpanded(item->GetPanel());
          wxString str = wxT("OPEN_") + item->GetPanel()->GetToolPanelConfigKeyName();
          TtaSetEnvBoolean((char*)(const char*)str.mb_str(wxConvUTF8), value, TRUE);
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::ShowHeader(bool bShow)
{
  wxSizer* sz = GetSizer();
  if(sz)
      sz->Show((size_t)0, bShow);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
const AmayaToolPanelItem* AmayaToolPanelBar::FindItem(const AmayaToolPanel* panel)const
{
  unsigned int n, max = m_items.GetCount();
  for (n = 0; n < max; n++)
    {
      AmayaToolPanelItem *item = m_items[n];
      if (item->GetPanel() == panel)
        {
          return item;
        }
    }
  return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanelItem* AmayaToolPanelBar::FindItem(const AmayaToolPanel* panel)
{
  unsigned int n, max = m_items.GetCount();
  for (n = 0; n < max; n++)
    {
      AmayaToolPanelItem *item = m_items[n];
      if (item->GetPanel() == panel)
        {
          return item;
        }
    }
  return NULL;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::MinimizePanel(AmayaToolPanel* panel, bool bMin)
{
  AmayaToolPanelItem* item = FindItem(panel);
  if (item)
    {
      item->Minimize(bMin);
      Layout();
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::ToggleMinimize(AmayaToolPanel* panel)
{
  AmayaToolPanelItem* item = FindItem(panel);
  if(item)
      MinimizePanel(panel, !item->IsMinimized());
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::IsMinimized(const AmayaToolPanel* panel)const
{
  const AmayaToolPanelItem* item = FindItem(panel);
  if(item)
    return item->IsMinimized();
  return false;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::IsExpanded(const AmayaToolPanel* panel)const
{
  return !IsMinimized(panel);  
}



/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanel* AmayaToolPanelBar::GetToolPanel(int type)
{
  unsigned int n, max = m_items.GetCount();
  for (n = 0; n < max; n++)
    {
      AmayaToolPanelItem* item = m_items[n];
      if (item->GetPanel()->GetToolPanelType() == type)
        {
          return item->GetPanel();
        }
    }
  return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::OpenToolPanel( int panel_type, bool bOpen)
{
  AmayaToolPanel* panel = GetToolPanel(panel_type);
  if(panel)
    MinimizePanel(panel, !bOpen);
}



//
//
// AmayaToolPanelItem
//
//

wxBitmap AmayaToolPanelItem::s_Bitmap_Minimized;
wxBitmap AmayaToolPanelItem::s_Bitmap_Expanded;
bool AmayaToolPanelItem::s_bmpOk = false;

BEGIN_EVENT_TABLE(AmayaToolPanelItem, wxPanel)
  EVT_BUTTON( XRCID("wxID_BUTTON_EXPAND"), AmayaToolPanelItem::OnMinimize)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanelItem::AmayaToolPanelItem(
    AmayaToolPanel* panel, 
    wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style):
wxPanel(),
m_panel(panel),
m_bMinimized(false)
{
  if(!s_bmpOk)
    {
      s_Bitmap_Expanded  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_MISC, "expand_on.png" ), wxBITMAP_TYPE_PNG);
      s_Bitmap_Minimized = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_MISC, "expand_off.png" ), wxBITMAP_TYPE_PNG);
      s_bmpOk = true;
    }
  
  wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, 
                                        wxT("wxID_DOCKED_TOOL_PANEL_CONTAINER"));
  
  XRCCTRL(*this, "wxID_LABEL_TITLE", wxStaticText)->SetLabel(panel->GetToolPanelName());
  
  panel->Create(this, wxID_ANY);  
  GetSizer()->Add(panel, 0, wxEXPAND);
  GetSizer()->Layout();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanelItem::~AmayaToolPanelItem()
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelItem::Minimize(bool bMinimize)
{
  wxSizer* sz = GetSizer();
  Document doc;
  int      view;

  if (sz->GetItem(1))
    {
      m_bMinimized = bMinimize;
      sz->Show((size_t)1, !bMinimize);
      if (bMinimize)
        XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton)->SetBitmapLabel( s_Bitmap_Minimized );
      else
        {
        XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton)->SetBitmapLabel( s_Bitmap_Expanded );
        // test if the attribute panel should be updated
        if (m_panel && m_panel->GetToolPanelType() == WXAMAYA_PANEL_ATTRIBUTE)
          {
            TtaGetActiveView (&doc, &view);
            if (doc)
              TtaUpdateAttrMenu (doc);
          }
        }
      GetParent()->Layout();
      return true;
    }
  else
    {
      return false;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelItem::OnMinimize(wxCommandEvent& event)
{
  Minimize(!IsMinimized());
}













//
//
// AmayaToolPanel
//
//

IMPLEMENT_ABSTRACT_CLASS(AmayaToolPanel, wxPanel)

BEGIN_EVENT_TABLE(AmayaToolPanel, wxPanel)
END_EVENT_TABLE()


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanel::AmayaToolPanel():
  wxPanel(),
  m_bar(NULL),
  m_ShouldBeUpdated(false),
  m_bExpanded(true),
  m_bFloating(false),
  m_bVisible(true)
{
  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanel::~AmayaToolPanel()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int AmayaToolPanel::GetToolPanelType()const
{
  return WXAMAYA_PANEL_UNKNOWN;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString AmayaToolPanel::GetToolPanelConfigKeyName()const
{
  return wxT("PANEL");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanel::ShouldBeUpdated( bool should_update )
{
  m_ShouldBeUpdated = should_update;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanel::DoUpdate()
{
  if (m_ShouldBeUpdated)
    {
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaToolPanel::DoUpdate"));
      m_ShouldBeUpdated = false;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanel::SendDataToPanel( AmayaParams& p )
{
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaToolPanel::SendDataToPanel"));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanel::SetColor (int color)
{
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaToolPanel::SetColor"));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanel::Layout()
{
  wxPanel::Layout();
  wxGetTopLevelParent(this)->Layout();
  return true;
}


/**
 * Function to register a new AmayaToolPanel class.
 */
void RegisterToolPanelClass(wxClassInfo* ci)
{
  g_AmayaToolPanelClassInfoSet.insert(ci);
}


#endif /* #ifdef _WX */
