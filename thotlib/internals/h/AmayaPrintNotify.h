#ifdef _WX

#ifndef __AMAYAPRINTNOTIFY_H__
#define __AMAYAPRINTNOTIFY_H__

#include "wx/wx.h"
#include "wx/dialog.h"

/*
 * =====================================================================================
 *        Class:  AmayaPrintNotify
 * 
 *  Description:  This is the dialog used to warn the user, something is printing
 *       Author:  Stephane GULLY
 *      Created:  08/07/2004 04:45:34 PM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaPrintNotify : public wxDialog
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaPrintNotify)

  AmayaPrintNotify( );
  virtual ~AmayaPrintNotify();
  
  void SetMessage( const char * p_message );

 protected:
  DECLARE_EVENT_TABLE()
  void OnButton( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event );

 protected:
  wxStaticText * m_pLabel;
  wxButton *     m_pCancelButton;
  wxBoxSizer *   m_pTopSizer;
};

#endif // __AMAYAPRINTNOTIFY_H__

#endif /* #ifdef _WX */
