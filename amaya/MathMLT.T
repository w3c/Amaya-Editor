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
	create '<mrow';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mrow>\12' after;
	end;

   MROOT :
        begin
	create '<mroot';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mroot>\12' after;
  	end;

   MSQRT :
        begin
	create '<msqrt';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</msqrt>\12' after;
  	end;

   MFRAC :
	begin
	create '<mfrac';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mfrac>\12' after;
	end;

   MSUBSUP :
	begin
	create '<msubsup';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</msubsup>\12' after;
	end;

   MSUB :
	begin
	create '<msub';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</msub>\12' after;
	end;

   MSUP :
	begin
	create '<msup';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</msup>\12' after;
	end;

   MUNDER :
	begin
	create '<munder';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</munder>\12' after;
	end;

   MOVER :
	begin
	create '<mover';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mover>\12' after;
	end;

   MUNDEROVER :
	begin
	create '<munderover';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</munderover>\12' after;
	end;

   MMULTISCRIPTS :
	begin
	create '<mmultiscripts';
	create Attributes;
	create tagend;
	indent +2;
	indent -2 after;
	create '</mmultiscripts>\12' after;
	end;
   Construct :
	begin
	if immediately within MSubscript
	   create '<none/>\12';
	if immediately within MSuperscript
	   create '<none/>\12';
	end;
   PostscriptPair :
	if empty Remove;
   PrescriptPair :
	if empty Remove;
   PrescriptPairs :
	if not empty
	   create '<mprescripts/>\12' before;

   MF :
	begin
	create '<mf';
	create Attributes;
	create '>';
	create '</mf>\12' after;
	end;

   MI :
	begin
	create '<mi';
	create Attributes;
	create '>';
	create '</mi>\12' after;
	end;

   MO :
	begin
	create '<mo';
	create Attributes;
	create '>';
	create '</mo>\12' after;
	end;

   MN :
	begin
	create '<mn';
	create Attributes;
	create '>';
	create '</mn>\12' after;
	end;

   MS :
	begin
	create '<ms';
	create Attributes;
	create '>';
	create '</ms>\12' after;
	end;

   MTEXT :
	begin
	create '<mtext';
	create Attributes;
	create '>';
	create '</mtext>\12' after;
	end;


   TEXT_UNIT :
	begin
	end;

   SYMBOL_UNIT :
	begin
	if NOT Within MF
		begin
		create '<mo';
		create Attributes;
		create '>';
		create '</mo>\12' after;
		end;
	end;

ATTRIBUTES

   entity:
	create entity;

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
