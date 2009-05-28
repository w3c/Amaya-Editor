/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/string.h"

/*----------------------------------------------------------------------
  TtaConvMessageToWX - 
  this is a specific wxWidget function which convert amaya message string into 
  comprehensive wxWidgets wxString objects.
  By default, input messages have UTF8 charset.
  params:
    + p_message : the message to convert (UTF8 by default)
  returns:
    + wxString : a wxString object (specific to wxWidgets)
  ----------------------------------------------------------------------*/
wxString TtaConvMessageToWX( const char * p_message )
{
  /* For the moment p_message is supposed to be UTF-8.
   * If needed it's possible to add a conditionnal variable to choose the p_message encoding.
   * See TtaGetMessageTable to understand how MessageTable are filled (in which encoding) */
  if (p_message == NULL)
    return wxString(wxT(""));
  else
    return wxString( p_message, wxConvUTF8 );
}
#endif /* _WX */
