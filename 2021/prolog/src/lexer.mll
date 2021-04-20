{
open Parser
exception Eof
}
rule token = parse
  | [' ' '\t'] { token lexbuf }
  | ['a'-'z' 'A'-'Z']['a'-'z' 'A'-'Z' '_']* { ID(Lexing.lexeme lexbuf) }
  | '(' { LPAREN }
  | ')' { RPAREN }
  | ',' { COMMA }
  | '.' { DOT }
  | ":-" { COLONMINUS }
  | '\n' { EOL }
  | eof { raise Eof }
