/*
 *  JIT compile ClamAV bytecode.
 *
 *  Copyright (C) 2009-2010 Sourcefire, Inc.
 *
 *  Authors: Török Edvin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */
#define DEBUG_TYPE "clamavjit"
#include <pthread.h>
#ifndef _WIN32
#include <sys/time.h>
#endif
#include "ClamBCModule.h"
#include "ClamBCDiagnostics.h"
#include "llvm/Analysis/DebugInfo.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Triple.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/CallingConv.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Function.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/JITEventListener.h"
#include "llvm/LLVMContext.h"
#include "llvm/Intrinsics.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/System/DataTypes.h"
#include "llvm/System/Host.h"
#include "llvm/System/Memory.h"
#include "llvm/System/Mutex.h"
#include "llvm/System/Signals.h"
#include "llvm/System/Threading.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/TargetFolder.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/System/ThreadLocal.h"
#include <cstdlib>
#include <csetjmp>
#include <new>
#include <cerrno>
#include <string>

#include "llvm/Config/config.h"
#if !ENABLE_THREADS
#error "Thread support was explicitly disabled. Cannot continue"
#endif

#ifdef _GLIBCXX_PARALLEL
#error "libstdc++ parallel mode is not supported for ClamAV. Please remove -D_GLIBCXX_PARALLEL from CXXFLAGS!"
#endif

#ifdef HAVE_CONFIG_H
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef PACKAGE_URL
#include "clamav-config.h"
#endif
#include "dconf.h"
#include "clamav.h"
#include "clambc.h"
#include "bytecode.h"
#include "bytecode_priv.h"
#include "type_desc.h"
extern "C" {
#include "md5.h"
}

#ifdef _WIN32
#include <winsock2.h>
#include <cw_inline.h>
#endif

#define MODULE "libclamav JIT: "

extern "C" unsigned int cli_rndnum(unsigned int max);
using namespace llvm;
typedef DenseMap<const struct cli_bc_func*, void*> FunctionMapTy;
struct cli_bcengine {
    ExecutionEngine *EE;
    JITEventListener *Listener;
    LLVMContext Context;
    FunctionMapTy compiledFunctions;
    union {
	unsigned char b[16];
	void* align;/* just to align field to ptr */
    } guard;
};

extern "C" BCAPI uint8_t cli_debug_flag;
namespace {

static sys::ThreadLocal<const jmp_buf> ExceptionReturn;

void do_shutdown() {
    llvm_shutdown();
}

static void NORETURN jit_exception_handler(void)
{
    longjmp(*(jmp_buf*)(ExceptionReturn.get()), 1);
}

static void NORETURN jit_ssp_handler(void)
{
    errs() << "Bytecode JIT: *** stack smashing detected, bytecode aborted\n";
    jit_exception_handler();
}

void llvm_error_handler(void *user_data, const std::string &reason)
{
    // Output it to stderr, it might exceed the 1k/4k limit of cli_errmsg
    errs() << MODULE << reason;
    jit_exception_handler();
}

// Since libgcc is not available on all compilers (for example on win32),
// just define what these functions should do, the compiler will forward to
// the appropriate libcall if needed.
static int64_t rtlib_sdiv_i64(int64_t a, int64_t b)
{
    return a/b;
}

static uint64_t rtlib_udiv_i64(uint64_t a, uint64_t b)
{
    return a/b;
}

static int64_t rtlib_srem_i64(int64_t a, int64_t b)
{
    return a%b;
}

static uint64_t rtlib_urem_i64(uint64_t a, uint64_t b)
{
    return a%b;
}

static int64_t rtlib_mul_i64(uint64_t a, uint64_t b)
{
    return a*b;
}

static int64_t rtlib_shl_i64(int64_t a, int32_t b)
{
    return a << b;
}

static int64_t rtlib_srl_i64(int64_t a, int32_t b)
{
    return (uint64_t)a >> b;
}
/* Implementation independent sign-extended signed right shift */
#ifdef HAVE_SAR
#define CLI_SRS(n,s) ((n)>>(s))
#else
#define CLI_SRS(n,s) ((((n)>>(s)) ^ (1<<(sizeof(n)*8-1-s))) - (1<<(sizeof(n)*8-1-s)))
#endif
static int64_t rtlib_sra_i64(int64_t a, int32_t b)
{
    return CLI_SRS(a, b);//CLI_./..
}

// Resolve integer libcalls, but nothing else.
static void* noUnknownFunctions(const std::string& name) {
    void *addr =
	StringSwitch<void*>(name)
	.Case("__divdi3", (void*)(intptr_t)rtlib_sdiv_i64)
	.Case("__udivdi3", (void*)(intptr_t)rtlib_udiv_i64)
	.Case("__moddi3", (void*)(intptr_t)rtlib_srem_i64)
	.Case("__umoddi3", (void*)(intptr_t)rtlib_urem_i64)
	.Case("__muldi3", (void*)(intptr_t)rtlib_mul_i64)
	.Case("__ashrdi3", (void*)(intptr_t)rtlib_sra_i64)
	.Case("__ashldi3", (void*)(intptr_t)rtlib_shl_i64)
	.Case("__lshrdi3", (void*)(intptr_t)rtlib_srl_i64)
	.Case("memmove", (void*)(intptr_t)memmove)
	.Case("memcpy", (void*)(intptr_t)memcpy)
	.Case("memset", (void*)(intptr_t)memset)
	.Case("abort", (void*)(intptr_t)jit_exception_handler)
	.Default(0);
    if (addr)
	return addr;

    std::string reason((Twine("Attempt to call external function ")+name).str());
    llvm_error_handler(0, reason);
    return 0;
}

class NotifyListener : public JITEventListener {
public:
    virtual void NotifyFunctionEmitted(const Function &F,
				       void *Code, size_t Size,
				       const EmittedFunctionDetails &Details)
    {
	if (!cli_debug_flag)
	    return;
	errs() << "bytecode JIT: emitted function " << F.getName() << 
	    " of " << Size << " bytes at 0x";
	errs().write_hex((uintptr_t)Code);
	errs() << "\n";
    }
};

class LLVMTypeMapper {
private:
    std::vector<PATypeHolder> TypeMap;
    LLVMContext &Context;
    unsigned numTypes;
    const Type *getStatic(uint16_t ty)
    {
	if (!ty)
	    return Type::getVoidTy(Context);
	if (ty <= 64)
	    return IntegerType::get(Context, ty);
	switch (ty) {
	    case 65:
		return PointerType::getUnqual(Type::getInt8Ty(Context));
	    case 66:
		return PointerType::getUnqual(Type::getInt16Ty(Context));
	    case 67:
		return PointerType::getUnqual(Type::getInt32Ty(Context));
	    case 68:
		return PointerType::getUnqual(Type::getInt64Ty(Context));
	}
	llvm_unreachable("getStatic");
    }
public:
    LLVMTypeMapper(LLVMContext &Context, const struct cli_bc_type *types,
		   unsigned count, const Type *Hidden=0) : Context(Context), numTypes(count)
    {
	TypeMap.reserve(count);
	// During recursive type construction pointers to Type* may be
	// invalidated, so we must use a TypeHolder to an Opaque type as a
	// start.
	for (unsigned i=0;i<count;i++) {
	    TypeMap.push_back(OpaqueType::get(Context));
	}
	std::vector<const Type*> Elts;
	for (unsigned i=0;i<count;i++) {
	    const struct cli_bc_type *type = &types[i];
	    Elts.clear();
	    unsigned n = type->kind == DArrayType ? 1 : type->numElements;
	    for (unsigned j=0;j<n;j++) {
		Elts.push_back(get(type->containedTypes[j]));
	    }
	    const Type *Ty;
	    switch (type->kind) {
		case DFunctionType:
		{
		    assert(Elts.size() > 0 && "Function with no return type?");
		    const Type *RetTy = Elts[0];
		    if (Hidden)
			Elts[0] = Hidden;
		    else
			Elts.erase(Elts.begin());
		    Ty = FunctionType::get(RetTy, Elts, false);
		    break;
		}
		case DPointerType:
		    if (!PointerType::isValidElementType(Elts[0]))
			Ty = PointerType::getUnqual(Type::getInt8Ty(Context));
		    else
			Ty = PointerType::getUnqual(Elts[0]);
		    break;
		case DStructType:
		    Ty = StructType::get(Context, Elts);
		    break;
		case DPackedStructType:
		    Ty = StructType::get(Context, Elts, true);
		    break;
		case DArrayType:
		    Ty = ArrayType::get(Elts[0], type->numElements);
		    break;
		default:
		    llvm_unreachable("type->kind");
	    }
	    // Make the opaque type a concrete type, doing recursive type
	    // unification if needed.
	    cast<OpaqueType>(TypeMap[i].get())->refineAbstractTypeTo(Ty);
	}
    }

    const Type *get(uint16_t ty)
    {
	ty &= 0x7fff;
	if (ty < 69)
	    return getStatic(ty);
	ty -= 69;
	assert(ty < numTypes && "TypeID out of range");
	return TypeMap[ty].get();
    }
};

struct CommonFunctions {
    Function *FHandler;
    Function *FMemset;
    Function *FMemmove;
    Function *FMemcpy;
    Function *FRealmemset;
    Function *FRealMemmove;
    Function *FRealmemcmp;
    Function *FRealmemcpy;
    Function *FBSwap16;
    Function *FBSwap32;
    Function *FBSwap64;
};

// loops with tripcounts higher than this need timeout check
static const unsigned LoopThreshold = 1000;

// after every N API calls we need timeout check
static const unsigned ApiThreshold = 100;

class RuntimeLimits : public FunctionPass {
    typedef SmallVector<std::pair<const BasicBlock*, const BasicBlock*>, 16>
	BBPairVectorTy;
    typedef SmallSet<BasicBlock*, 16> BBSetTy;
    typedef DenseMap<const BasicBlock*, unsigned> BBMapTy;
    bool loopNeedsTimeoutCheck(ScalarEvolution &SE, const Loop *L, BBMapTy &Map) {
	// This BB is a loop header, if trip count is small enough
	// no timeout checks are needed here.
	const SCEV *S = SE.getMaxBackedgeTakenCount(L);
	if (isa<SCEVCouldNotCompute>(S))
	    return true;
	DEBUG(errs() << "Found loop trip count" << *S << "\n");
	ConstantRange CR = SE.getUnsignedRange(S);
	uint64_t max = CR.getUnsignedMax().getLimitedValue();
	DEBUG(errs() << "Found max trip count " << max << "\n");
	if (max > LoopThreshold)
	    return true;
	unsigned apicalls = 0;
	for (Loop::block_iterator J=L->block_begin(),JE=L->block_end();
	     J != JE; ++J) {
	    apicalls += Map[*J];
	}
	apicalls *= max;
	if (apicalls > ApiThreshold) {
	    DEBUG(errs() << "apicall threshold exceeded: " << apicalls << "\n");
	    return true;
	}
	Map[L->getHeader()] = apicalls;
	return false;
    }

public:
    static char ID;
    RuntimeLimits() : FunctionPass(&ID) {}


