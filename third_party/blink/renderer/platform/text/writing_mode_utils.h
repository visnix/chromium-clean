// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_WRITING_MODE_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_WRITING_MODE_UTILS_H_

#include "third_party/blink/renderer/platform/text/writing_direction_mode.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"

namespace blink {

// Templates to map values between logical orientations and physical
// orientations. See https://www.w3.org/TR/css-writing-modes-3/ and
// https://www.w3.org/TR/css-logical-1/ for definitions of logical orientations.

// This file provides two types of templates:
//
// - Simple value mappers (PhysicalToLogical and LogicalToPhysical): they take
//   4 input values in physical or logical orientations, and provide accessors
//   to get values in logical or physical orientations. As the inputs may be
//   evaluated even if not used (in case that the compiler is unable to remove
//   unused evaluations, e.g. containing non-inlined function calls), for
//   performance-senstive code, the evaluation of the inputs should be simple
//   and/or be fully inlined.
//
// - Value mappers based on getter/setter methods (PhysicalToLogicalGetter,
//   LogicalToPhysicalGetter, PhysicalToLogicalSetter and
//   LogicalToPhysicalSetter): they take 4 method pointers as inputs pointing to
//   methods accessing values in physical or logical orientations, and provide
//   accessors to get or set values in logical or physical orientations. They
//   are suitable for mapping of setters, or getters implemented with non-
//   inlined functions. Evaluation of the input values are delayed when they are
//   actually needed.
//
// See WritingModeUtilsTest.cpp, LayoutBoxModelObject.h and ComputedStyle.h for
// examples.

template <typename Value>
class PhysicalToLogical {
  STACK_ALLOCATED();

 public:
  PhysicalToLogical(WritingDirectionMode writing_direction,
                    Value top,
                    Value right,
                    Value bottom,
                    Value left)
      : writing_direction_(writing_direction),
        top_(top),
        right_(right),
        bottom_(bottom),
        left_(left) {}

  Value InlineStart() const {
    if (writing_direction_.IsHorizontal())
      return writing_direction_.IsLtr() ? left_ : right_;
    return writing_direction_.IsLtr() ? top_ : bottom_;
  }

  Value InlineEnd() const {
    if (writing_direction_.IsHorizontal())
      return writing_direction_.IsLtr() ? right_ : left_;
    return writing_direction_.IsLtr() ? bottom_ : top_;
  }

  Value BlockStart() const {
    if (writing_direction_.IsHorizontal())
      return top_;
    return writing_direction_.IsFlippedBlocks() ? right_ : left_;
  }

  Value BlockEnd() const {
    if (writing_direction_.IsHorizontal())
      return bottom_;
    return writing_direction_.IsFlippedBlocks() ? left_ : right_;
  }

  Value Over() const {
    return writing_direction_.IsHorizontal() ? top_ : right_;
  }

  Value Under() const {
    return writing_direction_.IsHorizontal() ? bottom_ : left_;
  }

  Value LineLeft() const {
    return writing_direction_.IsHorizontal() ? left_ : top_;
  }

  Value LineRight() const {
    return writing_direction_.IsHorizontal() ? right_ : bottom_;
  }

  // Legacy logical directions.
  Value Start() const { return InlineStart(); }
  Value End() const { return InlineEnd(); }
  Value Before() const { return BlockStart(); }
  Value After() const { return BlockEnd(); }

 private:
  WritingDirectionMode writing_direction_;
  Value top_;
  Value right_;
  Value bottom_;
  Value left_;
};

template <typename Value>
class LogicalToPhysical {
  STACK_ALLOCATED();

 public:
  LogicalToPhysical(WritingDirectionMode writing_direction,
                    Value inline_start,
                    Value inline_end,
                    Value block_start,
                    Value block_end)
      : writing_direction_(writing_direction),
        inline_start_(inline_start),
        inline_end_(inline_end),
        block_start_(block_start),
        block_end_(block_end) {}

  Value Left() const {
    if (writing_direction_.IsHorizontal())
      return writing_direction_.IsLtr() ? inline_start_ : inline_end_;
    return writing_direction_.IsFlippedBlocks() ? block_end_ : block_start_;
  }

  Value Right() const {
    if (writing_direction_.IsHorizontal())
      return writing_direction_.IsLtr() ? inline_end_ : inline_start_;
    return writing_direction_.IsFlippedBlocks() ? block_start_ : block_end_;
  }

  Value Top() const {
    if (writing_direction_.IsHorizontal())
      return block_start_;
    return writing_direction_.IsLtr() ? inline_start_ : inline_end_;
  }

  Value Bottom() const {
    if (writing_direction_.IsHorizontal())
      return block_end_;
    return writing_direction_.IsLtr() ? inline_end_ : inline_start_;
  }

 private:
  WritingDirectionMode writing_direction_;
  Value inline_start_;  // a.k.a. start
  Value inline_end_;    // a.k.a. end
  Value block_start_;   // a.k.a. before
  Value block_end_;     // a.k.a. after
};

template <typename Value>
class LogicalToLogical {
  STACK_ALLOCATED();

 public:
  LogicalToLogical(WritingDirectionMode from_writing_direction,
                   WritingDirectionMode to_writing_direction,
                   Value inline_start,
                   Value inline_end,
                   Value block_start,
                   Value block_end)
      : LogicalToLogical(to_writing_direction,
                         LogicalToPhysical<Value>(from_writing_direction,
                                                  inline_start,
                                                  inline_end,
                                                  block_start,
                                                  block_end)) {}

