#include <cstdio>
#include "Parser.hpp"
#include "../Utils/Error.hpp"
#include <fstream>
Parser::Parser(const std::string &fname) {
    file = new File();
    const std::string& fcontent = file->open(fname.c_str());
    lexer = new Lexer(fcontent);
    lexer->next();
}
Parser::~Parser() {
    delete file;
    delete lexer;
}
Tokens& Parser::get_token(Tokens& token, TokenKind kind) {
    token = {};
    if(kind == SYNTAX) lexer->is_syntax(token); // eat in case of syntax id
    else if(kind == IDENTIFIER) lexer->eat_till(token);
    else if(kind == INTEGER) lexer->eat_int(token);
    else if(kind == STRING) lexer->eat_string(token);
    return token;
}

bool Parser::expect_next(TokenKind expect) {
    while(lexer->getchr(token, false)){
        if(token.kind != WHITESPACE && 
            token.kind != '\0' && 
            token.kind == expect) {
            lexer->next();
            return true;
        }
        else 
            return false;
    }
    return false;
}

bool Parser::pred(char token) {
    while(lexer->is_space(lexer->curr)) lexer->next();
    if(lexer->curr != token) return true;
    return false;
}

bool Parser::pred_next(char token) {
    while(lexer->is_space(lexer->curr)) lexer->next();
    if(lexer->curr != token) return true;
    lexer->next();
    return false;
}
ParamListNodePtr Parser::ParseArgs(const CodeScopePtr& codescopeptr) {
    Tokens token = {};
    ParamListNodePtr paramlistptr = create<ParamListNode>();
    do {
        TypeIdentifierPtr typeidPtr = create<TypeIdentifier>();
        ParamNodePtr paramnodePtr = create<ParamNode>();
        get_token(token, IDENTIFIER);
        if(token.kind != IDENTIFIER) ::Error("Expected token: Identifier");
        std::string argname = token.tokenbuff;
        paramnodePtr->setArgName(argname);
        expect_next(COLON);
        get_token(token, IDENTIFIER);
        typeidPtr->setName(token.tokenbuff);
        paramnodePtr->setArgType(typeidPtr);
        /// Creating variable for parameters in fn scope
        VarDeclPtr varptr = create<VarDecl>();
        varptr->setName(argname);
        varptr->setType(typeidPtr);
        codescopeptr->insert(varptr);
        paramlistptr->addParam(paramnodePtr);
    } while(expect_next(COMMA));
    return paramlistptr;
}

void Parser::ParseFnParams(Tokens& token, 
        FuncParamListPtr& paramlist) {
    while(true) {
        lexer->getchr(token, false);
        ExpressionPtr arg = ParseExpression(token);
        paramlist->insert(arg);
        if(expect_next(RIGHT_PAREN))
            return;
        else if(!expect_next(COMMA))
            ::Error("Expect , token\n");
        else 
            continue;
    }
    return;
} 

ExpressionPtr Parser::ParseSuffixExpr(Tokens& token) {
    if(token.kind == INTEGER) {
        get_token(token, INTEGER);
        IntegerLiteralPtr integer = create<IntegerLiteral>();
        integer->val = token.numbers.value;
        return integer;
    }
    if(token.kind == IDENTIFIER) {
        get_token(token, IDENTIFIER);
        SymbolPtr symptr = create<Symbol>();
        symptr->setsym(token.tokenbuff);
        return symptr;
    }
    if(token.kind == STRING) {
        get_token(token, STRING);
        StringLiteralPtr string = create<StringLiteral>();
        string->value = token.tokenbuff;
        return string;
    }
    return nullptr;
}
ExpressionPtr Parser::ParsePrefixDecl(Tokens& token) {
    if(token.kind == AMPERSAND){ // is that ptr?
        PrefixOperationPtr prefixptr = create<PrefixOperation>();
        lexer->getchr(token);
        ExpressionPtr suffixres = ParsePrefixDecl(token);
        prefixptr->setOperation(suffixres);
        prefixptr->setType(PrefixOperationGetPointer);
        return prefixptr;
    }
    ExpressionPtr suffixres = ParseSuffixExpr(token);
    if(expect_next(LEFT_PAREN)) {
        FnCallPtr fncall = create<FuncCall>();
        fncall->setFnExpr(suffixres);
        if(!expect_next(RIGHT_PAREN)){
            FuncParamListPtr plist = create<FuncParamList>();
            ParseFnParams(token, plist);
            fncall->setParams(plist);
        }
        if(!expect_next(SEMICOLON)) ::Error("Expected ; token\n");
        return fncall;
    }
    if(expect_next(EQUAL)){
        VarAssignPtr assignptr = create<VarAssign>();
        assignptr->setname(suffixres);
        lexer->getchr(token, false);
        ExpressionPtr exprptr = ParseExpression(token);
        assignptr->setexpr(exprptr);
        return assignptr;
    }
    return suffixres;
}
ExpressionPtr Parser::ParseExpression(Tokens& token) {
    ExpressionPtr res = ParsePrefixDecl(token);
    if(lexer->isBinOp(token)){
        BinaryExpressionPtr binexprptr = create<BinaryExpression>();
        ExpressionPtr rhs = ParsePrefixDecl(token);
        binexprptr->setBinOpType(token.binoptype);
        binexprptr->setlhs(res);
        binexprptr->setrhs(rhs);
        return binexprptr;
    }
    return res;
}

