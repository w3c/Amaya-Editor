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
void CreateInitConfirmDlgWX (ThotWindow parent, char *title, char *label)
{
#ifdef _WX
  // TODO : creer la class InitConfirmDlgWX a l'aide des ressources
  //  wxCSConv conv_ascii(_T("ISO-8859-1"));
  InitConfirmDlgWX * p_dlg = new InitConfirmDlgWX(
      parent, /* parent window */
      wxString(TtaGetMessage (LIB, TMSG_LIB_CONFIRM), AmayaApp::conv_ascii), /* title */
      wxString(label, AmayaApp::conv_ascii) ); /* message label */
  p_dlg->ShowModal();
  p_dlg->Destroy();
#endif /* _WX */
}
