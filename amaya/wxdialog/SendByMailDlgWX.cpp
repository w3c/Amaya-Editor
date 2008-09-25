/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaApp.h"
#include "SendByMailDlgWX.h"

#include <wx/valtext.h>
#include <wx/tokenzr.h>
#include <wx/recguard.h>
#include <wx/grid.h>


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
//  EVT_UPDATE_UI(  wxID_OK,                    SendByMailDlgWX::OnUpdateSendButton)
  EVT_RADIOBOX(   XRCID("wxID_RADIOBOX_SEND_CLASS"), SendByMailDlgWX::OnChangeMessageClass)
  
  EVT_BUTTON(wxID_OK,     SendByMailDlgWX::OnCloseDialog)
  EVT_BUTTON(wxID_CANCEL, SendByMailDlgWX::OnCloseDialog)
  
  EVT_BUTTON(XRCID("wxID_BUTTON_DEL"), SendByMailDlgWX::OnDeleteRecipient)
  EVT_COMBOBOX(XRCID("wxID_RECIPIENT_ADDRESS"), SendByMailDlgWX::OnChangeRecipientAddress)
  EVT_TEXT(XRCID("wxID_RECIPIENT_ADDRESS"), SendByMailDlgWX::OnChangeRecipientAddress)
  EVT_TEXT_ENTER(XRCID("wxID_RECIPIENT_ADDRESS"), SendByMailDlgWX::OnChangeRecipientAddress)

END_EVENT_TABLE()



/*----------------------------------------------------------------------
  SendByMailDlgWX create the Send by mail dialog 
  params:
    + parent : parent window
    + ps_file : postscript file
  ----------------------------------------------------------------------*/
