#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>

#define SERVER_PORT 10200
#define SOURCE_PORT 888
#define BUFF_SIZE 1024

int main()
{
    int sock_raw;
    char buffer[BUFF_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    memset(buffer, 0, BUFF_SIZE);
    char* message = "Hello!";
    int data_len = strlen(message);

    struct iphdr* ip_hdr = (struct iphdr*)buffer;
    ip_hdr->version = 4;
    ip_hdr->ihl = 5;
    ip_hdr->tos = 0;
    ip_hdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
    ip_hdr->id = htons(1010);
    ip_hdr->frag_off = 0;
    ip_hdr->ttl = 64;
    ip_hdr->protocol = IPPROTO_UDP;
    ip_hdr->check = 0;
    ip_hdr->saddr = inet_addr("127.0.0.1");
    ip_hdr->daddr = inet_addr("127.0.0.1");

    int ip_len = ip_hdr->ihl * 4;

    struct udphdr* udp_hdr = (struct udphdr*)(buffer + ip_len);
    udp_hdr->source = htons(SOURCE_PORT);
    udp_hdr->dest = htons(SERVER_PORT);
    udp_hdr->check = 0;
    udp_hdr->len = htons(sizeof(struct udphdr) + data_len);

    char *data = buffer + ip_len + sizeof(struct udphdr);
    strcpy(data, message);

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    client_addr.sin_port = htons(SERVER_PORT);

    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock_raw == -1)
    {
        perror("Error socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(sock_raw, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));

    int total_len = ip_len + sizeof(struct udphdr) + data_len;

    int send = sendto(sock_raw, buffer, total_len, 0, (struct sockaddr*)&client_addr, client_addr_len);
    if (send < 0)
    {
        perror("Error sendto");
        exit(EXIT_FAILURE);
    }

    printf("\nClient RAW send to server:\n");
    printf("Source_port: %d\n", ntohs(udp_hdr->source));
    printf("Dest: %d\n", ntohs(udp_hdr->dest));
    printf("Message: %s\n", data);
    printf("Lenght: %d bytes\n\n", ntohs(udp_hdr->len));
    

    char recv_buffer[BUFF_SIZE];
    struct sockaddr_in server_addr;
    socklen_t server_len = sizeof(server_addr);

    while (1)
    {
        int recv = recvfrom(sock_raw, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&server_addr, &server_len);
        if (recv < 0)
        {
            perror("Error recvfrom");
            exit(EXIT_FAILURE);
        }

        struct iphdr *ip_resp = (struct iphdr *)recv_buffer;
        int ip_header_len = ip_resp->ihl * 4;

        if (ip_resp->protocol == IPPROTO_UDP)
        {
            struct udphdr* udp_resp = (struct udphdr*)(recv_buffer + ip_header_len);  

            if (ntohs(udp_resp->source) == SERVER_PORT && ntohs(udp_resp->dest) == SOURCE_PORT)
            {
                int resp_data_len = ntohs(udp_resp->len) - sizeof(struct udphdr);
                char* resp_data = recv_buffer + ip_header_len + sizeof(struct udphdr);
                printf("Client RAW send to server:\n");
                printf("Source_port: %d\n", ntohs(udp_resp->source));
                printf("Dest: %d\n", ntohs(udp_resp->dest));
                printf("Message: %.*s\n", resp_data_len, resp_data);
                printf("Length: %ld bytes\n\n", ntohs(udp_resp->len) - sizeof(struct udphdr));

                printf("IP saddr_port: [%s]\n", inet_ntoa(*(struct in_addr*)&ip_resp->saddr)); 
                printf("IP daddr_port: [%s]\n", inet_ntoa(*(struct in_addr*)&ip_resp->daddr)); 
                printf("IP ID: [%d]\n\n",ntohs(ip_resp->id));
                break;
            }
        }      
    }
     
    close(sock_raw);   
    return 0;     
}