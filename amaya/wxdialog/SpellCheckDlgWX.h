#ifdef _WX

#ifndef __SPELLCHECKDLGWX_H__
#define __SPELLCHECKDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "AmayaDialog.h"

//-----------------------------------------------------------------------------
// Class definition: SpellCheckDlgWX
//-----------------------------------------------------------------------------

class SpellCheckDlgWX : public AmayaDialog
{
public: 
    
  // Constructor.
  SpellCheckDlgWX( int ref,
		   int base,
		   wxWindow* parent );
  
  // Destructor.                  
  virtual ~SpellCheckDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnCancelButton( wxCommandEvent& event );
  void OnSkipWithoutButton( wxCommandEvent& event );
  void OnSkipWithButton( wxCommandEvent& event );
  void OnReplaceWithoutButton( wxCommandEvent& event );
  void OnReplaceWithButton( wxCommandEvent& event );
  void OnIgnoreCapitalsChkBox ( wxCommandEvent& event );
  void OnIgnoreArabicsChkBox ( wxCommandEvent& event );
  void OnIgnoreRomansChkBox ( wxCommandEvent& event );
  void OnIgnoreSpecialsChkBox ( wxCommandEvent& event );
  void OnAreaRadioBox ( wxCommandEvent& event );
 // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  wxString   m_Printer;
  int        m_base;
  int        m_ref;
};

#endif  //__SPELLCHECKDLGWX_H__

#endif /* _WX */
