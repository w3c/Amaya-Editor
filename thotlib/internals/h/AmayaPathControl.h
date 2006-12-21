#ifndef AMAYAPATHCONTROL_H_
#define AMAYAPATHCONTROL_H_
#ifdef _WX

#include <wx/wx.h>
#include <wx/control.h>
#include <vector>

/**
 * Control displaying the path (in the document) of the current selection.
 */
class AmayaPathControl : public wxControl{
private:
  class AmayaPathControlItem
  {
  public:
    wxString label;
    Element  elem;
    wxRect   rect;
    
    void Draw(wxDC& dc, bool isFocused);
  };
  std::vector<AmayaPathControlItem> m_items;
  AmayaPathControlItem* m_focused;
    
public:
  AmayaPathControl(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = wxRAISED_BORDER);
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
