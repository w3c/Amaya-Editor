#ifdef _WX

#ifndef __AMAYAAPPLYCLASSPANEL_H__
#define __AMAYAAPPLYCLASSPANEL_H__

#include "wx/wx.h"
#include "wx/spinctrl.h"

#include "AmayaSubPanel.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaApplyClassPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
 */
class AmayaApplyClassPanel : public AmayaSubPanel
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaApplyClassPanel)

  AmayaApplyClassPanel( wxWindow * p_parent_window = NULL
			,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaApplyClassPanel();

  virtual bool IsActive();
  virtual int GetPanelType();
  virtual void RefreshToolTips();

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
