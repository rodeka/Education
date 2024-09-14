#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <bits/getopt_core.h>

#define DEFAULT_PORT 8080
#define DEFAULT_ADDRESS "127.0.0.1"

void print_help(){
    printf("Usage: lab2zrpN3250_client [-a address] [-p port] query\n");
    printf("Options:\n");
    printf("  -a address  Address of server\n");
    printf("  -p port     Port of server\n");
    printf("  -h          Show this help message\n");
    return;
}

int main(int argc, char *argv[]) {
    const char *address = DEFAULT_ADDRESS;
    int port = DEFAULT_PORT;
    int debug_mode = 0;
    
    if (getenv("LAB2DEBUG")) {
        debug_mode = 1;
    }

    if (getenv("LAB2PORT")) {
        port = atoi(getenv("LAB2PORT"));
        if (debug_mode) {
            fprintf(stderr, "DEBUG: LAB2PORT найден: %d\n", port);
        }
    }
    if (getenv("LAB2ADDR")) {
        address = getenv("LAB2ADDR");
        if (debug_mode) {
            fprintf(stderr, "DEBUG: LAB2ADDR найден: %s\n", address);
        }
    }

    int opt;
    while ((opt = getopt(argc, argv, "a:p:h")) != -1) {
        switch (opt) {
            case 'a':
                address = optarg;
                if (debug_mode) {
                    fprintf(stderr, "DEBUG: Адрес из командной строки: %s\n", address);
                }
                break;
            case 'p':
                port = atoi(optarg);
                if (debug_mode) {
                    fprintf(stderr, "DEBUG: Порт из командной строки: %d\n", port);
                }
                break;
            case 'h':
                print_help();
                break;
            default:
                fprintf(stderr, "Usage: lab2zrpN3250_client [-a address] [-p port] query\n");
                exit(EXIT_FAILURE);
        }
    }
    
    if (optind >= argc) {
        fprintf(stderr, "Expected query after options\n");
        exit(EXIT_FAILURE);
    }

    size_t query_len = strlen(argv[optind]) + 2;
    char *query = (char *)malloc(query_len);
    if (!query) {
        perror("Failed to allocate memory for query");
        exit(EXIT_FAILURE);
    }
    snprintf(query, query_len, "%s\n", argv[optind]);
    
    if (debug_mode) {
        fprintf(stderr, "DEBUG: Запрос: %s\n", query);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Could not create socket");
        free(query);
        exit(EXIT_FAILURE);
    }

    if (debug_mode) {
        fprintf(stderr, "DEBUG: Сокет создан\n");
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, address, &server_addr.sin_addr);
    
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Could not connect to server");
        close(sock);
        free(query);
        exit(EXIT_FAILURE);
    }

    if (debug_mode) {
        fprintf(stderr, "DEBUG: Успешное подключение к серверу %s:%d\n", address, port);
    }
    
    if(write(sock, query, strlen(query)) == -1){
        perror("Write error");
        exit(EXIT_FAILURE);
    }
    free(query);
    
    if (debug_mode) {
        fprintf(stderr, "DEBUG: Запрос отправлен\n");
    }

    size_t buffer_size = 1024;
    char *buffer = (char *)malloc(buffer_size);
    if (!buffer) {
        perror("Failed to allocate memory for buffer");
        close(sock);
        exit(EXIT_FAILURE);
    }

    int read_size = read(sock, buffer, buffer_size - 1);
    if (read_size > 0) {
        buffer[read_size] = '\0';
        printf("Response from server: %s", buffer);
    } else {
        perror("Error reading response from server");
    }

    free(buffer);
    close(sock);
    
    if (debug_mode) {
        fprintf(stderr, "DEBUG: Соединение закрыто\n");
    }

    return 0;
}
