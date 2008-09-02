#ifdef _WX

#ifndef __AMAYATEMPLATEPANEL_H__
#define __AMAYATEMPLATEPANEL_H__

#include "wx/wx.h"
#include "AmayaToolBar.h"

class AmayaNormalWindow;

/*
 * 
 */
class AmayaTemplatePanel : public wxPanel
{
public:
  AmayaTemplatePanel();
  AmayaTemplatePanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0,
      const wxString& name = wxT("AmayaSVGPanel"), wxObject* extra=NULL);
  virtual ~AmayaTemplatePanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaSVGPanel"), wxObject* extra=NULL);
protected:

  AmayaBaseToolBar *m_tbar1, *m_tbar2;
};


#endif // __AMAYATEMPLATEPANEL_H__

#endif /* #ifdef _WX */
