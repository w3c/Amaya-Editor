#ifdef _WX

#ifndef __AMAYASUBPANEL_H__
#define __AMAYASUBPANEL_H__

#include "wx/wx.h"
#include "wx/panel.h"
#include "paneltypes_wx.h"

class AmayaFloatingPanel;
class AmayaNormalWindow;
class AmayaSubPanelManager;

/*
 *  Description:  - AmayaSubPanel contains a sub-panel (xhtml, ...)
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
 */

class AmayaSubPanel : public wxPanel
{
 public:
  friend class AmayaSubPanelManager;

 public:
  typedef enum
    {
      wxAMAYA_SPANEL_EXPANDED  = 1
      ,wxAMAYA_SPANEL_FLOATING  = 2
    } wxAMAYA_SPANEL_STATE;

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

  virtual int GetPanelType();
  bool        IsExpanded();
  bool        IsFloating();
  bool        IsVisible();

  void ShouldBeUpdated( bool should_update = true );
  
  void SetTopAmayaWindow( AmayaNormalWindow * p_parent_window );
  
  //virtual void RefreshCheckButtonState( bool * p_checked_array );
  virtual void RefreshToolTips();

  void Raise();

 protected:
  virtual void SendDataToPanel( void * param1 = NULL, void * param2 = NULL, void * param3 = NULL,
				void * param4 = NULL, void * param5 = NULL, void * param6 = NULL );
  virtual void DoUpdate();

  void UnExpand();
  void Expand();
  void DoFloat();
  void DoUnfloat();

  unsigned int GetState();
  void ChangeState( unsigned int new_state );
  
  wxPanel * GetPanelContentDetach();

 protected:
  DECLARE_EVENT_TABLE()
  void OnExpand( wxCommandEvent& event );
  void OnDetach( wxCommandEvent& event );

 protected:
  AmayaNormalWindow * m_pParentNWindow;
  wxBoxSizer *        m_pTopSizer;
  AmayaSubPanelManager * m_pManager;

  wxString             m_PanelType;
  wxPanel *            m_pPanel;
  wxPanel *            m_pPanelContent;
  wxPanel *            m_pPanelContentDetach;
  wxPanel *            m_pPanelTitle;
  AmayaFloatingPanel * m_pFloatingPanel;

  unsigned int m_State; /* bit field of wxAMAYA_SPANEL_STATE */  
  bool         m_IsExpBeforeDetach; /* wxAMAYA_SPANEL_FLOATING == true && wxAMAYA_SPANEL_EXPANDED == true */

  bool m_DoUnfloat_Lock;
  bool m_ShouldBeUpdated;

  wxBitmap m_Bitmap_DetachOn;
  wxBitmap m_Bitmap_DetachOff;
  wxBitmap m_Bitmap_ExpandOn;
  wxBitmap m_Bitmap_ExpandOff;
};

#endif // __AMAYASUBPANEL_H__

#endif /* #ifdef _WX */
