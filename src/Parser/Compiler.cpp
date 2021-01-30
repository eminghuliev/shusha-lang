#include "../Ast/RootAst.hpp"
#include "../Ast/FnDef.hpp"
#include "Compiler.hpp"
#include <memory>
#include <llvm/IR/Module.h>
Compiler::Compiler(bool dumpir) : dump_ir(dumpir){
    this->init_builtin_types();
    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllAsmParsers();
    LLVMInitializeAllAsmPrinters();
    builder = LLVMCreateBuilder();
    module = LLVMModuleCreateWithName("shusha");
    std::string cpu_arch = LLVMGetHostCPUName();
    char *features = GetNativeFeatures();
    char *triple = LLVMGetDefaultTargetTriple(); 
    LLVMTargetRef targetRef;
    char *error_msg = nullptr;
    if(LLVMGetTargetFromTriple(triple, &targetRef, &error_msg)){
        printf("%s\n", error_msg);
    }
    targetMachine = LLVMCreateTargetMachine(targetRef, 
            triple,
            cpu_arch.c_str(), 
            LLVMGetHostCPUFeatures(), 
            LLVMCodeGenLevelNone, 
            LLVMRelocPIC,
            LLVMCodeModelDefault);
    targetDataRef = LLVMCreateTargetDataLayout(targetMachine);
}

Compiler::~Compiler(){
    if(dump_ir){
        LLVMDumpModule(module);
    }

    char *error_msg = nullptr;
    char* fname = strdup("output.o");
    if (LLVMTargetMachineEmitToFile(targetMachine, module, fname,
                LLVMObjectFile, &error_msg))
    {
        shusha_panic("unable to write object file: %s", error_msg);
    }
    LLVMDisposeModule(module);
    LLVMDisposeBuilder(builder);
    module = nullptr;
}

typedef std::shared_ptr<TypeNodeInteger> TypeNodeIntegerPtr;
typedef std::shared_ptr<TypeNodeString> TypeNodeStringPtr;
typedef std::shared_ptr<TypeNodeBool> TypeNodeBoolPtr;
typedef std::shared_ptr<TypeNodeVoid> TypeNodeVoidPtr;
typedef std::shared_ptr<TypeNodeNumberLt> TypeNodeNumberLtPtr;

void Compiler::init_builtin_types() {
    TypeNodeNumberLtPtr number = create<TypeNodeNumberLt>();
    number->type_id = TypeNodeTableNumLit;
    number->name = "(number)";
    builtin_types.insert(std::make_pair("integer", number));

    {
        TypeNodeIntegerPtr integer = create<TypeNodeInteger>();
        integer->type_id = TypeNodeTableIdInt;
        integer->kind = I8;
        integer->bit_count = 8;
        integer->llvmtype = LLVMIntType(8);
        integer->is_signed = 1;
        type_table.insert(std::make_pair("i8", integer));
    }
    
    {
        TypeNodeIntegerPtr integer = create<TypeNodeInteger>();
        integer->type_id = TypeNodeTableIdInt;
        integer->kind = I16;
        integer->bit_count = 16;
        integer->llvmtype = LLVMIntType(16);
        integer->is_signed = 1;
        type_table.insert(std::make_pair("i16", integer));
    }

    {
        TypeNodeIntegerPtr integer = create<TypeNodeInteger>();
        integer->type_id = TypeNodeTableIdInt;
        integer->kind = I32;
        integer->bit_count = 32;
        integer->llvmtype = LLVMIntType(32);
        integer->is_signed = 1;
        type_table.insert(std::make_pair("i32", integer));
    }

    {
        TypeNodeIntegerPtr integer = create<TypeNodeInteger>();
        integer->type_id = TypeNodeTableIdInt;
        integer->kind = U8;
        integer->bit_count = 8;
        integer->llvmtype = LLVMIntType(8);
        integer->is_signed = 0;
        type_table.insert(std::make_pair("u8", integer));
    }

    {
        TypeNodeIntegerPtr integer = create<TypeNodeInteger>();
        integer->type_id = TypeNodeTableIdInt;
        integer->kind = U16;
        integer->bit_count = 16;
        integer->llvmtype = LLVMIntType(16);
        integer->is_signed = 0;
        type_table.insert(std::make_pair("u16", integer));
    }
    
    {
        TypeNodeIntegerPtr integer = create<TypeNodeInteger>();
        integer->type_id = TypeNodeTableIdInt;
        integer->bit_count = 32;
        integer->llvmtype = LLVMIntType(32);
        integer->is_signed = 0;
        type_table.insert(std::make_pair("u32", integer));
    }

    TypeNodeBoolPtr bool_type = create<TypeNodeBool>();
    bool_type->type_id = TypeNodeTableBool;
    bool_type->kind = BOOL;
    bool_type->llvmtype = LLVMInt1Type();
    type_table.insert(std::make_pair("bool", bool_type));
    
    TypeNodeStringPtr str_type = create<TypeNodeString>();
    str_type->type_id = TypeNodeTableIdString;
    str_type->kind = STR;
    str_type->name = "(string)";
    str_type->llvmtype = LLVMPointerType(LLVMInt8Type(), 0);
    type_table.insert(std::make_pair("str", str_type));

    TypeNodeVoidPtr void_type = create<TypeNodeVoid>();
    void_type->type_id = TypeNodeTableBool;
    void_type->kind = VOID;
    void_type->llvmtype = LLVMVoidType();
    type_table.insert(std::make_pair("void", void_type));

}
void Compiler::visit(VarAssign* node) {
    if(node->var_decl->getType()->typeptr->type_id == TypeNodeTablePointer) {
        LLVMValueRef ref_val = LLVMBuildLoad(builder, node->var_decl->val_ref, "");
        if(node->getExpr()->nodetype != TypeNodeTableSymbol){
            node->getExpr()->var_decl = node->var_decl;
        }
        node->getExpr()->accept(this);
        LLVMBuildStore(builder, node->getExpr()->ret_ref, ref_val);
    }
    else {
        if(node->getExpr()->nodetype != TypeNodeTableSymbol){
            node->getExpr()->var_decl = node->var_decl;
        }
        node->getExpr()->accept(this);
        LLVMBuildStore(builder, node->getExpr()->ret_ref, node->var_decl->val_ref);
    }
}

