#pragma once

#include <ATen/ATen.h>
#include <ATen/core/ivalue.h>
#include <ATen/core/jit_type.h>
#include <ATen/core/stack.h>
#include <torch/csrc/WindowsTorchApiMacro.h>
#include <torch/csrc/jit/ir/ir.h>

#include <list>
#include <unordered_set>
#include <vector>

namespace torch {
namespace jit {

using ::c10::TensorTypePtr;

struct ProfilingRecord {
  // N.B. ProfilingRecord's copy and move c-tor are disabled, so we won't
  // end up accidentally copying or moving ProfilingRecords whose addresses
  // are captured in callbacks_
  ProfilingRecord(const ProfilingRecord&) = delete;
  ProfilingRecord(ProfilingRecord&&) noexcept = delete;
  TORCH_API static std::unique_ptr<ProfilingRecord> instrumentGraph(
      const std::shared_ptr<Graph>& graph);

  std::shared_ptr<Graph> profiled_graph_;
  std::mutex mutex_;
  size_t profiling_count_;
  std::unordered_set<Value*> seen_;
  bool ready() const {
    return profiling_count_ == 0;
  }
  std::shared_ptr<Graph> graph() const {
    return profiled_graph_;
  }

 private:
  ProfileOp* createProfileNode(
      const std::function<void(Stack&)>& fp,
      at::ArrayRef<Value*> inputs);
  void instrumentBlock(Block* block);
  void insertShapeProfile(Node* n, Value* i);
  ProfilingRecord(std::shared_ptr<Graph> g);
};

} // namespace jit
} // namespace torch
