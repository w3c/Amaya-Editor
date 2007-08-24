#ifdef _WX

#ifndef __AMAYASPECHARPANEL_H__
#define __AMAYASPECHARPANEL_H__

#include "wx/wx.h"
#include "AmayaPanel.h"

class AmayaNormalWindow;

WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, MathMLEntityHash );

/*
 *  Description:  - AmayaSpeCharPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
*/
class AmayaSpeCharToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaSpeCharToolPanel)
public:
  typedef enum
    {
      wxSPECHAR_ACTION_UNKNOWN,
      wxSPECHAR_ACTION_INIT,
      wxSPECHAR_ACTION_REFRESH,
    } wxSPECHAR_ACTION;

  AmayaSpeCharToolPanel();
  virtual ~AmayaSpeCharToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaSpeCharToolPanel"), wxObject* extra=NULL);
  
  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_SPECHAR;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("OPEN_PANEL_SPECHAR");}

protected:
 virtual void SendDataToPanel( AmayaParams& params );
 virtual void DoUpdate();

 void DoFilter( int * filtre );
 void RefreshButtonState();

 DECLARE_EVENT_TABLE()
 void OnButtonFiltre1( wxCommandEvent& event );
 void OnButtonFiltre2( wxCommandEvent& event );
 void OnButtonFiltre3( wxCommandEvent& event );
 void OnButtonFiltre4( wxCommandEvent& event );
 void OnButtonFiltre5( wxCommandEvent& event );
 void OnButtonFiltre6( wxCommandEvent& event );
 void OnButtonFiltre7( wxCommandEvent& event );
 void OnButtonFiltre8( wxCommandEvent& event );
 void OnButtonInsert( wxCommandEvent& event );

 wxComboBox * m_pList;
 wxColour m_OffColour;
 wxColour m_OnColour;

 static MathMLEntityHash m_MathMLEntityHash;
 static int * m_pActiveFiltre;
};

#endif // __AMAYASPECHARPANEL_H__

#endif /* #ifdef _WX */
