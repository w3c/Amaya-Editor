/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
   Management of external references files.

 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "constpiv.h"
#include "typemedia.h"
#include "fileaccess.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"

#include "tree_f.h"
#include "memory_f.h"
#include "references_f.h"
#include "readpivot_f.h"


/*----------------------------------------------------------------------
   ConvertIntToLabel converts integer num into a Thot label		
  ----------------------------------------------------------------------*/
void ConvertIntToLabel (int num, LabelString strng)
{
   sprintf (strng, "L%d", num);
}


/*----------------------------------------------------------------------
   ReadLabel reads a label from a file				
   		The label type is specified by labelType.		
  ----------------------------------------------------------------------*/
void ReadLabel (char labelType, LabelString label, BinFile file)
{
   int                 j, k;

   /* lit la valeur du label selon le type de label */
   switch (labelType)
	 {
	    case C_PIV_SHORT_LABEL:
	       if (TtaReadShort (file, &j))
		  ConvertIntToLabel (j, label);
	       else
		  /* error */
		  label[0] = EOS;
	       break;
	    case C_PIV_LONG_LABEL:
	       j = 0;
	       if (TtaReadShort (file, &j))
		  if (TtaReadShort (file, &k))
		     j = j * 65536 + k;
	       ConvertIntToLabel (j, label);
	       break;
	    case C_PIV_LABEL:
	       j = 0;
	       do
		 {
		    TtaReadByte (file, (unsigned char *)&label[j]);
		    /* drop last bytes if the label is too long */
		    if (j < MAX_LABEL_LEN)
		       j++;
		 }
	       while (label[j - 1] != EOS);
	       break;
	    default:
	       /*DisplayPivotMessage ("S")*/;
	       break;
	 }
}
