#ifdef _WX

#ifndef BASE64_H_
#define BASE64_H_

#include <wx/stream.h>



/**
 * Base64 decoder input stream
 */
class wxBase64DecInputStream : public wxFilterInputStream{
protected:
    unsigned char* m_out;
    unsigned char* m_in;
    int m_outLen;
    int m_inLen;
    int m_outOffset;
    int m_inOffset;
    
    void FillBuffers();
    virtual size_t OnSysRead(void *buffer, size_t size);
public:
    wxBase64DecInputStream(wxInputStream& stream);
    virtual ~wxBase64DecInputStream();
    
    wxFileOffset GetLength() const{return  wxInvalidOffset;}
    char Peek();
};

/**
 * Base64 encoder input stream
 */
class wxBase64EncInputStream : public wxFilterInputStream{
protected:
    unsigned char* m_out;
    unsigned char* m_in;
    int m_outLen;
    int m_inLen;
    int m_outOffset;
    int m_inOffset;
    
    virtual size_t OnSysRead(void *buffer, size_t size);
    void FillBuffers();
public:
    wxBase64EncInputStream(wxInputStream& stream);
    virtual ~wxBase64EncInputStream();

    wxFileOffset GetLength()const{return wxInvalidOffset;}
    char Peek();
};



/**
 * Base64 decoder output stream
 * @note If the target stream is not embedded, you must explicitly close the stream to finish the decoding process. 
 */
class wxBase64DecOutputStream : public wxFilterOutputStream{
protected:
    unsigned char m_in[4];
    int m_len;

    virtual size_t OnSysWrite(const void *buffer, size_t size);
public:
    wxBase64DecOutputStream(wxOutputStream& stream);
    virtual ~wxBase64DecOutputStream(){Close();}
    bool Close();
};

/**
 * Base64 encoder output stream
 * @note If the target stream is not embedded, you must explicitly close the stream to finish the encoding process. 
 */
class wxBase64EncOutputStream : public wxFilterOutputStream{
protected:
    unsigned char m_in[3]; // Buffer for keeping bytes.
    int m_len; /// number of keeping bytes in buffer.

    virtual size_t OnSysWrite(const void *buffer, size_t size);
    
public:
    wxBase64EncOutputStream(wxOutputStream& stream);
    virtual ~wxBase64EncOutputStream(){Close();}
    bool Close();
};


/**
 * Base 64 Input stream (decoding).
 */
typedef class wxBase64DecInputStream wxBase64InputStream;

/**
 * Base 64 Output stream (encoding).
 */
typedef class wxBase64EncOutputStream wxBase64OutputStream;


/**
 * Special output stream which inserts end-of-line regulary.
 */
class wxEndOfLineOutputStream : public wxFilterOutputStream{
protected:
  int m_size;
  int m_len;
  char* m_buffer;
  
  virtual size_t OnSysWrite(const void *buffer, size_t size);
public:
  wxEndOfLineOutputStream(wxOutputStream& stream, int linesize=76);
  virtual ~wxEndOfLineOutputStream();
  bool Close();
};

#endif /*BASE64_H_*/
#endif /* _WX */
