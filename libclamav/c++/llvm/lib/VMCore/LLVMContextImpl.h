//===-- LLVMContextImpl.h - The LLVMContextImpl opaque class ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file declares LLVMContextImpl, the opaque implementation 
//  of LLVMContext.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LLVMCONTEXT_IMPL_H
#define LLVM_LLVMCONTEXT_IMPL_H

#include "ConstantsContext.h"
#include "LeaksContext.h"
#include "TypesContext.h"
#include "llvm/LLVMContext.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Metadata.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/Support/ValueHandle.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringMap.h"
#include <vector>

namespace llvm {

class ConstantInt;
class ConstantFP;
class LLVMContext;
class Type;
class Value;

struct DenseMapAPIntKeyInfo {
  struct KeyTy {
    APInt val;
    const Type* type;
    KeyTy(const APInt& V, const Type* Ty) : val(V), type(Ty) {}
    KeyTy(const KeyTy& that) : val(that.val), type(that.type) {}
    bool operator==(const KeyTy& that) const {
      return type == that.type && this->val == that.val;
    }
    bool operator!=(const KeyTy& that) const {
      return !this->operator==(that);
    }
  };
  static inline KeyTy getEmptyKey() { return KeyTy(APInt(1,0), 0); }
  static inline KeyTy getTombstoneKey() { return KeyTy(APInt(1,1), 0); }
  static unsigned getHashValue(const KeyTy &Key) {
    return DenseMapInfo<void*>::getHashValue(Key.type) ^ 
      Key.val.getHashValue();
  }
  static bool isEqual(const KeyTy &LHS, const KeyTy &RHS) {
    return LHS == RHS;
  }
};

struct DenseMapAPFloatKeyInfo {
  struct KeyTy {
    APFloat val;
    KeyTy(const APFloat& V) : val(V){}
    KeyTy(const KeyTy& that) : val(that.val) {}
    bool operator==(const KeyTy& that) const {
      return this->val.bitwiseIsEqual(that.val);
    }
    bool operator!=(const KeyTy& that) const {
      return !this->operator==(that);
    }
  };
  static inline KeyTy getEmptyKey() { 
    return KeyTy(APFloat(APFloat::Bogus,1));
  }
  static inline KeyTy getTombstoneKey() { 
    return KeyTy(APFloat(APFloat::Bogus,2)); 
  }
  static unsigned getHashValue(const KeyTy &Key) {
    return Key.val.getHashValue();
  }
  static bool isEqual(const KeyTy &LHS, const KeyTy &RHS) {
    return LHS == RHS;
  }
};

class LLVMContextImpl {
public:
  typedef DenseMap<DenseMapAPIntKeyInfo::KeyTy, ConstantInt*, 
                         DenseMapAPIntKeyInfo> IntMapTy;
  IntMapTy IntConstants;
  
  typedef DenseMap<DenseMapAPFloatKeyInfo::KeyTy, ConstantFP*, 
                         DenseMapAPFloatKeyInfo> FPMapTy;
  FPMapTy FPConstants;
  
  StringMap<MDString*> MDStringCache;
  
  FoldingSet<MDNode> MDNodeSet;
  // MDNodes may be uniqued or not uniqued.  When they're not uniqued, they
  // aren't in the MDNodeSet, but they're still shared between objects, so no
  // one object can destroy them.  This set allows us to at least destroy them
  // on Context destruction.
  SmallPtrSet<MDNode*, 1> NonUniquedMDNodes;
  
  ConstantUniqueMap<char, Type, ConstantAggregateZero> AggZeroConstants;

  typedef ConstantUniqueMap<std::vector<Constant*>, ArrayType,
    ConstantArray, true /*largekey*/> ArrayConstantsTy;
  ArrayConstantsTy ArrayConstants;
  
  typedef ConstantUniqueMap<std::vector<Constant*>, StructType,
    ConstantStruct, true /*largekey*/> StructConstantsTy;
  StructConstantsTy StructConstants;
  
  typedef ConstantUniqueMap<Constant*, UnionType, ConstantUnion>
      UnionConstantsTy;
  UnionConstantsTy UnionConstants;
  
  typedef ConstantUniqueMap<std::vector<Constant*>, VectorType,
                            ConstantVector> VectorConstantsTy;
  VectorConstantsTy VectorConstants;
  
  ConstantUniqueMap<char, PointerType, ConstantPointerNull> NullPtrConstants;
  
  ConstantUniqueMap<char, Type, UndefValue> UndefValueConstants;
  
  DenseMap<std::pair<Function*, BasicBlock*> , BlockAddress*> BlockAddresses;
  ConstantUniqueMap<ExprMapKeyType, Type, ConstantExpr> ExprConstants;

  ConstantUniqueMap<InlineAsmKeyType, PointerType, InlineAsm> InlineAsms;
  
  ConstantInt *TheTrueVal;
  ConstantInt *TheFalseVal;
  
  LeakDetectorImpl<Value> LLVMObjects;
  
  // Basic type instances.
  const Type VoidTy;
  const Type LabelTy;
  const Type FloatTy;
  const Type DoubleTy;
  const Type MetadataTy;
  const Type X86_FP80Ty;
  const Type FP128Ty;
  const Type PPC_FP128Ty;
  const IntegerType Int1Ty;
  const IntegerType Int8Ty;
  const IntegerType Int16Ty;
  const IntegerType Int32Ty;
  const IntegerType Int64Ty;

  // Concrete/Abstract TypeDescriptions - We lazily calculate type descriptions
  // for types as they are needed.  Because resolution of types must invalidate
  // all of the abstract type descriptions, we keep them in a seperate map to 
  // make this easy.
  TypePrinting ConcreteTypeDescriptions;
  TypePrinting AbstractTypeDescriptions;
  
  TypeMap<ArrayValType, ArrayType> ArrayTypes;
  TypeMap<VectorValType, VectorType> VectorTypes;
  TypeMap<PointerValType, PointerType> PointerTypes;
  TypeMap<FunctionValType, FunctionType> FunctionTypes;
  TypeMap<StructValType, StructType> StructTypes;
  TypeMap<UnionValType, UnionType> UnionTypes;
  TypeMap<IntegerValType, IntegerType> IntegerTypes;

  // Opaque types are not structurally uniqued, so don't use TypeMap.
  typedef SmallPtrSet<const OpaqueType*, 8> OpaqueTypesTy;
  OpaqueTypesTy OpaqueTypes;

  /// Used as an abstract type that will never be resolved.
  OpaqueType *const AlwaysOpaqueTy;


  /// ValueHandles - This map keeps track of all of the value handles that are
  /// watching a Value*.  The Value::HasValueHandle bit is used to know
  // whether or not a value has an entry in this map.
  typedef DenseMap<Value*, ValueHandleBase*> ValueHandlesTy;
  ValueHandlesTy ValueHandles;
  
  /// CustomMDKindNames - Map to hold the metadata string to ID mapping.
  StringMap<unsigned> CustomMDKindNames;
  
  typedef std::pair<unsigned, TrackingVH<MDNode> > MDPairTy;
  typedef SmallVector<MDPairTy, 2> MDMapTy;

  /// MetadataStore - Collection of per-instruction metadata used in this
  /// context.
  DenseMap<const Instruction *, MDMapTy> MetadataStore;
  
  LLVMContextImpl(LLVMContext &C);
  ~LLVMContextImpl();
};

}

#endif
