#ifndef HOLYC_CC
#define HOLYC_CC

#include <stdbool.h>

// headers:
//   tokens
//   parser
//   codegen
//   obj
//   compiler
// src:
//   main
//     setup
//     tokens
//     parser
//     compiler

enum DebugFlags {
  DEBUG_LEX = (1 << 0),
  DEBUG_PARSE = (1 << 1),
  DEBUG_OBJ = (1 << 2),
};

typedef struct MainArgs {
  int    argc;
  char **argv;
  char **envp;
} MainArgs;

typedef int Flag;

typedef struct CompilerOpts {
  bool output_asm;
  Flag debug;
} CompilerOpts;

typedef struct Buffer {
  char *start;
  char *curr;
  int   size;
} Buffer;

typedef enum {
  // TK_ERROR,
  TK_EOF,
  TK_INT,
  TK_MIN,
  TK_PLUS,
  TK_DIV,
  TK_MUL,
  TK_LPAREN,
  TK_RPAREN,
  TK_SEMI,
} TokenType;

typedef struct Token {
  char     *start;
  int       size;
  int       value;
  TokenType type;

  // int line;
  // int col;
} Token;

// Global compiler state.
//
typedef struct CC {
  MainArgs      main_args;
  Buffer        input;
  Buffer        code;
  Token         curr_token;
  char         *token_pos;        // token start index
  int           token;            // token type
  int           int_val;          // if token is int
  CompilerOpts *opts;             // compiler options
  char         *token_table[][2]; // token names
} CC;

void debug(CC *cc);

void error(char *fmt, ...);

CC *cc_init(int argc, char **argv, char **envp, int input_size);

#endif // HOLYC_CC
