#include "Lexer.hpp"
#include "../Utils/Error.hpp"
#include <iostream>

Lexer::Lexer(const std::string stream)
    : fstream{ stream }
{
    keymap = {{
        #define SHUSHA_LANG(x, y)
        #define SHUSHA_LANG_KEYWORD(name, keyword) { keyword, TokenKind::name },
        TOKEN_LIST
        #undef SHUSHA_LANG_KEYWORD
        #undef SHUSHA_LANG
    }};

}

void Lexer::get_keyword(const char *ident){
    keymap.find(ident);
}

bool Lexer::is_space(char curr){
    switch(curr){
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return true;
        default:
            return false;
    }
}

bool Lexer::is_id(char chr){
    if(('a' <= chr && chr <= 'z') 
        || ('A' <= chr && chr <= 'Z')){
        return true;
    }
    return false;
}

bool Lexer::is_sym(char chr) {
    if(chr == '_'){
        return true;
    }
    return false;
}

bool Lexer::is_digit(char chr){
    if(('0' <= chr) && (chr <= '9')){
        return true;
    }
    return false;
}


bool Lexer::is_syntax(Tokens& token) {
    std::string idbuff;
    int restore_pos = pos - 1;
    bool is_syntax = false;
    while(is_space(curr)) next();
    while(is_id(curr) || is_digit(curr)){
        idbuff.push_back(curr);
        next();
    }
    unordered_map<std::string, TokenKind>::iterator 
        iterator = keymap.find(idbuff);
    if(iterator != keymap.end()){
        token.kind = iterator->second;
        token.tokenbuff = std::move(idbuff);
        token.symbol = true;
        return true;
    }
    pos = restore_pos;
    next();
    return false;
}

bool Lexer::eat_string(Tokens& token){
    std::string idbuff;
    State st = Start;
    bool is_string = false;
    while(is_space(curr) || curr == '"') next();
    while(curr != '"'){
        switch(st){
            case Start:
                if(curr == '\\'){
                    st =  Escape;
                    next();
                    continue;
                }
                break;
            case Escape:
                switch(curr){
                    case 'n':
                        curr = 0x0a;
                        st = Start;
                        break;
                    default:
                        break;
                }
                break;
            case Hex:
                break;
        }
        is_string = true;
        idbuff.push_back(curr);
        next();
    }
    if(is_string){
        next();
        token.tokenbuff = std::move(idbuff);
        token.symbol = false;
        token.kind = STRING;
        return true;
    }
    return false;
}
bool Lexer::eat_till(Tokens& token){
    std::string idbuff;
    bool is_identifier = false;
    while(is_space(curr)) next();
    while(isalnum(curr) || is_sym(curr)){
        is_identifier = true; // found at least one character
        idbuff.push_back(curr);
        next();
    }
    if(is_identifier){
        token.kind = TokenKind::IDENTIFIER;
        token.tokenbuff = std::move(idbuff);
        token.symbol = false;
        return true;
    }
    return false;
}
bool Lexer::eat_int(Tokens& token){
    token.numbers.base = 10;
    int base = 10; // decimal
    int64_t val;
    if(is_digit(curr))
        val = curr - '0'; // cast to integer
    next(); 
    while(is_digit(curr)){
        int integer = curr - '0';
        val = val * base + integer;
        next();
    }
    token.numbers.value = val;
    token.kind = TokenKind::INTEGER;
    /// TODO(): add float and double support
    return true;
}

void Lexer::next(){
    curr = fstream[pos];
    pos++;
}

bool Lexer::isBinOp(Tokens& token){
    BinOpType binoptype;
    switch(token.kind){
        case ASTERISK:
            binoptype = BinOpTypeMUL;
            while(is_space(curr)) next();
            break;
        case PLUS:
            binoptype = BinOpTypeADD;
            while(is_space(curr)) next();
            break;
        case MINUS:
            binoptype = BinOpTypeSUB;
            while(is_space(curr)) next();
            break; 
        case DIV:
            binoptype = BinOpTypeDIV;
            while(is_space(curr)) next();
            break;
        default:
            break;
    }
    if(is_digit(curr))
    {
        token.binoptype = binoptype;
        return true;
    }
    return false;
}


TokenKind Lexer::eat_next(char currval, TokenKind newtoken, TokenKind deftoken) { 
    TokenKind token;
    next();
    if(curr != currval){ 
        token = deftoken;
        return token;
    } 
    token = newtoken;
    return token;
}

bool Lexer::getchr(Tokens& token, bool do_next) {
    start:
        switch(curr){
            case '\n':
            case '\t':
            case ' ': // Ignore Whitespace
                next();
                goto start;
                break;
            case '?':
                token.kind = COLON;
                if(do_next) next();
                return true;
            case ':':
                token.kind = COLON;
                if(do_next) next();
                return true;
            case '=':
                token.kind = EQUAL;
                if(do_next) next();
                return true;
            case '>':
                if(do_next) next();
                return true;
            case '-':
                token.kind = eat_next('>', RET_TYPE, MINUS);
                if(do_next) next();
                return true;
            case '(':
                token.kind = LEFT_PAREN;
                if(do_next) next();
                return true;
            case ')':
                token.kind = RIGHT_PAREN;
                if(do_next) next();
                return true;
            case '{':
                token.kind = LEFT_BRACE;
                if(do_next) next();
                return true;
            case '}':
                token.kind = RIGHT_BRACE;
                if(do_next) next();
                return true;
            case '*':
                token.kind = ASTERISK;
                if(do_next) next();
                return true;
            case '/':
                token.kind = eat_next('/', COMMENT, DIV);
                while(token.kind == COMMENT && curr != 0x0a) next();
                if(do_next) next();
                return true;
            case ';':
                token.kind = SEMICOLON;
                if(do_next) next();
                return true;
            case '+':
                token.kind = PLUS;
                if(do_next) next();
                return true;
            case '&':
                token.kind = AMPERSAND;
                if(do_next) next();
                return true;
            case ',':
                token.kind = COMMA;
                if(do_next) next();
                return true;
            case '\'':
                token.kind = SINGLE_QUOTE;
                if(do_next) next();
                return true;
            case '"':
                token.kind = STRING;
                return true;
        }
        if(is_digit(curr)) {
            token.kind = INTEGER; 
            return true;
        }
        if(is_id(curr)){
            token.kind = IDENTIFIER;
            return true;
        }
        if(curr == '\0'){
            token.kind = END_OF_FILE;
            return false;
        }
        return false;
}

