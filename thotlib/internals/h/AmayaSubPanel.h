#ifdef _WX

#ifndef __AMAYASUBPANEL_H__
#define __AMAYASUBPANEL_H__

#include "wx/wx.h"
#include "wx/panel.h"

class AmayaFloatingPanel;
class AmayaNormalWindow;

/*
 *  Description:  - AmayaSubPanel contains a sub-panel (xhtml, ...)
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
*/

class AmayaSubPanel : public wxPanel
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaSubPanel)

  AmayaSubPanel( wxWindow *     p_parent_window = NULL
		 ,AmayaNormalWindow * p_parent_nwindow = NULL
		 ,const wxString& panel_xrcid   = _T("")
		 ,wxWindowID     id             = -1
		 ,const wxPoint& pos            = wxDefaultPosition
		 ,const wxSize&  size           = wxDefaultSize
		 ,long style                    = wxTAB_TRAVERSAL
	      );
  virtual ~AmayaSubPanel();

  void UnExpand();
  void Expand();
  void DoStick();
  void DoUnstick();

  void SetTopAmayaWindow( AmayaNormalWindow * p_parent_window );

  virtual void RefreshCheckButtonState( bool * p_checked_array );
  virtual void RefreshToolTips();

 protected:
  DECLARE_EVENT_TABLE()
  void OnExpand( wxCommandEvent& event );
  void OnDetach( wxCommandEvent& event );

  void DebugPanelSize( const wxString & prefix = _T("") );

  wxBoxSizer * m_pTopSizer;

  wxPanel *            m_pPanel;
  wxPanel *            m_pPanelContent;
  wxPanel *            m_pPanelTitle;
  AmayaFloatingPanel * m_pFloatingPanel;

  AmayaNormalWindow * m_pParentNWindow;

  wxSize    m_ContentSize;
  wxSize    m_TitleSize;
  bool      m_IsExpanded;
  bool      m_IsFloating;
  bool      m_IsExpBeforeDetach;

  bool m_DoUnstick_Lock;


  wxBitmap m_Bitmap_DetachOn;
  wxBitmap m_Bitmap_DetachOff;
  wxBitmap m_Bitmap_ExpandOn;
  wxBitmap m_Bitmap_ExpandOff;
};

#endif // __AMAYASUBPANEL_H__

#endif /* #ifdef _WX */
