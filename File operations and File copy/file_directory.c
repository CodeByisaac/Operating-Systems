#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void create_directory(const char *dir_name, mode_t mode); //create directory with given name and permission
void create_write_file(const char *file_name, const char *content); //creates file and writes in it
void read_proc_mem_info_to_file(const char *file_name); //reads memory info and saves to file
void directory_listing(const char *start_dir); //lists files in directory recursively
void remove_directory(const char *dir_name); // removes directory from system with y/n warning

//defines main function
int main() {
    int choice;
    char userinput[256];

    while (1) {
        printf("\n File and Directory Operations \n");
        printf("1. Create directory\n");
        printf("2. Create and Write to File\n");
        printf("3. Save /proc/meminfo to File\n");
        printf("4. List directory recursively\n");
        printf("5. Remove directory recursively\n");
        printf("99. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        switch (choice){
            case 1:
                printf("Enter directory name to create:");
                fgets(userinput, sizeof(userinput), stdin);
                userinput[strcspn(userinput, "\n")] = 0;
                create_directory(userinput, 0755);
                break;
            case 2:
                printf("Enter the file name");
                fgets(userinput, sizeof(userinput), stdin);
                userinput[strcspn(userinput, "\n")] = 0;
                create_write_file(userinput, "Operating systems is Fun!!!");
                break;

            case 3:
                printf("Enter output file name for /proc/meminfo:");
                fgets(userinput, sizeof(userinput), stdin);
                userinput[strcspn(userinput, "\n")] = 0;
                read_proc_mem_info_to_file(userinput);
                break;

            case 4:
                printf("Enter directory to list");
                fgets(userinput, sizeof(userinput), stdin);
                userinput[strcspn(userinput, "\n")] = 0;
                directory_listing(userinput);
                break;

            case 5:
                printf("Enter directory you want to remove: ");
                fgets(userinput, sizeof(userinput), stdin);
                userinput[strcspn(userinput, "\n")] = 0;
                remove_directory(userinput);
                break;

            case 99:
                exit(0);
            default:
                printf("Invalid choice try again. \n");
        }
    }

    return 0;
}

// implementing our functions
void create_directory(const char *dir_name, mode_t mode) {
    if (mkdir(dir_name, mode) == -1){
        perror("Error creating drectory");
    }else{
        printf("Directory '%s' created. \n", dir_name);
    }
}

void create_write_file(const char *file_name, const char *content){
    int fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1){
        perror("Error creating or opening file");
        return;
    }
    if (write(fd, content, strlen(content)) == -1){
        perror("Error writing to file");
    }else{
        printf("File '%s' created and written successfully.\n", file_name);
    }
    close(fd);
}

void read_proc_mem_info_to_file(const char *file_name){
    FILE *src = fopen("/proc/meminfo", "r");
    if (src == NULL){
        perror("Error opening /proc/meminfo");
        return;
    }
    FILE *dest = fopen(file_name, "w");
    if (dest == NULL){
        perror("Error creating output file");
        fclose(src);
        return;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), src) != NULL){
        fputs(buffer, dest);
    }
    fclose(src);
    fclose(dest);
    printf("Memory infom saved to '%s'.\n", file_name);
}

void directory_listing(const char *start_dir){
    DIR *dir = opendir(start_dir);
    if (dir == NULL){
        perror("Error opening directory");
        return;
    }
    struct dirent *entry;
    char path[1024];

    while ((entry = readdir(dir)) != NULL){
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(path, sizeof(path), "%s/%s", start_dir, entry->d_name);
        printf("%s\n", path);

        // does the recursing
        if (entry->d_type == DT_DIR){
            directory_listing(path);
        }
    }
    closedir(dir);
}

void remove_directory(const char *dir_name){
    char confirm;
    printf("Warning: you are about to delete the directory '%s' and all its contents. Proceed? (y/n): ", dir_name);
    scanf(" %c", &confirm);
    getchar();

    if (confirm != 'y' && confirm != 'Y'){
        printf("Operation cancelled.\n");
        return;
    }
    DIR *dir = opendir(dir_name);
    if (!dir){
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    char path[1024];

    while ((entry = readdir(dir)) != NULL){
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

        if (entry->d_type == DT_DIR){
            remove_directory(path);
        }else{
            if (remove(path) != 0){
                perror("error deleting file");
            }
        }
    }
    closedir(dir);
    if (rmdir(dir_name) != 0){
        perror("Error removing directory");
    }else{
        printf("Directory '%s' deleted.\n", dir_name);
    }
}
