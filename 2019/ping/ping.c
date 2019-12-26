#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct icmp_echo_head {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
  uint16_t ident;
  uint16_t seqno;
};

uint16_t ip_checksum(void *buf, size_t len) {
	uint16_t *words = buf;
	uint32_t sum = 0;
	for (size_t i = 0; i < len; ++i) {
		sum += *words;
	}
	return !(sum + (sum >> 16));
}

void send_packet(int connfd, struct sockaddr_in *destaddr, uint16_t ident) {
  struct icmp_echo_head icmp;
  icmp.type = 8;
  icmp.code = 0;
  icmp.ident = ident;
  icmp.checksum = 0;
  icmp.seqno = 0;
  void *sendbuf = &icmp;
  size_t len = 0;
  sendto(connfd, sendbuf, len, 0, (struct sockaddr *)destaddr,
         sizeof(struct sockaddr_in));
}

void ping(void) {
  int connfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  setuid(getuid());

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  inet_pton(AF_INET, "8.8.8.8", &servaddr.sin_addr);

  send_packet(connfd, NULL, 0);
}

int main(void) {
	return 0;
}
