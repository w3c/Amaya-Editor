#ifdef _WX

#ifndef __AMAYAFORMATPANEL_H__
#define __AMAYAFORMATPANEL_H__

#include "wx/wx.h"
#include "wx/spinctrl.h"

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
  typedef enum
    {
      wxFORMAT_MODIF_NONE = 0,
      wxFORMAT_MODIF_FORMAT = 1,
      wxFORMAT_MODIF_INDENT = 2,
      wxFORMAT_MODIF_LINESPACE = 4,
      wxFORMAT_MODIF_ALL = wxFORMAT_MODIF_FORMAT | wxFORMAT_MODIF_INDENT | wxFORMAT_MODIF_LINESPACE,
    } wxFORMAT_MODIF_TYPE;

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
  void RefreshFormatPanel();
  void LineSpaceChanged();
  void IndentChanged();

 protected:
  DECLARE_EVENT_TABLE()
  void OnRefresh( wxCommandEvent& event );
  void OnApply( wxCommandEvent& event );
  void OnLineSpaceChanged( wxSpinEvent& event );
  void OnLineSpaceChangedButton( wxCommandEvent& event );
  void OnIndentChanged( wxSpinEvent& event );
  void OnIndentChangedButton( wxCommandEvent& event );
  void OnFormatLeftChanged( wxCommandEvent& event );
  void OnFormatRightChanged( wxCommandEvent& event );
  void OnFormatCenterChanged( wxCommandEvent& event );
  void OnFormatJustifyChanged( wxCommandEvent& event );
  void OnDefaultFormat( wxCommandEvent& event );
  void OnDefaultIndent( wxCommandEvent& event );
  void OnDefaultLineSpace( wxCommandEvent& event );

 protected:
  wxColour m_OffColour;
  wxColour m_OnColour;

};

#endif // __AMAYAFORMATPANEL_H__

#endif /* #ifdef _WX */
