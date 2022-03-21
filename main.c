#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 38\r\n\r\n<meta charset=\"utf-8\">\n\xf0\x9f\x90\xa7\xe2\x9d\xa4\xef\xb8\x8f\xf0\x9f\x8d\x8b\n";

int main() {
    int server_fd, client_fd;
    static struct sockaddr_in client_address;
    static struct sockaddr_in server_address;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("Socket");

    int one = 1;
    if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int))) < 0)
        perror("Option");

    int port = atoi(getenv("PORT"));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        perror("Bind");

    if (listen(server_fd, 128) < 0)
        perror("Listen");

    while (1) {
        socklen_t client_address_length = sizeof(client_address);
        if ((client_fd = accept(server_fd, (struct sockaddr *) &client_address, &client_address_length)) < 0)
            perror("Accept");

        if (write(client_fd, response, strlen(response)) < 0)
            perror("Write");

        close(client_fd);
    }
}
