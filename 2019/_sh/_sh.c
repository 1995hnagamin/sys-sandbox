#include "chvec.h"
#include <stdio.h>
#include <stdlib.h>

char **
split_string_by_space(char *str, size_t len) {
	size_t cnt = 0;
	for (char *p = str, *end = str + len; p != end; ++p) {
		if (*p == ' ') { ++cnt; }
	}
	char **slist = malloc(sizeof(char *) * (cnt+2));
	*slist = str;
	*(slist+cnt+1) = NULL;
	char **cur = slist + 1;
	for (char *p = str, *end = str + len; p != end; ++p) {
		if (*p == ' ') {
			*cur = ++p;
			++cur;
		}
	}
	for (char **p = slist + 1; *p != NULL; ++p) {
		*(*p - 1) = '\0';
	}
	return slist;
}

struct chvec *
ussh_read_line(void) {
	struct chvec *cv = chvec_create(10);
	for (;;) {
		char c = getchar();
		if (c == EOF || c == '\n') {
			chvec_push_back(cv, '\0');
			break;
		}
		chvec_push_back(cv, c);
	}
	return cv;
}

void
ussh_repl(void) {
	for (;;) {
		printf("$ ");
		struct chvec *cv = ussh_read_line();
		char **slist = split_string_by_space(chvec_ptr(cv), chvec_size(cv));
		for (char **p = slist; *p != NULL; ++p) {
			printf("%s\n", *p);
		}
		free(slist);
		chvec_free(cv);
	}
}

int
main(int argc, char* argv[]) {
	ussh_repl();
	return 0;
}
