// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }

#include <meta>

using namespace std::meta;

constexpr float sample_func(float x) {
  float a = x * 2.0f;
  return a + 1.0f;
}

consteval bool test() {
  info body = body_of( ^^sample_func );
  auto stmts = statements_of(body);
  if (stmts.size() != 2) return false;

  // Stmt 0: declaration statement float a = x * 2.0f;
  if (!is_statement(stmts[0])) return false;
  if (!is_declaration_statement(stmts[0])) return false;
  if (is_return_statement(stmts[0])) return false;

  // declared_variable_of
  info var_a = declared_variable_of(stmts[0]);
  if (!is_variable(var_a)) return false;
  if (identifier_of(var_a) != "a") return false;

  // Stmt 1: return statement return a + 1.0f;
  if (!is_statement(stmts[1])) return false;
  if (!is_return_statement(stmts[1])) return false;
  if (is_declaration_statement(stmts[1])) return false;

  return true;
}

static_assert(test());
