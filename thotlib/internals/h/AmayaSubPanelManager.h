#ifdef _WX

#ifndef __AMAYASUBPANELMANAGER_H__
#define __AMAYASUBPANELMANAGER_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"

// declare our list class: this macro declares and partly implements MyList
// class (which derives from wxListBase)
WX_DECLARE_LIST(AmayaSubPanel, SubPanelList);

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
  void SendDataToPanel( AmayaSubPanel * p_panel,
			void * param1 = NULL, void * param2 = NULL, void * param3 = NULL,
			void * param4 = NULL, void * param5 = NULL, void * param6 = NULL );
  void UnExpand( AmayaSubPanel * p_panel );
  void Expand( AmayaSubPanel * p_panel );
  void DoFloat( AmayaSubPanel * p_panel );
  void DoUnfloat( AmayaSubPanel * p_panel );

 protected:
  bool CanChangeState( AmayaSubPanel * p_panel, unsigned int new_state );
  void DebugSubPanelList();

 protected:
  static AmayaSubPanelManager * m_pInstance;

  SubPanelList m_RegistredPanel;
};

#endif // __AMAYASUBPANELMANAGER_H__

#endif /* #ifdef _WX */
