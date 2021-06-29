// = holyc
//
// == references
//
// - Terry Davis's mini compiler demo:
//   https://github.com/cia-foundation/TempleOS/blob/archive/Demo/Lectures/MiniCompiler.HC
//
// - Michael Lazear's assembler:
//   https://github.com/lazear/lass/blob/master/assembler.c#L53
//
// - Elf quine: https://medium.com/@MrJamesFisher/understanding-the-elf-4bd60daac571
//   https://medium.com/@MrJamesFisher/understanding-the-elf-4bd60daac571

// SysV x86_64 fn call registers (in order), rest on the stack
//
// - rdi
// - rsi
// - rdx
// - rcx
// - r8
// - r9

#include "src/boot.c"
#include "src/stdint.h"
#include "src/stdlib.c"
#include "src/unistd.c"
#include "src/string.c"
#include "src/stdio.c"

// https://my.eng.utah.edu/~cs4400/malloc.pdf
//
// malloc


int sys_brk(int n) {
  asm("mov rax, 12"); // brk
  asm("syscall");
  // asm("mov [current_break], rax");
  // asm("mov [initial_break], rax");
}

#define INPUT_SIZE 4096

#include "src/elf.h"

// #define ELF_START 0x8048000
#define ELF_START 0x401000
#define ELF_SIZE  120

int write_elf(int program_length) {
  uint64_t elf_offset = 0;
  uint8_t *elf_output;

  warn("Writing elf header...\n");

  elf_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);

  printf("elf_offset: %d\n", elf_offset);

  // Elf64_Ehdr *e = malloc(sizeof(Elf64_Ehdr));
}


//   e->e_ident[EI_MAG0]       = 0x7f; // magic
//   e->e_ident[EI_MAG1]       = 'E';
//   e->e_ident[EI_MAG2]       = 'L';
//   e->e_ident[EI_MAG3]       = 'F';
//   e->e_ident[EI_CLASS]      = ELFCLASS64;
//   e->e_ident[EI_DATA]       = ELFDATA2LSB;
//   e->e_ident[EI_VERSION]    = EV_CURRENT;
//   e->e_ident[EI_OSABI]      = ELFOSABI_SYSV;
//   e->e_ident[EI_ABIVERSION] = 0;
//   e->e_ident[EI_PAD]        = 0;
//   e->e_type                 = ET_EXEC;
//   e->e_machine              = EM_X86_64;
//   e->e_version              = EV_CURRENT;
//   e->e_entry                = ELF_START + ELF_SIZE;
//   e->e_phoff                = sizeof(Elf64_Ehdr);
//   e->e_shoff                = 0;
//   e->e_flags                = 0;
//   e->e_ehsize               = sizeof(Elf64_Ehdr);
//   e->e_phentsize            = sizeof(Elf64_Phdr);
//   e->e_phnum                = 1;
//   e->e_shentsize            = 0;
//   e->e_shnum                = 0;
//   e->e_shstrndx             = 0;

//   Elf64_Phdr *p = malloc(sizeof(Elf64_Phdr));
//   p->p_type   = 1;
//   p->p_flags  = 5;
//   p->p_offset = 0;
//   p->p_vaddr  = ELF_START;
//   p->p_paddr  = 0x8048000;
//   p->p_filesz = program_length + elf_offset;
//   p->p_memsz = p->p_filesz;
//   p->p_align = 0x1000;
//   info("p_filesz: %ld bytes\n", p->p_filesz);

//   elf_output = malloc(elf_offset);
//   memcpy(elf_output, e, sizeof(Elf64_Ehdr));
//   memcpy(elf_output + sizeof(Elf64_Ehdr), p, sizeof(Elf64_Phdr));

//   write(STDOUT_FILENO, elf_output, elf_offset);
// }


int main(int argc, char **argv, char **envp) {
  char input[INPUT_SIZE];
  int num_read;

  if ((num_read = read(STDIN_FILENO, input, INPUT_SIZE)) < 0)
    die("read");

  // temp: cat for now
  // write(STDOUT_FILENO, input, num_read);

  printf("read %d bytes\n", num_read);

  write_elf(num_read);
  // write(STDOUT_FILENO, &input, num_read - 1); // rm \n

  int init_brk = sys_brk(0);
  printf("brk(0)=%d\n", init_brk);

  char *memstart = (char *)init_brk;
  printf("&memstart=%d\n", memstart);

  for (int i = 0; i < 10; i++)
    printf("brk(%d)=%d\n", i, sys_brk(init_brk + i*8));

  memstart[0] = 'A';
  printf("memstart[0]=%c\n", memstart[0]);

  return EXIT_SUCCESS;
}
