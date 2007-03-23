#ifdef _WX

#include "wx/wx.h"

#include "SMTP.h"
#include "base64.h"



#include <wx/object.h>
#include <wx/sckstrm.h>
#include <wx/tokenzr.h>
#include <wx/mimetype.h>
#include <wx/file.h>
#include <iostream>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/timer.h>

//==========================================================================
// wxQuotedPrintableOutputStream
//==========================================================================

wxQuotedPrintableOutputStream::wxQuotedPrintableOutputStream(wxOutputStream& stream):
wxFilterOutputStream(stream),
m_offset(0)
{
}

bool wxQuotedPrintableOutputStream::Close()
{
    return wxFilterOutputStream::Close();
}

size_t wxQuotedPrintableOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    wxString out;
  
    wxStringTokenizer tkz(wxString((const char*)buffer, *wxConvCurrent, size), wxT("\n"));
    while(tkz.HasMoreTokens())
    {
        wxString line = tkz.GetNextToken();
        wxString newpara;
        // Remove '\r' ending character.
        if(line.Last()==wxT('\r'))
            line.RemoveLast();
        
        // Process each character of the line.
        for(int j=0; j<(int)line.Len(); j++)
        {
            wxChar c = line[j];
            wxString newc;
            // Convert char if needed
            if(c<32 || c==61 || c>126)
            {
                newc.Printf(wxT("=%02X"), c);
            }
            else
                newc = c;

            // Truncate if needed
            if(m_offset+newc.Length()>76)
            {
                out << newpara << wxT("=\r\n");
                newpara.Empty();
                m_offset = 0;
            }
            newpara += newc;
            m_offset += newc.Length();
        }
        m_offset = newpara.Length();
        if(!line.IsEmpty())
        {
            out << newpara << wxT("\r\n");
        }
    }
    GetFilterOutputStream()->Write((const char*) out.mb_str(wxConvLibc), out.Length()); 
    return size;
}

//==========================================================================
// wxMimeSlot
//==========================================================================

IMPLEMENT_CLASS(wxMimeSlot, wxObject)

wxMimeSlot::wxMimeSlot():
wxObject()
{
}

wxMimeSlot::wxMimeSlot(const wxMimeSlot& slot):
wxObject(),
m_contentType(slot.m_contentType),
m_contentTypeExtraParams(slot.m_contentTypeExtraParams),
m_transfertEncoding(slot.m_transfertEncoding),
m_extraParams(slot.m_extraParams),
m_dataType(slot.m_dataType)
{
    switch(m_dataType)
    {
        case wxMimeSlotContentText:
            m_textContent = slot.m_textContent;
            break;
        case wxMimeSlotContentBinary:
            m_binaryContent.size = slot.m_binaryContent.size;
            m_binaryContent.data = slot.m_binaryContent.data;
            break;
        case wxMimeSlotContentMime:
            m_mimeContent = slot.m_mimeContent;
            break;
        case wxMimeSlotContentFile:
            m_fileContent = slot.m_fileContent;
            break;
    }
}

wxMimeSlot::wxMimeSlot(const wxString& contentType, const wxString& data):
wxObject(),
m_contentType(contentType),
m_transfertEncoding(wxMIME_CONTENT_TRANSFERT_ENCONDING_QUOTED_PRINTABLE),
m_dataType(wxMimeSlotContentText),
m_textContent(data)
{
}

wxMimeSlot::wxMimeSlot(const wxString& contentType, size_t size, void* data):
wxObject(),
m_contentType(contentType),
m_transfertEncoding(wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64),
m_dataType(wxMimeSlotContentBinary)
{
    m_binaryContent.size = size;
    m_binaryContent.data = data;
}

wxMimeSlot::wxMimeSlot(const wxFileName& filename, const wxString& sendPath, const wxString& contentType):
wxObject(),
m_contentType(contentType),
m_transfertEncoding(wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64),
m_dataType(wxMimeSlotContentFile)
{
    m_fileContent.filename = filename;
    if(!sendPath.IsEmpty())
        m_fileContent.sendpath = sendPath;
    else
        m_fileContent.sendpath = filename.GetFullName();
}



wxMimeSlot::wxMimeSlot(wxMultipartMimeContainer* mime):
wxObject(),
m_contentType(mime->GetContentType()),
m_transfertEncoding(wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64),
m_dataType(wxMimeSlotContentMime),
m_mimeContent(mime)
{
}
    
