// MinAll JavaScript Runtime Test Suite
// Basic functionality tests

print("=== MinAll Test Suite ===");

// Test 1: Variable declarations and assignments
print("\nTest 1: Variables");
var x = 42;
var y = 3.14;
var name = "MinAll";
print("x =", x);
print("y =", y);
print("name =", name);

// Test 2: Arithmetic operations
print("\nTest 2: Arithmetic");
var a = 10;
var b = 5;
print("a + b =", a + b);
print("a - b =", a - b);
print("a * b =", a * b);
print("a / b =", a / b);
print("a % b =", a % b);

// Test 3: String operations
print("\nTest 3: Strings");
var greeting = "Hello";
var target = "World";
print("Concatenation:", greeting + " " + target);
print("Mixed:", "Result: " + 42);

// Test 4: Comparison operations
print("\nTest 4: Comparisons");
print("10 > 5 =", 10 > 5);
print("10 < 5 =", 10 < 5);
print("10 == 10 =", 10 == 10);
print("10 != 5 =", 10 != 5);

// Test 5: Functions
print("\nTest 5: Functions");
function add(x, y) {
    return x + y;
}

function multiply(x, y) {
    return x * y;
}

var sum = add(15, 25);
var product = multiply(6, 7);
print("add(15, 25) =", sum);
print("multiply(6, 7) =", product);

// Test 6: Conditional statements
print("\nTest 6: Conditionals");
var score = 85;
if (score >= 90) {
    print("Grade: A");
} else {
    if (score >= 80) {
        print("Grade: B");
    } else {
        print("Grade: C");
    }
}

// Test 7: Recursive function
print("\nTest 7: Recursion");
function factorial(n) {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

var fact5 = factorial(5);
print("factorial(5) =", fact5);

// Test 8: Complex expressions
print("\nTest 8: Complex expressions");
var result = (10 + 5) * 2 - 8 / 4;
print("(10 + 5) * 2 - 8 / 4 =", result);

// Test 9: Nested function calls
print("\nTest 9: Nested calls");
function square(x) {
    return x * x;
}

var nested_result = add(square(3), square(4));
print("add(square(3), square(4)) =", nested_result);

print("\n=== All tests completed ===");
