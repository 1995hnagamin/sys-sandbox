#include "cell.h"
#include "chvec.h"
#include "parse.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void
close_pipes(int pipefd[2]) {
	close(pipefd[0]);
	close(pipefd[1]);
}

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
	for (char **p = slist; *p != NULL; ++p) {
		fprintf(stderr, "+ %s\n", *p);
	}
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
setup_pipe(struct tr_object *pair) {

	size_t len1 = tr_list_length(pair->car->cdr);
	char **slist1 = tr_list_to_sarr(pair->car->cdr, len1);
	size_t len2 = tr_list_length(pair->cdr->cdr);
	char **slist2 = tr_list_to_sarr(pair->cdr->cdr, len2);

	int pipefd[2];
	int ret = pipe(pipefd);
	assert(ret != -1);

	pid_t pid1 = fork();
	assert(pid1 != -1);
	if (pid1 == 0) {
		dup2(pipefd[1], fileno(stdout));
		close(pipefd[0]);
		close(pipefd[1]);
		for (char **p = slist1; *p != NULL; ++p) {
			fprintf(stderr, "+ %s\n", *p);
		}
		execvp(slist1[0], slist1);
		abort();
	}
	pid_t pid2 = fork();
	assert(pid2 != -1);
	if (pid2 == 0) {
		dup2(pipefd[0], fileno(stdin));
		close(pipefd[0]);
		close(pipefd[1]);
		for (char **p = slist2; *p != NULL; ++p) {
			fprintf(stderr, "- %s\n", *p);
		}
		execvp(slist2[0], slist2);
		abort();
	}

	fprintf(stderr, "I'm _sh\n");
	close(pipefd[0]);
	close(pipefd[1]);
	wait(NULL);
	wait(NULL);

	free(slist1);
	free(slist2);
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
		fprintf(stderr, "* ");
		tr_dump(list);

		setup_pipe(list);
		tr_free(list);
		chvec_free(cv);
		continue;

		size_t len = tr_list_length(list);
		char **slist = (char **)malloc(sizeof(char *) * (len + 1));
		slist[len] = NULL;
		struct tr_object *p = list;
		for (size_t i = 0; i < len; ++i, p = p->cdr) {
			slist[i] = chvec_ptr(p->car->cv);
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
