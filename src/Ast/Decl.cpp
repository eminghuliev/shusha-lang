#include "../Front/Lexer.hpp"
#include "RootAst.hpp"
#include "NodeExpr.hpp"
#include "VarDecl.hpp"

void VarDecl::accept(NodeVisitor *visitor){
    do_accept(visitor, &NodeVisitor::visitVar);
}
void VarDecl::setExpr(const ExpressionPtr& expression){ 
    expr = expression; 
}
void VarDecl::setType(const TypeIdentifierPtr& typeptr) { 
    type = typeptr; 
}
void VarDecl::setName(const std::string& varName) {
    name = varName; 
}
const std::string& VarDecl::getName(){
    return name;
}
