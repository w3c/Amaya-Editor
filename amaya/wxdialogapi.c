#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "amaya.h"


#ifdef _WX
  #include "wxdialog/InitConfirmDlgWX.h"
  #include "wxdialog/OpenDocDlgWX.h"
  #include "AmayaApp.h"
#endif /* _WX */

/*----------------------------------------------------------------------
  CreateInitConfirmDlgWX create the dialog for document changes (save/not save) comfirmation.
  params:
    + parent : parent window
    + title : dialog title
    + extrabutton : if it is present there is 3 button
    + confirmbutton : 
    + label : the message to show at dialog center
  returns:
  ----------------------------------------------------------------------*/
void CreateInitConfirmDlgWX ( ThotWindow parent,
			      char *title,
			      char *extrabutton,
			      char *confirmbutton,
			      char *label )
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


/*----------------------------------------------------------------------
  CreateOpenDocDlgWX create the dialog for openging new documents
  params:
    + parent : parent window
    + title : dialog title
    + listUrl : the url list to show in the combobox
    + docName : ??? not used
    + doc_select : ??? not used
    + dir_select : ??? not used 
    + doc_type : ??? not used
  returns:
  ----------------------------------------------------------------------*/
void CreateOpenDocDlgWX ( ThotWindow parent,
			  const char *title,
			  const char *urlList,
			  const char *docName,
			  int doc_select,
			  int dir_select,
			  DocumentType doc_type )
{
#ifdef _WX
  wxString wx_title( title, AmayaApp::conv_ascii );
  wxString wx_docName( docName, AmayaApp::conv_ascii );

  wxLogDebug( _T("CreateOpenDocDlgWX - title=")+wx_title+
	      _T("\tdocName=")+wx_docName );

  OpenDocDlgWX * p_dlg = new OpenDocDlgWX(
	 parent,
	 wx_title,
	 wx_docName );

  /* - Setup urlbar ------------------------------------------- */
  /* Append URL from url list to the urlbar */
  const char *ptr, *ptr1;
  wxString urltoappend;
  ptr = urlList;
  /* function will stop on double EOS */
  if (urlList)
    {
      while (*ptr != EOS)
	{
	  ptr1 = ptr;
	  while (*ptr1 != EOS)
	      ptr1++;
	  urltoappend = wxString( ptr, AmayaApp::conv_ascii );
	  p_dlg->AppendURL( urltoappend );
	  ptr = ptr1 + 1;
	}
    }

  /* the first url in the list is the used one for the current frame */
  wxString firsturl( urlList, AmayaApp::conv_ascii );
  p_dlg->SetCurrentURL( firsturl );
  /* ---------------------------------------------------------- */

  p_dlg->ShowModal();
  p_dlg->Destroy();

#endif /* _WX */
}
