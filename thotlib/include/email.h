#ifndef EMAIL_H_
#define EMAIL_H_

typedef void* EMail;

EMail TtaNewEMail(const char* subject, const char* text, const char* from);
void  TtaFreeMail(EMail mail);

void TtaAddEMailToRecipient(EMail mail, const char* address);
void TtaAddEMailCopyRecipient(EMail mail, const char* address);

void TtaAddEMailAlternative(EMail mail, const char* mimeType, const char* content, const char* charset);

ThotBool TtaAddEMailAlternativeFile(EMail mail, const char* mimeType, const char* filename, const char* charset);
ThotBool TtaAddEMailAttachmentFile(EMail mail, const char* mimeType, const char* filename);


ThotBool TtaSendEMail(EMail mail, const char* serverAddress, int port);


#endif /*EMAIL_H_*/
