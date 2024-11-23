#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BACKLOG 10

int main(void) {
    int sockfd, new_fd;
    socklen_t addrlen;
    struct addrinfo hints, *res, *rp;
    struct sockaddr_storage client_addr;

    const char *msg = "Yeehaw it works\n";
    int len, bytes_sent;

    // Get address to bind to
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, "3490", &hints, &res) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // Try to bind to each address until we succeed
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (sockfd == -1)
            continue;
        if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;

        close(sockfd);
    }

    freeaddrinfo(res);

    // If we couldn't bind, exit
    if (rp == NULL) {
        perror("Could not bind");
        exit(EXIT_FAILURE);
    }

    listen(sockfd, BACKLOG);

    // Accept a connection
    addrlen = sizeof client_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);

    // Send a message back to the client
    len = strlen(msg);
    bytes_sent = send(new_fd, msg, len, 0);

    close(new_fd);
}
