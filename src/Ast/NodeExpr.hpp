#ifndef NODE_EXPR_HPP
#define NODE_EXPR_HPP
#include <memory>
#include "../llvm-wrapper/Shusha-LLVM.hpp"
/// Declaration expression
class DeclExpression : public ASTNode {
public:
    DeclExpression() {}
    ~DeclExpression() {}
};

typedef std::shared_ptr<TypeNode> TypePtr;
typedef std::shared_ptr<IntegerLiteral> IntegerLiteralPtr;
typedef std::shared_ptr<Expression> ExpressionPtr;
typedef std::shared_ptr<Pointer> PointerPtr;
bool check_overflow(uint64_t value, int is_signed, int bit_count);
bool canCastType(TypePtr actual_type, 
        TypePtr expected_type, 
        const ExpressionPtr& const_expr);
class TypeNode: public Node {
public:
    void accept(NodeVisitor* visitor) { visitor->visit(this); }
    TypeNodeTable getTypeNode() { return type_id; }
    TypeKind getKind() { return kind; }
    ~TypeNode(){}
    TypeKind kind; /// PrimitiveTypes
    TypeNodeTable type_id; /// DerivedTypes
    LLVMTypeRef llvmtype;
    std::string name;
    PointerPtr ptr;
};

typedef std::shared_ptr<TypeNode> TypeNodePtr;
class TypeNodeNumberLt : public TypeNode {
public:
    TypeNodeNumberLt(){}
};

class TypeNodeInteger : public TypeNode {
public:
    TypeNodeInteger(){}
    bool is_signed;
    int bit_count;
};

class TypeNodeString : public TypeNode {
public:
    TypeNodeString(){}
};

class TypeNodePointer : public TypeNode {
public:
    TypeNodePointer(){}
};

class TypeNodeBool : public TypeNode {
public:
    TypeNodeBool() {}
};

class TypeNodeVoid : public TypeNode {
public:
    TypeNodeVoid() {}
};

class CodeScope : public Node {
private:
    std::vector<ASTNodePtr> statements;
public:
    CodeScope() {}
    ~CodeScope() {}
    virtual void accept(NodeVisitor* visitor);
    std::vector<ASTNodePtr>::iterator begin() { return statements.begin();}
    std::vector<ASTNodePtr>::iterator end() { return statements.end();}
    void insert(const ASTNodePtr& stmt) { statements.push_back(stmt); }
};

class AlignExpr {
public:
    AlignExpr(){}
    int getAlign() { return align; }
    void setAlign(const int val) { align = val; }
protected:
    int align;
};

typedef std::shared_ptr<AlignExpr> AlignExprPtr;
class Pointer : public Node {
public:
    void accept(NodeVisitor* visitor) { visitor->visit(this); }
    void setAlignPtr(const AlignExprPtr& ptr) { align = ptr; }
    void setMut(bool state) { is_mut = state; }
    void setVolatile(bool state) { is_volatile = state; }
    const AlignExprPtr& getAlign() { return align; }
    Pointer(){}
    ~Pointer(){}
    TypeNodePtr child_type;
protected:
    bool is_mut;
    bool is_volatile;
    AlignExprPtr align;
};

typedef std::shared_ptr<Expression> ExpressionPtr;
class IntegerLiteral : public Expression {
public:
    IntegerLiteral() {}
    virtual void accept(NodeVisitor* visitor);
    IntegerLiteral(IntegerLiteral& expr) {
        val = expr.val;
        should_type = expr.should_type;
    }
    std::string should_type;
    uint64_t val;
};

class StringLiteral : public Expression {
public:
    StringLiteral() {}
    virtual void accept(NodeVisitor* visitor);
    StringLiteral(StringLiteral& expr) {
        value = expr.value;
        should_type = expr.should_type;
    }
    std::string value;
    std::string should_type;
};

typedef std::shared_ptr<Expression> ExpressionPtr;

class FuncParamList {
public:
    void insert(const ExpressionPtr& expr) { param_list.push_back(expr); }
    std::vector<ExpressionPtr> param_list;
    std::vector<ExpressionPtr>::iterator begin() { return param_list.begin();}
    std::vector<ExpressionPtr>::iterator end() { return param_list.end();}
    int size() { return param_list.size(); }
};
typedef std::shared_ptr<FuncParamList> FuncParamListPtr;

