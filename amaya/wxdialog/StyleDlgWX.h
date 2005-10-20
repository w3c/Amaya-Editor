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

  // Destructor.
  virtual ~StyleDlgWX();

private:
  void GetValueDialog_Text();
  void GetValueDialog_Color();
  void GetValueDialog_Box();

  void OnOk( wxCommandEvent& event );
  void OnCancel( wxCommandEvent& event );
  void OnDefault( wxCommandEvent& event );
  void OnPageChanged( wxNotebookEvent& event );

  void OnColorPalette( wxCommandEvent& event );
  void OnColorChanged( wxCommandEvent& event );
  void OnColorTextChanged( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  // "Color" tab
  wxColourData colour_data;
  int          m_ref;
  bool         m_IsInitialized;
};

#endif  // __STYLEDLGWX_H__

#endif /* _WX */
