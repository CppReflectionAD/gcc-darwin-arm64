// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }
// Test std::meta::statements_of.

#include <meta>

using namespace std::meta;

constexpr int test_fn(int x) {
  int y = x + 1;
  return y;
}

consteval bool test_statements() {
  auto stmts = statements_of(body_of( ^^test_fn ));
  if (stmts.size() != 2)
    return false;
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
