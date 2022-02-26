#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 65536

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

    if (listen(server_fd, 64) < 0)
        perror("Listen");

    while (1) {
        socklen_t client_address_length = sizeof(client_address);
        if ((client_fd = accept(server_fd, (struct sockaddr *) &client_address, &client_address_length)) < 0)
            perror("Accept");

        long ret;
        static char buffer[BUFSIZE + 1]; /* static so zero filled */

        ret = read(client_fd, buffer, BUFSIZE);    /* read Web request in one go */
        if (ret == 0 || ret == -1) {    /* read failure stop now */
            perror("Read");
        }
        if (ret > 0 && ret < BUFSIZE)    /* return code is valid chars */
            buffer[ret] = 0;        /* terminate the buffer */
        else buffer[0] = 0;

        char emojis[] = {0xe2, 0x9d, 0xa4, 0xef, 0xb8, 0x8f, 0xf0, 0x9f, 0x8d, 0x8b, 0x00};
        int content_length = strlen(emojis) + 24;
        sprintf(buffer, "HTTP/1.1 200 OK\nContent-Length: %d\n\n<meta charset=\"utf-8\">\n%s\n", content_length,
                emojis);
        write(client_fd, buffer, strlen(buffer));

        close(client_fd);
    }
}