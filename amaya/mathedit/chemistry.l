/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: F. Wang
 */         

%option nounput
%option noyywrap

%%

H|D|T|He|Li|Be|B|C|N|O|F|Ne|Na|Mg|Al|Si|P|S|Cl|Ar|K|Ca|Sc|Ti|V|Cr|Mn|Fe|Co|Ni|Cu|Zn|Ga|Ge|As|Se|Br|Kr|Rb|Sr|Y|Zr|Nb|Mo|Tc|Ru|Rh|Pd|Ag|Cd|In|Sn|Sb|Te|I|Xe|Cs|Ba|La|Ce|Pr|Nd|Pm|Sm|Eu|Gd|Tb|Dy|Ho|Er|Tm|Yb|Lu|Hf|Ta|W|Re|Os|Ir|Pt|Au|Hg|Tl|Pb|Bi|Po|At|Rn|Fr|Ra|Ac|Th|Pa|U|Np|Pu|Am|Cm|Bk|Cf|Es|Fm|Md|No|Lr|Rf|Db|Sg|Bh|Hs|Mt|Ds|Rg|Uub|Uut|Uuq|Uup|Uuh|Uus|Uuo {
 yylval.string = yytext;
 return ATOM_Y; }

(\+|\-)?[I|V|X]+(\/[I|V|X]+)?|0 {
 yylval.string = yytext; return OXIDATION_NUMBER; }

\^\{((\+|\-)?[I|V|X]+(\/[I|V|X]+)?|0)\} {
 yytext[yyleng-1] = '\0';
 yylval.string = yytext+2; return OXIDATION_NUMBER; }

[1-9][0-9]* { yylval.string = yytext; return INTEGER; }

\=\=|---- { return QUADRUPLE_BOND; }
-\=|\=-|--- { return TRIPLE_BOND; }
\=|-- { return DOUBLE_BOND; }
_ { return SIMPLE_BOND; }
\.\.\. { return GENERIC_BOND; }

\. { return DOT; }

\]|\[|\)|\(|\{|\}|\-|\+|^|\* { return *yytext; }

. { return UNKNOWN_TOKEN;}

%%
