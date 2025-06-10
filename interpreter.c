#include "minall.h"

static Value execute_block(ASTNode* block, Context* ctx);
static Value evaluate_expression(ASTNode* expr, Context* ctx);

Value create_number(double num) {
    Value value;
    value.type = VALUE_NUMBER;
    value.data.number = num;
    return value;
}

Value create_string(const char* str) {
    Value value;
    value.type = VALUE_STRING;
    value.data.string = (char*)minall_malloc(strlen(str) + 1);
    strcpy(value.data.string, str);
    return value;
}

Value create_undefined() {
    Value value;
    value.type = VALUE_UNDEFINED;
    return value;
}

void print_value(Value value) {
    switch (value.type) {
        case VALUE_NUMBER:
            printf("%.2f", value.data.number);
            break;
        case VALUE_STRING:
            printf("%s", value.data.string);
            break;
        case VALUE_FUNCTION:
            printf("[Function]");
            break;
        case VALUE_UNDEFINED:
            printf("undefined");
            break;
    }
}

void init_context(Context* ctx) {
    ctx->var_count = 0;
    ctx->func_count = 0;
    ctx->has_return = false;
    ctx->return_value = create_undefined();
}

void set_variable(Context* ctx, const char* name, Value value) {
    // Check if variable already exists
    for (int i = 0; i < ctx->var_count; i++) {
        if (strcmp(ctx->variables[i].name, name) == 0) {
            ctx->variables[i].value = value;
            return;
        }
    }
    
    // Add new variable
    if (ctx->var_count < MAX_VARIABLES) {
        ctx->variables[ctx->var_count].name = (char*)minall_malloc(strlen(name) + 1);
        strcpy(ctx->variables[ctx->var_count].name, name);
        ctx->variables[ctx->var_count].value = value;
        ctx->var_count++;
    }
}

Value get_variable(Context* ctx, const char* name) {
    for (int i = 0; i < ctx->var_count; i++) {
        if (strcmp(ctx->variables[i].name, name) == 0) {
            return ctx->variables[i].value;
        }
    }
    return create_undefined();
}

static void register_function(Context* ctx, const char* name, char** params, int param_count, ASTNode* body) {
    if (ctx->func_count < MAX_FUNCTIONS) {
        ctx->functions[ctx->func_count].name = (char*)minall_malloc(strlen(name) + 1);
        strcpy(ctx->functions[ctx->func_count].name, name);
        ctx->functions[ctx->func_count].params = params;
        ctx->functions[ctx->func_count].param_count = param_count;
        ctx->functions[ctx->func_count].body = body;
        ctx->func_count++;
    }
}

static Function* get_function(Context* ctx, const char* name) {
    for (int i = 0; i < ctx->func_count; i++) {
        if (strcmp(ctx->functions[i].name, name) == 0) {
            return &ctx->functions[i];
        }
    }
    return NULL;
}

static Value call_function(Function* func, ASTNode** args, int arg_count, Context* parent_ctx) {
    // Create new context for function execution
    Context func_ctx;
    init_context(&func_ctx);
    
    // Copy functions from parent context
    for (int i = 0; i < parent_ctx->func_count; i++) {
        func_ctx.functions[i] = parent_ctx->functions[i];
    }
    func_ctx.func_count = parent_ctx->func_count;
    
    // Bind parameters
    for (int i = 0; i < func->param_count && i < arg_count; i++) {
        Value arg_value = evaluate_expression(args[i], parent_ctx);
        set_variable(&func_ctx, func->params[i], arg_value);
    }
    
    // Execute function body
    execute_block(func->body, &func_ctx);
    
    if (func_ctx.has_return) {
        return func_ctx.return_value;
    }
    
    return create_undefined();
}

static INLINE Value evaluate_binary_op(const char* operator, Value left, Value right) {
    // Fast path for number operations - most common case
    if (LIKELY(left.type == VALUE_NUMBER && right.type == VALUE_NUMBER)) {
        double l = left.data.number;
        double r = right.data.number;
        
        // Use switch on first character for faster dispatch
        switch (operator[0]) {
            case '+': return create_number(l + r);
            case '-': return create_number(l - r);
            case '*': return create_number(l * r);
            case '/': return create_number(LIKELY(r != 0) ? l / r : 0);
            case '%': return create_number(LIKELY(r != 0) ? (double)((int)l % (int)r) : 0);
            case '<': 
                if (operator[1] == '=') return create_number(l <= r ? 1 : 0);
                return create_number(l < r ? 1 : 0);
            case '>':
                if (operator[1] == '=') return create_number(l >= r ? 1 : 0);
                return create_number(l > r ? 1 : 0);
            case '=': return create_number(l == r ? 1 : 0);
            case '!': return create_number(l != r ? 1 : 0);
        }
    }
    
    // String concatenation
    if (strcmp(operator, "+") == 0 && 
        (left.type == VALUE_STRING || right.type == VALUE_STRING)) {
        char* result = (char*)minall_malloc(256);
        
        if (left.type == VALUE_STRING && right.type == VALUE_STRING) {
            snprintf(result, 256, "%s%s", left.data.string, right.data.string);
        } else if (left.type == VALUE_STRING && right.type == VALUE_NUMBER) {
            snprintf(result, 256, "%s%.2f", left.data.string, right.data.number);
        } else if (left.type == VALUE_NUMBER && right.type == VALUE_STRING) {
            snprintf(result, 256, "%.2f%s", left.data.number, right.data.string);
        }
        
        return create_string(result);
    }
    
    return create_undefined();
}

