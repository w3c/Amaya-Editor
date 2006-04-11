#ifdef _WX

#ifndef __AMAYAFILEDROPTARGET_H__
#define __AMAYAFILEDROPTARGET_H__

#include "wx/dnd.h"

class AmayaFrame;

class AmayaFileDropTarget : public wxFileDropTarget
{
public:
  AmayaFileDropTarget( AmayaFrame *pOwner ) { m_pOwner = pOwner; }
  virtual ~AmayaFileDropTarget();
  virtual bool OnDropFiles( wxCoord x, wxCoord y,
			    const wxArrayString& filenames );
  
private:
  AmayaFrame * m_pOwner;
};

#endif /* __AMAYAFILEDROPTARGET_H__ */

#endif /* _WX */
