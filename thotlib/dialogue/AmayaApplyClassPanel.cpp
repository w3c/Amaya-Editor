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
#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"
#include "panel.h"
#include "registry_wx.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"
#include "inites_f.h"
#include "presentmenu_f.h"

#include "AmayaApplyClassPanel.h"
#include "AmayaNormalWindow.h"


//
//
// AmayaApplyClassToolPanel
//
//
IMPLEMENT_DYNAMIC_CLASS(AmayaApplyClassToolPanel, AmayaToolPanel)

AmayaApplyClassToolPanel::AmayaApplyClassToolPanel():
  AmayaToolPanel()
,m_ApplyClassRef(0)
{
}

AmayaApplyClassToolPanel::~AmayaApplyClassToolPanel()
{
}

bool AmayaApplyClassToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_APPLYCLASS")))
    return false;
#ifdef _WINDOWS
  SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */
  XRCCTRL(*this,"wxID_LIST_APPLYCLASS",wxListBox)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SEL_CLASS)));
  XRCCTRL(*this,"wxID_REFRESH",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_REFRESH)));
  XRCCTRL(*this,"wxID_APPLY",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));

  m_pClassList = XRCCTRL(*this,"wxID_LIST_APPLYCLASS",wxListBox);

  return true;
}

wxString AmayaApplyClassToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY_CLASS));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApplyClassToolPanel
 *      Method:  GetDefaultAUIConfig
 * Description:  Return a default AUI config for the panel.
 -----------------------------------------------------------------------*/
wxString AmayaApplyClassToolPanel::GetDefaultAUIConfig()
{
  return wxT("dir=4;layer=0;row=0;pos=2");
}


/*----------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
  -----------------------------------------------------------------------*/
void AmayaApplyClassToolPanel::SendDataToPanel( AmayaParams& p )
{
  int          nb_class = p.param1;
  const char  *listBuffer = (char *)p.param2;
  const char  *currentClass = (char *)p.param3;
  intptr_t     ref = (intptr_t)p.param4;
  
  m_ApplyClassRef = ref;
  
  /* fill the list */
  m_pClassList->Clear();
  int i = 0;
  int index = 0;
  while (i < nb_class && listBuffer[index] != EOS)
    {
      m_pClassList->Append( TtaConvMessageToWX( &listBuffer[index] ) );
      index += strlen (&listBuffer[index]) + 1; /* one entry length */
      i++;
    }

  /* select the wanted item */
  if (currentClass && currentClass[0] != EOS)
    m_pClassList->SetStringSelection(TtaConvMessageToWX(currentClass));

  /* recalculate layout */
  Layout();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
  -----------------------------------------------------------------------*/
void AmayaApplyClassToolPanel::DoUpdate()
{
  AmayaToolPanel::DoUpdate();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  OnApply
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaApplyClassToolPanel::OnApply( wxCommandEvent& event )
{
  ThotCallback(m_ApplyClassRef, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  OnSelected
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaApplyClassToolPanel::OnSelected( wxCommandEvent& event )
{
  wxString s_selected = XRCCTRL(*this, "wxID_LIST_APPLYCLASS", wxListBox)->GetStringSelection();
  
  // allocate a temporary buffer
  char buffer[512];
  wxASSERT( s_selected.Len() < 512 );
  strcpy( buffer, (const char*)s_selected.mb_str(wxConvUTF8) );
  
  // call the callback : send the selected string 
  ThotCallback(m_ApplyClassRef+1, STRING_DATA, buffer);
}



/*----------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  OnRefresh
 * Description:  refresh the panel from current selection
  -----------------------------------------------------------------------*/
void AmayaApplyClassToolPanel::OnRefresh( wxCommandEvent& event )
{
  RefreshApplyClassPanel();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  RefreshApplyClassPanel
 * Description:  refresh the panel from current selection
  -----------------------------------------------------------------------*/
void AmayaApplyClassToolPanel::RefreshApplyClassPanel()
{
  Document doc;
  View view;

  TtaGiveActiveView( &doc, &view );
  /* force the refresh */
  if (doc > 0)
    TtaExecuteMenuAction ("ApplyClass", doc, view, TRUE);
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaApplyClassToolPanel, AmayaToolPanel)
  EVT_BUTTON( XRCID("wxID_APPLY"), AmayaApplyClassToolPanel::OnApply )
  EVT_BUTTON( XRCID("wxID_REFRESH"), AmayaApplyClassToolPanel::OnRefresh )
  EVT_LISTBOX( XRCID("wxID_LIST_APPLYCLASS"), AmayaApplyClassToolPanel::OnSelected ) 
  EVT_LISTBOX_DCLICK( XRCID("wxID_LIST_APPLYCLASS"), AmayaApplyClassToolPanel::OnApply )
END_EVENT_TABLE()


#endif /* #ifdef _WX */
