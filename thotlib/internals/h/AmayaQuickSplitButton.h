#ifdef _WX

#ifndef __AMAYAQUICKSPLITBUTTON_H__
#define __AMAYAQUICKSPLITBUTTON_H__

#include "wx/wx.h"

class AmayaQuickSplitButton : public wxButton
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaQuickSplitButton)

 public:
  typedef enum
    {
      wxAMAYA_QS_HORIZONTAL,
      wxAMAYA_QS_VERTICAL
    } wxAmayaQuickSplitMode;

  AmayaQuickSplitButton( wxWindow * p_parent_window = NULL
			 ,wxAmayaQuickSplitMode mode = wxAMAYA_QS_HORIZONTAL
			 ,int width = 5 );
  virtual ~AmayaQuickSplitButton();

  void ShowQuickSplitButton( bool show );

 protected:
  DECLARE_EVENT_TABLE()  
  void OnEnterWindow( wxMouseEvent& event );
  void OnLeaveWindow( wxMouseEvent& event );

  wxColour              m_OldColour;
  int                   m_Width;
  wxAmayaQuickSplitMode m_Mode;
};

#endif // __AMAYAQUICKSPLITBUTTON_H__

#endif /* #ifdef _WX */
