
#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "amaya.h"

#include "appdialogue_wx.h"
#include "message_wx.h"
#include "wxdialog/file_filters.h"

#ifdef _WX
  #include "wxdialog/AuthentDlgWX.h"
  #include "wxdialog/BgImageDlgWX.h"
  #include "wxdialog/CheckedListDlgWX.h"
  #include "wxdialog/CreateTableDlgWX.h"
  #include "wxdialog/DocInfoDlgWX.h"
  #include "wxdialog/EnumListDlgWX.h"
  #include "wxdialog/HRefDlgWX.h"
  #include "wxdialog/ImageDlgWX.h"
  #include "wxdialog/InitConfirmDlgWX.h"
  #include "wxdialog/ListDlgWX.h"
  #include "wxdialog/ListEditDlgWX.h"
  #include "wxdialog/NumDlgWX.h"
  #include "wxdialog/ObjectDlgWX.h"
  #include "wxdialog/OpenDocDlgWX.h"
  #include "wxdialog/PreferenceDlgWX.h"
  #include "wxdialog/PrintDlgWX.h"
  #include "wxdialog/SaveAsDlgWX.h"
  #include "wxdialog/SearchDlgWX.h"
  #include "wxdialog/SpellCheckDlgWX.h"
  #include "wxdialog/TextDlgWX.h"
  #include "wxdialog/TitleDlgWX.h"
#endif /* _WX */

// this global is used to remember the last filter when using a filebrowser
int g_Last_used_filter = 0;

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef _WX
wxArrayString BuildWX_URL_List( const char * url_list )
{
  /* Create the url list array */
  /* will stop on double EOS */
  wxArrayString wx_items;
  int index = 0;
  while (url_list[index] != EOS)
    {
      wx_items.Add( TtaConvMessageToWX( &url_list[index] ) );
      index += strlen (&url_list[index]) + 1; /* one entry length */
    }
  return wx_items;
}
#endif /* _WX */

