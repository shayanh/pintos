#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "userprog/process.h"

typedef int pid_t;
static int (*syscall_handlers[20]) (struct intr_frame *); /* Array of syscall functions */

/* Reads a byte at user virtual address UADDR.
   UADDR must be below PHYS_BASE.
   Returns the byte value if successful, -1 if a segfault
   occurred. */
static int
get_user (const uint8_t *uaddr)
{
  if (!is_user_vaddr(uaddr))
    return -1;
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}

/* Writes BYTE to user address UDST.
   UDST must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static bool
put_user (uint8_t *udst, uint8_t byte)
{
  if (!is_user_vaddr(udst))
    return false;
  int error_code;
  asm ("movl $1f, %0; movb %b2, %1; 1:"
       : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code != -1;
}


static bool is_valid_pointer(void * esp, uint8_t argc){
  uint8_t i = 0;
  for (; i < argc; ++i)
  {
    if (get_user(((uint8_t *)esp)+i) == -1){
      return false;
    }
  }
  return true;
}

static void
syscall_exit (int status)
{
  thread_exit ();
}

static void
syscall_halt(void)
{
  shutdown();
}

static void
syscall_sch_io(int device_id) {
  printf("sch_io syscall");
  printf(" device_id = %d\n", device_id);
  // TODO
}

static void
syscall_do_io(int device_id, int ticks) {
  printf("do_io syscall");
  printf(" device_id = %d, ticks = %d\n", device_id, ticks);
  // TODO
}

static int
syscall_exit_wrapper(struct intr_frame *f)
{
  int status;
  if (is_valid_pointer(f->esp + 4, 4))
    status = *((int*)f->esp+1);
  else
    return -1;
  syscall_exit(status);
  return 0;
}

static int
syscall_halt_wrapper(struct intr_frame *f UNUSED)
{
  syscall_halt();
  return 0;
}

static int
syscall_sch_io_wrapper(struct intr_frame *f) {
  int device_id;
  if (is_valid_pointer(f->esp + 4, 4))
    device_id = *(int*)(f->esp + 4);
  else
    return -1;
  syscall_sch_io(device_id);
  return 0;
}

static int
syscall_do_io_wrapper(struct intr_frame *f) {
  int device_id, ticks;
  if (is_valid_pointer(f->esp + 4, 8)) {
    device_id = *(int*)(f->esp + 4);
    ticks = *(int*)(f->esp + 8);
  }
  else
    return -1;
  syscall_do_io(device_id, ticks);
  return 0;
}

static void
kill_program(void)
{
  thread_exit();
}

static void
syscall_handler(struct intr_frame *f)
{
  printf("syscall handler!\n");

  if (!is_valid_pointer(f->esp, 4)){
    kill_program();
    return;
  }

  int syscall_num = * (int *)f->esp;

  if (syscall_num < 0 || syscall_num >= 22) {
    kill_program();
    return;
  }

  printf("running system call\n");

  if (syscall_handlers[syscall_num](f) == -1){
    kill_program();
    return;
  }
}

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
  syscall_handlers[SYS_EXIT] = &syscall_exit_wrapper;
  syscall_handlers[SYS_HALT] = &syscall_halt_wrapper;
  syscall_handlers[SYS_SCH_IO] = &syscall_sch_io_wrapper;
  syscall_handlers[SYS_DO_IO] = &syscall_do_io_wrapper;
}
