// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }
// Test expression deconstruction metafunctions.

#include <meta>

using namespace std::meta;

constexpr float exp(float x) {
  return 1.0f + x;
}

constexpr float sigmoid(float x) {
  return 1.0f + (exp(-x) + 1.0f);
}


consteval int count_leaves(info node) {
  if (is_expression(node)) {
    if (is_variable(node) || is_literal(node)) {
      return 1;
    }
    auto operands = operands_of(node);
    if (operands.size() == 1 && operands[0] == node) {
      return 1;
    }
    int sum = 0;
    for (info op : operands) {
      sum += count_leaves(op);
    }
    return sum;
  }

  if (!is_statement(node)) {
    return 1; // Non-statement, non-expression leaf (e.g. function symbol)
  }

  auto children = statements_of(node);
  if (children.size() == 1 && children[0] == node) {
    return 1;
  }
  int sum = 0;
  for (info child : children) {
    sum += count_leaves(child);
  }
  return sum;
}

consteval bool run_test() {
  info body = body_of( ^^sigmoid );
  auto stmts = statements_of(body);
  if (stmts.size() != 1) return false;

  // The returned assignment expression: res = (exp(-x) + 1.0f) + 1.0f
  info ret_expr = stmts[0];
  if (!is_expression(ret_expr)) return false;
  if (operator_of(ret_expr) != operators::op_equals) return false;

  auto operands = operands_of(ret_expr);
  if (operands.size() != 2) return false;

  // Left operand: RESULT_DECL variable
  info lhs = operands[0];
  if (!is_expression(lhs)) return false;
  if (!is_variable(lhs)) return false;

  // Right operand: (exp(-x) + 1.0f) + 1.0f
  info rhs = operands[1];
  if (!is_expression(rhs)) return false;
  if (!is_binary_operator(rhs)) return false;
  if (operator_of(rhs) != operators::op_plus) return false;

  auto rhs_ops = operands_of(rhs);
  if (rhs_ops.size() != 2) return false;

  // (exp(-x) + 1.0f)
  info inner_plus = rhs_ops[0];
  if (!is_binary_operator(inner_plus)) return false;
  if (operator_of(inner_plus) != operators::op_plus) return false;

  // 1.0f literal
  if (!is_literal(rhs_ops[1])) return false;

  auto inner_ops = operands_of(inner_plus);
  if (inner_ops.size() != 2) return false;

  // exp(-x) function call
  info call = inner_ops[0];
  if (!is_function_call(call)) return false;

  // 1.0f literal
  if (!is_literal(inner_ops[1])) return false;

  auto call_ops = operands_of(call);
  if (call_ops.size() != 2) return false;

  // callee: exp (function symbol, not an expression)
  if (is_expression(call_ops[0])) return false;
  if (!is_function(call_ops[0])) return false;

  // argument: the actual tree is NEGATE_EXPR(NOP_EXPR(x))
  // i.e. negate -> conversion -> variable
  info neg = call_ops[1];
  if (!is_unary_operator(neg)) return false;
  if (operator_of(neg) != operators::op_minus) return false;

  auto neg_ops = operands_of(neg);
  if (neg_ops.size() != 1) return false;

  // NOP_EXPR conversion (unary_op)
  info conv = neg_ops[0];
  if (!is_unary_operator(conv)) return false;

  auto conv_ops = operands_of(conv);
  if (conv_ops.size() != 1) return false;

  // x (PARM_DECL variable)
  if (!is_variable(conv_ops[0])) return false;

  // Count leaves of the sigmoid body.
  // Leaves: res_decl, exp (function), x (variable), 1.0f, 1.0f = 5
  if (count_leaves(body) != 5) return false;

  return true;
}

static_assert(run_test());
