#ifdef _WX

#ifndef __AMAYAURLBAR_H__
#define __AMAYAURLBAR_H__

#include "wx/combobox.h"

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
class AmayaURLBar : public wxComboBox
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaURLBar)

  AmayaURLBar ( wxWindow *     parent = NULL
		,AmayaWindow *  amaya_window_parent = NULL );
  virtual ~AmayaURLBar();

  void OnURLSelected( wxCommandEvent& event );
  void OnURLText( wxCommandEvent& event );
  void OnURLTextEnter( wxCommandEvent& event );

  void OnSize( wxSizeEvent& event );
  void ReSize( int width, int height );

 protected:
  DECLARE_EVENT_TABLE()
  
  AmayaWindow * m_pAmayaWindowParent;
  bool          m_NewURLSelected;
  bool          m_URLLocked;
};

#endif // __AMAYAURLBAR_H__

#endif /* #ifdef _WX */
