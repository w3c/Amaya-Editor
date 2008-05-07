#ifdef _WX

#ifndef __STYLEDLGWX_H__
#define __STYLEDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "wx/notebook.h"
#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: HRefDlgWX
//-----------------------------------------------------------------------------

class StyleDlgWX : public AmayaDialog
{

public: 
    
  // Constructor.
  StyleDlgWX( int ref,
              wxWindow* parent );
  void SetValue (const char *property, const char *value);

  // Destructor.
  virtual ~StyleDlgWX();

private:
  void InitValues ();
  void GetValueDialog_Text();
  void GetValueDialog_Color();
  void GetValueDialog_Box();
  void GetValueDialog_Format();

  void OnOk( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );
  void OnDefault( wxCommandEvent& event );
  void OnButton( wxCommandEvent& event );
  void OnValueChanged( wxCommandEvent& event );
  void SetColorTextChanged (int id);
  void OnColorPalette( wxCommandEvent& event );
  void OnColorChanged( wxCommandEvent& event );
  void OnColorTextChanged( wxCommandEvent& event );
  void OnBrowseButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  // "Color" tab
  wxColourData colour_data;
  int          m_ref;
  wxColour     m_OffColour;
  static bool  m_OnApplyLock;
};

#endif  // __STYLEDLGWX_H__

#endif /* _WX */
