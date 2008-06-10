#ifdef _WX

#ifndef __AMAYAXMLPANEL_H__
#define __AMAYAXMLPANEL_H__

#include "wx/wx.h"
#include "wx/spinctrl.h"

class AmayaNormalWindow;


/*
 *  Description:  - AmayaXMLPanel is a specific sub-panel
 *       Author:  Irene VATTON
 *      Created:  8 December 14:47:04 CET 2004
 *     Revision:  none
 */
class AmayaXMLPanel : public wxPanel
{
public:
  AmayaXMLPanel();
  AmayaXMLPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0,
      const wxString& name = wxT("AmayaXMLPanel"), wxObject* extra=NULL);
  virtual ~AmayaXMLPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaXMLPanel"), wxObject* extra=NULL);

  virtual void SendDataToPanel( AmayaParams& params );
protected:
 void RefreshXMLPanel();

 DECLARE_EVENT_TABLE()
 void OnRefresh( wxCommandEvent& event );
 void OnApply( wxCommandEvent& event );
 void OnSelected( wxCommandEvent& event );

 wxListBox *    m_pXMLList;
 int m_XMLRef;
 void* m_fnCallback;
};




#endif // __AMAYAXMLPANEL_H__

#endif /* #ifdef _WX */