wxMimeSlot::~wxMimeSlot()
{
}

wxString wxMimeSlot::Generate()const
{
    if(m_dataType==wxMimeSlotContentMime)
        return m_mimeContent->Generate();
    
  wxString msg;
  msg << wxT("Content-Type: ") << m_contentType;
  
  wxMimeExtraParamMap::const_iterator iter;
  for(iter = m_contentTypeExtraParams.begin();iter!=m_contentTypeExtraParams.end(); iter++)
  {
    msg << wxT(";    ") << iter->first;
    if(!iter->second.IsEmpty())
      msg << wxT("=") << iter->second;
  }
    if(m_dataType==wxMimeSlotContentFile)
    {
        msg << wxT("; name=\"") << m_fileContent.sendpath << wxT("\"");
    }
  msg << wxT("\r\n");
    
    wxMimeContentTransfertEncoding te = m_transfertEncoding; 
    if(te==wxMIME_CONTENT_TRANSFERT_ENCONDING_AUTO)
    {
        te = GetAutoContentTransfertEncoding();
    }

  msg << wxT("Content-Transfer-Encoding: ");
  switch(te)
  {
        case wxMIME_CONTENT_TRANSFERT_ENCONDING_7BITS:
            msg << wxT("7bit");
            break;
        case wxMIME_CONTENT_TRANSFERT_ENCONDING_8BITS:
            msg << wxT("8bit");
            break;
        case wxMIME_CONTENT_TRANSFERT_ENCONDING_QUOTED_PRINTABLE:
            msg << wxT("quoted-printable");
            break;
        case wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64:
            msg << wxT("base64");
            break;
        default:
            msg << wxT("binary");
            break;
  }
    msg << wxT("\r\n");

  for(iter = m_extraParams.begin();iter!=m_extraParams.end(); iter++)
  {
    msg << iter->first;
    if(!iter->second.IsEmpty())
      msg << wxT(": ") << iter->second;
        msg << wxT("\r\n");
  }

    // Empty line head/content separator.
    msg << wxT("\r\n");
    
    switch(m_dataType)
    {
        case wxMimeSlotContentMime: // Already returned.
            break;
        case wxMimeSlotContentText:
            switch(te)
            {
                case wxMIME_CONTENT_TRANSFERT_ENCONDING_QUOTED_PRINTABLE:
                {
                    wxStringOutputStream out;
                    wxQuotedPrintableOutputStream stm(out);
                    wxStringInputStream  in(m_textContent);
                    stm.Write(in);
                    stm.Close();
                    msg << out.GetString();
                    break;
                }
                case wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64:
                {
                    wxStringOutputStream out;
                    wxBase64EncOutputStream stm(out);
                    wxStringInputStream  in(m_textContent);
                    stm.Write(in);
                    stm.Close();
                    msg << out.GetString();
                    break;
                }
                default:
                    msg << m_textContent;
                    break;
            }
            break;
        case wxMimeSlotContentFile: // File
            switch(te)
            {
                case wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64:
                {
                    wxStringOutputStream out;
                    wxBase64EncOutputStream stm(out);
                    wxFileInputStream file(m_fileContent.filename.GetFullPath());

                    stm.Write(file);
                    stm.Close();
                    
                    msg << out.GetString();
                    break;
                }
                default:
                    /** \TODO */
                    break;
            }
            break;
            
        default: // Binary
            switch(te)
            {
                case wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64:
                {
                    wxStringOutputStream out;
                    wxBase64EncOutputStream stm(out);
                    stm.Write(m_binaryContent.data, m_binaryContent.size);
                    stm.Close();
                    msg << out.GetString();
                    break;
                }
                default:
                    /** \TODO */
                    break;
            }
            break;
    }
    
  return msg;
}

wxMimeContentTransfertEncoding wxMimeSlot::GetAutoContentTransfertEncoding()const
{
    return wxMIME_CONTENT_TRANSFERT_ENCONDING_QUOTED_PRINTABLE;
}

//==========================================================================
// wxMultipartMimeContainer
//==========================================================================
wxMultipartMimeContainer::wxMultipartMimeContainer(const wxString& contentType, const wxString& boundary, const wxString message):
wxList(),
m_contentType(contentType),
m_boundary(boundary),
m_message(message)
{
}

wxMultipartMimeContainer::~wxMultipartMimeContainer()
{
}

wxNode* wxMultipartMimeContainer::Append(const wxMimeSlot& slot)
{
  return wxList::Append(new wxMimeSlot(slot));
}

