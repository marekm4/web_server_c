#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define VERSION 25
#define BUFSIZE 65536 /* larger = more efficient */
#define ERROR      42
#define LOG        44
#define FORBIDDEN 403
#define NOTFOUND  404
#ifndef SIGCLD
#define SIGCLD SIGCHLD
#endif

/* this is a child web server process, so we can exit on errors */
void web(int fd, int hit)
{
    long ret;
    static char buffer[BUFSIZE+1]; /* static so zero filled */

    ret =read(fd,buffer,BUFSIZE); 	/* read Web request in one go */
    if(ret == 0 || ret == -1) {	/* read failure stop now */
        perror("Read");
    }
    if(ret > 0 && ret < BUFSIZE)	/* return code is valid chars */
        buffer[ret]=0;		/* terminate the buffer */
    else buffer[0]=0;

    (void)sprintf(buffer,"HTTP/1.1 200 OK\n\n"); /* Header + a blank line */
    (void)write(fd,buffer,strlen(buffer));

    sleep(1);	/* allow socket to drain before signalling the socket is closed */
    close(fd);
    exit(1);
}

int main(int argc, char **argv)
{
    char s[] = {0xe2, 0x9d, 0xa4, 0xef, 0xb8, 0x8f, 0xf0, 0x9f, 0x8d, 0x8b, 0x00};
    printf("%s\n", s);
    int port, pid, listenfd, socketfd, hit;
    socklen_t length;
    static struct sockaddr_in cli_addr; /* static = initialised to zeros */
    static struct sockaddr_in serv_addr; /* static = initialised to zeros */

    (void)signal(SIGCLD, SIG_IGN); /* ignore child death */
    /* setup the network socket */
    if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0)
        perror("Socket");
    port = atoi(getenv("PORT"));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0)
        perror("Bind");
    if( listen(listenfd,64) <0)
        perror("Listen");
    for(hit=1; ;hit++) {
        length = sizeof(cli_addr);
        if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0)
            perror("Accept");
        if((pid = fork()) < 0) {
            perror("Fork");
        }
        else {
            if(pid == 0) { 	/* child */
                (void)close(listenfd);
                web(socketfd,hit); /* never returns */
            } else { 	/* parent */
                (void)close(socketfd);
            }
        }
    }
}