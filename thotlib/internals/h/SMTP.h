#ifdef _WX

#ifndef SMTP_H_
#define SMTP_H_


#include "wx/filename.h"
#include "wx/socket.h"
#include "wx/hashmap.h"
#include "wx/file.h"
#include "wx/filename.h"
#include "wx/stream.h"


/**
 * Quoted-printable encoder output stream
 */
class wxQuotedPrintableOutputStream : public wxFilterOutputStream{
protected:
    char m_buffer[76+2+1];
    int  m_pos;
  
    virtual size_t OnSysWrite(const void *buffer, size_t size);
    
    void FlushBuffer();
    
public:
    wxQuotedPrintableOutputStream(wxOutputStream& stream);
    virtual ~wxQuotedPrintableOutputStream(){Close();}
    bool Close();
};

/**
 * Debug output stream
 */
class wxDebugOutputStream : public wxFilterOutputStream{
protected:
    virtual size_t OnSysWrite(const void *buffer, size_t size)
    {
      printf("(%05d) >> %s\n",  (int)size, (const char*)buffer);
      GetFilterOutputStream()->Write(buffer, size);
      return size;
    }
public:
    wxDebugOutputStream(wxOutputStream& stream):wxFilterOutputStream(stream){}
    virtual ~wxDebugOutputStream(){Close();}
    bool Close(){return wxFilterOutputStream::Close();}
};



class wxMultipartMimeContainer;

/**
 * MIME content trenasfert enconding.
 */
enum wxMimeContentTransfertEncoding
{
    wxMIME_CONTENT_TRANSFERT_ENCONDING_AUTO         = -1,
    wxMIME_CONTENT_TRANSFERT_ENCONDING_7BITS        = 0,
    wxMIME_CONTENT_TRANSFERT_ENCONDING_8BITS,
    wxMIME_CONTENT_TRANSFERT_ENCONDING_QUOTED_PRINTABLE,
    wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64,
    wxMIME_CONTENT_TRANSFERT_ENCONDING_BINARY,
    wxMIME_CONTENT_TRANSFERT_ENCONDING_CUSTOM
};

#define wxMIMETYPE_MULTIPART_MIXED wxT("multipart/mixed")
#define wxMIMETYPE_MULTIPART_DIGEST wxT("multipart/digest")
#define wxMIMETYPE_MULTIPART_ALTERNATIVE wxT("multipart/alternative")
#define wxMIMETYPE_MULTIPART_RELATED wxT("multipart/related")
#define wxMIMETYPE_MULTIPART_REPORT wxT("multipart/report")
#define wxMIMETYPE_MULTIPART_SIGNED wxT("multipart/signed")
#define wxMIMETYPE_MULTIPART_ENCRYPTED wxT("multipart/encrypted")
#define wxMIMETYPE_MULTIPART_FORM_DATA wxT("multipart/form-data")


WX_DECLARE_STRING_HASH_MAP( wxString, wxMimeExtraParamMap );

typedef struct{
    size_t size;
    void* data;
} wxMimeSlot_BinaryContent;

typedef struct{
    wxFileName filename;
    wxString   sendpath;
} wxMimeSlot_FileContent;


/**
 * Byte counter stream.
 * \see wx 
 */
class wxByteCounterOutputStream : public wxOutputStream
{
public:
  wxByteCounterOutputStream();

    bool Ok() const { return IsOk(); }
    bool IsOk() const { return true; }

    unsigned long GetCount(unsigned char c)const{return m_table[c];}
    unsigned long GetCount()const{return m_count;}
    
protected:
    virtual size_t OnSysWrite(const void *buffer, size_t size);
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode);
    virtual wxFileOffset OnSysTell() const;

    unsigned long m_table[256];
    unsigned long m_count;

    DECLARE_NO_COPY_CLASS(wxByteCounterOutputStream)
};


/**
 * MIME slot.
 */
class wxMimeSlot : public wxObject
{
    DECLARE_CLASS(wxMimeSlot)
protected:
    wxString  m_contentType;
    wxMimeExtraParamMap m_contentTypeExtraParams;
    wxMimeContentTransfertEncoding m_transfertEncoding;
    wxMimeExtraParamMap m_extraParams;
    enum wxMimeSlotContent{
        wxMimeSlotContentText,
        wxMimeSlotContentBinary,
        wxMimeSlotContentFile,
        wxMimeSlotContentMime,
    }m_dataType;
//    union
//    {
        wxString m_textContent;
		wxMimeSlot_BinaryContent m_binaryContent;
		wxMimeSlot_FileContent   m_fileContent;
        wxMultipartMimeContainer *m_mimeContent;
//    };
public:
    wxMimeSlot();
    wxMimeSlot(const wxMimeSlot& slot);
    wxMimeSlot(const wxString& contentType, const wxString& data);
    wxMimeSlot(const wxString& contentType, size_t size, void* data);
    wxMimeSlot(const wxFileName& filename, const wxString& sendPath = wxT(""), const wxString& contentType=wxT(""));
    wxMimeSlot(wxMultipartMimeContainer* mime);
    virtual ~wxMimeSlot();
    