wxNode* wxMultipartMimeContainer::Append(const wxString& contentType, const wxString& data)
{
  return wxList::Append(new wxMimeSlot(contentType, data));
}

wxNode* wxMultipartMimeContainer::Append(const wxString& contentType, size_t size, void* data)
{
  return wxList::Append(new wxMimeSlot(contentType, size, data));
}

wxNode* wxMultipartMimeContainer::Append(const wxFileName& filename, const wxString& sendPath, const wxString& contentType)
{
    return wxList::Append(new wxMimeSlot(filename, sendPath, contentType));
}

wxNode* wxMultipartMimeContainer::Append(wxMultipartMimeContainer* mime)
{
  return wxList::Append(new wxMimeSlot(mime));
}

wxNode* wxMultipartMimeContainer::Prepend(const wxMimeSlot& slot)
{
  return wxList::Insert(new wxMimeSlot(slot));
}

wxNode* wxMultipartMimeContainer::Prepend(const wxString& contentType, const wxString& data)
{
  return wxList::Insert(new wxMimeSlot(contentType, data));
}

wxNode* wxMultipartMimeContainer::Prepend(const wxString& contentType, size_t size, void* data)
{
  return wxList::Insert(new wxMimeSlot(contentType, size, data));
}

wxNode* wxMultipartMimeContainer::Prepend(const wxFileName& filename, const wxString& sendPath, const wxString& contentType)
{
    return wxList::Insert(new wxMimeSlot(filename, sendPath, contentType));
}

wxNode* wxMultipartMimeContainer::Prepend(wxMultipartMimeContainer* mime)
{
  return wxList::Insert(new wxMimeSlot(mime));
}

wxNode* wxMultipartMimeContainer::Insert(size_t pos, const wxMimeSlot& slot)
{
    return wxList::Insert(pos, new wxMimeSlot(slot));
}

wxNode* wxMultipartMimeContainer::Insert(size_t pos, const wxString& contentType, const wxString& data)
{
    return wxList::Insert(pos, new wxMimeSlot(contentType, data));
}

wxNode* wxMultipartMimeContainer::Insert(size_t pos, const wxString& contentType, size_t size, void* data)
{
    return wxList::Insert(pos, new wxMimeSlot(contentType, size, data));
}

wxNode* wxMultipartMimeContainer::Insert(size_t pos, const wxFileName& filename, const wxString& sendPath, const wxString& contentType)
{
    return wxList::Insert(pos, new wxMimeSlot(filename, sendPath, contentType));
}

wxNode* wxMultipartMimeContainer::Insert(size_t pos, wxMultipartMimeContainer* mime)
{
    return wxList::Insert(pos, new wxMimeSlot(mime));
}

wxString wxMultipartMimeContainer::GenerateBoundary()const
{
    static int nb = 0;
    wxLongLong ll = wxGetLocalTimeMillis() + nb++;
    return wxT("part") + ll.ToString();
}


wxString wxMultipartMimeContainer::Generate()const
{
    wxString msg;
    msg << wxT("Content-type: ") << m_contentType;
    
    wxString boundary = m_boundary;
    if(boundary.IsEmpty())
        boundary = GenerateBoundary();
    
    msg << wxT(";\r\n    boundary=\"") << boundary << wxT("\"\r\n"); 
    
    for(wxMimeExtraParamMap::const_iterator iter = m_contentTypeExtraParams.begin();iter!=m_contentTypeExtraParams.end(); iter++)
    {
        msg << wxT("; ") << iter->first;
        if(!iter->second.IsEmpty())
            msg << wxT("=") << iter->second;
        msg << wxT("\r\n");
    }

    msg << wxT("\r\n") << m_message << wxT("\r\n");
    
    // Empty line head/content separator.
    for(wxNode* node=GetFirst(); node; node=node->GetNext())
    {
        msg << wxT("--") << boundary << wxT("\r\n");
        wxMimeSlot* slot = (wxMimeSlot*) node->GetData();
        msg << slot->Generate();
        msg << wxT("\r\n");
    }
        msg << wxT("--") << boundary << wxT("--\r\n");
    
    return msg;
}



//==========================================================================
// wxCmdLineProtocol
//==========================================================================

