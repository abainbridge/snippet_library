#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int main() {    
    // Create a TCP socket.
    int listening_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_sock < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Bind the socket to a port.
    struct sockaddr_in server_addr = { 0 };
    enum { port = 5001 };
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(listening_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }
        
    // Start listening for the clients. Process will sleep waiting for the
    // incoming connection.
    listen(listening_sock, 5);
    
    // Accept connection from a client.
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_sock = accept(listening_sock, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sock < 0) {
        perror("ERROR on accept");
        exit(1);
    }

    puts("Client connected");
    
    // If connection is established then start communicating 
    char buffer[256] = {};
    int n = read(client_sock, buffer, sizeof(buffer));
    if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }
    
    printf("Here is the message: %s\n", buffer);
    
    // Write a response to the client.
    char const *response = "I got your message";
    n = write(client_sock, response, strlen(response) + 1);
    if (n < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
}
