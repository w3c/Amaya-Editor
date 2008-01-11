#ifdef _WX

#ifndef __AMAYAAPPLYCLASSPANEL_H__
#define __AMAYAAPPLYCLASSPANEL_H__

#include "wx/wx.h"
#include "wx/spinctrl.h"

#include "AmayaToolPanel.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaApplyClassPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
 */

class AmayaApplyClassToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaApplyClassToolPanel)
public:
  AmayaApplyClassToolPanel();
  virtual ~AmayaApplyClassToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaApplyClassToolPanel"), wxObject* extra=NULL);
  
  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_APPLYCLASS;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("PANEL_APPLYCLASS");}
  virtual bool GetDefaultVisibilityState()const{return true;}

  /** Return a default AUI config for the panel.*/
  virtual wxString GetDefaultAUIConfig();


protected:
 virtual void SendDataToPanel( AmayaParams& params );
 virtual void DoUpdate();
 void RefreshApplyClassPanel();
  
protected:
 DECLARE_EVENT_TABLE()
 void OnRefresh( wxCommandEvent& event );
 void OnApply( wxCommandEvent& event );
 void OnSelected( wxCommandEvent& event );

protected:
 wxListBox *    m_pClassList;
 int m_ApplyClassRef;
};
  
  

#endif // __AMAYAAPPLYCLASSPANEL_H__

#endif /* #ifdef _WX */
