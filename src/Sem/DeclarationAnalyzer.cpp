#include "../Ast/RootAst.hpp"
#include "../Ast/FnDef.hpp"
#include <memory>
#include "../CodeGen/Compiler.hpp"
#include "DeclarationAnalyzer.hpp"
#include "../Utils/Error.hpp"
typedef std::shared_ptr<TypeNodeString> TypeNodeStringPtr;
typedef std::shared_ptr<TypeNodeInteger> TypeNodeIntegerPtr;
void DeclarationAnalyzer::visitFunc(const FnDefPtr& node) {
    /// Update Function AST Node for each fn scope
    fndef = node;
    HashMap<std::string, ASTNodePtr>::Iterator it = rootast->new_decl_table.find(node->getFnName());
    if(it != rootast->new_decl_table.end()) {
        if(dynamic_cast<FnDef*>(it->val.get()))
            ::Error("Error: Redefinition of function");
    }
    else {
        rootast->new_decl_table.insert(node->getFnName(), node);
    }
    /// calling for CodeScope visitor
    node->getbody()->accept(this);
}
void DeclarationAnalyzer::visitCodeScope(const CodeScopePtr& node) {
    codescope = node;
    NodeVisitor::visitCodeScope(node);
}
void DeclarationAnalyzer::visit(StringLiteral* node) {
    node->typenode = create<TypeIdentifier>();
    node->typenode->typeptr = compilercontext->type_table.at("str");
    node->nodetype = TypeNodeTableIdString;
}
void DeclarationAnalyzer::visit(TypeIdentifier* node) {
    /// checking for primitive types
    unordered_map<std::string, TypeNodePtr>::iterator 
        iterator = compilercontext->type_table.find(node->getName());
    if(iterator != compilercontext->type_table.end()){
        node->typeptr = iterator->second;
        return;
    }
    ::Error("Undefined identifier for type\n");
}
void DeclarationAnalyzer::visit(Symbol* node) {
    /// Search for Variable Declarations
    node->typenode = create<TypeIdentifier>();
    std::unordered_map<std::string, VarDeclPtr>::iterator 
        iterator_var = fndef->var_table.find(node->getsym());
    /// Declaration Table
    std::unordered_map<std::string, ASTNodePtr>::iterator 
        iterator_decl = rootast->decl_table.find(node->getsym());
    if(iterator_var != fndef->var_table.end()) {
        node->setType(iterator_var->second->getType());
        node->setVarDecl(iterator_var->second);
    }
    else if(iterator_decl != rootast->decl_table.end()) {
        node->setFnDef(iterator_decl->second); 
    }
    else ::Error(
            string_format("Undefined identifier for symbol: %s\n", 
                node->getsym().c_str()));
    /// TODO(eminus) add decl_table for finding other declarations 
    node->nodetype= TypeNodeTableSymbol;
}

void DeclarationAnalyzer::visit(PrefixOperation* node) {
    node->typenode = create<TypeIdentifier>();
    node->getopexpr()->accept(this);
}

void DeclarationAnalyzer::visit(AsmExpr* node) {
    for(auto &nodes: node->inputs){
        nodes->exprptr->accept(this);
        if(nodes->exprptr->nodetype == TypeNodeTableNumLit)
            ::Error("Expected typed integer, found numeric literal");
    }
    
    for(auto &nodes: node->outputs){
        nodes->exprptr->accept(this);
        if(nodes->exprptr->nodetype == TypeNodeTableNumLit)
            ::Error("Expected typed integer, found numeric literal");
    }
}

void DeclarationAnalyzer::visit(IntegerLiteral* node) {
    node->typenode = create<TypeIdentifier>();
    node->typenode->typeptr = compilercontext->builtin_types.at("integer");
    node->nodetype = TypeNodeTableNumLit;
}

void DeclarationAnalyzer::visit(ReturnStatement *node) {
    node->getExpr()->accept(this);
}
void DeclarationAnalyzer::visit(FuncCall* node) {
    node->getFnExpr()->accept(this);
    if(node->getparams()){
        FnDefPtr fndef = dynamic_pointer_cast<FnDef>(node->getFnExpr()->getFnDef());
        int arg_cnt = 0;
        for(auto& nodeparam: *node->getparams()) {
            nodeparam->accept(this);
        }
    }
}
void DeclarationAnalyzer::visit(VarAssign* node) {
    if(node->getExpr())
        node->getExpr()->accept(this);
}
void DeclarationAnalyzer::visitVar(const VarDeclPtr& node) {
    std::unordered_map<std::string, VarDeclPtr>::iterator
        iterator = fndef->var_table.find(node->getName());
    if(iterator != fndef->var_table.end()){
        ::Error("Error: Redefinition of variable\n"); 
    }
    else {

        unordered_map<std::string, TypeNodePtr>::iterator 
            iterator = compilercontext->type_table.find(node->getName());
        if(iterator != compilercontext->type_table.end()){
            ::Error("Trying to shadow type to value\n");
        }
        fndef->var_table.insert(std::make_pair(node->getName(), node));
    }
    NodeVisitor::visitVar(node);
    if(node->is_ptr){
        node->getType()->typeptr->ptr = create<Pointer>();
    }
    if(node->getExpr()){
        node->getExpr()->accept(this);
    }   
}
void DeclarationAnalyzer::visitRoot(const RootAstPtr& node){
    rootast = node;
    NodeVisitor::visitRoot(node);
}
