#ifdef _WX

#ifndef MD5_H_
#define MD5_H_

#include <wx/stream.h>

/* fix for MSVC ...evil! */
#ifdef _MSC_VER
   #define CONST64(n) n ## ui64
   typedef unsigned __int64 ulong64;
#else
   #define CONST64(n) n ## ULL
   typedef unsigned long long ulong64;
#endif

/* this is the "32-bit at least" data type
 * Re-define it to suit your platform but it must be at least 32-bits
 */
#if defined(__x86_64__) || (defined(__sparc__) && defined(__arch64__))
   typedef unsigned ulong32;
#else
   typedef unsigned long ulong32;
#endif

#ifndef ENDIAN_NEUTRAL
#define ENDIAN_NEUTRAL
#endif


/**
 * Base hash stream
 */
class wxHashStream : public wxOutputStream
{
public:
  virtual size_t GetHashResultSize()const=0;
  virtual const unsigned char* GetHashResult()const=0;
  wxString GetHashStringResult()const;
};


/**
 * MD5 hash class.
 */
class wxMD5Stream : public wxHashStream
{
public:
    wxMD5Stream();
    virtual ~wxMD5Stream();

    size_t GetHashResultSize()const{return 16;}
    const unsigned char* GetHashResult()const{return m_out;}

    virtual bool Close();

protected:
    virtual size_t OnSysWrite(const void *buffer, size_t size);
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode){return wxInvalidOffset;}

    DECLARE_NO_COPY_CLASS(wxMD5Stream)

private:
    void Compress(unsigned char *buf);

    // result
  unsigned char m_out[16];

    // hash
    ulong64 length;
    ulong32 state[4], curlen;
    unsigned char buf[64];
};


/**
 * HMAC-MD5 hash class.
 */
class wxHMACMD5Stream : public wxHashStream
{
public:
  wxHMACMD5Stream(const unsigned char* key, size_t keysize);
    virtual ~wxHMACMD5Stream();

    size_t GetHashResultSize()const{return /*m_out.GetHashResultSize()*/16;}
    const unsigned char* GetHashResult()const{return m_out.GetHashResult();}

    virtual bool Close();

protected:
    virtual size_t OnSysWrite(const void *buffer, size_t size);
    virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode){return wxInvalidOffset;}

    DECLARE_NO_COPY_CLASS(wxHMACMD5Stream)

private:
    void Compress(unsigned char *buf);

    wxMD5Stream m_in, m_out;
};

#endif /*MD5_H_*/
#endif /* _WX */
