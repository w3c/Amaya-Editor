#ifdef _WX

#ifndef __AMAYASTATUSBAR_H__
#define __AMAYASTATUSBAR_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/dialog.h"
#include "AmayaPathControl.h"

//-----------------------------------------------------------------------------
// Class definition: AmayaStatusBar
//-----------------------------------------------------------------------------

class AmayaStatusBar : public wxStatusBar
{
 public: 
  DECLARE_DYNAMIC_CLASS(AmayaStatusBar)
  // Constructor.
  AmayaStatusBar( wxWindow * p_parent = NULL );
    
  // Destructor.                  
  virtual ~AmayaStatusBar();

  void EnableLogError( bool enable );

  virtual void SetStatusText(const wxString& text, int i = 0); 
  void SetSelectedElement(Element elem);

 private:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  void OnLogErrorButton(wxCommandEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnUpdateUI(wxUpdateUIEvent& event);
  
 protected:
  enum
    {
      Field_Path,
      Field_Text,
      Field_InsertMode,
      Field_LogError,
      Field_Max
    };
  
  wxBitmapButton * m_pLogErrorButton;
  wxBitmap         m_LogErrorBmp_Green;
  wxBitmap         m_LogErrorBmp_Red;
  AmayaPathControl* m_pathCtrl; 
};

#endif  // __AMAYASTATUSBAR_H__

#endif /* _WX */
