
#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#ifdef _WX
  #include "wxdialog/InitConfirmDlgWX.h"
  #include "wxdialog/OpenDocDlgWX.h"
  #include "wxdialog/TitleDlgWX.h"
  #include "wxdialog/SearchDlgWX.h"
  #include "wxdialog/PrintDlgWX.h"
  #include "wxdialog/SaveAsDlgWX.h"
  #include "wxdialog/CSSDlgWX.h"
  #include "wxdialog/DocInfoDlgWX.h"
  #include "wxdialog/HRefDlgWX.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "amaya.h"

#include "appdialogue_wx.h"
#include "message_wx.h"

/*----------------------------------------------------------------------
  CreateInitConfirmDlgWX create the dialog for document changes (save/not save) comfirmation.
  params:
    + the thotlib catalog reference
    + parent : parent window
    + title : dialog title
    + extrabutton : if it is present there is 3 button
    + confirmbutton : 
    + label : the message to show at dialog center
  returns:
    + true : the dialogue has been created
    + false : error, nothing is created
  ----------------------------------------------------------------------*/
ThotBool CreateInitConfirmDlgWX ( int ref,
				  ThotWindow parent,
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
      ref, /* thotlib catalog reference */
      parent, /* parent window */
      wx_title, /* title */
      wx_extrabutton,
      wx_confirmbutton,
      wx_label ); /* message label */

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    {
      /* the dialog has been sucesfully registred */
      return TRUE;
    }
  else
    {
      /* an error occured durring registration */
      p_dlg->Destroy();
      return FALSE;
    }
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

