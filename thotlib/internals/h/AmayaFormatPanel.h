#ifdef _WX

#ifndef __AMAYAFORMATPANEL_H__
#define __AMAYAFORMATPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaFormatPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
 */
class AmayaFormatPanel : public AmayaSubPanel
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaFormatPanel)

  AmayaFormatPanel( wxWindow * p_parent_window = NULL
		   ,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaFormatPanel();

  virtual bool IsActive();
  virtual int GetPanelType();
  void RefreshToolTips();

 protected:
  virtual void SendDataToPanel( AmayaPanelParams& params );
  virtual void DoUpdate();

 protected:
  DECLARE_EVENT_TABLE()
};

#endif // __AMAYAFORMATPANEL_H__

#endif /* #ifdef _WX */
