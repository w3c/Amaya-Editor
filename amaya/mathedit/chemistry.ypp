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

%token<string> ATOM_Y
%token<string> INTEGER
%token<string> OXIDATION_NUMBER
%token<string> SIMPLE_BOND
%token<string> DOUBLE_BOND
%token<string> TRIPLE_BOND
%token<string> QUADRUPLE_BOND
%token<string> GENERIC_BOND
%token<string> DOT
%token<string> UNKNOWN_TOKEN

%type<node> atom
%type<node> atom2
%type<node> integer
%type<node> oxidation_number
%type<node> sign

%type<node> chemical_compound
%type<node> chemical_compound2
%type<node> chemical_entity
%type<node> chemical_entity2
%type<node> ionic_charge

%type<node> bond
%type<node> simple_bond
%type<node> double_bond
%type<node> triple_bond
%type<node> quadruple_bond
%type<node> generic_bond

%type<node> dot

%start result
%expect 1

%destructor {
  printf("Element deleted\n");
  TtaDeleteTree($$, parser_doc);
 } atom atom2 integer oxidation_number sign chemical_compound chemical_compound2 chemical_entity chemical_entity2 ionic_charge bond simple_bond double_bond triple_bond quadruple_bond generic_bond dot
%%

result
:chemical_compound
  {
    parser_new_el = $1;
  }
;

chemical_compound
: chemical_compound2 ionic_charge { $$ = NewMSUP(parser_doc, $1, $2); }
| chemical_compound2 { $$ = $1; }
;

chemical_compound2
: chemical_compound2 chemical_entity
  {
    Element leaf;
    leaf = TtaGetLastChild($1);
    TtaInsertSibling($2, leaf, FALSE, parser_doc);
    $$ = $1;
  }

| chemical_entity
  {
    $$ = NewMROW(parser_doc);
    TtaInsertFirstChild(&($1), $$, parser_doc);
  }
;

chemical_entity
:chemical_entity2 integer { $$ = NewMSUB(parser_doc, $1, $2); }
| chemical_entity2 { $$ = $1; }
| '[' chemical_compound2 ']'
  {
    $$ = NewFencedExpression(parser_doc, $2, "[", "]");
  }
| bond { $$ = $1; }
| dot { $$ = $1; }
;

chemical_entity2
:  atom { $$ = $1 }

| '(' chemical_compound2  ')'
  {
    $$ = NewFencedExpression(parser_doc, $2, "(", ")");
  }

| '{' chemical_compound2 '}' { $$ = $2; }
;

ionic_charge
:integer sign
  {
    $$ = NewMROW(parser_doc);
    TtaInsertFirstChild(&($1), $$, parser_doc);
    TtaInsertSibling($2, $1, FALSE, parser_doc);
  }

| sign { $$ = $1; }
| '*' { $$ = NewSymbol(parser_doc, MathML_EL_MO, '*'); }
;

integer:
INTEGER { $$ = NewMathElement(parser_doc, MathML_EL_MN, $1); }
;

atom
:atom2 { $$ = $1; }
|atom2 oxidation_number { $$ = NewMSUP(parser_doc, $1, $2); }
;

atom2:
ATOM_Y { $$ = NewMathElement(parser_doc, MathML_EL_MI, $1); }
;

oxidation_number:
OXIDATION_NUMBER { $$ = NewMathElement(parser_doc, MathML_EL_MTEXT, $1); }
;

bond
: simple_bond { $$ = $1; }
| double_bond { $$ = $1; }
| triple_bond { $$ = $1; }
| quadruple_bond { $$ = $1; }
| generic_bond { $$ = $1; }

simple_bond
: SIMPLE_BOND { $$ = $$ = NewSymbol(parser_doc, MathML_EL_MO, '-');}

double_bond
: DOUBLE_BOND { $$ = NewSymbol(parser_doc, MathML_EL_MO, '=');}

triple_bond
: TRIPLE_BOND { $$ = NewSymbol(parser_doc, MathML_EL_MO, 0x2261);}

quadruple_bond
: QUADRUPLE_BOND { $$ = NewSymbol(parser_doc, MathML_EL_MO, 0x2263);}

generic_bond
: GENERIC_BOND { $$ = NewSymbol(parser_doc, MathML_EL_MO, 0x20dB); }

sign
: '+' { $$ = NewSymbol(parser_doc, MathML_EL_MO, '+'); }
| '-' { $$ = NewSymbol(parser_doc, MathML_EL_MO, 0x2212); }
;

dot
: DOT { $$ = NewSymbol(parser_doc, MathML_EL_MO, 0x00b7); }

%%

#include "chemistry.flex.c"
