	{ Translation Schema de structure for MathML.

	  I. Vatton			June 1997	}


TRANSLATION MathML;

LineLength 78;
	
RULES

   MathML:
	begin
	create '<mrow>';
	create '</mrow>' after;
	end;

   MROW:
	begin
	create '<mrow>';
	create '</mrow>' after;
	end;

   MROOT :
        begin
	create '<mroot>';
	create '</mroot>' after;
  	end;

   MSQRT :
        begin
	create '<msqrt>';
	create '</msqrt>' after;
  	end;

   MN :
	begin
	create '<mn>';
	create '</mn>' after;
	end;
	
   MFRAC :
	begin
	create '<mfrac>';
	create '</mfrac>' after;
	end;

   MSUBSUP :
	begin
	create '<msubsup>';
	create '</msubsup>' after;
	end;

   MSUB :
	begin
	create '<msub>';
	create '</msub>' after;
	end;

   MSUP :
	begin
	create '<msup>';
	create '</msup>' after;
	end;

   MFENCE :
	begin
	create '<mf>';
	create '</mf>' after;
	end;

   TEXT_UNIT :
	begin
	create '<mtext>';
	create '</mtext>' after;
	end;

   SYMBOL_UNIT :
	begin
	if NOT Within MFENCE
		begin
		create '<mo>';
		create '</mo>' after;
		end;
	end;

#include "Text_SGML.inc"
SYMBTRANSLATE
        BEGIN
	'i'     -> '&Integral;';
        'S'     -> '&Sum;';
        'P'     -> '&;';
        'U'     -> '&Union;';
        'I'     -> '&Intersection;';
        END;

END
