#ifdef _WX

#ifndef __AMAYASCROLLBAR_H__
#define __AMAYASCROLLBAR_H__

#include "wx/wx.h"
#include "wx/scrolbar.h"
class AmayaScrollBar : public wxScrollBar
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaScrollBar)

  AmayaScrollBar( wxWindow* parent = NULL, wxWindowID id = -1, long style = wxSB_HORIZONTAL );
  virtual ~AmayaScrollBar( );

protected:
  DECLARE_EVENT_TABLE()

  void OnSetFocus( wxFocusEvent & event );
  void OnKillFocus( wxFocusEvent & event );

};

#endif // __AMAYASCROLLBAR_H__

#endif /* #ifdef _WX */

