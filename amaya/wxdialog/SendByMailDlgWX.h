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

typedef enum
{
  SendByMailAsAttachment = 0,
  SendByMailAsMessage    = 1,
  SendByMailAsZip        = 2,
  DontSendByMail         = 3
}SendByMailMode;


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
  bool SendAsZip()const;

  void SetSendMode(int mode);
    
  wxArrayString GetToRecipients()const{return m_toArray;}
  wxArrayString GetCcRecipients()const{return m_ccArray;}
  
  void AddAddressToRecentList(const wxString& addr);

  
protected:
  void AddRecipientLine();
private:
  void UpdateMessageLabel();

    // Override base class functions of a wxDialog.
  void OnCancelButton( wxCommandEvent& event );

  void OnUpdateSendButton(wxUpdateUIEvent& event);

  void OnChangeMessageClass(wxCommandEvent& event);

  void SaveRecentList();
  void LoadRecentList();
  
  void OnCloseDialog(wxCommandEvent& event);

  
  
  void OnDeleteRecipient(wxCommandEvent& event);
  void OnChangeRecipientAddress(wxCommandEvent& event);
  
 // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()

  int m_ref;
  int m_currTo;
  
  wxArrayString m_rcptArray;
  
  wxArrayString m_toArray, m_ccArray;
  
  
  wxPanel* m_panel;
};



//-----------------------------------------------------------------------------
// Class definition: RecipientPanel
//-----------------------------------------------------------------------------
class RecipientPanel : public wxPanel
{
  DECLARE_CLASS(RecipientPanel)
public:
  RecipientPanel(wxWindow* parent);
  void AddDefaultRecipients(const wxArrayString& arr);
  
  wxString GetAddress()const;
  int      GetType()const;
};


#endif  //__SENDBYMAILDLGWX_H__

#endif /* _WX */
