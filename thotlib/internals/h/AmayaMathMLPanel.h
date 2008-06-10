#ifdef _WX

#ifndef __AMAYAMATHMLPANEL_H__
#define __AMAYAMATHMLPANEL_H__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/choicebk.h"
#include "wx/toolbook.h"
#include "wx/imaglist.h"
#include "AmayaToolBar.h"

class AmayaNormalWindow;


class AmayaMathMLToolBar : public AmayaBaseToolBar
{
  DECLARE_DYNAMIC_CLASS(AmayaMathMLToolBar)
  DECLARE_EVENT_TABLE()
public:
  AmayaMathMLToolBar();
  
private:
  void OnUpdate(wxUpdateUIEvent& event);

  static AmayaToolBarToolDefHashMap s_mymap;
  static bool s_isinit;
};


/*
 * 
 * 
 */
class AmayaMathMLPanel : public wxPanel
{
public:
  AmayaMathMLPanel();
  AmayaMathMLPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0,
      const wxString& name = wxT("AmayaMathMLToolPanel"), wxObject* extra=NULL);
  virtual ~AmayaMathMLPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaMathMLToolPanel"), wxObject* extra=NULL);

protected:
  void Initialize();

  wxChoicebook *m_pBook;
  wxImageList m_imagelist;
};


#endif // __AMAYAMATHMLPANEL_H__

#endif /* #ifdef _WX */
