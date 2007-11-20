#include "SMTP.h"

#ifdef _WX
#include <wx/wfstream.h>
#include <wx/sstream.h>
#endif /* _WX */

#include "thot_sys.h"
#include "email.h"
 
EMail TtaNewEMail(const char* subject, const char* text, const char* from)
{
#ifdef _WX
  return (EMail) new wxEmailMessage(wxString(subject, wxConvUTF8),
                                    wxString(text, wxConvUTF8),
                                    wxString(from, wxConvUTF8));
#else /* _WX */
  return NULL;
#endif /* _WX */
}

void  TtaFreeMail(EMail mail)
{
#ifdef _WX  
  delete (wxEmailMessage*)mail;
#endif /* _WX */
}

void TtaSetMailer(EMail mail, const char* mailer)
{
#ifdef _WX
  ((wxEmailMessage*)mail)->AddExtraHeader(wxT("X-Mailer"), wxString(mailer, wxConvUTF8));
#endif /* _WX */
}


void TtaAddEMailToRecipient(EMail mail, const char* address)
{
#ifdef _WX  
  ((wxEmailMessage*)mail)->AddTo(wxString(address, wxConvUTF8));
#endif /* _WX */
}

void TtaAddEMailCopyRecipient(EMail mail, const char* address)
{
#ifdef _WX  
  ((wxEmailMessage*)mail)->AddCc(wxString(address, wxConvUTF8));
#endif /* _WX */
}

void TtaAddEMailAlternative(EMail mail, const char* mimeType, const char* content, const char* charset)
{
#ifdef _WX  
  wxNode* node = ((wxEmailMessage*)mail)->AddAlternative(wxString(content, wxConvUTF8), wxString(mimeType, wxConvUTF8));
  if(node && charset && charset[0]!=0)
  {
    ((wxMimeSlot*)node->GetData())->SetContentTypeExtraParam(wxT("charset"), wxString(charset, wxConvUTF8));
  }
#endif /* _WX */
}

ThotBool TtaAddEMailAlternativeFile(EMail mail, const char* mimeType, const char* filename, const char* charset)
{
#ifdef _WX
  wxNode* node = ((wxEmailMessage*)mail)->AddAlternativeFile(wxFileName(wxString(filename, wxConvUTF8)), wxString(mimeType, wxConvUTF8));
  if(node && charset && charset[0]!=0)
  {
    ((wxMimeSlot*)node->GetData())->SetContentTypeExtraParam(wxT("charset"), wxString(charset, wxConvUTF8));
  }
  return node!=NULL;
#endif /* _WX */    
}

ThotBool TtaAddEMailAttachmentFile(EMail mail, const char* mimeType, const char* filename)
{
#ifdef _WX  
  return (((wxEmailMessage*)mail)->AddFile(wxString(filename, wxConvUTF8), wxString(mimeType, wxConvUTF8), false))!=NULL;
#endif /* _WX */  
}

ThotBool TtaAddEMailAttachmentFileAlternativeName(EMail mail, const char* mimeType, const char* filename, const char* name)
{
#ifdef _WX  
  return (((wxEmailMessage*)mail)->AddFile(wxString(filename, wxConvUTF8), wxString(mimeType, wxConvUTF8), false, wxString(name, wxConvUTF8)))!=NULL;
#endif /* _WX */  
}


ThotBool TtaSendEMail(EMail mail, const char* serverAddress, int port, int* error)
{
  ThotBool rep = FALSE;
#ifdef _WX
  long step;
  long err;
  wxSMTP server;
  wxIPV4address addr;
  addr.Hostname(wxString(serverAddress, wxConvUTF8));
  addr.Service(port);
  
//  server.SetTimeout(10);
  server.Connect(addr);
  if(server.SendMail(*(wxEmailMessage*)mail))
    rep = TRUE;
  
  server.Close();
  
  if(rep==FALSE && error!=NULL)
  {
    err = server.GetLastError(&step);
//    printf("Error : %d / %d\n", err, step);
    switch(step)
    {
      case wxSMTP_STEP_CONNECT:
        *error = EMAIL_SERVER_NOT_RESPOND;
        break;
      case wxSMTP_STEP_HELLO:
        *error = EMAIL_SERVER_REJECT;
        break;
      case wxSMTP_STEP_FROM:
        *error = EMAIL_FROM_BAD_ADDRESS;
        break;
      case wxSMTP_STEP_RECIPIENT:
        *error = EMAIL_TO_BAD_ADDRESS;
        break;
      case wxSMTP_STEP_DATA:
      case wxSMTP_STEP_CONTENT:
        *error = EMAIL_BAD_CONTENT;
        break;
      default:
        *error = EMAIL_OK;
        break;
    }
  }
  
#endif /* _WX */ 
  return rep; 
}
