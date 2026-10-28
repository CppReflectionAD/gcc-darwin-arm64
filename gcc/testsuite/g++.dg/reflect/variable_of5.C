// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }
// std::meta::variable_of works outside the parameter's function (see
// variable_of4.C), but the variable is still a local of that function:
// splicing it elsewhere is ill-formed, as for any other function's local.

#include <meta>

int
fn (int p)
{
  return p;
}

int
read_outside ()
{
  return [: std::meta::variable_of (std::meta::parameters_of (^^fn)[0]) :]; // { dg-error "" }
}