ExpressionPtr Parser::ParsePrefixOperation(const VarDeclPtr& vardeclptr) {
    Tokens token = {};
    PointerPtr pptr;
    lexer->getchr(token);
    if(token.kind == ASTERISK){
        vardeclptr->is_ptr = true; 
        get_token(token, IDENTIFIER); // identifier for type
    } else if(token.kind == IDENTIFIER) get_token(token, IDENTIFIER);
    TypeIdentifierPtr typeidPtr = create<TypeIdentifier>();
    typeidPtr->setName(token.tokenbuff);
    vardeclptr->setType(typeidPtr);
   
    if(expect_next(IDENTIFIER)){
        get_token(token, SYNTAX);
        if(token.kind == ALIGN){
            if(!expect_next(LEFT_PAREN)) ::Error("Expected ( token\n");
            get_token(token, INTEGER);
            AlignExprPtr align = create<AlignExpr>();
            align->setAlign(token.numbers.value);
            if(!expect_next(RIGHT_PAREN)) ::Error("Expect ) token\n");
            lexer->getchr(token);
        }
    }
    if(expect_next(EQUAL)){
        ExpressionPtr expr_res;
        lexer->getchr(token);
        expr_res = ParseExpression(token);
        if(!expect_next(SEMICOLON)){
            ::Error("Expected ; token\n");
        }
        return expr_res;
    }
    return nullptr;

}

DeclExpressionPtr Parser::ParseVar() {
    get_token(token, SYNTAX);
    bool is_mut = false;
    if(token.kind == MUT){
        get_token(token, IDENTIFIER);
        if(token.kind != IDENTIFIER) ::Error("Expected token: identifier\n"); 
        /// Variable is mutable
        is_mut = true;
    }
    else
    {
        get_token(token, IDENTIFIER);
        if(token.kind != IDENTIFIER) ::Error("Expected token: identifier\n"); 
    }
    VarDeclPtr expr = create<VarDecl>();
    expr->setName(token.tokenbuff);
    if(!expect_next(COLON)) ::Error("Expect : token\n");
    ExpressionPtr var_expr = ParsePrefixOperation(expr);
    expr->setExpr(var_expr);
    expr->setmut(is_mut);
    return expr;
}

ASTNodePtr Parser::ParseReturn() {
    Tokens token = {};
    RetStatementPtr ret = create<ReturnStatement>();
    lexer->getchr(token);
    ExpressionPtr ret_expr = ParseExpression(token);
    ret->setRet(ret_expr);
    if(!expect_next(SEMICOLON)) ::Error("Expected ; token\n");
    return ret;
}

/// Parse ASM input
void Parser::parse_asm_input(const AsmExprPtr& asmptr) {
    while(true){
        get_token(token, SYNTAX);
        if(token.kind == IN){
            if(!expect_next(LEFT_PAREN)) ::Error("Expect ( token\n");    
            get_token(token, STRING); 
            AsmInputPtr asminput = create<AsmInput>();
            asminput->constraint = token.tokenbuff;
            if(!expect_next(RIGHT_PAREN)) ::Error("Expect ) token\n");
            if(!expect_next(EQUAL)) ::Error("Expect = token\n");
            lexer->getchr(token);
            ExpressionPtr expr = ParseExpression(token);
            asminput->exprptr = expr;
            asmptr->insertin(asminput);
        }
        else if(token.kind == OUT) {
            if(!expect_next(LEFT_PAREN)) ::Error("Expect ( token\n");    
            get_token(token, STRING);
            AsmOutputPtr asmout = create<AsmOutput>();
            asmout->constraint = token.tokenbuff;
            if(!expect_next(RIGHT_PAREN)) ::Error("Expect ) token\n");
            if(!expect_next(EQUAL)) ::Error("Expect = token\n");
            lexer->getchr(token);
            ExpressionPtr expr = ParseExpression(token);
            asmout->exprptr = expr;
            asmptr->insertout(asmout);
        }
        else if(token.kind == CLOB){
            if(!expect_next(LEFT_PAREN)) ::Error("Expect ( token\n");
            do {
                token = get_token(token, STRING);
                if(token.kind == STRING){
                    AsmClobPtr asmclob = create<AsmClob>();
                    asmclob->clobname = token.tokenbuff;
                    asmptr->insertclob(asmclob);
                    expect_next(RIGHT_PAREN);
                }
            } while(expect_next(COMMA));
        }
        if(expect_next(COMMA)){
            lexer->next();
            continue;
        }
        else break;
    }
    if(!expect_next(RIGHT_PAREN)) ::Error("Expect ) token\n");
}

