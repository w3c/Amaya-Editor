#ifdef _WX

#ifndef FONTDLGWX_H_
#define FONTDLGWX_H_

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition
//-----------------------------------------------------------------------------

class FontDlgWX : public wxDialog
{
public:
  FontDlgWX(wxWindow* parent, const wxString& title);
  
  int GetFontFamily()const;
  int GetFontSize()const;
  
  void SetFontFamily(int family);
  void SetFontSize(int size);
  void OnCancel( wxCommandEvent& event );
  void OnOk( wxCommandEvent& event );

  DECLARE_EVENT_TABLE()
};


#endif /*FONTDLGWX_H_*/

#endif /* _WX */