static Value evaluate_unary_op(const char* operator, Value operand) {
    if (strcmp(operator, "-") == 0 && operand.type == VALUE_NUMBER) {
        return create_number(-operand.data.number);
    }
    
    if (strcmp(operator, "!") == 0) {
        bool truthy = false;
        if (operand.type == VALUE_NUMBER) {
            truthy = operand.data.number != 0;
        } else if (operand.type == VALUE_STRING) {
            truthy = strlen(operand.data.string) > 0;
        }
        return create_number(truthy ? 0 : 1);
    }
    
    return create_undefined();
}

static Value evaluate_expression(ASTNode* expr, Context* ctx) {
    if (!expr) return create_undefined();
    
    switch (expr->type) {
        case NODE_NUMBER:
            return create_number(expr->data.number);
            
        case NODE_STRING:
            return create_string(expr->data.string);
            
        case NODE_IDENTIFIER:
            return get_variable(ctx, expr->data.identifier);
            
        case NODE_BINARY_OP: {
            Value left = evaluate_expression(expr->data.binary_op.left, ctx);
            Value right = evaluate_expression(expr->data.binary_op.right, ctx);
            return evaluate_binary_op(expr->data.binary_op.operator, left, right);
        }
        
        case NODE_UNARY_OP: {
            Value operand = evaluate_expression(expr->data.unary_op.operand, ctx);
            return evaluate_unary_op(expr->data.unary_op.operator, operand);
        }
        
        case NODE_ASSIGNMENT: {
            if (expr->data.binary_op.left->type == NODE_IDENTIFIER) {
                Value value = evaluate_expression(expr->data.binary_op.right, ctx);
                set_variable(ctx, expr->data.binary_op.left->data.identifier, value);
                return value;
            }
            break;
        }
        
        case NODE_CALL: {
            if (expr->data.call.function->type == NODE_IDENTIFIER) {
                const char* func_name = expr->data.call.function->data.identifier;
                
                // Built-in functions
                if (strcmp(func_name, "print") == 0) {
                    for (int i = 0; i < expr->data.call.arg_count; i++) {
                        Value arg = evaluate_expression(expr->data.call.args[i], ctx);
                        print_value(arg);
                        if (i < expr->data.call.arg_count - 1) printf(" ");
                    }
                    printf("\n");
                    return create_undefined();
                }
                
                // User-defined functions
                Function* func = get_function(ctx, func_name);
                if (func) {
                    return call_function(func, expr->data.call.args, expr->data.call.arg_count, ctx);
                }
            }
            break;
        }
        
        default:
            break;
    }
    
    return create_undefined();
}

static Value execute_statement(ASTNode* stmt, Context* ctx) {
    if (!stmt) return create_undefined();
    
    switch (stmt->type) {
        case NODE_VAR_DECLARATION: {
            Value value = create_undefined();
            if (stmt->data.var_decl.value) {
                value = evaluate_expression(stmt->data.var_decl.value, ctx);
            }
            set_variable(ctx, stmt->data.var_decl.name, value);
            break;
        }
        
        case NODE_FUNCTION_DECLARATION: {
            register_function(ctx, stmt->data.func_decl.name, 
                            stmt->data.func_decl.params,
                            stmt->data.func_decl.param_count,
                            stmt->data.func_decl.body);
            break;
        }
        
        case NODE_IF: {
            Value condition = evaluate_expression(stmt->data.if_stmt.condition, ctx);
            bool is_truthy = false;
            
            if (condition.type == VALUE_NUMBER) {
                is_truthy = condition.data.number != 0;
            } else if (condition.type == VALUE_STRING) {
                is_truthy = strlen(condition.data.string) > 0;
            }
            
            if (is_truthy) {
                return execute_statement(stmt->data.if_stmt.then_branch, ctx);
            } else if (stmt->data.if_stmt.else_branch) {
                return execute_statement(stmt->data.if_stmt.else_branch, ctx);
            }
            break;
        }
        
        case NODE_WHILE: {
            Value last_value = create_undefined();
            while (true) {
                Value condition = evaluate_expression(stmt->data.while_stmt.condition, ctx);
                bool is_truthy = false;
                
                if (condition.type == VALUE_NUMBER) {
                    is_truthy = condition.data.number != 0;
                } else if (condition.type == VALUE_STRING) {
                    is_truthy = strlen(condition.data.string) > 0;
                }
                
                if (!is_truthy) {
                    break;
                }
                
                last_value = execute_statement(stmt->data.while_stmt.body, ctx);
                if (ctx->has_return) {
                    break;
                }
            }
            return last_value;
        }
        
        case NODE_RETURN: {
            if (stmt->data.return_stmt.value) {
                ctx->return_value = evaluate_expression(stmt->data.return_stmt.value, ctx);
            } else {
                ctx->return_value = create_undefined();
            }
            ctx->has_return = true;
            return ctx->return_value;
        }
        
        case NODE_BLOCK:
        case NODE_PROGRAM: {
            return execute_block(stmt, ctx);
        }
        
        default: {
            return evaluate_expression(stmt, ctx);
        }
    }
    
    return create_undefined();
}

static Value execute_block(ASTNode* block, Context* ctx) {
    Value last_value = create_undefined();
    
    for (int i = 0; i < block->data.block.count; i++) {
        last_value = execute_statement(block->data.block.statements[i], ctx);
        if (ctx->has_return) {
            break;
        }
    }
    
    return last_value;
}

Value interpret(ASTNode* node, Context* ctx) {
    return execute_statement(node, ctx);
}
