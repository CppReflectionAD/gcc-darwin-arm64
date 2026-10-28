// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }
// Test std::meta::variable_of outside the parameter's function, an extension
// to P3096: the result is the variable that a use of the parameter in the
// body names.

#include <meta>

using namespace std::meta;

// declaration_of of the first variable a function's return operand reads.
consteval info
first_use (info f)
{
  info e;
  for (info s : statements_of (body_of (f)))
    if (is_return_statement (s))
      e = return_operand_of (s);
  while (!is_variable_reference (e))
    e = operands_of (e)[0];
  return declaration_of (e);
}

int
fn (int a, int b)
{
  return a * b;
}

static_assert (is_variable (variable_of (parameters_of (^^fn)[0])));
static_assert (type_of (variable_of (parameters_of (^^fn)[0])) == ^^int);
static_assert (variable_of (parameters_of (^^fn)[0]) == first_use (^^fn));
static_assert (variable_of (parameters_of (^^fn)[1]) != first_use (^^fn));
static_assert (variable_of (parameters_of (^^fn)[0]) != parameters_of (^^fn)[0]);

// Declared, then defined: still the variable the body uses.
int redeclared (int a, int b);
int
redeclared (int a, int b)
{
  return a * b;
}

static_assert (variable_of (parameters_of (^^redeclared)[0])
	       == first_use (^^redeclared));

// Each element of an expanded pack has its own unnamed variable, including in
// a specialization nothing has instantiated yet.
template<typename... Ts>
int
pack (Ts... xs)
{
  return (xs + ...);
}

constexpr auto pack_params = define_static_array (parameters_of (^^pack<int, long, int>));
static_assert (pack_params.size () == 3);
static_assert (variable_of (pack_params[0]) != variable_of (pack_params[1]));
static_assert (variable_of (pack_params[0]) != variable_of (pack_params[2]));
static_assert (type_of (variable_of (pack_params[1])) == ^^long);
static_assert (variable_of (pack_params[0]) == first_use (^^pack<int, long, int>));

// A function with no definition has no parameter variables.
void undefined_fn (int);

consteval bool
has_variable_of (info r)
{
  try { variable_of (r); }
  catch (exception &) { return false; }
  return true;
}

static_assert (!has_variable_of (parameters_of (^^undefined_fn)[0]));
