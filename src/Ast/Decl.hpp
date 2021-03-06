#ifndef DECL_HPP
#define DECL_HPP
#include <memory>
typedef std::shared_ptr<TypeNode> TypeNodePtr;
typedef std::shared_ptr<ParamNode> ParamNodePtr;
typedef std::shared_ptr<ParamListNode> ParamListNodePtr;
typedef std::shared_ptr<FnDef> FnDefPtr;
typedef std::shared_ptr<ASTNode> ASTNodePtr;
typedef std::shared_ptr<RootAst> RootAstPtr;
typedef std::shared_ptr<DeclExpression> DeclExpressionPtr;
typedef std::shared_ptr<VarSymbol> VarSymbolPtr;
typedef std::shared_ptr<VarDecl> VarDeclPtr;
typedef std::shared_ptr<Pointer> PointerPtr;
typedef std::shared_ptr<AlignExpr> AlignExprPtr;
typedef std::shared_ptr<BlockContext> BlockContextPtr;
typedef std::shared_ptr<CodeScope> CodeScopePtr;
typedef std::shared_ptr<Expression> ExpressionPtr;
typedef std::shared_ptr<IntegerLiteral> IntegerLiteralPtr;
typedef std::shared_ptr<StringLiteral> StringLiteralPtr;
typedef std::shared_ptr<FuncCall> FnCallPtr;
typedef std::shared_ptr<BinaryExpression> BinaryExpressionPtr;
typedef std::shared_ptr<Symbol> SymbolPtr;
typedef std::shared_ptr<ReturnStatement> RetStatementPtr;
typedef std::shared_ptr<FnProto> FnProtoPtr;
typedef std::shared_ptr<AsmExpr> AsmExprPtr;
typedef std::shared_ptr<AsmInput> AsmInputPtr;
typedef std::shared_ptr<AsmOutput> AsmOutputPtr;
typedef std::shared_ptr<FuncParamList> FuncParamListPtr;
typedef std::shared_ptr<VarAssign> VarAssignPtr;
typedef std::shared_ptr<PrefixOperation> PrefixOperationPtr;
#endif
