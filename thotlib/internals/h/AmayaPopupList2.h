#ifdef _WX

#ifndef __AMAYAPOPUPLIST_H__
#define __AMAYAPOPUPLIST_H__

#include "wx/wx.h"
#include "wx/popupwin.h"

/*
 * =====================================================================================
 *        Class:  AmayaPopupList
 * 
 *       Author:  Stephane GULLY
 *      Created:  06/05/2004 04:45:34 PM CET
 * =====================================================================================
 */
class AmayaPopupList : public wxPopupTransientWindow
{

 public:
  DECLARE_DYNAMIC_CLASS(AmayaPopupList)

  AmayaPopupList( wxWindow * p_windows = NULL, int ref = 0 );
  virtual ~AmayaPopupList();

  void Append( int i, const wxString & label );
  virtual void Popup(wxWindow *focus);

 protected:
  DECLARE_EVENT_TABLE()
  void OnActivateItem( wxCommandEvent& event );
  void OnDismiss();

 protected:
  int m_Ref;
  wxListBox * m_pListBox;
};

#endif // __AMAYAPOPUPLIST_H__

#endif /* #ifdef _WX */
