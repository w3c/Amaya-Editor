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
  EVT_UPDATE_UI(  wxID_OK,                    SendByMailDlgWX::OnUpdateSendButton)
  EVT_RADIOBOX(   XRCID("wxID_RADIOBOX_SEND_CLASS"), SendByMailDlgWX::OnChangeMessageClass)
  
  EVT_BUTTON(wxID_OK,     SendByMailDlgWX::OnCloseDialog)
  EVT_BUTTON(wxID_CANCEL, SendByMailDlgWX::OnCloseDialog)
  
  EVT_GRID_CMD_CELL_CHANGE(wxID_ANY, SendByMailDlgWX::OnGridCellChange)
  EVT_SIZE(SendByMailDlgWX::OnSize)
END_EVENT_TABLE()



/*----------------------------------------------------------------------
  SendByMailDlgWX create the Send by mail dialog 
  params:
    + parent : parent window
    + ps_file : postscript file
  ----------------------------------------------------------------------*/
SendByMailDlgWX::SendByMailDlgWX( int ref, wxWindow* parent) :
  AmayaDialog( parent, ref ),
  m_grid(NULL)
{
  m_ref = ref;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SendByMailDlgWX"));
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_EMAILS_SEND_BY_MAIL) );
  SetTitle( wx_title );

  wxPanel* panel = XRCCTRL(*this, "wxID_PANEL_MAIL_HEADER", wxPanel);
  if(panel)
    {
      m_grid = new wxGrid(panel, wxID_ANY);
      panel->GetSizer()->Prepend(m_grid, 1, wxEXPAND);
      m_grid->CreateGrid(1, 1);
      m_grid->SetRowLabelSize(0);
      m_grid->SetDefaultColSize(2);
      m_grid->SetColLabelValue(0, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_TO_)));
      m_grid->SetColMinimalWidth(0, 160);
      m_grid->SetColSize(0, m_grid->GetSize().x-20);
      LoadRecentList();
      m_grid->SetDefaultEditor(new wxGridCellChoiceEditor(m_rcptArray, true));      
    }
  
  XRCCTRL(*this, "wxID_LABEL_SUBJECT",   wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SUBJECT_)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString((int)SendByMailAsAttachment, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_ATTACHMENT)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString((int)SendByMailAsMessage,    TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_MESSAGE)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString((int)SendByMailAsZip,        TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SEND_AS_ZIP)) );
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetString((int)DontSendByMail,         TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_DONT_SEND)) );

  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_EMAILS_SEND) ));

  UpdateMessageLabel();

  Layout();
  SetAutoLayout( TRUE );
  SetSize(600, 400);
}

/*----------------------------------------------------------------------
  Called when a grid cell content was changed.
  ----------------------------------------------------------------------*/
void SendByMailDlgWX::OnGridCellChange(wxGridEvent& event)
{
  wxGrid* grid = (wxGrid*)event.GetEventObject();
  if(event.GetRow()==grid->GetNumberRows()-1)
    grid->AppendRows();
  event.Skip();
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

void SendByMailDlgWX::OnUpdateSendButton(wxUpdateUIEvent& event)
{
  if(m_grid && m_grid->GetNumberRows()>0)
    {
      int n;
      for(n=0; n<m_grid->GetNumberRows(); n++)
        {
          if(!(m_grid->GetCellValue(n, 0).Trim().IsEmpty()))
            {
              event.Enable(true);
              return;
            }
        }
    }
  event.Enable(false);
}

void SendByMailDlgWX::OnChangeMessageClass(wxCommandEvent& WXUNUSED(event))
{
  UpdateMessageLabel();
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
  }
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
  return XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->GetSelection()==SendByMailAsAttachment;
}

bool SendByMailDlgWX::SendAsContent()const
{
  return XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->GetSelection()==SendByMailAsMessage;
}

bool SendByMailDlgWX::SendAsZip()const
{
  return XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->GetSelection()==SendByMailAsZip;
}

void SendByMailDlgWX::SetSendMode(int mode)
{
  XRCCTRL(*this, "wxID_RADIOBOX_SEND_CLASS",   wxRadioBox)->SetSelection(mode);
}


wxArrayString  SendByMailDlgWX::GetRecipients()const
{
  wxArrayString arr;
  int n;
  for(n=0; n<m_grid->GetNumberRows(); n++)
    {
      wxString str = m_grid->GetCellValue(n, 0).Trim(); 
      if(!str.IsEmpty())
          arr.Add(str);
    }
  return arr;
}

void SendByMailDlgWX::SetRecipients(const wxArrayString & rcpt)
{
  m_grid->BeginBatch();
  
  unsigned int n;
  for(n=0; n<rcpt.GetCount(); n++)
    {
      wxString str = rcpt[n].Trim();
      if(!str.IsEmpty())
        {
          m_grid->AppendRows();
          m_grid->SetCellValue(m_grid->GetNumberRows()-1, 0, str);
        }
    }
  m_grid->EndBatch();
}

wxString SendByMailDlgWX::GetRecipientList()const
{
  wxString res;
  int n;
  for(n=0; n<m_grid->GetNumberRows(); n++)
    {
      wxString str = m_grid->GetCellValue(n, 0).Trim(); 
      if(!str.IsEmpty())
        res << wxT("|") << str;
    }
  res.Remove(0,1);
  return res;
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
    m_rcptArray = ::wxStringTokenize(wxString(rcpts, wxConvUTF8), wxT("|"));
  }
  if(m_rcptArray.GetCount()==0)
    m_rcptArray.Add(wxString(TtaGetEnvString ("EMAILS_FROM_ADDRESS"), wxConvUTF8));
}

void SendByMailDlgWX::OnCloseDialog(wxCommandEvent& event)
{
  if(event.GetId()==wxID_OK)
    {
      int n;
      for(n=0; n<m_grid->GetNumberRows(); n++)
        {
          wxString str = m_grid->GetCellValue(n, 0).Trim(); 
          if(!str.IsEmpty())
            AddAddressToRecentList(str);
        }      
    }
  SaveRecentList();
  event.Skip();
}


void SendByMailDlgWX::OnSize(wxSizeEvent& event)
{
  if(m_grid)
    m_grid->SetColSize(0, m_grid->GetSize().x-20);
  event.Skip();
}

#endif /* _WX */
