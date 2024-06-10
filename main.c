#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER 256
char template[] = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html; charset=utf-8\r\n\r\n%s";

int main(void) {
    FILE *file;
    if ((file = fopen("index.html", "r")) == NULL)
        perror("Content");
    long length;
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content;
    if ((content = malloc(length + 1)) == NULL)
        perror("Malloc");
    fread(content, 1, length, file);
    content[length] = 0;
    fclose(file);

    char *response;
    if (asprintf(&response, template, strlen(content), content) < 0)
        perror("Response");

    int server_fd, client_fd;
    static struct sockaddr_in6 client_address;
    static struct sockaddr_in6 server_address;

    if ((server_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
        perror("Socket");

    int one = 1;
    if ((setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int))) < 0)
        perror("Option");

    int port = atoi(getenv("PORT"));
    server_address.sin6_family = AF_INET6;
    server_address.sin6_addr = in6addr_any;
    server_address.sin6_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        perror("Bind");

    if (listen(server_fd, 128) < 0)
        perror("Listen");

    char buf[BUFFER];
    while (1) {
        socklen_t client_address_length = sizeof(client_address);
        if ((client_fd = accept(server_fd, (struct sockaddr *) &client_address, &client_address_length)) < 0)
            perror("Accept");

        if (write(client_fd, response, strlen(response)) < 0)
            perror("Write");

        if (shutdown(client_fd, SHUT_WR) < 0)
            perror("Shutdown");

        int n;
        do {
            if ((n = read(client_fd, buf, BUFFER)) < 0)
                perror("Read");
        } while(n > 0);

        close(client_fd);
    }
}
