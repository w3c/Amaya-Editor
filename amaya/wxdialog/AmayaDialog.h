#ifdef _WX

#ifndef __AMAYADIALOG_H__
#define __AMAYADIALOG_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/dialog.h"

//-----------------------------------------------------------------------------
// Class definition: AmayaDialog
//-----------------------------------------------------------------------------

class AmayaDialog : public wxDialog
{
 public: 
    
  // Constructor.
  AmayaDialog( wxWindow * p_parent, int ref );
    
  // Destructor.                  
  virtual ~AmayaDialog();

 private:
  void OnClose( wxCloseEvent& event );

  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

 protected:
  int m_Ref;
};

#endif  // __AMAYADIALOG_H__

#endif /* _WX */