    virtual bool runOnFunction(Function &F) {
	BBSetTy BackedgeTargets;
	if (!F.isDeclaration()) {
	    // Get the common backedge targets.
	    // Note that we don't rely on LoopInfo here, since
	    // it is possible to construct a CFG that doesn't have natural loops,
	    // yet it does have backedges, and thus can lead to unbounded/high
	    // execution time.
	    BBPairVectorTy V;
	    FindFunctionBackedges(F, V);
	    for (BBPairVectorTy::iterator I=V.begin(),E=V.end();I != E; ++I) {
		BackedgeTargets.insert(const_cast<BasicBlock*>(I->second));
	    }
	}
	BBSetTy  needsTimeoutCheck;
	BBMapTy BBMap;
	DominatorTree &DT = getAnalysis<DominatorTree>();
	for (Function::iterator I=F.begin(),E=F.end(); I != E; ++I) {
	    BasicBlock *BB = &*I;
	    unsigned apicalls = 0;
	    for (BasicBlock::const_iterator J=BB->begin(),JE=BB->end();
		 J != JE; ++J) {
		if (const CallInst *CI = dyn_cast<CallInst>(J)) {
		    Function *F = CI->getCalledFunction();
		    if (!F || F->isDeclaration())
			apicalls++;
		}
	    }
	    if (apicalls > ApiThreshold) {
		DEBUG(errs() << "apicall threshold exceeded: " << apicalls << "\n");
		needsTimeoutCheck.insert(BB);
		apicalls = 0;
	    }
	    BBMap[BB] = apicalls;
	}
	if (!BackedgeTargets.empty()) {
	    LoopInfo &LI = getAnalysis<LoopInfo>();
	    ScalarEvolution &SE = getAnalysis<ScalarEvolution>();

	    // Now check whether any of these backedge targets are part of a loop
	    // with a small constant trip count
	    for (BBSetTy::iterator I=BackedgeTargets.begin(),E=BackedgeTargets.end();
		 I != E; ++I) {
		const Loop *L = LI.getLoopFor(*I);
		if (L && L->getHeader() == *I &&
		    !loopNeedsTimeoutCheck(SE, L, BBMap))
		    continue;
		needsTimeoutCheck.insert(*I);
		BBMap[*I] = 0;
	    }
	}
	// Estimate number of apicalls by walking dominator-tree bottom-up.
	// BBs that have timeout checks are considered to have 0 APIcalls
	// (since we already checked for timeout).
	for (po_iterator<DomTreeNode*> I = po_begin(DT.getRootNode()),
	     E = po_end(DT.getRootNode()); I != E; ++I) {
	    if (needsTimeoutCheck.count(I->getBlock()))
		continue;
	    unsigned apicalls = BBMap[I->getBlock()];
	    for (DomTreeNode::iterator J=I->begin(),JE=I->end();
		 J != JE; ++J) {
		apicalls += BBMap[(*J)->getBlock()];
	    }
	    if (apicalls > ApiThreshold) {
		needsTimeoutCheck.insert(I->getBlock());
		apicalls = 0;
	    }
	    BBMap[I->getBlock()] = apicalls;
	}
	if (needsTimeoutCheck.empty())
	    return false;
	DEBUG(errs() << "needs timeoutcheck:\n");
	std::vector<const Type*>args;
	FunctionType* abrtTy = FunctionType::get(
	    Type::getVoidTy(F.getContext()),args,false);
	Constant *func_abort =
	    F.getParent()->getOrInsertFunction("abort", abrtTy);
	BasicBlock *AbrtBB = BasicBlock::Create(F.getContext(), "", &F);
        CallInst* AbrtC = CallInst::Create(func_abort, "", AbrtBB);
        AbrtC->setCallingConv(CallingConv::C);
        AbrtC->setTailCall(true);
        AbrtC->setDoesNotReturn(true);
        AbrtC->setDoesNotThrow(true);
        new UnreachableInst(F.getContext(), AbrtBB);
	IRBuilder<false> Builder(F.getContext());
	Function *LSBarrier = Intrinsic::getDeclaration(F.getParent(),
							Intrinsic::memory_barrier);
	Value *Flag = F.arg_begin();
	Value *MBArgs[] = {
	    ConstantInt::getFalse(F.getContext()),
	    ConstantInt::getFalse(F.getContext()),
	    ConstantInt::getTrue(F.getContext()),
	    ConstantInt::getFalse(F.getContext()),
	    ConstantInt::getFalse(F.getContext())
	};
	verifyFunction(F);
	BasicBlock *BB = &F.getEntryBlock();
	Builder.SetInsertPoint(BB, BB->getTerminator());
	Flag = Builder.CreatePointerCast(Flag, PointerType::getUnqual(
		Type::getInt1Ty(F.getContext())));
	for (BBSetTy::iterator I=needsTimeoutCheck.begin(),
	     E=needsTimeoutCheck.end(); I != E; ++I) {
	    BasicBlock *BB = *I;
	    Builder.SetInsertPoint(BB, BB->getTerminator());
	    // store-load barrier: will be a no-op on x86 but not other arches
	    Builder.CreateCall(LSBarrier, MBArgs, MBArgs+5);
	    // Load Flag that tells us we timed out (first byte in bc_ctx)
	    Value *Cond = Builder.CreateLoad(Flag, true);
	    BasicBlock *newBB = SplitBlock(BB, BB->getTerminator(), this);
	    TerminatorInst *TI = BB->getTerminator();
	    BranchInst::Create(AbrtBB, newBB, Cond, TI);
	    TI->eraseFromParent();
	    // Update dominator info
	    DomTreeNode *N = DT.getNode(AbrtBB);
	    if (!N) {
		DT.addNewBlock(AbrtBB, BB);
	    } else {
		BasicBlock *DomBB = DT.findNearestCommonDominator(BB,
								  N->getIDom()->getBlock());
		DT.changeImmediateDominator(AbrtBB, DomBB);
	    }
	    DEBUG(errs() << *I << "\n");
	}
	//verifyFunction(F);
	return true;
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
      AU.addRequired<LoopInfo>();
      AU.addRequired<ScalarEvolution>();
      AU.addRequired<DominatorTree>();
    }
};
char RuntimeLimits::ID;

class VISIBILITY_HIDDEN LLVMCodegen {
private:
    const struct cli_bc *bc;
    Module *M;
    LLVMContext &Context;
    ExecutionEngine *EE;
    PassManager &PM;
    LLVMTypeMapper *TypeMap;

    Function **apiFuncs;
    LLVMTypeMapper &apiMap;
    FunctionMapTy &compiledFunctions;
    Twine BytecodeID;

    TargetFolder Folder;
    IRBuilder<false, TargetFolder> Builder;

    std::vector<Value*> globals;
    DenseMap<unsigned, unsigned> GVoffsetMap;
    DenseMap<unsigned, const Type*> GVtypeMap;
    Value **Values;
    unsigned numLocals;
    unsigned numArgs;
    std::vector<MDNode*> mdnodes;

    struct CommonFunctions *CF;

    Value *getOperand(const struct cli_bc_func *func, const Type *Ty, operand_t operand)
    {
	unsigned map[] = {0, 1, 2, 3, 3, 4, 4, 4, 4};
	if (operand < func->numValues)
	    return Values[operand];
	unsigned w = Ty->getPrimitiveSizeInBits();
	if (w > 1)
	    w = (w+7)/8;
	else
	    w = 0;
	return convertOperand(func, map[w], operand);
    }

    Value *convertOperand(const struct cli_bc_func *func, const Type *Ty, operand_t operand)
    {
	unsigned map[] = {0, 1, 2, 3, 3, 4, 4, 4, 4};
	if (operand < func->numArgs)
	    return Values[operand];
	if (operand < func->numValues) {
	    Value *V = Values[operand];
	    if (func->types[operand]&0x8000 && V->getType() == Ty) {
		return V;
	    }
	    V = Builder.CreateLoad(V);
	    if (V->getType() != Ty &&
		isa<PointerType>(V->getType()) &&
		isa<PointerType>(Ty))
		V = Builder.CreateBitCast(V, Ty);
	    if (V->getType() != Ty) {
		errs() << operand << " ";
		V->dump();
		Ty->dump();
		llvm_report_error("(libclamav) Type mismatch converting operand");
	    }
	    return V;
	}
	unsigned w = Ty->getPrimitiveSizeInBits();
	if (w > 1)
	    w = (w+7)/8;
	else
	    w = 0;
	return convertOperand(func, map[w], operand);
    }

    Value *convertOperand(const struct cli_bc_func *func,
			  const struct cli_bc_inst *inst,  operand_t operand)
    {
	return convertOperand(func, inst->interp_op%5, operand);
    }

