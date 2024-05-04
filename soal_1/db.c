#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

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
    // Attach to shared memory
    int shmid = shmget(SHM_KEY, sizeof(struct shared_data), 0644);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    struct shared_data *data = shmat(shmid, NULL, 0);
    if (data == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    // Open the log file
    FILE *log_file = fopen("db.log", "a");
    if (log_file == NULL) {
        perror("fopen");
        exit(1);
    }

    for (int i = 0; i < data->count; i++) {
        // Create the new file path
        char new_filepath[1024];
        sprintf(new_filepath, "./database/%s", data->files[i].filename);

        // Write the file data to the new file
        FILE *new_file = fopen(new_filepath, "w");
        if (new_file == NULL) {
            perror(new_filepath); // Print the error message
            exit(1);
        } else {
            fputs(data->files[i].filedata, new_file);
            fclose(new_file);
        }

        // Get the current time
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        // Write the log entry
        fprintf(log_file, "[%02d/%02d/%04d %02d:%02d:%02d] [%s] [%s]\n",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec,
                strstr(data->files[i].filename, "trashcan") ? "Trash Can" : "Parking Lot",
                data->files[i].filename);

        // Remove the original file
        char old_filepath[1024];
        sprintf(old_filepath, "/home/azrael/sisop/modul3/soal1/new-data/%s", data->files[i].filename);
        if (remove(old_filepath) != 0) {
            perror(old_filepath); // Print the error message
            exit(1);
        }
    }

    // Close the log file
    fclose(log_file);

    // Detach from shared memory
    shmdt(data);

    return 0;
}
