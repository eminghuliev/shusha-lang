#ifndef PARSER_HPP
#define PARSER_HPP
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include "../Ast/RootAst.hpp"
#include "../Ast/FnDef.hpp"
#include "../Sem/SemanticAnalyzer.hpp"
#include "../Sem/DeclarationAnalyzer.hpp"
#include "../CodeGen/Compiler.hpp"
class Parser {
    public:
        Parser(const std::string &fname);
        ~Parser();
        bool start(const RootAstPtr& programPtr);
        bool expect_next(TokenKind expect);
        bool expect(TokenKind expect);
        bool pred(char token);
        bool pred_bak(TokenKind expect);
        bool pred_next(char token);
        void Error(const std::string& err);
        ASTNodePtr ParseFn();
        ASTNodePtr ParseStatements();
        void ParseFnParams(Tokens& token, FuncParamListPtr& paramlist); 
        CodeScopePtr ParseCodeScope(const CodeScopePtr& codescopeptr);
        ParamListNodePtr ParseArgs(const CodeScopePtr& codescopeptr);
        Tokens& get_token(Tokens& token, TokenKind kind);
        ASTNodePtr parseTopLevel(Tokens& token);
        DeclExpressionPtr ParseVar();
        ExpressionPtr ParsePrefixOperation(const VarDeclPtr& vardeclptr);
        ExpressionPtr ParseExpression(Tokens& token);
        ExpressionPtr ParseSuffixExpr(Tokens& token);
        ExpressionPtr ParsePrefixDecl(Tokens& token);
        FnCallPtr FnCall(Tokens& token);
        bool isBinOp(Tokens& token);
        ASTNodePtr ParseReturn();
        ASTNodePtr ParseAsm();
        ASTNodePtr ParseId(Tokens& token); 
        void parse_asm_input(const AsmExprPtr& asmptr);
        TypeNodeTable itype_to_typetbl(TypeKind type);


    private:
        Lexer* lexer;
        File* file;
        Tokens token;

};
#endif
