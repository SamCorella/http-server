#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#define BACKLOG 10

int main() {
    int status;
    int sfd, new_fd;
    socklen_t addr_size;
    struct addrinfo hints;
    struct addrinfo *res, *rp;
    struct sockaddr_storage client_addr;

    char *msg = "Yeehaw it works\n";
    int len, bytes_sent;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, "3490", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

        if (sfd == -1)
            continue;
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;

        close(sfd);
    }

    freeaddrinfo(res);

    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(1);
    }

    listen(sfd, BACKLOG);

    addr_size = sizeof client_addr;
    new_fd = accept(sfd, (struct sockaddr *)&client_addr, &addr_size);

    len = strlen(msg);
    bytes_sent = send(new_fd, msg, len, 0);
    close(new_fd);
}
