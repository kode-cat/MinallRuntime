// MinAll JavaScript Runtime Benchmark Suite
// Performance testing scripts

print("=== MinAll Benchmark Suite ===");

// Fibonacci benchmark
function fibonacci(n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

print("\nFibonacci(20):", fibonacci(20));

// Prime number calculation
function isPrime(n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0) return 0;
    
    var i = 3;
    while (i * i <= n) {
        if (n % i == 0) return 0;
        i = i + 2;
    }
    return 1;
}

function countPrimes(limit) {
    var count = 0;
    var i = 2;
    while (i < limit) {
        if (isPrime(i)) {
            count = count + 1;
        }
        i = i + 1;
    }
    return count;
}

print("Primes up to 100:", countPrimes(100));

// Array simulation with loops
function arraySum(size) {
    var sum = 0;
    var i = 0;
    while (i < size) {
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}

print("Sum 1 to 1000:", arraySum(1000));

// Mathematical operations benchmark
function mathBench() {
    var result = 0;
    var i = 0;
    while (i < 1000) {
        result = result + i * i - i / 2 + i % 7;
        i = i + 1;
    }
    return result;
}

print("Math benchmark result:", mathBench());

print("\n=== Benchmark completed ===");