/*----------------------------------------------------------------------
  CreateInitConfirmDlgWX create the dialog for document changes
  (save/not save) comfirmation.
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
				  char *label,
				  char *label2,
				  char *label3 )
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_label = TtaConvMessageToWX( label );
  wxString wx_label2 = TtaConvMessageToWX( label2 );
  wxString wx_label3 = TtaConvMessageToWX( label3 );
  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_LIB_CONFIRM) );
  wxString wx_extrabutton;
  wxString wx_confirmbutton;

  if (extrabutton && extrabutton[0] != EOS)
    /* a message with 3 buttons */
    wx_extrabutton = TtaConvMessageToWX( extrabutton );
  else
    /* just 2 buttons */
    wx_extrabutton = TtaConvMessageToWX( "" );

  if (confirmbutton && confirmbutton[0] != EOS)
    wx_confirmbutton = TtaConvMessageToWX( confirmbutton );
  else
    wx_confirmbutton = TtaConvMessageToWX( "" );

  InitConfirmDlgWX * p_dlg = new InitConfirmDlgWX(
      ref, /* thotlib catalog reference */
      parent, /* parent window */
      wx_title, /* title */
      wx_extrabutton,
      wx_confirmbutton,
      wx_label, wx_label2, wx_label3 ); /* message label2 */

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
ThotBool CreateOpenDocDlgWX ( int ref, ThotWindow parent,
			      const char *title,
			      const char *urlList,
			      const char *urlToOpen,
			      const char *docName,
			      int doc_select,
			      int dir_select,
			      DocumentType doc_type,
			      ThotBool newfile)
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_title     = TtaConvMessageToWX( title );
  wxString wx_docName   = TtaConvMessageToWX( docName );
  wxString wx_urlToOpen = TtaConvMessageToWX( urlToOpen );
  wxString wx_filter;
  wxString wx_profiles = _T("");

  if (doc_type == docHTML)
    {
    wx_filter = APPHTMLNAMEFILTER;
    if (newfile)
      /* create a new HTML document: activat the list of profiles */
      wx_profiles = _T("XHTML 1.1");
    }
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

  OpenDocDlgWX * p_dlg = new OpenDocDlgWX( ref, parent,
					   wx_title,
					   wx_docName,
					   BuildWX_URL_List(urlList),
					   wx_urlToOpen,
					   wx_filter,
					   &g_Last_used_filter,
					   wx_profiles);

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  CreateImageDlgWX create the dialog for creating new image
  params:
    + parent : parent window
    + title : dialog title
    + urlToOpen : suggested url
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateImageDlgWX ( int ref, ThotWindow parent,
			    const char *title,
			    const char *urlToOpen,
			    const char *alt)
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_title   = TtaConvMessageToWX( title );
  wxString wx_urlToOpen = TtaConvMessageToWX( urlToOpen );
  wxString wx_alt = TtaConvMessageToWX( alt );
  wxString wx_filter = APPIMAGENAMEFILTER;

  ImageDlgWX * p_dlg = new ImageDlgWX( ref,
				       parent,
				       wx_title,
				       wx_urlToOpen,
				       wx_alt,
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

/*----------------------------------------------------------------------
  CreateObjectDlgWX create the dialog for creating new object
  params:
    + parent : parent window
    + title : dialog title
    + urlToOpen : suggested url
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateObjectDlgWX ( int ref, ThotWindow parent,
			     const char *title,
			     const char *urlToOpen,
			     const char *type)
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_title   = TtaConvMessageToWX( title );
  wxString wx_urlToOpen = TtaConvMessageToWX( urlToOpen );
  wxString wx_type = TtaConvMessageToWX( type );
  wxString wx_filter = APPIMAGENAMEFILTER;

  ObjectDlgWX * p_dlg = new ObjectDlgWX( ref,
					 parent,
					 wx_title,
					 wx_urlToOpen,
					 wx_type,
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

/*----------------------------------------------------------------------
  CreateTitleDlgWX create the dialog to change the document title
  params:
    + doc_title : the current document title
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateTitleDlgWX ( int ref, ThotWindow parent,
			    char *doc_title )
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_title     = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_CHANGE_TITLE) );
  wxString wx_doc_title = TtaConvMessageToWX( doc_title );

  TitleDlgWX * p_dlg = new TitleDlgWX( ref,
				       parent,
				       wx_title,
				       wx_doc_title );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
    + caption : the caption (including the document title)
    + searched : the initial searched string
    + replace : the  initial replace string
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateSearchDlgWX ( int ref, ThotWindow parent,  char* caption,
			     char* searched,  char* replace,
			     ThotBool withReplace, ThotBool searchAfter)
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_caption = TtaConvMessageToWX( caption );
  wxString wx_searched = TtaConvMessageToWX( searched );
  wxString wx_replace = TtaConvMessageToWX( replace );

  SearchDlgWX * p_dlg = new SearchDlgWX( ref,
					 parent,
					 wx_caption,
					 wx_searched,
					 wx_replace,
					 withReplace,
					 searchAfter );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    /* the dialog has been sucesfully registred */
    return TRUE;
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
ThotBool CreatePrintDlgWX ( int ref, ThotWindow parent,
			    char* printer_file,
			    char* ps_file,
			    int paper_format,
			    int orientation,
			    int disposition,
			    int paper_print,
			    ThotBool manual_feed,
			    ThotBool with_toc,
			    ThotBool with_links,
			    ThotBool with_url,
			    ThotBool ignore_css )
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_printer_file = TtaConvMessageToWX( printer_file );
  wxString wx_ps_file = TtaConvMessageToWX( ps_file );

  PrintDlgWX * p_dlg = new PrintDlgWX( ref,
				       parent,
				       wx_printer_file,
				       wx_ps_file,
				       paper_format,
				       orientation,
				       disposition,
				       paper_print,
				       manual_feed,
				       with_toc,
				       with_links,
				       with_url,
				       ignore_css );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_pathname = TtaConvMessageToWX( pathname );
  SaveAsDlgWX * p_dlg = new SaveAsDlgWX( ref,
					 parent,
					 wx_pathname,
					 doc );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  CreateSaveObject create the SaveAs dialog
  params:
    + pathname : file location
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateSaveObject ( int ref, ThotWindow parent, char* objectname)
{
#ifdef _WX
  // Create a generic filedialog
  wxFileDialog * p_dlg = new wxFileDialog
    (
     parent,
     TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OBJECT_LOCATION) ),
     _T(""),
     TtaConvMessageToWX( objectname ),
     _T("*.*"),
     wxSAVE | wxCHANGE_DIR /* wxCHANGE_DIR -> remember the last directory used. */
     );

  // do not force the directory, let wxWidgets choose for the current one
  // p_dlg->SetDirectory(wxGetHomeDir());
  if (p_dlg->ShowModal() == wxID_OK)
    {
      wxString url = p_dlg->GetPath();
      // allocate a temporary buffer to copy the 'const char *' url buffer 
      wxASSERT( url.Len() < 512 );
      strcpy( SavePath, (const char*)url.mb_str(wxConvUTF8) );
      // destroy the dlg before calling thotcallback because it's a child of this
      // dialog and thotcallback will delete the dialog...
      // so if I do not delete it manualy here it will be deleted twice
      p_dlg->Destroy();
      // create or load the new document
      ThotCallback (ref, INTEGER_DATA, (char*)1);
    }
  else
    {
      p_dlg->Destroy();
      ThotCallback (ref, INTEGER_DATA, (char*)0);
    }
  return TRUE;
