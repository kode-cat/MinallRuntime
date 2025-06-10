#ifndef MINALL_H
#define MINALL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Memory pool configuration - optimized for speed
#define MEMORY_POOL_SIZE (2 * 1024 * 1024) // 2MB for better performance
#define MAX_TOKENS 50000
#define MAX_VARIABLES 1000
#define MAX_FUNCTIONS 100
#define MAX_CALL_STACK 100

// Performance optimizations
#define INLINE __attribute__((always_inline)) inline
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

// Token types
typedef enum {
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_VAR,
    TOKEN_FUNCTION,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    double number;
    int line;
    int column;
} Token;

// AST Node types
typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECLARATION,
    NODE_FUNCTION_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_CALL,
    NODE_IF,
    NODE_FOR,
    NODE_WHILE,
    NODE_RETURN,
    NODE_BLOCK,
    NODE_IDENTIFIER,
    NODE_NUMBER,
    NODE_STRING
} NodeType;

// Fast execution opcodes for hot loops
typedef enum {
    OP_LOAD_NUMBER,
    OP_LOAD_VAR,
    OP_STORE_VAR,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_CMP_LT,
    OP_CMP_GT,
    OP_CMP_EQ,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_CALL,
    OP_RETURN,
    OP_HALT
} OpCode;

typedef struct {
    OpCode op;
    union {
        double number;
        int var_index;
        int jump_offset;
    } operand;
} Instruction;

typedef struct ASTNode {
    NodeType type;
    union {
        double number;
        char* string;
        char* identifier;
        struct {
            char* name;
            struct ASTNode* value;
        } var_decl;
        struct {
            char* name;
            char** params;
            int param_count;
            struct ASTNode* body;
        } func_decl;
        struct {
            char* operator;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary_op;
        struct {
            char* operator;
            struct ASTNode* operand;
        } unary_op;
        struct {
            struct ASTNode* function;
            struct ASTNode** args;
            int arg_count;
        } call;
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_stmt;
        struct {
            struct ASTNode* init;
            struct ASTNode* condition;
            struct ASTNode* update;
            struct ASTNode* body;
        } for_stmt;
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_stmt;
        struct {
            struct ASTNode* value;
        } return_stmt;
        struct {
            struct ASTNode** statements;
            int count;
        } block;
    } data;
} ASTNode;

// Value types
typedef enum {
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_FUNCTION,
    VALUE_UNDEFINED
} ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        char* string;
        struct {
            char** params;
            int param_count;
            ASTNode* body;
        } function;
    } data;
} Value;

// Variable storage
typedef struct {
    char* name;
    Value value;
} Variable;

// Function storage
typedef struct {
    char* name;
    char** params;
    int param_count;
    ASTNode* body;
} Function;

// Execution context
typedef struct {
    Variable variables[MAX_VARIABLES];
    int var_count;
    Function functions[MAX_FUNCTIONS];
    int func_count;
    Value return_value;
    bool has_return;
} Context;

// Memory management
extern char memory_pool[MEMORY_POOL_SIZE];
extern size_t memory_offset;

// Lexer functions
Token* tokenize(const char* source, int* token_count);
void print_tokens(Token* tokens, int count);

// Parser functions
ASTNode* parse(Token* tokens, int token_count);
void print_ast(ASTNode* node, int depth);

// Interpreter functions
Value interpret(ASTNode* node, Context* ctx);
void init_context(Context* ctx);
void set_variable(Context* ctx, const char* name, Value value);
Value get_variable(Context* ctx, const char* name);

// Memory management functions
void* minall_malloc(size_t size);
void minall_reset();

// Benchmarking functions
double benchmark_execution(const char* source, int iterations);
void run_performance_tests();

// Fast loop execution functions
void init_fast_vm();
bool is_simple_for_loop(ASTNode* node);
double execute_fast_loop(ASTNode* for_node, Context* ctx);

// Utility functions
Value create_number(double num);
Value create_string(const char* str);
Value create_undefined();
void print_value(Value value);

#endif
