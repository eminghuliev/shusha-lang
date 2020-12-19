#ifndef SEMANTIC_ANALYZER
#define SEMANTIC_ANALYZER
#include "../Parser/Compiler.hpp"
class SemanticAnalyzer : public NodeVisitor {
public:
    SemanticAnalyzer(Compiler* context) : compilercontext(context) {}
    virtual void visitFunc(const FnDefPtr& node) override;
    virtual void visitCodeScope(const CodeScopePtr& node) override;
    virtual void visitRoot(const RootAstPtr& node) override;
    virtual void visit(ReturnStatement* node) override;
    virtual void visitVar(const VarDeclPtr& node) override;
    virtual void visit(FnProto* node) override;
    virtual void visit(VarAssign* node) override;
    virtual void visit(FuncCall* node) override;
    virtual void visit(TypeIdentifier* node) override;
    virtual void visit(PrefixOperation* node) override;
    void copy_variable(const VarDeclPtr& node);
    void generate_ptr(TypeNodePtr& typeptr, TypeNodePtr& actual_type);
    void analyze_ptr(const VarDeclPtr& node);
    void handle_prefix_op(const VarDeclPtr& node, 
            const PrefixOperationPtr& prefixop);
    void check_cast_decl(const TypePtr& expected_type,
        const TypePtr& actual_type,
        const ExpressionPtr& expr); 
    ~SemanticAnalyzer() { }
    FnDefPtr fndef;
    Compiler* compilercontext;
    RootAstPtr rootast;
};
#endif
