#ifdef _WX

#ifndef __AMAYAFLOATINGPANEL_H__
#define __AMAYAFLOATINGPANEL_H__

#include "wx/wx.h"
#include "wx/frame.h"

class AmayaSubPanel;

/*
 *  Description:  - AmayaFloatingPanel contains a floating sub-panel (xhtml, ...)
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
*/

class AmayaFloatingPanel : public wxFrame
{
public:
  AmayaFloatingPanel( AmayaSubPanel * p_subpanel    = NULL
		      ,wxWindowID     id             = -1
		      ,const wxPoint& pos            = wxDefaultPosition
		      ,const wxSize&  size           = wxDefaultSize
		      ,long style                    = wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxSTAY_ON_TOP | wxFRAME_NO_TASKBAR | wxRESIZE_BORDER | wxRESIZE_BOX
		      );
  virtual ~AmayaFloatingPanel();

 protected:
  DECLARE_EVENT_TABLE()
  void OnClose( wxCloseEvent& event );

  wxBoxSizer * m_pTopSizer;

  AmayaSubPanel * m_pPanel;
  wxPanel *       m_pPanelContent;
  wxWindow *      m_pPanelContentParent;

  wxPanel * m_pPanelTitle;

};

#endif // __AMAYAFLOATINGPANEL_H__

#endif /* #ifdef _WX */
