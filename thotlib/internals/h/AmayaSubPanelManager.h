#ifdef _WX

#ifndef __AMAYASUBPANELMANAGER_H__
#define __AMAYASUBPANELMANAGER_H__

// this class is used to pass multipl parameters to SendDataToPanel
class AmayaPanelParams
{
 public:
  void * param1;
  void * param2;
  void * param3;
  void * param4;
  void * param5;
  void * param6;
  void * param7;
  void * param8;
  void * param9;
};

// declare our list class: this macro declares and partly implements MyList
// class (which derives from wxListBase)
#include "wx/wx.h"
#include "AmayaSubPanel.h"
WX_DECLARE_LIST(AmayaSubPanel, SubPanelList);

#include "paneltypes_wx.h"

/*
 *  Description:  - AmayaSubPanelManager manage the global behaviour between every sub panels
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
 */
class AmayaSubPanelManager
{
 public:
  AmayaSubPanelManager();
  virtual ~AmayaSubPanelManager();

  static AmayaSubPanelManager * GetInstance();

  bool RegisterSubPanel( AmayaSubPanel * p_panel );
  bool UnregisterSubPanel( AmayaSubPanel * p_panel );
  void SendDataToPanel( int panel_type, AmayaPanelParams& params );
  void UnExpand( AmayaSubPanel * p_panel );
  void Expand( AmayaSubPanel * p_panel );
  void DoFloat( AmayaSubPanel * p_panel );
  void DoUnfloat( AmayaSubPanel * p_panel );

  bool IsActive( int panel_type );
  void ShouldBeUpdated( int panel_type, bool should_update = true );
  void AmayaSubPanelManager::CheckForDoUpdate( int panel_type = WXAMAYA_PANEL_UNKNOWN );

 protected:
  bool CanChangeState( AmayaSubPanel * p_panel, unsigned int new_state );
  void DebugSubPanelList();

 protected:
  static AmayaSubPanelManager * m_pInstance;

  SubPanelList m_RegistredPanel;
};

#endif // __AMAYASUBPANELMANAGER_H__

#endif /* #ifdef _WX */
