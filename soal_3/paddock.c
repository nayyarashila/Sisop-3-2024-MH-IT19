#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "actions.c"

#define PORT 8080
#define BUFFER_SIZE 1024
#define LOG_FILE "/home/azrael/sisop/modul3/soal3/server/race.log"

// Function to write log
void write_log(char *source, char *command, char *info, const char *response) {
    chdir("/home/azrael/sisop/modul3/soal3/server"); // Change working directory
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(log_file, "[%s] [%02d/%02d/%04d %02d:%02d:%02d]: [%s] [%s]\n", source, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, command, info);
    fprintf(log_file, "[Paddock] [%02d/%02d/%04d %02d:%02d:%02d]: [%s] [%s]\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, command, response);
    fclose(log_file);
}

void wallahidaemon() {
      pid_t pid, sid;

  pid = fork();

  if (pid < 0) {
    exit(EXIT_FAILURE);
  }

  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  umask(0);

  sid = setsid();
  if (sid < 0) {
    exit(EXIT_FAILURE);
  }

  if ((chdir("/home/azrael/sisop/modul3/soal3/server")) < 0) {
    exit(EXIT_FAILURE);
  }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    //open specific file that acts like a blackhole (everything that happen in it will be erased immediately)
    stdin = fopen ("/dev/null", "r");
    stdout = fopen ("/dev/null", "w+");
    stderr = fopen ("/dev/null", "w+");
}


int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create daemon process
    wallahidaemon();

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    while (1) {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Read command from driver
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_read < 0) {
            perror("Read failed");
            close(new_socket);
            continue;
        }
        buffer[bytes_read] = '\0'; // Add null terminator

        // Parse command and info
        char *command = strtok(buffer, "|");
        char *info = strtok(NULL, "\n");

        // Process command
        const char *result;
        if (strcmp(command, "Gap") == 0) {
            float distance = atof(info);
            result = gap(distance);
        } else if (strcmp(command, "Fuel") == 0) {
            result = fuel(info);
        } else if (strcmp(command, "Tire") == 0) {
            int tire_usage = atoi(info);
            result = tire(tire_usage);
        } else if (strcmp(command, "Tire Change") == 0) {
            result = tire_change(info);
        } else {
            printf("Invalid command\n");
            close(new_socket);
            continue;
        }

        // Send result to driver
        int bytes_sent = send(new_socket, result, strlen(result), 0);
        if (bytes_sent < 0) {
            perror("Send failed");
        }

        // Write log
        write_log("Driver", command, info, result);

        close(new_socket);
    }

    return 0;
}
