
#include "email.h"
#include "SMTP.h"

void TestSendMail()
{
    wxIPV4address addr;
    addr.Hostname(wxT("smtp-serv.inrialpes.fr"));
    addr.Service(25);
    
    wxSMTP smtp;
    smtp.Connect(addr);

    wxEmailMessage message(wxT("Test of wxSMTP."), wxT("This is a test for sending mails."), wxT("emilien.kia@inrialpes.fr"));
    message.AddTo(wxT("emilien.kia@gmail.com"));
    message.AddTo(wxT("emilien.kia@inrialpes.fr"));
  
    message.AddAlternative(wxT("<html><body><h1>Bonjour</h1><p>Plop !</p></body></html>"), wxT("text/html"));
    message.AddFile(wxFileName(wxT("/home/kia/divers/cursor/cursor-16.png")), wxT("image/png"));

  
//    printf("%s\n", message.GetMessageContent().GetData());    
    smtp.SendMail(message);
    
    smtp.Close();
}
