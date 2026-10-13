#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>
#include <stddef.h>

#define COMMAND_MAGIC 0x48A9B2F1E3C47058

typedef struct command {
    uint64_t magic;
    uint64_t method;
    uint64_t size;
} command_t;

typedef struct pfargs {
    uint64_t fnsize;
    uint64_t totsize;
} pfargs_t;

typedef void (*cmdh)(int, command_t *);

void do_getfile(int cfd, command_t *command);
void do_putfile(int cfd, command_t *command);
void do_putbinfile(int cfd, command_t *command);
void do_exec(int cfd, command_t *command);

cmdh resolve_command(command_t *command);

#endif
