#include "cell.h"
#include "chvec.h"
#include "parse.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct chvec *
ussh_read_line(void) {
	struct chvec *cv = chvec_create(10);
	for (;;) {
		char c = getchar();
		if (c == EOF) {
			chvec_free(cv);
			return NULL;
		}
		if (c == '\n') {
			chvec_push_back(cv, '\0');
			break;
		}
		chvec_push_back(cv, c);
	}
	return cv;
}

void
ussh_exec(char **slist) {
	pid_t pid = fork();
	assert(pid != -1);
	if (pid != 0) {
		int status;
		wait(&status);
		return;
	}
	execvp(slist[0], slist);
	exit(EXIT_SUCCESS);
}

void
ussh_repl(void) {
	for (;;) {
		printf("$ ");
		struct chvec *cv = ussh_read_line();
		if (!cv) {
			printf("\n");
			return;
		}

		struct tr_object *list = parse(chvec_ptr(cv));
		size_t len = tr_list_length(list);
		char **slist = (char **)malloc(sizeof(char *) * (len + 1));
		slist[len] = NULL;
		struct tr_object *p = list;
		for (size_t i = 0; i < len; ++i, p = p->cell.cdr) {
			slist[i] = chvec_ptr(p->cell.car->cv);
		}

		ussh_exec(slist);
		free(slist);
		tr_free(list);
		chvec_free(cv);
	}
}

int
main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;
	ussh_repl();
	return 0;
}