    Value *convertOperand(const struct cli_bc_func *func,
			  unsigned w, operand_t operand) {
	if (operand < func->numArgs)
	    return Values[operand];
	if (operand < func->numValues) {
	    if (func->types[operand]&0x8000)
		return Values[operand];
	    return Builder.CreateLoad(Values[operand]);
	}

	if (operand & 0x80000000) {
	    operand &= 0x7fffffff;
	    assert(operand < globals.size() && "Global index out of range");
	    // Global
	    if (!operand)
		return ConstantPointerNull::get(PointerType::getUnqual(Type::getInt8Ty(Context)));
	    assert(globals[operand]);
	    if (GlobalVariable *GV = dyn_cast<GlobalVariable>(globals[operand])) {
		if (ConstantExpr *CE = dyn_cast<ConstantExpr>(GV->getInitializer())) {
		    return CE;
		}
		return GV;
	    }
	    return globals[operand];
	}
	// Constant
	operand -= func->numValues;
	// This was already validated by libclamav.
	assert(operand < func->numConstants && "Constant out of range");
	uint64_t *c = &func->constants[operand];
	uint64_t v;
	const Type *Ty;
	switch (w) {
	    case 0:
	    case 1:
		Ty = w ? Type::getInt8Ty(Context) :
		    Type::getInt1Ty(Context);
		v = *(uint8_t*)c;
		break;
	    case 2:
		Ty = Type::getInt16Ty(Context);
		v = *(uint16_t*)c;
		break;
	    case 3:
		Ty = Type::getInt32Ty(Context);
		v = *(uint32_t*)c;
		break;
	    case 4:
		Ty = Type::getInt64Ty(Context);
		v = *(uint64_t*)c;
		break;
	    default:
		llvm_unreachable("width");
	}
	return ConstantInt::get(Ty, v);
    }

    void Store(uint16_t dest, Value *V)
    {
	assert(dest >= numArgs && dest < numLocals+numArgs && "Instruction destination out of range");
	Builder.CreateStore(V, Values[dest]);
    }

    // Insert code that calls \arg CF->FHandler if \arg FailCond is true.
    void InsertVerify(Value *FailCond, BasicBlock *&Fail, Function *FHandler, 
		      Function *F) {
	if (!Fail) {
	    Fail = BasicBlock::Create(Context, "fail", F);
	    CallInst::Create(FHandler,"",Fail);
	    new UnreachableInst(Context, Fail);
	}
	BasicBlock *OkBB = BasicBlock::Create(Context, "", F);
	Builder.CreateCondBr(FailCond, Fail, OkBB);
	Builder.SetInsertPoint(OkBB);
    }

    const Type* mapType(uint16_t typeID)
    {
	return TypeMap->get(typeID&0x7fffffff);
    }

    Constant *buildConstant(const Type *Ty, uint64_t *components, unsigned &c)
    {
        if (const PointerType *PTy = dyn_cast<PointerType>(Ty)) {
          Value *idxs[1] = {
	      ConstantInt::get(Type::getInt64Ty(Context), components[c++])
	  };
	  unsigned idx = components[c++];
	  if (!idx)
	      return ConstantPointerNull::get(PTy);
	  assert(idx < globals.size());
	  GlobalVariable *GV = cast<GlobalVariable>(globals[idx]);
	  const Type *IP8Ty = PointerType::getUnqual(Type::getInt8Ty(Ty->getContext()));
	  Constant *C = ConstantExpr::getPointerCast(GV, IP8Ty);
	  //TODO: check constant bounds here
	  return ConstantExpr::getPointerCast(
	      ConstantExpr::getInBoundsGetElementPtr(C, idxs, 1),
	      PTy);
        }
	if (isa<IntegerType>(Ty)) {
	    return ConstantInt::get(Ty, components[c++]);
	}
	if (const ArrayType *ATy = dyn_cast<ArrayType>(Ty)) {
	   std::vector<Constant*> elements;
	   elements.reserve(ATy->getNumElements());
	   for (unsigned i=0;i<ATy->getNumElements();i++) {
	       elements.push_back(buildConstant(ATy->getElementType(), components, c));
	   }
	   return ConstantArray::get(ATy, elements);
	}
	if (const StructType *STy = dyn_cast<StructType>(Ty)) {
	   std::vector<Constant*> elements;
	   elements.reserve(STy->getNumElements());
	   for (unsigned i=0;i<STy->getNumElements();i++) {
	       elements.push_back(buildConstant(STy->getElementType(i), components, c));
	   }
	   return ConstantStruct::get(STy, elements);
	}
	Ty->dump();
	llvm_unreachable("invalid type");
	return 0;
    }


public:
    LLVMCodegen(const struct cli_bc *bc, Module *M, struct CommonFunctions *CF, FunctionMapTy &cFuncs,
		ExecutionEngine *EE, PassManager &PM,
		Function **apiFuncs, LLVMTypeMapper &apiMap)
	: bc(bc), M(M), Context(M->getContext()), EE(EE),
	PM(PM), apiFuncs(apiFuncs),apiMap(apiMap),
	compiledFunctions(cFuncs), BytecodeID("bc"+Twine(bc->id)),
	Folder(EE->getTargetData()), Builder(Context, Folder), CF(CF) {

	for (unsigned i=0;i<cli_apicall_maxglobal - _FIRST_GLOBAL;i++) {
	    unsigned id = cli_globals[i].globalid;
	    GVoffsetMap[id] = cli_globals[i].offset;
	}
    }

    template <typename InputIterator>
    Value* createGEP(Value *Base, const Type *ETy, InputIterator Start, InputIterator End) {
	const Type *Ty = GetElementPtrInst::getIndexedType(Base->getType(), Start, End);
	if (!Ty || (ETy && (Ty != ETy && (!isa<IntegerType>(Ty) || !isa<IntegerType>(ETy))))) {
	    errs() << MODULE << "Wrong indices for GEP opcode: "
		<< " expected type: " << *ETy;
	    if (Ty)
		errs() << " actual type: " << *Ty;
	    errs() << " base: " << *Base << ";";
	    Base->getType()->dump();
	    errs() << "\n indices: ";
	    for (InputIterator I=Start; I != End; I++) {
		errs() << **I << ", ";
	    }
	    errs() << "\n";
	    return 0;
	}
	return Builder.CreateGEP(Base, Start, End);
    }

    template <typename InputIterator>
    bool createGEP(unsigned dest, Value *Base, InputIterator Start, InputIterator End) {
	assert(dest >= numArgs && dest < numLocals+numArgs && "Instruction destination out of range");
	const Type *ETy = cast<PointerType>(cast<PointerType>(Values[dest]->getType())->getElementType())->getElementType();
	Value *V = createGEP(Base, ETy, Start, End);
	if (!V) {
	    errs() << "@ " << dest << "\n";
	    return false;
	}
	V = Builder.CreateBitCast(V, PointerType::getUnqual(ETy));
	Store(dest, V);
	return true;
    }

    MDNode *convertMDNode(unsigned i) {
	if (i < mdnodes.size()) {
	    if (mdnodes[i])
		return mdnodes[i];
	} else 
	    mdnodes.resize(i+1);
	assert(i < mdnodes.size());
	const struct cli_bc_dbgnode *node = &bc->dbgnodes[i];
	Value **Vals = new Value*[node->numelements];
	for (unsigned j=0;j<node->numelements;j++) {
	    const struct cli_bc_dbgnode_element* el = &node->elements[j];
	    Value *V;
	    if (!el->len) {
		if (el->nodeid == ~0u)
		    V = 0;
		else if (el->nodeid)
		    V = convertMDNode(el->nodeid);
		else
		    V = MDString::get(Context, "");
	    } else if (el->string) {
		V = MDString::get(Context, StringRef(el->string, el->len));
	    } else {
		V = ConstantInt::get(IntegerType::get(Context, el->len),
				     el->constant);
	    }
	    Vals[j] = V;
	}
	MDNode *N = MDNode::get(Context, Vals, node->numelements);
	delete[] Vals;
	mdnodes[i] = N;
	return N;
    }

    void AddStackProtect(Function *F)
    {
	BasicBlock &BB = F->getEntryBlock();
	if (isa<AllocaInst>(BB.begin())) {
	    // Have an alloca -> some instruction uses its address otherwise
	    // mem2reg would have converted it to an SSA register.
	    // Enable stack protector for this function.
	    F->addFnAttr(Attribute::StackProtect);
	    F->addFnAttr(Attribute::StackProtectReq);
	}
    }

    Value *GEPOperand(Value *V) {
	if (LoadInst *LI = dyn_cast<LoadInst>(V)) {
	    Value *VI = LI->getOperand(0);
	    StoreInst *SI = 0;
	    for (Value::use_iterator I=VI->use_begin(),
		 E=VI->use_end(); I != E; ++I) {
		if (StoreInst *S = dyn_cast<StoreInst>(I)) {
		    if (SI)
			return V;
		    SI = S;
		} else if (!isa<LoadInst>(I))
		    return V;
	    }
	    V = SI->getOperand(0);
	}
	if (EE->getTargetData()->getPointerSize() == 8) {
	    // eliminate useless trunc, GEP can take i64 too
	    if (TruncInst *I = dyn_cast<TruncInst>(V)) {
		Value *Src = I->getOperand(0);
		if (Src->getType() == Type::getInt64Ty(Context) &&
		    I->getType() == Type::getInt32Ty(Context))
		    return Src;
	    }
	}
	return V;
    }

