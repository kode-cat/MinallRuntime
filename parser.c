#include "minall.h"

typedef struct {
    Token* tokens;
    int count;
    int current;
} Parser;

static Token* current_token(Parser* parser) {
    if (parser->current >= parser->count) {
        return &parser->tokens[parser->count - 1]; // EOF token
    }
    return &parser->tokens[parser->current];
}

static Token* advance(Parser* parser) {
    if (parser->current < parser->count - 1) {
        parser->current++;
    }
    return current_token(parser);
}

static bool match(Parser* parser, TokenType type) {
    if (current_token(parser)->type == type) {
        advance(parser);
        return true;
    }
    return false;
}

static ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)minall_malloc(sizeof(ASTNode));
    node->type = type;
    return node;
}

// Forward declarations
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_assignment(Parser* parser);
static ASTNode* parse_logical_or(Parser* parser);
static ASTNode* parse_logical_and(Parser* parser);
static ASTNode* parse_equality(Parser* parser);
static ASTNode* parse_relational(Parser* parser);
static ASTNode* parse_additive(Parser* parser);
static ASTNode* parse_multiplicative(Parser* parser);
static ASTNode* parse_unary(Parser* parser);
static ASTNode* parse_call(Parser* parser);
static ASTNode* parse_primary(Parser* parser);

static ASTNode* parse_block(Parser* parser) {
    if (!match(parser, TOKEN_LBRACE)) {
        return NULL;
    }
    
    ASTNode* block = create_node(NODE_BLOCK);
    block->data.block.statements = (ASTNode**)minall_malloc(100 * sizeof(ASTNode*));
    block->data.block.count = 0;
    
    while (current_token(parser)->type != TOKEN_RBRACE && 
           current_token(parser)->type != TOKEN_EOF) {
        ASTNode* stmt = parse_statement(parser);
        if (stmt) {
            block->data.block.statements[block->data.block.count++] = stmt;
        }
    }
    
    match(parser, TOKEN_RBRACE);
    return block;
}

static ASTNode* parse_var_declaration(Parser* parser) {
    advance(parser); // consume 'var'
    
    if (current_token(parser)->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_VAR_DECLARATION);
    node->data.var_decl.name = (char*)minall_malloc(strlen(current_token(parser)->value) + 1);
    strcpy(node->data.var_decl.name, current_token(parser)->value);
    
    advance(parser);
    
    if (match(parser, TOKEN_ASSIGN)) {
        node->data.var_decl.value = parse_expression(parser);
    } else {
        node->data.var_decl.value = NULL;
    }
    
    match(parser, TOKEN_SEMICOLON);
    return node;
}

static ASTNode* parse_function_declaration(Parser* parser) {
    advance(parser); // consume 'function'
    
    if (current_token(parser)->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_FUNCTION_DECLARATION);
    node->data.func_decl.name = (char*)minall_malloc(strlen(current_token(parser)->value) + 1);
    strcpy(node->data.func_decl.name, current_token(parser)->value);
    
    advance(parser);
    
    if (!match(parser, TOKEN_LPAREN)) {
        return NULL;
    }
    
    node->data.func_decl.params = (char**)minall_malloc(10 * sizeof(char*));
    node->data.func_decl.param_count = 0;
    
    while (current_token(parser)->type != TOKEN_RPAREN && 
           current_token(parser)->type != TOKEN_EOF) {
        if (current_token(parser)->type == TOKEN_IDENTIFIER) {
            node->data.func_decl.params[node->data.func_decl.param_count] = 
                (char*)minall_malloc(strlen(current_token(parser)->value) + 1);
            strcpy(node->data.func_decl.params[node->data.func_decl.param_count], 
                   current_token(parser)->value);
            node->data.func_decl.param_count++;
            advance(parser);
            
            if (current_token(parser)->type == TOKEN_COMMA) {
                advance(parser);
            }
        } else {
            break;
        }
    }
    
    match(parser, TOKEN_RPAREN);
    node->data.func_decl.body = parse_block(parser);
    
    return node;
}

