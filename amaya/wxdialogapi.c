
#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#ifdef _WX
  #include "wxdialog/InitConfirmDlgWX.h"
  #include "wxdialog/OpenDocDlgWX.h"
  #include "wxdialog/TitleDlgWX.h"
  #include "wxdialog/SearchDlgWX.h"
  #include "AmayaApp.h"

  #include "appdialogue_wx.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "amaya.h"

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
  wxString wx_label = TtaConvMessageToWX( label );
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_CONFIRM) );
  wxString wx_extrabutton;
  wxString wx_confirmbutton;

  if (extrabutton && extrabutton[0] != EOS)
  {
    /* a message with 3 buttons */
    wx_extrabutton = TtaConvMessageToWX( extrabutton );
    if (confirmbutton && confirmbutton[0] != EOS)
      wx_confirmbutton = TtaConvMessageToWX( confirmbutton );
    else
      wx_confirmbutton = TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_LIB_CONFIRM) );
  }
  else
  {
    /* just 2 buttons */
    wx_extrabutton = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_CONFIRM) );
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
  wxString wx_title = TtaConvMessageToWX( title );
  wxString wx_docName = TtaConvMessageToWX( docName );

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
	  urltoappend = TtaConvMessageToWX( ptr );
	  p_dlg->AppendURL( urltoappend );
	  ptr = ptr1 + 1;
	}
    }

  /* the first url in the list is the used one for the current frame */
  wxString firsturl = TtaConvMessageToWX( urlList );
  p_dlg->SetCurrentURL( firsturl );
  /* ---------------------------------------------------------- */

  p_dlg->ShowModal();
  p_dlg->Destroy();

#endif /* _WX */
}

/*----------------------------------------------------------------------
  CreateTitleDlgWX create the dialog to change the document title
  params:
    + doc_title : the current document title
  returns:
  ----------------------------------------------------------------------*/
void CreateTitleDlgWX ( ThotWindow parent,
			char *doc_title )
{
#ifdef _WX
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_CHANGE_TITLE) );
  wxString wx_doc_title = TtaConvMessageToWX( doc_title );

  wxLogDebug( _T("TitlelgWX - title=")+wx_title+
	      _T("\tdoc_title=")+wx_doc_title );

  TitleDlgWX * p_dlg = new TitleDlgWX(
      parent,
      wx_title,
      wx_doc_title );

  p_dlg->ShowModal();
  p_dlg->Destroy();
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CreateSearchDlgWX create the Search dialog
  params:
    + doc_title : the current document title
  returns:
  ----------------------------------------------------------------------*/
void CreateSearchDlgWX ( ThotWindow parent,  char* caption)
{
#ifdef _WX
  wxString wx_caption = TtaConvMessageToWX( caption );

  wxLogDebug( _T("SearchDlgWX - caption=")+wx_caption );

  SearchDlgWX * p_dlg = new SearchDlgWX(
      parent,
      wx_caption );

  p_dlg->ShowModal();
  p_dlg->Destroy();
#endif /* _WX */
}
