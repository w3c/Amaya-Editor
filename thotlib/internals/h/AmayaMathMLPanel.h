#ifdef _WX

#ifndef __AMAYAMATHMLPANEL_H__
#define __AMAYAMATHMLPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaMathMLPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
 */
class AmayaMathMLPanel : public AmayaSubPanel
{
 public:
  typedef enum
    {
      wxMATHML_ACTION_UNKNOWN,
      wxMATHML_ACTION_INIT,
      wxMATHML_ACTION_REFRESH,
    } wxMATHML_ACTION;
  
 public:
  DECLARE_DYNAMIC_CLASS(AmayaMathMLPanel)

  AmayaMathMLPanel( wxWindow * p_parent_window = NULL
		   ,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaMathMLPanel();

  virtual bool IsActive();
  virtual int GetPanelType();
  void RefreshToolTips();

 protected:
  virtual void SendDataToPanel( AmayaParams& params );
  virtual void DoUpdate();

 protected:
  DECLARE_EVENT_TABLE()
  void OnButton( wxCommandEvent& event );
};

#endif // __AMAYAMATHMLPANEL_H__

#endif /* #ifdef _WX */
