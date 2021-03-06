#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

struct mcat_conn {
	int connfd;
	FILE *txtout;
};

void
setup_mcat_conn(struct mcat_conn *conn, int connfd, struct sockaddr_in *cliaddr) {
	size_t const ipaddrsz = 18;
	size_t const filename_length = 30; // 123.123.123.123-65535.txt

	printf("%s:%d\n", inet_ntoa(cliaddr->sin_addr), ntohs(cliaddr->sin_port));

	char ipaddr[ipaddrsz], filename[filename_length];
	inet_ntop(AF_INET, &(cliaddr->sin_addr), ipaddr, ipaddrsz);
	sprintf(filename, "%s-%d.txt", ipaddr, ntohs(cliaddr->sin_port));

	FILE *txtout = fopen(filename, "a");

	conn->connfd = connfd;
	conn->txtout = txtout;
}

int
add_client_to_pollfd(struct pollfd *clients, struct mcat_conn *conns, size_t sz, int listenfd) {
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	int connfd = accept(listenfd, (struct sockaddr *)(&cliaddr), &clilen);
	size_t i;
	for (i = 1; i < sz; ++i) {
		if (clients[i].fd < 0) {
			setup_mcat_conn(conns + i, connfd, &cliaddr);
			clients[i].fd = connfd;
			clients[i].events = POLLIN;
			return i;
		}
	}

	char const msg[] = "Sorry\n";
	write(connfd, msg, sizeof(msg));
	close(connfd);
	return -1;
}

void
handle_connection(struct pollfd *clients, struct mcat_conn *conn) {
	size_t const readbufsz = 10;
	char readbuf[readbufsz];
	ssize_t const res = read(conn->connfd, readbuf, readbufsz - 1);
	if (res <= 0) {
		close(conn->connfd);
		fclose(conn->txtout);
		clients->fd = -1;
		return;
	}
	fwrite(readbuf, sizeof(char), res, conn->txtout);
}

size_t const MAX_NUM_CLIENTS = 10;

int
run_server(int port) {
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("listenfd = %d\n", listenfd);

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	struct pollfd clients[MAX_NUM_CLIENTS + 1];
	struct mcat_conn conns[MAX_NUM_CLIENTS + 1];
	clients[0].fd = listenfd;
	clients[0].events = POLLIN;
	for (size_t i = 1; i <= MAX_NUM_CLIENTS; ++i) {
		clients[i].fd = -1, clients[i].events = 0;
	}

	bind(listenfd, (struct sockaddr *)(&servaddr), sizeof(servaddr));

	int const backlog = 5;
	listen(listenfd, backlog);

	for (;;) {
		int nfds = poll(clients, MAX_NUM_CLIENTS + 1, -1);
		if (clients[0].revents & POLLIN) {
			add_client_to_pollfd(clients, conns, MAX_NUM_CLIENTS + 1, listenfd);
		}
		for (size_t i = 1; i <= MAX_NUM_CLIENTS; ++i) {
			if (clients[i].revents & POLLIN) {
				handle_connection(clients + i, conns + i);
			}
		}
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
