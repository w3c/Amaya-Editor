#ifdef _WX

#ifndef __AMAYAURLBAR_H__
#define __AMAYAURLBAR_H__

#include "wx/panel.h"

class AmayaFrame;
class AmayaWindow;

class wxComboBox;
class wxBitmapButton;

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

  void OnURLSelected( wxCommandEvent& event );
  //  void OnURLText( wxCommandEvent& event );
  void OnURLTextEnter( wxCommandEvent& event );

  void Clear();
  void Append( const wxString & newurl );
  void SetValue( const wxString & newurl );
  wxString GetValue();

  //  void OnChar( wxKeyEvent& event );

 protected:
  DECLARE_EVENT_TABLE()
  
  AmayaWindow * m_pAmayaWindowParent;

  wxComboBox * m_pComboBox;
  wxBitmapButton * m_pValidateButton;
};

#endif // __AMAYAURLBAR_H__

#endif /* #ifdef _WX */
