#include "minall.h"

double benchmark_execution(const char* source, int iterations) {
    clock_t total_start = clock();
    
    for (int i = 0; i < iterations; i++) {
        minall_reset();
        
        int token_count;
        Token* tokens = tokenize(source, &token_count);
        ASTNode* ast = parse(tokens, token_count);
        
        Context ctx;
        init_context(&ctx);
        interpret(ast, &ctx);
    }
    
    clock_t total_end = clock();
    return ((double)(total_end - total_start)) / CLOCKS_PER_SEC;
}

void run_performance_tests() {
    printf("MinAll Performance Benchmarks\n");
    printf("==============================\n\n");
    
    // Test 1: Simple arithmetic
    const char* test1 = "var x = 10; var y = 20; var z = x + y * 2;";
    printf("Test 1: Simple arithmetic\n");
    printf("Code: %s\n", test1);
    double time1 = benchmark_execution(test1, 10000);
    printf("10,000 iterations: %.6f seconds (%.2f ops/sec)\n\n", time1, 10000.0 / time1);
    
    // Test 2: Function calls
    const char* test2 = 
        "function add(a, b) { return a + b; }"
        "var result = add(5, 10);";
    printf("Test 2: Function calls\n");
    printf("Code: %s\n", test2);
    double time2 = benchmark_execution(test2, 5000);
    printf("5,000 iterations: %.6f seconds (%.2f ops/sec)\n\n", time2, 5000.0 / time2);
    
    // Test 3: Loops and conditionals
    const char* test3 = 
        "var sum = 0;"
        "for (var i = 0; i < 10; i = i + 1) {"
        "  if (i % 2 == 0) {"
        "    sum = sum + i;"
        "  }"
        "}";
    printf("Test 3: Loops and conditionals\n");
    printf("Code: %s\n", test3);
    double time3 = benchmark_execution(test3, 1000);
    printf("1,000 iterations: %.6f seconds (%.2f ops/sec)\n\n", time3, 1000.0 / time3);
    
    // Test 4: Recursive function
    const char* test4 = 
        "function factorial(n) {"
        "  if (n <= 1) return 1;"
        "  return n * factorial(n - 1);"
        "}"
        "var result = factorial(10);";
    printf("Test 4: Recursive function\n");
    printf("Code: %s\n", test4);
    double time4 = benchmark_execution(test4, 1000);
    printf("1,000 iterations: %.6f seconds (%.2f ops/sec)\n\n", time4, 1000.0 / time4);
    
    // Memory usage statistics
    printf("Memory Statistics\n");
    printf("-----------------\n");
    printf("Memory pool size: %d bytes\n", MEMORY_POOL_SIZE);
    printf("Current memory usage: %zu bytes\n", memory_offset);
    printf("Memory efficiency: %.2f%%\n", (double)memory_offset / MEMORY_POOL_SIZE * 100);
    
    // Performance summary
    printf("\nPerformance Summary\n");
    printf("-------------------\n");
    printf("Total benchmark time: %.6f seconds\n", time1 + time2 + time3 + time4);
    printf("Average operations per second: %.2f\n", 
           (10000 + 5000 + 1000 + 1000) / (time1 + time2 + time3 + time4));
}
