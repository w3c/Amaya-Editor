#ifdef _WX

#ifndef __AMAYAXHTMLPANEL_H__
#define __AMAYAXHTMLPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaXHTMLPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
*/

class AmayaXHTMLPanel : public AmayaSubPanel
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaXHTMLPanel)

  AmayaXHTMLPanel( wxWindow * p_parent_window = NULL
		   ,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaXHTMLPanel();

  virtual int GetPanelType();
  //void RefreshCheckButtonState( bool * p_checked_array );
  void RefreshToolTips();

 protected:
  virtual void SendDataToPanel( void * param1 = NULL, void * param2 = NULL, void * param3 = NULL,
				void * param4 = NULL, void * param5 = NULL, void * param6 = NULL );
  virtual void DoUpdate();

 protected:
  DECLARE_EVENT_TABLE()
  void OnButton( wxCommandEvent& event );

  wxColour m_OffColour;
  wxColour m_OnColour;
};

#endif // __AMAYAXHTMLPANEL_H__

#endif /* #ifdef _WX */