SendByMailDlgWX::SendByMailDlgWX( int ref, wxWindow* parent) :
  AmayaDialog( parent, ref )
{
  m_ref = ref;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SendByMailDlgWX"));

  SetTitle(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_BY_MAIL)));

  // TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_TO_)));

  LoadRecentList();
  
  XRCCTRL(*this, "wxID_LABEL_SUBJECT",   wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SUBJECT_)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString((int)SendByMailAsAttachment, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_ATTACHMENT)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString((int)SendByMailAsMessage,    TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_MESSAGE)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString((int)SendByMailAsZip,        TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_ZIP)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString((int)DontSendByMail,         TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_DONT_SEND)) );

  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_EMAILS_SEND) ));

  UpdateMessageLabel();

  m_panel = new wxPanel(this, -1);
  m_panel->SetSizer(new wxBoxSizer(wxVERTICAL));
  AddRecipientLine();
  GetSizer()->Prepend(m_panel, 0, wxEXPAND|wxALL, 4);

  Layout();
  SetAutoLayout( TRUE );
  SetSize(600, 480);
  GetSizer()->SetSizeHints( this );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SendByMailDlgWX::~SendByMailDlgWX()
{
  /* when the dialog is destroyed, It important to cleanup context */
  ThotCallback (m_ref, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::UpdateMessageLabel()
{
  if(XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS", wxRadioBox)->GetSelection()==1)
  {
    XRCCTRL(*this, "wxID_LABEL_MESSAGE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_MESSAGE_ALTERN)) );
  }
  else
  {
    XRCCTRL(*this, "wxID_LABEL_MESSAGE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_MESSAGE_BODY)) );
  }
}

/*----------------------------------------------------------------------
  OnCancelButton called when clicking on cancel button
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::OnCancelButton( wxCommandEvent& event )
{
  ThotCallback (m_ref, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
  event.Skip();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::OnUpdateSendButton(wxUpdateUIEvent& event)
{
// TODO :
//  if(m_grid && m_grid->GetNumberRows()>0)
//    {
//      int n;
//      for(n=0; n<m_grid->GetNumberRows(); n++)
//        {
//          if(!(m_grid->GetCellValue(n, 0).Trim().IsEmpty()))
//            {
//              event.Enable(true);
//              return;
//            }
//        }
//    }
//  event.Enable(false);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::OnChangeMessageClass(wxCommandEvent& WXUNUSED(event))
{
  UpdateMessageLabel();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
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
  }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString SendByMailDlgWX::GetSubject()const
{
  return XRCCTRL(*this, "wxID_EDIT_SUBJECT",   wxTextCtrl)->GetValue();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::SetSubject(const wxString& subject)
{
  XRCCTRL(*this, "wxID_EDIT_SUBJECT",   wxTextCtrl)->SetValue(subject);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString SendByMailDlgWX::GetMessage()const
{
  return XRCCTRL(*this, "wxID_EDIT_MESSAGE",   wxTextCtrl)->GetValue();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::SetMessage(const wxString& message)
{
  XRCCTRL(*this, "wxID_EDIT_MESSAGE",   wxTextCtrl)->SetValue(message);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool SendByMailDlgWX::SendAsAttachment()const
{
  return XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->GetSelection()==SendByMailAsAttachment;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool SendByMailDlgWX::SendAsContent()const
{
  return XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->GetSelection()==SendByMailAsMessage;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool SendByMailDlgWX::SendAsZip()const
{
  return XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->GetSelection()==SendByMailAsZip;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::SetSendMode(int mode)
{
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetSelection(mode);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::LoadRecentList()
{
  char* lastRcpt = TtaGetEnvString ("EMAILS_LAST_RCPT");
  if(lastRcpt)
  {
    wxString rcpts(lastRcpt, wxConvUTF8);
    m_rcptArray = ::wxStringTokenize(wxString(rcpts, wxConvUTF8), wxT("|"));
  }
  if(m_rcptArray.GetCount()==0)
    m_rcptArray.Add(wxString(TtaGetEnvString ("EMAILS_FROM_ADDRESS"), wxConvUTF8));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::OnCloseDialog(wxCommandEvent& event)
{
  if(event.GetId()==wxID_OK)
    {
      wxSizerItemList& list = m_panel->GetSizer()->GetChildren();
      wxSizerItemList::compatibility_iterator iter;
      for(iter=list.GetFirst(); iter; iter=iter->GetNext())
        {
          wxSizerItem* item = iter->GetData();
          if(item)
            {
              RecipientPanel* panel = wxDynamicCast(item->GetWindow(), RecipientPanel);
              if(panel)
                {
                  if(!panel->GetAddress().IsEmpty())
                    {
                      wxString str = panel->GetAddress();
                      if(panel->GetType()==1)
                        m_ccArray.Add(str);
                      else
                        m_toArray.Add(str);
                      AddAddressToRecentList(str);
                    }
                }
            }
        }
    }

  SaveRecentList();
  event.Skip();
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::OnDeleteRecipient(wxCommandEvent& event)
{
  wxWindow* win = wxDynamicCast(event.GetEventObject(), wxWindow);
  if(win)
    {
      RecipientPanel* panel = wxDynamicCast(win->GetParent(), RecipientPanel);
      if(panel)
        {
          m_panel->GetSizer()->Detach(panel);
          delete panel;
          
          if(m_panel->GetSizer()->GetChildren().GetCount()==0)
            AddRecipientLine();
          Layout();
        }
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::AddRecipientLine()
{
  RecipientPanel* recipient;
  recipient = new RecipientPanel(m_panel);
  recipient->AddDefaultRecipients(m_rcptArray);
  m_panel->GetSizer()->Add(recipient, 0, wxEXPAND|wxBOTTOM, 4);
  Layout();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::OnChangeRecipientAddress(wxCommandEvent& event)
{
  wxSizerItemList& list = m_panel->GetSizer()->GetChildren();
  wxSizerItemList::compatibility_iterator iter = list.GetLast();
  if(iter)
    {
      wxSizerItem* item = iter->GetData();
      if(item)
        {
          RecipientPanel* panel = wxDynamicCast(item->GetWindow(), RecipientPanel);
          if(panel)
            {
              if(!panel->GetAddress().IsEmpty())
                AddRecipientLine();
            }
        }
    }

}



//
//
// RecipientPanel
//
//

IMPLEMENT_CLASS(RecipientPanel, wxPanel)

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
RecipientPanel::RecipientPanel(wxWindow* parent):
wxPanel()
{
  wxXmlResource::Get()->LoadPanel(this, parent, wxT("RecipientPanel"));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void RecipientPanel::AddDefaultRecipients(const wxArrayString& arr)
{
  XRCCTRL(*this, "wxID_RECIPIENT_ADDRESS", wxComboBox)->Append(arr);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString RecipientPanel::GetAddress()const
{
  return XRCCTRL(*this, "wxID_RECIPIENT_ADDRESS", wxComboBox)->GetValue();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int RecipientPanel::GetType()const
{
  return XRCCTRL(*this, "wxID_RECIPIENT_TYPE", wxChoice)->GetSelection();
}

#endif /* _WX */
