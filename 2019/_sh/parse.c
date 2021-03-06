#include "cell.h"
#include "chvec.h"
#include "parse.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>

static int
is_idchar(char c) {
	return isalnum(c) || c == '-' || c == '/' || c == ']' || c == '[';
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
	struct tr_object *list = tr_create_cell(
			tr_create_int(ussh_sycmd),
			tr_create_cell(tr_create_str(head), NULL));

	struct tr_object *tail = list->cdr;
	while (is_idchar(*p)) {
		struct chvec *str = parse_string(&p);
		tail->cdr = tr_create_cell(tr_create_str(str), NULL);
		while (isspace(*p)) { ++p; }
		tail = tail->cdr;
	}
	*str = p;
	return list;
}

struct tr_object *
parse_piped_cmds(char **str) {
	char *p = *str;

	struct tr_object *cmd1 = parse_str_list(&p);
	while (isspace(*p)) { ++p; }
	struct tr_object *pipe = tr_create_cell(
			tr_create_int(ussh_sypipe),
			tr_create_cell(cmd1, NULL));

	struct tr_object *tail = pipe->cdr;
	while (*p == '|') {
		++p;
		while (isspace(*p)) { ++p; }
		struct tr_object *cmd = parse_str_list(&p);
		while (isspace(*p)) { ++p; }
		tail->cdr = tr_create_cell(
				tr_create_int(ussh_sypipe),
				tr_create_cell(cmd, NULL));
		tail = tail->cdr->cdr;
	}

	*str = p;
	return pipe;
}

struct tr_object *
parse(char *str) {
	while (isspace(*str)) { ++str; }
	struct tr_object *list = parse_piped_cmds(&str);
	return list;
}
