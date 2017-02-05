/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *   - Laura Schlimmer <laura@eventql.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <eventql/sql/statements/select/nested_loop_join.h>

namespace csql {

NestedLoopJoin::NestedLoopJoin(
    Transaction* txn,
    JoinType join_type,
    const Vector<JoinNode::InputColumnRef>& input_map,
    Vector<ValueExpression> select_expressions,
    Option<ValueExpression> join_cond_expr,
    Option<ValueExpression> where_expr,
    ScopedPtr<TableExpression> base_tbl,
    ScopedPtr<TableExpression> joined_tbl) :
    txn_(txn),
    join_type_(join_type),
    input_map_(input_map),
    input_buf_(input_map.size()),
    select_exprs_(std::move(select_expressions)),
    join_cond_expr_(std::move(join_cond_expr)),
    where_expr_(std::move(where_expr)),
    base_tbl_(std::move(base_tbl)),
    base_tbl_mincols_(0),
    joined_tbl_(std::move(joined_tbl)),
    joined_tbl_pos_(0),
    joined_tbl_mincols_(0) {
  for (size_t i = 0; i < input_map_.size(); ++i) {
    const auto& m = input_map_[i];

    switch (m.table_idx) {
      case 0:
        base_tbl_mincols_ = std::max(base_tbl_mincols_, m.column_idx + 1);
        break;
      case 1:
        joined_tbl_mincols_ = std::max(joined_tbl_mincols_, m.column_idx + 1);
        break;
      default:
        RAISE(kRuntimeError, "invalid table index");
    }
  }
}

static const size_t kMaxInMemoryRows = 1000000;

ReturnCode NestedLoopJoin::execute() {
  auto joined_rc = joined_tbl_->execute();
  if (!joined_rc.isSuccess()) {
    return joined_rc;
  }
/*
  Vector<SValue> row(joined_tbl_->getColumnCount());
  while (joined_tbl_->next(row.data(), row.size())) {
    if (row.size() < joined_tbl_mincols_) {
      RAISE(
          kRuntimeError,
          "INTERNAL ERROR: Nested Loop JOIN joined input row is too small");
    }

    joined_tbl_data_.emplace_back(row);

    if (joined_tbl_data_.size() >= kMaxInMemoryRows) {
      RAISE(
          kRuntimeError,
          "Nested Loop JOIN intermediate result set is too large, try using an"
          " equi-join instead.");
    }
  }

  auto base_rc = base_tbl_->execute();
  if (!base_rc.isSuccess()) {
    return base_rc;
  }

  base_tbl_row_.resize(base_tbl_->getColumnCount());
  switch (join_type_) {
    case JoinType::OUTER:
      return executeOuterJoin();
    case JoinType::INNER:
      if (join_cond_expr_.isEmpty()) {
        /\* fallthrough *\/
      } else {
        return executeInnerJoin();
      }
    case JoinType::CARTESIAN:
      return executeCartesianJoin();
    default:
      RAISE(kIllegalStateError);
  }

*/
  return ReturnCode::success();
}

size_t NestedLoopJoin::getColumnCount() const {
  return select_exprs_.size();
}

SType NestedLoopJoin::getColumnType(size_t idx) const {
  assert(idx < select_exprs_.size());
  return select_exprs_[idx].getReturnType();
}

ReturnCode NestedLoopJoin::nextBatch(
    SVector* columns,
    size_t* nrecords) {
  return ReturnCode::error("ERUNTIME", "NestedLoopJoin::nextBatch not yet implemented");
}

/*
ReturnCode NestedLoopJoin::executeCartesianJoin() {
  cursor_ = [this] (SValue* row, int row_len) -> bool {
    for (;;) {
      if (joined_tbl_pos_ == 0 || joined_tbl_pos_ == joined_tbl_data_.size()) {
        joined_tbl_pos_ = 0;

        if (!base_tbl_->next(
              base_tbl_row_.data(),
              base_tbl_row_.size())) {
          return false;
        }

        if (base_tbl_row_.size() < base_tbl_mincols_) {
          RAISE(
              kRuntimeError,
              "INTERNAL ERROR: Nested Loop JOIN base input row is too small");
        }
      }

      while (joined_tbl_pos_ < joined_tbl_data_.size()) {
        const auto& joined_table_row = joined_tbl_data_[joined_tbl_pos_++];

        for (size_t i = 0; i < input_map_.size(); ++i) {
          const auto& m = input_map_[i];

          switch (m.table_idx) {
            case 0:
              input_buf_[i] = base_tbl_row_[m.column_idx];
              break;
            case 1:
              input_buf_[i] = joined_table_row[m.column_idx];
              break;
            default:
              RAISE(kRuntimeError, "invalid table index");
          }
        }

        if (!where_expr_.isEmpty()) {
          VM::evaluateBoxed(
              txn_,
              where_expr_.get().program(),
              where_expr_.get().program()->method_call,
              &vm_stack_,
              nullptr,
              input_buf_.size(),
              input_buf_.data());

          if (!popBool(&vm_stack_)) {
            continue;
          }
        }

        for (int i = 0; i < select_exprs_.size() && i < row_len; ++i) {
          VM::evaluateBoxed(
              txn_,
              select_exprs_[i].program(),
              select_exprs_[i].program()->method_call,
              &vm_stack_,
              nullptr,
              input_buf_.size(),
              input_buf_.data());

          popBoxed(&vm_stack_, &row[i]);
        }

        return true;
      }
    }
  };

  return ReturnCode::success();
}

ReturnCode NestedLoopJoin::executeInnerJoin() {
  cursor_ = [this] (SValue* row, int row_len) -> bool {
    for (;;) {
      if (joined_tbl_pos_ == 0 || joined_tbl_pos_ == joined_tbl_data_.size()) {
        joined_tbl_pos_ = 0;

        if (!base_tbl_->next(
              base_tbl_row_.data(),
              base_tbl_row_.size())) {
          return false;
        }

        if (base_tbl_row_.size() < base_tbl_mincols_) {
          RAISE(
              kRuntimeError,
              "INTERNAL ERROR: Nested Loop JOIN base input row is too small");
        }
      }

      while (joined_tbl_pos_ < joined_tbl_data_.size()) {
        const auto& joined_table_row = joined_tbl_data_[joined_tbl_pos_++];

        for (size_t i = 0; i < input_map_.size(); ++i) {
          const auto& m = input_map_[i];

          switch (m.table_idx) {
            case 0:
              input_buf_[i] = base_tbl_row_[m.column_idx];
              break;
            case 1:
              input_buf_[i] = joined_table_row[m.column_idx];
              break;
            default:
              RAISE(kRuntimeError, "invalid table index");
          }
        }

        {
          VM::evaluateBoxed(
              txn_,
              join_cond_expr_.get().program(),
              join_cond_expr_.get().program()->method_call,
              &vm_stack_,
              nullptr,
              input_buf_.size(),
              input_buf_.data());

          if (!popBool(&vm_stack_)) {
            continue;
          }
        }

        if (!where_expr_.isEmpty()) {
          VM::evaluateBoxed(
              txn_,
              where_expr_.get().program(),
              where_expr_.get().program()->method_call,
              &vm_stack_,
              nullptr,
              input_buf_.size(),
              input_buf_.data());

          if (!popBool(&vm_stack_)) {
            continue;
          }
        }

        for (int i = 0; i < select_exprs_.size() && i < row_len; ++i) {
          VM::evaluateBoxed(
              txn_,
              select_exprs_[i].program(),
              select_exprs_[i].program()->method_call,
              &vm_stack_,
              nullptr,
              input_buf_.size(),
              input_buf_.data());

          popBoxed(&vm_stack_, &row[i]);
        }

        return true;
      }
    }
  };

  return ReturnCode::success();
}

ReturnCode NestedLoopJoin::executeOuterJoin() {
  cursor_ = [this] (SValue* row, int row_len) -> bool {
    for (;;) {
      if (joined_tbl_pos_ == 0 ||
          joined_tbl_pos_ == joined_tbl_data_.size()) {
        joined_tbl_pos_ = 0;
        joined_tbl_row_found_ = false;

        if (!base_tbl_->next(
              base_tbl_row_.data(),
              base_tbl_row_.size())) {
          return false;
        }

        if (base_tbl_row_.size() < base_tbl_mincols_) {
          RAISE(
              kRuntimeError,
              "INTERNAL ERROR: Nested Loop JOIN base input row is too small");
        }
      }

      bool match = false;
      while (joined_tbl_pos_ < joined_tbl_data_.size()) {
        const auto& joined_table_row = joined_tbl_data_[joined_tbl_pos_++];

        for (size_t i = 0; i < input_map_.size(); ++i) {
          const auto& m = input_map_[i];

          switch (m.table_idx) {
            case 0:
              input_buf_[i] = base_tbl_row_[m.column_idx];
              break;
            case 1:
              input_buf_[i] = joined_table_row[m.column_idx];
              break;
            default:
              RAISE(kRuntimeError, "invalid table index");
          }
        }

        VM::evaluateBoxed(
            txn_,
            join_cond_expr_.get().program(),
            join_cond_expr_.get().program()->method_call,
            &vm_stack_,
            nullptr,
            input_buf_.size(),
            input_buf_.data());

        if (!popBool(&vm_stack_)) {
          continue;
        }

        joined_tbl_row_found_ = true;
        match = true;
        break;
      }

      if (match || !joined_tbl_row_found_) {
        if (!joined_tbl_row_found_) {
          for (size_t i = 0; i < input_map_.size(); ++i) {
            switch (input_map_[i].table_idx) {
              case 0:
                input_buf_[i] = base_tbl_row_[input_map_[i].column_idx];
                break;
              case 1:
                input_buf_[i] = SValue{};
                break;
              default:
                RAISE(kRuntimeError, "invalid table index");
            }
          }
        }

        if (!where_expr_.isEmpty()) {
          VM::evaluateBoxed(
              txn_,
              where_expr_.get().program(),
              where_expr_.get().program()->method_call,
              &vm_stack_,
              nullptr,
              input_buf_.size(),
              input_buf_.data());

          if (!popBool(&vm_stack_)) {
            continue;
          }
        }

        for (int i = 0; i < select_exprs_.size() && i < row_len; ++i) {
          VM::evaluateBoxed(
              txn_,
              select_exprs_[i].program(),
              select_exprs_[i].program()->method_call,
              &vm_stack_,
              nullptr,
              input_buf_.size(),
              input_buf_.data());

          popBoxed(&vm_stack_, &row[i]);
        }

        return true;
      }
    }
  };

  return ReturnCode::success();
}
*/

} // namespace csql
