/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
   Management of external references files.

 */

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
#include "fileaccess_f.h"
#include "message_f.h"


/*----------------------------------------------------------------------
   ConvertIntToLabel converts integer num into a Thot label		
  ----------------------------------------------------------------------*/


#ifdef __STDC__
void                ConvertIntToLabel (int num, LabelString strng)

#else  /* __STDC__ */
void                ConvertIntToLabel (num, strng)
int                 num;
LabelString         strng;

#endif /* __STDC__ */

{
   sprintf (strng, "L%d", num);
}


/*----------------------------------------------------------------------
   ReadLabel reads a label from a file				
   		The label type is specified by labelType.		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ReadLabel (char labelType, LabelString label, BinFile file)

#else  /* __STDC__ */
void                ReadLabel (labelType, label, file)
char                labelType;
LabelString         label;
BinFile             file;

#endif /* __STDC__ */

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
		    TtaReadByte (file, &label[j]);
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
