// SPDX-License-Identifier: Apache-2.0
// SPDX-FileCopyrightText: 2019-2022 Second State INC

//===-- wasmedge/ast/type.h - type class definitions ----------------------===//
//
// Part of the WasmEdge Project.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the type classes: Limit, FunctionType,
/// MemoryType, TableType, and GlobalType.
///
//===----------------------------------------------------------------------===//
#pragma once

#include "common/executable.h"
#include "common/span.h"
#include "common/symbol.h"
#include "common/types.h"

#include <vector>

namespace WasmEdge {
namespace AST {

/// AST Limit node.
class Limit {
public:
  /// Limit type enumeration class.
  enum class LimitType : uint8_t {
    HasMin = 0x00,
    HasMinMax = 0x01,
    SharedNoMax = 0x02,
    Shared = 0x03
  };

  /// Constructors.
  Limit() noexcept : Type(LimitType::HasMin), Min(0U), Max(0U) {}
  Limit(uint32_t MinVal) noexcept
      : Type(LimitType::HasMin), Min(MinVal), Max(MinVal) {}
  Limit(uint32_t MinVal, uint32_t MaxVal, bool Shared = false) noexcept
      : Min(MinVal), Max(MaxVal) {
    if (Shared) {
      Type = LimitType::Shared;
    } else {
      Type = LimitType::HasMinMax;
    }
  }

  /// Getter and setter of limit mode.
  bool hasMax() const noexcept {
    return Type == LimitType::HasMinMax || Type == LimitType::Shared;
  }
  bool isShared() const noexcept { return Type == LimitType::Shared; }
  void setType(LimitType TargetType) noexcept { Type = TargetType; }

  /// Getter and setter of min value.
  uint32_t getMin() const noexcept { return Min; }
  void setMin(uint32_t Val) noexcept { Min = Val; }

  /// Getter and setter of max value.
  uint32_t getMax() const noexcept { return Max; }
  void setMax(uint32_t Val) noexcept { Max = Val; }

private:
  /// \name Data of Limit.
  /// @{
  LimitType Type;
  uint32_t Min;
  uint32_t Max;
  /// @}
};

/// AST FunctionType node.
class FunctionType {
public:
  /// Constructors.
  FunctionType() = default;
  FunctionType(Span<const ValType> P, Span<const ValType> R)
      : ParamTypes(P.begin(), P.end()), ReturnTypes(R.begin(), R.end()) {}
  FunctionType(Span<const ValType> P, Span<const ValType> R,
               Symbol<Executable::Wrapper> S)
      : ParamTypes(P.begin(), P.end()), ReturnTypes(R.begin(), R.end()),
        WrapSymbol(std::move(S)) {}

  /// `==` and `!=` operator overloadings.
  friend bool operator==(const FunctionType &LHS,
                         const FunctionType &RHS) noexcept {
    return LHS.ParamTypes == RHS.ParamTypes &&
           LHS.ReturnTypes == RHS.ReturnTypes;
  }

  friend bool operator!=(const FunctionType &LHS,
                         const FunctionType &RHS) noexcept {
    return !(LHS == RHS);
  }

  /// Getter of param types.
  const std::vector<ValType> &getParamTypes() const noexcept {
    return ParamTypes;
  }
  std::vector<ValType> &getParamTypes() noexcept { return ParamTypes; }

  /// Getter of return types.
  const std::vector<ValType> &getReturnTypes() const noexcept {
    return ReturnTypes;
  }
  std::vector<ValType> &getReturnTypes() noexcept { return ReturnTypes; }

  /// Getter and setter of symbol.
  const auto &getSymbol() const noexcept { return WrapSymbol; }
  void setSymbol(Symbol<Executable::Wrapper> S) noexcept {
    WrapSymbol = std::move(S);
  }

private:
  /// \name Data of FunctionType.
  /// @{
  std::vector<ValType> ParamTypes;
  std::vector<ValType> ReturnTypes;
  Symbol<Executable::Wrapper> WrapSymbol;
  /// @}
};

/// AST MemoryType node.
class MemoryType {
public:
  /// Constructors.
  MemoryType() noexcept = default;
  MemoryType(uint32_t MinVal) noexcept : Lim(MinVal) {}
  MemoryType(uint32_t MinVal, uint32_t MaxVal, bool Shared = false) noexcept
      : Lim(MinVal, MaxVal, Shared) {}
  MemoryType(const Limit &L) noexcept : Lim(L) {}

  /// Getter of limit.
  const Limit &getLimit() const noexcept { return Lim; }
  Limit &getLimit() noexcept { return Lim; }

private:
  /// \name Data of MemoryType.
  /// @{
  Limit Lim;
  /// @}
};

/// AST TableType node.
class TableType {
public:
  /// Constructors.
  TableType() noexcept : Type(TypeCode::FuncRef), Lim() {
    assuming(Type.isRefType());
  }
  TableType(const ValType &RType, uint32_t MinVal) noexcept
      : Type(RType), Lim(MinVal) {
    assuming(Type.isRefType());
  }
  TableType(const ValType &RType, uint32_t MinVal, uint32_t MaxVal) noexcept
      : Type(RType), Lim(MinVal, MaxVal) {
    assuming(Type.isRefType());
  }
  TableType(const ValType &RType, const Limit &L) noexcept
      : Type(RType), Lim(L) {
    assuming(Type.isRefType());
  }

  /// Getter of reference type.
  const ValType &getRefType() const noexcept { return Type; }
  void setRefType(const ValType &RType) noexcept {
    assuming(RType.isRefType());
    Type = RType;
  }

  /// Getter of limit.
  const Limit &getLimit() const noexcept { return Lim; }
  Limit &getLimit() noexcept { return Lim; }

private:
  /// \name Data of TableType.
  /// @{
  ValType Type;
  Limit Lim;
  /// @}
};

/// AST GlobalType node.
class GlobalType {
public:
  /// Constructors.
  GlobalType() noexcept : Type(TypeCode::I32), Mut(ValMut::Const) {}
  GlobalType(const ValType &VType, ValMut VMut) noexcept
      : Type(VType), Mut(VMut) {}

  /// Getter and setter of value type.
  const ValType &getValType() const noexcept { return Type; }
  void setValType(const ValType &VType) noexcept { Type = VType; }

  /// Getter and setter of value mutation.
  ValMut getValMut() const noexcept { return Mut; }
  void setValMut(ValMut VMut) noexcept { Mut = VMut; }

private:
  /// \name Data of GlobalType.
  /// @{
  ValType Type;
  ValMut Mut;
  /// @}
};

} // namespace AST
} // namespace WasmEdge
