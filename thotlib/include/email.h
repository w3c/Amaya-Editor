#ifndef EMAIL_H_
#define EMAIL_H_

typedef void* EMail;

EMail TtaNewEMail(const char* subject, const char* text, const char* from);
void  TtaFreeMail(EMail mail);

void TtaSetMailer(EMail mail, const char* mailer);

void TtaAddEMailToRecipient(EMail mail, const char* address);
void TtaAddEMailCopyRecipient(EMail mail, const char* address);

void TtaAddEMailAlternative(EMail mail, const char* mimeType, const char* content, const char* charset);

ThotBool TtaAddEMailAlternativeFile(EMail mail, const char* mimeType, const char* filename, const char* charset);
ThotBool TtaAddEMailAttachmentFile(EMail mail, const char* mimeType, const char* filename);
ThotBool TtaAddEMailAttachmentFileAlternativeName(EMail mail, const char* mimeType, const char* filename, const char* name);


typedef enum _EmailErrorCode
{
  EMAIL_OK = 0,
  EMAIL_SERVER_NOT_RESPOND,
  EMAIL_SERVER_REJECT,
  EMAIL_FROM_BAD_ADDRESS,
  EMAIL_TO_BAD_ADDRESS,
  EMAIL_BAD_CONTENT,
  EMAIL_UNKNOW_ERROR
}EmailErrorCode;

ThotBool TtaSendEMail(EMail mail, const char* serverAddress, int port, int* error);


#endif /*EMAIL_H_*/
