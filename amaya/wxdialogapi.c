#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "amaya.h"
//#include "constmenu.h"


#ifdef _WX
  #include "wxdialog/InitConfirmDlgWX.h"
  #include "AmayaApp.h"
#endif /* _WX */

/*-----------------------------------------------------------------------
 CreateInitConfirmDlgWindow
 ------------------------------------------------------------------------*/
void CreateInitConfirmDlgWX (ThotWindow parent, char *title, char *extrabutton, char *confirmbutton, char *label)
{
#ifdef _WX
  wxString wx_label(label, AmayaApp::conv_ascii);
  wxString wx_title(TtaGetMessage (LIB, TMSG_LIB_CONFIRM), AmayaApp::conv_ascii);
  wxString wx_extrabutton;
  wxString wx_confirmbutton;

  if (extrabutton && extrabutton[0] != EOS)
  {
    /* a meesage with 3 buttons */
    wx_extrabutton = wxString(extrabutton, AmayaApp::conv_ascii);
    if (confirmbutton && confirmbutton[0] != EOS)
      wx_confirmbutton = wxString(confirmbutton, AmayaApp::conv_ascii);
    else
      wx_confirmbutton = wxString( TtaGetMessage(LIB, TMSG_LIB_CONFIRM), AmayaApp::conv_ascii);    
  }
  else
  {
    /* just 2 buttons */
    wx_extrabutton = wxString(TtaGetMessage (LIB, TMSG_LIB_CONFIRM), AmayaApp::conv_ascii);
  }

  InitConfirmDlgWX * p_dlg = new InitConfirmDlgWX(
      parent, /* parent window */
      wx_title, /* title */
      wx_extrabutton,
      wx_confirmbutton,
      wx_label ); /* message label */
  p_dlg->ShowModal();
  p_dlg->Destroy();
#endif /* _WX */
}