  Value InlineStart() const { return logical_.InlineStart(); }
  Value BlockStart() const { return logical_.BlockStart(); }
  Value InlineEnd() const { return logical_.InlineEnd(); }
  Value BlockEnd() const { return logical_.BlockEnd(); }

 private:
  LogicalToLogical(WritingDirectionMode to_writing_direction,
                   LogicalToPhysical<Value> physical)
      : logical_(to_writing_direction,
                 physical.Top(),
                 physical.Right(),
                 physical.Bottom(),
                 physical.Left()) {}

  PhysicalToLogical<Value> logical_;
};

template <typename Value, typename Object>
class LogicalToPhysicalGetter {
  STACK_ALLOCATED();

 public:
  using Getter = Value (Object::*)() const;
  LogicalToPhysicalGetter(WritingDirectionMode writing_direction,
                          const Object& object,
                          Getter inline_start_getter,
                          Getter inline_end_getter,
                          Getter block_start_getter,
                          Getter block_end_getter)
      : object_(object),
        converter_(writing_direction,
                   inline_start_getter,
                   inline_end_getter,
                   block_start_getter,
                   block_end_getter) {}

  Value Left() const { return (object_.*converter_.Left())(); }
  Value Right() const { return (object_.*converter_.Right())(); }
  Value Top() const { return (object_.*converter_.Top())(); }
  Value Bottom() const { return (object_.*converter_.Bottom())(); }

 private:
  const Object& object_;
  LogicalToPhysical<Getter> converter_;
};

template <typename Value, typename Object>
class PhysicalToLogicalGetter {
  STACK_ALLOCATED();

 public:
  using Getter = Value (Object::*)() const;
  PhysicalToLogicalGetter(WritingDirectionMode writing_direction,
                          const Object& object,
                          Getter top_getter,
                          Getter right_getter,
                          Getter bottom_getter,
                          Getter left_getter)
      : object_(object),
        converter_(writing_direction,
                   top_getter,
                   right_getter,
                   bottom_getter,
                   left_getter) {}

  Value InlineStart() const { return (object_.*converter_.InlineStart())(); }
  Value InlineEnd() const { return (object_.*converter_.InlineEnd())(); }
  Value BlockStart() const { return (object_.*converter_.BlockStart())(); }
  Value BlockEnd() const { return (object_.*converter_.BlockEnd())(); }
  Value Over() const { return (object_.*converter_.Over())(); }
  Value Under() const { return (object_.*converter_.Under())(); }
  Value LineLeft() const { return (object_.*converter_.LineLeft())(); }
  Value LineRight() const { return (object_.*converter_.LineRight())(); }
  Value Start() const { return (object_.*converter_.Start())(); }
  Value End() const { return (object_.*converter_.End())(); }
  Value Before() const { return (object_.*converter_.Before())(); }
  Value After() const { return (object_.*converter_.After())(); }

 private:
  const Object& object_;
  PhysicalToLogical<Getter> converter_;
};

template <typename Value, typename Object>
class PhysicalToLogicalSetter {
  STACK_ALLOCATED();

 public:
  using Setter = void (Object::*)(Value);
  PhysicalToLogicalSetter(WritingDirectionMode writing_direction,
                          Object& object,
                          Setter inline_start_setter,
                          Setter inline_end_setter,
                          Setter block_start_setter,
                          Setter block_end_setter)
      : object_(object),
        converter_(writing_direction,
                   inline_start_setter,
                   inline_end_setter,
                   block_start_setter,
                   block_end_setter) {}

  void SetLeft(Value v) { (object_.*converter_.Left())(v); }
  void SetRight(Value v) { (object_.*converter_.Right())(v); }
  void SetTop(Value v) { (object_.*converter_.Top())(v); }
  void SetBottom(Value v) { (object_.*converter_.Bottom())(v); }

 private:
  Object& object_;
  // This converter converts logical setters to physical setters which accept
  // physical values and call the logical setters to set logical values.
  LogicalToPhysical<Setter> converter_;
};

template <typename Value, typename Object>
class LogicalToPhysicalSetter {
  STACK_ALLOCATED();

 public:
  using Setter = void (Object::*)(Value);
  LogicalToPhysicalSetter(WritingDirectionMode writing_direction,
                          Object& object,
                          Setter top_setter,
                          Setter right_setter,
                          Setter bottom_setter,
                          Setter left_setter)
      : object_(object),
        converter_(writing_direction,
                   top_setter,
                   right_setter,
                   bottom_setter,
                   left_setter) {}

  void SetInlineStart(Value v) { (object_.*converter_.InlineStart())(v); }
  void SetInlineEnd(Value v) { (object_.*converter_.InlineEnd())(v); }
  void SetBlockStart(Value v) { (object_.*converter_.BlockStart())(v); }
  void SetBlockEnd(Value v) { (object_.*converter_.BlockEnd())(v); }
  void SetOver(Value v) { (object_.*converter_.Over())(v); }
  void SetUnder(Value v) { (object_.*converter_.Under())(v); }
  void SetLineLeft(Value v) { (object_.*converter_.LineLeft())(v); }
  void SetLineRight(Value v) { (object_.*converter_.LineRight())(v); }
  void SetStart(Value v) { (object_.*converter_.Start())(v); }
  void SetEnd(Value v) { (object_.*converter_.End())(v); }
  void SetBefore(Value v) { (object_.*converter_.Before())(v); }
  void SetAfter(Value v) { (object_.*converter_.After())(v); }

 private:
  Object& object_;
  // This converter converts physical setters to logical setters which accept
  // logical values and call the physical setters to set physical values.
  PhysicalToLogical<Setter> converter_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_WRITING_MODE_UTILS_H_
