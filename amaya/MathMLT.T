	{ Translation Schema de structure for MathML.

	  I. Vatton			June 1997	}


TRANSLATION MathML;

LineLength 78;

CONST
   tagend = '>\12';
	
RULES

   MathML:
	begin
	end;

   MROW:
	begin
	create '<MROW';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MROW>\12' after;
	end;

   MROOT :
        begin
	create '<MROOT';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MROOT>\12' after;
  	end;
   RootBase:
	if empty create '<NONE/>\12';
   Index:
	if empty create '<NONE/>\12';

   MSQRT :
        begin
	create '<MSQRT';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MSQRT>\12' after;
  	end;

   MFRAC :
	begin
	create '<MFRAC';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MFRAC>\12' after;
	end;
   Numerator:
	if empty create '<NONE/>\12';
   Denominator:
	if empty create '<NONE/>\12';


   MSUBSUP :
	begin
	create '<MSUBSUP';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MSUBSUP>\12' after;
	end;
   Base:
	if empty create '<NONE/>\12';
   Subscript:
	if empty create '<NONE/>\12';
   Superscript:
	if empty create '<NONE/>\12';

   MSUB :
	begin
	create '<MSUB';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MSUB>\12' after;
	end;

   MSUP :
	begin
	create '<MSUP';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MSUP>\12' after;
	end;

   MUNDER :
	begin
	create '<MUNDER';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MUNDER>\12' after;
	end;
   UnderOverBase:
	if empty create '<NONE/>\12';
   Underscript:
	if empty create '<NONE/>\12';

   MOVER :
	begin
	create '<MOVER';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MOVER>\12' after;
	end;
   Overscript:
	if empty create '<NONE/>\12';

   MUNDEROVER :
	begin
	create '<MUNDEROVER';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MUNDEROVER>\12' after;
	end;

   MMULTISCRIPTS :
	begin
	create '<MMULTISCRIPTS';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MMULTISCRIPTS>\12' after;
	end;
   MultiscriptBase:
	if empty create '<NONE/>\12';
   
   Construct :
	begin
	if immediately within MSubscript
	   create '<NONE/>\12';
	if immediately within MSuperscript
	   create '<NONE/>\12';
	end;
   PostscriptPair :
	if empty Remove;
   PrescriptPair :
	if empty Remove;
   PrescriptPairs :
	if not empty
	   create '<MPRESCRIPTS/>\12' before;

   MF :
	begin
	create '<MF';
	create Attributes;
	create '>';
	create '</MF>\12' after;
	end;

   MN :
	begin
	create '<MN';
	create Attributes;
	create '>';
	create '</MN>\12' after;
	end;

   MI :
	begin
	create '<MI';
	create Attributes;
	create '>';
	create '</MI>\12' after;
	end;

   MO :
	begin
	create '<MO';
	create Attributes;
	create '>';
	create '</MO>\12' after;
	end;

   MS :
	begin
	create '<MS';
	create Attributes;
	create '>';
	create '</MS>\12' after;
	end;

   MTEXT :
	begin
	create '<MTEXT';
	create Attributes;
	create '>';
	create '</MTEXT>\12' after;
	end;

   C_Empty:
	remove;

   SEP:
	create '<SEP/>\12';

   MSPACE:
	create '<MSPACE/>\12';

   MSTYLE:
	begin
	create '<MSTYLE';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MSTYLE>\12' after;
	end;

   MERROR:
	begin
	create '<MERROR';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MERROR>\12' after;
	end;

   MPHANTOM:
	begin
	create '<MPHANTOM';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</MPHANTOM>\12' after;
	end;

   TEXT_UNIT :
	begin
	end;

   SYMBOL_UNIT :
	begin
	if NOT Within MF
		begin
		create '<MO';
		create Attributes;
		create '>';
		create '</MO>\12' after;
		end;
	end;

ATTRIBUTES

   entity:
	begin
	notranslation;	{ do not translate & into &amp; }
	end;

#include "Text_SGML.inc"
#include "greek.sgml"
SYMBTRANSLATE
        BEGIN
	'i'     -> '&Integral;';
        'S'     -> '&Sum;';
        'P'     -> '&PI;';
        'U'     -> '&Cap;';
        'I'     -> '&Cup;';
        END;

END