ASTNodePtr Parser::ParseAsm() {
    Tokens token = {};
    AsmExprPtr asmptr = create<AsmExpr>();
    if(!expect_next(LEFT_PAREN)) ::Error("Expect ( token\n");
    if(!expect_next(STRING)) ::Error("Expected string in asm\n");
    get_token(token, STRING); 
    if(token.kind == STRING) asmptr->asmtemplate = token.tokenbuff;
    if(!expect_next(RIGHT_PAREN)) {
        if(expect_next(COLON))
            parse_asm_input(asmptr);
        else
            ::Error("Expected : token\n");
    }
    if(!expect_next(SEMICOLON)){
        ::Error("Expected ; token\n");
    }
    return asmptr;
}
ASTNodePtr Parser::ParseStatements() {
    Tokens token = {};
    get_token(token, SYNTAX);
    switch(token.kind){
        case VAR:
            return ParseVar();
            break;
        case IF:
            break;
        case WHILE:
            break;
        case RET:
            return ParseReturn();
            break;
        case ASM:
            return ParseAsm();
        default:
            break;
    }
    lexer->getchr(token);
    if(token.kind == END_OF_FILE){
        return nullptr;
    }
    ExpressionPtr ret = ParseExpression(token);
    return ret;
}

CodeScopePtr Parser::ParseCodeScope(const CodeScopePtr& codescopeptr) {
    Tokens token = {};
    while(!expect_next(RIGHT_BRACE) && lexer->curr != 0x0){
        ASTNodePtr statement = ParseStatements();
        if(statement)
            codescopeptr->insert(statement);
    }
    return codescopeptr;
}

ASTNodePtr Parser::ParseFn() {
    Tokens token = {};
    FnDefPtr fndef = create<FnDef>();
    FnProtoPtr fnprotoptr = create<FnProto>();
    ParamListNodePtr plistptr;
    CodeScopePtr codescopeptr = create<CodeScope>();
    lexer->next();
    get_token(token, IDENTIFIER);// fn_name
    if(token.kind != IDENTIFIER) ::Error("Expected token: FN_NAME\n");
    fndef->setFnName(token.tokenbuff);
    fndef->fnproto = fnprotoptr;
    if(!expect_next(LEFT_PAREN)) 
        ::Error("Expected token (\n");
    while(!expect_next(RIGHT_PAREN))
        plistptr = ParseArgs(codescopeptr);
    if(plistptr)
        fnprotoptr->setParamList(plistptr);
    lexer->next();
    /// Go to next char and check to get return type
    lexer->getchr(token);
    /// Checking to make sure that function has return type
    if(token.kind == RET_TYPE){
        get_token(token, IDENTIFIER);
        TypeIdentifierPtr typeptr = create<TypeIdentifier>();
        typeptr->setName(token.tokenbuff);
        fnprotoptr->setRetType(typeptr);
        lexer->getchr(token);
    }
    if(token.kind == LEFT_BRACE) {
        /// Parse Function block scope. It starts with { and ends with }
        fndef->setbody(ParseCodeScope(codescopeptr));
    }
    else {
        ::Error("Expected { token\n");
    }
    return fndef;
}

ASTNodePtr Parser::parseTopLevel(Tokens& token) {
    get_token(token, SYNTAX);
    switch(token.kind){
        case FN:
            return ParseFn();
        case VAR:
        case END_OF_FILE:
        case END:
        default:
            break;
    }
    return nullptr;
}

bool Parser::start(const RootAstPtr& programPtr) {
    Tokens token = {};
    // Parse top level declaration
    while(lexer->getchr(token)){
        ASTNodePtr toplevelptr = parseTopLevel(token);
        if(!toplevelptr){
            continue;
        }
        programPtr->addAst(toplevelptr);
    }
    return false;
}

int main(int argc, char **argv) {
    bool dump_ir = false;
    if(argc < 2){
        shusha_panic("No input files");
    }
    std::string fname = argv[1];
    for(int ii = 1; ii < argc; ii +=1){
        char *argument = argv[ii];
        if(std::strcmp(argument, "--dump-ir") == 0){
            dump_ir = true;
        }
    }
    Compiler* compilerctx = new Compiler(dump_ir);
    RootAstPtr programPtr = std::make_shared<RootAst>();
    Parser parser(fname);
    parser.start(programPtr);
    DeclarationAnalyzer* declanalyzer = new DeclarationAnalyzer(compilerctx);
    SemanticAnalyzer* sem = new SemanticAnalyzer(compilerctx);
    programPtr->accept(declanalyzer);
    programPtr->accept(sem);
    programPtr->accept(compilerctx);
    delete sem;
    delete declanalyzer;
    delete compilerctx;
    return 1;
}