void Compiler::store_assign(const VarDeclPtr& node) {
    node->val_ref = LLVMBuildAlloca(builder, node->getType()->typeptr->llvmtype, 
            node->getName().c_str());
    LLVMBuildStore(builder, node->getExpr()->ret_ref, 
            node->val_ref);
}

void Compiler::visit(PrefixOperation* node) {
    if(node->getOperation() == PrefixOperationGetPointer && 
            node->getopexpr()->nodetype == TypeNodeTableSymbol) {
        node->ret_ref = node->getopexpr()->getVarDecl()->val_ref;
        node->var_decl->val_ref = node->getopexpr()->getVarDecl()->val_ref;
    }
}

void Compiler::visit(Symbol* node) {
    TypeNodePtr type = node->var_decl->getType()->typeptr;
    if(type->type_id == TypeNodeTableIdString){
        LLVMValueRef zeroIndex = LLVMConstInt( LLVMInt64Type(), 0, true );
        LLVMValueRef indexes[2] = { zeroIndex, zeroIndex };
        LLVMValueRef paramval = LLVMBuildLoad(builder, node->var_decl->val_ref, "");
        node->ret_ref = 
            LLVMBuildInBoundsGEP(builder, paramval, indexes, 2, "");
    }
    else if (type->type_id == TypeNodeTableIdInt){
        node->ret_ref = 
            LLVMBuildLoad(builder, node->var_decl->val_ref, "");
    }
    else if(type->type_id == TypeNodeTablePointer){
         node->ret_ref = LLVMBuildLoad(builder, node->var_decl->val_ref, "");
    }
}

void Compiler::visit(IntegerLiteral* node) {
    TypeNodePtr type = node->var_decl->getType()->typeptr;
    if(type->type_id == TypeNodeTablePointer){
        type = node->var_decl->getType()->typeptr->ptr->child_type;
    }
    LLVMValueRef value = LLVMConstInt(type->llvmtype, node->val, false);
    node->ret_ref = value;
}