static ASTNode* parse_if_statement(Parser* parser) {
    advance(parser); // consume 'if'
    
    if (!match(parser, TOKEN_LPAREN)) {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_IF);
    node->data.if_stmt.condition = parse_expression(parser);
    
    if (!match(parser, TOKEN_RPAREN)) {
        return NULL;
    }
    
    node->data.if_stmt.then_branch = parse_statement(parser);
    
    if (current_token(parser)->type == TOKEN_ELSE) {
        advance(parser);
        node->data.if_stmt.else_branch = parse_statement(parser);
    } else {
        node->data.if_stmt.else_branch = NULL;
    }
    
    return node;
}

static ASTNode* parse_while_statement(Parser* parser) {
    advance(parser); // consume 'while'
    
    if (!match(parser, TOKEN_LPAREN)) {
        return NULL;
    }
    
    ASTNode* node = create_node(NODE_WHILE);
    node->data.while_stmt.condition = parse_expression(parser);
    
    if (!match(parser, TOKEN_RPAREN)) {
        return NULL;
    }
    
    node->data.while_stmt.body = parse_statement(parser);
    
    return node;
}

static ASTNode* parse_return_statement(Parser* parser) {
    advance(parser); // consume 'return'
    
    ASTNode* node = create_node(NODE_RETURN);
    
    if (current_token(parser)->type != TOKEN_SEMICOLON) {
        node->data.return_stmt.value = parse_expression(parser);
    } else {
        node->data.return_stmt.value = NULL;
    }
    
    match(parser, TOKEN_SEMICOLON);
    return node;
}

static ASTNode* parse_statement(Parser* parser) {
    switch (current_token(parser)->type) {
        case TOKEN_VAR:
            return parse_var_declaration(parser);
        case TOKEN_FUNCTION:
            return parse_function_declaration(parser);
        case TOKEN_IF:
            return parse_if_statement(parser);
        case TOKEN_WHILE:
            return parse_while_statement(parser);
        case TOKEN_RETURN:
            return parse_return_statement(parser);
        case TOKEN_LBRACE:
            return parse_block(parser);
        default: {
            ASTNode* expr = parse_expression(parser);
            match(parser, TOKEN_SEMICOLON);
            return expr;
        }
    }
}

static ASTNode* parse_expression(Parser* parser) {
    return parse_assignment(parser);
}

static ASTNode* parse_assignment(Parser* parser) {
    ASTNode* expr = parse_logical_or(parser);
    
    if (current_token(parser)->type == TOKEN_ASSIGN) {
        advance(parser);
        ASTNode* node = create_node(NODE_ASSIGNMENT);
        node->data.binary_op.left = expr;
        node->data.binary_op.right = parse_assignment(parser);
        node->data.binary_op.operator = "=";
        return node;
    }
    
    return expr;
}

static ASTNode* parse_logical_or(Parser* parser) {
    ASTNode* expr = parse_logical_and(parser);
    
    while (current_token(parser)->type == TOKEN_OR) {
        advance(parser);
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->data.binary_op.left = expr;
        node->data.binary_op.right = parse_logical_and(parser);
        node->data.binary_op.operator = "||";
        expr = node;
    }
    
    return expr;
}

static ASTNode* parse_logical_and(Parser* parser) {
    ASTNode* expr = parse_equality(parser);
    
    while (current_token(parser)->type == TOKEN_AND) {
        advance(parser);
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->data.binary_op.left = expr;
        node->data.binary_op.right = parse_equality(parser);
        node->data.binary_op.operator = "&&";
        expr = node;
    }
    
    return expr;
}

static ASTNode* parse_equality(Parser* parser) {
    ASTNode* expr = parse_relational(parser);
    
    while (current_token(parser)->type == TOKEN_EQUAL || 
           current_token(parser)->type == TOKEN_NOT_EQUAL) {
        TokenType op = current_token(parser)->type;
        advance(parser);
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->data.binary_op.left = expr;
        node->data.binary_op.right = parse_relational(parser);
        node->data.binary_op.operator = (op == TOKEN_EQUAL) ? "==" : "!=";
        expr = node;
    }
    
    return expr;
}

