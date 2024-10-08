#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>

/*
 ___                  _                     _    _
| __|__ _ ____  _    /_\   ______ ___ _ __ | |__| |___ _ _
| _|/ _` (_-< || |  / _ \ (_-<_-</ -_) '  \| '_ \ / -_) '_|
|___\__,_/__/\_, | /_/ \_\/__/__/\___|_|_|_|_.__/_\___|_|
             |__/
Designed by: M.H Aghaee - 0xTFS
*/

#define MAX_FILENAME_LENGTH 256
#define MAX_CMD_LENGTH 1024

#define YELLOW "\033[33m"
#define RED    "\033[31m"
#define RESET  "\033[0m"

int file_exists(const char *filename) {
    return access(filename, F_OK) == 0;
}

void handle_ld_output(FILE *fp, int verbose) {
    char output_line[256];
    while (fgets(output_line, sizeof(output_line), fp) != NULL) {
        if (strstr(output_line, "warning: cannot find entry symbol _start") != NULL) {
            printf(YELLOW "WAR: " YELLOW RED "Unable to find entry symbol (_start).\n" RESET);
            printf("------------------------------------------\n\n");
        } else {
            printf("%s", output_line);
        }
    }
}

void print_verbose(const char *message) {
    printf(YELLOW "VERBOSE: %s" RESET "\n", message);
}



int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(RED " ___                  _                     _    _\n");
        printf("| __|__ _ ____  _    /_\\   ______ ___ _ __ | |__| |___ _ _\n");
        printf("| _|/ _` (_-< || |  / _ \\ (_-<_-</ -_) '  \\| '_ \\ / -_) '_|\n");
        printf("|___\\__,_/__/\\_, | /_/ \\_\\/__/__/\\___|_|_|_|_.__/_\\___|_|  v1.1\n");
        printf("             |__/\n" YELLOW "                                    BY: M.H Aghaee - 0xTFS\n\n" RESET);
        printf("Usage: asm <filename.s/.asm> [Switches]\n");
        printf("----------------------------\n");
        printf("        -nof | --no-output-file         .o and executable files will be deleted\n");
        printf("                                        after running the code.\n");
        printf("        -g   | --gdb-debug              run the assembly through gdb.\n");
        printf("        -v   | --verbose                verbose mode.\n");
        printf("----------------------------\n\n");
        printf("Example:\n");
        printf("    easm helloworld.asm -nof -v -g\n");
        printf("    easm helloworld.asm --no-output-file --verbose --gdb-debug\n \n");
        return 1;
    }

    int remove_output_files = 0;
    int debug_mode = 0;
    int verbose_mode = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-nof") == 0 || strcmp(argv[i], "--no-output-file") == 0) {
            remove_output_files = 1;
        } else if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--gdb-debug") == 0) {
            debug_mode = 1;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose_mode = 1;
        } else {
            printf(RED "ERR: Unknown argument: %s\n" RESET, argv[i]);
            return 1;
        }
    }

    char *dot = strrchr(argv[1], '.');
    if (dot == NULL || (strcmp(dot + 1, "s") != 0 && strcmp(dot + 1, "asm") != 0)) {
        printf(RED "ERR:" RED YELLOW " Unsupported file extension. Please use .s or .asm\n" RESET);
        return 1;
    }

    char fileName[MAX_FILENAME_LENGTH];
    snprintf(fileName, sizeof(fileName), "%.*s", (int)(dot - argv[1]), argv[1]);

    if (!file_exists(argv[1])) {
        printf(RED "ERR:" RED YELLOW " provided file not found: %s\n" RESET, argv[1]);
        return 1;
    }

    struct utsname unameData;
    uname(&unameData);
    char *arch = unameData.machine;

    char *nasm_format;
    if (strcmp(arch, "x86_64") == 0) {
        nasm_format = "elf64";
    } else if (strcmp(arch, "i686") == 0 || strcmp(arch, "i386") == 0) {
        nasm_format = "elf32";
    } else {
        printf(RED "ERR:" RED YELLOW " Unsupported system architecture: %s\n" RESET, arch);
        return 1;
    }

    if (strcmp(nasm_format, "elf32") == 0) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            printf(RED "ERR:" RED YELLOW " Failed to open file.\n" RESET);
            return 1;
        }

        char line[MAX_FILENAME_LENGTH];
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "r8") || strstr(line, "r9") ||
                strstr(line, "rax") || strstr(line, "rbx") ||
                strstr(line, "rcx") || strstr(line, "rdx") ||
                strstr(line, "rsi") || strstr(line, "rdi") ||
                strstr(line, "mov r")) {
                printf(RED "ERR: The provided assembly code is not supported by your system's architecture.\n" RESET);
                fclose(file);
                return 1;
            }
        }
        fclose(file);
    }

    char nasm_cmd[MAX_CMD_LENGTH], ld_cmd[MAX_CMD_LENGTH];
    snprintf(nasm_cmd, sizeof(nasm_cmd), "nasm -f %s %s.%s", nasm_format, fileName, dot + 1);
    snprintf(ld_cmd, sizeof(ld_cmd), "ld %s.o -o %s 2>&1", fileName, fileName);

    if (verbose_mode) print_verbose("Running NASM command...");
    if (system(nasm_cmd) != 0) {
        printf(RED "ERR: NASM compilation failed.\n" RESET);
        return 1;
    }

    if (verbose_mode) print_verbose("Running linker command...");
    FILE *ld_fp = popen(ld_cmd, "r");
    if (ld_fp == NULL) {
        perror("popen failed");
        return 1;
    }

    handle_ld_output(ld_fp, verbose_mode);
    pclose(ld_fp);

    char exec_file[MAX_FILENAME_LENGTH + 3];
    snprintf(exec_file, sizeof(exec_file), "./%s", fileName);

    if (file_exists(exec_file)) {
        if (verbose_mode) print_verbose("Running the executable...");
        system(exec_file);
    } else {
        printf(RED "ERR: Executable file not found: %s\n" RESET, exec_file);
    }

    if (debug_mode) {
        char gdb_cmd[MAX_CMD_LENGTH];
        snprintf(gdb_cmd, sizeof(gdb_cmd), "gdb -q %s", exec_file);
        if (verbose_mode) print_verbose("Running gdb...");
        system(gdb_cmd);
    }

    

    if (remove_output_files) {
        char object_file[MAX_FILENAME_LENGTH + 2];
        snprintf(object_file, sizeof(object_file), "%s.o", fileName);

        if (verbose_mode) print_verbose("Removing output files...");
        if (remove(object_file) != 0) {
            perror(RED "ERR: Failed to remove object file.\n" RESET);
        }
        if (remove(exec_file) != 0) {
            perror(RED "ERR: Failed to remove executable file.\n" RESET);
        }
    }

    return 0;
}
