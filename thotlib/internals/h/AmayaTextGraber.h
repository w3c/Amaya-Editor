#ifndef __AMAYATEXTGRABER_H__
#define __AMAYATEXTGRABER_H__

#include "wx/textctrl.h"

class AmayaFrame;

// a text ctrl which allows to call different wxTextCtrl functions
// interactively by pressing function keys in it
class AmayaTextGraber : public wxTextCtrl
{
 public:
  AmayaTextGraber( int frame_id, wxWindow *parent, wxWindowID id, const wxString &value,
		   const wxPoint &pos, const wxSize &size, int style = 0 )
    : wxTextCtrl(parent, id, value, pos, size, style),
    m_AmayaFrameId( frame_id )
    {
    }
  
  void OnKeyDown( wxKeyEvent& event );
  void OnKeyUp(   wxKeyEvent& event );
  void OnChar(    wxKeyEvent& event );  
  void OnText(    wxCommandEvent& event );

 private:
  
#ifdef __WXDEBUG__
    static inline wxChar GetChar(bool on, wxChar c) { return on ? c : _T('-'); }
    void LogKeyEvent(const wxChar *name, wxKeyEvent& event) const;
#endif /* #ifdef __WXDEBUG__ */
    
    static int m_ThotMask;
    int m_AmayaFrameId;
    
    DECLARE_EVENT_TABLE()
};

#endif /* __AMAYATEXTGRABER_H__ */