static ASTNode* parse_relational(Parser* parser) {
    ASTNode* expr = parse_additive(parser);
    
    while (current_token(parser)->type == TOKEN_LESS || 
           current_token(parser)->type == TOKEN_GREATER ||
           current_token(parser)->type == TOKEN_LESS_EQUAL ||
           current_token(parser)->type == TOKEN_GREATER_EQUAL) {
        TokenType op = current_token(parser)->type;
        advance(parser);
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->data.binary_op.left = expr;
        node->data.binary_op.right = parse_additive(parser);
        
        switch (op) {
            case TOKEN_LESS: node->data.binary_op.operator = "<"; break;
            case TOKEN_GREATER: node->data.binary_op.operator = ">"; break;
            case TOKEN_LESS_EQUAL: node->data.binary_op.operator = "<="; break;
            case TOKEN_GREATER_EQUAL: node->data.binary_op.operator = ">="; break;
            default: break;
        }
        expr = node;
    }
    
    return expr;
}

static ASTNode* parse_additive(Parser* parser) {
    ASTNode* expr = parse_multiplicative(parser);
    
    while (current_token(parser)->type == TOKEN_PLUS || 
           current_token(parser)->type == TOKEN_MINUS) {
        TokenType op = current_token(parser)->type;
        advance(parser);
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->data.binary_op.left = expr;
        node->data.binary_op.right = parse_multiplicative(parser);
        node->data.binary_op.operator = (op == TOKEN_PLUS) ? "+" : "-";
        expr = node;
    }
    
    return expr;
}

static ASTNode* parse_multiplicative(Parser* parser) {
    ASTNode* expr = parse_unary(parser);
    
    while (current_token(parser)->type == TOKEN_MULTIPLY || 
           current_token(parser)->type == TOKEN_DIVIDE ||
           current_token(parser)->type == TOKEN_MODULO) {
        TokenType op = current_token(parser)->type;
        advance(parser);
        ASTNode* node = create_node(NODE_BINARY_OP);
        node->data.binary_op.left = expr;
        node->data.binary_op.right = parse_unary(parser);
        
        switch (op) {
            case TOKEN_MULTIPLY: node->data.binary_op.operator = "*"; break;
            case TOKEN_DIVIDE: node->data.binary_op.operator = "/"; break;
            case TOKEN_MODULO: node->data.binary_op.operator = "%"; break;
            default: break;
        }
        expr = node;
    }
    
    return expr;
}

static ASTNode* parse_unary(Parser* parser) {
    if (current_token(parser)->type == TOKEN_NOT || 
        current_token(parser)->type == TOKEN_MINUS) {
        TokenType op = current_token(parser)->type;
        advance(parser);
        ASTNode* node = create_node(NODE_UNARY_OP);
        node->data.unary_op.operand = parse_unary(parser);
        node->data.unary_op.operator = (op == TOKEN_NOT) ? "!" : "-";
        return node;
    }
    
    return parse_call(parser);
}

static ASTNode* parse_call(Parser* parser) {
    ASTNode* expr = parse_primary(parser);
    
    while (current_token(parser)->type == TOKEN_LPAREN) {
        advance(parser);
        ASTNode* node = create_node(NODE_CALL);
        node->data.call.function = expr;
        node->data.call.args = (ASTNode**)minall_malloc(10 * sizeof(ASTNode*));
        node->data.call.arg_count = 0;
        
        while (current_token(parser)->type != TOKEN_RPAREN && 
               current_token(parser)->type != TOKEN_EOF) {
            node->data.call.args[node->data.call.arg_count++] = parse_expression(parser);
            
            if (current_token(parser)->type == TOKEN_COMMA) {
                advance(parser);
            }
        }
        
        match(parser, TOKEN_RPAREN);
        expr = node;
    }
    
    return expr;
}

