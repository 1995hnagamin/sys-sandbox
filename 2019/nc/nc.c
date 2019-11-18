#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

int
run_server(int port) {
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("listenfd = %d\n", listenfd);
	close(listenfd);
	return 0;
}

int
main(int argc, char* argv[]) {
	return run_server();
}
