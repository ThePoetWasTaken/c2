#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "base.h"
#include "commands.h"

static volatile bool keep = true;

void terminate(int sig) {
    keep = false;
}

void process_command(int cfd, command_t *command) {
    assertr(command->magic == COMMAND_MAGIC, "unrecognized magic")
    info("processing command " fmt64, command->method);

    cmdh cmd_handle = resolve_command(command);

    assertr(cmd_handle != NULL, "unresolved handler");
    cmd_handle(cfd, command);
}

int main(void) {
    int sfd;
    int opt;
    int retval;
    struct sockaddr_in addr;
    
    signal(SIGINT, terminate);
    
    retval = 0;
    info("%s", "Hello, World!");
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    
    assertev(sfd >= 0, "socket failed", 1);

    opt = 1;
    asserte(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) >= 0, "sockopt failed", retval = 1; goto l_sclean);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1337);
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);

    asserte(bind(sfd, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in)) >= 0, "bind failed", retval = 1; goto l_sclean);
    asserte(listen(sfd, 1) >= 0, "listen failed", retval = 1; goto l_sclean);

    while(keep) {
        char ip[INET_ADDRSTRLEN];
        command_t command;
        ssize_t n;
        
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(sfd, (struct sockaddr *)&client_addr, &client_len);

        asserte(client_fd >= 0, "socket accept error", continue);

        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        info("connected: %s:%u", ip, (unsigned)ntohs(client_addr.sin_port));

        n = recv(client_fd, &command, sizeof(command), 0);

        assertj(n >= 0, "recv failure", l_cclean);

        process_command(client_fd, &command);

l_cclean:
        close(client_fd);

        info("%s:%u left", ip, (unsigned)ntohs(client_addr.sin_port));   
    }

l_sclean:
    close(sfd);

    return retval;
}
