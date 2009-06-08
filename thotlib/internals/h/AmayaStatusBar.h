#ifdef _WX

#ifndef __AMAYASTATUSBAR_H__
#define __AMAYASTATUSBAR_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/dialog.h"
#include "AmayaPathControl.h"

class AmayaStatusText;

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
  void EnableLock( int status );

  virtual void SetStatusText(const wxString& text, int i = 0); 
  void SetSelectedElement(Element elem);

  int HitTest(const wxPoint& pt);
  
 private:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  void OnLogErrorButton(wxCommandEvent& event);
  void OnLockButton(wxCommandEvent& event);
  void OnSize(wxSizeEvent& event);
  void OnUpdateUI(wxUpdateUIEvent& event);
  
  void OnDoubleClic(wxMouseEvent& event);
  
  void OnMouseEnter(wxMouseEvent& event);
  void OnMouseExit(wxMouseEvent& event);
  void OnMouseMove(wxMouseEvent& event);
   
 protected:
  enum
    {
      Field_Path,
      Field_Text,
      Field_InsertMode,
      Field_Lock,
      Field_LogError,
      Field_Max
    };
  
  wxBitmapButton   *m_pLogErrorButton;
  wxBitmap          m_LogErrorBmp_Green;
  wxBitmap          m_LogErrorBmp_Red;
  wxBitmapButton   *m_pLockButton;
  wxBitmap          m_Lock_Yes;
  wxBitmap          m_Lock_No;
  AmayaPathControl *m_pathCtrl;
  AmayaStatusText  *m_insertMode;
};

#endif  // __AMAYASTATUSBAR_H__

#endif /* _WX */
