#ifndef ROOTAST_HPP
#define ROOTAST_HPP
#include <memory>
#include <vector>
#include <unordered_map>
#include "../Utils/HashMap.hpp"
#include "../Utils/ObjectAllocator.hpp"
#include "../Utils/Error.hpp"
#include "../llvm-wrapper/Shusha-LLVM.hpp"
class ASTNode;
class FnDef;
class TypeNode;
class ParamNode;
class ParamListNode;
class NodeExpression;
class VarSymbol;
class VarDecl;
class RootAst;
class Node;
class Pointer;
class BinaryExpression;
class BlockContext;
class ReturnStatement;
class CodeScope;
class Symbol;
class TypeIdentifier;
class FnProto;
class Expression;
class IntegerLiteral;
class StringLiteral;
class VarAssign;
class AsmExpr;
class FuncCall;
class PrefixOperation;
typedef std::shared_ptr<VarDecl> VarDeclPtr;
typedef std::shared_ptr<ASTNode> ASTNodePtr;
typedef std::shared_ptr<BlockContext> BlockContextPtr;
typedef std::shared_ptr<TypeIdentifier> TypeIdentifierPtr;
typedef std::shared_ptr<FnDef> FnDefPtr;
typedef std::shared_ptr<RootAst> RootAstPtr;
typedef std::shared_ptr<VarAssign> VarAssignPtr;
typedef std::shared_ptr<FuncCall> FnCallPtr;
typedef std::shared_ptr<TypeNode> TypePtr;
typedef std::shared_ptr<CodeScope> CodeScopePtr;
#define DO_ACCEPT(nodes) \
    do { \
        Node* nodeptr = nodes.get(); \
        if(nodeptr) nodeptr->accept(this); \
    } while (0)


enum TypeNodeTable {
    TypeNodeTableIdStruct,
    TypeNodeTableIdInt,
    TypeNodeTableIdString,
    TypeNodeTablePointer,
    TypeNodeTableUnion,
    TypeNodeTableFn,
    TypeNodeTableVoid,
    TypeNodeTableBool,
    TypeNodeTableEnum,
    TypeNodeTableNumLit,
    TypeNodeTableSymbol,
    TypeNodeTableNotFound,
};

enum PrefixOperations {
    PrefixOperationGetPointer,
    PrefixOperationNotOperation,
};

class NodeVisitor {
public:
    virtual ~NodeVisitor() {}
    virtual void visitFunc(const FnDefPtr& node);
    virtual void visit(TypeNode*);
    virtual void visit(ParamNode*);
    virtual void visit(ParamListNode*);
    virtual void visit(Pointer*);
    virtual void visitVar(const VarDeclPtr& node);
    virtual void visitRoot(const RootAstPtr& node);
    virtual void visit(BinaryExpression*);
    virtual void visitCodeScope(const CodeScopePtr& node);
    virtual void visit(ReturnStatement*);
    virtual void visit(Symbol*);
    virtual void visit(TypeIdentifier*);
    virtual void visit(FnProto*);
    virtual void visit(IntegerLiteral*);
    virtual void visit(Expression*);
    virtual void visit(StringLiteral*);
    virtual void visit(AsmExpr*);
    virtual void visit(FuncCall*); 
    virtual void visit(PrefixOperation*);
    virtual void visit(VarAssign*);
};
class Node : public std::enable_shared_from_this<Node> {
public:
    template<class VisitNodes>
    void do_accept(NodeVisitor* visitor,
            void (NodeVisitor::*visit)(const std::shared_ptr<VisitNodes>&))
    {
        std::shared_ptr<VisitNodes> self_ptr = 
            std::static_pointer_cast<VisitNodes>(shared_from_this());
        (visitor->*visit)(self_ptr);
    }
    virtual ~Node() = default;
    virtual void accept(NodeVisitor*) = 0;
};
class ASTNode : public Node {
public:
    ASTNode() {}
};
class RootAst : public Node {
private:
    std::vector<ASTNodePtr> ast;
public:
    RootAst(){}
    virtual void accept(NodeVisitor* visitor) override;
    std::vector<ASTNodePtr>::iterator begin() { return ast.begin();}
    std::vector<ASTNodePtr>::iterator end() { return ast.end();}
    void addAst(const ASTNodePtr& ptr) {ast.push_back(ptr);}
    HashMap<std::string, ASTNodePtr> new_decl_table;
    std::unordered_map<std::string, ASTNodePtr> decl_table;
};

class Expression : public ASTNode {
public:
    Expression() {}
    ~Expression() {}
    const TypeIdentifierPtr& getType() { return typenode; }
    void setType(const TypeIdentifierPtr& typeptr){ typenode = typeptr; }
    void setVarDecl(const VarDeclPtr& decl) { var_decl = decl; }
    const VarDeclPtr& getVarDecl() { return var_decl; }
    const ASTNodePtr& getFnDef() { return fn_def; }
    void setFnDef(const ASTNodePtr& def) { fn_def = def; };
    TypeIdentifierPtr typenode;
    TypeNodeTable nodetype;
    VarDeclPtr var_decl;
    LLVMValueRef val_ref;
    LLVMValueRef ret_ref; // Return from expression
protected:
    ASTNodePtr fn_def;
};

#endif
