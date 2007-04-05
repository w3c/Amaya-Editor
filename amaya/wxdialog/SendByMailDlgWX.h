#ifdef _WX

#ifndef __SENDBYMAILDLGWX_H__
#define __SENDBYMAILDLGWX_H__

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "wx/spinctrl.h"
#include "AmayaDialog.h"

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

  void OnNewToTextModified(wxCommandEvent& event);
  void OnNewToEnterPressed(wxCommandEvent& event);
  
  
  void OnToItemSelected(wxCommandEvent& event);

  void OnSupprToItem(wxCommandEvent& event);

  void OnUpdateSendButton(wxUpdateUIEvent& event);
  
  void OnChangeMessageClass(wxCommandEvent& event);

  void SetCurrentToItemText();
  void SuggestAddress();
  void FillRecentAddress();
  void SaveRecentList();
  void LoadRecentList();
  
  void OnCloseDialog(wxCommandEvent& event);

 // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

  int m_ref;
  int m_currTo;
  
  wxArrayString m_rcptArray;
  
  wxListBox*  m_tos;
  wxComboBox* m_newto;
};

#endif  //__SENDBYMAILDLGWX_H__

#endif /* _WX */
