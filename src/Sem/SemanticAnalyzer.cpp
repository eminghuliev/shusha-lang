#include "../Ast/RootAst.hpp"
#include "../Ast/FnDef.hpp"
#include "SemanticAnalyzer.hpp"
#include "../Utils/Error.hpp"
#include <memory>
void SemanticAnalyzer::visitFunc(const FnDefPtr& node) {
    fndef = node;
    node->fnproto->accept(this);
    if(node->fnproto->getRetType())
        node->fnproto->getRetType()->accept(this);
    node->getbody()->accept(this);
}

void SemanticAnalyzer::visit(FnProto* node) {
    int arg_count = 0;
    if(node->getParamList()){
        for(ParamNodePtr &param: *node->getParamList()){
            param->getArgType()->accept(this);
            arg_count+=1;
        }
    }
    node->arg_count = arg_count;
}

typedef std::shared_ptr<TypeNodeInteger> TypeNodeIntegerPtr;
void SemanticAnalyzer::visit(ReturnStatement* node) {
    TypeNodePtr expected_ret_type = fndef->fnproto->getRetType()->typeptr;
    if(!canCastType(node->getExpr()->typenode->typeptr, 
                expected_ret_type, node->getExpr())){
        ::Error("Return statement has invalid type\n");
    }
}
typedef std::shared_ptr<StringLiteral> StringLiteralPtr;
void SemanticAnalyzer::visit(VarAssign* node) {
    StringLiteralPtr strptr = 
        static_pointer_cast<StringLiteral>(node->getsym());
    if(node->getExpr() && strptr) {
        std::unordered_map<std::string, VarDeclPtr>::iterator 
            iterator = fndef->var_table.find(strptr->value);
        if(iterator != fndef->var_table.end()){
            auto expected_val_type = iterator->second->getType()->typeptr;
            auto var_expr = iterator->second->getExpr();
            
            PrefixOperationPtr prefixop = 
                    dynamic_pointer_cast<PrefixOperation>(var_expr);
            
            if((expected_val_type->type_id == TypeNodeTablePointer &&
                    var_expr->typenode->typeptr->type_id == TypeNodeTablePointer && prefixop)) {
                TypeIdentifierPtr vartype = prefixop->getopexpr()->getVarDecl()->getType();
                if(!prefixop->getopexpr()->getVarDecl()->getMut()) 
                    ::Error("Trying to change immutable variable\n");
                if(!canCastType(node->getExpr()->typenode->typeptr, 
                            vartype->typeptr, 
                            node->getExpr()))
                    ::Error(string_format("Assigned value doesn't match %s\n", 
                                vartype->getName().c_str()));
            }else {
                if(!iterator->second->getMut()) 
                    ::Error("Trying to change immutable variable\n");
                if (!canCastType(node->getExpr()->typenode->typeptr, 
                            expected_val_type, 
                            node->getExpr())){
                    ::Error(string_format("Assigned value doesn't match to %s\n", 
                                iterator->second->getType()->getName().c_str()));
                }
            }
            if(node->getExpr()->nodetype == TypeNodeTableIdString) {
                StringLiteralPtr old_str_slice =
                    std::static_pointer_cast<StringLiteral>(iterator->second->getExpr());
                StringLiteralPtr new_str_slice = 
                    std::static_pointer_cast<StringLiteral>(node->getExpr());
                if(old_str_slice->value.length() != new_str_slice->value.length()){
                    ::Error("Old slice length doesn't match with new length\n");
                }
            }
            node->var_decl = iterator->second;
        }
        else ::Error("Undefined identifier for variable assignment\n");
    }
}
/// Type checking for Identifiers
void SemanticAnalyzer::visit(TypeIdentifier* node){
    /// checking for builtin types
    unordered_map<std::string, TypeNodePtr>::iterator 
        iterator = compilercontext->type_table.find(node->getName());
    if(iterator != compilercontext->type_table.end()){
        node->typeptr = iterator->second;
        return;
    }
    ::Error("Undefined identifier for type\n");
}
void SemanticAnalyzer::visit(FuncCall* node) {
    if(node->getparams()){
        FnDefPtr fndef = dynamic_pointer_cast<FnDef>(node->getFnExpr()->getFnDef());
        if(!node->getparams() && fndef->fnproto->arg_count >= 1){
            ::Error(string_format("Expected %d arguments found 0\n", 
                        fndef->fnproto->arg_count));
        }
        else if(fndef->fnproto->arg_count != node->getparams()->size()){
            ::Error(string_format("Expected %d arguments found %d\n",
                        fndef->fnproto->arg_count,
                        node->getparams()->size()));
        }
        int arg_cnt = 0;
        for(auto &nodes: *node->getparams()) {
            auto expected_val_type = 
                fndef->fnproto->getParamList()->at(arg_cnt)->getArgType()->typeptr;
            if(!canCastType(nodes->typenode->typeptr, expected_val_type, nodes)){
                ::Error("Passed function types don't match with function proto types\n"); 
            }
            arg_cnt+=1;
        }
    }
}

