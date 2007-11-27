
#include "wx/wx.h"
#include "wx/archive.h"   // for wxArchive*
#include "wx/zipstrm.h"   // for wxZip*Stream
#include "wx/wfstream.h"  // for wxF[F]ile*Stream
#include "wx/dir.h"       // for wxDir
#include "wx/filename.h"  // for wxFileName
#include "wx/string.h"    // for wxString
#include "message_wx.h" // for TtaConvMessageToWX

#include "archives.h"

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
ThotBool TtaCreateZipArchive(const char* srcpath, const char* dstfile)
{
  wxFileName name;
  bool isDir = false;
  wxString src = TtaConvMessageToWX(srcpath);
  
  
  // Check for source validity
  wxDir dir(src);
  if(dir.IsOpened())
    isDir = true;
  else if(!wxFile::Exists(src))
    return FALSE;

  // Check for destination validity
  wxFFileOutputStream out(TtaConvMessageToWX(dstfile));
  if(!out.IsOk())
    return FALSE;
  wxZipOutputStream zip(out);
  
  if(isDir)
    {
      wxArrayString files;
      wxDir::GetAllFiles(src, &files);
      int sz;
      src += wxFileName::GetPathSeparator();
      sz = src.Length();
      
      for(unsigned n=0; n<files.GetCount(); n++)
        {
          wxFFileInputStream in(files[n]);
          src = files[n];
          src.Remove(0, sz);
          zip.PutNextEntry(src);
          zip.Write(in);
        }
    }
  else
    {
      wxFFileInputStream in(src);
      zip.PutNextEntry(wxFileName::FileName(src).GetFullName());
      zip.Write(in);
    }
  zip.Close();

  return TRUE;
}
