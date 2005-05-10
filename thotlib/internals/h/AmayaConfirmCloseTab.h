#ifdef _WX

#ifndef __AMAYACONFIRMCLOSETAB_H__
#define __AMAYACONFIRMCLOSETAB_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/dialog.h"

//-----------------------------------------------------------------------------
// Class definition: AmayaConfirmCloseTab
//-----------------------------------------------------------------------------

class AmayaConfirmCloseTab : public wxDialog
{
 public: 
  DECLARE_DYNAMIC_CLASS(AmayaConfirmCloseTab)
  // Constructor.
  AmayaConfirmCloseTab( wxWindow * p_parent = NULL, int nb_tab = 0 );
    
  // Destructor.                  
  virtual ~AmayaConfirmCloseTab();

  static bool DoesUserWantToShowMe();

 private:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  void OnOk(wxCommandEvent& event);
  void OnCancel(wxCommandEvent& event);
};

#endif  // __AMAYACONFIRMCLOSETAB_H__

#endif /* _WX */
