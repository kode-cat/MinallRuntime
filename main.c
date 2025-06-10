#include "minall.h"

static char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = (char*)malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';
    
    fclose(file);
    return content;
}

static void execute_file(const char* filename) {
    char* source = read_file(filename);
    if (!source) return;
    
    printf("Executing %s...\n", filename);
    
    clock_t start = clock();
    
    // Reset memory pool
    minall_reset();
    
    // Tokenize
    int token_count;
    Token* tokens = tokenize(source, &token_count);
    
    // Parse
    ASTNode* ast = parse(tokens, token_count);
    
    // Interpret
    Context ctx;
    init_context(&ctx);
    interpret(ast, &ctx);
    
    clock_t end = clock();
    double execution_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("Execution completed in %.6f seconds\n", execution_time);
    printf("Memory used: %zu bytes\n", memory_offset);
    
    free(source);
}

int main(int argc, char* argv[]) {
    printf("MinAll JavaScript Runtime v1.0\n");
    printf("High-speed minimal JavaScript runtime in C\n\n");
    
    if (argc < 2) {
        printf("Usage: %s <script.js> [options]\n", argv[0]);
        printf("Options:\n");
        printf("  --benchmark  Run performance benchmarks\n");
        printf("  --ast        Print AST for debugging\n");
        return 1;
    }
    
    bool run_benchmark = false;
    bool show_ast = false;
    
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--benchmark") == 0) {
            run_benchmark = true;
        } else if (strcmp(argv[i], "--ast") == 0) {
            show_ast = true;
        }
    }
    
    if (run_benchmark) {
        run_performance_tests();
        return 0;
    }
    
    if (show_ast) {
        char* source = read_file(argv[1]);
        if (source) {
            minall_reset();
            int token_count;
            Token* tokens = tokenize(source, &token_count);
            ASTNode* ast = parse(tokens, token_count);
            printf("AST for %s:\n", argv[1]);
            print_ast(ast, 0);
            free(source);
        }
        return 0;
    }
    
    execute_file(argv[1]);
    
    return 0;
}
