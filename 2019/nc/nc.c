#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

int max(int x, int y) {
	return x > y ? x : y;
}

int
read_and_write(int readfd, int writefd) {
	size_t const readbufsz = 10;
	char readbuf[readbufsz];
	ssize_t res;
	if ((res = read(readfd, readbuf, readbufsz)) > 0) {
		write(writefd, readbuf, res);
		return res;
	}
	return 0;
}

void
handle_connection(int connfd) {
	int const maxfdp1 = 1 + max(fileno(stdout), connfd);

	for (;;) {
		fd_set rset;
		FD_ZERO(&rset);
		FD_SET(fileno(stdin), &rset);
		FD_SET(connfd, &rset);

		int nfds = select(maxfdp1, &rset, NULL, NULL, NULL);
		if (FD_ISSET(fileno(stdin), &rset)) {
			read_and_write(fileno(stdin), connfd);
		}
		if (FD_ISSET(connfd, &rset)) {
			int res = read_and_write(connfd, fileno(stdout));
			if (res == 0) {
				return;
			}
		}
	}
}

int
run_server(int port) {
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	char lfd_msg[100];
	sprintf(lfd_msg, "listenfd = %d\n", listenfd);
	write(fileno(stdout), lfd_msg, strlen(lfd_msg));

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenfd, (struct sockaddr *)(&servaddr), sizeof(servaddr));

	int const backlog = 5;
	listen(listenfd, backlog);

	for (;;) {
		struct sockaddr_in cliaddr;
		socklen_t clilen = sizeof(cliaddr);
		int connfd = accept(listenfd, (struct sockaddr *)(&cliaddr), &clilen);
		handle_connection(connfd);
		close(connfd);
	}
	close(listenfd);
	return 0;
}

int
main(int argc, char* argv[]) {
	int port;
	int n = sscanf(argv[1], "%d", &port);
	return run_server(port);
}
