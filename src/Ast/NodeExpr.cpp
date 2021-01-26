#include "RootAst.hpp"
#include "../Front/Lexer.hpp"
#include "NodeExpr.hpp"
#include <memory>
bool check_overflow(uint64_t value, int is_signed, int bit_count){
    if(is_signed){
        if((value & 0x80000000) != 0){ // value is negative
            if(value <= ((uint64_t)INT8_MAX) + 1)
                return bit_count >=8;
            else if(value <= ((uint64_t)INT16_MAX) + 1)
                return bit_count >=16;
            else if(value <= ((uint64_t)INT32_MAX) + 1)
                return bit_count >= 32;
            else
                return bit_count >= 64;
        }
        else if(value <= ((uint64_t)INT8_MAX))
            return bit_count >=8;
        else if(value <= ((uint64_t)INT16_MAX))
            return bit_count >=16;
        else if(value <= ((uint64_t)INT32_MAX))
            return bit_count >= 32;
        else
            return bit_count >= 64;
    } else {
        if((value & 0x80000000) != 0)
            return value == 0;
        else {
            if(value <= UINT8_MAX)
                return bit_count >=8;
            else if(value <= UINT16_MAX)
                return bit_count >=16;
            else if(value <= UINT32_MAX)
                return bit_count >= 32;
            else
                return bit_count >= 64;
        }
    }
}
typedef std::shared_ptr<IntegerLiteral> IntegerLiteralPtr;
typedef std::shared_ptr<TypeNodeInteger> TypeNodeIntegerPtr;
/// checking for implicit casting (doing some type coerceing ...)
bool canCastType(TypePtr actual_type, 
        TypePtr expected_type, 
        const ExpressionPtr& const_expr) {
    if(actual_type == expected_type) {
        return true;
    }
    if(expected_type->type_id == TypeNodeTablePointer && 
            actual_type->type_id == TypeNodeTablePointer){
        return canCastType(actual_type->ptr->child_type, 
                expected_type->ptr->child_type,
                const_expr);
        return false;
    }

    if(expected_type->type_id == TypeNodeTableIdInt && 
            actual_type->type_id == TypeNodeTableIdInt) {
        TypeNodeIntegerPtr actual_val_type = 
            static_pointer_cast<TypeNodeInteger>(actual_type);
        TypeNodeIntegerPtr expected_val_type = 
            static_pointer_cast<TypeNodeInteger>(expected_type);
        if(actual_val_type->is_signed == expected_val_type->is_signed && 
                expected_val_type->bit_count >= actual_val_type->bit_count) {
            return true;
        }
        if(expected_val_type->type_id == TypeNodeTableIdInt && expected_val_type->is_signed &&
                actual_val_type->type_id == TypeNodeTableIdInt && !actual_val_type->is_signed &&
                expected_val_type->bit_count > actual_val_type->bit_count) {
            return true;
        }
    }
    if(actual_type->getTypeNode() == TypeNodeTableIdString 
            && expected_type->getTypeNode() == TypeNodeTableIdString){
        return true;
    }

    if(actual_type->type_id == TypeNodeTableNumLit && 
            expected_type->type_id == TypeNodeTableIdInt) {
        TypeNodeIntegerPtr expected_int_type = 
            static_pointer_cast<TypeNodeInteger>(expected_type);
        IntegerLiteralPtr
            intval = static_pointer_cast<IntegerLiteral>(const_expr);
        if(check_overflow(intval->val, expected_int_type->is_signed, 
            expected_int_type->bit_count)){
            return true;
        }
    }
    return false;
}

void BinaryExpression::accept(NodeVisitor* visitor){
    visitor->visit(this);
}
void ReturnStatement::accept(NodeVisitor* visitor){
    visitor->visit(this);
}
void Symbol::accept(NodeVisitor* visitor) {
    visitor->visit(this);
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void FuncCall::accept(NodeVisitor* visitor){
    visitor->visit(this);
}
void IntegerLiteral::accept(NodeVisitor* visitor){
    visitor->visit(this);
}
void StringLiteral::accept(NodeVisitor* visitor){
    visitor->visit(this);
}
void TypeIdentifier::accept(NodeVisitor *visitor){
    visitor->visit(this);
}
void AsmExpr::accept(NodeVisitor *visitor){
    visitor->visit(this);
}
void VarAssign::accept(NodeVisitor *visitor){
    visitor->visit(this);
}
void PrefixOperation::accept(NodeVisitor *visitor){
    visitor->visit(this);
}
#pragma GCC diagnostic pop
