#ifndef SOCKBUF_H
#define SOCKBUF_H

struct sockbuf {
	int infd;
	int outfd;

	char *begin;
	size_t cap;

	char *cur; // cur <= end
	char *end; // end <= begin + cap
};

void sockbuf_init(struct sockbuf *p, int infd, int outfd, char *buf, size_t sz);

int sockbuf_empty(struct sockbuf *sb);

void setup_rwset(fd_set *rset, fd_set *wset, struct sockbuf *sbarray, size_t sz);

void handle_connection(int connfd);

#endif /* !SOCKBUF_H */
