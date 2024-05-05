#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT 8080
#define IP "127.0.0.1"
#define BUFFER_SIZE 1024

void read_input(char *input, int max_length) {
    fgets(input, max_length, stdin);
    input[strcspn(input, "\n")] = '\0'; // Deleting newline from the input
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char command[BUFFER_SIZE] = {0};
    char info_input[BUFFER_SIZE] = {0};

    //Create new socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //Set the server address and port number
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    //Convert the IP address from text format to binary format and store it in the server address structure
    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    //Make a connection to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read Command and Info
        printf("Command (Gap/Fuel/Tire/Tire Change): ");
        read_input(command, BUFFER_SIZE);
        printf("Info: ");
        read_input(info_input, BUFFER_SIZE);
        char info[BUFFER_SIZE] = {0};
        strncpy(info, info_input, BUFFER_SIZE - 1);

        // Sending command and info
        char combined_command_info[BUFFER_SIZE] = {0};
        sprintf(combined_command_info, "%s|%s", command, info); // Using "|" as a delimiter
        send(sock, combined_command_info, strlen(combined_command_info), 0);

        // Receive respons from the server
        int bytes_read = read(sock, buffer, BUFFER_SIZE);
        if (bytes_read < 0) {
            perror("Read failed");
            exit(1);
        }
        buffer[bytes_read] = '\0';

        // Showing the output
        printf("[Driver] : [%s] [%s]\n", command, info);
        printf("[Paddock]: [%s]\n", buffer);

        // Cleaning Buffer
        memset(buffer, 0, sizeof(buffer));

        // Stop after finished
        break;
    }

    return 0;
}
