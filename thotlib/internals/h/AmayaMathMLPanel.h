#ifdef _WX

#ifndef __AMAYAMATHMLPANEL_H__
#define __AMAYAMATHMLPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"

class AmayaNormalWindow;

WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, MathMLEntityHash );

/*
 *  Description:  - AmayaMathMLPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
*/

class AmayaMathMLPanel : public AmayaSubPanel
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaMathMLPanel)

  AmayaMathMLPanel( wxWindow * p_parent_window = NULL
		   ,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaMathMLPanel();

  virtual bool IsActive();
  virtual int GetPanelType();
  //void RefreshCheckButtonState( bool * p_checked_array );
  void RefreshToolTips();

 protected:
  virtual void SendDataToPanel( AmayaParams& params );
  virtual void DoUpdate();

  void DoFilter( int * filtre );

 protected:
  DECLARE_EVENT_TABLE()
  void OnButton( wxCommandEvent& event );
  void OnButtonFiltre1( wxCommandEvent& event );
  void OnButtonFiltre2( wxCommandEvent& event );
  void OnButtonFiltre3( wxCommandEvent& event );
  void OnButtonFiltre4( wxCommandEvent& event );
  void OnButtonFiltre5( wxCommandEvent& event );
  void OnButtonFiltre6( wxCommandEvent& event );
  void OnButtonFiltre7( wxCommandEvent& event );

  wxComboBox * m_pList;

  static MathMLEntityHash m_MathMLEntityHash;
};

#endif // __AMAYAMATHMLPANEL_H__

#endif /* #ifdef _WX */
