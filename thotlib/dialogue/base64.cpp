/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include <wx/wx.h>
#include "base64.h"
#include <iostream>
#include <string.h>
#include <wx/strconv.h>
#define bufferSize  5

/**
 * @{
 * Parts of b64.c of the base64.sf.net project
 */

/* **  Translation Table as described in RFC1113 */
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
/* ** Translation Table to decode (created by author) */
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*----------------------------------------------------------------------
  encodeblock
  encode 3 8-bit binary bytes as 4 '6-bit' characters
  ----------------------------------------------------------------------*/
static inline void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
  out[0] = cb64[ in[0] >> 2 ];
  out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
  out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
  out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static inline void encodeblock_full( unsigned char in[3], unsigned char out[4])
{
  out[0] = cb64[ in[0] >> 2 ];
  out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
  out[2] = cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ];
  out[3] = cb64[ in[2] & 0x3f ];
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static inline void encodeblock_end( unsigned char in[3], unsigned char out[4], int len)
{
  out[0] = cb64[ in[0] >> 2 ];
  if (len==1)
    {
      out[1] = cb64[ ((in[0] & 0x03) << 4) ];
      out[2] = '=';
    }
  else
    {
      out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
      out[2] = cb64[ ((in[1] & 0x0f) << 2) ];
    }
  out[3] = '=';
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static inline void encodeblocks(unsigned char* in, unsigned char* out, int len )
{
  while (len>=3)
    {
      encodeblock_full(in, out);
      in += 3;
      out += 4;
      len -= 3;
    }
  if (len>0)
    encodeblock_end(in, out, len);
}


/*----------------------------------------------------------------------
  decodeblock
  decode 4 '6-bit' characters into 3 8-bit binary bytes
  ----------------------------------------------------------------------*/
static inline void decodeblock( unsigned char in[4], unsigned char out[3] )
{   
  out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
  out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
  out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static inline void decodeblocks( unsigned char* in, unsigned char* out, int len )
{
  while (len>0)
    {
      decodeblock(in, out);
      in+=4;
      out+=3;
      len-=4;
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static inline unsigned char FilterChar(unsigned char c)
{
  c = (unsigned char) ((c < 43 || c > 122) ? 0 : cd64[ c - 43 ]);
  if (c)
    c = (unsigned char) ((c == '$') ? 0 : c - 61);
  return c;
}

/*----------------------------------------------------------------------
  wxBase64DecInputStream
  ----------------------------------------------------------------------*/
wxBase64DecInputStream::wxBase64DecInputStream(wxInputStream& stream):
  wxFilterInputStream(stream),
  m_outLen(0),
  m_inLen(0),
  m_outOffset(0),
  m_inOffset(0)
{
  m_out = new unsigned char [bufferSize*3+1];
  m_in  = new unsigned char [bufferSize*4+1];
  m_out[bufferSize*3] = m_in[bufferSize*4] = 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxBase64DecInputStream::~wxBase64DecInputStream()
{
  delete [] m_out;
  delete [] m_in;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
size_t wxBase64DecInputStream::OnSysRead(void *buffer, size_t size)
{
  size_t count = 0;

  if (m_outLen==0)
    {
      // Fill the buffer if no buffer.
      FillBuffers();
    }
  if (m_outLen>0)
    {
      // Flush the buffer
      count = m_outLen-m_outOffset;
      if (count>size)
        count = size;
      memcpy(buffer, m_out+m_outOffset, count);
      m_outOffset+=count;
      if (m_outOffset>=m_outLen)
        m_outLen = m_outOffset =0;
    }
  else if (GetFilterInputStream()->Eof())
    m_lasterror = wxSTREAM_EOF;
  return count;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void wxBase64DecInputStream::FillBuffers()
{
  if (m_outLen>0)
    return; /* Buffer already has something.*/
  m_outOffset = 0;

  if (m_inLen>0 && m_inOffset!=0)
    {
      for(int i=0; i<(m_inLen-m_inOffset); i++)
        /* Move existant data to begin of in buffer.*/
        m_in[i] = m_in[m_inOffset+i];
      m_inLen -= m_inOffset;
      m_inOffset = 0;
    }
    
  // Bufferize revelant octets.
  while (!GetFilterInputStream()->Eof() &&
         GetFilterInputStream()->CanRead() &&
         m_inLen<(bufferSize*4))
    {
      char c = GetFilterInputStream()->GetC();
      if (GetFilterInputStream()->Eof())
        break;
      c = FilterChar(c);
      if (c!=0)
        m_in[m_inLen++] = c-1;
    }
    
  // Decode
  if (!GetFilterInputStream()->Eof())
    {
      m_inOffset = m_inLen - (m_inLen%4);
      decodeblocks(m_in, m_out, m_inOffset);
      m_outLen = m_inLen/4*3;
      if (m_inOffset==m_inLen)
        {
          // All is encoded
          m_inLen = m_inOffset = 0;
        }
    }
  else
    {
      if (m_inLen>0)
        {
          decodeblocks(m_in, m_out, m_inLen);
          m_inOffset = m_inLen - (m_inLen%4);
          if (m_inOffset<m_inLen)
            m_inOffset+= 4;
          m_outLen = m_inOffset/4*3-1;
          m_inOffset = m_inLen = 0;
          // End of stream, all is encoded.
        }
    }

}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
char wxBase64DecInputStream::Peek()
{
  if (m_outLen==0)
    FillBuffers();
  return m_out[m_outOffset];
}

/*----------------------------------------------------------------------
  wxBase64EncInputStream
  ----------------------------------------------------------------------*/
wxBase64EncInputStream::wxBase64EncInputStream(wxInputStream& stream):
  wxFilterInputStream(stream),
  m_outLen(0),
  m_inLen(0),
  m_outOffset(0),
  m_inOffset(0)
{
  m_out = new unsigned char [bufferSize*4+1];
  m_in  = new unsigned char [bufferSize*3+1];
  m_out[bufferSize*4] = m_in[bufferSize*3] = 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxBase64EncInputStream::~wxBase64EncInputStream()
{
  delete [] m_out;
  delete [] m_in;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
size_t wxBase64EncInputStream::OnSysRead(void *buffer, size_t size)
{
  size_t count = 0;

  if (m_outLen==0)
    // Fill the buffer if no buffer.
    FillBuffers();
  if (m_outLen>0)
    {
      // Flush the buffer
      count = m_outLen-m_outOffset;
      if (count>size)
        count = size;
      memcpy(buffer, m_out+m_outOffset, count);
      m_outOffset+=count;
      if (m_outOffset>=m_outLen)
        m_outLen = m_outOffset =0;
    }
  else if (GetFilterInputStream()->Eof())
    m_lasterror = wxSTREAM_EOF;
  return count;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void wxBase64EncInputStream::FillBuffers()
{
  if (m_outLen>0)
    return; /* Buffer already has something.*/
  m_outOffset = 0;

  if (m_inLen>0 && m_inOffset!=0)
    {
      for(int i=0; i<(m_inLen-m_inOffset); i++)
        /* Move existant data to begin of in buffer.*/
        m_in[i] = m_in[m_inOffset+i];
      m_inLen -= m_inOffset;
      m_inOffset = 0;
    }
    
  // Bufferize at least 3 octets.
  while (m_inLen<3 && !GetFilterInputStream()->Eof())
    {
      GetFilterInputStream()->Read(m_in+m_inLen, 3*bufferSize-m_inLen);
      if (GetFilterInputStream()->LastRead()>0)
        m_inLen += GetFilterInputStream()->LastRead();
    }
    
  // Encode
  if (!GetFilterInputStream()->Eof())
    {
      m_inOffset = m_inLen - (m_inLen%3);
      encodeblocks(m_in, m_out, m_inOffset);
      m_outLen = m_inLen/3*4;
      if (m_inOffset==m_inLen)
        // All is encoded
        m_inLen = m_inOffset = 0;
    }
  else
    {
      encodeblocks(m_in, m_out, m_inLen);
      m_inOffset = m_inLen - (m_inLen%3);
      if (m_inOffset<m_inLen)
        m_inOffset+= 3;
      m_outLen = m_inOffset/3*4;
      m_inOffset = m_inLen = 0;
      // End of stream, all is encoded.
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
char wxBase64EncInputStream::Peek()
{
  if (m_outLen==0)
    FillBuffers();
  return m_out[m_outOffset];
}



/*----------------------------------------------------------------------
  wxBase64DecOutputStream
  ----------------------------------------------------------------------*/
wxBase64DecOutputStream::wxBase64DecOutputStream(wxOutputStream& stream):
  wxFilterOutputStream(stream),
  m_len(0)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static inline unsigned char PickFilteredChar (const unsigned char *buffer,
                                              size_t size, int& pos)
{
  unsigned char c = 0;
  while (pos<(int)size && (c = FilterChar(buffer[pos++]), c==0)){}
  return c;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
size_t wxBase64DecOutputStream::OnSysWrite(const void *buffer, size_t size)
{
  int offset = 0;
  unsigned char out[3];
  size_t count = 0;

  while (offset<(int)size)
    {
      m_in[m_len] = PickFilteredChar((unsigned char*)buffer, size, offset);
      if (m_in[m_len]!=0)
        {
          m_in[m_len]--;
          m_len++;
        }
      if (m_len==4)
        {
          decodeblock( m_in, out );
          GetFilterOutputStream()->Write((void*)out, 3);
          count+=3;
          m_len = 0;
        }
    }
  return size;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxBase64DecOutputStream::Close()
{
  unsigned char out[3];
  if (m_len>0)
    {
      decodeblock( m_in, out );
      GetFilterOutputStream()->Write((void*)out, m_len);
    }
  m_len=0;
  return wxFilterOutputStream::Close();
}

/*----------------------------------------------------------------------
  wxBase64EncOutputStream
  ----------------------------------------------------------------------*/
wxBase64EncOutputStream::wxBase64EncOutputStream(wxOutputStream& stream):
  wxFilterOutputStream(stream),
  m_len(0)
{
  m_in[0] = m_in[1] = m_in[2] = 0;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
size_t wxBase64EncOutputStream::OnSysWrite(const void *buffer, size_t size)
{
  int offset = 0, thisstep = 0;
  unsigned char out[4];

  // if we step entry by 510 bytes, we need output buffer of 680 bytes.
  const int     step = 510; 
  unsigned char result[680];
    
  if (m_len>0 && m_len+size>=3)
    {
      // Process keeping bytes.
      switch(m_len)
        {
        case 1:
          m_in[1] = ((unsigned char*)buffer)[0];
          m_in[2] = ((unsigned char*)buffer)[1];
          encodeblock(m_in, out, 3);
          GetFilterOutputStream()->Write((void*)out, 4);
          offset = 2;
          break;
        case 2:
          m_in[2] = ((unsigned char*)buffer)[0];
          encodeblock(m_in, out, 3);
          GetFilterOutputStream()->Write((void*)out, 4);
          offset = 1;
          break;
        default:
          break;
        }
    }
    
  while (size-offset>=3)
    {
      unsigned char* tab = ((unsigned char*)buffer)+offset;
      if (offset+step<=(int)size)
        thisstep = step;
      else
        {
          thisstep = size-offset;
          thisstep -= thisstep%3;
        }
      encodeblocks(tab, result, thisstep);
      GetFilterOutputStream()->Write((void*)result, thisstep/3*4);
      offset += thisstep;
    }
    
  m_len = size-offset;
  if (m_len==1)
    m_in[0] = ((unsigned char*)buffer)[size-1];
  else if (m_len==2)
    {
      m_in[0] = ((unsigned char*)buffer)[size-2];
      m_in[1] = ((unsigned char*)buffer)[size-1];
    }
    
  return size;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxBase64EncOutputStream::Close()
{
  unsigned char out[4];

  if (m_len>0)
    {
      encodeblock(m_in, out, m_len);
      GetFilterOutputStream()->Write((void*)out, 4);
    }
  m_len = 0;
  return wxFilterOutputStream::Close();
}


/*----------------------------------------------------------------------
  wxEndOfLineOutputStream
  ----------------------------------------------------------------------*/
wxEndOfLineOutputStream::wxEndOfLineOutputStream(wxOutputStream& stream, int linesize):
  wxFilterOutputStream(stream),
  m_size(linesize),
  m_len(0),
  m_buffer(NULL)
{
  m_buffer = new char[linesize + 2]; // size of line + crlf
  m_buffer[linesize]   = '\r';
  m_buffer[linesize+1] = '\n';
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxEndOfLineOutputStream::~wxEndOfLineOutputStream()
{
  Close();
  if (m_buffer!=NULL)
    {
      delete m_buffer;
      m_buffer = NULL;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool wxEndOfLineOutputStream::Close()
{
  if (m_len>0)
    {
      GetFilterOutputStream()->Write(m_buffer, m_len);
      m_len = 0;
    }
  return wxFilterOutputStream::Close();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
size_t wxEndOfLineOutputStream::OnSysWrite(const void *buffer, size_t size)
{
  const char*  buff = (const char*)buffer;
  size_t sz = size;

  while (m_len+(int)sz>=m_size)
    {
      int diff = m_size-m_len;
      memcpy(m_buffer+m_len, buff, diff);
      GetFilterOutputStream()->Write(m_buffer, m_size+2);
      m_len = 0;
      sz    -= diff;
      buff  += diff;
    }

  if (sz>0)
    {
      // bufferize 
      memcpy(m_buffer+m_len, buff, sz);
      m_len += sz;
    }
  return size;
}
#endif /* _WX */
