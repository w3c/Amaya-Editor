#ifdef _WX

#ifndef __AMAYAPOPUPLIST_H__
#define __AMAYAPOPUPLIST_H__

#include "wx/wx.h"
#include "wx/panel.h"

class AmayaPopupL : public wxListBox
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaPopupL)

  AmayaPopupL( wxWindow * parent = NULL );
  virtual ~AmayaPopupL();

 protected:
  DECLARE_EVENT_TABLE()
  void OnKillFocus( wxFocusEvent & event );
  void OnSetFocus( wxFocusEvent & event );
};

/*
 * =====================================================================================
 *        Class:  AmayaURLBar
 * 
 *       Author:  Stephane GULLY
 *      Created:  06/05/2004 04:45:34 PM CET
 * =====================================================================================
 */
class AmayaPopupList : public wxPanel
{

 public:
  DECLARE_DYNAMIC_CLASS(AmayaPopupList)

  AmayaPopupList ( wxWindow * parent = NULL, wxPoint pos = wxDefaultPosition );
  virtual ~AmayaPopupList();

  void Append( const wxString & item );
  virtual void SetSize( int w, int h );

 protected:
  DECLARE_EVENT_TABLE()
  void OnKillFocus( wxFocusEvent & event );

 protected:
  AmayaPopupL * m_pList;

};

#endif // __AMAYAPOPUPLIST_H__

#endif /* #ifdef _WX */
