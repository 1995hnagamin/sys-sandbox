#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "sockbuf.h"

int
connect_server(char *address, int port) {
	int connfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	int status = inet_pton(AF_INET, address, &servaddr.sin_addr);
	switch (status) {
		case 0:
			fprintf(stderr, "invalid network address: \"%s\"\n", address);
			return -1;
		case -1:
			fprintf(stderr, "error: %s\n", strerror(errno));
			return -1;
	}
	status = connect(connfd, (struct sockaddr *)(&servaddr), sizeof(servaddr));
	if (status == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		return -1;
	}
	return connfd;
}

int
main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("usage: client <address> <port>\n");
		return 0;
	}
	int port;
	sscanf(argv[2], "%d", &port);
	int connfd = connect_server(argv[1], port);
	if (connfd < 0) {
		exit(EXIT_FAILURE);
	}
	handle_connection(connfd);
	close(connfd);
	return 0;
}
