#ifndef AMAYAPATHCONTROL_H_
#define AMAYAPATHCONTROL_H_
#ifdef _WX

#include <wx/wx.h>
#include <wx/control.h>
#include <wx/list.h>

class AmayaPathControlItem
{
public:
  wxString label;
  Element  elem;
  wxRect   rect;
  bool     isXTiger;
  void Draw(wxDC& dc, bool isFocused);
};

WX_DECLARE_LIST(AmayaPathControlItem, AmayaPathControlItemList);


/**
 * Control displaying the path (in the document) of the current selection.
 */
class AmayaPathControl : public wxControl{
private:
  AmayaPathControlItemList m_items;
  AmayaPathControlItem* m_focused;
  int m_height;
public:
  AmayaPathControl(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0);
  virtual ~AmayaPathControl();
  
  /**
   * Change the selection.
   */
  void SetSelection(Element elem);
  
protected:
  DECLARE_EVENT_TABLE()
  void OnDraw(wxPaintEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnMouseMove(wxMouseEvent& event);
  void OnMouseEnter(wxMouseEvent& event);
  void OnMouseLeave(wxMouseEvent& event);
  void OnMouseLeftUp(wxMouseEvent& event);
  void PreCalcPositions();
};


#endif /* #ifdef _WX */
#endif /*AMAYAPATHCONTROL_H_*/