    bool generate() {
	TypeMap = new LLVMTypeMapper(Context, bc->types + 4, bc->num_types - 5);
	for (unsigned i=0;i<bc->dbgnode_cnt;i++) {
	    mdnodes.push_back(convertMDNode(i));
	}

	for (unsigned i=0;i<cli_apicall_maxglobal - _FIRST_GLOBAL;i++) {
	    unsigned id = cli_globals[i].globalid;
	    const Type *Ty = apiMap.get(cli_globals[i].type);
	    /*if (const ArrayType *ATy = dyn_cast<ArrayType>(Ty))
		Ty = PointerType::getUnqual(ATy->getElementType());*/
	    GVtypeMap[id] = Ty;
	}

	// The hidden ctx param to all functions
	unsigned maxh = cli_globals[0].offset + sizeof(struct cli_bc_hooks);
	const Type *HiddenCtx = PointerType::getUnqual(ArrayType::get(Type::getInt8Ty(Context), maxh));

	globals.reserve(bc->num_globals);
	BitVector FakeGVs;
	FakeGVs.resize(bc->num_globals);
	globals.push_back(0);
	for (unsigned i=1;i<bc->num_globals;i++) {
	    const Type *Ty = mapType(bc->globaltys[i]);

	    // TODO: validate number of components against type_components
	    unsigned c = 0;
	    GlobalVariable *GV;
	    if (isa<PointerType>(Ty)) {
		unsigned g = bc->globals[i][1];
		if (GVoffsetMap.count(g)) {
		    FakeGVs.set(i);
		    globals.push_back(0);
		    continue;
		}
	    }
	    Constant *C = buildConstant(Ty, bc->globals[i], c);
	    GV = new GlobalVariable(*M, Ty, true,
				    GlobalValue::InternalLinkage,
				    C, "glob"+Twine(i));
	    globals.push_back(GV);
	}
	Function **Functions = new Function*[bc->num_func];
	for (unsigned j=0;j<bc->num_func;j++) {
	    PrettyStackTraceString CrashInfo("Generate LLVM IR functions");
	    // Create LLVM IR Function
	    const struct cli_bc_func *func = &bc->funcs[j];
	    std::vector<const Type*> argTypes;
	    argTypes.push_back(HiddenCtx);
	    for (unsigned a=0;a<func->numArgs;a++) {
		argTypes.push_back(mapType(func->types[a]));
	    }
	    const Type *RetTy = mapType(func->returnType);
	    FunctionType *FTy =  FunctionType::get(RetTy, argTypes,
							 false);
	    Functions[j] = Function::Create(FTy, Function::InternalLinkage,
					   BytecodeID+"f"+Twine(j), M);
	    Functions[j]->setDoesNotThrow();
	    Functions[j]->setCallingConv(CallingConv::Fast);
	}
	const Type *I32Ty = Type::getInt32Ty(Context);
	PM.add(createDeadCodeEliminationPass());
	if (!bc->trusted)
	    PM.add(createClamBCRTChecks());
	PM.add(new RuntimeLimits());
	for (unsigned j=0;j<bc->num_func;j++) {
	    PrettyStackTraceString CrashInfo("Generate LLVM IR");
	    const struct cli_bc_func *func = &bc->funcs[j];

	    // Create all BasicBlocks
	    Function *F = Functions[j];
	    BasicBlock **BB = new BasicBlock*[func->numBB];
	    for (unsigned i=0;i<func->numBB;i++) {
		BB[i] = BasicBlock::Create(Context, "", F);
	    }

	    BasicBlock *Fail = 0;
	    Values = new Value*[func->numValues];
	    Builder.SetInsertPoint(BB[0]);
	    Function::arg_iterator I = F->arg_begin();
	    assert(F->arg_size() == (unsigned)(func->numArgs + 1) && "Mismatched args");
	    ++I;
	    for (unsigned i=0;i<func->numArgs; i++) {
		assert(I != F->arg_end());
		Values[i] = &*I;
		++I;
	    }
	    for (unsigned i=func->numArgs;i<func->numValues;i++) {
		if (!func->types[i]) {
		    //instructions without return value, like store
		    Values[i] = 0;
		    continue;
		}
		Values[i] = Builder.CreateAlloca(mapType(func->types[i]));
	    }
	    numLocals = func->numLocals;
	    numArgs = func->numArgs;

	    if (FakeGVs.any()) {
		Argument *Ctx = F->arg_begin();
		for (unsigned i=0;i<bc->num_globals;i++) {
		    if (!FakeGVs[i])
			continue;
		    unsigned g = bc->globals[i][1];
		    unsigned offset = GVoffsetMap[g];

		    Constant *Idx = ConstantInt::get(Type::getInt32Ty(Context),
						     offset);
		    Value *Idxs[2] = {
			ConstantInt::get(Type::getInt32Ty(Context), 0),
			Idx
		    };
		    Value *GEP = Builder.CreateInBoundsGEP(Ctx, Idxs, Idxs+2);
		    const Type *Ty = GVtypeMap[g];
		    Ty = PointerType::getUnqual(PointerType::getUnqual(Ty));
		    Value *Cast = Builder.CreateBitCast(GEP, Ty);
		    Value *SpecialGV = Builder.CreateLoad(Cast);
		    const Type *IP8Ty = Type::getInt8Ty(Context);
		    IP8Ty = PointerType::getUnqual(IP8Ty);
		    SpecialGV = Builder.CreateBitCast(SpecialGV, IP8Ty);
		    SpecialGV->setName("g"+Twine(g-_FIRST_GLOBAL)+"_");
		    Value *C[] = {
			ConstantInt::get(Type::getInt32Ty(Context), bc->globals[i][0])
		    };
		    globals[i] = createGEP(SpecialGV, 0, C, C+1);
		    if (!globals[i]) {
			errs() << i << ":" << g << ":" << bc->globals[i][0] <<"\n";
			Ty->dump();
			llvm_report_error("(libclamav) unable to create fake global");
		    }
		    globals[i] = Builder.CreateBitCast(globals[i], Ty);
		    if(GetElementPtrInst *GI = dyn_cast<GetElementPtrInst>(globals[i])) {
			GI->setIsInBounds(true);
			GI->setName("geped"+Twine(i)+"_");
		    }
		}
	    }

	    // Generate LLVM IR for each BB
	    for (unsigned i=0;i<func->numBB;i++) {
		bool unreachable = false;
		const struct cli_bc_bb *bb = &func->BB[i];
		Builder.SetInsertPoint(BB[i]);
		unsigned c = 0;
		for (unsigned j=0;j<bb->numInsts;j++) {
		    const struct cli_bc_inst *inst = &bb->insts[j];
		    Value *Op0=0, *Op1=0, *Op2=0;
		    // libclamav has already validated this.
		    assert(inst->opcode < OP_BC_INVALID && "Invalid opcode");
		    if (func->dbgnodes) {
			if (func->dbgnodes[c] != ~0u) {
			unsigned j = func->dbgnodes[c];
			assert(j < mdnodes.size());
			Builder.SetCurrentDebugLocation(mdnodes[j]);
			} else
			    Builder.SetCurrentDebugLocation(0);
		    }
		    c++;
		    switch (inst->opcode) {
			case OP_BC_JMP:
			case OP_BC_BRANCH:
			case OP_BC_CALL_API:
			case OP_BC_CALL_DIRECT:
			case OP_BC_ZEXT:
			case OP_BC_SEXT:
			case OP_BC_TRUNC:
			case OP_BC_GEP1:
			case OP_BC_GEPZ:
			case OP_BC_GEPN:
			case OP_BC_STORE:
			case OP_BC_COPY:
			case OP_BC_RET:
			case OP_BC_PTRDIFF32:
			case OP_BC_PTRTOINT64:
			    // these instructions represents operands differently
			    break;
			default:
			    switch (operand_counts[inst->opcode]) {
				case 1:
				    Op0 = convertOperand(func, inst, inst->u.unaryop);
				    break;
				case 2:
				    Op0 = convertOperand(func, inst, inst->u.binop[0]);
				    Op1 = convertOperand(func, inst, inst->u.binop[1]);
				    if (Op0->getType() != Op1->getType()) {
					Op0->dump();
					Op1->dump();
					llvm_report_error("(libclamav) binop type mismatch");
				    }
				    break;
				case 3:
				    Op0 = convertOperand(func, inst, inst->u.three[0]);
				    Op1 = convertOperand(func, inst, inst->u.three[1]);
				    Op2 = convertOperand(func, inst, inst->u.three[2]);
				    break;
			    }
		    }

		    switch (inst->opcode) {
			case OP_BC_ADD:
			    Store(inst->dest, Builder.CreateAdd(Op0, Op1));
			    break;
			case OP_BC_SUB:
			    Store(inst->dest, Builder.CreateSub(Op0, Op1));
			    break;
			case OP_BC_MUL:
			    Store(inst->dest, Builder.CreateMul(Op0, Op1));
			    break;
			case OP_BC_UDIV:
			{
			    Value *Bad = Builder.CreateICmpEQ(Op1, ConstantInt::get(Op1->getType(), 0));
			    InsertVerify(Bad, Fail, CF->FHandler, F);
			    Store(inst->dest, Builder.CreateUDiv(Op0, Op1));
			    break;
			}
			case OP_BC_SDIV:
			{
			    //TODO: also verify Op0 == -1 && Op1 = INT_MIN
			    Value *Bad = Builder.CreateICmpEQ(Op1, ConstantInt::get(Op1->getType(), 0));
			    InsertVerify(Bad, Fail, CF->FHandler, F);
			    Store(inst->dest, Builder.CreateSDiv(Op0, Op1));
			    break;
			}
			case OP_BC_UREM:
			{
			    Value *Bad = Builder.CreateICmpEQ(Op1, ConstantInt::get(Op1->getType(), 0));
			    InsertVerify(Bad, Fail, CF->FHandler, F);
			    Store(inst->dest, Builder.CreateURem(Op0, Op1));
			    break;
			}
			case OP_BC_SREM:
			{
			    //TODO: also verify Op0 == -1 && Op1 = INT_MIN
			    Value *Bad = Builder.CreateICmpEQ(Op1, ConstantInt::get(Op1->getType(), 0));
			    InsertVerify(Bad, Fail, CF->FHandler, F);
			    Store(inst->dest, Builder.CreateSRem(Op0, Op1));
			    break;
			}
			case OP_BC_SHL:
			    Store(inst->dest, Builder.CreateShl(Op0, Op1));
			    break;
			case OP_BC_LSHR:
			    Store(inst->dest, Builder.CreateLShr(Op0, Op1));
			    break;
			case OP_BC_ASHR:
			    Store(inst->dest, Builder.CreateAShr(Op0, Op1));
			    break;
			case OP_BC_AND:
			    Store(inst->dest, Builder.CreateAnd(Op0, Op1));
			    break;
			case OP_BC_OR:
			    Store(inst->dest, Builder.CreateOr(Op0, Op1));
			    break;
			case OP_BC_XOR:
			    Store(inst->dest, Builder.CreateXor(Op0, Op1));
			    break;
			case OP_BC_TRUNC:
			{
			    Value *Src = convertOperand(func, inst, inst->u.cast.source);
			    const Type *Ty = mapType(func->types[inst->dest]);
			    Store(inst->dest, Builder.CreateTrunc(Src,  Ty));
			    break;
			}
			case OP_BC_ZEXT:
			{
			    Value *Src = convertOperand(func, inst, inst->u.cast.source);
			    const Type *Ty = mapType(func->types[inst->dest]);
			    Store(inst->dest, Builder.CreateZExt(Src,  Ty));
			    break;
			}
			case OP_BC_SEXT:
			{
			    Value *Src = convertOperand(func, inst, inst->u.cast.source);
			    const Type *Ty = mapType(func->types[inst->dest]);
			    Store(inst->dest, Builder.CreateSExt(Src,  Ty));
			    break;
			}
			case OP_BC_BRANCH:
			{
			    Value *Cond = convertOperand(func, inst, inst->u.branch.condition);
			    BasicBlock *True = BB[inst->u.branch.br_true];
			    BasicBlock *False = BB[inst->u.branch.br_false];
			    if (Cond->getType() != Type::getInt1Ty(Context)) {
				errs() << MODULE << "type mismatch in condition\n";
				return false;
			    }
			    Builder.CreateCondBr(Cond, True, False);
			    break;
			}
			case OP_BC_JMP:
			{
			    BasicBlock *Jmp = BB[inst->u.jump];
			    Builder.CreateBr(Jmp);
			    break;
			}
			case OP_BC_RET:
			{
			    Op0 = convertOperand(func, F->getReturnType(), inst->u.unaryop);
			    Builder.CreateRet(Op0);
			    break;
			}
			case OP_BC_RET_VOID:
			    Builder.CreateRetVoid();
			    break;
			case OP_BC_ICMP_EQ:
			    Store(inst->dest, Builder.CreateICmpEQ(Op0, Op1));
			    break;
			case OP_BC_ICMP_NE:
			    Store(inst->dest, Builder.CreateICmpNE(Op0, Op1));
			    break;
			case OP_BC_ICMP_UGT:
			    Store(inst->dest, Builder.CreateICmpUGT(Op0, Op1));
			    break;
			case OP_BC_ICMP_UGE:
			    Store(inst->dest, Builder.CreateICmpUGE(Op0, Op1));
			    break;
			case OP_BC_ICMP_ULT:
			    Store(inst->dest, Builder.CreateICmpULT(Op0, Op1));
			    break;
			case OP_BC_ICMP_ULE:
			    Store(inst->dest, Builder.CreateICmpULE(Op0, Op1));
			    break;
			case OP_BC_ICMP_SGT:
			    Store(inst->dest, Builder.CreateICmpSGT(Op0, Op1));
			    break;
			case OP_BC_ICMP_SGE:
			    Store(inst->dest, Builder.CreateICmpSGE(Op0, Op1));
			    break;
			case OP_BC_ICMP_SLT:
			    Store(inst->dest, Builder.CreateICmpSLT(Op0, Op1));
			    break;
			case OP_BC_SELECT:
			    Store(inst->dest, Builder.CreateSelect(Op0, Op1, Op2));
			    break;
			case OP_BC_COPY:
			{
			    Value *Dest = Values[inst->u.binop[1]];
			    const PointerType *PTy = cast<PointerType>(Dest->getType());
			    Op0 = convertOperand(func, PTy->getElementType(), inst->u.binop[0]);
			    PTy = PointerType::getUnqual(Op0->getType());
			    Dest = Builder.CreateBitCast(Dest, PTy);
			    Builder.CreateStore(Op0, Dest);
			    break;
			}
			case OP_BC_CALL_DIRECT:
			{
			    Function *DestF = Functions[inst->u.ops.funcid];
			    SmallVector<Value*, 2> args;
			    args.push_back(&*F->arg_begin()); // pass hidden arg
			    for (unsigned a=0;a<inst->u.ops.numOps;a++) {
				operand_t op = inst->u.ops.ops[a];
				args.push_back(convertOperand(func, DestF->getFunctionType()->getParamType(a+1), op));
			    }
			    CallInst *CI = Builder.CreateCall(DestF, args.begin(), args.end());
			    CI->setCallingConv(CallingConv::Fast);
			    CI->setDoesNotThrow(true);
			    if (CI->getType()->getTypeID() != Type::VoidTyID)
				Store(inst->dest, CI);
			    break;
			}
			case OP_BC_CALL_API:
			{
			    assert(inst->u.ops.funcid < cli_apicall_maxapi && "APICall out of range");
			    std::vector<Value*> args;
			    Function *DestF = apiFuncs[inst->u.ops.funcid];
			    args.push_back(&*F->arg_begin()); // pass hidden arg
			    for (unsigned a=0;a<inst->u.ops.numOps;a++) {
				operand_t op = inst->u.ops.ops[a];
				args.push_back(convertOperand(func, DestF->getFunctionType()->getParamType(a+1), op));
			    }
			    CallInst *CI = Builder.CreateCall(DestF, args.begin(), args.end());
			    CI->setDoesNotThrow(true);
			    Store(inst->dest, CI);
			    break;
			}
			case OP_BC_GEP1:
			{
			    const Type *SrcTy = mapType(inst->u.three[0]);
			    Value *V = convertOperand(func, SrcTy, inst->u.three[1]);
			    Value *Op = convertOperand(func, I32Ty, inst->u.three[2]);
			    Op = GEPOperand(Op);
			    if (!createGEP(inst->dest, V, &Op, &Op+1))
				return false;
			    break;
			}
			case OP_BC_GEPZ:
			{
			    Value *Ops[2];
			    Ops[0] = ConstantInt::get(Type::getInt32Ty(Context), 0);
			    const Type *SrcTy = mapType(inst->u.three[0]);
			    Value *V = convertOperand(func, SrcTy, inst->u.three[1]);
			    Ops[1] = convertOperand(func, I32Ty, inst->u.three[2]);
			    Ops[1] = GEPOperand(Ops[1]);
			    if (!createGEP(inst->dest, V, Ops, Ops+2))
				return false;
			    break;
			}
			case OP_BC_GEPN:
			{
			    std::vector<Value*> Idxs;
			    assert(inst->u.ops.numOps > 2);
			    const Type *SrcTy = mapType(inst->u.ops.ops[0]);
			    Value *V = convertOperand(func, SrcTy, inst->u.ops.ops[1]);
			    for (unsigned a=2;a<inst->u.ops.numOps;a++) {
				Value *Op = convertOperand(func, I32Ty, inst->u.ops.ops[a]);
				Op = GEPOperand(Op);
				Idxs.push_back(Op);
			    }
			    if (!createGEP(inst->dest, V, Idxs.begin(), Idxs.end()))
				return false;
			    break;
			}
			case OP_BC_STORE:
			{
			    Value *Dest = convertOperand(func, inst, inst->u.binop[1]);
			    Value *V = convertOperand(func, inst, inst->u.binop[0]);
			    const Type *VPTy = PointerType::getUnqual(V->getType());
			    if (VPTy != Dest->getType())
				Dest = Builder.CreateBitCast(Dest, VPTy);
			    Builder.CreateStore(V, Dest);
			    break;
			}
			case OP_BC_LOAD:
			{
			    Op0 = Builder.CreateBitCast(Op0,
							Values[inst->dest]->getType());
			    Op0 = Builder.CreateLoad(Op0);
			    Store(inst->dest, Op0);
			    break;
			}
			case OP_BC_MEMSET:
			{
			    Value *Dst = convertOperand(func, inst, inst->u.three[0]);
			    Dst = Builder.CreatePointerCast(Dst, PointerType::getUnqual(Type::getInt8Ty(Context)));
			    Value *Val = convertOperand(func, Type::getInt8Ty(Context), inst->u.three[1]);
			    Value *Len = convertOperand(func, Type::getInt32Ty(Context), inst->u.three[2]);
			    CallInst *c = Builder.CreateCall4(CF->FMemset, Dst, Val, Len,
								ConstantInt::get(Type::getInt32Ty(Context), 1));
			    c->setTailCall(true);
			    c->setDoesNotThrow();
			    break;
			}
			case OP_BC_MEMCPY:
			{
			    Value *Dst = convertOperand(func, inst, inst->u.three[0]);
			    Dst = Builder.CreatePointerCast(Dst, PointerType::getUnqual(Type::getInt8Ty(Context)));
			    Value *Src = convertOperand(func, inst, inst->u.three[1]);
			    Src = Builder.CreatePointerCast(Src, PointerType::getUnqual(Type::getInt8Ty(Context)));
			    Value *Len = convertOperand(func, Type::getInt32Ty(Context), inst->u.three[2]);
			    CallInst *c = Builder.CreateCall4(CF->FMemcpy, Dst, Src, Len,
								ConstantInt::get(Type::getInt32Ty(Context), 1));
			    c->setTailCall(true);
			    c->setDoesNotThrow();
			    break;
			}
			case OP_BC_MEMMOVE:
			{
			    Value *Dst = convertOperand(func, inst, inst->u.three[0]);
			    Dst = Builder.CreatePointerCast(Dst, PointerType::getUnqual(Type::getInt8Ty(Context)));
			    Value *Src = convertOperand(func, inst, inst->u.three[1]);
			    Src = Builder.CreatePointerCast(Src, PointerType::getUnqual(Type::getInt8Ty(Context)));
			    Value *Len = convertOperand(func, Type::getInt32Ty(Context), inst->u.three[2]);
			    CallInst *c = Builder.CreateCall4(CF->FMemmove, Dst, Src, Len,
								ConstantInt::get(Type::getInt32Ty(Context), 1));
			    c->setTailCall(true);
			    c->setDoesNotThrow();
			    break;
			}
			case OP_BC_MEMCMP:
			{
			    Value *Dst = convertOperand(func, inst, inst->u.three[0]);
			    Dst = Builder.CreatePointerCast(Dst, PointerType::getUnqual(Type::getInt8Ty(Context)));
			    Value *Src = convertOperand(func, inst, inst->u.three[1]);
			    Src = Builder.CreatePointerCast(Src, PointerType::getUnqual(Type::getInt8Ty(Context)));
			    Value *Len = convertOperand(func, EE->getTargetData()->getIntPtrType(Context), inst->u.three[2]);
			    CallInst *c = Builder.CreateCall3(CF->FRealmemcmp, Dst, Src, Len);
			    c->setTailCall(true);
			    c->setDoesNotThrow();
			    Store(inst->dest, c);
			    break;
			}
			case OP_BC_ISBIGENDIAN:
			    Store(inst->dest, WORDS_BIGENDIAN ?
				  ConstantInt::getTrue(Context) :
				  ConstantInt::getFalse(Context));
			    break;
			case OP_BC_ABORT:
			    if (!unreachable) {
				CallInst *CI = Builder.CreateCall(CF->FHandler);
				CI->setDoesNotReturn();
				CI->setDoesNotThrow();
				Builder.CreateUnreachable();
				unreachable = true;
			    }
			    break;
			case OP_BC_BSWAP16:
			    {
				CallInst *C = Builder.CreateCall(CF->FBSwap16, convertOperand(func, inst, inst->u.unaryop));
				C->setTailCall(true);
				C->setDoesNotThrow(true);
				Store(inst->dest, C);
				break;
			    }
			case OP_BC_BSWAP32:
			    {
				CallInst *C = Builder.CreateCall(CF->FBSwap32, convertOperand(func, inst, inst->u.unaryop));
				C->setTailCall(true);
				C->setDoesNotThrow(true);
				Store(inst->dest, C);
				break;
			    }
			case OP_BC_BSWAP64:
			    {
				CallInst *C = Builder.CreateCall(CF->FBSwap64, convertOperand(func, inst, inst->u.unaryop));
				C->setTailCall(true);
				C->setDoesNotThrow(true);
				Store(inst->dest, C);
				break;
			    }
			case OP_BC_PTRDIFF32:
			    {
				Value *P1 = convertOperand(func, inst, inst->u.binop[0]);
				Value *P2 = convertOperand(func, inst, inst->u.binop[1]);
				P1 = Builder.CreatePtrToInt(P1, Type::getInt64Ty(Context));
				P2 = Builder.CreatePtrToInt(P2, Type::getInt64Ty(Context));
				Value *R = Builder.CreateSub(P1, P2);
				R = Builder.CreateTrunc(R, Type::getInt32Ty(Context));
				Store(inst->dest, R);
				break;
			    }
			case OP_BC_PTRTOINT64:
			    {
				Value *P1 = convertOperand(func, inst, inst->u.unaryop);
				P1 = Builder.CreatePtrToInt(P1, Type::getInt64Ty(Context));
				Store(inst->dest, P1);
				break;
			    }
			default:
			    errs() << MODULE << "JIT doesn't implement opcode " <<
				inst->opcode << " yet!\n";
			    return false;
		    }
		}
	    }

	    if (verifyFunction(*F, PrintMessageAction)) {
		errs() << MODULE << "Verification failed\n";
		F->dump();
		// verification failed
		return false;
	    }
	    delete [] Values;
	    delete [] BB;
	}

	PM.run(*M);
	for (unsigned j=0;j<bc->num_func;j++) {
	    PrettyStackTraceString CrashInfo("Generate LLVM IR2");
	    Function *F = Functions[j];
	    AddStackProtect(F);
	}
	DEBUG(M->dump());
	delete TypeMap;
	std::vector<const Type*> args;
	args.clear();
	args.push_back(HiddenCtx);
	FunctionType *Callable = FunctionType::get(Type::getInt32Ty(Context),
						   args, false);
	for (unsigned j=0;j<bc->num_func;j++) {
	    const struct cli_bc_func *func = &bc->funcs[j];
	    PrettyStackTraceString CrashInfo2("Native machine codegen");

	    // If prototype matches, add to callable functions
	    if (Functions[j]->getFunctionType() == Callable) {
		// All functions have the Fast calling convention, however
		// entrypoint can only be C, emit wrapper
		Function *F = Function::Create(Functions[j]->getFunctionType(),
					       Function::ExternalLinkage,
					       Functions[j]->getName()+"_wrap", M);
		F->setDoesNotThrow();
		BasicBlock *BB = BasicBlock::Create(Context, "", F);
		std::vector<Value*> args;
		for (Function::arg_iterator J=F->arg_begin(),
		     JE=F->arg_end(); J != JE; ++JE) {
		    args.push_back(&*J);
		}
		CallInst *CI = CallInst::Create(Functions[j], args.begin(), args.end(), "", BB);
		CI->setCallingConv(CallingConv::Fast);
		ReturnInst::Create(Context, CI, BB);

		if (verifyFunction(*F, PrintMessageAction) == 0) {
			DEBUG(errs() << "Generating code\n");
			// Codegen current function as executable machine code.
			EE->getPointerToFunction(Functions[j]);
			void *code = EE->getPointerToFunction(F);
			DEBUG(errs() << "Code generation finished\n");

			compiledFunctions[func] = code;
		}
	  }
	}
	delete [] Functions;
	return true;
    }
};

static sys::Mutex llvm_api_lock;

// This class automatically acquires the lock when instantiated,
// and releases the lock when leaving scope.
class LLVMApiScopedLock {
    public:
	// when multithreaded mode is false (no atomics available),
	// we need to wrap all LLVM API calls with a giant mutex lock, but
	// only then.
	LLVMApiScopedLock() {
	    if (!llvm_is_multithreaded())
		llvm_api_lock.acquire();
	}
	~LLVMApiScopedLock() {
	    if (!llvm_is_multithreaded())
		llvm_api_lock.release();
	}
};

static void addFunctionProtos(struct CommonFunctions *CF, ExecutionEngine *EE, Module *M)
{
    LLVMContext &Context = M->getContext();
    FunctionType *FTy = FunctionType::get(Type::getVoidTy(Context),
					  false);
    CF->FHandler = Function::Create(FTy, Function::ExternalLinkage,
					  "clamjit.fail", M);
    CF->FHandler->setDoesNotReturn();
    CF->FHandler->setDoesNotThrow();
    CF->FHandler->addFnAttr(Attribute::NoInline);

    EE->addGlobalMapping(CF->FHandler, (void*)(intptr_t)jit_exception_handler);
    EE->InstallLazyFunctionCreator(noUnknownFunctions);
    EE->getPointerToFunction(CF->FHandler);

    std::vector<const Type*> args;
    args.push_back(PointerType::getUnqual(Type::getInt8Ty(Context)));
    args.push_back(Type::getInt8Ty(Context));
    args.push_back(Type::getInt32Ty(Context));
    args.push_back(Type::getInt32Ty(Context));
    FunctionType* FuncTy_3 = FunctionType::get(Type::getVoidTy(Context),
					       args, false);
    CF->FMemset = Function::Create(FuncTy_3, GlobalValue::ExternalLinkage,
					 "llvm.memset.i32", M);
    CF->FMemset->setDoesNotThrow();
    CF->FMemset->setDoesNotCapture(1, true);

    args.clear();
    args.push_back(PointerType::getUnqual(Type::getInt8Ty(Context)));
    args.push_back(PointerType::getUnqual(Type::getInt8Ty(Context)));
    args.push_back(Type::getInt32Ty(Context));
    args.push_back(Type::getInt32Ty(Context));
    FunctionType* FuncTy_4 = FunctionType::get(Type::getVoidTy(Context),
					       args, false);
    CF->FMemmove = Function::Create(FuncTy_4, GlobalValue::ExternalLinkage,
					  "llvm.memmove.i32", M);
    CF->FMemmove->setDoesNotThrow();
    CF->FMemmove->setDoesNotCapture(1, true);

    CF->FMemcpy = Function::Create(FuncTy_4, GlobalValue::ExternalLinkage,
					 "llvm.memcpy.i32", M);
    CF->FMemcpy->setDoesNotThrow();
    CF->FMemcpy->setDoesNotCapture(1, true);

    args.clear();
    args.push_back(Type::getInt16Ty(Context));
    FunctionType *FuncTy_5 = FunctionType::get(Type::getInt16Ty(Context), args, false);
    CF->FBSwap16 = Function::Create(FuncTy_5, GlobalValue::ExternalLinkage,
					  "llvm.bswap.i16", M);
    CF->FBSwap16->setDoesNotThrow();

    args.clear();
    args.push_back(Type::getInt32Ty(Context));
    FunctionType *FuncTy_6 = FunctionType::get(Type::getInt32Ty(Context), args, false);
    CF->FBSwap32 = Function::Create(FuncTy_6, GlobalValue::ExternalLinkage,
					  "llvm.bswap.i32", M);
    CF->FBSwap32->setDoesNotThrow();

    args.clear();
    args.push_back(Type::getInt64Ty(Context));
    FunctionType *FuncTy_7 = FunctionType::get(Type::getInt64Ty(Context), args, false);
    CF->FBSwap64 = Function::Create(FuncTy_7, GlobalValue::ExternalLinkage,
					  "llvm.bswap.i64", M);
    CF->FBSwap64->setDoesNotThrow();

    FunctionType* DummyTy = FunctionType::get(Type::getVoidTy(Context), false);
    CF->FRealmemset = Function::Create(DummyTy, GlobalValue::ExternalLinkage,
					     "memset", M);
    EE->addGlobalMapping(CF->FRealmemset, (void*)(intptr_t)memset);
    EE->getPointerToFunction(CF->FRealmemset);
    CF->FRealMemmove = Function::Create(DummyTy, GlobalValue::ExternalLinkage,
					      "memmove", M);
    EE->addGlobalMapping(CF->FRealMemmove, (void*)(intptr_t)memmove);
    EE->getPointerToFunction(CF->FRealMemmove);
    CF->FRealmemcpy = Function::Create(DummyTy, GlobalValue::ExternalLinkage,
					     "memcpy", M);
    EE->addGlobalMapping(CF->FRealmemcpy, (void*)(intptr_t)memcpy);
    EE->getPointerToFunction(CF->FRealmemcpy);

    args.clear();
    args.push_back(PointerType::getUnqual(Type::getInt8Ty(Context)));
    args.push_back(PointerType::getUnqual(Type::getInt8Ty(Context)));
    args.push_back(EE->getTargetData()->getIntPtrType(Context));
    FuncTy_5 = FunctionType::get(Type::getInt32Ty(Context),
				 args, false);
    CF->FRealmemcmp = Function::Create(FuncTy_5, GlobalValue::ExternalLinkage, "memcmp", M);
    EE->addGlobalMapping(CF->FRealmemcmp, (void*)(intptr_t)memcmp);
    EE->getPointerToFunction(CF->FRealmemcmp);
}

}

