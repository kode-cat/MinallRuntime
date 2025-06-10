#include "minall.h"

static INLINE bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static INLINE bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static INLINE bool is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

static INLINE bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static TokenType get_keyword_type(const char* word) {
    if (strcmp(word, "var") == 0) return TOKEN_VAR;
    if (strcmp(word, "function") == 0) return TOKEN_FUNCTION;
    if (strcmp(word, "if") == 0) return TOKEN_IF;
    if (strcmp(word, "else") == 0) return TOKEN_ELSE;
    if (strcmp(word, "for") == 0) return TOKEN_FOR;
    if (strcmp(word, "while") == 0) return TOKEN_WHILE;
    if (strcmp(word, "return") == 0) return TOKEN_RETURN;
    return TOKEN_IDENTIFIER;
}

Token* tokenize(const char* source, int* token_count) {
    Token* tokens = (Token*)minall_malloc(MAX_TOKENS * sizeof(Token));
    int count = 0;
    int line = 1;
    int column = 1;
    
    const char* current = source;
    
    while (*current != '\0' && count < MAX_TOKENS - 1) {
        // Skip whitespace - optimized
        if (LIKELY(is_whitespace(*current))) {
            if (UNLIKELY(*current == '\n')) {
                line++;
                column = 1;
            } else {
                column++;
            }
            current++;
            continue;
        }
        
        // Skip comments
        if (*current == '/' && *(current + 1) == '/') {
            while (*current != '\n' && *current != '\0') {
                current++;
            }
            continue;
        }
        
        Token* token = &tokens[count++];
        token->line = line;
        token->column = column;
        
        // Numbers
        if (is_digit(*current)) {
            token->type = TOKEN_NUMBER;
            double num = 0;
            double fraction = 0;
            double divisor = 1;
            bool has_decimal = false;
            
            while (is_digit(*current) || (*current == '.' && !has_decimal)) {
                if (*current == '.') {
                    has_decimal = true;
                } else if (!has_decimal) {
                    num = num * 10 + (*current - '0');
                } else {
                    divisor *= 10;
                    fraction = fraction * 10 + (*current - '0');
                }
                current++;
                column++;
            }
            
            token->number = num + fraction / divisor;
            continue;
        }
        
        // Strings
        if (*current == '"' || *current == '\'') {
            char quote = *current;
            current++;
            column++;
            
            char* str_start = (char*)current;
            int str_len = 0;
            
            while (*current != quote && *current != '\0') {
                str_len++;
                current++;
                column++;
            }
            
            if (*current == quote) {
                current++;
                column++;
            }
            
            token->type = TOKEN_STRING;
            token->value = (char*)minall_malloc(str_len + 1);
            strncpy(token->value, str_start, str_len);
            token->value[str_len] = '\0';
            continue;
        }
        
        // Identifiers and keywords
        if (is_alpha(*current)) {
            char* id_start = (char*)current;
            int id_len = 0;
            
            while (is_alnum(*current)) {
                id_len++;
                current++;
                column++;
            }
            
            char* identifier = (char*)minall_malloc(id_len + 1);
            strncpy(identifier, id_start, id_len);
            identifier[id_len] = '\0';
            
            token->type = get_keyword_type(identifier);
            token->value = identifier;
            continue;
        }
        
        // Two-character operators
        if (*current == '=' && *(current + 1) == '=') {
            token->type = TOKEN_EQUAL;
            current += 2;
            column += 2;
            continue;
        }
        
        if (*current == '!' && *(current + 1) == '=') {
            token->type = TOKEN_NOT_EQUAL;
            current += 2;
            column += 2;
            continue;
        }
        
        if (*current == '<' && *(current + 1) == '=') {
            token->type = TOKEN_LESS_EQUAL;
            current += 2;
            column += 2;
            continue;
        }
        
        if (*current == '>' && *(current + 1) == '=') {
            token->type = TOKEN_GREATER_EQUAL;
            current += 2;
            column += 2;
            continue;
        }
        
        if (*current == '&' && *(current + 1) == '&') {
            token->type = TOKEN_AND;
            current += 2;
            column += 2;
            continue;
        }
        
        if (*current == '|' && *(current + 1) == '|') {
            token->type = TOKEN_OR;
            current += 2;
            column += 2;
            continue;
        }
        
        // Single-character tokens
        switch (*current) {
            case '=': token->type = TOKEN_ASSIGN; break;
            case '+': token->type = TOKEN_PLUS; break;
            case '-': token->type = TOKEN_MINUS; break;
            case '*': token->type = TOKEN_MULTIPLY; break;
            case '/': token->type = TOKEN_DIVIDE; break;
            case '%': token->type = TOKEN_MODULO; break;
            case '<': token->type = TOKEN_LESS; break;
            case '>': token->type = TOKEN_GREATER; break;
            case '!': token->type = TOKEN_NOT; break;
            case '(': token->type = TOKEN_LPAREN; break;
            case ')': token->type = TOKEN_RPAREN; break;
            case '{': token->type = TOKEN_LBRACE; break;
            case '}': token->type = TOKEN_RBRACE; break;
            case ';': token->type = TOKEN_SEMICOLON; break;
            case ',': token->type = TOKEN_COMMA; break;
            case '.': token->type = TOKEN_DOT; break;
            default: token->type = TOKEN_UNKNOWN; break;
        }
        
        current++;
        column++;
    }
    
    // EOF token
    tokens[count].type = TOKEN_EOF;
    tokens[count].line = line;
    tokens[count].column = column;
    count++;
    
    *token_count = count;
    return tokens;
}

void print_tokens(Token* tokens, int count) {
    for (int i = 0; i < count; i++) {
        printf("Token %d: Type=%d", i, tokens[i].type);
        if (tokens[i].type == TOKEN_NUMBER) {
            printf(" Number=%.2f", tokens[i].number);
        } else if (tokens[i].value) {
            printf(" Value=%s", tokens[i].value);
        }
        printf(" Line=%d Column=%d\n", tokens[i].line, tokens[i].column);
    }
}
