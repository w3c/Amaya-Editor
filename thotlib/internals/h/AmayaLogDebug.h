#ifdef _WX

#ifndef __AMAYALOGDEBUG_H__
#define __AMAYALOGDEBUG_H__

#ifdef __WXDEBUG__

#include "wx/wx.h"
#include "wx/dialog.h"

/*
 * =====================================================================================
 *        Class:  AmayaLogDebug
 * 
 *  Description:  This is the dialog used for debuging purpose
 *       Author:  Stephane GULLY
 *      Created:  08/07/2004 04:45:34 PM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaLogDebug : public wxDialog
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaLogDebug)

  AmayaLogDebug( wxWindow * p_parent = NULL );
  virtual ~AmayaLogDebug();

 protected:
  DECLARE_EVENT_TABLE()
  void OnClose( wxCloseEvent& event );
  void OnCheckButton(wxCommandEvent& event);
  void OnTestCaseButton(wxCommandEvent& event);

  void DoCheck(int id, bool checked);

 protected:
  wxSizer * m_pTopSizer;
  wxButton * m_pCancelButton;
  wxButton * m_pTestCaseButton;
};

#endif /* #ifdef __WXDEBUG__ */

#endif // __AMAYALOGDEBUG_H__

#endif /* #ifdef _WX */
