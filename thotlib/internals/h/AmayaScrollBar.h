#ifdef _WX

#ifndef __AMAYASCROLLBAR_H__
#define __AMAYASCROLLBAR_H__

#include "wx/wx.h"
#include "wx/scrolbar.h"

class AmayaScrollBar : public wxScrollBar
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaScrollBar)

  AmayaScrollBar( wxWindow* parent = NULL
		  ,int parent_frame_id = 0
		  ,long style = wxSB_HORIZONTAL );
  virtual ~AmayaScrollBar( );

protected:
  DECLARE_EVENT_TABLE()
  void OnSetFocus( wxFocusEvent & event );
  void OnKillFocus( wxFocusEvent & event );
  void OnThumbRelease( wxScrollEvent& event );
  void OnThumbTrack( wxScrollEvent& event );
  void OnLineDown( wxScrollEvent& event );
  void OnLineUp( wxScrollEvent& event );
  void OnBottom( wxScrollEvent& event );
  void OnTop( wxScrollEvent& event );

 protected:
  int m_ParentFrameID;
};

#endif // __AMAYASCROLLBAR_H__

#endif /* #ifdef _WX */

