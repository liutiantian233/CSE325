#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <zconf.h>

int main(int argc, const char * argv[]) {
    // Initialization parameter
    int parameter_a = 0, parameter_t = 0;
    // Initialize the buffers
    long buffers = 256;
    char * source = NULL;  // Initialize the source point
    char * destination = NULL;  // Initialize the destination point

    // For loop for all parameters
    for (int i = 1; i < argc; i++) {

        // When the parameter is -a
        if (!strncmp(argv[i], "-a", strlen(argv[i]))) {
            parameter_a = 1;
        } else if (!strncmp(argv[i], "-t", strlen(argv[i]))) {  // When the parameter is -t
            parameter_t = 1;
        } else if (!strncmp(argv[i], "-b", strlen(argv[i]))) {  // When the parameter is -b
            i = i + 1;

            // For loop for num parameters
            for (int k = 0; k < strlen(argv[i]); k++) {
                if (!isdigit(argv[i][k])) {
                    printf("Parameter type error.\n");
                    return 0;
                }
                char *point;
                buffers = strtol(argv[i], &point, 10);  // Turn into Numbers
            }
        } else {
            if (source == NULL) {  // Find the source file parameters
                source = (char *) argv[i];
            } else if (destination == NULL) {  // Find the destination file parameters
                destination = (char *) argv[i];
            }
        }
    }

    // Make judgments about special cases
    if (source == NULL || destination == NULL || (parameter_a && parameter_t)) {
        printf("Parameter error.\n");
        return 0;
    }

    // No source files
    if (access(source, F_OK)) {
        printf("The source file does not exist.\n");
        return 0;
    }

    // There is no parameter
    if (!parameter_a && !parameter_t) {
        printf("Without modification. Parameter not specified.\n");
        return 0;
    }

    // Copy and paste
    int open_destination = 0;
    int open_source = open(source, O_RDONLY, 0666);

    // When the parameter is -a
    if (parameter_a == 1) {
        open_destination = open(destination, O_CREAT | O_WRONLY | O_APPEND, 0666);
    }
    // When the parameter is -t
    if (parameter_t == 1) {
        open_destination = open(destination, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    }
    // File cannot be opened
    if (open_source == -1) {
        printf("Source file failed to open.\n");
        return 0;
    }

    // Create a file
    char file_buffer[buffers];
    int read_file, write_file;
    read_file = read(open_source, file_buffer, buffers);

    // The source file is unreadable
    if (read_file == -1) {
        printf("Source file failed to read.\n");
        return 0;
    }
    // The destination file is cannot be written
    write_file = write(open_destination, file_buffer, read_file);
    if (write_file == -1) {
        printf("Destination file failed to write.\n");
        return 0;
    }

    // Close the file
    int close_source = close(open_source);
    int close_destination = close(open_destination);
    // Close the file have fail
    if (close_source == -1 || close_destination == -1) {
        printf("Failed to close the file.\n");
        return 0;
    }
    printf("Copy the file successfully.\n");
    printf("From %s to %s\n", source, destination);
    return 0;
}