struct bc_watchdog {
    volatile uint8_t* timeout;
    struct timespec * abstimeout;
    pthread_mutex_t   mutex;
    pthread_cond_t    cond;
    int finished;
};

static void *bytecode_watchdog(void *arg)
{
    int ret = 0;
    struct bc_watchdog *w = (struct bc_watchdog*)arg;
    pthread_mutex_lock(&w->mutex);
    while (!w->finished && ret != ETIMEDOUT) {
	ret = pthread_cond_timedwait(&w->cond, &w->mutex, w->abstimeout);
    }
    pthread_mutex_unlock(&w->mutex);
    if (ret == ETIMEDOUT) {
	*w->timeout = 1;
	errs() << "Bytecode run timed out, timeout flag set\n";
    }
    return NULL;
}

static int bytecode_execute(intptr_t code, struct cli_bc_ctx *ctx)
{
    jmp_buf env;
    // execute;
    if (setjmp(env) == 0) {
	// setup exception handler to longjmp back here
	ExceptionReturn.set((const jmp_buf*)&env);
	uint32_t result = ((uint32_t (*)(struct cli_bc_ctx *))(intptr_t)code)(ctx);
	*(uint32_t*)ctx->values = result;
	return 0;
    }
    errs() << "\n";
    errs().changeColor(raw_ostream::RED, true) << MODULE
	<< "*** JITed code intercepted runtime error!\n";
    errs().resetColor();
    return CL_EBYTECODE;
}