static ASTNode* parse_primary(Parser* parser) {
    Token* token = current_token(parser);
    
    switch (token->type) {
        case TOKEN_NUMBER: {
            advance(parser);
            ASTNode* node = create_node(NODE_NUMBER);
            node->data.number = token->number;
            return node;
        }
        case TOKEN_STRING: {
            advance(parser);
            ASTNode* node = create_node(NODE_STRING);
            node->data.string = (char*)minall_malloc(strlen(token->value) + 1);
            strcpy(node->data.string, token->value);
            return node;
        }
        case TOKEN_IDENTIFIER: {
            advance(parser);
            ASTNode* node = create_node(NODE_IDENTIFIER);
            node->data.identifier = (char*)minall_malloc(strlen(token->value) + 1);
            strcpy(node->data.identifier, token->value);
            return node;
        }
        case TOKEN_LPAREN: {
            advance(parser);
            ASTNode* expr = parse_expression(parser);
            match(parser, TOKEN_RPAREN);
            return expr;
        }
        default:
            return NULL;
    }
}

ASTNode* parse(Token* tokens, int token_count) {
    Parser parser = {tokens, token_count, 0};
    
    ASTNode* program = create_node(NODE_PROGRAM);
    program->data.block.statements = (ASTNode**)minall_malloc(100 * sizeof(ASTNode*));
    program->data.block.count = 0;
    
    while (current_token(&parser)->type != TOKEN_EOF) {
        ASTNode* stmt = parse_statement(&parser);
        if (stmt) {
            program->data.block.statements[program->data.block.count++] = stmt;
        }
    }
    
    return program;
}

void print_ast(ASTNode* node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    
    switch (node->type) {
        case NODE_PROGRAM:
        case NODE_BLOCK:
            printf("Block (%d statements)\n", node->data.block.count);
            for (int i = 0; i < node->data.block.count; i++) {
                print_ast(node->data.block.statements[i], depth + 1);
            }
            break;
        case NODE_VAR_DECLARATION:
            printf("VarDecl: %s\n", node->data.var_decl.name);
            if (node->data.var_decl.value) {
                print_ast(node->data.var_decl.value, depth + 1);
            }
            break;
        case NODE_FUNCTION_DECLARATION:
            printf("FuncDecl: %s (%d params)\n", 
                   node->data.func_decl.name, node->data.func_decl.param_count);
            print_ast(node->data.func_decl.body, depth + 1);
            break;
        case NODE_BINARY_OP:
            printf("BinaryOp: %s\n", node->data.binary_op.operator);
            print_ast(node->data.binary_op.left, depth + 1);
            print_ast(node->data.binary_op.right, depth + 1);
            break;
        case NODE_UNARY_OP:
            printf("UnaryOp: %s\n", node->data.unary_op.operator);
            print_ast(node->data.unary_op.operand, depth + 1);
            break;
        case NODE_CALL:
            printf("Call (%d args)\n", node->data.call.arg_count);
            print_ast(node->data.call.function, depth + 1);
            for (int i = 0; i < node->data.call.arg_count; i++) {
                print_ast(node->data.call.args[i], depth + 1);
            }
            break;
        case NODE_IF:
            printf("If\n");
            print_ast(node->data.if_stmt.condition, depth + 1);
            print_ast(node->data.if_stmt.then_branch, depth + 1);
            if (node->data.if_stmt.else_branch) {
                print_ast(node->data.if_stmt.else_branch, depth + 1);
            }
            break;
        case NODE_WHILE:
            printf("While\n");
            print_ast(node->data.while_stmt.condition, depth + 1);
            print_ast(node->data.while_stmt.body, depth + 1);
            break;
        case NODE_RETURN:
            printf("Return\n");
            if (node->data.return_stmt.value) {
                print_ast(node->data.return_stmt.value, depth + 1);
            }
            break;
        case NODE_IDENTIFIER:
            printf("Identifier: %s\n", node->data.identifier);
            break;
        case NODE_NUMBER:
            printf("Number: %.2f\n", node->data.number);
            break;
        case NODE_STRING:
            printf("String: %s\n", node->data.string);
            break;
        default:
            printf("Unknown node type\n");
            break;
    }
}
