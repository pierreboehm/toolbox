#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define TRUE  1
#define FALSE 0

int is_host_reachable(char *host) {
    int result = TRUE;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port   = htons(65432);
    inet_pton(AF_INET, host, &sin.sin_addr);

    // errno 111 - connection refused (no service at specified port)
    // errno 113 - no route to host (host not reachable)

    if (connect(sockfd, (struct sockaddr *) &sin, sizeof(sin)) == -1) {
        if (errno == 113) {
            result = FALSE;
        }
    }

    shutdown(sockfd, 0);

    return result;
}