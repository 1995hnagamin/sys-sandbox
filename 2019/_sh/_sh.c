#include "cell.h"
#include "chvec.h"
#include "parse.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

size_t
count_ncmds(struct tr_object *p) {
	size_t cnt;
	for (cnt = 0; p != NULL; ++cnt, p = p->cdr->cdr) {
		struct tr_object *head = p->cdr->car->cdr->car;
		if (chvec_size(head->cv) == 0) {
			break;
		}
	}
	return cnt;
}

char ***
get_command_arr(struct tr_object *tree, size_t ncmds) {
	size_t sz = ncmds + 1;
	char ***cmds = (char ***)malloc(sizeof(char **) * sz);
	cmds[sz - 1] = NULL;
	struct tr_object *p = tree;
	for (int i = 0; p != NULL; ++i, p = p->cdr->cdr) {
		struct tr_object *list = p->cdr->car->cdr;
		size_t len = tr_list_length(list);
		cmds[i] = tr_list_to_sarr(list, len);
	}
	return cmds;
}

void
free_command_arr(char ***cmds) {
	for (char ***p = cmds; *p != NULL; ++p) {
		free(*p);
	}
	free(cmds);
}

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
setup_pipe(char ***cmds, size_t ncmds) {
	int bckpip[2], fwdpip[2];

	for (size_t i = 0; i < ncmds; ++i) {
		int ret = pipe(fwdpip);
		assert(ret != -1);
		pid_t pid = fork();
		assert(pid != -1);
		if (pid == 0) {
			/* ith children */
			signal(SIGINT, SIG_DFL);
			if (i < ncmds - 1) {
				int ofd = fileno(stdout);
				close(ofd);
				dup2(fwdpip[1], ofd);
				close_pipes(fwdpip);
			}
			if (i > 0) {
				int ifd = fileno(stdin);
				close(ifd);
				dup2(bckpip[0], ifd);
				close_pipes(bckpip);
			}
			execvp(cmds[i][0], cmds[i]);
			abort();
		}
		/* parent */
		if (i > 0) {
			close_pipes(bckpip);
		}
		bckpip[0] = fwdpip[0], bckpip[1] = fwdpip[1];
	}

	close_pipes(fwdpip);
	for (size_t i = 0; i < ncmds; ++i) {
		wait(NULL);
	}
}

void
ussh_repl(void) {
	signal(SIGINT, SIG_IGN);
	for (;;) {
		printf("$ ");
		struct chvec *cv = ussh_read_line();
		if (!cv) {
			printf("exit\n");
			return;
		}

		struct tr_object *list = parse(chvec_ptr(cv));
		fprintf(stderr, "* ");
		tr_dump(list);

		size_t len = count_ncmds(list);
		char ***cmds = get_command_arr(list, len);
		setup_pipe(cmds, len);
		free_command_arr(cmds);
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