extern "C" const char *cli_strerror(int errnum, char* buf, size_t len);
int cli_vm_execute_jit(const struct cli_all_bc *bcs, struct cli_bc_ctx *ctx,
		       const struct cli_bc_func *func)
{
    char buf[1024];
    int ret;
    pthread_t thread;
    struct timeval tv0, tv1;
    uint32_t timeoutus;
    // no locks needed here, since LLVM automatically acquires a JIT lock
    // if needed.
    void *code = bcs->engine->compiledFunctions[func];
    if (!code) {
	errs() << MODULE << "Unable to find compiled function\n";
	if (func->numArgs)
	    errs() << MODULE << "Function has "
		<< (unsigned)func->numArgs << " arguments, it must have 0 to be called as entrypoint\n";
	return CL_EBYTECODE;
    }
    gettimeofday(&tv0, NULL);
    struct timespec abstime;

    timeoutus = (ctx->bytecode_timeout%1000)*1000 + tv0.tv_usec;
    abstime.tv_sec = tv0.tv_sec + ctx->bytecode_timeout/1000 + timeoutus/1000000;
    abstime.tv_nsec = 1000*(timeoutus%1000000);
    ctx->timeout = 0;

    struct bc_watchdog w = {
	&ctx->timeout,
	&abstime,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_COND_INITIALIZER,
	0
    };

    if ((ret = pthread_create(&thread, NULL, bytecode_watchdog, &w))) {
	errs() << "Bytecode: failed to create new thread!";
	errs() << cli_strerror(ret, buf, sizeof(buf));
	errs() << "\n";
	return CL_EBYTECODE;
    }

    ret = bytecode_execute((intptr_t)code, ctx);
    pthread_mutex_lock(&w.mutex);
    w.finished = 1;
    pthread_cond_signal(&w.cond);
    pthread_mutex_unlock(&w.mutex);
    pthread_join(thread, NULL);

    if (cli_debug_flag) {
	gettimeofday(&tv1, NULL);
	tv1.tv_sec -= tv0.tv_sec;
	tv1.tv_usec -= tv0.tv_usec;
	errs() << "bytecode finished in " << (tv1.tv_sec*1000000 + tv1.tv_usec) << "us\n";
    }
    return ctx->timeout ? CL_ETIMEOUT : ret;
}

