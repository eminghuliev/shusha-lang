#ifndef VARDECL_H
#define VARDECL_H
typedef std::shared_ptr<TypeIdentifier> TypeIdentifierPtr;
typedef std::shared_ptr<Expression> ExpressionPtr;
typedef std::shared_ptr<Pointer> PointerPtr;
class VarDecl : public DeclExpression {
protected:
    bool mut;
    std::string name;
    TypeIdentifierPtr type;
    ExpressionPtr expr;
public:
    void setExpr(const ExpressionPtr& expression);
    virtual void accept(NodeVisitor* visitor);
    void setType(const TypeIdentifierPtr& typeptr);
    void setName(const std::string& varName);
    const TypeIdentifierPtr& getType() { return type; }
    const std::string& getName();
    const ExpressionPtr& getExpr() { return expr; }
    void setmut(bool setmutable) { mut = setmutable; }
    bool getMut() { return mut; }
    LLVMValueRef const_val_ref;
    LLVMValueRef val_ref;
    bool is_ptr;
    bool is_sym_ptr;
};

class PrefixOpAddress  {
public:
    PrefixOpAddress() {}
    ~PrefixOpAddress() {}
};

#endif
