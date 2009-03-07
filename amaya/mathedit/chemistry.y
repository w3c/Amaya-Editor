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

%type<node> chemical_compound
%type<node> chemical_entity
%type<node> chemical_entity2
%type<node> chemical_entity3
%type<node> ionic_charge

%start result

%destructor {
  printf("Element deleted\n");
  TtaDeleteTree($$, parser_doc);
 } atom integer sign chemical_compound chemical_entity chemical_entity2 chemical_entity3 ionic_charge

%%

result
:chemical_compound
  {
    parser_new_el = $1;
  }
;

chemical_compound
: chemical_compound chemical_entity
  {
    Element leaf;
    leaf = TtaGetLastChild($1);
    TtaInsertSibling($2, leaf, FALSE, parser_doc);
    $$ = $1;
  }

| chemical_entity
  {
    $$ = ParserNewMROW(parser_doc);
    TtaInsertFirstChild(&($1), $$, parser_doc);
  }
;

chemical_entity
: chemical_entity3 ionic_charge { $$ = ParserNewMSUP(parser_doc, $1, $2); }
| chemical_entity2 { $$ = $1; }
;

chemical_entity2
:chemical_entity3 integer { $$ = ParserNewMSUB(parser_doc, $1, $2); }
| chemical_entity3 { $$ = $1; }
;

chemical_entity3
:  atom { $$ = $1 }

| '(' chemical_compound  ')'
  {
    $$ = ParserNewFencedExpression(parser_doc, $2, "(", ")");
  }

| '[' chemical_compound ']'
  {
    $$ = ParserNewFencedExpression(parser_doc, $2, "[", "]");
  }

| '{' chemical_compound '}' { $$ = $2; }
;

ionic_charge
:integer sign
  {
    $$ = ParserNewMROW(parser_doc);
    TtaInsertFirstChild(&($1), $$, parser_doc);
    TtaInsertSibling($2, $1, FALSE, parser_doc);
  }

| sign { $$ = $1; }
;

integer:
INTEGER { $$ = ParserNewMathElement(parser_doc, $1, MathML_EL_MN); }
;

sign:
SIGN { $$ = ParserNewMathElement(parser_doc, $1, MathML_EL_MO); }
;

atom:
ATOM { $$ = ParserNewMathElement(parser_doc, $1, MathML_EL_MI); }
;
%%

#include "lex.yy.c"
