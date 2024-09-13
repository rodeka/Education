#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <errno.h>
#include <getopt.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "plugin_api.h"

#define MAX_PLUGINS 16
#define MAX_PLUGIN_OPTS 16

typedef struct {
    char *plugin_path;
    void *plugin_handle;
    struct plugin_info plugin_metadata;
    int (*file_processor)(const char *filename, struct option options[], size_t options_length);
    struct option *active_options;
    char **option_args;
    size_t active_options_length;
    int is_loaded;
} plugin_wrapper_t;

plugin_wrapper_t loaded_plugins[MAX_PLUGINS];
int plugin_count = 0;

void scan_directory_for_plugins(const char *directory) {
    char *DEBUG = getenv("LAB1DEBUG");
    if (DEBUG) {
        printf("DEBUG: Scanning directory for plugins: %s\n", directory);
    }

    DIR *plugin_directory = opendir(directory);
    if (!plugin_directory) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *plugin_entry;
    while ((plugin_entry = readdir(plugin_directory)) != NULL) {
        if (strstr(plugin_entry->d_name, ".so")) {
            char *plugin_full_path = malloc(strlen(directory) + strlen(plugin_entry->d_name) + 2);
            if (!plugin_full_path) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            snprintf(plugin_full_path, strlen(directory) + strlen(plugin_entry->d_name) + 2, "%s/%s", directory, plugin_entry->d_name);

            if (plugin_count < MAX_PLUGINS) {
                loaded_plugins[plugin_count].plugin_path = plugin_full_path;
                loaded_plugins[plugin_count].plugin_handle = NULL;
                loaded_plugins[plugin_count].active_options = malloc(MAX_PLUGIN_OPTS * sizeof(struct option));
                loaded_plugins[plugin_count].option_args = malloc(MAX_PLUGIN_OPTS * sizeof(char *));
                
                if (!loaded_plugins[plugin_count].active_options || !loaded_plugins[plugin_count].option_args) {
                    fprintf(stderr, "ERROR: Memory allocation failed for plugin options\n");
                    exit(EXIT_FAILURE);
                }

                loaded_plugins[plugin_count].active_options_length = 0;
                loaded_plugins[plugin_count].is_loaded = 0;

                if (DEBUG) {
                    printf("DEBUG: Plugin found: %s\n", plugin_full_path);
                }

                plugin_count++;
            } else {
                fprintf(stderr, "ERROR: Maximum plugin limit reached\n");
                free(plugin_full_path);
                break;
            }
        }
    }

    closedir(plugin_directory);
}

void initialize_plugin(plugin_wrapper_t *plugin) {
    char *DEBUG = getenv("LAB1DEBUG");
    if (DEBUG) {
        printf("DEBUG: Initializing plugin: %s\n", plugin->plugin_path);
    }

    if (plugin->plugin_handle) {
        if (DEBUG) {
            printf("DEBUG: Plugin already initialized: %s\n", plugin->plugin_path);
        }
        return;
    }

    plugin->plugin_handle = dlopen(plugin->plugin_path, RTLD_LAZY);
    if (!plugin->plugin_handle) {
        fprintf(stderr, "ERROR: dlopen() failed: %s\n", dlerror());
        return;
    }

    int (*get_plugin_info)(struct plugin_info *) = dlsym(plugin->plugin_handle, "plugin_get_info");
    if (!get_plugin_info) {
        fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
        dlclose(plugin->plugin_handle);
        plugin->plugin_handle = NULL;
        return;
    }

    if (get_plugin_info(&plugin->plugin_metadata) != 0) {
        fprintf(stderr, "ERROR: plugin_get_info() failed\n");
        dlclose(plugin->plugin_handle);
        plugin->plugin_handle = NULL;
        return;
    }

    plugin->file_processor = dlsym(plugin->plugin_handle, "plugin_process_file");
    if (!plugin->file_processor) {
        fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
        dlclose(plugin->plugin_handle);
        plugin->plugin_handle = NULL;
    }

    if (DEBUG) {
        printf("DEBUG: Plugin successfully initialized: %s\n", plugin->plugin_path);
    }
}

