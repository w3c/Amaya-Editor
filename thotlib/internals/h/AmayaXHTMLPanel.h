#ifdef _WX

#ifndef __AMAYAXHTMLPANEL_H__
#define __AMAYAXHTMLPANEL_H__

#include "wx/wx.h"
#include "AmayaToolBar.h"

class AmayaNormalWindow;

/*
 * 
 */
class AmayaXHTMLPanel : public wxPanel
{
public:
  AmayaXHTMLPanel();
  AmayaXHTMLPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0,
      const wxString& name = wxT("AmayaXHTMLPanel"), wxObject* extra=NULL);
  virtual ~AmayaXHTMLPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaXHTMLPanel"), wxObject* extra=NULL);
protected:

  AmayaBaseToolBar *m_tbar1, *m_tbar2, *m_tbar3, *m_tbar4, *m_tbar5;
};


#endif // __AMAYAXHTMLPANEL_H__

#endif /* #ifdef _WX */
