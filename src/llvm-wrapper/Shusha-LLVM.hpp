#ifndef SHUSHA_LLVM_HPP
#define SHUSHA_LLVM_HPP
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Initialization.h>
#include <llvm-c/TargetMachine.h>

char *GetNativeFeatures();
#endif