#else /* _WX */
  return FALSE;
#endif /* _WX */
}

/*----------------------------------------------------------------------
  CreateAuthentDlgWX create the Authentification dialog
  params:
    + pathname : file location
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateAuthentDlgWX ( int ref, ThotWindow parent,
			     char *auth_realm, char *server)
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  AuthentDlgWX * p_dlg = new AuthentDlgWX( ref,
					   parent,
					   auth_realm, server);
  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  CreateCSSDlgWX creates the open/enable/disable.. css files
  params:
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateCSSDlgWX( int ref, int subref, ThotWindow parent, char *title,
			 int nb_item, char *items)
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

  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  /* create the dialog */
  ListDlgWX * p_dlg = new ListDlgWX( ref, subref, parent,
				     wx_title,
				     wx_items );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  CreateListDlgWX proposes 
  params:
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateListDlgWX( int ref, int subref, ThotWindow parent, char *title,
			  int nb_item, char *items)
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

  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  /* create the dialog */
  ListDlgWX * p_dlg = new ListDlgWX( ref, subref, parent,
				     wx_title,
				     wx_items );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  CreateCheckedListDlgWX creates the open/enable/disable.. css files
  params:
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateCheckedListDlgWX( int ref, ThotWindow parent, char *title,
				 int nb_item, char *items, ThotBool *checks)
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

  if ( nb_item )
    {
      /* check if the dialog is alredy open */
      if (TtaRaiseDialogue (ref))
	return FALSE;

      /* create the dialog */
      CheckedListDlgWX * p_dlg = new CheckedListDlgWX( ref,
						       parent,
						       wx_title,
						       nb_item,
						       wx_items,
						       (bool *)checks);
      
      if ( TtaRegisterWidgetWX( ref, p_dlg ) )
	/* the dialog has been sucesfully registred */
	return TRUE;
      else
	/* an error occured durring registration */
	p_dlg->Destroy();
    }
  return FALSE;
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
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  DocInfoDlgWX * p_dlg = new DocInfoDlgWX( ref,
					   parent,
					   doc );
  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
