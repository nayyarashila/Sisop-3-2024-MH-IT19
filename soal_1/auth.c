#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY 0x00001234
#define MAX_FILENAME_SIZE 100
#define MAX_FILEDATA_SIZE 10000

struct shared_data {
    int count;
    struct {
        char filename[MAX_FILENAME_SIZE];
        char filedata[MAX_FILEDATA_SIZE];
    } files[2]; // Increase this if you want to handle more files
};

int main() {
    DIR *d;
    struct dirent *dir;
    d = opendir("./new-data");
    if (d) {
        int shmid = shmget(SHM_KEY, sizeof(struct shared_data), 0644|IPC_CREAT);
        if (shmid == -1) {
            perror("shmget");
            exit(1);
        }
        struct shared_data *data = shmat(shmid, NULL, 0);
        if (data == (void *) -1) {
            perror("shmat");
            exit(1);
        }
        data->count = 0;
        while ((dir = readdir(d)) != NULL && data->count < 2) {
            if (strstr(dir->d_name, "trashcan.csv") || strstr(dir->d_name, "parkinglot.csv")) {
                strncpy(data->files[data->count].filename, dir->d_name, MAX_FILENAME_SIZE);

                char filepath[1024];
                sprintf(filepath, "./new-data/%s", dir->d_name);
                FILE *file = fopen(filepath, "r");
                if (file != NULL) {
                    size_t new_len = fread(data->files[data->count].filedata, sizeof(char), MAX_FILEDATA_SIZE-1, file);
                    if (new_len == 0) {
                        fputs("Error reading file", stderr);
                    } else {
                        data->files[data->count].filedata[++new_len] = '\0'; // Just to be safe
                    }
                    fclose(file);
                }

                data->count++;
            } else {
                char filepath[1024];
                sprintf(filepath, "./new-data/%s", dir->d_name);
                remove(filepath);
            }
        }
        closedir(d);
        shmdt(data);
    }
    return 0;
}