#define APPFILENAMEFILTER _T("HTML files (*.*htm[l])|*.*htm*|XML files (*.xml)|*.xml|MathML Files (*.mml)|*.mml|SVG files (*.svg)|*.svg|CSS files (*.css)|*.css|All files (*.*)|*.*")
#define APPCSSNAMEFILTER _T("CSS files (*.css)|*.css|All files (*.*)|*.*")
#define APPSVGNAMEFILTER _T("SVG files (*.svg)|*.svg|All files (*.*)|*.*")
#define APPMATHNAMEFILTER _T("MathML files (*.mml)|*.mml|All files (*.*)|*.*")
#define APPHTMLNAMEFILTER _T("HTML files (*.*htm[l])|*.*htm*|All files (*.*)|*.*")
#define APPIMAGENAMEFILTER _T("All files (*.*)|*.*|Image files (*.png)|*.png|Image files (*.jpg)|*.jpg|Image files (*.gif)|*.gif|Image files (*.bmp)|*.bmp")
#define APPLIBRARYNAMEFILTER _T("Library files (*.lhtml)|*.lhtml|All files (*.*)|*.*")
#define APPALLFILESFILTER _T("All files (*.*)|*.*")

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
ThotBool CreateOpenDocDlgWX ( int ref,
			      ThotWindow parent,
			      const char *title,
			      const char *urlList,
			      const char *urlToOpen,
			      const char *docName,
			      int doc_select,
			      int dir_select,
			      DocumentType doc_type )
{
#ifdef _WX
  wxString wx_title   = TtaConvMessageToWX( title );
  wxString wx_docName = TtaConvMessageToWX( docName );
  wxString wx_filter;

  if (doc_type == docHTML)
    wx_filter = APPHTMLNAMEFILTER;
  else if (doc_type == docMath)
    wx_filter = APPMATHNAMEFILTER;
  else if (doc_type == docSVG)
    wx_filter = APPSVGNAMEFILTER;
  else if (doc_type == docCSS)
    wx_filter = APPCSSNAMEFILTER;
  else if (doc_type == docImage)
    wx_filter = APPIMAGENAMEFILTER;
  else if (doc_type == docImage)
    wx_filter = APPIMAGENAMEFILTER;
  else if (doc_type == docLibrary)
    wx_filter = APPLIBRARYNAMEFILTER;
  else 
    wx_filter = APPFILENAMEFILTER;


  wxLogDebug( _T("CreateOpenDocDlgWX - title=")+wx_title+
	      _T("\tdocName=")+wx_docName+
	      _T("\tfilter=")+wx_filter );

  OpenDocDlgWX * p_dlg = new OpenDocDlgWX( ref,
					   parent,
					   wx_title,
					   wx_docName,
					   wx_filter );

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
  wxString firsturl  = TtaConvMessageToWX( urlList );
  wxString wx_urlToOpen = TtaConvMessageToWX( urlToOpen );
  p_dlg->SetCurrentURL( wx_urlToOpen/*firsturl*/ );
  /* ---------------------------------------------------------- */

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    {
      /* the dialog has been sucesfully registred */
      return TRUE;
    }
  else
    {
      /* an error occured durring registration */
      p_dlg->Destroy();
      return FALSE;
    }
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CreateTitleDlgWX create the dialog to change the document title
  params:
    + doc_title : the current document title
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateTitleDlgWX ( int ref,
			    ThotWindow parent,
			    char *doc_title )
{
#ifdef _WX
  wxString wx_title     = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_CHANGE_TITLE) );
  wxString wx_doc_title = TtaConvMessageToWX( doc_title );

  wxLogDebug( _T("TitlelgWX - title=")+wx_title+
	      _T("\tdoc_title=")+wx_doc_title );

  TitleDlgWX * p_dlg = new TitleDlgWX( ref,
				       parent,
				       wx_title,
				       wx_doc_title );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    {
      /* the dialog has been sucesfully registred */
      return TRUE;
    }
  else
    {
      /* an error occured durring registration */
      p_dlg->Destroy();
      return FALSE;
    }
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CreateSearchDlgWX create the Search dialog
  params:
    + caption : the widget caption (including document title)
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateSearchDlgWX ( int ref, ThotWindow parent,  char* caption)
{
#ifdef _WX
  wxString wx_caption = TtaConvMessageToWX( caption );

  wxLogDebug( _T("SearchDlgWX - caption=")+wx_caption );

  SearchDlgWX * p_dlg = new SearchDlgWX( ref,
					 parent,
					 wx_caption );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    {
      /* the dialog has been sucesfully registred */
      return TRUE;
    }
  else
    {
      /* an error occured durring registration */
      p_dlg->Destroy();
      return FALSE;
    }
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CreatePrintDlgWX create the SearchSetupAndPrint dialog
  params:
    + ps_file : postscript file
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreatePrintDlgWX ( int ref, ThotWindow parent,  char* ps_file)
{
#ifdef _WX
  wxString wx_ps_file = TtaConvMessageToWX( ps_file );

  wxLogDebug( _T("PrintDlgWX - ps_file=")+wx_ps_file );

  PrintDlgWX * p_dlg = new PrintDlgWX( ref,
				       parent,
				       wx_ps_file );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    {
      /* the dialog has been sucesfully registred */
      return TRUE;
    }
  else
    {
      /* an error occured durring registration */
      p_dlg->Destroy();
      return FALSE;
    }
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CreateSaveAsDlgWX create the SaveAs dialog
  params:
    + pathname : file location
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateSaveAsDlgWX ( int ref, ThotWindow parent,
			     char* pathname, int doc)
{
#ifdef _WX
  wxString wx_pathname = TtaConvMessageToWX( pathname );

  wxLogDebug( _T("SaveAsDlgWX - pathname=")+wx_pathname );

  SaveAsDlgWX * p_dlg = new SaveAsDlgWX( ref,
					 parent,
					 wx_pathname,
					 doc );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    {
      /* the dialog has been sucesfully registred */
      return TRUE;
    }
  else
    {
      /* an error occured durring registration */
      p_dlg->Destroy();
      return FALSE;
    }
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CreateCSSDlgWX create the open/enable/disable.. css files
  params:
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateCSSDlgWX( int ref,
			 ThotWindow parent,
			 int nb_item,
			 char *items,
			 char *title)
{
#ifdef _WX
  wxString      wx_title = TtaConvMessageToWX( title );
  wxArrayString wx_items;
  
  /* build the css filename list */
  int i = 0;
  int index = 0;
  while (i < nb_item && items[index] != EOS)
    {
      wx_items.Add( TtaConvMessageToWX( &items[index] ) );
      index += strlen (&items[index]) + 1; /* one entry length */
      i++;
    }

  if ( nb_item <= 0 )
    {
      wxMessageDialog messagedialog( NULL,
				     TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_NO_CSS)), 
				     wx_title,
				     (long) wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
      messagedialog.ShowModal();
      return FALSE;
    }

  /* create the dialog */
  CSSDlgWX * p_dlg = new CSSDlgWX( ref,
				   parent,
				   wx_title,
				   wx_items );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    {
      /* the dialog has been sucesfully registred */
      return TRUE;
    }
  else
    {
      /* an error occured durring registration */
      p_dlg->Destroy();
      return FALSE;
    }
#else /* _WX */
  return FALSE;
#endif /* _WX */  
}

/*----------------------------------------------------------------------
  CreateDocInfoDlgWX create the Documnent Infos dialog
  ----------------------------------------------------------------------*/
ThotBool CreateDocInfoDlgWX ( int ref, ThotWindow parent, int doc)
{
#ifdef _WX
  wxLogDebug( _T("CreateDocInfoDlgWX") );

  DocInfoDlgWX * p_dlg = new DocInfoDlgWX( ref,
					   parent,
					   doc );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    {
      /* the dialog has been sucesfully registred */
      return TRUE;
    }
  else
    {
      /* an error occured durring registration */
      p_dlg->Destroy();
      return FALSE;
    }
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

/*-----------------------------------------------------------------------
 CreateHRefDlgWX
 Used to :
  - Add CSS file
  - Create/Modify a link
 ------------------------------------------------------------------------*/
ThotBool CreateHRefDlgWX ( int ref, ThotWindow parent,
			   const char *url_list,
			   const char *HRefValue,
			   int doc_select, int dir_select, int doc_type)
{
#ifdef _WX
  wxString wx_title      = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_ATTRIBUTE) );
  wxString wx_filter;
  wxString wx_init_value = TtaConvMessageToWX( HRefValue );

  if (doc_type == docCSS)
    wx_filter = APPCSSNAMEFILTER;
  else 
    wx_filter = APPFILENAMEFILTER;


  /* ------------------------- */
  /* Create the url list array */
  /* function will stop on double EOS */
  wxArrayString wx_items;
  int index = 0;
  while (url_list[index] != EOS)
    {
      wx_items.Add( TtaConvMessageToWX( &url_list[index] ) );
      index += strlen (&url_list[index]) + 1; /* one entry length */
    }
  /* ------------------------- */

  wxLogDebug( _T("CreateHRefDlgWX - title=")+wx_title+
	      _T("\tfilter=")+wx_filter );

  HRefDlgWX * p_dlg = new HRefDlgWX( ref,
				     parent,
				     wx_items,
				     wx_init_value,
				     wx_title,
				     wx_filter );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    {
      /* the dialog has been sucesfully registred */
      return TRUE;
    }
  else
    {
      /* an error occured durring registration */
      p_dlg->Destroy();
      return FALSE;
    }
#else /* _WX */
  return FALSE;
#endif /* _WX */
}