ThotBool CreateHRefDlgWX ( int ref,
			   ThotWindow parent,
			   const char *url_list,
			   const char *HRefValue,
			   int doc_type)
{
#ifdef _WX
  wxString wx_title      = TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_ATTRIBUTE) );
  wxString wx_init_value = TtaConvMessageToWX( HRefValue );
  wxString wx_filter;
  if (doc_type == docCSS)
    wx_filter = APPCSSNAMEFILTER;
  else 
    wx_filter = APPFILENAMEFILTER;
  wxArrayString wx_items = BuildWX_URL_List(url_list);

  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  HRefDlgWX * p_dlg = new HRefDlgWX( ref,
				     parent,
				     wx_title,
				     wx_items,
				     wx_init_value,
				     wx_filter );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  ImageDlgWX create the dialog for enter a text (generic)
  params:
    + parent : parent window
    + title : dialog title
    + value : init value
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateTextDlgWX ( int ref, int subref, ThotWindow parent,
			   const char *title, const char *label,
			   const char *value )
{
#ifdef _WX
  wxString wx_title   = TtaConvMessageToWX( title );
  wxString wx_label   = TtaConvMessageToWX( label );
  wxString wx_value = TtaConvMessageToWX( value );

  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  TextDlgWX * p_dlg = new TextDlgWX( ref, subref, parent,
				     wx_title,
				     wx_label,
				     wx_value );

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
 CreateCreateTableDlgWX
 params: nb of cols, nb of rows, border attribute
 ------------------------------------------------------------------------*/
ThotBool CreateCreateTableDlgWX ( int ref, ThotWindow parent,
				  int def_cols, int def_rows, int def_border)
{
#ifdef _WX

  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_title = TtaConvMessageToWX( TtaGetMessage (LIB, TMSG_BUTTON_TABLE) );
  CreateTableDlgWX * p_dlg = new CreateTableDlgWX( ref,
						   parent,
						   def_cols,
						   def_rows,
						   def_border,
						   wx_title );
  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
 CreatePreferenceDlgWX
 Used to :
  - Change Amaya configuration options
 ------------------------------------------------------------------------*/
ThotBool CreatePreferenceDlgWX ( int ref, ThotWindow parent,
				 const char *url_list )
{
#ifdef _WX

  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxArrayString wx_items = BuildWX_URL_List(url_list);
  PreferenceDlgWX * p_dlg = new PreferenceDlgWX( ref,
						 parent,
						 wx_items);
  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
 CreateSpellCheckDlgWX
 Used to :
  - Create the Spell Checker Amaya dialog
 ------------------------------------------------------------------------*/
ThotBool CreateSpellCheckDlgWX ( int ref, int base, ThotWindow parent,
				 int checkingArea)
{
#ifdef _WX

  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  SpellCheckDlgWX * p_dlg = new SpellCheckDlgWX( ref, base, parent,
						 checkingArea);
  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  CreateBgImageDlgWX create the dialog for creating new background image
  params:
    + parent : parent window
    + title : dialog title
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateBgImageDlgWX ( int ref, ThotWindow parent, const char * urlToOpen, int RepeatValue )
{
#ifdef _WX
  /* check if the dialog is already open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString wx_urlToOpen = TtaConvMessageToWX( urlToOpen );
  
  BgImageDlgWX * p_dlg = new BgImageDlgWX( ref,
					   parent,
					   wx_urlToOpen,
					   RepeatValue );

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
  CreateListEditDlgWX proposes (generic way)
  params:
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateListEditDlgWX( int ref, ThotWindow parent,
			      const char *title, const char * list_title,
			      int nb_item, const char *items, const char * selected_item )
{
#ifdef _WX
  wxString      wx_title         = TtaConvMessageToWX( title );
  wxString      wx_selected_item = TtaConvMessageToWX( selected_item );
  wxString      wx_list_title    = TtaConvMessageToWX( list_title );
  wxArrayString wx_items;
  
  /* convert the (char *) list to wxArrayString */
  int i = 0;
  int index = 0;
  while (i < nb_item && items[index] != EOS)
    {
      wx_items.Add( TtaConvMessageToWX( &items[index] ) );
      index += strlen (&items[index]) + 1; /* one entry length */
      i++;
    }

  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  /* create the dialog */
  ListEditDlgWX * p_dlg = new ListEditDlgWX( ref, parent,
					     wx_title,
					     wx_list_title,
					     wx_items,
					     wx_selected_item );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  CreateEnumListDlgWX is a generic enum list
  params:
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateEnumListDlgWX( int ref, int subref, ThotWindow parent,
			      const char *title,
			      const char *label,
			      int nb_item,
			      const char *items,
			      int selection )
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString      wx_title = TtaConvMessageToWX( title );
  wxString      wx_label = TtaConvMessageToWX( label );
  wxArrayString wx_items;
  
  /* build the enum list strings */
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
      /* TODO: change the message when there is no items, should never occured */
      wxMessageDialog messagedialog( NULL,
				     TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_NO_CSS)),
				     wx_title,
				     (long) wxOK | wxICON_EXCLAMATION | wxSTAY_ON_TOP);
      messagedialog.ShowModal();
      return FALSE;
    }

  /* create the dialog */
  EnumListDlgWX * p_dlg = new EnumListDlgWX( ref, subref,
					     parent,
					     wx_title,
					     wx_label,
					     wx_items,
					     selection );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
      /* the dialog has been sucesfully registred */
      return TRUE;
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
  CreateNumDlgWX is a generic numeric dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
ThotBool CreateNumDlgWX( int ref, int subref, ThotWindow parent,
			  const char *title,
			  const char *label,
			  int value )
{
#ifdef _WX
  /* check if the dialog is alredy open */
  if (TtaRaiseDialogue (ref))
    return FALSE;

  wxString      wx_title = TtaConvMessageToWX( title );
  wxString      wx_label = TtaConvMessageToWX( label );
  
  /* create the dialog */
  NumDlgWX * p_dlg = new NumDlgWX( ref, subref,
				   parent,
				   wx_title,
				   wx_label,
				   value );

  if ( TtaRegisterWidgetWX( ref, p_dlg ) )
    /* the dialog has been sucesfully registred */
    return TRUE;
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
