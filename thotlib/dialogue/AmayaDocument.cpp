#ifdef _WX

#include "AmayaDocument.h"

AmayaDocument::AmayaDocument( wxWindow * p_parent ) : 
  wxPanel( p_parent,-1, wxDefaultPosition, wxDefaultSize )
{
  // Create the drawing area
  m_pCanvas = new wxPanel ( this );
  
  // Create vertical and horizontal scrallbars
  m_pScrollBarH = new wxScrollBar( this,
				   -1,
				   wxDefaultPosition,
				   wxDefaultSize,
				   wxSB_HORIZONTAL );
  m_pScrollBarV = new wxScrollBar( this,
				   -1,
				   wxDefaultPosition,
				   wxDefaultSize,
				   wxSB_VERTICAL );

  // Create a flexible sizer (first col and first row should be extensible)
  wxFlexGridSizer *pSizer = new wxFlexGridSizer(2,2);
  pSizer->AddGrowableCol(0);
  pSizer->AddGrowableRow(0);

  // Insert elements into sizer
  pSizer->Add( m_pCanvas, 1, wxEXPAND );
  pSizer->Add( m_pScrollBarV, 1, wxEXPAND );
  pSizer->Add( m_pScrollBarH, 1, wxEXPAND );    

  SetSizer(pSizer);
  
  SetAutoLayout(TRUE);

  // just for DEBUG
  m_pCanvas->SetBackgroundColour( wxT("RED") );
  SetBackgroundColour( wxT("GREEN") );  
}

AmayaDocument::~AmayaDocument()
{
}

BEGIN_EVENT_TABLE(AmayaDocument, wxPanel) 
END_EVENT_TABLE()

#endif /* #ifdef _WX */

