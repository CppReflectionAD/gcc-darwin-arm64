// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }
// Test std::meta::statements_of.

#include <meta>

using namespace std::meta;

constexpr int test_if(int x) {
  if (x > 0)
    return x;
  else
    return -x;
}

constexpr int test_while(int x) {
  int sum = 0;
  while (x > 0) {
    sum += x;
    --x;
  }
  return sum;
}

consteval bool test_statements() {
  // Test test_if deconstruction
  auto if_stmts = statements_of(body_of( ^^test_if ));
  if (if_stmts.empty()) return false;
  
  auto cond_stmt = if_stmts[0];
  if (display_string_of(cond_stmt) != "<cond_expr>") return false;

  auto branches = statements_of(cond_stmt);
  if (branches.size() != 3) return false;
  if (display_string_of(branches[0]) != "<gt_expr>") return false;
  if (display_string_of(branches[1]) != "<return_expr>") return false;
  if (display_string_of(branches[2]) != "<return_expr>") return false;

  // Test test_while deconstruction
  auto while_stmts = statements_of(body_of( ^^test_while ));
  if (while_stmts.size() < 3) return false;
  if (display_string_of(while_stmts[0]) != "<decl_expr>") return false;
  if (display_string_of(while_stmts[2]) != "<return_expr>") return false;

  return true;
}

consteval bool test_invalid() {
  try {
    statements_of( ^^int );
  }
  catch (std::meta::exception &) {
    return true;
  }
  return false;
}

static_assert (test_statements());
static_assert (test_invalid());
