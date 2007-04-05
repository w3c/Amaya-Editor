#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SendByMailDlgWX.h"

#include <wx/valtext.h>
#include <wx/tokenzr.h>
#include <wx/recguard.h>


#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

#define MAX_LAST_RCPT_COUNT 24

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SendByMailDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       SendByMailDlgWX::OnCancelButton)
  EVT_TEXT(       XRCID("wxID_COMBO_NEW_TO"),  SendByMailDlgWX::OnNewToTextModified)
  EVT_TEXT_ENTER( XRCID("wxID_COMBO_NEW_TO"),  SendByMailDlgWX::OnNewToEnterPressed)
  
  EVT_LISTBOX(    XRCID("wxID_LIST_TO"),      SendByMailDlgWX::OnToItemSelected)
  EVT_MENU(       wxID_DELETE,                SendByMailDlgWX::OnSupprToItem)
  EVT_UPDATE_UI(  wxID_OK,                    SendByMailDlgWX::OnUpdateSendButton)
  EVT_RADIOBOX(   XRCID("wxID_RADIOBOX_SEND_CLASS"), SendByMailDlgWX::OnChangeMessageClass)
  
  EVT_BUTTON(wxID_OK,     SendByMailDlgWX::OnCloseDialog)
  EVT_BUTTON(wxID_CANCEL, SendByMailDlgWX::OnCloseDialog)
END_EVENT_TABLE()



/*----------------------------------------------------------------------
  SendByMailDlgWX create the Send by mail dialog 
  params:
    + parent : parent window
    + ps_file : postscript file
  ----------------------------------------------------------------------*/
SendByMailDlgWX::SendByMailDlgWX( int ref, wxWindow* parent) :
  AmayaDialog( parent, ref ),
  m_currTo(wxID_ANY)
{
  m_ref = ref;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SendByMailDlgWX"));
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_EMAILS_SEND_BY_MAIL) );
  SetTitle( wx_title );

  XRCCTRL(*this, "wxID_LABEL_TO",   wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_TO_)) );
  XRCCTRL(*this, "wxID_LABEL_SUBJECT",   wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SUBJECT_)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_ATTACHMENT)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_MESSAGE)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString(2, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_DONT_SEND)) );

  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_EMAILS_SEND) ));

  m_tos   = XRCCTRL(*this, "wxID_LIST_TO",     wxListBox);
  m_newto = XRCCTRL(*this, "wxID_COMBO_NEW_TO", wxComboBox);

  wxAcceleratorEntry entries[2];
  entries[0].Set(wxACCEL_NORMAL,  WXK_DELETE, wxID_DELETE);
  entries[1].Set(wxACCEL_NORMAL,  WXK_BACK, wxID_DELETE);
  wxAcceleratorTable accel(2, entries);
  m_tos->SetAcceleratorTable(accel);

  UpdateMessageLabel();

  LoadRecentList();
  
  FillRecentAddress();
  m_newto->SetValue(wxT(""));

  Layout();
  SetAutoLayout( TRUE );
  SetSize(600, 400);
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SendByMailDlgWX::~SendByMailDlgWX()
{
  /* when the dialog is destroyed, It important to cleanup context */
  ThotCallback (m_ref, INTEGER_DATA, (char*) 0);
}

void SendByMailDlgWX::UpdateMessageLabel()
{
  if(XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS", wxRadioBox)->GetSelection()==1)
  {
    XRCCTRL(*this, "wxID_LABEL_MESSAGE",   wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_MESSAGE_ALTERN)) );
  }
  else
  {
    XRCCTRL(*this, "wxID_LABEL_MESSAGE",   wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_MESSAGE_BODY)) );
  }
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancel button
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 0);
  event.Skip();
}

void SendByMailDlgWX::OnNewToTextModified(wxCommandEvent& WXUNUSED(event))
{
  SetCurrentToItemText();
  SuggestAddress();
}

void SendByMailDlgWX::OnNewToEnterPressed(wxCommandEvent& WXUNUSED(event))
{
  SetCurrentToItemText();
  AddAddressToRecentList(m_newto->GetValue());
  m_currTo = m_tos->Append(wxT(""));
  m_tos->SetSelection(m_currTo);
  m_newto->Clear();
}

void SendByMailDlgWX::OnToItemSelected(wxCommandEvent& event)
{
  m_currTo = event.GetSelection();
  m_newto->SetValue(m_tos->GetString(m_currTo));
}

void SendByMailDlgWX::OnSupprToItem(wxCommandEvent& WXUNUSED(event))
{
  if(m_currTo!=wxID_ANY)
  {
    m_newto->Clear();
    m_tos->Delete(m_currTo);
    if(m_currTo>=m_tos->GetCount())
      m_currTo = m_tos->GetCount()-1;
    m_tos->SetSelection(m_currTo);
  }
}

void SendByMailDlgWX::OnUpdateSendButton(wxUpdateUIEvent& event)
{
  event.Enable(m_tos->GetCount()>0);
}

