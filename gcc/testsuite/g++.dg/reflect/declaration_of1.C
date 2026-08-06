// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }

#include <meta>

int my_var = 42;

constexpr int add(int a, int b) {
  return a + b;
}

consteval bool test() {
  // Test 1: global variable reflection
  auto expr_info = ^^my_var;
  auto decl_info = std::meta::declaration_of(expr_info);
  if (std::meta::identifier_of(decl_info) != "my_var") return false;

  // Test 2: function reflection
  auto fn_info = ^^add;
  auto fn_decl = std::meta::declaration_of(fn_info);
  if (std::meta::identifier_of(fn_decl) != "add") return false;

  return true;
}

static_assert(test());
