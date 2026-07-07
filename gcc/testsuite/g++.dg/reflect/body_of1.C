// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }
// Test std::meta::body_of.

#include <meta>

using namespace std::meta;

constexpr int foo() {
  return 42;
}

void bar(); // declared but not defined

consteval bool test_defined() {
  info b = body_of( ^^foo );
  return b != info{};
}

consteval bool test_undefined() {
  try {
    body_of( ^^bar );
  }
  catch (std::meta::exception &) {
    return true;
  }
  return false;
}

consteval bool test_not_fn() {
  try {
    body_of( ^^int );
  }
  catch (std::meta::exception &) {
    return true;
  }
  return false;
}

static_assert (test_defined());
static_assert (test_undefined());
static_assert (test_not_fn());
