#ifdef _WX

#ifndef __AMAYACHARSTYLEPANEL_H__
#define __AMAYACHARSTYLEPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaCharStylePanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
 */
class AmayaCharStylePanel : public AmayaSubPanel
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaCharStylePanel)

  AmayaCharStylePanel( wxWindow * p_parent_window = NULL
		   ,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaCharStylePanel();

  virtual bool IsActive();
  virtual int GetPanelType();
  void RefreshToolTips();

 protected:
  virtual void SendDataToPanel( AmayaPanelParams& params );
  virtual void DoUpdate();
  void RefreshCharStylePanel();

 protected:
  DECLARE_EVENT_TABLE()
  void OnRefresh( wxCommandEvent& event );
  void OnApply( wxCommandEvent& event );
};

#endif // __AMAYACHARSTYLEPANEL_H__

#endif /* #ifdef _WX */
