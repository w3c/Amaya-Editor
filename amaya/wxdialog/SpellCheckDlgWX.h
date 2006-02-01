#ifdef _WX

#ifndef __SPELLCHECKDLGWX_H__
#define __SPELLCHECKDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "wx/spinctrl.h"
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
		   wxWindow* parent,
		   int checkingArea );
  
  // Destructor.                  
  virtual ~SpellCheckDlgWX();

private:
    // Override base class functions of a wxDialog.
  void OnCancelButton( wxCommandEvent& event );
  void OnSkipWithoutButton( wxCommandEvent& event );
  void OnSkipWithButton( wxCommandEvent& event );
  void OnReplaceWithoutButton( wxCommandEvent& event );
  void OnIgnoreCapitalsChkBox ( wxCommandEvent& event );
  void OnIgnoreArabicsChkBox ( wxCommandEvent& event );
  void OnIgnoreRomansChkBox ( wxCommandEvent& event );
  void OnIgnoreSpecialsChkBox ( wxCommandEvent& event );
  void OnAreaRadioBox ( wxCommandEvent& event );
  void OnSpecialChar ( wxCommandEvent& event );
  void OnChangeProposals ( wxCommandEvent& event );
  void Set_Proposals ();

 // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
    
private:
  int        m_base;
  int        m_ref;
  int        m_max_proposals;
  wxString   m_special_char;
};

#endif  //__SPELLCHECKDLGWX_H__

#endif /* _WX */
