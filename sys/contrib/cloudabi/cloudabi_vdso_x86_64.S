// Copyright (c) 2016 Nuxi (https://nuxi.nl/) and contributors.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//
// This file is automatically generated. Do not edit.
//
// Source: https://github.com/NuxiNL/cloudabi

#define ENTRY(name)      \
  .text;                 \
  .p2align 4, 0x90;      \
  .global name;          \
  .type name, @function; \
name:

#define END(name) .size name, . - name

ENTRY(cloudabi_sys_clock_res_get)
  push %rsi
  mov $0, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_clock_res_get)

ENTRY(cloudabi_sys_clock_time_get)
  push %rdx
  mov $1, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_clock_time_get)

ENTRY(cloudabi_sys_condvar_signal)
  mov $2, %eax
  syscall
  ret
END(cloudabi_sys_condvar_signal)

ENTRY(cloudabi_sys_fd_close)
  mov $3, %eax
  syscall
  ret
END(cloudabi_sys_fd_close)

ENTRY(cloudabi_sys_fd_create1)
  push %rsi
  mov $4, %eax
  syscall
  pop %rcx
  jc 1f
  mov %eax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_fd_create1)

ENTRY(cloudabi_sys_fd_create2)
  push %rsi
  push %rdx
  mov $5, %eax
  syscall
  pop %rsi
  pop %rcx
  jc 1f
  mov %eax, (%rcx)
  mov %edx, (%rsi)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_fd_create2)

ENTRY(cloudabi_sys_fd_datasync)
  mov $6, %eax
  syscall
  ret
END(cloudabi_sys_fd_datasync)

ENTRY(cloudabi_sys_fd_dup)
  push %rsi
  mov $7, %eax
  syscall
  pop %rcx
  jc 1f
  mov %eax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_fd_dup)

ENTRY(cloudabi_sys_fd_pread)
  mov %rcx, %r10
  push %r8
  mov $8, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_fd_pread)

ENTRY(cloudabi_sys_fd_pwrite)
  mov %rcx, %r10
  push %r8
  mov $9, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_fd_pwrite)

ENTRY(cloudabi_sys_fd_read)
  push %rcx
  mov $10, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_fd_read)

ENTRY(cloudabi_sys_fd_replace)
  mov $11, %eax
  syscall
  ret
END(cloudabi_sys_fd_replace)

ENTRY(cloudabi_sys_fd_seek)
  push %rcx
  mov $12, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_fd_seek)

ENTRY(cloudabi_sys_fd_stat_get)
  mov $13, %eax
  syscall
  ret
END(cloudabi_sys_fd_stat_get)

ENTRY(cloudabi_sys_fd_stat_put)
  mov $14, %eax
  syscall
  ret
END(cloudabi_sys_fd_stat_put)

ENTRY(cloudabi_sys_fd_sync)
  mov $15, %eax
  syscall
  ret
END(cloudabi_sys_fd_sync)

ENTRY(cloudabi_sys_fd_write)
  push %rcx
  mov $16, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_fd_write)

ENTRY(cloudabi_sys_file_advise)
  mov %rcx, %r10
  mov $17, %eax
  syscall
  ret
END(cloudabi_sys_file_advise)

ENTRY(cloudabi_sys_file_allocate)
  mov $18, %eax
  syscall
  ret
END(cloudabi_sys_file_allocate)

ENTRY(cloudabi_sys_file_create)
  mov %rcx, %r10
  mov $19, %eax
  syscall
  ret
END(cloudabi_sys_file_create)

ENTRY(cloudabi_sys_file_link)
  mov %rcx, %r10
  mov $20, %eax
  syscall
  ret
END(cloudabi_sys_file_link)

ENTRY(cloudabi_sys_file_open)
  mov %rcx, %r10
  push %r9
  mov $21, %eax
  syscall
  pop %rcx
  jc 1f
  mov %eax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_file_open)

ENTRY(cloudabi_sys_file_readdir)
  mov %rcx, %r10
  push %r8
  mov $22, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_file_readdir)

ENTRY(cloudabi_sys_file_readlink)
  mov %rcx, %r10
  push %r9
  mov $23, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_file_readlink)

ENTRY(cloudabi_sys_file_rename)
  mov %rcx, %r10
  mov $24, %eax
  syscall
  ret
END(cloudabi_sys_file_rename)

ENTRY(cloudabi_sys_file_stat_fget)
  mov $25, %eax
  syscall
  ret
END(cloudabi_sys_file_stat_fget)

ENTRY(cloudabi_sys_file_stat_fput)
  mov $26, %eax
  syscall
  ret
END(cloudabi_sys_file_stat_fput)

ENTRY(cloudabi_sys_file_stat_get)
  mov %rcx, %r10
  mov $27, %eax
  syscall
  ret
END(cloudabi_sys_file_stat_get)

