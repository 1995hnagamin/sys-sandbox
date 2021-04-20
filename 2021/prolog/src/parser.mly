%{
open Syntax
%}
%token LPAREN RPAREN COMMA DOT COLONMINUS EOL

%token <Syntax.id> ID

%start main
%type <Syntax.rule> main

%%

main:
| rule EOL { $1 }

rule:
| prop COLONMINUS props DOT { Syntax.Rule($1, $3) }
| prop DOT { Syntax.Fact([$1]) }

props:
| prop { [$1] }
| prop COMMA props { $1::$3 }

prop:
| ID LPAREN arg_list RPAREN { Syntax.Prop(Syntax.Var($1), $3) }

arg_list:
| ID { [Syntax.Var($1)] }
| ID COMMA arg_list { Syntax.Var($1)::$3 }
