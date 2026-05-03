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
    socklen_t udp_len = sizeof(client_addr);

    memset(buffer, 0, BUFF_SIZE);
    char* message = "Hello!";
    int data_len = strlen(message);

    struct udphdr* udp_hdr = (struct udphdr*)buffer;

    udp_hdr->source = htons(SOURCE_PORT);
    udp_hdr->dest = htons(SERVER_PORT);
    udp_hdr->check = 0;
    udp_hdr->len = htons(sizeof(struct udphdr) + data_len);

    char *data = buffer + sizeof(struct udphdr);
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

    int total_len = sizeof(struct udphdr) + data_len;

    int send = sendto(sock_raw, buffer, total_len, 0, (struct sockaddr*)&client_addr, udp_len);
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
        struct udphdr* udp_resp = (struct udphdr*)(recv_buffer + ip_header_len);  
        
        if (ntohs(udp_resp->source) == SERVER_PORT )
        {
            int resp_data_len = ntohs(udp_resp->len) - sizeof(struct udphdr);
            char* resp_data = recv_buffer + ip_header_len + sizeof(struct udphdr);
            printf("Client RAW send to server:\n");
            printf("Source_port: %d\n", ntohs(udp_resp->source));
            printf("Dest: %d\n", ntohs(udp_resp->dest));
            printf("Message: %.*s\n", resp_data_len, resp_data);
            printf("Length: %ld bytes\n\n", ntohs(udp_resp->len) - sizeof(struct udphdr));
            break;
        }
        else
        {
            continue;
        }
    }
     
    close(sock_raw);   
    return 0;     
}