class FuncCall : public Expression {
protected:
ExpressionPtr fnexpr;
FuncParamListPtr params;
public:
    void setFnExpr(const ExpressionPtr& fn_expr) {
        fnexpr = fn_expr; 
    }
    void setParams(const FuncParamListPtr& params_expr) {
        params = params_expr; 
    }
    const ExpressionPtr& getFnExpr(){
        return fnexpr;
    }
    const FuncParamListPtr& getparams(){
        return params;
    }
    virtual void accept(NodeVisitor* visitor);
    FuncCall() {}
};

class PrefixOperation : public Expression {
protected:
    PrefixOperations op;
    ExpressionPtr opexpr;
public:
    PrefixOperation() {}
    ~PrefixOperation() {}
    virtual void accept(NodeVisitor* visitor);
    void setOperation(const ExpressionPtr& expr) { opexpr = expr;}
    void setType(PrefixOperations optype) { op = optype;}
    PrefixOperations getOperation() { return op; }
    const ExpressionPtr& getopexpr(){ return opexpr;}
};

class BinaryExpression : public Expression {
protected:
    ExpressionPtr lhs;
    ExpressionPtr rhs;
    BinOpType typekind;
public:
    BinaryExpression() {}
    ~BinaryExpression() {}
    virtual void accept(NodeVisitor* visitor);
    void setBinOpType(BinOpType kind) { typekind = kind; }
    void setlhs(const ExpressionPtr& lhsval) { lhs = lhsval; }
    void setrhs(const ExpressionPtr& rhsval) { rhs = rhsval; }
    ExpressionPtr left() { return lhs; }
    ExpressionPtr right() { return rhs; }
};

class Symbol : public Expression {
protected:
    std::string name;
public:
    Symbol() {}
    void setsym(const std::string& sym) {name = sym;}
    const std::string& getsym() { return name; }
    virtual void accept(NodeVisitor* visitor);
};

class ReturnStatement : public ASTNode {
protected:
    ExpressionPtr ret;
public:
    ReturnStatement(){}
    ~ReturnStatement() {}
    virtual void accept(NodeVisitor* visitor) override;
    void setRet(const ExpressionPtr& retstatement){ ret = retstatement; }
    const ExpressionPtr& getExpr() { return ret; }
};

class TypeIdentifier {
protected:
    std::string name;
public:
    const std::string& getName() { return name; }
    void setName(const std::string& typeName) { name = typeName; }
    TypeIdentifier(){}
    virtual void accept(NodeVisitor* visitor);
    TypeNodePtr typeptr;
};

class AsmInput  {
public:
    AsmInput() {}
    ~AsmInput() {}
    ExpressionPtr exprptr;
    std::string constraint;
};

class AsmOutput  {
public:
    AsmOutput() {}
    ~AsmOutput() {}
    ExpressionPtr exprptr;
    std::string constraint;
};

class AsmClob {
public:
    AsmClob(){}
    ~AsmClob(){}
    std::string clobname;
};

typedef std::shared_ptr<AsmInput> AsmInputPtr;
typedef std::shared_ptr<AsmOutput> AsmOutputPtr;
typedef std::shared_ptr<AsmClob> AsmClobPtr;
class AsmExpr : public ASTNode {
public:
    AsmExpr() {}
    ~AsmExpr() {}
    void insertin(const AsmInputPtr& asminput) { inputs.push_back(asminput); }
    virtual void accept(NodeVisitor* visitor) override;
    void insertclob(const AsmClobPtr& asmclob) { clobbers.push_back(asmclob); }
    void insertout(const AsmOutputPtr& asmoutput) { outputs.push_back(asmoutput); }
    std::string asmtemplate;
    std::vector<AsmInputPtr> inputs;
    std::vector<AsmOutputPtr> outputs;
    std::vector<AsmClobPtr> clobbers;
};

class VarAssign : public Expression {
protected:
    ExpressionPtr name;
    ExpressionPtr exprptr;
public:
    VarAssign() {}
    ~VarAssign() {}
    void setname(const ExpressionPtr& exprname) { name = exprname; }
    const ExpressionPtr& getsym() { return name; }
    void setexpr(const ExpressionPtr& expr) { exprptr = expr; }
    const ExpressionPtr& getExpr() { return exprptr; } 
    virtual void accept(NodeVisitor* visitor);

    
};
#endif