void Compiler::visit(StringLiteral* node) {
    StringLiteral* str_literal = node;
    node->var_decl->getType()->typeptr->llvmtype = LLVMPointerType(LLVMArrayType(LLVMInt8Type(), str_literal->value.length() + 1), 0);
    LLVMValueRef strconst = LLVMConstString(str_literal->value.c_str(), str_literal->value.length(), false);
    LLVMValueRef glob = LLVMAddGlobal(module, LLVMTypeOf(strconst), "");
    LLVMSetInitializer(glob, strconst);
    LLVMSetLinkage(glob, LLVMPrivateLinkage);
    LLVMSetGlobalConstant(glob, true);
    LLVMSetUnnamedAddr(glob, true);
    node->ret_ref = glob;
}

void Compiler::visitVar(const VarDeclPtr& node) {
    LLVMValueRef vardecl_res;
    auto VarExpr = node->getExpr();
    if(!VarExpr) {
        node->val_ref = LLVMBuildAlloca(builder, node->getType()->typeptr->llvmtype, 
            node->getName().c_str());
        if(node->const_val_ref)
            LLVMBuildStore(builder, node->const_val_ref, node->val_ref);
        return;
    }
    auto varexpr_type = node->getExpr()->typenode;
    auto var_type = node->getType();
    if(node->getExpr()->nodetype != TypeNodeTableSymbol) {
        node->getExpr()->var_decl = node;
    } 
    node->getExpr()->accept(this);
    store_assign(node);
}

void Compiler::visit(AsmExpr* node) {
    int inout_count = node->inputs.size() + node->outputs.size();
    int total_count = node->inputs.size() + 
                      node->outputs.size() + 
                      node->clobbers.size();
    /// TODO(eminus) do that with dynamic memory allocation 
    std::string const_buffer;
    int index = 0;
    int param_index = 0;
    LLVMValueRef* param_values = 
        static_cast<LLVMValueRef*>(calloc(inout_count, sizeof(LLVMValueRef)));
    LLVMTypeRef* param_types = 
        static_cast<LLVMTypeRef*>(calloc(inout_count, sizeof(LLVMTypeRef)));
    for(auto& nodes: node->inputs) {
        param_types[param_index] = 
            nodes->exprptr->typenode->typeptr->llvmtype;
        param_values[param_index] = genExpr(nodes->exprptr, nodes->exprptr->typenode->typeptr);
        const_buffer.append(string_format("{%s}", nodes->constraint.c_str()));
        param_index+=1;
        index+=1;
        if(index < total_count){
            const_buffer.append(",");
        }
    }
    for(auto& nodes: node->outputs) {
        param_types[param_index] = 
            nodes->exprptr->typenode->typeptr->llvmtype;
        param_values[param_index] = genExpr(nodes->exprptr, nodes->exprptr->getVarDecl()->getType()->typeptr);;
        param_index+=1;
        const_buffer.append(string_format("=%s", nodes->constraint.c_str()));
        index+=1;
        if(index < total_count){
            const_buffer.append(",");
        }
    }

    for(auto& nodes: node->clobbers) {
        const_buffer.append(string_format("~{%s}", nodes->clobname.c_str()));
        index+=1;
        if(index < total_count){
            const_buffer.append(",");
        }
    }
    
    LLVMTypeRef ret_type = LLVMVoidType();
    LLVMTypeRef function_type = LLVMFunctionType(ret_type, param_types, inout_count, false);
    LLVMValueRef fn = LLVMConstInlineAsm(function_type, node->asmtemplate.c_str(), const_buffer.c_str(), 0, 0);
    LLVMBuildCall(builder, fn, param_values, inout_count, "");
    free(param_values);
    free(param_types);
}

void Compiler::visit(ReturnStatement* node) {
}

