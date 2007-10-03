#ifdef _WX

#ifndef __SENDBYMAILDLGWX_H__
#define __SENDBYMAILDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "wx/spinctrl.h"
#include "AmayaDialog.h"

class wxGrid;
class wxGridEvent;

//-----------------------------------------------------------------------------
// Class definition: SendByMailDlgWX
//-----------------------------------------------------------------------------

class SendByMailDlgWX : public AmayaDialog
{
public: 
    
  // Constructor.
  SendByMailDlgWX( int ref, wxWindow* parent);
  
  // Destructor.                  
  virtual ~SendByMailDlgWX();

  wxString GetSubject()const;
  void SetSubject(const wxString& subject);
  
  wxString GetMessage()const;
  void SetMessage(const wxString& message);
  
  bool SendAsAttachment()const;
  bool SendAsContent()const;

  void SetSendMode(int mode);
    
  wxArrayString  GetRecipients()const;
  void SetRecipients(const wxArrayString & rcpt);
  
  wxString GetRecipientList()const;
  
  void AddAddressToRecentList(const wxString& addr);

private:
  void UpdateMessageLabel();

    // Override base class functions of a wxDialog.
  void OnCancelButton( wxCommandEvent& event );

  void OnUpdateSendButton(wxUpdateUIEvent& event);

  void OnChangeMessageClass(wxCommandEvent& event);

  void SaveRecentList();
  void LoadRecentList();
  
  void OnCloseDialog(wxCommandEvent& event);
  void OnGridCellChange(wxGridEvent& event);
  
  void OnSize(wxSizeEvent& event);
  
 // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

  int m_ref;
  int m_currTo;
  
  wxArrayString m_rcptArray;
  
  wxGrid*     m_grid;
};

#endif  //__SENDBYMAILDLGWX_H__

#endif /* _WX */
