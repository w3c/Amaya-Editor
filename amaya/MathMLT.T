	{ Translation Schema de structure for MathML.

	  I. Vatton			June 1997	}


TRANSLATION MathML;

LineLength 78;
	
RULES

   MathML:
	begin
	end;

   MROW:
	begin
	create '<mrow>\12';
	indent +2;
	indent -2 after;
	create '</mrow>\12' after;
	end;

   MROOT :
        begin
	create '<mroot>\12';
	indent +2;
	indent -2 after;
	create '</mroot>\12' after;
  	end;

   MSQRT :
        begin
	create '<msqrt>\12';
	indent +2;
	indent -2 after;
	create '</msqrt>\12' after;
  	end;

   MFRAC :
	begin
	create '<mfrac>\12';
	indent +2;
	indent -2 after;
	create '</mfrac>\12' after;
	end;

   MSUBSUP :
	begin
	create '<msubsup>\12';
	indent +2;
	indent -2 after;
	create '</msubsup>\12' after;
	end;

   MSUB :
	begin
	create '<msub>\12';
	indent +2;
	indent -2 after;
	create '</msub>\12' after;
	end;

   MSUP :
	begin
	create '<msup>\12';
	indent +2;
	indent -2 after;
	create '</msup>\12' after;
	end;

   MUNDER :
	begin
	create '<munder>\12';
	indent +2;
	indent -2 after;
	create '</munder>\12' after;
	end;

   MOVER :
	begin
	create '<mover>\12';
	indent +2;
	indent -2 after;
	create '</mover>\12' after;
	end;

   MUNDEROVER :
	begin
	create '<munderover>\12';
	indent +2;
	indent -2 after;
	create '</munderover>\12' after;
	end;

   MMULTISCRIPTS :
	begin
	create '<mmultiscripts>\12';
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
	create '<mf>';
	create '</mf>\12' after;
	end;

   MI :
	begin
	create '<mi>';
	create '</mi>\12' after;
	end;

   MO :
	begin
	create '<mo>';
	create '</mo>\12' after;
	end;

   MN :
	begin
	create '<mn>';
	create '</mn>\12' after;
	end;

   MS :
	begin
	create '<ms>';
	create '</ms>\12' after;
	end;

   MTEXT :
	begin
	create '<mtext>';
	create '</mtext>\12' after;
	end;


   TEXT_UNIT :
	begin
	end;

   SYMBOL_UNIT :
	begin
	if NOT Within MF
		begin
		create '<mo>';
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
        'P'     -> '&;';
        'U'     -> '&Union;';
        'I'     -> '&Intersection;';
        END;

END
