#ifdef _WX

#ifndef __ENUMLISTDLGWX_H__
#define __ENUMLISTDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: EnumListDlgWX
//-----------------------------------------------------------------------------
class EnumListDlgWX : public AmayaDialog
{
public: 
  // Constructor.
  EnumListDlgWX( int ref,
		 int subref,
		 wxWindow* parent,
		 const wxString & title,
		 const wxString & label,
		 const wxArrayString& items,
		 int selection );
    
  // Destructor.                  
  virtual ~EnumListDlgWX();

private:
  void OnOkButton( wxCommandEvent& event );
  void OnCancelButton( wxCommandEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

private:
  int m_SubRef;
  wxRadioBox * m_pRadiobox;
};

#endif  //__ENUMLISTDLGWX_H__

#endif /* _WX */
