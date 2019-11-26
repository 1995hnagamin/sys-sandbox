#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

// 2019-11-18 12:34:56
// 1234567890123456789
size_t const DATETIME_BUFFER_LENGTH = 25;

// 123.123.123.123-65535.txt
// 1234567890123456789012345
size_t const FILENAME_LENGTH = 30;

void
handle_connection(int connfd, struct sockaddr_in *cliaddr) {
	printf("%s:%d\n", inet_ntoa(cliaddr->sin_addr), ntohs(cliaddr->sin_port));

	char filename[FILENAME_LENGTH];
	sprintf(filename, "%s-%d.txt", inet_ntoa(cliaddr->sin_addr), ntohs(cliaddr->sin_port));
	FILE *txtout = fopen(filename, "a");
	size_t const readbufsz = 10;
	char readbuf[readbufsz];
	ssize_t res;
	while ((res = read(connfd, readbuf, readbufsz - 1)) > 0) {
		fwrite(readbuf, sizeof(char), res, txtout);
	}
	fclose(txtout);
}

int
run_server(int port) {
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("listenfd = %d\n", listenfd);

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
		printf("waiting...\n");
		int connfd = accept(listenfd, (struct sockaddr *)(&cliaddr), &clilen);
		handle_connection(connfd, &cliaddr);
		close(connfd);
		printf("closed.\n");
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