    virtual bool Write(wxOutputStream& out, bool bInline = false)const;
    virtual wxMimeContentTransfertEncoding GetAutoContentTransfertEncoding()const;

    void SetContentType(const wxString& contentType){m_contentType = contentType;}
    void SetContentTypeExtraParam(const wxString& name, const wxString& value){m_contentTypeExtraParams[name] = value;}
    void SetTransfertEncondingMethod(wxMimeContentTransfertEncoding transfertEncoding){m_transfertEncoding=transfertEncoding;}
    void SetExtraParam(const wxString& name, const wxString& value){m_extraParams[name] = value;}

    wxString GetContentType()const{return m_contentType;}
    wxString GetContentTypeExtraParam(const wxString& name){return m_contentTypeExtraParams[name];}
    wxMimeContentTransfertEncoding GetTransfertEncondingMethod()const{return m_transfertEncoding;}
    wxString GetExtraParam(const wxString& name){return m_extraParams[name];}

    void SetContent(const wxString& text){m_dataType=wxMimeSlotContentText;m_textContent=text;}
    void SetContent(size_t size, void* data){m_dataType=wxMimeSlotContentBinary;m_binaryContent.size=size;m_binaryContent.data=data;}
    void SetContent(wxMultipartMimeContainer* mime){m_dataType=wxMimeSlotContentMime;m_mimeContent=mime;}
    
    wxString GetTextContent()const{return m_textContent;}
    void     GetBinaryContent(size_t* size, void** data)const{*size=m_binaryContent.size;*data=m_binaryContent.data;}
    wxMultipartMimeContainer* GetMimeContent()const{return m_mimeContent;}    
    wxFileName GetFilename()const{return m_fileContent.filename;}
    wxString GetSendpath()const{return m_fileContent.sendpath;}
    
    bool IsTextContent()const{return m_dataType==wxMimeSlotContentText;}
    bool IsBinaryContent()const{return m_dataType==wxMimeSlotContentBinary;}
    bool IsFileContent()const{return m_dataType==wxMimeSlotContentFile;}
    bool IsMimeContent()const{return m_dataType==wxMimeSlotContentMime;}
};

/**
 * Multipart MIME container.
 */
class wxMultipartMimeContainer : public wxList
{
protected:
    wxString m_contentType;
    wxString m_boundary;
    wxString m_message;
    wxMimeExtraParamMap m_contentTypeExtraParams;
public:
    wxMultipartMimeContainer(const wxString& contentType=wxMIMETYPE_MULTIPART_MIXED, const wxString& boundary=wxT(""), const wxString message=wxT(""));
    virtual ~wxMultipartMimeContainer();

    virtual wxString GenerateBoundary()const;
    virtual bool Write(wxOutputStream& out)const;

    wxString GetMessage()const{return m_message;}
  wxString GetContentType()const{return m_contentType;}
  wxString GetBoundary()const{return m_boundary;}
  wxString GetContentTypeExtraParam(const wxString& name){return m_contentTypeExtraParams[name];}
  
    void SetMessage(const wxString& msg){m_message = msg;}
  void SetContentType(const wxString& contentType){m_contentType=contentType;}
  void SetBoundary(const wxString& boundary){m_boundary=boundary;}
  void SetContentTypeExtraParam(const wxString& name, const wxString& value){m_contentTypeExtraParams[name]=value;}

    wxNode* Append(const wxMimeSlot& slot);
    wxNode* Append(const wxString& contentType, const wxString& data);
    wxNode* Append(const wxString& contentType, size_t size, void* data);
    wxNode* Append(const wxFileName& filename, const wxString& sendPath = wxT(""), const wxString& contentType=wxT(""));
    wxNode* Append(wxMultipartMimeContainer* mime);

    wxNode* Prepend(const wxMimeSlot& slot);
    wxNode* Prepend(const wxString& contentType, const wxString& data);
    wxNode* Prepend(const wxString& contentType, size_t size, void* data);
    wxNode* Prepend(const wxFileName& filename, const wxString& sendPath = wxT(""), const wxString& contentType=wxT(""));
    wxNode* Prepend(wxMultipartMimeContainer* mime);

    wxNode* Insert(size_t pos, const wxMimeSlot& slot);
    wxNode* Insert(size_t pos, const wxString& contentType, const wxString& data);
    wxNode* Insert(size_t pos, const wxString& contentType, size_t size, void* data);
    wxNode* Insert(size_t pos, const wxFileName& filename, const wxString& sendPath = wxT(""), const wxString& contentType=wxT(""));
    wxNode* Insert(size_t pos, wxMultipartMimeContainer* mime);
};


/** Base command line protocol.
 * Base class for all protocols based on textual request/response couples.
 */
class wxCmdLineProtocol : public wxSocketClient
{
protected:
    wxString m_buffer;
public:
    wxCmdLineProtocol(wxSocketFlags flags = wxSOCKET_NONE):wxSocketClient(flags){}
    virtual ~wxCmdLineProtocol(){}
    
