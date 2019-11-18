#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

int
main(int argc, char* argv[]) {
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	printf("listenfd = %d\n", listenfd);
	close(listenfd);
	return 0;
}