wxString wxCmdLineProtocol::SendCommand(const wxString& request, bool* haveError)
{
    if(haveError!=NULL)
        *haveError = false;

//printf(">> %s\n", (const char*)(request.mb_str(wxConvLibc)));

    /* Send request. */
    wxSocketClient::Write(request.mb_str(wxConvLibc), request.Length());
    if(Error())
    {
        if(haveError!=NULL)
            *haveError = true;
        return wxT("");
    }
    wxSocketClient::Write("\r\n", 2);
    if(Error())
    {
        if(haveError!=NULL)
            *haveError = true;
        return wxT("");
    }

    /* Wait for a response. */    
    const int buffsize = 256; 
    char buff[buffsize];
    int search = 0;

    while(true)
    {
        wxSocketClient::Read(buff, buffsize);
        m_buffer += wxString(buff, *wxConvCurrent, LastCount());
        
        if(Error())
        {
            if(haveError!=NULL)
                *haveError = true;
            return wxT("");
        }
        
        for(; search < (int) m_buffer.Length()-1; search++)
        {
            if(m_buffer[search]==wxT('\r'))
            {
                if(m_buffer[search+1]==wxT('\n'))
                {
                    wxString rep = m_buffer.Left(search);
                    m_buffer = m_buffer.Mid(search+2);
                    
//                    printf("<< %s\n", (const char*)(rep.mb_str(wxConvLibc)));
                    
                    return rep;
                }
            }
        }
    }
}

wxString wxCmdLineProtocol::ReadLine(bool* haveError)
{
    /* Wait for a response. */    
    const int buffsize = 256; 
    char buff[buffsize];
    int search = 0;

    while(true)
    {
        wxSocketClient::Read(buff, buffsize);
        m_buffer += wxString(buff, *wxConvCurrent, LastCount());
        
        if(Error())
        {
            if(haveError)
                *haveError = true;
            return wxT("");
        }
        
        for(; search < (int)m_buffer.Length()-1; search++)
        {
            if(m_buffer[search]==wxT('\r'))
            {
                if(m_buffer[search+1]==wxT('\n'))
                {
                    wxString rep = m_buffer.Left(search);
                    m_buffer = m_buffer.Mid(search+2);
                    return rep;
                }
            }
        }
    }
}


//==========================================================================
// wxEmailMessage
//==========================================================================
wxEmailMessage::wxEmailMessage(const wxString& subject, const wxString& text, const wxString& from):
m_subject(subject),
m_text(text),
m_from(from),
m_attachements(wxMIMETYPE_MULTIPART_MIXED),
m_alternatives(wxMIMETYPE_MULTIPART_ALTERNATIVE)
{
}

wxNode* wxEmailMessage::AddFile(const wxFileName& fileName, wxString mimeType, bool bInline)
{
    if(fileName.FileExists())
    {
        if(mimeType.IsEmpty())
        {
            wxFileType* type = wxTheMimeTypesManager->GetFileTypeFromExtension(fileName.GetExt());
            if(type)
            {
                if(!type->GetMimeType(&mimeType))
                    mimeType = wxT("application/octet-stream");
            }
        }
        wxNode *node = m_attachements.Append(fileName, wxT(""), mimeType);
        if(node)
        {
            ((wxMimeSlot*)node->GetData())->SetExtraParam(wxT("Content-Disposition"), bInline?wxT("inline"):wxT("attachment"));
        }
        return node;
    }
    return NULL;
}

wxNode* wxEmailMessage::AddAlternative(const wxString data, const wxString& mimeType)
{
    return m_alternatives.Append(mimeType, data);
}

void wxEmailMessage::AddRecipient(const wxString& address)
{
    m_rcptArray.Add(address);
}

void wxEmailMessage::AddTo(const wxString& address)
{
    m_toArray.Add(address);
    m_rcptArray.Add(address);
}

void wxEmailMessage::AddCc(const wxString& address)
{
    m_ccArray.Add(address);
    m_rcptArray.Add(address);
}

void wxEmailMessage::AddBcc(const wxString& address)
{
    m_bccArray.Add(address);
    m_rcptArray.Add(address);
}