static unsigned char name_salt[16] = { 16, 38, 97, 12, 8, 4, 72, 196, 217, 144, 33, 124, 18, 11, 17, 253 };
static void setGuard(unsigned char* guardbuf)
{
    cli_md5_ctx ctx;
    char salt[48];
    memcpy(salt, name_salt, 16);
    for(unsigned i = 16; i < 48; i++)
	salt[i] = cli_rndnum(255);

    cli_md5_init(&ctx);
    cli_md5_update(&ctx, salt, 48);
    cli_md5_final(guardbuf, &ctx);
}

int cli_bytecode_prepare_jit(struct cli_all_bc *bcs)
{
  if (!bcs->engine)
      return CL_EBYTECODE;
  jmp_buf env;
  LLVMApiScopedLock scopedLock;
  // setup exception handler to longjmp back here
  ExceptionReturn.set((const jmp_buf*)&env);
  if (setjmp(env) != 0) {
      errs() << "\n";
      errs().changeColor(raw_ostream::RED, true) << MODULE 
      << "*** FATAL error encountered during bytecode generation\n";
      errs().resetColor();
      return CL_EBYTECODE;
  }
  // LLVM itself never throws exceptions, but operator new may throw bad_alloc
  try {
    Module *M = new Module("ClamAV jit module", bcs->engine->Context);
    {
	// Create the JIT.
	std::string ErrorMsg;
	EngineBuilder builder(M);
	builder.setErrorStr(&ErrorMsg);
	builder.setEngineKind(EngineKind::JIT);
	builder.setOptLevel(CodeGenOpt::Default);
	ExecutionEngine *EE = bcs->engine->EE = builder.create();
	if (!EE) {
	    if (!ErrorMsg.empty())
		errs() << MODULE << "error creating execution engine: " << ErrorMsg << "\n";
	    else
		errs() << MODULE << "JIT not registered?\n";
	    return CL_EBYTECODE;
	}
	bcs->engine->Listener  = new NotifyListener();
	EE->RegisterJITEventListener(bcs->engine->Listener);
//	EE->RegisterJITEventListener(createOProfileJITEventListener());
	// Due to LLVM PR4816 only X86 supports non-lazy compilation, disable
	// for now.
	EE->DisableLazyCompilation();
	EE->DisableSymbolSearching();

	struct CommonFunctions CF;
	addFunctionProtos(&CF, EE, M);

	PassManager OurFPM;
	M->setDataLayout(EE->getTargetData()->getStringRepresentation());
	M->setTargetTriple(sys::getHostTriple());
	// Set up the optimizer pipeline.  Start with registering info about how
	// the target lays out data structures.
	OurFPM.add(new TargetData(*EE->getTargetData()));
	// Promote allocas to registers.
	OurFPM.add(createPromoteMemoryToRegisterPass());
	OurFPM.add(createDeadCodeEliminationPass());

	//TODO: create a wrapper that calls pthread_getspecific
	unsigned maxh = cli_globals[0].offset + sizeof(struct cli_bc_hooks);
	const Type *HiddenCtx = PointerType::getUnqual(ArrayType::get(Type::getInt8Ty(bcs->engine->Context), maxh));

	LLVMTypeMapper apiMap(bcs->engine->Context, cli_apicall_types, cli_apicall_maxtypes, HiddenCtx);
	Function **apiFuncs = new Function *[cli_apicall_maxapi];
	for (unsigned i=0;i<cli_apicall_maxapi;i++) {
	    const struct cli_apicall *api = &cli_apicalls[i];
	    const FunctionType *FTy = cast<FunctionType>(apiMap.get(69+api->type));
	    Function *F = Function::Create(FTy, Function::ExternalLinkage,
					   api->name, M);
	    void *dest;
	    switch (api->kind) {
		case 0:
		    dest = (void*)(intptr_t)cli_apicalls0[api->idx];
		    break;
		case 1:
		    dest = (void*)(intptr_t)cli_apicalls1[api->idx];
		    break;
		case 2:
		    dest = (void*)(intptr_t)cli_apicalls2[api->idx];
		    break;
		case 3:
		    dest = (void*)(intptr_t)cli_apicalls3[api->idx];
		    break;
		case 4:
		    dest = (void*)(intptr_t)cli_apicalls4[api->idx];
		    break;
		case 5:
		    dest = (void*)(intptr_t)cli_apicalls5[api->idx];
		    break;
		case 6:
		    dest = (void*)(intptr_t)cli_apicalls6[api->idx];
		    break;
		case 7:
		    dest = (void*)(intptr_t)cli_apicalls7[api->idx];
		    break;
		case 8:
		    dest = (void*)(intptr_t)cli_apicalls8[api->idx];
		    break;
		case 9:
		    dest = (void*)(intptr_t)cli_apicalls9[api->idx];
		    break;
		default:
		    llvm_unreachable("invalid api type");
	    }
	    if (!dest) {
		std::string reason((Twine("No mapping for builtin api ")+api->name).str());
		llvm_error_handler(0, reason);
	    }
	    EE->addGlobalMapping(F, dest);
	    EE->getPointerToFunction(F);
	    apiFuncs[i] = F;
	}

	// stack protector
	FunctionType *FTy = FunctionType::get(Type::getVoidTy(M->getContext()),
						    false);
	GlobalVariable *Guard = new GlobalVariable(*M, PointerType::getUnqual(Type::getInt8Ty(M->getContext())),
						    true, GlobalValue::ExternalLinkage, 0, "__stack_chk_guard"); 
	unsigned plus = 0;
	if (2*sizeof(void*) <= 16 && cli_rndnum(2)==2) {
	    plus = sizeof(void*);
	}
	EE->addGlobalMapping(Guard, (void*)(&bcs->engine->guard.b[plus]));
	setGuard(bcs->engine->guard.b);
	bcs->engine->guard.b[plus+sizeof(void*)-1] = 0x00;
//	printf("%p\n", *(void**)(&bcs->engine->guard.b[plus]));
	Function *SFail = Function::Create(FTy, Function::ExternalLinkage,
					      "__stack_chk_fail", M);
	EE->addGlobalMapping(SFail, (void*)(intptr_t)jit_ssp_handler);
        EE->getPointerToFunction(SFail);

	for (unsigned i=0;i<bcs->count;i++) {
	    const struct cli_bc *bc = &bcs->all_bcs[i];
	    if (bc->state == bc_skip)
		continue;
	    LLVMCodegen Codegen(bc, M, &CF, bcs->engine->compiledFunctions, EE,
				OurFPM, apiFuncs, apiMap);
	    if (!Codegen.generate()) {
		errs() << MODULE << "JIT codegen failed\n";
		return CL_EBYTECODE;
	    }
	}

	for (unsigned i=0;i<bcs->count;i++) {
	    bcs->all_bcs[i].state = bc_jit;
	}
	// compile all functions now, not lazily!
	for (Module::iterator I = M->begin(), E = M->end(); I != E; ++I) {
	    Function *Fn = &*I;
	    if (!Fn->isDeclaration())
		EE->getPointerToFunction(Fn);
	}
	delete [] apiFuncs;
    }
    return -1;
  } catch (std::bad_alloc &badalloc) {
      errs() << MODULE << badalloc.what() << "\n";
      return CL_EMEM;
  } catch (...) {
      errs() << MODULE << "Unexpected unknown exception occurred.\n";
      return CL_EBYTECODE;
  }
}

