#ifndef FNDEF_HPP
#define FNDEF_HPP
#include <vector>
#include "../Front/Lexer.hpp"
#include "NodeExpr.hpp"
#include "Decl.hpp"
#include "VarDecl.hpp"
#include <memory>
class ASTNode;
class ParamNode : public Node {
public:
    void accept(NodeVisitor* visitor) { visitor->visit(this); }
    ~ParamNode(){}
    void setArgType(const TypeIdentifierPtr& type) { argType = type; }
    void setArgName(const std::string& name) { argName = name; }
    const std::string& getArgName() { return argName; }
    TypeIdentifierPtr& getArgType() { return argType; }
protected:
    TypeIdentifierPtr argType;
    std::string argName; // Argument name
};

class ParamListNode : public Node {
public:
    void accept(NodeVisitor* visitor) { visitor->visit(this); }
    void addParam(const ParamNodePtr& ptr) { params.push_back(ptr); }
    std::vector<ParamNodePtr>::iterator begin() { return params.begin();}
    std::vector<ParamNodePtr>::iterator end() { return params.end();}
    const ParamNodePtr& at(int index) { return params.at(index);}
    ~ParamListNode(){}
private:
    std::vector<ParamNodePtr> params;
};

class FnProto : public Node {
protected:
    TypeIdentifierPtr ret_type;
    ParamListNodePtr paramlist;
public:
    void accept(NodeVisitor* visitor) { visitor->visit(this); } 
    TypeIdentifierPtr getRetType() const { return ret_type; };
    void setRetType(const TypeIdentifierPtr& typeptr) { ret_type = typeptr; }
    void setParamList(const ParamListNodePtr& plist) { paramlist = plist; }
    const ParamListNodePtr& getParamList() { return paramlist; }
    int arg_count = 0;
};

class FnDef : public ASTNode {
protected:
    std::string fnName;
    CodeScopePtr body;
public: 
    FnDef() {}
    const std::string& getFnName() { return fnName; }
    void setFnName(const std::string& name) { fnName = name; }
    void accept(NodeVisitor* visitor) {
        do_accept(visitor, &NodeVisitor::visitFunc);
    }
    void setbody(const CodeScopePtr& scope){ body = scope; }
    CodeScopePtr getbody() { return body; }
    ~FnDef(){}
    std::unordered_map<std::string, ASTNode*> decl_table;
    std::unordered_map<std::string, VarDeclPtr> var_table;
    FnProtoPtr fnproto;
    bool isextern = false;
    LLVMTypeRef rawTypeRef;
    LLVMValueRef LLVMFunc;
    bool have_return = false;
};
#endif
