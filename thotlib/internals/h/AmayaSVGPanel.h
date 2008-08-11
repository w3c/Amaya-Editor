#ifdef _WX

#ifndef __AMAYASVGPANEL_H__
#define __AMAYASVGPANEL_H__

#include "wx/wx.h"
#include "AmayaToolBar.h"

class AmayaNormalWindow;

/*
 * 
 */
class AmayaSVGPanel : public wxPanel
{
  DECLARE_EVENT_TABLE()
public:
  AmayaSVGPanel();
  AmayaSVGPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0,
      const wxString& name = wxT("AmayaSVGPanel"), wxObject* extra=NULL);
  virtual ~AmayaSVGPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaSVGPanel"), wxObject* extra=NULL);
protected:
  int last_menu;

  void DisplayMenu(int i);
  void InsertElement(int i, int j);

  void OnMenu0(wxCommandEvent& event);
  void OnMenu1(wxCommandEvent& event);
  void OnMenu2(wxCommandEvent& event);
  void OnMenu3(wxCommandEvent& event);
  void OnMenu4(wxCommandEvent& event);
  void OnMenu5(wxCommandEvent& event);
  void OnMenu6(wxCommandEvent& event);
  void OnMenu7(wxCommandEvent& event);
  void OnMenu8(wxCommandEvent& event);
  void OnMenu9(wxCommandEvent& event);
  void OnMenu10(wxCommandEvent& event);
  void OnMenu11(wxCommandEvent& event);
  void OnMenu12(wxCommandEvent& event);
  void OnMenu13(wxCommandEvent& event);
  void OnMenu14(wxCommandEvent& event);
  void OnMenu15(wxCommandEvent& event);
  void OnInsertElement(wxCommandEvent& event);
    
  AmayaBaseToolBar *m_tbar1, *m_tbar2, *m_tbar3, *m_tbar4;
};


#endif // __AMAYASVGPANEL_H__

#endif /* #ifdef _WX */
