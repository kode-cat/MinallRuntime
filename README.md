# MinAll JavaScript Runtime

A minimal, high-speed JavaScript runtime implemented in C, designed for maximum performance with minimal overhead.

## Features

- **Ultra-fast execution**: Optimized C implementation with minimal abstraction layers
- **Minimal memory footprint**: Custom memory pool allocator for zero-fragmentation
- **Core JavaScript support**: Variables, functions, control flow, and basic operations
- **Built-in benchmarking**: Performance testing framework included
- **Simple architecture**: Clean, maintainable codebase focused on speed

## Supported JavaScript Features

### Core Language
- Variable declarations (`var`)
- Function declarations and calls
- Arithmetic operations (`+`, `-`, `*`, `/`, `%`)
- Comparison operations (`<`, `>`, `<=`, `>=`, `==`, `!=`)
- Logical operations (`&&`, `||`, `!`)
- String concatenation
- Control flow (`if`/`else`)
- Return statements

### Built-in Functions
- `print()` - Output values to console

## Building

### Requirements
- GCC or Clang compiler
- Make (optional, but recommended)

### Quick Build
```bash
make
