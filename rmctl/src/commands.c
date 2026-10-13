#define _GNU_SOURCE
#include "commands.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <sys/sendfile.h>

#include "base.h"


const cmdh handles[] = {
    NULL,
    &do_getfile,
    &do_putfile,
    &do_putbinfile,
    &do_exec,
};

cmdh resolve_command(command_t *command) {
    size_t handlers = sizeof(handles) / sizeof(cmdh); 

    if (handlers > command->method) {
        cmdh handle = handles[command->method];
        return handle;
    }

    return NULL;
}

void do_getfile(int cfd, command_t *command) {
    ssize_t ret;
    int file;
    struct stat st;

    char *filename = malloc(command->size + 1);

    assertr(filename != NULL, "couldnt allocate filename");

    read(cfd, filename, command->size);
    filename[command->size] = 0;

    info("sending %s", filename);

    file = open(filename, O_RDONLY);
    fstat(file, &st);

    ret = sendfile(cfd, file, NULL, st.st_size);

    assertj(ret >= 0, "sendfile error", l_cleanup);

l_cleanup:
    close(file);
    free(filename);
}


void do_putfile(int cfd, command_t *command) {
    int file;
    int pipefd[2];
    ssize_t ret;
    pfargs_t args;

    ret = read(cfd, &args, sizeof(args));
    assertr(ret >= 0, "read of args failed");

    char *filename = malloc(args.fnsize + 1);

    assertr(filename != NULL, "couldnt allocate filename");

    assertj(pipe(pipefd) >= 0, "pipe creation failed", l_nclean);

    ret = read(cfd, filename, args.fnsize);
    assertj(ret >= 0, "filename read failure", l_nclean);

    filename[args.fnsize] = 0;

    info("receiving %s", filename);

    file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assertj(file >= 0, "couldnt open file", l_pclean);

    ret = splice(cfd, NULL, pipefd[1], NULL, (size_t)args.totsize, 0);
    assertp(ret >= 0, "splice failure", l_fclean);
    
    ret = splice(pipefd[0], NULL, file, NULL, (size_t)args.totsize, 0);
    assertp(ret >= 0, "splice failure", l_fclean);

l_fclean:
    close(file);
l_pclean:
    close(pipefd[0]);
    close(pipefd[1]);
l_nclean:
    free(filename);
}

void do_putbinfile(int cfd, command_t *command) {
    int file;
    int pipefd[2];
    ssize_t ret;
    pfargs_t args;

    read(cfd, &args, sizeof(args));

    char *filename = malloc(args.fnsize + 1);

    assertr(filename != NULL, "couldnt allocate filename");

    assertj(pipe(pipefd) >= 0, "pipe creation failed", l_nclean);

    ret = read(cfd, filename, args.fnsize);
    assertj(ret >= 0, "filename read failure", l_nclean);

    filename[args.fnsize] = 0;

    info("receiving %s", filename);

    file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    assertj(file >= 0, "couldnt open file", l_pclean);

    ret = splice(cfd, NULL, pipefd[1], NULL, (size_t)args.totsize, 0);
    assertp(ret >= 0, "splice failure", l_fclean);
    
    ret = splice(pipefd[0], NULL, file, NULL, (size_t)args.totsize, 0);
    assertp(ret >= 0, "splice failure", l_fclean);

    ret = chmod(filename, S_IRWXU);
    assertp(ret >= 0, "chmod", l_fclean);

    info("%s: %s", "added permissions for", filename);
    
l_fclean:
    close(file);
l_pclean:
    close(pipefd[0]);
    close(pipefd[1]);
l_nclean:
    free(filename);
}



void do_exec(int cfd, command_t *command) {
    pid_t pid;

    char *shcmd = malloc(command->size + 1);

    assertr(shcmd != NULL, "couldnt allocate filename");

    read(cfd, shcmd, command->size);
    shcmd[command->size] = 0;
    info("executing '%s'", shcmd);

    pid = fork();
    assertp(pid >= 0, "fork failed", l_cleanup);

    if (pid == 0) {
        char *argv[] = {"/bin/sh", "-c", shcmd, NULL};

        for (char **i = argv; *i; i++) {
            info("%s", *i);
        }
 
        dup2(cfd, 0);
        dup2(cfd, 1);
        dup2(cfd, 2);

        execv("/bin/sh", argv);
    } 
    
l_cleanup:
    free(shcmd);
}