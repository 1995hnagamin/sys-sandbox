#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

int max(int x, int y) {
	return x > y ? x : y;
}

// 2019-11-18 12:34:56
// 1234567890123456789
size_t const DATETIME_BUFFER_LENGTH = 25;

void
write_current_time(int fd) {
	time_t now = time(NULL);
	struct tm *tmnow = localtime(&now);
	char buf[DATETIME_BUFFER_LENGTH];
	strftime(buf, DATETIME_BUFFER_LENGTH, "%Y-%m-%d %H:%M:%S\n", tmnow);
	write(fd, buf, strlen(buf));
}

void
handle_connection(int connfd) {
	char const welcome_msg[] = "Hello I'm a teapot\n";
	ssize_t written = write(connfd, welcome_msg, strlen(welcome_msg));
	write_current_time(connfd);

	size_t const readbufsz = 10;
	char readbuf[readbufsz];
	ssize_t res;
	while ((res = read(connfd, readbuf, readbufsz)) > 0) {
		write(fileno(stdout), readbuf, res);
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
