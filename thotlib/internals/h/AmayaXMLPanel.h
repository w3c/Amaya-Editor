#ifdef _WX

#ifndef __AMAYAXMLPANEL_H__
#define __AMAYAXMLPANEL_H__

#include "wx/wx.h"
#include "wx/spinctrl.h"

#include "AmayaSubPanel.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaXMLPanel is a specific sub-panel
 *       Author:  Irene VATTON
 *      Created:  8 December 14:47:04 CET 2004
 *     Revision:  none
 */
class AmayaXMLPanel : public AmayaSubPanel
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaXMLPanel)

  AmayaXMLPanel( wxWindow * p_parent_window = NULL
			,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaXMLPanel();

  virtual bool IsActive();
  virtual int GetPanelType();
  virtual void RefreshToolTips();

 protected:
  virtual void SendDataToPanel( AmayaParams& params );
  virtual void DoUpdate();
  void RefreshXMLPanel();

 protected:
  DECLARE_EVENT_TABLE()
  void OnRefresh( wxCommandEvent& event );
  void OnApply( wxCommandEvent& event );
  void OnSelected( wxCommandEvent& event );

 protected:
  wxListBox *    m_pXMLList;
  int m_XMLRef;
};

#endif // __AMAYAXMLPANEL_H__

#endif /* #ifdef _WX */
