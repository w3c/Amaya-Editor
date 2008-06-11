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

  AmayaBaseToolBar *m_tbar1, *m_tbar2, *m_tbar3, *m_tbar4, *m_tbar5;
};


#endif // __AMAYASVGPANEL_H__

#endif /* #ifdef _WX */
