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
  InitConfirmDlgWX dlg(
      parent, /* parent window */
      wxString(TtaGetMessage (LIB, TMSG_LIB_CONFIRM), AmayaApp::conv_ascii), /* title */
      wxString(label, AmayaApp::conv_ascii) ); /* message label */
  dlg.ShowModal();
#endif /* _WX */
}
