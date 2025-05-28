#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    //this checks for correct number of arguments
    if (argc != 3) {
        printf("Insufficient parameters passed.\n");
        return 1;
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    //open input with error cherck
    int input_fd = open(input_file, O_RDONLY);
    if (input_fd == -1) {
        perror("Error opening input file");
        return 1;
    }

    // if file exists this removes it
    unlink(output_file);

    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("Error creating output file");
        close(input_fd);
        return 1;
    }

    //copy content with loop and check errors
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(output_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to output file");
            close(input_fd);
            close(output_fd);
            return 1;
        }
    }
    if (bytes_read == -1) {
        perror("Error reading input file");
    }

    //close files
    close(input_fd);
    close(output_fd);

    printf("The contents of file %s have been succesfully copied into the %s file.\n", input_file, output_file);
    return 0;
}