    /**
     * Send a request and wait for an error.
     * \param request Request text (dont add CRLF, added automatically).
     * \param haveError Address of an error flag, set to true if an error has occurs.
     * \return The returned response message.
     */
    virtual wxString SendCommand(wxString request, bool* haveError=NULL);
    
    /**
     * Wait for a response line without sending any request.
     * \param haveError Address of an error flag, set to true if an error has occurs.
     * \return The returned response message.
     */
    virtual wxString ReadLine(bool* haveError=NULL);
    
    wxString GetCurrentBuffer()const{return m_buffer;}
    void ClearBuffer(){m_buffer.Empty();}
};

/**
 * Mail container.
 */
class wxEmailMessage
{
protected:
    wxString m_subject, m_text, m_from;
    wxArrayString m_rcptArray;  // envelope to; (complete)
    wxArrayString m_toArray;    // to: header
    wxArrayString m_ccArray;    // cc: header
    wxArrayString m_bccArray;   // bcc: header?
    wxArrayString m_extraHeaders;
    wxMultipartMimeContainer m_attachements;
    wxMultipartMimeContainer m_alternatives;

    wxFile    m_tempFile; /* Temporary stored mail content. */
    
public:
    wxEmailMessage(const wxString& subject, const wxString& text, const wxString& from);
    virtual ~wxEmailMessage(){}

    /**
     * Gets the from string, which was specified in the constructor.
     * \return The from string.
     */
    const wxString& GetFrom()const{return m_from;}

    /**
     * Adds a file attachment. If no mime-type is specified,
     * the type will be determined by the file extension.
     * \param fileName Filename of the new file attachment.
     * \param mimeType MIME main type, like 'image/jpeg'.
     * \param bInline True if the file is attached inline or false if attachment
     * \param name Alternative name used in the mail. 
     * \return The wxMimeSlot node. 
     */
    wxNode* AddFile(const wxFileName& fileName, wxString mimeType = wxT(""), bool bInline=false, const wxString& name=wxT(""));
    
    wxNode* AddAlternative(const wxString data, const wxString& mimeType = wxT("text/plain"));
    wxNode* AddAlternativeFile(const wxFileName& fileName, wxString mimeType = wxT("text/plain"));

    /**
     * Adds an additional recipient. You can add as many recipients
     * as you want and don't have to specify it in the email message,
     * for example if you want to send a mailing list. The RFC 821
     * (SMTP) garantuees 100 recipients at once, but most 
     * \param address Email address of the additional recipient.
     */
    void AddRecipient(const wxString& address);
    /**
     * Adds an additional recipient in the to-field.
     * \param address The email address of the recipient.
     */
    void AddTo(const wxString& address);

    /**
     * Adds an additional recipient in the cc-field.
     * \param address The email address of the recipient.
     */
    void AddCc(const wxString& address);

    /**
     * Adds an additional recipient in the bcc-field.
     * \param address The email address of the recipient.
     */
    void AddBcc(const wxString& address);
    
    /**
     * Get the number of recipient.
     */
    int GetRecipientCount()const{return m_rcptArray.GetCount();}
    /**
     * Get a recipient.
     */
    wxString GetRecipient(int index)const{return m_rcptArray[index];}
    
    /**
     * Add an email header.
     */
    void AddExtraHeader(const wxString& name, const wxString& value);

    /**
     * Write the message to a stream.
     */
    virtual bool Write(wxOutputStream& out);
    
    /**
     * QEncode a text.
     */ 
    static wxString QEncode(const wxString& str);
};

enum wxSMTP_STEP
{
  wxSMTP_STEP_UNKNOW,
  wxSMTP_STEP_CONNECT,
  wxSMTP_STEP_HELLO,
  wxSMTP_STEP_FROM,
  wxSMTP_STEP_RECIPIENT,
  wxSMTP_STEP_DATA,
  wxSMTP_STEP_CONTENT,
  wxSMTP_STEP_QUIT,
  wxSMTP_STEP_DONE
};

WX_DECLARE_STRING_HASH_MAP( wxString, wxSmtpParamMap );


/** SMTP sender. */
class wxSMTP : public wxCmdLineProtocol
{
public:
    wxSMTP();
    virtual ~wxSMTP(){}

    void UseAuthentification(const wxString& name, const wxString& passwd);
    
    bool Connect(wxSockAddress& address, bool wait = true);

    void SetMailer(const wxString& mailer);

    virtual bool Close();

    bool SendMail(wxEmailMessage& message);
    
    long GetLastError(long* step=NULL);
protected:
    virtual bool SendHello();
    virtual bool SendQuit();
    virtual bool SendFrom(const wxString& addr);
    virtual bool SendTo(const wxString& addr);
    virtual bool SendData();
    
    virtual bool SendContent(wxEmailMessage& message);
    
    virtual bool SendAuthentication();
    
    static int GetResponseCode(const wxString& rep);
    virtual wxString SendCommand(const wxString& request, bool* haveError=NULL);
    
    long m_error;
    long m_errorStep;
private:
    wxSmtpParamMap m_params;
    
    bool m_useAuth;
    wxString m_user, m_passwd;
};


#endif /*SMTP_H_*/
#endif /*_WX*/
