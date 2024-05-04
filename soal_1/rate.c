#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    for (int i = 0; i < data->count; i++) {
        // Make a copy of the file data
        char filedata_copy[MAX_FILEDATA_SIZE];
        strcpy(filedata_copy, data->files[i].filedata);

        // Process the CSV data from the copy
        char *line = strtok(filedata_copy, "\n");
        float max_rating = 0.0;
        char best_place[1024] = {0};
        char best_filename[MAX_FILENAME_SIZE] = {0};
        strcpy(best_filename, data->files[i].filename); // Copy the file name from shared memory

        // Read the CSV data line by line
        while (line != NULL) {
            char place[1024];
            float rating;
            // Assuming the CSV format is: place_name,rating
            if (sscanf(line, "%[^,],%f", place, &rating) == 2) {
                if (rating > max_rating) {
                    max_rating = rating;
                    strcpy(best_place, place);
                }
            }
            line = strtok(NULL, "\n");
        }

        // Output the best place with the highest rating
        printf("Output:\n");
        printf("Type: %s\n", strstr(best_filename, "trashcan") ? "Trash Can" : "Parking Lot");
        printf("Filename: %s\n", best_filename);
        printf("------------------------\n");
        printf("Name: %s\n", best_place);
        printf("Rating: %.2f\n", max_rating);
    }

    // Detach from shared memory
    shmdt(data);

    return 0;
}
