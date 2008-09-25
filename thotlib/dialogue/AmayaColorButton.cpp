/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/colordlg.h"

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


#include "AmayaColorButton.h"

//
//
// AmayaColorButton
//
//

wxColourData AmayaColorButton::s_colData;


IMPLEMENT_DYNAMIC_CLASS(AmayaColorButton, wxControl)

BEGIN_EVENT_TABLE(AmayaColorButton, wxControl)
  EVT_LEFT_DOWN(AmayaColorButton::OnLeftButtonDown)
END_EVENT_TABLE()


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaColorButton::AmayaColorButton():
wxControl()
{
  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaColorButton::~AmayaColorButton()
{
  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaColorButton::AmayaColorButton(wxWindow* parent, wxWindowID id, const wxColour& col,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name):
wxControl(parent, id, pos, size, style, wxDefaultValidator, name)
{
  SetMinSize(wxSize(16,16));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaColorButton::Create(wxWindow* parent, wxWindowID id, const wxColour& col,
     const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  SetMinSize(wxSize(16,16));
  return wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxColour AmayaColorButton::GetColour()const
{
  return GetBackgroundColour();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaColorButton::SetColour(const wxColour& col)
{
  SetBackgroundColour(col);
  Refresh();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxColour AmayaColorButton::ChooseColour()
{
  wxColourDialog dialog (this, &s_colData);
  if (dialog.ShowModal() == wxID_OK)
    {
      s_colData = dialog.GetColourData();
      SetColour(s_colData.GetColour());
      return GetColour();
    }
  else
    return wxNullColour;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaColorButton::OnLeftButtonDown(wxMouseEvent& event)
{
  wxColour col = GetColour();
  
  if(HasFlag(AMAYA_COLOR_BUTTON_QUERY_ON_CLICK))
      col = ChooseColour();
  
  AmayaColorButtonEvent evt(col, AMAYA_COLOR_CHANGED, GetId());
  ProcessEvent(evt);
}


//
//
// AmayaColorButtonEvent
//
//

DEFINE_EVENT_TYPE(AMAYA_COLOR_CHANGED)

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaColorButtonEvent::AmayaColorButtonEvent(const wxColour& col, wxEventType commandType, int winid):
  wxCommandEvent(commandType, winid),
  m_col(col)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaColorButtonEvent::AmayaColorButtonEvent(const AmayaColorButtonEvent& event):
  wxCommandEvent(event),
  m_col(event.m_col)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaColorButtonEvent::~AmayaColorButtonEvent()
{  
}


#endif /* #ifdef _WX */
