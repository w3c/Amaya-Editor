#ifdef _WX

#ifndef __AMAYADOCUMENT_H__
#define __AMAYADOCUMENT_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

class AmayaDocument : public wxPanel
{
public:
  AmayaDocument( wxWindow * p_parent );
  
  virtual ~AmayaDocument( );

protected:
  DECLARE_EVENT_TABLE()

//  wxFlexGridSizer * m_pSizer;
  wxBoxSizer *      m_pSizerH;
  wxBoxSizer *      m_pSizerV;  
  wxScrollBar *     m_pScrollBarH;
  wxScrollBar *     m_pScrollBarV;
  wxPanel *         m_pCanvas;
  
    /*
  enum
   {
   };
*/
    
};

#endif // __AMAYADOCUMENT_H__

#endif /* #ifdef _WX */

