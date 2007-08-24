#ifdef _WX

#ifndef __AMAYAQUICKSPLITBUTTON_H__
#define __AMAYAQUICKSPLITBUTTON_H__

#include "wx/wx.h"
#include "wx/panel.h"

class AmayaQuickSplitButton : public wxPanel
{
 public:
  DECLARE_CLASS(AmayaQuickSplitButton)

 public:
  typedef enum
    {
      wxAMAYA_QS_HORIZONTAL,
      wxAMAYA_QS_VERTICAL,
      wxAMAYA_QS_TOOLS
    } wxAmayaQuickSplitMode;

  AmayaQuickSplitButton( wxWindow * p_parent_window,
      wxWindowID id = wxID_ANY,
			wxAmayaQuickSplitMode mode = wxAMAYA_QS_HORIZONTAL,
			int width = 4);
  virtual ~AmayaQuickSplitButton();

 protected:
  DECLARE_EVENT_TABLE()  
  void OnEnterWindow( wxMouseEvent& event );
  void OnLeaveWindow( wxMouseEvent& event );
  void OnActivate( wxMouseEvent& event );

  wxColour              m_OldColour;
  int                   m_Width;
  wxAmayaQuickSplitMode m_Mode;
};

#endif // __AMAYAQUICKSPLITBUTTON_H__

#endif /* #ifdef _WX */
