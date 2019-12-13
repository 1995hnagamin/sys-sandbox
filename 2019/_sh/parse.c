#include "cell.h"
#include "chvec.h"
#include <ctype.h>

static int
is_idchar(char c) {
	return isalnum(c) || c == '-' || c == '/';
}

struct chvec *
parse_string(char **str) {
	char *p = *str;
	struct chvec *cv = chvec_create(10);
	while (is_idchar(*p)) {
		chvec_push_back(cv, *p);
		++p;
	}
	chvec_push_back(cv, '\0');
	*str = p;
	return cv;
}

struct tr_object *
parse_str_list(char **str) {
	char *p = *str;

	struct chvec *head = parse_string(&p);
	while (isspace(*p)) { ++p; }
	struct tr_object *list = tr_create_cell(tr_create_str(head), NULL);

	struct tr_object *tail = list;
	while (is_idchar(*p)) {
		struct chvec *str = parse_string(&p);
		tail->cell.cdr = tr_create_cell(tr_create_str(str), NULL);
		while (isspace(*p)) { ++p; }
		tail = tail->cell.cdr;
	}
	*str = p;
	return list;
}

struct tr_object *
parse(char *str) {
	struct tr_object *list = parse_str_list(&str);
	return list;
}
