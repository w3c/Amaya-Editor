/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
#include <wx/mstream.h>
#include <wx/timer.h>

/*----------------------------------------------------------------------
  wxByteCounterOutputStream
  ----------------------------------------------------------------------*/
wxByteCounterOutputStream::wxByteCounterOutputStream ():
  m_count(0)
{
  memset(m_table, 0, sizeof(m_table));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
size_t wxByteCounterOutputStream::OnSysWrite(const void * buffer,
                                             size_t size)
{
  const unsigned char* in = (const unsigned char*)buffer;
  size_t pos;
  unsigned char c;
  
  for (pos=0; pos<size; pos++)
    {
      c = in[pos];
      m_table[c]++;
    }
  m_count += size;
  return size;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxFileOffset wxByteCounterOutputStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
  return wxInvalidOffset;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxFileOffset wxByteCounterOutputStream::OnSysTell() const
{
  return wxInvalidOffset;
}


/*----------------------------------------------------------------------
  wxQuotedPrintableOutputStream
  ----------------------------------------------------------------------*/
wxQuotedPrintableOutputStream::wxQuotedPrintableOutputStream(wxOutputStream& stream):
  wxFilterOutputStream(stream),
  m_pos(0)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxQuotedPrintableOutputStream::Close()
{
  if (m_pos>0)
    {
      GetFilterOutputStream()->Write(m_buffer, m_pos);
      m_pos = 0;
    }
  return wxFilterOutputStream::Close();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void wxQuotedPrintableOutputStream::FlushBuffer()
{
  if (m_pos>=3 && m_buffer[m_pos-2]=='\r' && m_buffer[m_pos-1]=='\n')
    {
      // Trap CRLF
      if (m_buffer[m_pos-3]==' ')
        {
          // Penultimate character is space, must escape it.
          m_buffer[m_pos-3] = '=';
          m_buffer[m_pos-2] = '2';
          m_buffer[m_pos-1] = '0';
          m_buffer[m_pos++] = '\r';
          m_buffer[m_pos++] = '\n';
        }
      else if (m_buffer[m_pos-1]=='\t')
        {
          // Penultimate character is tab, must escape it.        
          m_buffer[m_pos-3] = '=';
          m_buffer[m_pos-2] = '0';
          m_buffer[m_pos-1] = '9';
          m_buffer[m_pos++] = '\r';
          m_buffer[m_pos++] = '\n';
        }

    }
  else if (m_pos>=2 && (m_buffer[m_pos-1]=='\r'||m_buffer[m_pos-1]=='\n'))
    {
      // Trap CR or LF
      char c = m_buffer[m_pos-1];
      if (m_buffer[m_pos-2]==' ')
        {
          // Penultimate character is space, must escape it.
          m_buffer[m_pos-2] = '=';
          m_buffer[m_pos-1] = '2';
          m_buffer[m_pos++] = '0';
          m_buffer[m_pos++] = c;
        }
      else if (m_buffer[m_pos-1]=='\t')
        {
          // Penultimate character is tab, must escape it.        
          m_buffer[m_pos-2] = '=';
          m_buffer[m_pos-1] = '0';
          m_buffer[m_pos++] = '9';
          m_buffer[m_pos++] = c;
        }
    }
  else if (m_pos>=1)
    {
      if (m_buffer[m_pos-1]==' ')
        {
          // Last character is space, must escape it.
          m_buffer[m_pos-1] = '=';
          m_buffer[m_pos++] = '2';
          m_buffer[m_pos++] = '0';
        }
      else if (m_buffer[m_pos-1]=='\t')
        {
          // Last character is tab, must escape it.        
          m_buffer[m_pos-1] = '=';
          m_buffer[m_pos++] = '0';
          m_buffer[m_pos++] = '9';
        }
      m_buffer[m_pos++] = '=';
      m_buffer[m_pos++] = '\r';
      m_buffer[m_pos++] = '\n';
    }
  
  GetFilterOutputStream()->Write(m_buffer, m_pos);
  m_pos = 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
size_t wxQuotedPrintableOutputStream::OnSysWrite(const void *buffer, size_t size)
{
  const char *in = (const char*)buffer;
  int pos = 0;
  char c;
  
  while (pos<(int)size)
    {
      // Verify buffer size and flush if needed.
      if (m_pos>=(76-6))
        FlushBuffer();

      c = in[pos];
      if (c=='\r'||c=='\n')
        {
          m_buffer[m_pos++] = c;
          FlushBuffer(); // Force flush to dump line.
        }
      else if ( c==61 )
        {
          // '=' symbol, must escape it.
          m_buffer[m_pos++] = '=';
          m_buffer[m_pos++] = '3';
          m_buffer[m_pos++] = 'D';
        }
      else if ( (c>=33 && c<=126 /*&& c!=61*/) || c==9 || c==32)
        {
          // Allowed character.
          m_buffer[m_pos++] = c;
        }
      else
        {
          short s = (short)(unsigned char)c;
          sprintf(m_buffer+m_pos, "=%02hX", s);
          m_pos += 3;
        }
      pos++;
    }
  return size;
}

//==========================================================================
//==========================================================================
IMPLEMENT_CLASS(wxMimeSlot, wxObject)

/*----------------------------------------------------------------------
 wxMimeSlot
 ----------------------------------------------------------------------*/
wxMimeSlot::wxMimeSlot():
  wxObject()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxMimeSlot::wxMimeSlot(const wxString& contentType, const wxString& data):
  wxObject(),
  m_contentType(contentType),
  m_transfertEncoding(wxMIME_CONTENT_TRANSFERT_ENCONDING_AUTO),
  m_dataType(wxMimeSlotContentText),
  m_textContent(data)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxMimeSlot::wxMimeSlot(const wxString& contentType, size_t size, void* data):
  wxObject(),
  m_contentType(contentType),
  m_transfertEncoding(wxMIME_CONTENT_TRANSFERT_ENCONDING_AUTO),
  m_dataType(wxMimeSlotContentBinary)
{
  m_binaryContent.size = size;
  m_binaryContent.data = data;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxMimeSlot::wxMimeSlot(const wxFileName& filename, const wxString& sendPath, const wxString& contentType):
  wxObject(),
  m_contentType(contentType),
  m_transfertEncoding(wxMIME_CONTENT_TRANSFERT_ENCONDING_AUTO),
  m_dataType(wxMimeSlotContentFile)
{
  m_fileContent.filename = filename;
  if (!sendPath.IsEmpty())
    m_fileContent.sendpath = sendPath;
  else
    m_fileContent.sendpath = filename.GetFullName();

  if (contentType==wxT("text/plain") || contentType==wxT("text/html"))
    {
      m_transfertEncoding = wxMIME_CONTENT_TRANSFERT_ENCONDING_AUTO;
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxMimeSlot::wxMimeSlot(wxMultipartMimeContainer* mime):
  wxObject(),
  m_contentType(mime->GetContentType()),
  m_transfertEncoding(wxMIME_CONTENT_TRANSFERT_ENCONDING_AUTO),
  m_dataType(wxMimeSlotContentMime),
  m_mimeContent(mime)
{
}
    
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxMimeSlot::~wxMimeSlot()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxMimeSlot::Write(wxOutputStream& out, bool bInline)const
{
  if (m_dataType==wxMimeSlotContentMime)
    return m_mimeContent->Write(out);

  // Write the header.
  wxString msg;
  msg << wxT("Content-Type: ") << m_contentType;
  
  wxMimeExtraParamMap::const_iterator iter;
  for (iter = m_contentTypeExtraParams.begin();iter!=m_contentTypeExtraParams.end(); iter++)
    {
      msg << wxT(";    ") << iter->first;
      if (!iter->second.IsEmpty())
        msg << wxT("=") << iter->second;
    }
  if (m_dataType==wxMimeSlotContentFile && !bInline)
    msg << wxT("; name=\"") << m_fileContent.sendpath << wxT("\"");
  msg << wxT("\r\n");
    
  wxMimeContentTransfertEncoding te = m_transfertEncoding; 
  if (te==wxMIME_CONTENT_TRANSFERT_ENCONDING_AUTO)
    te = GetAutoContentTransfertEncoding();

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

  for (iter = m_extraParams.begin();iter!=m_extraParams.end(); iter++)
    {
      msg << iter->first;
      if (!iter->second.IsEmpty())
        msg << wxT(": ") << iter->second;
      msg << wxT("\r\n");
    }

  // Empty line head/content separator.
  msg << wxT("\r\n");

  // Flush the header.
  out.Write((const char*)msg.mb_str(wxConvLibc), msg.Length());

  // Prepare input stream.
  wxInputStream* in = NULL;
  switch(m_dataType)
    {
    case wxMimeSlotContentMime: // Already returned.
      break;
    case wxMimeSlotContentText:
      in = new wxStringInputStream(m_textContent);
      break;
    case wxMimeSlotContentFile:
      in = new wxFFileInputStream(m_fileContent.filename.GetFullPath());
      break;
    default: // Binary
      in = new wxMemoryInputStream(m_binaryContent.data, m_binaryContent.size);
      break;
    }
  
  // Prepare intermediate output stream and write it.
  if (in)
    {
      switch(te)
        {
        case wxMIME_CONTENT_TRANSFERT_ENCONDING_QUOTED_PRINTABLE:
          {
            wxQuotedPrintableOutputStream qp(out);
            qp.Write(*in);
            qp.Close();
            break;
          }
        case wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64:
          {
            wxEndOfLineOutputStream eol(out);
            wxBase64EncOutputStream base64(eol);
            base64.Write(*in);
            base64.Close();
            eol.Close();
            break;
          }
        default:
          {
            out.Write(*in);
            break;
          }
        }
      delete in;
    }
  return true;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxMimeContentTransfertEncoding wxMimeSlot::GetAutoContentTransfertEncoding()const
{
  wxByteCounterOutputStream stm;
 
  unsigned long l128_255 = 0;
  unsigned long l33_126  = 0;
  unsigned long l0_32    = 0;
  unsigned long l127     = 0;
  unsigned long lGlobal;
  
  int i;
  
  // Process count from source.
  switch(m_dataType)
    {
    case wxMimeSlotContentMime:
      break;
    case wxMimeSlotContentText:
      {
        wxStringInputStream  in(m_textContent);
        stm.Write(in);
        break;
      }
    case wxMimeSlotContentFile:
      {
        wxFile f(m_fileContent.filename.GetFullPath());
        wxFileInputStream file(f);
        stm.Write(file);
        break;      
      }
    default: // Binary;
      {
        stm.Write(m_binaryContent.data, m_binaryContent.size);
        break;
      }
    }
  
  lGlobal = stm.GetCount();
  for (i=0; i<=32; i++)
    l0_32 += stm.GetCount(i);
  for (i=33; i<=126; i++)
    l33_126 += stm.GetCount(i);
  l127 = stm.GetCount(127);
  for (i=128; i<=255; i++)
    l128_255 += stm.GetCount(i);

  if ( (l0_32+l127+l128_255)*3 < l33_126)
    return wxMIME_CONTENT_TRANSFERT_ENCONDING_QUOTED_PRINTABLE;
  
  return wxMIME_CONTENT_TRANSFERT_ENCONDING_BASE64;
}

/*----------------------------------------------------------------------
  wxMultipartMimeContainer
  ----------------------------------------------------------------------*/
wxMultipartMimeContainer::wxMultipartMimeContainer(const wxString& contentType, const wxString& boundary, const wxString message):
  wxList(),
  m_contentType(contentType),
  m_boundary(boundary),
  m_message(message)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxMultipartMimeContainer::~wxMultipartMimeContainer()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Append(const wxMimeSlot& slot)
{
  return wxList::Append(new wxMimeSlot(slot));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Append(const wxString& contentType, const wxString& data)
{
  return wxList::Append(new wxMimeSlot(contentType, data));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Append(const wxString& contentType, size_t size, void* data)
{
  return wxList::Append(new wxMimeSlot(contentType, size, data));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Append(const wxFileName& filename, const wxString& sendPath, const wxString& contentType)
{
  return wxList::Append(new wxMimeSlot(filename, sendPath, contentType));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Append(wxMultipartMimeContainer* mime)
{
  return wxList::Append(new wxMimeSlot(mime));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Prepend(const wxMimeSlot& slot)
{
  return wxList::Insert(new wxMimeSlot(slot));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Prepend(const wxString& contentType, const wxString& data)
{
  return wxList::Insert(new wxMimeSlot(contentType, data));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Prepend(const wxString& contentType, size_t size, void* data)
{
  return wxList::Insert(new wxMimeSlot(contentType, size, data));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Prepend(const wxFileName& filename, const wxString& sendPath, const wxString& contentType)
{
  return wxList::Insert(new wxMimeSlot(filename, sendPath, contentType));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Prepend(wxMultipartMimeContainer* mime)
{
  return wxList::Insert(new wxMimeSlot(mime));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Insert(size_t pos, const wxMimeSlot& slot)
{
  return wxList::Insert(pos, new wxMimeSlot(slot));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Insert(size_t pos, const wxString& contentType, const wxString& data)
{
  return wxList::Insert(pos, new wxMimeSlot(contentType, data));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Insert(size_t pos, const wxString& contentType, size_t size, void* data)
{
  return wxList::Insert(pos, new wxMimeSlot(contentType, size, data));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Insert(size_t pos, const wxFileName& filename, const wxString& sendPath, const wxString& contentType)
{
  return wxList::Insert(pos, new wxMimeSlot(filename, sendPath, contentType));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxMultipartMimeContainer::Insert(size_t pos, wxMultipartMimeContainer* mime)
{
  return wxList::Insert(pos, new wxMimeSlot(mime));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString wxMultipartMimeContainer::GenerateBoundary()const
{
  static int nb = 0;
  wxLongLong ll = wxGetLocalTimeMillis() + nb++;
  return wxT("part") + ll.ToString();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxMultipartMimeContainer::Write(wxOutputStream& out)const
{
  bool bInline = (m_contentType==wxMIMETYPE_MULTIPART_ALTERNATIVE); 
  wxString msg;
    
  // Bufferize the header
  msg << wxT("Content-type: ") << m_contentType;
  wxString boundary = m_boundary;
  if (boundary.IsEmpty())
    boundary = GenerateBoundary();    
  msg << wxT(";\r\n    boundary=\"") << boundary << wxT("\"\r\n"); 
  for (wxMimeExtraParamMap::const_iterator iter = m_contentTypeExtraParams.begin();iter!=m_contentTypeExtraParams.end(); iter++)
    {
      msg << wxT("; ") << iter->first;
      if (!iter->second.IsEmpty())
        msg << wxT("=") << iter->second;
      msg << wxT("\r\n");
    }
  msg << wxT("\r\n") << m_message << wxT("\r\n");
  // Flush the header.
  out.Write((const char*)msg.mb_str(wxConvLibc), msg.Length());

  // Stream the content.
  for (wxNode* node=GetFirst(); node; node=node->GetNext())
    {
      msg = wxT("--") + boundary + wxT("\r\n");
      out.Write((const char*)msg.mb_str(wxConvLibc), msg.Length());
      wxMimeSlot* slot = (wxMimeSlot*) node->GetData();
      slot->Write(out, bInline);
      msg = wxT("\r\n");
      out.Write((const char*)msg.mb_str(wxConvLibc), msg.Length());
    }
  msg = wxT("--") + boundary + wxT("--\r\n");
  out.Write((const char*)msg.mb_str(wxConvLibc), msg.Length());
    
  return true;
}


/*----------------------------------------------------------------------
  wxEmailMessage
  ----------------------------------------------------------------------*/
wxEmailMessage::wxEmailMessage(const wxString& subject, const wxString& text, const wxString& from):
  m_subject(subject),
  m_text(text),
  m_from(from),
  m_attachements(wxMIMETYPE_MULTIPART_MIXED),
  m_alternatives(wxMIMETYPE_MULTIPART_ALTERNATIVE)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxEmailMessage::AddFile(const wxFileName& fileName, wxString mimeType, bool bInline, const wxString& name)
{
  if (fileName.FileExists())
    {
      if (mimeType.IsEmpty())
        {
          wxFileType* type = wxTheMimeTypesManager->GetFileTypeFromExtension(fileName.GetExt());
          if (type)
            {
              if (!type->GetMimeType(&mimeType))
                mimeType = wxT("application/octet-stream");
            }
        }
      wxNode *node = m_attachements.Append(fileName, name, mimeType);
      if (node)
        {
          ((wxMimeSlot*)node->GetData())->SetExtraParam(wxT("Content-Disposition"), bInline?wxT("inline"):wxT("attachment"));
        }
      return node;
    }
  return NULL;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxEmailMessage::AddAlternativeFile(const wxFileName& fileName, wxString mimeType)
{
  if (fileName.FileExists())
    {
      if (mimeType.IsEmpty())
        {
          wxFileType* type = wxTheMimeTypesManager->GetFileTypeFromExtension(fileName.GetExt());
          if (type)
            {
              if (!type->GetMimeType(&mimeType))
                mimeType = wxT("application/octet-stream");
            }
        }
      wxNode *node = m_alternatives.Append(fileName, wxT(""), mimeType);
      if (node)
        {
          ((wxMimeSlot*)node->GetData())->SetExtraParam(wxT("Content-Disposition"), wxT("inline"));
        }
      return node;
    }
  return NULL;  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxNode* wxEmailMessage::AddAlternative(const wxString data, const wxString& mimeType)
{
  return m_alternatives.Append(mimeType, data);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void wxEmailMessage::AddRecipient(const wxString& address)
{
  m_rcptArray.Add(address);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void wxEmailMessage::AddTo(const wxString& address)
{
  m_toArray.Add(address);
  m_rcptArray.Add(address);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void wxEmailMessage::AddCc(const wxString& address)
{
  m_ccArray.Add(address);
  m_rcptArray.Add(address);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void wxEmailMessage::AddBcc(const wxString& address)
{
  m_bccArray.Add(address);
  m_rcptArray.Add(address);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void wxEmailMessage::AddExtraHeader(const wxString& name, const wxString& value)
{
  m_extraHeaders.Add(name + wxT(": ") + value);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxEmailMessage::Write(wxOutputStream& out)
{
  wxString msg;
  int i;
  msg << wxT("From: ") << QEncode(m_from) << wxT("\r\n");

  if (m_toArray.GetCount() > 0) {
    msg << wxT("To: ");
    for (i = 0; i < (int)m_toArray.GetCount() ; i++) {
      if (i > 0) msg << wxT(",") << wxT("\r\n    ");
      msg << QEncode(m_toArray[i]);
    }
    msg << wxT("\r\n");
  }
  if (m_ccArray.GetCount() > 0) {
    msg << wxT("Cc: ");
    for (i = 0; i < (int)m_ccArray.GetCount() ; i++) {
      if (i > 0) msg << wxT(",") << wxT("\r\n    ");
      msg << QEncode(m_ccArray[i]);
    }
    msg << wxT("\r\n");
  }
  if (m_bccArray.GetCount() > 0) {
    msg << wxT("Cc: ");
    for (i = 0; i < (int)m_bccArray.GetCount() ; i++) {
      if (i > 0) msg << wxT(",") << wxT("\r\n    ");
      msg << QEncode(m_bccArray[i]);
    }
    msg << wxT("\r\n");
  }
    
  if (!m_subject.IsEmpty())
    {
      msg << wxT("Subject: ") << QEncode(m_subject) << wxT("\r\n");
    }
    
  for (i = 0; i < (int)m_extraHeaders.GetCount() ; i++) {
    msg << m_extraHeaders[i] << wxT("\r\n");
  }
    
  out.Write((const char*)msg.mb_str(wxConvLibc), msg.Length());

  msg.Empty();
  if (m_attachements.IsEmpty())
    {
      if (m_alternatives.IsEmpty())
        {
          msg << wxT("Content-Type: text/plain; charset=UTF-8\r\n")
              << m_text << wxT("\r\n");
          out.Write((const char*)msg.mb_str(wxConvUTF8), msg.Length());
          return true;
        }
      else
        {
          if (!m_text.IsEmpty())
            m_alternatives.Prepend(wxT("text/plain; charset=UTF-8"), m_text);
          m_alternatives.SetMessage(wxT("This is a multi-part message in MIME format."));
          msg << wxT("MIME-version: 1.0\r\n");
          out.Write((const char*)msg.mb_str(wxConvUTF8), msg.Length());
          return m_alternatives.Write(out);
        }
    }
  else
    {
      m_alternatives.SetMessage(wxT("This is a multi-part message in MIME format."));
      if (!m_text.IsEmpty())
        m_alternatives.Prepend(wxT("text/plain; charset=UTF-8"), m_text);
      m_attachements.Prepend(&m_alternatives);
        
      msg = wxT("MIME-version: 1.0\r\n");
      out.Write((const char*)msg.mb_str(wxConvUTF8), msg.Length());
      return m_attachements.Write(out);
        
    }
}

/*----------------------------------------------------------------------
  QEncode
  Make a Q encoding for the string (in UTF-8).
  Usefull to allow extended character in mail header (title ...)
  ----------------------------------------------------------------------*/
wxString wxEmailMessage::QEncode(const wxString& str)
{
  wxString res;
  char buffer[1024];
  bool needed = false;
  int i, n;
  unsigned char c;

  strcpy(buffer, (const char*)str.mb_str(wxConvUTF8));  
  n = strlen(buffer);
  for (i=0; i<n; i++)
    {
      c = buffer[i];
      if (c<33 || c>126)
        {
          needed = true;
          break;
        }
    }

  if (needed)
    {
      res += wxT("=?UTF-8?Q?");
      
      for (i=0; i<n; i++)
        {
          c = buffer[i];
          if (c==' ')
            res += wxT('_');
          else if (c<33 || c>126 || c=='?' || c=='=' || c=='_')
            {
              res += wxString::Format(wxT("=%02hX"), (short)c);
            }
          else
            res += buffer[i];
        }
      
      res += wxT("?=");
      return res;
    }
  else
    return str;
}

/*----------------------------------------------------------------------
  wxSMTP
  ----------------------------------------------------------------------*/
wxSMTP::wxSMTP():
  wxCmdLineProtocol(),
  m_error(0),
  m_errorStep(wxSMTP_STEP_UNKNOW),
  m_useAuth(false)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void wxSMTP::UseAuthentification(const wxString& name, const wxString& passwd)
{
  m_useAuth = true;
  m_user    = name;
  m_passwd  = passwd;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::Connect(wxSockAddress& address, bool wait)
{
  m_errorStep = wxSMTP_STEP_CONNECT;

  wxCmdLineProtocol::Connect(address, wait);
  ReadLine();
  SendHello();
  return true;
    
  if (!wxCmdLineProtocol::Connect(address, wait))
    return false;
  if (!SendHello())
    return true;
  Close();
  return false;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::Close()
{
  SendQuit();
  return wxCmdLineProtocol::Close();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::SendHello()
{
  m_errorStep = wxSMTP_STEP_HELLO;
  wxIPV4address addr;
  wxString str;
  if (GetLocal(addr))
    str = addr.Hostname();
  else
    str = wxT("localhost");
    
  str = SendCommand(wxT("EHLO ")+str);
  m_error = GetResponseCode(str);
    
  /* Process ESMTP params. */
  wxStringTokenizer tkz(str, wxT("\r\n"));
  bool bFirst = true;
  while ( tkz.HasMoreTokens() )
    {
      wxString line = tkz.GetNextToken();
      if (bFirst)
        {
          /* Skip first line. */
          bFirst = false;
          continue;
        }
      line.Remove(0, 4);
      line.Trim();
      wxString name, value;
      name = line.BeforeFirst(wxT(' '));
      value = line.AfterFirst(wxT(' '));
      m_params[name] = value;
    }
  return m_error==250;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::SendQuit()
{
  m_errorStep = wxSMTP_STEP_QUIT;
  m_error = GetResponseCode(SendCommand(wxT("QUIT")));
  return true;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::SendFrom(const wxString& addr)
{
  m_errorStep = wxSMTP_STEP_FROM;
  m_error = GetResponseCode(SendCommand(wxT("MAIL FROM: <")+addr+wxT(">")));
  return m_error==250;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::SendTo(const wxString& addr)
{
  wxString a;
  int pos1, pos2;
  m_errorStep = wxSMTP_STEP_RECIPIENT;
  pos1 = addr.Find(wxT('<'), true);
  pos2 = addr.Find(wxT('>'), true);
  if (pos1!=wxNOT_FOUND && pos2!=wxNOT_FOUND && pos1<pos2)
    {
      a = addr.Mid(pos1+1, pos2-pos1-1);
    }
  else if (pos1!=wxNOT_FOUND && (pos2!=wxNOT_FOUND || pos1>pos2))
    {
      a = addr.Mid(pos1+1);
    }
  else
    a = addr;
  m_error = GetResponseCode(SendCommand(wxT("RCPT TO: <")+a+wxT(">")));
  return m_error==250;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::SendData()
{
  m_errorStep = wxSMTP_STEP_DATA;
  m_error = GetResponseCode(SendCommand(wxT("DATA")));
  return m_error==354;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::SendContent(wxEmailMessage& message)
{
  m_errorStep = wxSMTP_STEP_CONTENT;
    
  {
    wxSocketOutputStream out(*this);
    message.Write(out);
  }

  m_error = GetResponseCode(SendCommand(wxT(".\r\n")));
  return m_error==250;

}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::SendMail(wxEmailMessage& message)
{
  if (!SendFrom(message.GetFrom()))
    return false;

  if (m_useAuth)
    if (!SendAuthentication())
      return false;
      
    
    
  for (int i=0; i<message.GetRecipientCount(); i++)
    {
      if (!SendTo(message.GetRecipient(i)))
        return false;
    }
    
  if (!SendData())
    return false;
    
  if (!SendContent(message))
    {
      return false;
    }
  return true;
    
    
  m_errorStep = wxSMTP_STEP_DONE;
  m_error = 0;
  return true;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxSMTP::SendAuthentication()
{
  if (m_params[wxT("AUTH")].IsEmpty())
    return true;
  
  // TODO !!!
  
  return true;
}

int wxSMTP::GetResponseCode(const wxString& rep)
{
  wxString num = rep.BeforeFirst(wxT(' '));
  long l=0;
  if (num.ToLong(&l))
    return l;
  else
    return wxNOT_FOUND;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString wxSMTP::SendCommand(const wxString& request, bool* haveError)
{
  wxString line, res;
    
  res = wxCmdLineProtocol::SendCommand(request, haveError);
    
  if (res.Length()>4 && res[3]!=wxT(' '))
    {
      line = ReadLine();
      while (!line.IsEmpty() && line[3]!=wxT(' '))
        {
          res << wxT("\r\n") << line;
          line = ReadLine();
        }
    }
    
  return res;

}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
long wxSMTP::GetLastError(long* step)
{
  if (step!=NULL)
    *step = m_errorStep;
  return m_error;
}

/*----------------------------------------------------------------------
  wxCmdLineProtocol
  ----------------------------------------------------------------------*/
wxString wxCmdLineProtocol::SendCommand(wxString request, bool* haveError)
{
  wxStringInputStream stmin(request << wxT("\r\n"));
  wxSocketOutputStream stmout(*this);
  stmout.Write(stmin);
  stmout.Close();

  /* Wait for a response. */    
  const int buffsize = 256; 
  char buff[buffsize];
  int search = 0;

  while (true)
    {
      wxSocketClient::Read(buff, buffsize);
      m_buffer += wxString(buff, *wxConvCurrent, LastCount());
        
      if (Error())
        {
          if (haveError!=NULL)
            *haveError = true;
          return wxT("");
        }
        
      for (; search < (int) m_buffer.Length()-1; search++)
        {
          if (m_buffer[search]==wxT('\r'))
            {
              if (m_buffer[search+1]==wxT('\n'))
                {
                  wxString rep = m_buffer.Left(search);
                  m_buffer = m_buffer.Mid(search+2);
                  return rep;
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString wxCmdLineProtocol::ReadLine(bool* haveError)
{
  /* Wait for a response. */    
  const int buffsize = 256; 
  char buff[buffsize];
  int search = 0;

  while (true)
    {
      search = m_buffer.Find(wxT("\r\n"));
      if (search!=wxNOT_FOUND)
        {
          wxString rep = m_buffer.Left(search);
          m_buffer = m_buffer.Mid(search+2);
          return rep;            
        }
      
      wxSocketClient::Read(buff, buffsize);
      m_buffer += wxString(buff, *wxConvCurrent, LastCount());
        
      if (Error())
        {
          if (haveError)
            *haveError = true;
          return wxT("");
        }
    }
}
#endif /* _WX */
