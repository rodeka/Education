#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

void print_usage(const char *prog_name) {
    printf("Usage: %s [options] <directory> <target_sequence>\n", prog_name);
    printf("Options:\n");
    printf("  -h, --help      Show this help message and exit\n");
    printf("  -v, --version   Show version information and exit\n");
}

void print_version() {
    printf("lab11zrpN3250 version 1.0\n");
}

int hex_to_bytes(const char *hex_string, unsigned char **byte_sequence, size_t *length) {
    size_t hex_length = strlen(hex_string);
    if (hex_length % 2 != 0) {
        fprintf(stderr, "Invalid hex string length.\n");
        return -1;
    }
    
    *length = hex_length / 2;
    *byte_sequence = malloc(*length);
    if (*byte_sequence == NULL) {
        perror("malloc");
        return -1;
    }
    
    for (size_t i = 0; i < *length; ++i) {
        sscanf(hex_string + 2*i, "%2hhx", &((*byte_sequence)[i]));
    }
    
    return 0;
}

int contains_sequence(const char *file_path, const unsigned char *sequence, size_t seq_len) {
    if (getenv("LAB11DEBUG")) {
        fprintf(stderr, "Start searching pattern in: %s\n", file_path);
    }
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        perror(file_path);
        return 0;
    }
    
    unsigned char* buffer = malloc(sizeof(unsigned char)*4096);
    ssize_t bytes_read;
    size_t match_len = 0;
    
    while ((bytes_read = read(fd, buffer, 4096)) > 0) {
        for (ssize_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] == sequence[match_len]) {
                match_len++;
                if (match_len == seq_len) {
                    free(buffer);
                    close(fd);
                    return 1;
                }
            } else {
                match_len = 0;
            }
        }
    }
    free(buffer);
    close(fd);
    return 0;
}

void search_directory(const char *dir_path, const unsigned char *sequence, size_t seq_len) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror(dir_path);
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        char *full_path;
        if(asprintf(&full_path, "%s/%s", dir_path, entry->d_name)){
            if(getenv("LAB11DEBUG")){
                printf("Correct allocated\n");
            }
        } else {
            fprintf(stderr, "Error on allocated fullpath\n");
            continue;
        }
        
        struct stat path_stat;
        if (stat(full_path, &path_stat) != 0) {
            perror(full_path);
            free(full_path);
            continue;
        }
        
        if (S_ISDIR(path_stat.st_mode)) {
            search_directory(full_path, sequence, seq_len);
        } else if (S_ISREG(path_stat.st_mode)) {
            if (contains_sequence(full_path, sequence, seq_len)) {
                printf("%s\n", full_path);
                if (getenv("LAB11DEBUG")) {
                    fprintf(stderr, "Match found in file: %s\n", full_path);
                }
            }
        }  
        free(full_path);
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "hv")) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case 'v':
                print_version();
                exit(EXIT_SUCCESS);
            default:
                print_usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    if (argc - optind < 2) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    
    const char *directory = argv[optind];
    const char *hex_sequence = argv[optind + 1];
    
    if (strncmp(hex_sequence, "0x", 2) != 0) {
        fprintf(stderr, "Invalid target sequence format. Must start with '0x'.\n");
        exit(EXIT_FAILURE);
    }
    
    hex_sequence += 2;
    unsigned char *byte_sequence;
    size_t seq_length;
    
    if (hex_to_bytes(hex_sequence, &byte_sequence, &seq_length) != 0) {
        fprintf(stderr, "Failed to parse target sequence.\n");
        exit(EXIT_FAILURE);
    }
    
    search_directory(directory, byte_sequence, seq_length);
    
    free(byte_sequence);
    return 0;
}
