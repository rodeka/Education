#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/wait.h>
#include <bits/getopt_core.h>

#define DEFAULT_PORT 8080
#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_LOGFILE "/tmp/lab2.log"

int requests_served = 0;
int error_count = 0;
time_t start_time;
int log_fd = -1;
int wait_time = 0;

void log_message(const char *message) {
    time_t now = time(NULL);

    char *time_buf = malloc(64);
    if (!time_buf) {
        perror("Memory allocation error for time buffer");
        return;
    }
    strftime(time_buf, 64, "%d.%m.%y %H:%M:%S", localtime(&now));

    size_t log_size = strlen(time_buf) + strlen(message) + 4;
    char *log_buf = malloc(log_size);
    if (!log_buf) {
        perror("Memory allocation error for log buffer");
        free(time_buf);
        return;
    }

    snprintf(log_buf, log_size, "%s: %s\n", time_buf, message);

    if (log_fd >= 0) {
        if(write(log_fd, log_buf, strlen(log_buf))==-1){
            perror("Write error to log");
            exit(EXIT_FAILURE);
        };
    }

    free(time_buf);
    free(log_buf);
}


void handle_signal(int sig) {
    if (sig == SIGUSR1) {
        char stats[256];
        snprintf(stats, sizeof(stats), "Server stats: Uptime: %ld seconds, Requests served: %d, Errors: %d",
                 time(NULL) - start_time, requests_served, error_count);
        log_message(stats);
    } else {
        log_message("Shutting down server");
        close(log_fd);
        exit(0);
    }
}

int is_central_polygonal(int n) {
    int k = 0;
    while ((k * (k + 1)) / 2 + 1 < n)
        k++;

    return ((k * (k + 1)) / 2 + 1 == n) ? 1 : 0;
}

void handle_client(int client_socket) {
    size_t buffer_size = 1024;
    char *buffer = malloc(buffer_size);
    if (!buffer) {
        log_message("Memory allocation error");
        error_count++;
        close(client_socket);
        exit(1);
    }

    int read_size = read(client_socket, buffer, buffer_size - 1);
    if (read_size <= 0) {
        log_message("Error reading from client");
        error_count++;
        free(buffer);
        close(client_socket);
        exit(1);
    }

    buffer[read_size] = '\0';
    char *token = strtok(buffer, " \n");
    int has_central = 0;
    int has_non_central = 0;

    while (token != NULL) {
        int num = strtol(token, NULL, 0);
        if (is_central_polygonal(num)) {
            has_central = 1;
        } else {
            has_non_central = 1;
        }
        token = strtok(NULL, " \n");
    }

    const char *response;
    if (has_central && !has_non_central) {
        response = "ALL\n";
    } else if (has_central) {
        response = "SOME\n";
    } else {
        response = "NONE\n";
    }

    if (wait_time > 0) {
        sleep(wait_time);
    }

    if(write(client_socket, response, strlen(response)) == -1){
        perror("Write error from response to client_socket");
        exit(EXIT_FAILURE);
    };
    requests_served++;
    free(buffer);
    close(client_socket);
}

int main(int argc, char *argv[]) {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    
    int port = DEFAULT_PORT;
    const char *address = DEFAULT_ADDRESS;
    const char *logfile = DEFAULT_LOGFILE;
    int daemonize = 0;

    // Обработка переменных среды
    if (getenv("LAB2PORT")) {
        port = atoi(getenv("LAB2PORT"));
    }
    if (getenv("LAB2ADDR")) {
        address = getenv("LAB2ADDR");
    }
    if (getenv("LAB2LOGFILE")) {
        logfile = getenv("LAB2LOGFILE");
    }
    if (getenv("LAB2WAIT")) {
        wait_time = atoi(getenv("LAB2WAIT"));
    }

    int opt;
    while ((opt = getopt(argc, argv, "a:p:l:dw:")) != -1) {
        switch (opt) {
            case 'a':
                address = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'l':
                logfile = optarg;
                break;
            case 'd':
                daemonize = 1;
                break;
            case 'w':
                wait_time = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-a address] [-p port] [-l logfile] [-d] [-w wait]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    log_fd = open(logfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd < 0) {
        perror("Could not open log file");
        exit(EXIT_FAILURE);
    }
    
    if (daemonize && fork() > 0) {
        exit(0);
    }
    
    start_time = time(NULL);
    log_message("Server started");
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, address, &server_addr.sin_addr);
    
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Could not bind socket");
        exit(EXIT_FAILURE);
    }
    
    listen(server_socket, 5);
    
    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Error accepting connection");
            continue;
        }
        
        if (fork() == 0) {
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        } else {
            close(client_socket);
            waitpid(-1, NULL, WNOHANG);
        }
    }
    
    close(server_socket);
    return 0;
}
