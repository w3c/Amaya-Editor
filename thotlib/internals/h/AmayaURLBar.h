#ifdef _WX

#ifndef __AMAYAURLBAR_H__
#define __AMAYAURLBAR_H__

#include "wx/wx.h"
#include "wx/panel.h"

class AmayaFrame;
class AmayaWindow;

/*
 * =====================================================================================
 *        Class:  AmayaURLBar
 * 
 *       Author:  Stephane GULLY
 *      Created:  06/05/2004 04:45:34 PM CET
 * =====================================================================================
 */
class AmayaURLBar : public wxPanel
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaURLBar)

  AmayaURLBar ( wxWindow *     parent = NULL
		,AmayaWindow *  amaya_window_parent = NULL );
  virtual ~AmayaURLBar();

  void Clear();
  void Append( const wxString & newurl );
  void SetValue( const wxString & newurl );
  wxString GetValue();

 protected:
  DECLARE_EVENT_TABLE()
  void OnURLSelected( wxCommandEvent& event );
  void OnURLTextEnter( wxCommandEvent& event );
  
  AmayaWindow *    m_pAmayaWindowParent;
  wxComboBox *     m_pComboBox;
  wxBitmapButton * m_pValidateButton;
};

#endif // __AMAYAURLBAR_H__

#endif /* #ifdef _WX */
