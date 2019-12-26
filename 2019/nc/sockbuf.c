#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include "sockbuf.h"

int max(int x, int y) {
	return x > y ? x : y;
}

void
sockbuf_init(struct sockbuf *p, int infd, int outfd, char *buf, size_t sz) {
	p->infd = infd;
	p->outfd = outfd;
	p->begin = buf;
	p->cap = sz;
	p->cur = buf;
	p->end = buf;
}

int
sockbuf_empty(struct sockbuf *sb) {
	return sb->cur == sb->end;
}

void
setup_rwset(fd_set *rset, fd_set *wset, struct sockbuf *sbarray, size_t sz) {
	FD_ZERO(rset);
	FD_ZERO(wset);
	struct sockbuf *const end = sbarray + sz;
	for (struct sockbuf *p = sbarray; p != end; ++p) {
		if (sockbuf_empty(p)) {
			FD_SET(p->infd, rset);
		} else {
			FD_SET(p->outfd, wset);
		}
	}
}

void
handle_connection(int connfd) {
	int const maxfdp1 = 1 + max(fileno(stdout), connfd);
	size_t const bufsz = 10;
	char sendbuf[bufsz], recbuf[bufsz];
	struct sockbuf sbs[2];
	sockbuf_init(sbs, fileno(stdin), connfd, sendbuf, bufsz);
	sockbuf_init(sbs+1, connfd, fileno(stdout), recbuf, bufsz);

	for (;;) {
		fd_set rset, wset;
		setup_rwset(&rset, &wset, sbs, 2);

		select(maxfdp1, &rset, &wset, NULL, NULL);
		for (struct sockbuf *p = sbs; p != sbs + 2; ++p) {
			if (FD_ISSET(p->infd, &rset)) {
				p->cur = p->begin;
				ssize_t res = read(p->infd, p->begin, p->cap);
				if (res <= 0) { return; }
				p->end = p->begin + res;
			}
			if (FD_ISSET(p->outfd, &wset)) {
				size_t len = p->end - p->cur;
				ssize_t res = write(p->outfd, p->cur, len);
				if (res <= 0) { return; }
				p->cur += res;
			}
		}
	}
}