void release_plugins() {
    char *DEBUG = getenv("LAB1DEBUG");
    if (DEBUG) {
        printf("DEBUG: Releasing all plugins\n");
    }

    for (int i = 0; i < plugin_count; i++) {
        if (DEBUG) {
            printf("DEBUG: Releasing plugin: %s\n", loaded_plugins[i].plugin_path);
        }

        if (loaded_plugins[i].plugin_handle) {
            dlclose(loaded_plugins[i].plugin_handle);
            loaded_plugins[i].plugin_handle = NULL;
        }

        free(loaded_plugins[i].plugin_path);
        free(loaded_plugins[i].active_options);
        free(loaded_plugins[i].option_args);
    }

    if (DEBUG) {
        printf("DEBUG: All plugins successfully released\n");
    }
}

int evaluate_file(const char *file_path, int use_and_logic, int invert_results) {
    char *DEBUG = getenv("LAB1DEBUG");
    if (DEBUG) {
        printf("DEBUG: Evaluating file: %s\n", file_path);
    }

    int result = use_and_logic ? 1 : 0;

    for (int i = 0; i < plugin_count; i++) {
        if (!loaded_plugins[i].plugin_handle) {
            continue;
        }

        struct option plugin_options[MAX_PLUGIN_OPTS];
        char *plugin_arguments[MAX_PLUGIN_OPTS];
        size_t options_count = 0;

        for (size_t j = 0; j < loaded_plugins[i].plugin_metadata.sup_opts_len; j++) {
            for (size_t k = 0; k < loaded_plugins[i].active_options_length; k++) {
                if (strcmp(loaded_plugins[i].plugin_metadata.sup_opts[j].opt.name, loaded_plugins[i].active_options[k].name) == 0) {
                    plugin_options[options_count] = loaded_plugins[i].active_options[k];
                    plugin_arguments[options_count] = loaded_plugins[i].option_args[k];
                    plugin_options[options_count].flag = (int *) plugin_arguments[options_count];
                    options_count++;
                    break;
                }
            }
        }

        if (options_count == 0) {
            if (DEBUG) {
                printf("DEBUG: No applicable options for plugin: %s\n", loaded_plugins[i].plugin_path);
            }
            continue;
        }

        int process_result = loaded_plugins[i].file_processor(file_path, plugin_options, options_count);
        if (use_and_logic) {
            result &= (process_result == 0);
        } else {
            result |= (process_result == 0);
        }
    }

    return invert_results ? !result : result;
}

void search_directory(const char *directory_path, int use_and_logic, int invert_results) {
    char *DEBUG = getenv("LAB1DEBUG");
    if (DEBUG) {
        printf("DEBUG: Searching in directory: %s\n", directory_path);
    }

    DIR *directory = opendir(directory_path);
    if (!directory) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char full_path[4096];
            snprintf(full_path, sizeof(full_path), "%s/%s", directory_path, entry->d_name);

            struct stat file_stat;
            if (stat(full_path, &file_stat) == -1) {
                perror("stat");
                continue;
            }

            if (S_ISDIR(file_stat.st_mode)) {
                search_directory(full_path, use_and_logic, invert_results);
            } else if (S_ISREG(file_stat.st_mode)) {
                if (evaluate_file(full_path, use_and_logic, invert_results)) {
                    printf("\033[0;32mMatching file found: %s\033[0m\n", full_path);
                }
            }
        }
    }

    closedir(directory);
}

