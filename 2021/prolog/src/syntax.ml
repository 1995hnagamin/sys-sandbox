type id = string

type exp =
  | Var of id
  | Name of id

type prop =
  | Prop of exp * (exp list)

type rule =
  | Rule of prop * (prop list)
  | Fact of (prop list)
