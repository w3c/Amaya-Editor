#ifdef _WX

#ifndef __AMAYACOLORSPANEL_H__
#define __AMAYACOLORSPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaColorsPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
 */
class AmayaColorsPanel : public AmayaSubPanel
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaColorsPanel)

  AmayaColorsPanel( wxWindow * p_parent_window = NULL
		   ,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaColorsPanel();

  virtual bool IsActive();
  virtual int GetPanelType();
  void RefreshToolTips();

 protected:
  virtual void SendDataToPanel( AmayaPanelParams& params );
  virtual void DoUpdate();

  int ChooseCustomColor( const wxColour & start_colour );

 protected:
  DECLARE_EVENT_TABLE()
  void OnModifyColor( wxCommandEvent& event );
  void OnGetColor( wxCommandEvent& event );
  void OnDefaultColors( wxCommandEvent& event );
  void OnSwitchColors( wxCommandEvent& event );
  void OnChooseBGColor( wxCommandEvent& event );
  void OnChooseFGColor( wxCommandEvent& event );

  int m_ThotBGColor;
  int m_ThotFGColor;

  wxBitmap m_Bitmap_Empty;
  wxBitmap m_Bitmap_DefaultColor;
  wxColour m_Color_ButtonBG;

  // custom color palette
  wxColourData m_ColourData;
};

#endif // __AMAYACOLORSPANEL_H__

#endif /* #ifdef _WX */
