#ifndef COMPILER_HPP
#define COMPILER_HPP
#include "../llvm-wrapper/Shusha-LLVM.hpp"
#include "../Utils/File.hpp"
#include "../Utils/String.hpp"

class Compiler : public NodeVisitor {
protected:
    bool dump_ir;
public:
    unordered_map<std::string, TypeNodePtr> type_table;
    unordered_map<std::string, TypeNodePtr> builtin_types;
    Compiler(bool dumpir);
    ~Compiler();
    void init_builtin_types();
    virtual void visitFunc(const FnDefPtr& node) override;
    virtual void visitCodeScope(const CodeScopePtr& node) override;
    virtual void visitRoot(const RootAstPtr& node) override;
    virtual void visit(FnProto* node) override;
    virtual void visit(ReturnStatement* node) override;
    virtual void visit(AsmExpr* node) override;
    virtual void visit(StringLiteral *node) override;
    virtual void visit(Symbol* node) override;
    const VarDeclPtr getVar(const FnDefPtr& fndesc, 
                                  const std::string& name);
    virtual void visit(VarAssign* node) override;
    virtual void visitVar(const VarDeclPtr& node) override;
    virtual void visit(FuncCall* node) override;
    virtual void visit(PrefixOperation* node) override;
    virtual void visit(IntegerLiteral* node) override;
    void store_assign(const VarDeclPtr& node);
    LLVMModuleRef module;
    LLVMValueRef genExpr(const ExpressionPtr& expr, const TypePtr& type);
    FnDefPtr fndef;
    LLVMTargetMachineRef targetMachine;
    LLVMTargetDataRef targetDataRef;
    LLVMBuilderRef builder;
};
#endif
