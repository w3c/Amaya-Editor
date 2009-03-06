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

%{
int yylex (void);

void yyerror(const char *s)
{
  fprintf (stderr, "%s\n", s);
}

%}

%union {
  Element node;
  char *string;
}

%token<string> ATOM
%token<string> INTEGER
%token<string> SIGN
%token<string> UNKNOWN_TOKEN

%type<node> atom
%type<node> integer
%type<node> sign

%type<node> list_of_formulae
%type<node> chemical_formula
%type<node> chemical_formula2
%type<node> exponent

%start result

%destructor {
  printf("Element deleted\n");
  TtaDeleteTree($$, parser_doc);
 } atom integer sign list_of_formulae chemical_formula chemical_formula2 exponent

%%

result:
  list_of_formulae
  {
    parser_new_el = $1;
  }
;

list_of_formulae
: list_of_formulae chemical_formula
  {
    Element leaf;
    leaf = TtaGetLastChild($1);
    TtaInsertSibling($2, leaf, FALSE, parser_doc);
    $$ = $1;
  }

| chemical_formula
  {
    $$ = ParserNewMROW(parser_doc);
    TtaInsertFirstChild(&($1), $$, parser_doc);
  }
;

chemical_formula
: chemical_formula2 { $$ = $1}
| chemical_formula2 integer { $$ = ParserNewMSUB(parser_doc, $1, $2); }
;

chemical_formula2:
  atom { $$ = $1 }
| '(' list_of_formulae ')' { $$ = $2; }
| '[' list_of_formulae ']' { $$ = $2; }
| chemical_formula2 exponent { $$ = ParserNewMSUP(parser_doc, $1, $2); }

exponent:
integer sign
  {
    $$ = ParserNewMROW(parser_doc);
    TtaInsertFirstChild(&($1), $$, parser_doc);
    TtaInsertSibling($2, $1, FALSE, parser_doc);
  }

| sign { $$ = $1; }
;

integer:
INTEGER { $$ = ParserNewMathElement(parser_doc, $1, MathML_EL_MN); }

sign:
SIGN { $$ = ParserNewMathElement(parser_doc, $1, MathML_EL_MO); }

atom:
ATOM { $$ = ParserNewMathElement(parser_doc, $1, MathML_EL_MI); }

%%

#include "lex.yy.c"
