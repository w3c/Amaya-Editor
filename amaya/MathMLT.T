	{ Translation Schema de structure for MathML.

	  I. Vatton			June 1997	}


TRANSLATION MathML;

LineLength 78;
	
RULES

   MathML:
	begin
	create '<mrow>\12';
	indent +2;
	indent -2 after;
	create '</mrow>\12' after;
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

   MFENCE :
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
	if NOT Within MFENCE
		begin
		create '<mo>';
		create '</mo>\12' after;
		end;
	end;

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