int bytecode_init(void)
{
    // If already initialized return
    if (llvm_is_multithreaded()) {
	errs() << "bytecode_init: already initialized";
	return CL_EARG;
    }
    llvm_install_error_handler(llvm_error_handler);
#ifdef CL_DEBUG
    sys::PrintStackTraceOnErrorSignal();
#else
    llvm::DisablePrettyStackTrace = true;
#endif
    atexit(do_shutdown);

#ifdef CL_DEBUG
    //disable this for now, it leaks
    llvm::JITEmitDebugInfo = false;
//    llvm::JITEmitDebugInfo = true;
#else
    llvm::JITEmitDebugInfo = false;
#endif
    llvm::DwarfExceptionHandling = false;
    llvm_start_multithreaded();

    // If we have a native target, initialize it to ensure it is linked in and
    // usable by the JIT.
    InitializeNativeTarget();

    if (!llvm_is_multithreaded()) {
	//TODO:cli_dbgmsg
	DEBUG(errs() << "WARNING: ClamAV JIT built w/o atomic builtins\n"
	      << "On x86 for best performance ClamAV should be built for i686, not i386!\n");
    }
    return 0;
}

// Called once when loading a new set of BC files
int cli_bytecode_init_jit(struct cli_all_bc *bcs, unsigned dconfmask)
{
    LLVMApiScopedLock scopedLock;
    bcs->engine = NULL;
    Triple triple(sys::getHostTriple());
    if (cli_debug_flag)
	errs() << "host triple is: " << sys::getHostTriple() << "\n";
    enum Triple::ArchType arch = triple.getArch();
    switch (arch) {
	case Triple::arm:
	    if (!(dconfmask & BYTECODE_JIT_ARM)) {
		if (cli_debug_flag)
		    errs() << "host triple is: " << sys::getHostTriple() << "\n";
		return 0;
	    }
	    break;
	case Triple::ppc:
	case Triple::ppc64:
	    if (!(dconfmask & BYTECODE_JIT_PPC)) {
		if (cli_debug_flag)
		    errs() << "JIT disabled for ppc\n";
		return 0;
	    }
	    break;
	case Triple::x86:
	case Triple::x86_64:
	    if (!(dconfmask & BYTECODE_JIT_X86)) {
		if (cli_debug_flag)
		    errs() << "JIT disabled for x86\n";
		return 0;
	    }
	    break;
	default:
	    errs() << "Not supported architecture for " << triple.str() << "\n";
	    return CL_EBYTECODE;
    }

    std::string cpu = sys::getHostCPUName();
    if (cli_debug_flag)
	errs() << "host cpu is: " << cpu << "\n";
    if (!cpu.compare("i386") ||
	!cpu.compare("i486")) {
	bcs->engine = 0;
	DEBUG(errs() << "i[34]86 detected, falling back to interpreter (JIT needs pentium or better\n");
	/* i386 and i486 has to fallback to interpreter */
	return 0;
    }
    std::string ErrMsg;
    sys::MemoryBlock B = sys::Memory::AllocateRWX(4096, NULL, &ErrMsg);
    if (B.base() == 0) {
	errs() << MODULE << ErrMsg << "\n";
#ifdef __linux__
	errs() << MODULE << "SELinux is preventing 'execmem' access. Run 'setsebool -P clamd_use_jit on' to allow access\n";
#endif
	errs() << MODULE << "falling back to interpreter mode\n";
	return 0;
    } else {
	sys::Memory::ReleaseRWX(B);
    }

    bcs->engine = new(std::nothrow) cli_bcengine;
    if (!bcs->engine)
	return CL_EMEM;
    bcs->engine->EE = 0;
    bcs->engine->Listener = 0;
    return 0;
}

int cli_bytecode_done_jit(struct cli_all_bc *bcs)
{
    LLVMApiScopedLock scopedLock;
    if (bcs->engine) {
	if (bcs->engine->EE) {
	    if (bcs->engine->Listener)
		bcs->engine->EE->UnregisterJITEventListener(bcs->engine->Listener);
	    delete bcs->engine->EE;
	}
	delete bcs->engine->Listener;
	delete bcs->engine;
	bcs->engine = 0;
    }
    return 0;
}

void cli_bytecode_debug(int argc, char **argv)
{
  cl::ParseCommandLineOptions(argc, argv);
}

typedef struct lines {
    MemoryBuffer *buffer;
    std::vector<const char*> linev;
} linesTy;

static struct lineprinter {
    StringMap<linesTy*> files;
} LinePrinter;

void cli_bytecode_debug_printsrc(const struct cli_bc_ctx *ctx)
{
    if (!ctx->file || !ctx->directory || !ctx->line) {
	errs() << (ctx->directory ? "d":"null") << ":" << (ctx->file ? "f" : "null")<< ":" << ctx->line << "\n";
	return;
    }
    // acquire a mutex here
    sys::Mutex mtx(false);
    sys::SmartScopedLock<false> lock(mtx);

    std::string path = std::string(ctx->directory) + "/" + std::string(ctx->file);
    StringMap<linesTy*>::iterator I = LinePrinter.files.find(path);
    linesTy *lines;
    if (I == LinePrinter.files.end()) {
	lines = new linesTy;
	std::string ErrorMessage;
	lines->buffer = MemoryBuffer::getFile(path, &ErrorMessage);
	if (!lines->buffer) {
	    errs() << "Unable to open file '" << path << "'\n";
	    return ;
	}
	LinePrinter.files[path] = lines;
    } else {
	lines = I->getValue();
    }
    while (lines->linev.size() <= ctx->line+1) {
	const char *p;
	if (lines->linev.empty()) {
	    p = lines->buffer->getBufferStart();
	    lines->linev.push_back(p);
	} else {
	    p = lines->linev.back();
	    if (p == lines->buffer->getBufferEnd())
		break;
	    p = strchr(p, '\n');
	    if (!p) {
		p = lines->buffer->getBufferEnd();
		lines->linev.push_back(p);
	    } else
		lines->linev.push_back(p+1);
	}
    }
    if (ctx->line >= lines->linev.size()) {
	errs() << "Line number " << ctx->line << "out of file\n";
	return;
    }
    assert(ctx->line < lines->linev.size());

    int line = (int)ctx->line ? (int)ctx->line : -1;
    int col = (int)ctx->col ? (int)ctx->col : -1;
    SMDiagnostic diag(ctx->file, line, col,
		 "", std::string(lines->linev[ctx->line-1], lines->linev[ctx->line]-1));
    diag.Print("[trace]", errs());
}

int have_clamjit=1;
void cli_bytecode_printversion()
{
  cl::PrintVersionMessage();
}

void cli_printcxxver()
{
    /* Try to print information about some commonly used compilers */
#ifdef __GNUC__
    printf("GNU C++: %s (%u.%u.%u)\n", __VERSION__, __GNUC__, __GNUC_MINOR__,
	   __GNUC_PATCHLEVEL__);
#endif
#ifdef __INTEL_COMPILER
    printf("Intel Compiler C++ %u\n", __INTEL_COMPILER);
#endif
#ifdef _MSC_VER
    printf("Microsoft Visual C++ %u\n", _MSC_VER);
#endif
}

namespace ClamBCModule {
void stop(const char *msg, llvm::Function* F, llvm::Instruction* I)
{
    llvm::errs() << msg << "\n";
}
}

void printValue(llvm::Value *V, bool a, bool b) {
    std::string DisplayName;
    std::string Type;
    unsigned Line;
    std::string File;
    std::string Dir;
    if (!getLocationInfo(V, DisplayName, Type, Line, File, Dir)) {
	errs() << *V << "\n";
	return;
    }
    errs() << "'" << DisplayName << "' (" << File << ":" << Line << ")";
}

void printLocation(llvm::Instruction *I, bool a, bool b) {
    if (MDNode *N = I->getMetadata("dbg")) {
	DILocation Loc(N);
	errs() << Loc.getFilename() << ":" << Loc.getLineNumber();
	if (unsigned Col = Loc.getColumnNumber()) {
  	    errs() << ":" << Col;
  	}
  	errs() << ": ";
  	return;
    }
    errs() << *I << ":\n";
}
