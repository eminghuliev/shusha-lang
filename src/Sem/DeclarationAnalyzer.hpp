#ifndef DECLARATION_ANALYZER_HPP
#define DECLARATION_ANALYZER_HPP
/// Copy smart pointer based objects
template<class T, class... Args>
shared_ptr<T> make_shared(Args&&... args)
{
  return shared_ptr<T>( new T( std::forward<Args>( args )... ) );
}
typedef unordered_map<std::string, TypeNodePtr> typeTablePtr;
typedef unordered_map<std::string, TypeNodePtr> builtintypePtr;
class DeclarationAnalyzer : public NodeVisitor {
protected:
    Compiler* compilercontext;
public:
    DeclarationAnalyzer(Compiler* context) : compilercontext(context) {}
    RootAstPtr rootast;
    FnDefPtr fndef;
    CodeScopePtr codescope;
    virtual void visitRoot(const RootAstPtr& node) override;
    virtual void visitFunc(const FnDefPtr& node) override;
    virtual void visitCodeScope(const CodeScopePtr& node) override;
    virtual void visitVar(const VarDeclPtr& node) override;
    virtual void visit(StringLiteral* node) override;
    virtual void visit(IntegerLiteral* node) override;
    virtual void visit(TypeIdentifier* node) override;
    virtual void visit(Symbol* node) override;
    virtual void visit(VarAssign* node) override;
    virtual void visit(ReturnStatement* node) override;
    virtual void visit(FuncCall* node) override;
    virtual void visit(AsmExpr* node) override;
    virtual void visit(PrefixOperation* node) override;
    void copy_variable(const VarDeclPtr& node);
    ~DeclarationAnalyzer() { }
};
#endif