void SendByMailDlgWX::OnChangeMessageClass(wxCommandEvent& WXUNUSED(event))
{
  UpdateMessageLabel();
}


/**----------------------------------------------------------------------
  SetCurrentToItemText
  Set the label of the current to item in the list
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::SetCurrentToItemText()
{
  wxString str = m_newto->GetValue();

  if(m_currTo==wxID_ANY)
  {
    if(!str.IsEmpty())
    {
      m_currTo = m_tos->Append(str);
      m_tos->SetSelection(m_currTo);
    }
  }
  else
  {
    m_tos->SetString(m_currTo, str);
  }
}

void SendByMailDlgWX::SuggestAddress()
{
  long from, to;
  wxString str = m_newto->GetValue();
  m_newto->GetSelection(&from, &to);
  if(from+1==(int)str.Length())
  {
    if(str.Length()>2)
    {
      int i;
      for(i=0; i<(int)m_rcptArray.GetCount(); i++)
      {
        if(m_rcptArray[i].StartsWith((const wxChar*)str))
        {
          m_newto->SetValue(m_rcptArray[i]);
          m_newto->SetSelection(str.Length(), -1);
          break;
        }
      }
    }
  }
}

void SendByMailDlgWX::AddAddressToRecentList(const wxString& addr)
{
  if(!addr.IsEmpty())
  {
    int pos = m_rcptArray.Index(addr);
    if(pos!=wxNOT_FOUND)
      m_rcptArray.RemoveAt(pos);
    m_rcptArray.Insert(addr, 0);
    if(m_rcptArray.GetCount()>MAX_LAST_RCPT_COUNT)
    {
      m_rcptArray.RemoveAt(m_rcptArray.GetCount()-1);
    }
    FillRecentAddress();
  }
}

void SendByMailDlgWX::FillRecentAddress()
{
  m_newto->Clear();
  m_newto->Append(m_rcptArray);
}

wxString SendByMailDlgWX::GetSubject()const
{
  return XRCCTRL(*this, "wxID_EDIT_SUBJECT",   wxTextCtrl)->GetValue();
}

void SendByMailDlgWX::SetSubject(const wxString& subject)
{
  XRCCTRL(*this, "wxID_EDIT_SUBJECT",   wxTextCtrl)->SetValue(subject);
}

wxString SendByMailDlgWX::GetMessage()const
{
  return XRCCTRL(*this, "wxID_EDIT_MESSAGE",   wxTextCtrl)->GetValue();
}

void SendByMailDlgWX::SetMessage(const wxString& message)
{
  XRCCTRL(*this, "wxID_EDIT_MESSAGE",   wxTextCtrl)->SetValue(message);
}

bool SendByMailDlgWX::SendAsAttachment()const
{
  return XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->GetSelection()==0;
}

bool SendByMailDlgWX::SendAsContent()const
{
  return XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->GetSelection()==1;
}

void SendByMailDlgWX::SetSendMode(int mode)
{
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetSelection(mode);
}


wxArrayString  SendByMailDlgWX::GetRecipients()const
{
  return m_tos->GetStrings();
}

void SendByMailDlgWX::SetRecipients(const wxArrayString & rcpt)
{
  m_tos->Append(rcpt);
}

wxString SendByMailDlgWX::GetRecipientList()const
{
  wxString str;
  if(m_tos->GetCount()>0)
  {
    for(int i=0; i< m_tos->GetCount(); i++)
    {
      wxString s = m_tos->GetString(i);
      if(!s.IsEmpty())
        str << wxT("|") << s;
    }
  }
  str.Remove(0,1);
  return str;
}

void SendByMailDlgWX::SaveRecentList()
{
  /* Save m_rcptArray .*/
  int i;
  if(m_rcptArray.GetCount()>0)
  {
    wxString str = m_rcptArray[0];
    for(i=1; i<(int)m_rcptArray.GetCount(); i++)
    {
      str << wxT("|") << m_rcptArray[i];
    }
    TtaSetEnvString("EMAILS_LAST_RCPT", (char*)(const char*)str.mb_str(wxConvUTF8), TRUE);
  }
}

void SendByMailDlgWX::LoadRecentList()
{
  char* lastRcpt = TtaGetEnvString ("EMAILS_LAST_RCPT");
  if(lastRcpt)
  {
    wxString rcpts(lastRcpt, wxConvUTF8);
    wxStringTokenizer tkz(wxString(rcpts, wxConvUTF8), wxT("|"));
    while ( tkz.HasMoreTokens() )
    {
      m_rcptArray.Add(tkz.GetNextToken());
    }
  }
  if(m_rcptArray.GetCount()==0)
  {
    m_rcptArray.Add(wxString(TtaGetEnvString ("EMAILS_FROM_ADDRESS"), wxConvUTF8));
  }
}

void SendByMailDlgWX::OnCloseDialog(wxCommandEvent& event)
{
  SaveRecentList();
  event.Skip();
}

#endif /* _WX */
