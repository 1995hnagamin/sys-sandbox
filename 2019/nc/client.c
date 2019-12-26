#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "sockbuf.h"

void
connect_server(char *hostname, int port) {
	int connfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	int status = inet_pton(AF_INET, hostname, &servaddr.sin_addr);
	if (status != 1) {
		fprintf(stderr, "error\n");
		exit(EXIT_FAILURE);
	}
	connect(connfd, (struct sockaddr *)(&servaddr), sizeof(servaddr));

	handle_connection(connfd);
	close(connfd);
}

int
main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("usage: client <hostname> <port>\n");
		return 0;
	}
	int port;
	sscanf(argv[2], "%d", &port);
	connect_server(argv[1], port);
	return 0;
}
