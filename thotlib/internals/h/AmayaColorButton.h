#ifdef _WX

#ifndef AMAYACOLORBUTTON_H_
#define AMAYACOLORBUTTON_H_

#include "wx/wx.h"
#include "AmayaPanel.h"
#include "AmayaToolBar.h"

class AmayaNormalWindow;



DECLARE_EVENT_TYPE(AMAYA_COLOR_CHANGED, wxID_ANY)

class AmayaColorButtonEvent : public wxCommandEvent
{
public:
  AmayaColorButtonEvent(const wxColour& col, wxEventType commandType = AMAYA_COLOR_CHANGED, int winid = 0);
  AmayaColorButtonEvent(const AmayaColorButtonEvent& event);
  virtual ~AmayaColorButtonEvent();
  
  wxColour GetColour()const{return m_col;}
  void SetColour(const wxColour& col){m_col=col;}
private:
  wxColour m_col;
};

typedef void (wxEvtHandler::*AmayaColorButtonEventFunction)(AmayaColorButtonEvent&);

#define AmayaColorButtonEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(AmayaColorButtonEventFunction, &func)

#define EVT_AMAYA_COLOR_CHANGED(id, fn) wx__DECLARE_EVT1(AMAYA_COLOR_CHANGED, id, AmayaColorButtonEventHandler(fn))


#define AMAYA_COLOR_BUTTON_QUERY_ON_CLICK   0x1L

/*
 *  Description:  - AmayaColorButton is a special button for choosing a color.
 *       Author:  Emilien Kia
*/
class AmayaColorButton : public wxControl
{
  DECLARE_DYNAMIC_CLASS(AmayaColorButton)
public:
  AmayaColorButton();
  virtual ~AmayaColorButton();
  
  AmayaColorButton(wxWindow* parent, wxWindowID id, const wxColour& col, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0,
      const wxString& name = wxT("AmayaColorButton"));
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxColour& col, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaColorButton"));
  
  wxColour GetColour()const;
  void SetColour(const wxColour& col);

  wxColour ChooseColour();
  
protected:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  
  static wxColourData s_colData;
  
  void OnLeftButtonDown(wxMouseEvent& event);
};

#endif /*AMAYACOLORBUTTON_H_*/

#endif /* #ifdef _WX */