ENTRY(cloudabi_sys_file_stat_put)
  mov %rcx, %r10
  mov $28, %eax
  syscall
  ret
END(cloudabi_sys_file_stat_put)

ENTRY(cloudabi_sys_file_symlink)
  mov %rcx, %r10
  mov $29, %eax
  syscall
  ret
END(cloudabi_sys_file_symlink)

ENTRY(cloudabi_sys_file_unlink)
  mov %rcx, %r10
  mov $30, %eax
  syscall
  ret
END(cloudabi_sys_file_unlink)

ENTRY(cloudabi_sys_lock_unlock)
  mov $31, %eax
  syscall
  ret
END(cloudabi_sys_lock_unlock)

ENTRY(cloudabi_sys_mem_advise)
  mov $32, %eax
  syscall
  ret
END(cloudabi_sys_mem_advise)

ENTRY(cloudabi_sys_mem_lock)
  mov $33, %eax
  syscall
  ret
END(cloudabi_sys_mem_lock)

ENTRY(cloudabi_sys_mem_map)
  mov %rcx, %r10
  mov $34, %eax
  syscall
  jc 1f
  mov 8(%rsp), %rcx
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_mem_map)

ENTRY(cloudabi_sys_mem_protect)
  mov $35, %eax
  syscall
  ret
END(cloudabi_sys_mem_protect)

ENTRY(cloudabi_sys_mem_sync)
  mov $36, %eax
  syscall
  ret
END(cloudabi_sys_mem_sync)

ENTRY(cloudabi_sys_mem_unlock)
  mov $37, %eax
  syscall
  ret
END(cloudabi_sys_mem_unlock)

ENTRY(cloudabi_sys_mem_unmap)
  mov $38, %eax
  syscall
  ret
END(cloudabi_sys_mem_unmap)

ENTRY(cloudabi_sys_poll)
  push %rcx
  mov $39, %eax
  syscall
  pop %rcx
  jc 1f
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_poll)

ENTRY(cloudabi_sys_poll_fd)
  mov %rcx, %r10
  mov $40, %eax
  syscall
  jc 1f
  mov 8(%rsp), %rcx
  mov %rax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_poll_fd)

ENTRY(cloudabi_sys_proc_exec)
  mov %rcx, %r10
  mov $41, %eax
  syscall
  ret
END(cloudabi_sys_proc_exec)

ENTRY(cloudabi_sys_proc_exit)
  mov $42, %eax
  syscall
END(cloudabi_sys_proc_exit)

ENTRY(cloudabi_sys_proc_fork)
  push %rdi
  push %rsi
  mov $43, %eax
  syscall
  pop %rsi
  pop %rcx
  jc 1f
  mov %eax, (%rcx)
  mov %edx, (%rsi)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_proc_fork)

ENTRY(cloudabi_sys_proc_raise)
  mov $44, %eax
  syscall
  ret
END(cloudabi_sys_proc_raise)

ENTRY(cloudabi_sys_random_get)
  mov $45, %eax
  syscall
  ret
END(cloudabi_sys_random_get)

ENTRY(cloudabi_sys_sock_accept)
  push %rdx
  mov $46, %eax
  syscall
  pop %rcx
  jc 1f
  mov %eax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_sock_accept)

ENTRY(cloudabi_sys_sock_bind)
  mov %rcx, %r10
  mov $47, %eax
  syscall
  ret
END(cloudabi_sys_sock_bind)

ENTRY(cloudabi_sys_sock_connect)
  mov %rcx, %r10
  mov $48, %eax
  syscall
  ret
END(cloudabi_sys_sock_connect)

ENTRY(cloudabi_sys_sock_listen)
  mov $49, %eax
  syscall
  ret
END(cloudabi_sys_sock_listen)

ENTRY(cloudabi_sys_sock_recv)
  mov $50, %eax
  syscall
  ret
END(cloudabi_sys_sock_recv)

ENTRY(cloudabi_sys_sock_send)
  mov $51, %eax
  syscall
  ret
END(cloudabi_sys_sock_send)

ENTRY(cloudabi_sys_sock_shutdown)
  mov $52, %eax
  syscall
  ret
END(cloudabi_sys_sock_shutdown)

ENTRY(cloudabi_sys_sock_stat_get)
  mov $53, %eax
  syscall
  ret
END(cloudabi_sys_sock_stat_get)

ENTRY(cloudabi_sys_thread_create)
  push %rsi
  mov $54, %eax
  syscall
  pop %rcx
  jc 1f
  mov %eax, (%rcx)
  xor %eax, %eax
1:
  ret
END(cloudabi_sys_thread_create)

ENTRY(cloudabi_sys_thread_exit)
  mov $55, %eax
  syscall
END(cloudabi_sys_thread_exit)

ENTRY(cloudabi_sys_thread_yield)
  mov $56, %eax
  syscall
  ret
END(cloudabi_sys_thread_yield)
