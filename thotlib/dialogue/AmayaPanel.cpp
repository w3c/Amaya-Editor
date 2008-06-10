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
  unsigned int n;
  for(n=0; n<m_panels.GetCount(); n++)
    {
      delete m_panels[n];
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style, const wxString& name)
{
  printf("AmayaToolPanelBar::Create\n");
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_PANEL")))
    return false;
  
  XRCCTRL(*this, "wxID_LABEL_TOOLS", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_TOOLS)));
  XRCCTRL(*this, "wxID_BUTTON_CLOSE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DONE)));
  
  // To remove to show bar title.
  wxSizer* sz = GetSizer();
  if(sz)
    {
      sz->Show((size_t)0, false);
      sz->Show((size_t)1, false);
    }
  
  Initialize();

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
void AmayaToolPanelBar::Initialize()
{
  wxSizer* sz = NULL;
  m_scwin = XRCCTRL(*this, "wxID_PANEL_SWIN", wxScrolledWindow);
  if(m_scwin)
    sz = m_scwin->GetSizer();
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::AddPanel(AmayaToolPanel* panel)
{
  AmayaToolPanelBarListItem* item = new AmayaToolPanelBarListItem;
  item->ci        = panel->GetClassInfo();
  item->panel     = panel;
  item->dock      = new AmayaDockedToolPanelContainer(panel, this, m_scwin, wxID_ANY);
  item->floa      = NULL;
  item->shown     = true;
  item->floating  = false;
  item->minimized = false;
  m_panels.Add(item);
  item->panel->Create(m_scwin, wxID_ANY);
  item->panel->SetExapndedFlag(true);
  item->panel->SetFloatingFlag(false);
  item->panel->SetVisibleFlag(true);
  item->panel->SetWindow((AmayaNormalWindow*)this->GetParent());
  item->dock->Attach();
  m_scwin->GetSizer()->Add(item->dock, 0, wxEXPAND|wxBOTTOM, 0);
  return true;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::LoadConfig()
{
  // Show or hide panel using env
  unsigned int n;
  for(n=0; n<m_panels.GetCount(); n++)
    {
      AmayaToolPanelBarListItem &item = *(m_panels[n]);
      if (item.panel)
        {
          ThotBool value;
          wxString str = wxT("OPEN_") + item.panel->GetToolPanelConfigKeyName();
          TtaGetEnvBoolean((char*)(const char*)str.mb_str(wxConvUTF8), &value);
          MinimizePanel(item.panel, !value);
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::SaveConfig()
{
  unsigned int n;
  for(n=0; n<m_panels.GetCount(); n++)
    {
      AmayaToolPanelBarListItem &item = *(m_panels[n]);
      if(item.panel)
        {
          ThotBool value = IsExpanded(item.panel);
          wxString str = wxT("OPEN_") + item.panel->GetToolPanelConfigKeyName();
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
    {
      sz->Show((size_t)0, bShow);
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
const AmayaToolPanelBarListItem* AmayaToolPanelBar::FindItem(const AmayaToolPanel* panel)const
{
  unsigned int n;
  for (n = 0; n < m_panels.GetCount(); n++)
    {
      AmayaToolPanelBarListItem &item = *(m_panels[n]);
      if (item.panel == panel)
        {
          return &item;
        }
    }
  return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanelBarListItem* AmayaToolPanelBar::FindItem(const AmayaToolPanel* panel)
{
  unsigned int n;
  for (n = 0; n < m_panels.GetCount(); n++)
    {
      AmayaToolPanelBarListItem &item = *(m_panels[n]);
      if (item.panel == panel)
        {
          return &item;
        }
    }
  return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::ShowPanel(AmayaToolPanel* panel, bool bShow)
{
  AmayaToolPanelBarListItem* item = FindItem(panel);
  if (item && item->shown != bShow)
    {
      item->shown = bShow;
      if(m_scwin)
        m_scwin->GetSizer()->Show(item->dock, bShow);
      if(item->floa)
        item->floa->Show(bShow);
      panel->SetVisibleFlag(bShow);
      Layout();
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::MinimizePanel(AmayaToolPanel* panel, bool bMin)
{
  AmayaToolPanelBarListItem* item = FindItem(panel);
  if (item && item->minimized != bMin && !item->floating && item->dock)
    {
      if(item->dock->Minimize(bMin))
        item->minimized = bMin;
      Layout();
      panel->SetExapndedFlag(!bMin);
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::ToggleMinimize(AmayaToolPanel* panel)
{
  AmayaToolPanelBarListItem* item = FindItem(panel);
  if(item)
      MinimizePanel(panel, !item->minimized);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::FloatPanel(AmayaToolPanel* panel, bool bFloat)
{
  AmayaToolPanelBarListItem* item = FindItem(panel);
  if(item && item->floating!=bFloat && item->dock)
    {
      item->floating = bFloat;
      if(bFloat)
        {
          item->dock->Detach();
          if(item->floa==NULL)
              item->floa = new AmayaFloatingToolPanelContainer(panel, this,
                    this, wxID_ANY);
          item->floa->Attach();
          item->floa->Show();
        }
      else
        {
          if(item->floa!=NULL)
            {
              item->floa->Detach();
              item->floa->Destroy();
              item->floa = NULL;
            }
          item->dock->Attach();
        }
      panel->SetFloatingFlag(bFloat);
      Layout();
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaToolPanelBar::ToggleFloat(AmayaToolPanel* panel)
{
  AmayaToolPanelBarListItem* item = FindItem(panel);
  if(item)
    FloatPanel(panel, !item->floating);  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::IsMinimized(const AmayaToolPanel* panel)const
{
  const AmayaToolPanelBarListItem* item = FindItem(panel);
  if(item)
    return item->minimized;
  return false;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::IsExpanded(const AmayaToolPanel* panel)const
{
  const AmayaToolPanelBarListItem* item = FindItem(panel);
  if(item)
    return !item->minimized;
  return false;  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::IsFloating(const AmayaToolPanel* panel)const
{
  const AmayaToolPanelBarListItem* item = FindItem(panel);
  if(item)
    return item->floating;
  return false;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaToolPanelBar::IsShown(const AmayaToolPanel* panel)const
{
  const AmayaToolPanelBarListItem* item = FindItem(panel);
  if(item)
    return item->shown;
  return false;  
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaToolPanel* AmayaToolPanelBar::GetToolPanel(int type)
{
  unsigned int n;
  for(n=0; n<m_panels.GetCount(); n++)
    {
      AmayaToolPanelBarListItem &item = *(m_panels[n]);
      if(item.panel->GetToolPanelType()==type)
        {
          return item.panel;
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
// AmayaDockedToolPanelContainer
//
//

wxBitmap AmayaDockedToolPanelContainer::s_Bitmap_DetachOn;
wxBitmap AmayaDockedToolPanelContainer::s_Bitmap_DetachOff;
wxBitmap AmayaDockedToolPanelContainer::s_Bitmap_ExpandOn;
wxBitmap AmayaDockedToolPanelContainer::s_Bitmap_ExpandOff;
bool AmayaDockedToolPanelContainer::s_bmpOk = false;

BEGIN_EVENT_TABLE(AmayaDockedToolPanelContainer, wxPanel)
  EVT_BUTTON( XRCID("wxID_BUTTON_EXPAND"), AmayaDockedToolPanelContainer::OnMinimize) 
  EVT_BUTTON( XRCID("wxID_BUTTON_DETACH"), AmayaDockedToolPanelContainer::OnDetach)
  
  EVT_UPDATE_UI(XRCID("wxID_BUTTON_EXPAND"), AmayaDockedToolPanelContainer::OnUpdateMinimizeUI)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaDockedToolPanelContainer::AmayaDockedToolPanelContainer(
    AmayaToolPanel* panel, AmayaToolPanelBar* bar, 
    wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style):
wxPanel(),
AmayaToolPanelContainer(panel, bar),
m_bMinimized(false),
m_bDetached(false)
{
  if(!s_bmpOk)
    {
      s_Bitmap_DetachOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_MISC, "detach_floating.png" ), wxBITMAP_TYPE_PNG);
      s_Bitmap_DetachOff = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_MISC, "detach.png" ), wxBITMAP_TYPE_PNG);
      s_Bitmap_ExpandOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_MISC, "expand_on.png" ), wxBITMAP_TYPE_PNG);
      s_Bitmap_ExpandOff = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_MISC, "expand_off.png" ), wxBITMAP_TYPE_PNG);
      s_bmpOk = true;
    }
  
  wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, 
                                        wxT("wxID_DOCKED_TOOL_PANEL_CONTAINER"));
  
  XRCCTRL(*this, "wxID_LABEL_TITLE", wxStaticText)->SetLabel(panel->GetToolPanelName());
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaDockedToolPanelContainer::~AmayaDockedToolPanelContainer()
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaDockedToolPanelContainer::Minimize(bool bMinimize)
{
  wxSizer* sz = GetSizer();
  if(sz->GetItem(1))
    {
      m_bMinimized = bMinimize;
      sz->Show((size_t)1, !bMinimize);
      if (bMinimize)
        XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton)->SetBitmapLabel( s_Bitmap_ExpandOff );
      else
        XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton)->SetBitmapLabel( s_Bitmap_ExpandOn );
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
bool AmayaDockedToolPanelContainer::Detach()
{
  wxSizer* sz = GetSizer();
  if(sz)
    {
      m_bDetached = true;
      // XRCCTRL(*this, "wxID_BUTTON_DETACH", wxBitmapButton)->SetBitmapLabel( s_Bitmap_DetachOn );
      if(!sz->Detach(GetPanel()))
        return false;
      GetParent()->Layout();
      return true;
    }  
  return false;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaDockedToolPanelContainer::Attach()
{
  GetPanel()->Reparent(this);
  wxSizer* sz = GetSizer();
  if(sz)
    {
      m_bDetached = false;
      //XRCCTRL(*this, "wxID_BUTTON_DETACH", wxBitmapButton)->SetBitmapLabel( s_Bitmap_DetachOff );
      XRCCTRL(*this, "wxID_BUTTON_DETACH", wxBitmapButton)->Hide();
      GetPanel()->Show();
      sz->Add(GetPanel(), 1, wxEXPAND)->Show(!m_bMinimized);
      sz->Layout();
      GetParent()->Layout();
      return true;
    }
  return false;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaDockedToolPanelContainer::OnMinimize(wxCommandEvent& event)
{
  if(GetBar())
    GetBar()->ToggleMinimize(GetPanel());
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaDockedToolPanelContainer::OnUpdateMinimizeUI(wxUpdateUIEvent& event)
{
  event.Enable(!m_bDetached);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaDockedToolPanelContainer::OnDetach(wxCommandEvent& event)
{
  if(GetBar())
    GetBar()->ToggleFloat(GetPanel());  
}


//
//
// AmayaFloatingToolPanelContainer
//
//
BEGIN_EVENT_TABLE(AmayaFloatingToolPanelContainer, wxDialog)
  EVT_CLOSE( AmayaFloatingToolPanelContainer::OnClose )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaFloatingToolPanelContainer::AmayaFloatingToolPanelContainer(
    AmayaToolPanel* panel, AmayaToolPanelBar* bar, 
    wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style):
wxDialog(parent, id, wxT(""), pos, size, style),
AmayaToolPanelContainer(panel, bar)
{
  wxSizer* sz = new wxBoxSizer(wxVERTICAL);
  SetSizerAndFit(sz);
  SetTitle(panel->GetToolPanelName());
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaFloatingToolPanelContainer::~AmayaFloatingToolPanelContainer()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaFloatingToolPanelContainer::OnClose( wxCloseEvent& event )
{
  GetBar()->DockPanel(GetPanel());
  event.Skip();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaFloatingToolPanelContainer::Detach()
{
  wxSizer* sz = GetSizer();
  if(sz)
    {
      return sz->Detach(GetPanel());
    }
  return false;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaFloatingToolPanelContainer::Attach()
{
  GetPanel()->Reparent(this);
  wxSizer* sz = GetSizer();
  if(sz)
    {
      GetPanel()->Show();
      sz->Add(GetPanel(), 0, wxEXPAND)->Show(true);
      sz->Layout();
      Fit();
      return true;
    }
  return false;
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


/**
 * Function to register a new AmayaToolPanel class.
 */
void RegisterToolPanelClass(wxClassInfo* ci)
{
  g_AmayaToolPanelClassInfoSet.insert(ci);
}


#endif /* #ifdef _WX */