wxString wxEmailMessage::GetMessageContent()
{
    wxString msg;
    msg << wxT("From: ") << m_from << wxT("\r\n");

    if(m_toArray.GetCount() > 0) {
        msg << wxT("To: ");
        for(unsigned int i = 0; i < m_toArray.GetCount() ; i++) {
            if(i > 0) msg << wxT(",") << wxT("\r\n    ");
            msg << m_toArray[i];
        }
        msg << wxT("\r\n");
    }
    if(m_ccArray.GetCount() > 0) {
        msg << wxT("Cc: ");
        for(unsigned int i = 0; i < m_ccArray.GetCount() ; i++) {
            if(i > 0) msg << wxT(",") << wxT("\r\n    ");
            msg << m_ccArray[i];
        }
        msg << wxT("\r\n");
    }
    if(m_bccArray.GetCount() > 0) {
        msg << wxT("Cc: ");
        for(unsigned int i = 0; i < m_bccArray.GetCount() ; i++) {
            if(i > 0) msg << wxT(",") << wxT("\r\n    ");
            msg << m_bccArray[i];
        }
        msg << wxT("\r\n");
    }
    
    if(!m_subject.IsEmpty())
    {
        msg << wxT("Subject: ") << m_subject << wxT("\r\n");
    }

    if(m_attachements.IsEmpty())
    {
        if(m_alternatives.IsEmpty())
        {
            msg << m_text + wxT("\r\n");
        }
        else
        {
            m_alternatives.Prepend(wxT("text/plain"), m_text);
            m_alternatives.SetMessage(wxT("This is a multi-part message in MIME format."));
            msg << wxT("MIME-version: 1.0\r\n") << m_alternatives.Generate();
        }
    }
    else
    {
        m_alternatives.SetMessage(wxT("This is a multi-part message in MIME format."));
        if(m_alternatives.IsEmpty())
        {
            m_attachements.Prepend(wxT("text/plain"), m_text);
        }
        else
        {
            m_alternatives.Prepend(wxT("text/plain"), m_text);
            m_attachements.Prepend(&m_alternatives);
        }
        
        msg << wxT("MIME-version: 1.0\r\n") << m_attachements.Generate();
        
    }

    return msg;
}

//==========================================================================
// wxSMTP
//==========================================================================
wxSMTP::wxSMTP():
wxCmdLineProtocol()
{
}

bool wxSMTP::Connect(wxSockAddress& address, bool wait)
{
    wxCmdLineProtocol::Connect(address, wait);
    ReadLine();
    SendHello();
    return true;
    
    if(!wxCmdLineProtocol::Connect(address, wait))
        return false;
    if(!SendHello())
        return true;
    Close();
    return false;
}

bool wxSMTP::Close()
{
    SendQuit();
    return wxCmdLineProtocol::Close();
}

bool wxSMTP::SendHello()
{
    wxIPV4address addr;
    wxString str;
    if(GetLocal(addr))
        str = addr.Hostname();
    else
        str = wxT("localhost");

  bool err;

    return GetResponseCode(SendCommand(wxT("HELO ")+str, &err))==220;
}

bool wxSMTP::SendQuit()
{
    SendCommand(wxT("QUIT"));
    ReadLine();
    return true;
}

bool wxSMTP::SendFrom(const wxString& addr)
{
    return GetResponseCode(SendCommand(wxT("MAIL FROM: <")+addr+wxT(">")))==250;
}

bool wxSMTP::SendTo(const wxString& addr)
{
    return GetResponseCode(SendCommand(wxT("RCPT TO: <")+addr+wxT(">")))==250;
}

bool wxSMTP::SendData()
{
    return GetResponseCode(SendCommand(wxT("DATA")))>0;
}

bool wxSMTP::SendContent(const wxString& content)
{
    return GetResponseCode(SendCommand(content+wxT("\r\n.\r\n")))>0;
}

bool wxSMTP::SendMail(wxEmailMessage& message)
{
    if(!SendFrom(message.GetFrom()))
        return false;

    for(int i=0; i<message.GetRecipientCount(); i++)
    {
        if(!SendTo(message.GetRecipient(i)))
            return false;
    }
    
    if(!SendData())
        return false;
    
    return SendContent(message.GetMessageContent());
}

int wxSMTP::GetResponseCode(const wxString& rep)
{
    wxString num = rep.BeforeFirst(wxT(' '));
    long l=0;
    if(num.ToLong(&l))
        return l;
    else
        return wxNOT_FOUND;
}

wxString wxSMTP::SendCommand(const wxString& request, bool* haveError)
{
    wxString line, res;
    
    res = wxCmdLineProtocol::SendCommand(request, haveError);
    
    if(res.Length()>4 && res[3]!=wxT(' '))
    {
        line = ReadLine();
        while(!line.IsEmpty() && line[3]!=wxT(' '))
        {
            res << wxT("\r\n") << line;
            line = ReadLine();
        }
    }
    
    return res;

}


#endif /* _WX */
