#include "Shusha-LLVM.hpp"
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DiagnosticInfo.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/InitializePasses.h>
#include <llvm/MC/SubtargetFeature.h>
#include <llvm/PassRegistry.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetParser.h>
#include <llvm/Support/Timer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/CodeGenCWrappers.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/AlwaysInliner.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>

using namespace llvm;
char *GetNativeFeatures() {
    SubtargetFeatures features;

    StringMap<bool> host_features;
    if (sys::getHostCPUFeatures(host_features)) {
        for (auto &F : host_features)
            features.AddFeature(F.first(), F.second);
    }

    return strdup(reinterpret_cast<const char *>(StringRef(features.getString()).bytes_begin()));
}