LLVMValueRef Compiler::genExpr(const ExpressionPtr& expr, const TypePtr& type) {
    TypeNodeTable type_id = type->type_id;
    if(dynamic_pointer_cast<IntegerLiteral>(expr)) {
        IntegerLiteralPtr literal = std::static_pointer_cast<IntegerLiteral>(expr);
        LLVMValueRef value = LLVMConstInt(type->llvmtype, literal->val, false);
        return value;
    }
    else if(dynamic_pointer_cast<StringLiteral>(expr)) {
        StringLiteralPtr str_literal = std::static_pointer_cast<StringLiteral>(expr);
        type->llvmtype = LLVMPointerType(LLVMArrayType(LLVMInt8Type(), str_literal->value.length() + 1), 0);
        LLVMValueRef strconst = LLVMConstString(str_literal->value.c_str(), str_literal->value.length(), false);
        LLVMValueRef glob = LLVMAddGlobal(module, LLVMTypeOf(strconst), "");
        LLVMSetInitializer(glob, strconst);
        LLVMSetLinkage(glob, LLVMPrivateLinkage);
        LLVMSetGlobalConstant(glob, true);
        LLVMSetUnnamedAddr(glob, true);
        return glob;
    }
    else if(dynamic_pointer_cast<Symbol>(expr)) {
        if(type_id == TypeNodeTableIdString){
            LLVMValueRef zeroIndex = LLVMConstInt( LLVMInt64Type(), 0, true );
            LLVMValueRef indexes[2] = { zeroIndex, zeroIndex };
            LLVMValueRef paramval = LLVMBuildLoad(builder, expr->var_decl->val_ref, "");
            return LLVMBuildInBoundsGEP(builder, paramval, indexes, 2, "");
        }
        else if (type_id == TypeNodeTableIdInt){
            return LLVMBuildLoad(builder, expr->var_decl->val_ref, "");
        }
    }
    return nullptr;
}

void Compiler::visit(FuncCall* node) {
    LLVMValueRef* param_values = nullptr;
    FnDefPtr fndef = dynamic_pointer_cast<FnDef>(node->getFnExpr()->getFnDef());
    int arg_cnt = 0;
    if(node->getparams()){
        for(auto &nodes: *node->getparams()) {
            param_values = 
                static_cast<LLVMValueRef*>(calloc(node->getparams()->size(), sizeof(LLVMValueRef)));
            LLVMValueRef retval = genExpr(nodes, 
                    fndef->fnproto->getParamList()->at(arg_cnt)->getArgType()->typeptr);   
            param_values[arg_cnt] = retval;
            arg_cnt+=1;
        }
    }
    LLVMBuildCall(builder, 
            fndef->LLVMFunc, 
            param_values, 
            arg_cnt, 
            "");
}

const VarDeclPtr Compiler::getVar(const FnDefPtr& fndesc, 
                                  const std::string& name) {
    std::unordered_map<std::string, VarDeclPtr>::iterator
        iterator = fndesc->var_table.find(name);
    if(iterator != fndesc->var_table.end()){
        return iterator->second;
    }
    return nullptr;
}

void Compiler::visitFunc(const FnDefPtr& node) {
    fndef = node;
    node->fnproto->accept(this);
    node->LLVMFunc = 
        LLVMAddFunction(module, node->getFnName().c_str(), fndef->rawTypeRef);
    if(node->fnproto->getParamList()) {
        int arg_cnt = 0;
        for(auto& nodes: *node->fnproto->getParamList()) {
            auto vardecl = getVar(node, nodes->getArgName());
            vardecl->const_val_ref = LLVMGetParam(node->LLVMFunc, arg_cnt);
            arg_cnt += 1;
        }
    }
    LLVMBasicBlockRef InitBasicBlock = 
        LLVMAppendBasicBlock(node->LLVMFunc, "init");
    LLVMPositionBuilderAtEnd(builder, InitBasicBlock);
    node->getbody()->accept(this);
    LLVMPositionBuilderAtEnd(builder, InitBasicBlock);
    if(!fndef->have_return) {
        LLVMBuildRetVoid(builder);
    }
}

void Compiler::visitCodeScope(const CodeScopePtr& node) {
    for(ASTNodePtr& nodes: *node){
        Node* nodeptr = nodes.get();
        if(nodeptr) nodeptr->accept(this);
    }
}
void Compiler::visit(FnProto* node) {
    int arg_count = 0;
    LLVMTypeRef* param_types = (LLVMTypeRef*)malloc(node->arg_count);
    LLVMTypeRef ret_type = LLVMVoidType();
    if(node->arg_count > 0){
        for(ParamNodePtr &param: *node->getParamList()) {
            param_types[arg_count] = param->getArgType()->typeptr->llvmtype;
            arg_count+=1;
        }
    }
    if(node->getRetType())
        ret_type = node->getRetType()->typeptr->llvmtype;

    fndef->rawTypeRef = LLVMFunctionType(ret_type,
            param_types, 
            arg_count, 
            0); 
}
void Compiler::visitRoot(const RootAstPtr& node) {
    NodeVisitor::visitRoot(node);
}
