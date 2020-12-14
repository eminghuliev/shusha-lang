#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>
using namespace std;

#define TOKEN_LIST \
SHUSHA_LANG (FIRST_TOKEN, "<first-token-marker>") \
SHUSHA_LANG (END_OF_FILE, "end of file") \
SHUSHA_LANG (ASSIGN, ":=") \
SHUSHA_LANG (ASTERISK, "*") \
SHUSHA_LANG (COLON, ":") \
SHUSHA_LANG (DIFFERENT, "!=") \
SHUSHA_LANG (EQUAL, "=") \
SHUSHA_LANG (LEFT_PAREN, "(") \
SHUSHA_LANG (MINUS, "-") \
SHUSHA_LANG (PLUS, "+") \
SHUSHA_LANG (COMMA, ",") \
SHUSHA_LANG (RIGHT_PAREN, ")") \
SHUSHA_LANG (SEMICOLON, ";") \
SHUSHA_LANG (DOUBLE_QUOTES, "\"") \
SHUSHA_LANG (SINGLE_QUOTE, "'") \
SHUSHA_LANG (DIV, "/") \
SHUSHA_LANG (PERCENT, "%") \
SHUSHA_LANG (LEFT_BRACKET, "[") \
SHUSHA_LANG (RIGHT_BRACKET, "]") \
SHUSHA_LANG (LEFT_BRACE, "{") \
SHUSHA_LANG (RIGHT_BRACE, "}") \
SHUSHA_LANG (GREATER, ">") \
SHUSHA_LANG (QUESTION, "?") \
SHUSHA_LANG (AMPERSAND, "&") \
SHUSHA_LANG (GREATER_OR_EQUAL, ">=") \
SHUSHA_LANG (RET_TYPE, "->") \
SHUSHA_LANG (COMMENT, "//") \
SHUSHA_LANG (LOWER, "<") \
SHUSHA_LANG (WHITESPACE, " ") \
SHUSHA_LANG (LOWER_OR_EQUAL, "<=") \
SHUSHA_LANG (IDENTIFIER, "identifier") \
SHUSHA_LANG (INTEGER, "integer literal") \
SHUSHA_LANG (REAL, "real literal") \
SHUSHA_LANG (STRING, "string literal") \
SHUSHA_LANG (SYNTAX, "syntax") \
SHUSHA_LANG (BIN_OP, "binary operation") \
\
SHUSHA_LANG_KEYWORD (AND, "and") \
SHUSHA_LANG_KEYWORD (DO, "do") \
SHUSHA_LANG_KEYWORD (ELSE, "else") \
SHUSHA_LANG_KEYWORD (END, "end") \
SHUSHA_LANG_KEYWORD (FOR, "for") \
SHUSHA_LANG_KEYWORD (IF, "if") \
SHUSHA_LANG_KEYWORD (NOT, "not") \
SHUSHA_LANG_KEYWORD (OR, "or") \
SHUSHA_LANG_KEYWORD (READ, "read") \
SHUSHA_LANG_KEYWORD (THEN, "then") \
SHUSHA_LANG_KEYWORD (VAR, "let") \
SHUSHA_LANG_KEYWORD (ALIGN, "align") \
SHUSHA_LANG_KEYWORD (VOLATILE, "volatile") \
SHUSHA_LANG_KEYWORD (MUT, "mut") \
SHUSHA_LANG_KEYWORD (RET, "ret") \
SHUSHA_LANG_KEYWORD (WHILE, "while") \
SHUSHA_LANG_KEYWORD (WRITE, "write") \
SHUSHA_LANG_KEYWORD (ASM, "asm") \
SHUSHA_LANG_KEYWORD (FN, "fn") \
SHUSHA_LANG_KEYWORD (IN, "in") \
SHUSHA_LANG_KEYWORD (OUT, "out") \
SHUSHA_LANG_KEYWORD (CLOB, "clob") \
\
SHUSHA_LANG (LAST_TOKEN, "<last-token-marker>")

#define TYPE_TOKENS \
SHUSHA_LANG_TYPE(I32, "i32") \
SHUSHA_LANG_TYPE(I16, "i16") \
SHUSHA_LANG_TYPE(I8,  "i8") \
SHUSHA_LANG_TYPE(U32, "u32") \
SHUSHA_LANG_TYPE(U16, "u16") \
SHUSHA_LANG_TYPE(U8,  "u8") \
SHUSHA_LANG_TYPE(STR, "str") \
SHUSHA_LANG_TYPE(VOID, "void") \
SHUSHA_LANG_TYPE(FLOAT, "float") \
SHUSHA_LANG_TYPE(DOUBLE, "double") \
SHUSHA_LANG_TYPE(BOOL, "bool")


typedef enum {
    Start,
    Escape,
    Hex,
} State;

typedef enum
{
    #define SHUSHA_LANG(name, _) name,
    #define SHUSHA_LANG_KEYWORD(x, y) SHUSHA_LANG (x, y)
    TOKEN_LIST
    #undef SHUSHA_LANG_KEYWORD
    #undef SHUSHA_LANG
} TokenKind;

typedef enum
{
    #define SHUSHA_LANG_TYPE(type, str) type,
    TYPE_TOKENS
    #undef SHUSHA_LANG_TYPE
} TypeKind;

enum BinOpType {
    BinOpTypeADD,
    BinOpTypeSUB,
    BinOpTypeDIV,
    BinOpTypeMUL,
    BinOpTypeAND,
    BinOpTypeXOR,
    BinOpTypeBoolOR,
    BinOpTypeBoolAND,
    BinOpTypeMod,
};


typedef struct {
    TokenKind kind;
    TypeKind typekind;
    BinOpType binoptype;
    std::string tokenbuff;
    bool symbol;
    union {
        struct {
            int base;
            int64_t value;
        } numbers;
    };
} Tokens;

class Lexer {
    private:
        const std::string fstream;
    public:
        char curr;
        int pos = 0;
        int pos_id = 0;
        typedef struct{
            char const *symbol;
            TokenKind token;
        } Keyword;

        unordered_map<std::string, TokenKind> keymap;
        Lexer(const std::string stream);

        void next();
        bool eat_till(Tokens& token);
        bool eat_types(Tokens& token); 
        bool eat_string(Tokens& token);
        bool getchr(Tokens& token, bool do_next = true);
        bool is_space(char curr);
        bool is_sym(char chr); 
        bool is_id(char chr);
        bool is_digit(char chr);
        bool eat_int(Tokens& token);
        bool get_types(Tokens& token);
        bool check_type(Tokens& token, const std::string &idbuff);
        char get_next();
        bool is_syntax(Tokens& token);
        void get_keyword(const char *ident);
        bool isBinOp(Tokens& token);
        TokenKind eat_next(char currval, TokenKind currtype, TokenKind deftype);
    
    private:
        
};
