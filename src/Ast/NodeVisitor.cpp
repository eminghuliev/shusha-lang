#include "RootAst.hpp"
#include "../Front/Lexer.hpp"
#include "NodeExpr.hpp"
#include "VarDecl.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void NodeVisitor::visitVar(const VarDeclPtr& node) {
    node->getType()->accept(this);
}
void NodeVisitor::visit(Pointer* node){
}
void NodeVisitor::visitCodeScope(const CodeScopePtr& node){
    for(ASTNodePtr& nodes: *node){
        DO_ACCEPT(nodes);
    }
}
void NodeVisitor::visit(FnProto* node) { }

void NodeVisitor::visit(TypeIdentifier* node){ }

void NodeVisitor::visit(ParamListNode* node){ }

void NodeVisitor::visit(ParamNode* node){ }

void NodeVisitor::visit(TypeNode* node){ }

void NodeVisitor::visitFunc(const FnDefPtr& node){ }

void NodeVisitor::visit(BinaryExpression * node){ }

void NodeVisitor::visit(ReturnStatement * node){ }

void NodeVisitor::visit(IntegerLiteral* node){ }

void NodeVisitor::visit(Symbol* node) { }

void NodeVisitor::visit(Expression* node) { }

void NodeVisitor::visit(StringLiteral* node) { }

void NodeVisitor::visit(AsmExpr* node) { }

void NodeVisitor::visit(VarAssign* node) { }

void NodeVisitor::visit(PrefixOperation* node) {}

void NodeVisitor::visitRoot(const RootAstPtr& node){
    /// Forward to Semantic analyser
    for(ASTNodePtr nodes: *node) {
        Node* nodeptr = nodes.get();
        if(nodeptr) nodeptr->accept(this);
    }
}

void NodeVisitor::visit(FuncCall* node){ }

void RootAst::accept(NodeVisitor* visitor){
    //visitor->visit(this);
    do_accept(visitor, &NodeVisitor::visitRoot);
}
void CodeScope::accept(NodeVisitor* visitor){
    do_accept(visitor, &NodeVisitor::visitCodeScope);
}
#pragma GCC diagnostic pop

