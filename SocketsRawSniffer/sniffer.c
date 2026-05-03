#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>

void process_packet(unsigned char* buffer, int size) 
{
    struct iphdr *ip_header = (struct iphdr*)buffer;
    struct udphdr *udp_header;
    
    // Проверяем, что это UDP (протокол 17)
    if(ip_header->protocol == 17) 
    {
        // Получаем заголовки
        udp_header = (struct udphdr*)(buffer + (ip_header->ihl * 4));
        
        // Преобразуем IP-адреса в читаемый формат
        struct sockaddr_in source, dest;
        source.sin_addr.s_addr = ip_header->saddr;
        dest.sin_addr.s_addr = ip_header->daddr;
        
        // Выводим информацию о пакете
        printf("\n=== UDP Packet Received ===\n");
        printf("Source IP: %s\n", inet_ntoa(source.sin_addr));
        printf("Destination IP: %s\n", inet_ntoa(dest.sin_addr));
        printf("Source Port: %d\n", ntohs(udp_header->source));
        printf("Dest Port: %d\n", ntohs(udp_header->dest));
        printf("UDP Length: %d\n", ntohs(udp_header->len));
        printf("==========================\n");
    }
}

int main() 
{
    int raw_socket;
    unsigned char *buffer = (unsigned char *)malloc(65536);
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    
    // Создаём raw socket для всех IP протоколов
    raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    
    if(raw_socket < 0) 
    {
        perror("Socket creation failed");
        return 1;
    }
    
    // Для включения IP заголовка в получаемые данные
    int one = 1;
    if(setsockopt(raw_socket, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) 
    {
        perror("setsockopt failed");
        return 1;
    }
    
    printf("UDP Sniffer started. Listening for UDP traffic...\n");
    printf("Run with sudo: sudo ./udp_sniffer\n\n");
    
    while(1) 
    {
        int packet_size = recvfrom(raw_socket, buffer, 65536, 0, 
                                   (struct sockaddr*)&addr, (socklen_t*)&addr_len);
        
        if(packet_size < 0) 
        {
            perror("recvfrom failed");
            close(raw_socket);
            return 1;
        }
        
        process_packet(buffer, packet_size);
    }
    
    close(raw_socket);
    free(buffer);
    return 0;
}