typedef std::shared_ptr<TypeNodePointer> TypeNodePointerPtr;
typedef std::shared_ptr<TypeNode> TypeNodePtr;
void SemanticAnalyzer::generate_ptr(TypeNodePtr& typeptr, TypeNodePtr& actual_type) {
    TypeNodePointerPtr ptr_type = create<TypeNodePointer>();
    TypeNodePtr child_type = typeptr;
    ptr_type->type_id = TypeNodeTablePointer;
    ptr_type->name = "(pointer)";
    ptr_type->llvmtype = LLVMPointerType(actual_type->llvmtype, 0);
    typeptr = ptr_type;
    typeptr->ptr = create<Pointer>();
    typeptr->ptr->child_type = child_type;
}

void SemanticAnalyzer::visit(PrefixOperation* node){
    if(node->getOperation() == PrefixOperationGetPointer){
        generate_ptr(node->typenode->typeptr,
                node->getopexpr()->typenode->typeptr);
    }
}
void SemanticAnalyzer::check_cast_decl(const TypePtr& expected_type,
        const TypePtr& actual_type,
        const ExpressionPtr& expr) {
    auto expected_val_type = expected_type;
    if(!canCastType(actual_type, 
                expected_val_type, expr)){
        ::Error("Declared variable's type doesn't match with value type\n");
    }
}

void SemanticAnalyzer::analyze_ptr(const VarDeclPtr& node) {
    generate_ptr(node->getType()->typeptr, node->getType()->typeptr);
}

void SemanticAnalyzer::handle_prefix_op(const VarDeclPtr& node, const PrefixOperationPtr& prefixop){
    node->getExpr()->accept(this);
    if(prefixop->getOperation() == PrefixOperationGetPointer && 
            prefixop->getopexpr()->typenode->typeptr->type_id == TypeNodeTablePointer && 
            node->getType()->typeptr->type_id == TypeNodeTablePointer){
            check_cast_decl(node->getType()->typeptr->ptr->child_type, 
                    prefixop->getopexpr()->typenode->typeptr->ptr->child_type, 
                    prefixop->getopexpr());
    }
}

void SemanticAnalyzer::visitVar(const VarDeclPtr& node) {
    NodeVisitor::visitVar(node);
    if(node->is_ptr) {
        analyze_ptr(node);
        PrefixOperationPtr prefixop =
            dynamic_pointer_cast<PrefixOperation>(node->getExpr());
        if(prefixop){
            handle_prefix_op(node, prefixop);
            return;
        }
    }
    auto VarExpr = node->getExpr();
    if(VarExpr)
        check_cast_decl(node->getType()->typeptr, node->getExpr()->typenode->typeptr, VarExpr);
}
void SemanticAnalyzer::visitCodeScope(const CodeScopePtr& node){
    NodeVisitor::visitCodeScope(node);
}
void SemanticAnalyzer::visitRoot(const RootAstPtr& node){
    rootast = node;
    NodeVisitor::visitRoot(node);
}
