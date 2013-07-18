//===--- GenPoly.h - Swift IR generation for polymorphism -------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
//
//  This file provides the private interface to the code for translating
//  between polymorphic and monomorphic values.
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_IRGEN_GENPOLY_H
#define SWIFT_IRGEN_GENPOLY_H

#include "llvm/ADT/DenseMap.h"

namespace llvm {
  class Type;
  template <class T> class ArrayRef;
}

namespace swift {
  class CanType;
  class Substitution;
  
namespace Mangle {
  enum class ExplosionKind : unsigned;
}

namespace irgen {
  class Explosion;
  class IRGenFunction;
  class IRGenModule;
  
  /// A struct to hold type metadata and witnesses from an existential
  /// container used to fulfill an archetype substitution.
  struct ExistentialSubstitution {
    CanType protocolType;
    llvm::Value *metadata;
    std::vector<llvm::Value *> witnesses;
  };
  
  using ExistentialSubstitutionMap
    = llvm::DenseMap<ArchetypeType*, ExistentialSubstitution>;

  /// Do the given types differ by abstraction when laid out as memory?
  bool differsByAbstractionInMemory(IRGenModule &IGM,
                                    CanType origTy, CanType substTy);

  /// Do the given types differ by abstraction when passed in an explosion?
  bool differsByAbstractionInExplosion(IRGenModule &IGM,
                                       CanType origTy, CanType substTy,
                                       Mangle::ExplosionKind explosionLevel);

  /// Do the given types differ by abstraction when used as a function?
  ///
  /// Note that origTy and substTy must be canonical types.
  bool differsByAbstractionAsFunction(IRGenModule &IGM,
                                      AnyFunctionType *origTy,
                                      AnyFunctionType *substTy,
                                      Mangle::ExplosionKind explosionLevel,
                                      unsigned uncurryLevel);

  /// Given a substituted explosion, re-emit it as an unsubstituted one.
  ///
  /// For example, given an explosion which begins with the
  /// representation of an (Int, Float), consume that and produce the
  /// representation of an (Int, T).
  ///
  /// The substitutions must carry origTy to substTy. If existential type
  /// variables are used to substitute for archetypes, the runtime type and
  /// witness information from the existential will be added to the
  /// ExistentialSubstitutionMap given.
  void reemitAsUnsubstituted(IRGenFunction &IGF,
                             CanType origTy, CanType substTy,
                             ArrayRef<Substitution> subs,
                             Explosion &src, Explosion &dest,
                             ExistentialSubstitutionMap &existentialSubs);

  /// Given an unsubstituted explosion, re-emit it as a substituted one.
  ///
  /// For example, given an explosion which begins with the
  /// representation of an (Int, T), consume that and produce the
  /// representation of an (Int, Float).
  ///
  /// The substitutions must carry origTy to substTy.
  void reemitAsSubstituted(IRGenFunction &IGF,
                           CanType origTy, CanType substTy,
                           ArrayRef<Substitution> subs,
                           Explosion &src, Explosion &dest);

} // end namespace irgen
} // end namespace swift

#endif