void display_help() {
    printf("Usage: program_name [options] [directory]\n");
    printf("Options:\n");
    printf("  -P <dir>    Specify plugin directory\n");
    printf("  -A          Use AND logic for criteria\n");
    printf("  -O          Use OR logic for criteria\n");
    printf("  -N          Invert criteria\n");
    printf("  -v          Show version information\n");
    printf("  -h          Show this help message\n");
    printf("  -d          Enable debug mode\n");

    for (int i = 0; i < plugin_count; i++) {
        if (loaded_plugins[i].plugin_handle) {
            printf("\nPlugin %d:\n", i + 1);
            printf("  Purpose: %s\n", loaded_plugins[i].plugin_metadata.plugin_purpose);
            printf("  Author: %s\n", loaded_plugins[i].plugin_metadata.plugin_author);
            printf("  Options:\n");
            for (size_t j = 0; j < loaded_plugins[i].plugin_metadata.sup_opts_len; j++) {
                printf("    --%s    %s\n", loaded_plugins[i].plugin_metadata.sup_opts[j].opt.name, loaded_plugins[i].plugin_metadata.sup_opts[j].opt_descr);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    char *DEBUG = getenv("LAB1DEBUG");
    char *plugin_directory = ".";
    int use_and_logic = 1;
    int invert_results = 0;
    int show_version = 0;
    int show_help = 0;

    if (DEBUG) {
        printf("DEBUG: Program started\n");
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-P") == 0 && i + 1 < argc) {
            plugin_directory = argv[i + 1];
            break;
        }
    }

    if (DEBUG) {
        printf("DEBUG: Scanning for plugins in: %s\n", plugin_directory);
    }
    scan_directory_for_plugins(plugin_directory);

    struct option *long_options = calloc(plugin_count * MAX_PLUGIN_OPTS + 7, sizeof(struct option));
    if (!long_options) {
        fprintf(stderr, "ERROR: Memory allocation failed for long_options\n");
        exit(EXIT_FAILURE);
    }
    int long_options_length = 0;

    long_options[long_options_length++] = (struct option){"P", required_argument, 0, 'P'};
    long_options[long_options_length++] = (struct option){"A", no_argument, 0, 'A'};
    long_options[long_options_length++] = (struct option){"O", no_argument, 0, 'O'};
    long_options[long_options_length++] = (struct option){"N", no_argument, 0, 'N'};
    long_options[long_options_length++] = (struct option){"v", no_argument, 0, 'v'};
    long_options[long_options_length++] = (struct option){"h", no_argument, 0, 'h'};
    long_options[long_options_length++] = (struct option){"d", no_argument, 0, 'd'};

    for (int i = 0; i < plugin_count; i++) {
        initialize_plugin(&loaded_plugins[i]);
        if (loaded_plugins[i].plugin_handle) {
            for (size_t j = 0; j < loaded_plugins[i].plugin_metadata.sup_opts_len; j++) {
                long_options[long_options_length++] = loaded_plugins[i].plugin_metadata.sup_opts[j].opt;
            }
        }
    }

    if (DEBUG) {
        printf("DEBUG: Parsing command line options\n");
    }

    int option_index = 0;
    int opt;
    while ((opt = getopt_long(argc, argv, "P:AOvNhd", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'P':
                plugin_directory = optarg;
                break;
            case 'A':
                use_and_logic = 1;
                break;
            case 'O':
                use_and_logic = 0;
                break;
            case 'N':
                invert_results = 1;
                break;
            case 'v':
                show_version = 1;
                break;
            case 'h':
                show_help = 1;
                break;
            default:
                for (int i = 0; i < plugin_count; i++) {
                    for (size_t j = 0; j < loaded_plugins[i].plugin_metadata.sup_opts_len; j++) {
                        if (strcmp(long_options[option_index].name, loaded_plugins[i].plugin_metadata.sup_opts[j].opt.name) == 0) {
                            if (loaded_plugins[i].active_options_length < MAX_PLUGIN_OPTS) {
                                loaded_plugins[i].active_options[loaded_plugins[i].active_options_length] = loaded_plugins[i].plugin_metadata.sup_opts[j].opt;
                                loaded_plugins[i].option_args[loaded_plugins[i].active_options_length] = optarg;
                                loaded_plugins[i].active_options_length++;
                            } else {
                                fprintf(stderr, "ERROR: Maximum options per plugin exceeded\n");
                            }
                            break;
                        }
                    }
                }
                break;
        }
    }

    if (show_version) {
        printf("Version: 1.0.0\nAuthor: Custom Author\n");
    }

    if (show_help) {
        display_help();
    }

    if (optind < argc) {
        const char *directory = argv[optind];
        if (DEBUG) {
            printf("DEBUG: Searching in directory: %s\n", directory);
        }
        search_directory(directory, use_and_logic, invert_results);
    }

    free(long_options);
    release_plugins();

    if (DEBUG) {
        printf("DEBUG: Program completed\n");
    }

    return 0;
}
