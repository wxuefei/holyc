// = holyc
//
// SysV x86_64 fn call registers (in order), rest on the stack
//
// - rdi
// - rsi
// - rdx
// - rcx
// - r8
// - r9
//
// Linux syscalls use:
//
//  rdi
//  rsi
//  rdx
//  r10
//  r9
//
//  clobbers rcx, r11, and rax


#include "lib/boot.c"
#include "lib/stddef.h"
#include "lib/stdint.h"
#include "lib/mman.c"
#include "lib/stdlib.c"
#include "lib/unistd.c"
#include "lib/string.c"
#include "lib/stdio.c"
#include "lib/elf.h"
#include "lib/stdbool.h"

#define ELF_START 0x401000
#define ELF_SIZE  120

int write_elf_header(int program_length) {
  uint64_t elf_offset = 0;
  uint8_t *elf_output;

  warn("Writing elf header...\n");

  elf_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);

  Elf64_Ehdr *e = malloc(sizeof(Elf64_Ehdr));
  e->e_ident[EI_MAG0]       = ELFMAG0;
  e->e_ident[EI_MAG1]       = ELFMAG1;
  e->e_ident[EI_MAG2]       = ELFMAG2;
  e->e_ident[EI_MAG3]       = ELFMAG3;
  e->e_ident[EI_CLASS]      = ELFCLASS64;
  e->e_ident[EI_DATA]       = ELFDATA2LSB;
  e->e_ident[EI_VERSION]    = EV_CURRENT;
  e->e_ident[EI_OSABI]      = ELFOSABI_SYSV;
  e->e_ident[EI_ABIVERSION] = 0;
  e->e_ident[EI_PAD]        = 0;
  e->e_type                 = ET_EXEC;
  e->e_machine              = EM_X86_64;
  e->e_version              = EV_CURRENT;
  e->e_entry                = ELF_START + ELF_SIZE;
  e->e_phoff                = sizeof(Elf64_Ehdr);
  e->e_shoff                = 0;
  e->e_flags                = 0;
  e->e_ehsize               = sizeof(Elf64_Ehdr);
  e->e_phentsize            = sizeof(Elf64_Phdr);
  e->e_phnum                = 1;
  e->e_shentsize            = 0;
  e->e_shnum                = 0;
  e->e_shstrndx             = 0;

  Elf64_Phdr *p = malloc(sizeof(Elf64_Phdr));
  p->p_type   = 1;
  p->p_flags  = 5;
  p->p_offset = 0;
  p->p_vaddr  = ELF_START;
  p->p_paddr  = ELF_START;
  p->p_filesz = program_length + elf_offset;
  p->p_memsz = p->p_filesz;
  p->p_align = 0x1000; // ?

  elf_output = malloc(elf_offset);
  memcpy(elf_output, e, sizeof(Elf64_Ehdr));
  memcpy(elf_output + sizeof(Elf64_Ehdr), p, sizeof(Elf64_Phdr));

  write(STDOUT_FILENO, elf_output, elf_offset);
}

typedef struct CC {
  bool output_asm;
} CC;

// Set defaults, update cc options from argv values.
//
void parse_options(CC *cc, int argc, char **argv) {
  cc->output_asm = false;

  for (int i = 0; i < argc; i++) {
    char *arg = argv[i];

    if (*arg == '-')
      arg++;
    else
      continue;

    if (*arg == 'S')
      cc->output_asm = true;
  }
}

void emit_mov_rax_imm(CC *cc, char **_code, int imm) {
  if (cc->output_asm) {
    printf("MOV RAX,%d\n", imm);
    return;
  }
  char *code = *_code;
  *code++ = 0x48; // REX
  *code++ = 0xB8; // MOV RAX,imm
  *code = imm;
  code += 8;
  *_code = code;
}

void emit_start(CC *cc, char **_code, char **input) {
  if (cc->output_asm) {
    printf("\n_start:\n");
    printf("  // CALL main\n");
    printf("  MOV RDI,RAX  // arg1, main()\n");
    printf("  MOV RAX,60   // exit\n");
    printf("  SYSCALL      // exit()\n");
    return;
  }

  char *code = *_code;
  *code++ = 0x48; // REX
  *code++ = 0x89; // MOV RDI,reg
  *code++ = 0xC7; //   RAX

  emit_mov_rax_imm(cc, &code, 60);

  *code++ = 0x0F; // SYSCALL
  *code++ = 0x05;

  *_code = code;
}

#define INPUT_SIZE 4096

int process(CC *cc, char *input, int size) {
  char code[INPUT_SIZE];
  char *c = code;
  char *p = input;

  if (!cc->output_asm)
    write_elf_header(size);

  int n = strtol(p, &p, 10);
  emit_mov_rax_imm(cc, &c, n);

  // while (*p) {
  //   if (*p == '-') {
  //   }
  // }
  emit_start(cc, &c, &p);

  warnf("Writing %d bytes of machine code\n", c - code);
  write(STDOUT_FILENO, code, c - code);

  return EXIT_SUCCESS;
}

int main(int argc, char **argv, char **envp) {
  char input[INPUT_SIZE];
  int num_read;

  CC *cc = malloc(sizeof(CC));
  parse_options(cc, argc, argv);

  if ((num_read = read(STDIN_FILENO, input, INPUT_SIZE)) < 0)
    die("read");

  return process(cc, input, num_read);
}


