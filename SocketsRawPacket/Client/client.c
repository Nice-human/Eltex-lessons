#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>

#define SERVER_PORT 10200
#define SOURCE_PORT 8888
#define BUFF_SIZE 1024

// Функция расчета контрольной суммы IP
unsigned short ip_checksum(unsigned short *buffer, int length) 
{
    unsigned long sum = 0;
    
    while (length > 1) 
    {
        sum += *buffer++;
        length -= 2;
    }
    
    if (length == 1) 
    {
        sum += *(unsigned char *)buffer;
    }
    
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    
    return (unsigned short)(~sum);
}

// Функция расчета контрольной суммы UDP
unsigned short udp_checksum(struct iphdr *ip, struct udphdr *udp, unsigned char *data, int data_len) 
{
    unsigned char buf[BUFF_SIZE];
    int offset = 0;
    unsigned long sum = 0;
    unsigned short *ptr;
    int udp_len = sizeof(struct udphdr) + data_len;
    
    // Псевдозаголовок для UDP
    struct pseudo_header 
    {
        u_int32_t src;
        u_int32_t dst;
        u_int8_t zero;
        u_int8_t proto;
        u_int16_t len;
    } pseudo;
    
    pseudo.src = ip->saddr;
    pseudo.dst = ip->daddr;
    pseudo.zero = 0;
    pseudo.proto = IPPROTO_UDP;
    pseudo.len = htons(udp_len);
    
    // Собираем все для расчета
    memcpy(buf + offset, &pseudo, sizeof(pseudo));
    offset += sizeof(pseudo);
    memcpy(buf + offset, udp, sizeof(struct udphdr));
    offset += sizeof(struct udphdr);
    memcpy(buf + offset, data, data_len);
    offset += data_len;
    
    // Расчет checksum
    ptr = (unsigned short *)buf;
    for (int i = 0; i < offset / 2; i++) 
    {
        sum += ptr[i];
    }
    
    if (offset % 2) 
    {
        sum += buf[offset - 1];
    }
    
    while (sum >> 16) 
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return (unsigned short)(~sum);
}

int main()
{
    int sock_raw;
    char buffer[BUFF_SIZE];
    struct sockaddr_ll client_addr;

    memset(buffer, 0, BUFF_SIZE);
    char* message = "Hello!";
    int data_len = strlen(message);

    struct ethhdr *eth_hdr = (struct ethhdr *)buffer;
    
    // MAC адрес назначения 
    unsigned char dest_mac[6];
    // MAC адрес источника 
    unsigned char src_mac[6];
    
    sscanf("08:00:27:83:7a:42", "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &dest_mac[0], &dest_mac[1], &dest_mac[2], 
           &dest_mac[3], &dest_mac[4], &dest_mac[5]);
    
    sscanf("08:00:27:19:34:4b", "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &src_mac[0], &src_mac[1], &src_mac[2],
           &src_mac[3], &src_mac[4], &src_mac[5]);

    memcpy(eth_hdr->h_dest, dest_mac, ETH_ALEN);
    memcpy(eth_hdr->h_source, src_mac, ETH_ALEN);
    eth_hdr->h_proto = htons(ETH_P_IP);

    struct iphdr* ip_hdr = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    memset(ip_hdr, 0, sizeof(struct iphdr));
    ip_hdr->version = 4;
    ip_hdr->ihl = 5;
    ip_hdr->tos = 0;
    ip_hdr->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + data_len);
    ip_hdr->id = htons(1010);
    ip_hdr->frag_off = 0;
    ip_hdr->ttl = 64;
    ip_hdr->protocol = IPPROTO_UDP;
    ip_hdr->saddr = inet_addr("192.168.0.178");  // Мой IP
    ip_hdr->daddr = inet_addr("192.168.0.158");  // IP сервера
    ip_hdr->check = ip_checksum((unsigned short *)ip_hdr, ip_hdr->ihl * 4);

    int ip_len = ip_hdr->ihl * 4;

    struct udphdr* udp_hdr = (struct udphdr*)(buffer + sizeof(struct ethhdr) + ip_len);
    memset(udp_hdr, 0, sizeof(struct udphdr));
    udp_hdr->source = htons(SOURCE_PORT);
    udp_hdr->dest = htons(SERVER_PORT);
    udp_hdr->len = htons(sizeof(struct udphdr) + data_len);

    char *data = buffer + sizeof(struct ethhdr) + ip_len + sizeof(struct udphdr);
    strcpy(data, message);
    udp_hdr->check = udp_checksum(ip_hdr, udp_hdr, (unsigned char *)data, data_len);  

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sll_family = AF_PACKET;
    client_addr.sll_protocol = htons(ETH_P_ALL); 
    client_addr.sll_ifindex = if_nametoindex("enp0s3");
    memcpy(client_addr.sll_addr, dest_mac, ETH_ALEN);
    client_addr.sll_halen = ETH_ALEN;

    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw == -1)
    {
        perror("Error socket");
        exit(EXIT_FAILURE);
    }


    int total_len = sizeof(struct ethhdr) + ip_len + sizeof(struct udphdr) + data_len;

    int send = sendto(sock_raw, buffer, total_len, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
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
    struct sockaddr_ll server_addr;
    socklen_t server_len = sizeof(server_addr);

    while (1)
    {
        int recv = recvfrom(sock_raw, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&server_addr, &server_len);
        if (recv < 0)
        {
            perror("Error recvfrom");
            exit(EXIT_FAILURE);
        }

        struct ethhdr *resp_eth = (struct ethhdr *)recv_buffer;
        if (ntohs(resp_eth->h_proto) == ETH_P_IP) 
        {
            struct iphdr *ip_resp = (struct iphdr *)(recv_buffer + sizeof(struct ethhdr));
            int ip_header_len = ip_resp->ihl * 4;
        
            
            if (ip_resp->protocol == IPPROTO_UDP)
            {
                struct udphdr* udp_resp = (struct udphdr*)(recv_buffer + sizeof(struct ethhdr) + ip_header_len);  

                if (ntohs(udp_resp->source) == SERVER_PORT && ntohs(udp_resp->dest) == SOURCE_PORT)
                {
                    int resp_data_len = ntohs(udp_resp->len) - sizeof(struct udphdr);
                    char* resp_data = recv_buffer + sizeof(struct ethhdr) + ip_header_len + sizeof(struct udphdr);
                    printf("Client RAW received data from the server:\n");
                    printf("Source_port: %d\n", ntohs(udp_resp->source));
                    printf("Dest: %d\n", ntohs(udp_resp->dest));
                    printf("Message: %.*s\n", resp_data_len, resp_data);
                    printf("Length: %d bytes\n\n", resp_data_len);

                    printf("IP saddr_port: [%s]\n", inet_ntoa(*(struct in_addr*)&ip_resp->saddr)); 
                    printf("IP daddr_port: [%s]\n", inet_ntoa(*(struct in_addr*)&ip_resp->daddr)); 
                    printf("IP ID: [%d]\n\n",ntohs(ip_resp->id));
                    break;
                }
            } 
        }     
    }
     
    close(sock_raw);   
    return 0;     
}