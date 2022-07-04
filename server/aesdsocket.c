/**
 * @file aesdsocket.c
 * @author Pasakorn Tiwatthanont (ptiwatthanont@gmail.com)
 * @brief aesdsocket server
 * @version 0.1
 * @date 2022-07-04
 *
 * @copyright GNU Public License.
 *
 * Original from https://beej.us/guide/bgnet/html/#client-server-background
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


#define PORT "9000"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold


void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}


/**
 * @brief Get the in addr object, IPv4 or IPv6
 *
 * @param sa
 * @return void*
 */
void * get_in_addr(struct sockaddr * sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


/**
 * @brief Main
 *
 * @return int
 */
int main(void) {
    int                     sockfd, new_fd;  // Listen on sock_fd, new connection on new_fd
    struct addrinfo         hints, *servinfo, *p;

    struct sockaddr_storage their_addr;  // Connector's address information
    socklen_t               sin_size;

    struct sigaction        sa;

    int                     yes = 1;
    char                    s[INET6_ADDRSTRLEN];
    int                     rv;


    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE; // use my IP


    // Get addr. info.
    // https://beej.us/guide/bgnet/html/#getaddrinfoprepare-to-launch
    rv = getaddrinfo(NULL,          // (input) String of IP. None -- as a server
                     PORT,          // (input) String of Port number, or, "http" for example
                     &hints,        // (input) Struct addrinfo
                     &servinfo);    // (output) Struct addrinfo -- what we want!
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }


    // loop through all the results, and, bind to the first we can
    // WHY does it has so many?
    for (p = servinfo; p != NULL; p = p->ai_next) {

        // Pick one
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("server: socket");
            continue;
        }

        // set SO_REUSEADDR on a socket to true (1):
        if (setsockopt(sockfd,
                       SOL_SOCKET,      // Level
                       SO_REUSEADDR,    // Optname: Option name where Optval depends on it.
                                        // Allows other sockets to bind() to this port,
                                        //  unless there is an active listening socket bound to the port already.
                                        // This enables you to get around those “Address already in use” error messages
                                        //  when you try to restart your server after a crash.
                       &yes,            // Optval
                       sizeof(int)      // Optlen
                       ) == -1) {
            perror("setsockopt");
            exit(-1);
        }

        // Bind it to the port we passed in to getaddrinfo().
        // Anyways, commonly done on listen() for incoming connections on a specific port.
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }


    freeaddrinfo(servinfo);  // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(-1);
    }

    // Listen
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(-1);
    }


    // Set signal handler
    sa.sa_handler = sigchld_handler;  // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(-1);
    }


    // Server ready
    printf("server: waiting for connections...\n");

    while (1) {  // main accept() loop

        // Accept an incoming connection on a listening socket
        sin_size = sizeof their_addr;
        new_fd   = accept(sockfd,                           // The listen()ing socket descriptor.
                          (struct sockaddr *)&their_addr,   // This is filled in with the address of the site that’s connecting to you.
                          &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        // Convert IP addresses to human-readable form and back.
        // n for network, and, p for presentation
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s,
                  sizeof s);
        printf("server: got connection from %s\n", s);

        // Fork:
        // Parent -- go to listen waiting for incomming again.
        // Child -- do the task.
        if (!fork()) {      // this is the child process

            // ### Child Process -- Begin ###
            close(sockfd);  // child doesn't need the listener

            if (send(new_fd,            // Destined sockfd
                     "Hello, world!",   // Data
                     13,                // Data length
                     0                  // Flag
                     ) == -1)
                perror("send");
            close(new_fd);
            exit(0);
            // ### Child Process -- End ###

        }
        close(new_fd);  // parent doesn't need this
    }

    return 0;
}
