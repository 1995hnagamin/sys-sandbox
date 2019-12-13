#ifndef PARSE_H
#define PARSE_H

enum ussh_syn_kind {
	ussh_syinvalid = 0,
	ussh_sycmd,
	ussh_sypipe,
};

struct tr_object *parse(char *str);

#endif /* !PARSE_H */
