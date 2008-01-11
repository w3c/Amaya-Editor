#ifdef _WX

#ifndef __AMAYAXMLPANEL_H__
#define __AMAYAXMLPANEL_H__

#include "wx/wx.h"
#include "wx/spinctrl.h"

#include "AmayaToolPanel.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaXMLPanel is a specific sub-panel
 *       Author:  Irene VATTON
 *      Created:  8 December 14:47:04 CET 2004
 *     Revision:  none
 */
class AmayaXMLToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaXMLToolPanel)
public:
  AmayaXMLToolPanel();
  virtual ~AmayaXMLToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaXMLToolPanel"), wxObject* extra=NULL);
  
  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_XML;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("PANEL_XML");}

  /** Return a default AUI config for the panel.*/
  virtual wxString GetDefaultAUIConfig();

protected:
 virtual void SendDataToPanel( AmayaParams& params );
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
